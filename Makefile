include Makefile.config

RESET = reset_graph
REMAP_ELIM = remap_elim

all: $(RESET) $(REMAP_ELIM)

clean: 
	rm -f $(RESET) $(REMAP_ELIM)

remap_elim: remap_elim.cpp 
	$(CC) $(CXXFLAGS) -o remap_elim remap_elim.cpp $(LDFLAGS) $(LIBS)

reset_graph: reset_graph.cpp 
	$(CC) $(CXXFLAGS) -o reset_graph reset_graph.cpp $(LDFLAGS) $(LIBS)