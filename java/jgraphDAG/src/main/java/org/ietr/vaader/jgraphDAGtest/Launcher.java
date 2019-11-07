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

		
		final DirectedAcyclicGraph<DAGvertex, DefaultEdge> dag = new DirectedAcyclicGraph<>(null, SupplierUtil.createDefaultEdgeSupplier(), false);
	
		DAGvertex readYUV = new DAGvertex("Read_YUV_0");
		DAGvertex split = new DAGvertex("Split_0");
		DAGvertex sobel = new DAGvertex("Sobel_0");

		dag.addVertex(readYUV);
		dag.addVertex(split);
		dag.addVertex(sobel);
		
		
		
//		dag.addEdge(readYUV, split, new DAGedge());
//		dag.addEdge(split, sobel, new DAGedge());

//		dag.addEdge(readYUV, split);		
		dag.addEdge(split, sobel);

		
		// crash if clone
		
		@SuppressWarnings("unchecked")
		final DirectedAcyclicGraph<DAGvertex, DAGedge> clone = (DirectedAcyclicGraph<DAGvertex, DAGedge>) dag.clone();				
		
		DAGvertex ssSplitSobel = new DAGvertex("ssSplitSobel");
		DAGvertex seSplitSobel = new DAGvertex("seSplitSobel");
		dag.addVertex(ssSplitSobel);
		dag.addVertex(seSplitSobel);
		
		
//		dag.addEdge(ssSplitSobel, seSplitSobel, new DAGedge());
//		dag.addEdge(split, ssSplitSobel, new DAGedge());

		dag.addEdge(ssSplitSobel, seSplitSobel);
		dag.addEdge(split, ssSplitSobel);
		
		System.err.println("Everything went fine");
		
	}

    
}
