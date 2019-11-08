package org.ietr.vaader.jgraphDAGtest;

import java.util.List;
import java.util.function.Supplier;

import org.jgrapht.alg.cycle.JohnsonSimpleCycles;
import org.jgrapht.graph.DirectedAcyclicGraph;
import org.jgrapht.graph.SimpleDirectedGraph;

public class Launcher {


	public static class DAGvertex {
		String name;

		public DAGvertex(String name) {
			this.name = name;
		}

		public String toString() {
			return name;
		}
	}


	public static class DAGedge {
		static int count = 0;
		int id;

		public DAGedge() {
			id = count ++;
		}

		public String toString() {
			return "edge n°" + id;
		}
	}


	static public void main(String[] argv) {

		Supplier<DAGedge> eSupplier = new Supplier<DAGedge>() {
			@Override
			public DAGedge get() {
				return new DAGedge();
			}
		};


		// edge style does not change anything

		//		final DirectedAcyclicGraph<DAGvertex, DefaultEdge> dag = new DirectedAcyclicGraph<>(null, SupplierUtil.createDefaultEdgeSupplier(), false);
		final DirectedAcyclicGraph<DAGvertex, DAGedge> dag = new DirectedAcyclicGraph<>(null, eSupplier, false);

		DAGvertex readYUV = new DAGvertex("Read_YUV_0");
		DAGvertex split = new DAGvertex("Split_0");
		DAGvertex sobel = new DAGvertex("Sobel_0");

		dag.addVertex(readYUV);
		dag.addVertex(split);
		dag.addVertex(sobel);

		dag.addEdge(split, sobel);


		// crash if clone

		for (DAGedge e: dag.edgeSet()) {
			System.err.println(e + " : " + dag.getEdgeSource(e) + " --> " + dag.getEdgeTarget(e));
		}
		System.err.println("Edges before clone.");

		// faulty clone
		@SuppressWarnings("unchecked")
		final DirectedAcyclicGraph<DAGvertex, DAGedge> clone = (DirectedAcyclicGraph<DAGvertex, DAGedge>) dag.clone();				

		DAGvertex ssSplitSobel = new DAGvertex("ssSplitSobel");
		DAGvertex seSplitSobel = new DAGvertex("seSplitSobel");
		dag.addVertex(ssSplitSobel);
		dag.addVertex(seSplitSobel);

		dag.addEdge(ssSplitSobel, seSplitSobel);

		// clone at this place is fine
		//		@SuppressWarnings("unchecked")
		//		final DirectedAcyclicGraph<DAGvertex, DAGedge> clone = (DirectedAcyclicGraph<DAGvertex, DAGedge>) dag.clone();				


		for (DAGedge e: dag.edgeSet()) {
			System.err.println(e + " : " + dag.getEdgeSource(e) + " --> " + dag.getEdgeTarget(e));
		}
		System.err.println("Edges after clone.");
		for (DAGedge e: clone.edgeSet()) {
			System.err.println(e + " : " + clone.getEdgeSource(e) + " --> " + clone.getEdgeTarget(e));
		}
		System.err.println("Edges of clone.");

		// faulty new edge if clone
		try {
			dag.addEdge(split, ssSplitSobel);          
		} catch (IllegalArgumentException e) {
			System.err.println("ERRRROOOOOOR");
			SimpleDirectedGraph<DAGvertex, DAGedge> copy = new SimpleDirectedGraph<>(DAGedge.class);
			dag.vertexSet().forEach(copy::addVertex);
			dag.edgeSet().forEach(edge -> {
				DAGvertex src = dag.getEdgeSource(edge);
				DAGvertex tgt = dag.getEdgeTarget(edge);
				copy.addEdge(src, tgt, edge);
			});
			copy.addEdge(split, ssSplitSobel, new DAGedge());
			final JohnsonSimpleCycles<DAGvertex, DAGedge> cycleFinder = new JohnsonSimpleCycles<>(copy);
			final List<List<DAGvertex>> cycles = cycleFinder.findSimpleCycles();
			System.err.println("Cycle detector found: " + cycles.size() + " cycles.");
			for (List<DAGvertex> cycle : cycles) {
				final StringBuilder sb = new StringBuilder();
				cycle.stream().forEach(a -> sb.append(" -> " + a));
				System.err.println(sb.toString());
			}
			throw e;
		}

		System.err.println("Everything went fine");

	}


}
