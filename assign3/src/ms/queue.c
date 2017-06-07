#include "ms/queue.h"
#include <stdio.h>
#include <stdlib.h>

#include "misc.h"

struct QueueNode {
  char path[BUFSIZE];
  int sock;
  struct QueueNode* next;
};

struct Queue {
  size_t len;
  struct QueueNode* front;
  struct QueueNode* back;
};

struct Queue* QueueInit(void) {
  struct Queue* q = malloc(sizeof(*q));
  if (q == NULL) {
    Error("malloc");
  }
  q->len = 0;
  q->front = q->back = NULL;
  return q;
}

void QueueReset(struct Queue** q) {
  if (*q != NULL) {
    struct QueueNode* iter = (*q)->front;
    while (iter != NULL) {
      struct QueueNode* tmp = iter;
      iter = iter->next;
      free(tmp);
    }
    free(*q);
    *q = NULL;
  }
}

size_t QueueLen(const struct Queue* q) {
  return q->len;
}

void QueuePop(struct Queue* q, char* path, int* sock) {
  if (q->len == 0) {
    *path = 0;
    *sock = 0;
    return;
  }

  snprintf(path, sizeof(q->front->path), "%s", q->front->path);
  *sock = q->front->sock;

  struct QueueNode* tmp = q->front;
  q->front = q->front->next;
  if (q->front == NULL) {
    q->back = NULL;
  }
  free(tmp);
  --q->len;
}

void QueuePush(struct Queue* q, const char* path, int sock) {
  struct QueueNode* tmp = malloc(sizeof(*tmp));
  if (tmp == NULL) {
    Error("malloc");
  }

  snprintf(tmp->path, sizeof(tmp->path), "%s", path);
  tmp->sock = sock;
  tmp->next = NULL;

  if (q->front == NULL) {
    q->front = tmp;
  } else {
    q->back->next = tmp;
  }
  q->back = tmp;
  ++q->len;
}
