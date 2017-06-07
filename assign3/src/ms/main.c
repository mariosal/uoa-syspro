#include <arpa/inet.h>
#include <inttypes.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ms/queue.h"
#include "misc.h"
#include "str.h"

static bool Prefix(const char* pr, const char* s) {
  for (size_t i = 0; pr[i]; ++i) {
    if (pr[i] != s[i]) {
      return false;
    }
  }
  return true;
}

static void MirrorManager(struct Queue* q, const char* host, uint16_t port, const char* path, unsigned delay) {
  printf("%s\n", host);
  printf("%hu\n", port);
  printf("%s\n", path);
  printf("%u\n", delay);

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    Error("socket");
  }

  struct hostent* hp = gethostbyname(host);
  if (hp == NULL) {
    Error("gethostbyname");
  }

  struct sockaddr_in addr;
  memcpy(&addr.sin_addr, hp->h_addr_list[0], (size_t)hp->h_length);
  addr.sin_port = htons(port);
  addr.sin_family = AF_INET;
  if (connect(sock, (struct sockaddr*)&addr, sizeof(addr))) {
    Error("connect");
  }
  printf("ACCEPTED: %d\n", sock);

  char cmd[BUFSIZE];
  snprintf(cmd, sizeof(cmd), "LIST 0 %d\n", delay);
  WriteAll(sock, cmd, strlen(cmd));

  struct Str* buf = StrInit();
  StrRead(buf, sock);
  size_t num_dirs;
  sscanf(StrS(buf), "%zu", &num_dirs);

  for (size_t i = 0; i < num_dirs; ++i) {
    StrReadNl(buf, sock);
    if (Prefix(path + 2, StrS(buf) + 2)) {
      printf("%s\n", StrS(buf));
    }
  }

  StrRead(buf, sock);
  size_t num_files;
  sscanf(StrS(buf), "%zu", &num_files);

  for (size_t i = 0; i < num_files; ++i) {
    StrReadNl(buf, sock);
    if (Prefix(path + 2, StrS(buf) + 2)) {
      QueuePush(q, StrS(buf), sock);
    }
  }

  while (QueueLen(q) > 0) {
    char qpath[BUFSIZE];
    int qsock;
    QueuePop(q, qpath, &qsock);
    printf("%s %d\n", qpath, qsock);
  }

  StrReset(&buf);
  close(sock);
}

int main(int argc, char** argv) {
  uint16_t port = 6000;
  for (int i = 1; i < argc; ++i) {
    if (Equals(argv[i], "p")) {
      ++i;
      sscanf(argv[i], "%" SCNu16, &port);
    } else if (Equals(argv[i], "d")) {
      ++i;
    }
  }

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

    struct Queue* q = QueueInit();

    struct Str* buf = StrInit();
    StrRead(buf, csock);
    size_t num_cs;
    sscanf(StrS(buf), "%zu", &num_cs);

    for (size_t i = 0; i < num_cs; ++i) {
      char host[BUFSIZE];
      StrRead(buf, csock);
      strcpy(host, StrS(buf));

      uint16_t cs_port;
      StrRead(buf, csock);
      sscanf(StrS(buf), "%" SCNu16, &cs_port);

      char path[BUFSIZE];
      StrReadNl(buf, csock);
      strcpy(path, StrS(buf));

      unsigned delay;
      StrRead(buf, csock);
      sscanf(StrS(buf), "%u", &delay);

      MirrorManager(q, host, cs_port, path, delay);
    }
    StrReset(&buf);
    QueueReset(&q);
  }
}
