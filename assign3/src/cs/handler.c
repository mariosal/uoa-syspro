#include "cs/handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "misc.h"
#include "str.h"

struct Handler {
  unsigned int delay;
  int id;
  int sock;
};

struct Handler* HandlerInit(int sock) {
  struct Handler* handler = malloc(sizeof(*handler));
  if (handler == NULL) {
    Error("malloc");
  }
  handler->delay = 0;
  handler->id = 0;
  handler->sock = sock;
  return handler;
}

void HandlerReset(struct Handler** handler) {
  free(*handler);
  *handler = NULL;
}

void HandlerRead(struct Handler* handler) {
  struct Str* buf = StrInit();
  while (1) {
    StrRead(buf, handler->sock);
    if (StrLen(buf) == 0) {
      break;
    }
    if (!strcmp(StrS(buf), "LIST")) {
      StrRead(buf, handler->sock);
      sscanf(StrS(buf), "%d", &handler->id);
      StrRead(buf, handler->sock);
      sscanf(StrS(buf), "%u", &handler->delay);
      HandlerList(handler);
    } else {
      StrReadNl(buf, handler->sock);
      HandlerFetch(handler, StrS(buf));
    }
  }

  StrReset(&buf);
  close(handler->sock);
}

void HandlerList(struct Handler* handler) {
  WriteCmd(handler->sock, "find -type d | wc -l");
  WriteCmd(handler->sock, "find -type d");
  WriteCmd(handler->sock, "find -type f | wc -l");
  WriteCmd(handler->sock, "find -type f");
}

void HandlerFetch(struct Handler* handler, const char* path) {
  sleep(handler->delay);
  char buf[BUFSIZE];
  snprintf(buf, sizeof(buf), "wc -l < %s", path);
  WriteCmd(handler->sock, buf);
  snprintf(buf, sizeof(buf), "cat %s", path);
  WriteCmd(handler->sock, buf);
}
