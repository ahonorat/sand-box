# jgraphDAG

This small project shows a bug when using jgraph edge supplier and clone on a DAG.
Edge supplier is needed when computing transitive closure of a graph (see org.jgrapht.alg.TransitiveClosure).

## Bug:

After calling clone() method, jgraph states that the graph is not a DAG when adding another edge.
When not executing the clone() method, everything is fine.

## Source of bug:

No idea.

## Work around:

Copy the graph by another method.