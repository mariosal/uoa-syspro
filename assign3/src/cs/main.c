#include <arpa/inet.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "cs/handler.h"
#include "misc.h"

static void* Thread(void* sock_desc) {
  if (pthread_detach(pthread_self())) {
    Error("detach");
  }
  int sock = *(int*)sock_desc;
  struct Handler* handler = HandlerInit(sock);
  HandlerRead(handler);
  HandlerReset(&handler);
  pthread_exit(NULL);
}

int main(int argc, char** argv) {
  uint16_t port = 5000;
  char root[BUFSIZE];
  strcpy(root, "etc");
  for (int i = 1; i < argc; ++i) {
    if (Equals(argv[i], "p")) {
      ++i;
      sscanf(argv[i], "%" SCNu16, &port);
    } else if (Equals(argv[i], "d")) {
      ++i;
      strcpy(root, argv[i]);
    }
  }
  chdir(root);

  int lsock = socket(AF_INET, SOCK_STREAM, 0);
  if (lsock < 0) {
    Error("socket");
  }

  struct sockaddr_in addr;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  addr.sin_family = AF_INET;
  setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
  if (bind(lsock, (struct sockaddr*)&addr, sizeof(addr))) {
    Error("bind");
  }
  if (listen(lsock, 5)) {
    Error("listen");
  }

  while (true) {
    int csock = accept(lsock, NULL,  NULL);
    if (csock < 0) {
      Error("accept");
    }
    pthread_t t;
    if (pthread_create(&t, NULL, Thread, &csock)) {
      Error("create");
    }
  }
}
