#ifndef HASH_H_
#define HASH_H_

#include <stdbool.h>
#include <stddef.h>
#include "cdr.h"

struct Hash* HashInit(size_t size, size_t bucket_size);
void HashReset(struct Hash** hash, bool remove);
void HashInsert(struct Hash* hash, long long num, struct Cdr* cdr);
void HashDelete(struct Hash* hash, long long num, const char* id);
void HashFind(const struct Hash* hash, long long num, int t1, int t2, int d1, int d2);
void HashTopdest(const struct Hash* hash, long long num);
void HashPrint(const struct Hash* hash);

#endif  // HASH_H_
