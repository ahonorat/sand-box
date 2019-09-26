#include <omp.h>
#include <stdio.h>


int main(int argc, char **argv) {


#pragma omp parallel
  {
#pragma omp single
    {
      for (int i = 0; i < 4; ++i) {
#pragma omp task
	{
	  printf("IUH: thread %d\n", omp_get_thread_num());
    
	}
      }
    }
  }

  return 0;
}
