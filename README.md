# Maximum Bandwidth Path Problem
This project is about implementing an efficient algorithm for the Maximum Bandwidth Path problem. Briefly, given a graph, `G`, where each edge has some
associated "capacity", we want to find the Maximum bandwidth path between two nodes. Bandwidth of a path is defined as the capacity of the "bottleneck" in
the path, i.e. the edge with the least capacity.

This problem has applications in Network Routing, Supply Chains, Transport Theory, etc.

### Changing type of Graph
Please change line 13 in main.cpp as follows for G1 or G2. Although this is less convinient than taking it as input,
We only have 2 possible graph types, so simplicity was preferred. This also makes runtime measurements easier.

```int type = 1 // or 2;```

### Compiling the Program
Please use the following command for compilation on Linux:

```g++ -std=c++17 main.cpp```

If you are using MacOS, then the following command could be used instead (although not tested)

```clang++ -std=c++17 main.cpp```

### Running the Program
Run the following command:

```./a.out```
