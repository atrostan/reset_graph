#include <chrono>
#include <cstdio>
#include <cstring>
#include <vector>
#include <sstream>
#include <fstream>
#include <parallel/algorithm>

#include <string>
#include <iostream>
#include <iterator>
#include <set>
#include <assert.h>     /* assert */

#include <mpi.h>
#include <unistd.h>

char const *input_filename = "";
char const *output_filename = "";
char const *reorder_filename = "";
size_t n_nodes = 0;
size_t n_edges = 0;

// before remapping the vertices of the graph, reorder by 
// ascending (source, dest)
void reorder(char const *input_filename, char const *reorder_filename)
{
  // read edgelist to a vector of pairs
  std::set<std::pair<int, int>> edge_set;
  std::ifstream infile(input_filename);
  std::string line;

  // number of lines in infile is the original number of edges (maybe with dups)
  size_t orig_n_edges = 0;
  
  while (std::getline(infile, line))
  {
      std::istringstream iss(line);
      int src, dest;
      if (!(iss >> src >> dest)) { 
        continue; 
      } 
      iss >> src >> dest;
      edge_set.emplace(std::make_pair(src, dest));
      orig_n_edges++;
  }

  // size of edge_set after reading infile is the number of unique edges
  size_t n_unique_edges = edge_set.size();

  std::cout << "original number of edges (maybe with dups): " << orig_n_edges <<
              std::endl;

  std::cout << "n_unique_edges: " << n_unique_edges << std::endl;
  std::vector<std::pair<int, int>> edges;
  std::copy(edge_set.begin(), edge_set.end(), std::back_inserter(edges));

  std::cout << "gnu_parallel sorting... " << std::endl;
  __gnu_parallel::sort(edges.begin(), edges.end());
  std::cout << "finished gnu_parallel sorting" << std::endl;
  std::cout << "writing to " << reorder_filename << std::endl;
  std::ofstream outfile(reorder_filename);
  
  for (auto pr: edges)
  {
    outfile << pr.first << " " << pr.second << std::endl;
  }
  std::cout << "finished writing to " << reorder_filename << std::endl;

  outfile.close();
}

// try to get a vertex's mapping from the isomorphism map
// if the vertex is in the map, return the associated value
// if the vertex is not in the map, set its value to the latest vertex id 
int get_or_set(std::map<int, int> &mp, int v, int *latest_vid)
{
  std::map<int, int>::iterator lb = mp.lower_bound(v);

  int result;
  // key already exists
  // update lb->second if you care to
  if(lb != mp.end() && !(mp.key_comp()(v, lb->first)))
  {
    // std::cout << "key already exists " << lb->first << std::endl;
    // std::cout << "associated with " << lb->second << std::endl;
    result = lb->second;
  }
  // the key does not exist in the map
  // add it to the map
  else
  {
    // Use lb as a hint to insert, so it can avoid another lookup
    mp.insert(lb, std::map<int, int>::value_type(v, *latest_vid));    
    // std::cout << "doesn't exist, autoincrement " << *latest_vid << std::endl;
    result = *latest_vid;
    (*latest_vid)++;
  }
  return result;

}

// get an isomorphism map for the input graph 
// build the map by iterating over the graph's edge list and mapping each 
// `newly seen` vertex to an autoincremented vertex id
std::pair<size_t, size_t> get_map(char const *input_filename, 
                                  std::map<int, int> &mp) {
  std::cout << "started reading from: " << input_filename << std::endl;

  std::set<int> dests;

  std::ifstream infile(input_filename);

  std::string line;
  int latest_vid = 0;
  int edge_count = 0;
  while (std::getline(infile, line))
  {
      std::istringstream iss(line);
      int src, dest;
      if (!(iss >> src >> dest)) { 
        continue; 
      } 
      iss >> src >> dest;
      get_or_set(mp, src, &latest_vid);
      dests.insert(dest);
      edge_count++;
  }

  for (auto dest: dests){
    get_or_set(mp, dest, &latest_vid);
  }
  std::pair<size_t, size_t> nm (mp.size(), edge_count);
  return nm;
}

void rewrite_graph(char const *input_filename, char const *output_filename, 
                  std::map<int, int> &mp, size_t n_nodes, size_t n_edges) {
  std::cout << "started writing to: " << output_filename << std::endl;
  std::ifstream infile(input_filename);
  std::ofstream outfile(output_filename);
  std::string line;
  std::vector<std::pair<int, int>> edges;

  outfile << "# Nodes: " << n_nodes << " Edges: " << n_edges << std::endl;
  while (std::getline(infile, line))
  {
      std::istringstream iss(line);
      int orig_src, orig_dest;
      if (!(iss >> orig_src >> orig_dest)) { 
        continue; 
      } 
      int mp_src = mp.find(orig_src)->second;
      int mp_dest = mp.find(orig_dest)->second;
      edges.push_back(std::make_pair(mp_src, mp_dest));
      // outfile << mp_src << " " << mp_dest << std::endl;
  }
  __gnu_parallel::sort(edges.begin(), edges.end());
  for (auto pr: edges)
  {
    outfile << pr.first << " " << pr.second << std::endl;
  }
  outfile.close();
}

int main(int argc, char* argv[]) {
  int opt;
  opterr = 0;
  while ((opt = getopt(argc, argv, "i:r:o:n:m:")) != -1) {
    switch (opt) {
      case 'i':
        input_filename = optarg;
        break;
      case 'r':
        reorder_filename = optarg;
        break;
      case 'o':
        output_filename = optarg;
        break;
      case 'n':
        n_nodes = atoi(optarg);
        break;
      case 'm':
        n_edges = atoi(optarg);
        break;
      default:
        abort();
    }
  }

  std::cout << "reordering: " << input_filename << std::endl;

  reorder(input_filename, reorder_filename);
  std::map<int, int> mp;
  std::cout << "building isomorphism map" << std::endl;

  std::pair<size_t, size_t> nm = get_map(reorder_filename, mp);
  assert ((nm.first == n_nodes) && (nm.second == n_edges));

  std::cout << "rewriting graph" << std::endl;
  rewrite_graph(reorder_filename, output_filename, mp, n_nodes, n_edges);

  return 0;
}