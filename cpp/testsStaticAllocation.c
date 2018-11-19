#include <stdlib.h>
#include <stdio.h>

// constant propagation and static allocation test

#define VAL_ARG 7
#define VAL_ADD 2


void allocate(int size) {
  int sizeA = size + VAL_ADD;
  //unsigned char* buffer; //does not work
  //buffer = unsigned char[size+VAL_ADD];

  int buffer[sizeA];

  int i;
  for (i = 0; i < sizeA; ++i) {
    buffer[i] = i;
  }
  for (i = 0; i < sizeA; ++i) {
    fprintf(stdout, "%d\n", buffer[i]);
  }
}

int main(int argc, char** argv) {
  allocate(VAL_ARG);
}
