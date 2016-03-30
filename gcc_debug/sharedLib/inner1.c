#include "inner1.h"

#include "inner2.h"

#include <stdio.h>

void inner1() {
  printf("Inner1\n");
  inner2();
}
