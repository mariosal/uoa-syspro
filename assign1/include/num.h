#ifndef NUM_H_
#define NUM_H_

#include <stdbool.h>
#include <stddef.h>

struct Num* NumInit(const char* str);
void NumReset(struct Num** num);
char* NumStr(const struct Num* num);
size_t NumMod(const struct Num* num, size_t modulo);
bool NumEquals(const struct Num* a, const struct Num* b);

#endif  // NUM_H_
