#include "inta.h"

#include <stdlib.h>

#include "misc.h"

struct Inta {
  size_t len;
  size_t size;
  size_t* s;
};

size_t IntaLen(const struct Inta* inta) {
  return inta->len;
}

const size_t* IntaS(const struct Inta* inta) {
  return inta->s;
}

struct Inta* IntaInit(void) {
  struct Inta* inta = malloc(sizeof(*inta));
  if (inta == NULL) {
    Error("malloc");
  }
  inta->len = 0;
  inta->size = 1;
  inta->s = malloc(sizeof(*inta->s) * inta->size);
  return inta;
}

void IntaReset(struct Inta** inta) {
  free((*inta)->s);
  free(*inta);
  *inta = NULL;
}

void IntaInsert(struct Inta* inta, size_t c) {
  if (inta->len == inta->size) {
    inta->size *= 2;
    inta->s = realloc(inta->s, sizeof(*inta->s) * inta->size);
  }
  inta->s[inta->len] = c;
  ++inta->len;
}
