#/bin/bash

# graph_name="cit-Patents.txt"
# graph_name="com-amazon.ungraph.txt"
# graph_name="web-BerkStan.txt"
graph_name="com-orkut"

./reset_graph \
    -i /home/atrostan/Workspace/repos/reset_graph/data/${graph_name} \
    -r /home/atrostan/Workspace/repos/reset_graph/data/reorder-${graph_name}\
    -o /home/atrostan/Workspace/repos/reset_graph/data/reset-${graph_name} \
    -n 3072441 \
    -m 117185083