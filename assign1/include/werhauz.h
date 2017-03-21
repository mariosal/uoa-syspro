#ifndef WERHAUZ_H_
#define WERHAUZ_H_

#include <stdio.h>

struct Werhauz* WerhauzInit(size_t h1_num_entries, size_t h2_num_entries, size_t bucket_size, FILE* charge_file);
void WerhauzInsert(struct Werhauz* werhauz, const char* str);
void WerhauzDelete(struct Werhauz* werhauz, const char* caller, const char* id);
void WerhauzFind(struct Werhauz* werhauz, const char* caller, const char* str);
void WerhauzLookup(struct Werhauz* werhauz, const char* callee, const char* str);
void WerhauzIndist(struct Werhauz* werhauz, const char* caller1, const char* caller2);
void WerhauzTopdest(struct Werhauz* werhauz, const char* caller);
void WerhauzTop(struct Werhauz* werhauz, int k);
void WerhauzBye(struct Werhauz* werhauz);
void WerhauzPrint(struct Werhauz* werhauz, const char* str);
void WerhauzReset(struct Werhauz** werhauz);

#endif  // WERHAUZ_H_
