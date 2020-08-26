// compile with -lpthread on Linux

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200112L
#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <sched.h>
#include <pthread.h>


#define INSIDE_LOOP 1e3
#define OUTSIDE_LOOP 1e6

pthread_barrier_t iter_barrier;


static inline void dumpTime(int id, uint64_t * dumpBuffer) {
    struct timespec clock;
    clock_gettime(CLOCK_MONOTONIC, &clock);
    dumpBuffer[id] = (uint64_t) (clock.tv_sec * 1e9) + clock.tv_nsec;
}

static inline double getElapsedNanoSec(uint64_t *start, uint64_t *end) {
  return ((*end) - (*start));
}

void timeConsumingFunction() {
    for (unsigned int i = 0; i < INSIDE_LOOP; i++) {
    long a = 2*i;
  }
}


void * timeConsumingFunctionInThread(void * nothing) {
  uint64_t tab[2] = {0, 0}; 
  dumpTime(0, tab);
  for (unsigned int i = 0; i < OUTSIDE_LOOP; i++) {
    timeConsumingFunction();
    pthread_barrier_wait(&iter_barrier);
  }
  dumpTime(1, tab);
  fprintf(stderr, "Elapsed ns: %lf\n", getElapsedNanoSec(tab, tab+1));

  return nothing;
}


int main(int argc, char **argv) {

  uint64_t tab[2] = {0, 0}; 

  fprintf(stderr, "--> Without pthread barrier:\n");
  dumpTime(0, tab);
  for (unsigned int i = 0; i < OUTSIDE_LOOP; i++) {
    timeConsumingFunction();
  }
  dumpTime(1, tab);
  fprintf(stderr, "Elapsed ns: %lf\n", getElapsedNanoSec(tab, tab+1));


  fprintf(stderr, "--> With pthread barrier:\n");
  pthread_barrier_init(&iter_barrier, NULL, 1);
  // even if replacing the following call by the thread call (commented below)
  // the barrier without any active thread creates overhead about 400 ms / 1e6 calls on my laptop
  timeConsumingFunctionInThread(NULL);
  /* // init pthread attributes */
  /* pthread_attr_t attr; */
  /* pthread_attr_init(&attr); */
  /* // init cpuset struct */
  /* cpu_set_t cpuset; */
  /* CPU_ZERO(&cpuset); */
  /* unsigned int core_id = 0; */
  /* CPU_SET(core_id, &cpuset); */
  /* // set pthread affinity */
  /* pthread_attr_setaffinity_np(&attr, sizeof(cpuset), &cpuset); */
  /* // */
  /* sched_setaffinity(getpid(), sizeof(cpuset), &cpuset); */
  /* // create thread */
  /* pthread_t thread; */
  /* pthread_create(&thread, &attr, &timeConsumingFunctionInThread, NULL); */
  /* pthread_join(thread, NULL); */
  
  
  return EXIT_SUCCESS;
}
