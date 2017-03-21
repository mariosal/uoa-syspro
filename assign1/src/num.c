#include "num.h"
#include <stdio.h>
#include <stdlib.h>

struct Num {
  int prefix;
  long long num;
};

struct Num* NumInit(const char* str) {
  struct Num* num = malloc(sizeof(*num));
  if (num == NULL) {
    exit(EXIT_FAILURE);
  }
  sscanf(str, "%d-%lld", &num->prefix, &num->num);
  return num;
}

void NumReset(struct Num** num) {
  free(*num);
  *num = NULL;
}

char* NumStr(const struct Num* num) {
  char* str = malloc(sizeof(*str) * 15);
  snprintf(str, sizeof(*str) * 14, "%03d-%010lld", num->prefix, num->num);
  return str;
}

size_t NumMod(const struct Num* num, size_t modulo) {
  return ((num->prefix % modulo) + (num->num % modulo)) % modulo;
}

bool NumEquals(const struct Num* a, const struct Num* b) {
  return a->prefix == b->prefix && a->num == b->num;
}
