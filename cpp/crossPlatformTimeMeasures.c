#include <stdio.h>

#ifdef _WIN32

#include <windows.h>
// override uint64_t
#ifdef uint64_t
#undef uint64_t
#endif
typedef unsigned _int64 uint64_t;


static inline void dumpTime(int id, uint64_t * dumpBuffer) {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    dumpBuffer[id] = now.QuadPart;
}

static inline double getElapsedNanoSec(uint64_t *start, uint64_t *end) {
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    double elapsed = ((*end) - (*start)) * ((double) 1e9);
    return elapsed / frequency.QuadPart;
}

#elif defined(__APPLE__) && defined(__MACH__)

#include <unistd.h>
#include <stdint.h>
#include <mach/mach_time.h>
#include <stdlib.h>

static inline void dumpTime(int id, uint64_t * dumpBuffer) {
	dumpBuffer[id] = mach_absolute_time();
}


static inline double getElapsedNanoSec(uint64_t *start, uint64_t *end) {
	mach_timebase_info_data_t timeBase;
	mach_timebase_info(&timeBase);
	fprintf(stderr, "Mach time: %i/%i\n", timeBase.numer, timeBase.denom);
	double res = (double) timeBase.numer /	timeBase.denom;
	res *= ((*end) - (*start));
	return res;
}


#elif defined(__gnu_linux__)  //defined(__linux__) && !defined(__ANDROID__) instead of __GNUC__ but note that clang also defines __GNUC__ 

#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>

static inline void dumpTime(int id, uint64_t * dumpBuffer) {
    struct timespec clock;
    clock_gettime(CLOCK_MONOTONIC, &clock);
    dumpBuffer[id] = (uint64_t) (clock.tv_sec * 1e9) + clock.tv_nsec;
}

static inline double getElapsedNanoSec(uint64_t *start, uint64_t *end) {
    return ((*end) - (*start));
}

#else // Undefined or Unsupported platform, thus no assumption is made on available hardware

static inline void dumpTime(int id, uint64_t * dumpBuffer) {
    dumpBuffer[id] = 0;
}

static inline double getElapsedNanoSec(uint64_t *start, uint64_t *end) {
    return 0;
}
#endif




int main(int argc, char **argv) {
  uint64_t tab[2] = {0, 0}; 
  dumpTime(0, tab);
#ifdef _WIN32
  Sleep(1000); // ms on windows
#else
  sleep(1); // s on Linux
#endif 
  dumpTime(1, tab);
  fprintf(stderr, "Elapsed ns: %lf\n", getElapsedNanoSec(tab, tab+1));
  
  return EXIT_SUCCESS;
}
