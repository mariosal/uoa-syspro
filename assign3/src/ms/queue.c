#include "ms/queue.h"
#include <pthread.h>
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
  pthread_mutex_t mutex;
  pthread_cond_t cond;
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

  if (pthread_cond_init(&q->cond, NULL)) {
    Error("cond_init");
  }
  if (pthread_mutex_init(&q->mutex, NULL)) {
    Error("mutex_init");
  }
  return q;
}

void QueueReset(struct Queue** q) {
  if (*q != NULL) {
    pthread_mutex_lock(&(*q)->mutex);
    struct QueueNode* iter = (*q)->front;
    while (iter != NULL) {
      struct QueueNode* tmp = iter;
      iter = iter->next;
      free(tmp);
    }
    pthread_mutex_unlock(&(*q)->mutex);
    pthread_mutex_destroy(&(*q)->mutex);
    pthread_cond_destroy(&(*q)->cond);
    free(*q);
    *q = NULL;
  }
}

size_t QueueLen(struct Queue* q) {
  size_t ret;
  pthread_mutex_lock(&q->mutex);
  ret = q->len;
  pthread_mutex_unlock(&q->mutex);
  return ret;
}

void QueuePop(struct Queue* q, char* path, int* sock) {
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
  pthread_mutex_lock(&q->mutex);

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
  if (q->len == 1) {
    pthread_cond_broadcast(&q->cond);
  }
  pthread_mutex_unlock(&q->mutex);
}
