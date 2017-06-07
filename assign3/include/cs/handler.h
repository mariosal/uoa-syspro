#ifndef HANDLER_H_
#define HANDLER_H_

struct Handler* HandlerInit(int sock);
void HandlerReset(struct Handler** handler);
void HandlerRead(struct Handler* handler);
void HandlerList(struct Handler* handler);
void HandlerFetch(struct Handler* handler, const char* path);

#endif  // HANDLER_H_
