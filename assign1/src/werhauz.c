#include "werhauz.h"
#include <stdlib.h>
#include "hash.h"

struct Werhauz {
  struct Hash* h1;
  struct Hash* h2;
};

struct Werhauz* WerhauzInit(size_t h1_num_entries, size_t h2_num_entries, size_t bucket_size, FILE* charge_file) {
  struct Werhauz* werhauz = malloc(sizeof(*werhauz));
  if (werhauz == NULL) {
    exit(EXIT_FAILURE);
  }
  werhauz->h1 = HashInit(h1_num_entries, bucket_size);
  werhauz->h2 = HashInit(h2_num_entries, bucket_size);
  return werhauz;
}

void WerhauzInsert(struct Werhauz* werhauz, const char* str) {
  struct Cdr* cdr = CdrInit(str);
  HashInsert(werhauz->h1, CdrCaller(cdr), cdr);
  HashInsert(werhauz->h2, CdrCallee(cdr), cdr);
}

void WerhauzDelete(struct Werhauz* werhauz, const char* caller, const char* id) {
  struct Num* num = NumInit(caller);
  HashDelete(werhauz->h1, num, id);
  NumReset(&num);
}

void WerhauzFind(struct Werhauz* werhauz, const char* caller, const char* str) {
}

void WerhauzLookup(struct Werhauz* werhauz, const char* callee, const char* str) {
}

void WerhauzIndist1(struct Werhauz* werhauz, const char* caller1, const char* caller2) {
}

void WerhauzTopdest(struct Werhauz* werhauz, const char* caller) {
}

void WerhauzTop(struct Werhauz* werhauz, int k) {
}

void WerhauzBye(struct Werhauz* werhauz) {
}

void WerhauzPrint(struct Werhauz* werhauz, const char* str) {
}

void WerhauzReset(struct Werhauz** werhauz) {
  HashReset(&(*werhauz)->h1, false);
  HashReset(&(*werhauz)->h2, true);
  free(*werhauz);
  *werhauz = NULL;
}
