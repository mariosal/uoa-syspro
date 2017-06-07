#include "misc.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

bool Equals(const char* a, const char* b) {
  if (a[0] == '-') {
    return Equals(a + 1, b);
  }
  if (b[0] == '-') {
    return Equals(a, b + 1);
  }
  if (tolower(a[0]) == tolower(b[0])) {
    return a[0] == '\0' || Equals(a + 1, b + 1);
  }
  return false;
}

void Error(const char* msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}
