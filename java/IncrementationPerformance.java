//*******************************************************************
// Made with www.compilejava.net
//*******************************************************************

import java.util.function.Consumer;

public class IncrementationPerformance
{
  
  public int a;
  
  public static void main(String[] args)
  {
 	Consumer<int[]> r = tab -> tab[0]++;
	int[] d = {0};
    long startTime = System.nanoTime();
    for (int i = 0; i < 10000000; ++i) {
      	d[0] = 0;
    	r.accept(d);
    }
    long endTime = System.nanoTime();    
    System.out.println("Tab("+d[0]+") time: "+(endTime-startTime));

    IncrementationPerformance t = new IncrementationPerformance();
    Consumer<IncrementationPerformance> s = cl -> cl.a++;
    startTime = System.nanoTime();
    for (int i = 0; i < 10000000; ++i) {
      	t.a = 0;
    	s.accept(t);
    }
    endTime = System.nanoTime();    
    System.out.println("Class("+t.a+") time: "+(endTime-startTime));
    
    int c = 0;
    startTime = System.nanoTime();
    for (int i = 0; i < 10000000; ++i) {
	    c = 0;
    	c++;
    }
    endTime = System.nanoTime();    
    System.out.println("Standard("+c+") time: "+(endTime-startTime));
  }
}
