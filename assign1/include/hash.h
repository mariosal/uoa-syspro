#ifndef HASH_H_
#define HASH_H_

#include <stdbool.h>
#include <stddef.h>
#include "cdr.h"
#include "num.h"

struct Hash* HashInit(size_t size, size_t bucket_size);
void HashReset(struct Hash** hash, bool remove);
void HashInsert(struct Hash* hash, const struct Num* num, struct Cdr* cdr);
void HashDelete(struct Hash* hash, const struct Num* num, const char* id);

#endif  // HASH_H_
