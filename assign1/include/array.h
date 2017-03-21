#ifndef ARRAY_H_
#define ARRAY_H_

#include <stddef.h>

struct Array* ArrayInit(void);
size_t ArrayLen(const struct Array* array);
void ArrayReset(struct Array** array);
void ArrayInsert(struct Array* array, long long n);
long long ArrayGet(const struct Array* array, size_t i);

#endif  // ARRAY_H_
