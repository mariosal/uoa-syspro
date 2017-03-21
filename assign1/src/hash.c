#include "hash.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
  struct Num* num;
  struct CdrNode* head;
};

static struct BucketNode* BucketNodeInit(const struct Num* num, size_t size) {
  struct BucketNode* node = malloc(sizeof(*node));
  if (node == NULL) {
    exit(EXIT_FAILURE);
  }
  char* str = NumStr(num);
  node->num = NumInit(str);
  free(str);
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
    NumReset(&(*node)->num);
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

static void HashBucketInsert(struct HashBucket* bucket, const struct Num* num, struct Cdr* cdr) {
  for (size_t i = 0; i < bucket->len; ++i) {
    if (NumEquals(bucket->nodes[i]->num, num)) {
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

static void HashBucketDelete(struct HashBucket* bucket, const struct Num* num, const char* id) {
  if (bucket == NULL) {
    return;
  }
  for (size_t i = 0; i < bucket->len; ++i) {
    if (NumEquals(bucket->nodes[i]->num, num)) {
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

  hash->bucket_size = bucket_size;
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

void HashInsert(struct Hash* hash, const struct Num* num, struct Cdr* cdr) {
  size_t pos = NumMod(num, hash->size);
  if (hash->table[pos] == NULL) {
    hash->table[pos] = HashBucketInit(hash->bucket_size);
  }
  HashBucketInsert(hash->table[pos], num, cdr);
}

void HashDelete(struct Hash* hash, const struct Num* num, const char* id) {
  size_t pos = NumMod(num, hash->size);
  HashBucketDelete(hash->table[pos], num, id);
  if (hash->table[pos]->len == 0) {
    HashBucketReset(&hash->table[pos], false);
  }
}
