#include "num.h"
#include <stdio.h>
#include <stdlib.h>

struct Num {
  int prefix;
  int num;
};

struct Num* NumInit(const char* str) {
  struct Num* num = malloc(sizeof(*num));
  if (num == NULL) {
    exit(EXIT_FAILURE);
  }
  sscanf(str, "%d%d", &num->prefix, &num->num);
  return num;
}

void NumReset(struct Num** num) {
  free(*num);
  *num = NULL;
}
