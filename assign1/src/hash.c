#include "hash.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int Max(int a, int b) {
  if (a < b) {
    return b;
  }
  return a;
}

struct CdrNode {
  size_t len;
  size_t size;
  struct Cdr** cdrs;
  struct CdrNode* next;
};

static struct CdrNode* CdrNodeInit(size_t size) {
  struct CdrNode* node = malloc(sizeof(*node));
  if (node == NULL) {
    exit(EXIT_FAILURE);
  }
  node->len = 0;
  node->size = size;
  node->next = NULL;
  node->cdrs = malloc(sizeof(*node->cdrs) * size);
  if (node->cdrs == NULL) {
    exit(EXIT_FAILURE);
  }
  for (size_t i = 0; i < size; ++i) {
    node->cdrs[i] = NULL;
  }
  return node;
}

static void CdrNodeInsert(struct CdrNode* node, struct Cdr* cdr) {
  if (node->len < node->size) {
    node->cdrs[node->len] = cdr;
    ++node->len;
  } else if (node->next != NULL) {
    CdrNodeInsert(node->next, cdr);
  } else {
    node->next = CdrNodeInit(node->size);
    CdrNodeInsert(node->next, cdr);
  }
}

static void CdrNodeDelete(struct CdrNode* node, const char* id) {
  for (size_t i = 0; i < node->len; ++i) {
    if (strcmp(CdrId(node->cdrs[i]), id) == 0) {
      struct CdrNode* it = node;
      while (it->next != NULL) {
        it = it->next;
      }
      struct Cdr* tmp = it->cdrs[it->len - 1];
      it->cdrs[it->len - 1] = node->cdrs[i];
      node->cdrs[i] = tmp;
      --it->len;
      return;
    }
  }
  CdrNodeDelete(node->next, id);
}

static void CdrNodePrint(const struct CdrNode* node, int t1, int t2, int d1, int d2) {
  if (node == NULL) {
    return;
  }
  for (size_t i = 0; i < node->len; ++i) {
    if (t1 <= CdrTime(node->cdrs[i]) && CdrTime(node->cdrs[i]) <= t2 &&
        d1 <= CdrDate(node->cdrs[i]) && CdrDate(node->cdrs[i]) <= d2) {
      CdrPrint(node->cdrs[i]);
    }
  }
  CdrNodePrint(node->next, t1, t2, d1, d2);
}

static void CdrNodeGetCdrs(const struct CdrNode* node, long long num, struct Array* a) {
  if (node == NULL) {
    return;
  }
  for (size_t i = 0; i < node->len; ++i) {
    if (CdrCaller(node->cdrs[i]) == num) {
      ArrayInsert(a, CdrCallee(node->cdrs[i]));
    } else {
      ArrayInsert(a, CdrCaller(node->cdrs[i]));
    }
  }
  CdrNodeGetCdrs(node->next, num, a);
}

static bool CdrNodeContains(const struct CdrNode* node, long long num1, long long num2) {
  if (node == NULL) {
    return false;
  }
  for (size_t i = 0; i < node->len; ++i) {
    if (CdrCaller(node->cdrs[i]) == num1) {
      if (CdrCallee(node->cdrs[i]) == num2) {
        return true;
      }
    } else {
      if (CdrCaller(node->cdrs[i]) == num2) {
        return true;
      }
    }
  }
  return CdrNodeContains(node->next, num1, num2);
}

static void CdrNodeTopdest(const struct CdrNode* node, long long num, int* map) {
  if (node == NULL) {
    return;
  }
  for (size_t i = 0; i < node->len; ++i) {
    ++map[CdrCallee(node->cdrs[i]) / 10000000000LL];
  }
  CdrNodeTopdest(node->next, num, map);
}

static void CdrNodeReset(struct CdrNode** node, bool remove) {
  if (node != NULL) {
    if (remove) {
      for (size_t i = 0; i < (*node)->len; ++i) {
        CdrReset(&(*node)->cdrs[i]);
      }
    }
    free((*node)->cdrs);
  }
  free(*node);
  *node = NULL;
}

struct BucketNode {
  size_t size;
  long long num;
  struct CdrNode* head;
};

static struct BucketNode* BucketNodeInit(long long num, size_t size) {
  struct BucketNode* node = malloc(sizeof(*node));
  if (node == NULL) {
    exit(EXIT_FAILURE);
  }
  node->num = num;
  node->size = size;
  node->head = NULL;
  return node;
}

static void BucketNodeInsert(struct BucketNode* node, struct Cdr* cdr) {
  if (node->head == NULL) {
    node->head = CdrNodeInit(node->size);
  }
  CdrNodeInsert(node->head, cdr);
}

static void BucketNodeDelete(struct BucketNode* node, const char* id) {
  if (node->head == NULL) {
    return;
  }
  CdrNodeDelete(node->head, id);
  if (node->head->len == 0) {
    CdrNodeReset(&node->head, false);
  }
}

static void BucketNodeReset(struct BucketNode** node, bool remove) {
  if (node != NULL) {
    struct CdrNode* it = (*node)->head;
    while (it != NULL) {
      struct CdrNode* tmp = it;
      it = it->next;
      CdrNodeReset(&tmp, remove);
    }
  }
  free(*node);
  *node = NULL;
}

struct HashBucket {
  size_t len;
  size_t size;
  struct BucketNode** nodes;
  struct HashBucket* next;
};

static struct HashBucket* HashBucketInit(size_t size) {
  struct HashBucket* bucket = malloc(sizeof(*bucket));
  if (bucket == NULL) {
    exit(EXIT_FAILURE);
  }
  bucket->len = 0;
  bucket->size = size;
  bucket->nodes = malloc(sizeof(*bucket->nodes) * size);
  bucket->next = NULL;
  if (bucket->nodes == NULL) {
    exit(EXIT_FAILURE);
  }
  for (size_t i = 0; i < size; ++i) {
    bucket->nodes[i] = NULL;
  }
  return bucket;
}

static void HashBucketInsert(struct HashBucket* bucket, long long num, struct Cdr* cdr) {
  for (size_t i = 0; i < bucket->len; ++i) {
    if (bucket->nodes[i]->num == num) {
      BucketNodeInsert(bucket->nodes[i], cdr);
      return;
    }
  }
  if (bucket->next != NULL) {
    HashBucketInsert(bucket->next, num, cdr);
  } else if (bucket->len < bucket->size) {
    bucket->nodes[bucket->len] = BucketNodeInit(num, bucket->size);
    BucketNodeInsert(bucket->nodes[bucket->len], cdr);
    ++bucket->len;
  } else {
    bucket->next = HashBucketInit(bucket->size);
    HashBucketInsert(bucket->next, num, cdr);
  }
}

static void HashBucketDelete(struct HashBucket* bucket, long long num, const char* id) {
  if (bucket == NULL) {
    return;
  }
  for (size_t i = 0; i < bucket->len; ++i) {
    if (bucket->nodes[i]->num == num) {
      BucketNodeDelete(bucket->nodes[i], id);
      if (bucket->nodes[i]->head == NULL) {
        BucketNodeReset(&bucket->nodes[i], false);
        struct HashBucket* it = bucket;
        while (it->next != NULL) {
          it = it->next;
        }
        struct BucketNode* tmp = it->nodes[it->len - 1];
        it->nodes[it->len - 1] = bucket->nodes[i];
        bucket->nodes[i] = tmp;
        --it->len;
        return;
      }
    }
  }
  HashBucketDelete(bucket->next, num, id);
}

static void HashBucketFind(const struct HashBucket* bucket, long long num, int t1, int t2, int d1, int d2) {
  if (bucket == NULL) {
    return;
  }
  for (size_t i = 0; i < bucket->len; ++i) {
    if (bucket->nodes[i]->num == num) {
      CdrNodePrint(bucket->nodes[i]->head, t1, t2, d1, d2);
      return;
    }
  }
  HashBucketFind(bucket->next, num, t1, t2, d1, d2);
}

static void HashBucketTopdest(const struct HashBucket* bucket, long long num) {
  if (bucket == NULL) {
    return;
  }
  for (size_t i = 0; i < bucket->len; ++i) {
    if (bucket->nodes[i]->num == num) {
      int map[1000];
      memset(map, 0, sizeof(map));
      CdrNodeTopdest(bucket->nodes[i]->head, num, map);
      int max = map[0];
      for (size_t j = 0; j < 1000; ++j) {
        max = Max(max, map[j]);
      }
      for (size_t j = 0; j < 1000; ++j) {
        if (map[j] == max && max != 0) {
          printf("Country code: %03zu, Calls made: %d\n", j, map[j]);
        }
      }
      return;
    }
  }
  HashBucketTopdest(bucket->next, num);
}

static void HashBucketPrint(const struct HashBucket* bucket) {
  if (bucket == NULL) {
    return;
  }
  for (size_t i = 0; i < bucket->len; ++i) {
    printf("Num %03lld-%010lld:\n", bucket->nodes[i]->num / 10000000000LL, bucket->nodes[i]->num % 10000000000LL);
    CdrNodePrint(bucket->nodes[i]->head, 0, 9999, 0, 99999999);
    printf("\n");
  }
  HashBucketPrint(bucket->next);
}

static void HashBucketGetCdrs(struct HashBucket* bucket, long long num, struct Array* a) {
  if (bucket == NULL) {
    return;
  }
  for (size_t i = 0; i < bucket->len; ++i) {
    if (bucket->nodes[i]->num == num) {
      CdrNodeGetCdrs(bucket->nodes[i]->head, num, a);
      return;
    }
  }
  HashBucketGetCdrs(bucket->next, num, a);
}

static bool HashBucketContains(const struct HashBucket* bucket, long long num1, long long num2) {
  if (bucket == NULL) {
    return false;
  }
  for (size_t i = 0; i < bucket->len; ++i) {
    if (bucket->nodes[i]->num == num1) {
      return CdrNodeContains(bucket->nodes[i]->head, num1, num2);
    }
  }
  return HashBucketContains(bucket->next, num1, num2);
}


static void HashBucketReset(struct HashBucket** bucket, bool remove) {
  if (bucket != NULL) {
    for (size_t i = 0; i < (*bucket)->len; ++i) {
      BucketNodeReset(&(*bucket)->nodes[i], remove);
    }
    free((*bucket)->nodes);
  }
  free(*bucket);
  *bucket = NULL;
}

struct Hash {
  size_t bucket_size;
  size_t size;
  struct HashBucket** table;
};

struct Hash* HashInit(size_t size, size_t bucket_size) {
  struct Hash* hash = malloc(sizeof(*hash));
  if (hash == NULL) {
    exit(EXIT_FAILURE);
  }

  hash->bucket_size = bucket_size / sizeof(struct BucketNode);
  hash->size = size;
  hash->table = malloc(sizeof(*hash->table) * size);
  if (hash->table == NULL) {
    exit(EXIT_FAILURE);
  }
  for (size_t i = 0; i < size; ++i) {
    hash->table[i] = NULL;
  }
  return hash;
}

void HashReset(struct Hash** hash, bool remove) {
  for (size_t i = 0; i < (*hash)->size; ++i) {
    struct HashBucket* it = (*hash)->table[i];
    while (it != NULL) {
      struct HashBucket* tmp = it;
      it = it->next;
      HashBucketReset(&tmp, remove);
    }
  }
  free((*hash)->table);
  free(*hash);
  *hash = NULL;
}

void HashBye(struct Hash* hash, bool remove) {
  for (size_t i = 0; i < hash->size; ++i) {
    struct HashBucket* it = hash->table[i];
    while (it != NULL) {
      struct HashBucket* tmp = it;
      it = it->next;
      HashBucketReset(&tmp, remove);
    }
    hash->table[i] = NULL;
  }
}

void HashInsert(struct Hash* hash, long long num, struct Cdr* cdr) {
  size_t pos = num % hash->size;
  if (hash->table[pos] == NULL) {
    hash->table[pos] = HashBucketInit(hash->bucket_size);
  }
  HashBucketInsert(hash->table[pos], num, cdr);
}

void HashDelete(struct Hash* hash, long long num, const char* id) {
  size_t pos = num % hash->size;
  HashBucketDelete(hash->table[pos], num, id);
  if (hash->table[pos]->len == 0) {
    HashBucketReset(&hash->table[pos], false);
  }
}

void HashFind(const struct Hash* hash, long long num, int t1, int t2, int d1, int d2) {
  if (t1 > t2) {
    HashFind(hash, num, t2, t1, d1, d2);
    return;
  }
  if (d1 > d2) {
    HashFind(hash, num, t1, t2, d2, d1);
    return;
  }
  size_t pos = num % hash->size;
  HashBucketFind(hash->table[pos], num, t1, t2, d1, d2);
}

void HashTopdest(const struct Hash* hash, long long num) {
  size_t pos = num % hash->size;
  HashBucketTopdest(hash->table[pos], num);
}

void HashPrint(const struct Hash* hash) {
  for (size_t i = 0; i < hash->size; ++i) {
    printf("Position %zu:\n", i);
    HashBucketPrint(hash->table[i]);
    printf("\n");
  }
}

void HashGetCdrs(const struct Hash* hash, long long num, struct Array* a) {
  size_t pos = num % hash->size;
  HashBucketGetCdrs(hash->table[pos], num, a);
}

bool HashContains(const struct Hash* hash, long long num1, long long num2) {
  size_t pos = num1 % hash->size;
  return HashBucketContains(hash->table[pos], num1, num2);
}
