#include <stdio.h>
#include <limits.h>
#include <float.h>

int main(int argc, char **argv) {

  char dummy[512];
  
  int intD = snprintf(dummy, sizeof dummy, "%d", INT_MIN);
  printf("Number of char max for int:\t%d (%d)\n", intD, INT_MIN);
  int float2f = snprintf(dummy, sizeof dummy, "%.2f", -FLT_MAX);
  printf("Number of char max for float2f:\t%d (%.2f)\n", float2f, -FLT_MAX);

  return 0;
}
