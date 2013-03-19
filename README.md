This repo contains some experimental code to explore possiblity of speeding up traversals between partitions of a graph.

### Details
------------

Consider the following graph:
  P1 -->  P2 --> P3

Now if we want to find a path from Node: p (in P1) to Node: q (in P3), we dont need to traverse the internal nodes of partition P2.
This implies that by properly preprocessing partitions, we can significantly prune away internal nodes, while taking a hop from unrelated
partiton. This will result in a shorter graph and hence faster traversals.

### Terminology
----------------

External Nodes :- All the nodes in a partition which have an incoming or outgoing inter-partition edge, (i.e Nodes on the partition boundary).

Hop Edges :- Every two external nodes in a partition which are reachable from one another can be connected by a hop edge.
             Hop Edges are different from Graph edges in the sense that they are not actual edges, they just cache the path from
             src to dst of the hop, and avoid traversing internal nodes for an unrelated partition.

External Graph :- For a given partitioned graph, an external graph represents the connectivity between the external nodes. This implies 
                  the only edges present in an external graph will be either Interpartition Edges, or Hop Edges or intra-partition 
                  Graph Edges that connect two external nodes.

### Algorithm used for Pruning
------------------------------

    foreach partition in Graph
        Identify all the 'external nodes'.
        foreach pair(src, dst) in 'external nodes'
            traverse from src to dst
            If a shortest path with more than one edge exists,
            Create a 'hop edge' that caches this path information.
        end
    end

### Observations
----------------

This however may reduce intra-partition traversals, but inter-partition traversal will still continue to be a bottleneck to performance.
An idea I was exploring was to create an 'external graph' from the partitioned graph.

Finding a path between nodes that lie in different partitions will involve:
* Find a path from source node to the nodes on partition boundary (traversal on source partition)
* Perform inter-partition routing using the 'external graph', and identify reachable nodes at destination partition boundary.
* Find a path from these reachable nodes to destination node. (traversal on destination partition)

'External Graph' can be created at the time when graph was being created. A single partition can hold responsiblity for traversing the external graph.
However, Any subsequent addition/removals to the original graph must be reflected in the external graph as well.

By performing Graph pruning as described above, the resultant graph will be a different representation of the original graph, and the traversal algorithms
will need to modified to suit the requirements.

