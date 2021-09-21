import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import json
from pprint import pprint
from pathlib import Path
import shutil
import subprocess

data_dir = '/home/atrostan/Workspace/repos/reset_graph/data'
reset_graph_exec_path = '/home/atrostan/Workspace/repos/reset_graph/reset_graph'

graph_attrs_json_path = \
    '/home/atrostan/Workspace/School/ubc/graphs/graphslab/sheep_to_dbg/graph_attrs.json'
with open(graph_attrs_json_path) as f:
    graph_attrs = json.load(f)

for graph_name in graph_attrs.keys():
    print(f'reordering, resetting {graph_name}')
    if graph_name != 'com-orkut':
        print('skip')
        continue
    # make a dir for each graph
    # graph_dir = f'{data_dir}/{graph_name}'
    # Path(graph_dir).mkdir(parents=True, exist_ok=True) 

    # move the graph from current path to it's own dir
    # curr_graph_path = f'{data_dir}/{graph_name}.txt'
    # dest_graph_path = f'{data_dir}/{graph_name}/orig.txt'
    # shutil.move(curr_graph_path, dest_graph_path)
    graph_path = f'{data_dir}/{graph_name}/orig.txt'
    reorder_path = f'{data_dir}/{graph_name}/reorder-orig.txt'
    reset_path = f'{data_dir}/{graph_name}/reset-orig.txt'

    print(graph_path)
    print(reorder_path)
    print(reset_path)    

    graph_attr = graph_attrs[graph_name]
    args = [
        reset_graph_exec_path,
        '-i', graph_path,
        '-r', reorder_path,
        '-o', reset_path,
        '-n', str(graph_attr['n_nodes']),
        '-m', str(graph_attr['n_edges']),
    ]
    log_path = f'{data_dir}/{graph_name}/reset_graph.log'

    with open(log_path, 'w') as f:
        subprocess.call(args, stdout=f, stderr=f)

    # copy each reset graph to its destination in the sheep_to_dbg repo
    dest_dir = \
        '/home/atrostan/Workspace/School/ubc/graphs/graphslab/sheep_to_dbg/data'
    dest_path = f'{dest_dir}/{graph_name}/orig.net'
    src_path = reset_path
    shutil.move(src_path, dest_path)
