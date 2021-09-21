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

char const *seq_filename = "";
char const *elim_input_filename = "";
char const *elim_output_filename = "";
char const *bfs_elim_input_filename = "";
char const *bfs_elim_output_filename = "";
size_t n_nodes = 0;
size_t n_edges = 0;

void build_map(char const *seq_filename, std::map<int, int> &mp)
{
  std::ifstream infile(seq_filename);
  std::string line;
  size_t line_number = 0;
  while (std::getline(infile, line))
  {
    std::istringstream iss(line);
    int idx;
    if (!(iss >> idx)) { 
        continue; 
    } 
    iss >> idx;
    mp.insert(std::pair<int, int> (line_number, idx));
    line_number++;
  }
  return;
}

void rewrite(char const *in_filename, char const *out_filename, 
              std::map<int, int> &mp, size_t n_nodes, size_t n_edges)
{
  std::ifstream infile(in_filename);
  std::string line;
  std::vector<std::pair<int, int>> edges;

  while (std::getline(infile, line))
  {
    std::istringstream iss(line);
    int vid, mapped_vid;
    if (!(iss >> vid >> mapped_vid)) { 
        continue; 
    } 
    iss >> vid >> mapped_vid;
    // add to vector so that we can resort by the original vid
    edges.push_back(std::make_pair(mp.find(vid)->second, mapped_vid));
  }
  __gnu_parallel::sort(edges.begin(), edges.end());
  
  std::ofstream outfile(out_filename);

  outfile << n_nodes << std::endl;
  outfile << n_edges << std::endl;

  for (auto pr: edges)
  {
    outfile << pr.first << " " << pr.second << std::endl;
  }
  outfile.close();
}

int main(int argc, char* argv[]) {
  int opt;
  opterr = 0;
  while ((opt = getopt(argc, argv, "s:e:f:b:c:n:m:")) != -1) {
    switch (opt) {
      case 's':
        seq_filename = optarg;
        break;
      case 'e':
        elim_input_filename = optarg;
        break;
      case 'f':
        elim_output_filename = optarg;
        break;
      case 'b':
        bfs_elim_input_filename = optarg;
        break;
      case 'c':
        bfs_elim_output_filename = optarg;
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

  std::map<int, int> mp;
  std::cout << "building map to original vids.." << std::endl;
  build_map(seq_filename, mp);
  std::cout << "rewriting elimination tree.." << std::endl;
  rewrite(elim_input_filename, elim_output_filename, mp, n_nodes, n_edges);
  std::cout << "rewriting bfs elimination tree.." << std::endl;
  rewrite(bfs_elim_input_filename, bfs_elim_output_filename, mp, n_nodes, 
          n_edges);
  std::cout << "finished!" << std::endl;
}