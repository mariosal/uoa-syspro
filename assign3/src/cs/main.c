#include <arpa/inet.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "str.h"

#define BUFSIZE 1024

static ssize_t WriteAll(int fd, const void* buff, size_t count) {
  ssize_t written = 0;
  for (ssize_t i = 0; written < count; written += i) {
    i = write(fd, (const unsigned char*)buff + written, count - written);
  }
  return written;
}

static void List(int sock, int id, int delay) {
  printf("Listing\n");
  FILE* pipe = popen("find -type d", "r");
  if (pipe == NULL) {
    Error("popen");
  }
  while (true) {
    char c;
    if (fread(&c, sizeof(c), 1, pipe) == 0) {
      break;
    }
    write(sock, &c, sizeof(c));
  }
  pclose(pipe);

  pipe = popen("find -type f", "r");
  if (pipe == NULL) {
    Error("popen");
  }
  write(sock, &"f\n", 2 * sizeof(char));
  while (true) {
    char c;
    if (fread(&c, sizeof(c), 1, pipe) == 0) {
      break;
    }
    write(sock, &c, sizeof(c));
  }
  pclose(pipe);
}

static void Fetch(int sock, int id, const char* path) {
  char buf[BUFSIZE];
  snprintf(buf, sizeof(buf), "cat %s", path);
  printf("%s\n", buf);
  FILE* pipe = popen(buf, "r");
  if (pipe == NULL) {
    Error("popen");
  }
  while (true) {
    char c;
    if (fread(&c, sizeof(c), 1, pipe) == 0) {
      break;
    }
    write(sock, &c, sizeof(c));
  }
  pclose(pipe);
}

static void Handle(void* sock_desc, const char* root) {
  int sock = *(int*)sock_desc;
  struct Str* buf = StrInit();
  int id = 0;
  int delay = 0;

  StrRead(buf, sock);
  if (!strcmp(StrS(buf), "LIST")) {
    StrReadNl(buf, sock);
    sscanf(StrS(buf), "%d%d", &id, &delay);
    List(sock, id, delay);
  } else {
    StrRead(buf, sock);
    sscanf(StrS(buf), "%d", &id);
    StrReadNl(buf, sock);
    Fetch(sock, id, StrS(buf));
  }

  StrReset(&buf);
  close(sock);
}

int main(int argc, char** argv) {
  setbuf(stdout, NULL);
  int port = 5000;
  char root[BUFSIZE];
  strcpy(root, "etc");
  for (int i = 1; i < argc; ++i) {
    if (Equals(argv[i], "p")) {
      ++i;
      sscanf(argv[i], "%d", &port);
    } else if (Equals(argv[i], "d")) {
      ++i;
      strcpy(root, argv[i]);
    }
  }

  printf("%d %s\n", port, root);
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
    Handle(&csock, root);
  }

  return EXIT_SUCCESS;
}
