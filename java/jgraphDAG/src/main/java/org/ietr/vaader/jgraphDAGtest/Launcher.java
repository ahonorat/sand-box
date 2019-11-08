package org.ietr.vaader.jgraphDAGtest;

import java.util.function.Supplier;

import org.jgrapht.graph.DefaultEdge;
import org.jgrapht.graph.DirectedAcyclicGraph;
import org.jgrapht.util.SupplierUtil;

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
		//		final DirectedAcyclicGraph<DAGvertex, DAGedge> dag = new DirectedAcyclicGraph<>(null, eSupplier, false);
		final DirectedAcyclicGraph<DAGvertex, DAGedge> dag = new DirectedAcyclicGraph<>(DAGedge.class);

		DAGvertex readYUV = new DAGvertex("Read_YUV_0");
		DAGvertex split = new DAGvertex("Split_0");
		DAGvertex sobel = new DAGvertex("Sobel_0");

		dag.addVertex(readYUV);
		dag.addVertex(split);
		dag.addVertex(sobel);

		dag.addEdge(split, sobel, new DAGedge());
		//		dag.addEdge(split, sobel);


		// crash if clone

		for (DAGedge e: dag.edgeSet()) {
			System.err.println(e + " : " + dag.getEdgeSource(e) + " --> " + dag.getEdgeTarget(e));
		}
		System.err.println("Edges before clone.");



		DAGvertex ssSplitSobel = new DAGvertex("ssSplitSobel");
		DAGvertex seSplitSobel = new DAGvertex("seSplitSobel");
		dag.addVertex(ssSplitSobel);
		dag.addVertex(seSplitSobel);

		// faulty clone
		@SuppressWarnings("unchecked")
		final DirectedAcyclicGraph<DAGvertex, DAGedge> clone = (DirectedAcyclicGraph<DAGvertex, DAGedge>) dag.clone();				

		dag.addEdge(ssSplitSobel, seSplitSobel, new DAGedge());
		//		dag.addEdge(ssSplitSobel, seSplitSobel);

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
		dag.addEdge(split, ssSplitSobel, new DAGedge());
		//		dag.addEdge(split, ssSplitSobel);


		System.err.println("Everything went fine");

	}


}
