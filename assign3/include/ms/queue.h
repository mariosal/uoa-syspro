#ifndef QUEUE_H_
#define QUEUE_H_

#include <stddef.h>

struct Queue* QueueInit(void);
void QueueReset(struct Queue** q);
size_t QueueLen(const struct Queue* q);
void QueuePop(struct Queue* q, char* path, int* sock);
void QueuePush(struct Queue* q, const char* path, int sock);

#endif  // QUEUE_H_
