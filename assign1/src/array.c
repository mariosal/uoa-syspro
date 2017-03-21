#include "array.h"
#include <stdlib.h>

struct Array {
  size_t len;
  size_t size;
  long long* a;
};

size_t ArrayLen(const struct Array* array) {
  return array->len;
}

struct Array* ArrayInit(void) {
  struct Array* array = malloc(sizeof(*array));
  if (array == NULL) {
    exit(EXIT_FAILURE);
  }
  array->len = 0;
  array->size = 1;
  array->a = malloc(sizeof(*array->a) * array->size);
  return array;
}

void ArrayReset(struct Array** array) {
  free((*array)->a);
  free(*array);
  *array = NULL;
}

void ArrayInsert(struct Array* array, long long n) {
  if (array->len == array->size) {
    array->size *= 2;
    array->a = realloc(array->a, sizeof(*array->a) * array->size);
  }
  array->a[array->len] = n;
  ++array->len;
}

long long ArrayGet(const struct Array* array, size_t i) {
  if (array->len <= i) {
    return 0;
  }
  return array->a[i];
}
