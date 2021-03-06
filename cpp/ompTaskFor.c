#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LOOP 1000000000


int main(int argc, char **argv) {


  if (argc > 1 ) {
    omp_set_num_threads(atoi(argv[1]));
  }
  
  int a = 0;
  
#pragma omp parallel
  {
#pragma omp single
    {

      // path 1
#pragma omp task
      {

	// node 0
#pragma omp task
	{
	  fprintf(stderr, "Begin of path 1\n");
	}
#pragma omp taskwait
	
	// node 1
	for (int i = 2; i <= 8; i += 2) {
#pragma omp task
	  {
	    for (int j = 0; j <= MAX_LOOP/i; ++j) {
	      if (j == MAX_LOOP/i) {
#pragma omp critical
		{
		  a += 1;
		}	      
	      }
	    }
	    fprintf(stderr, "IUH: task finished: thread %d (i: %d)\n", omp_get_thread_num(), i);
	  }
	}
#pragma omp taskwait
	// node 2
#pragma omp task
	{
	  fprintf(stderr, "End of path 1: thread %d (result: %d)\n", omp_get_thread_num(), a);
	}
	
      }

      // path 2
#pragma omp task
      {
	// node 0
#pragma omp task
	{
	  fprintf(stderr, "Begin of path 2\n");
	}
#pragma omp taskwait
	// node 1
	for (int i = 1; i <= 7; i += 2) {
#pragma omp task
	  {

#pragma omp task
	    {
	      for (int j = 0; j <= MAX_LOOP/i; ++j) {
		if (j == MAX_LOOP/i) {
#pragma omp critical
		  {
		    a += 1;
		  }	      
		}
	      }
	      fprintf(stderr, "OZG: subtask finished: thread %d (i: %d)\n", omp_get_thread_num(), i);
	    }
	    fprintf(stderr, "OZG: task finished: thread %d (i: %d)\n", omp_get_thread_num(), i);
#pragma omp taskwait
	    // this last taskwait is needed because otherwise the subtask of node 1 are not waited by node 2
	    // the other possibility is to create a task group ... but it seems slower, but maybe safer?
	  }
	}
#pragma omp taskwait
	// node 2
#pragma omp task
	{
	  fprintf(stderr, "End of path 2: thread %d (result: %d)\n", omp_get_thread_num(), a);
	}
      }
    }
  }

  return 0;
}
