#include <arpa/inet.h>
#include <inttypes.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "misc.h"
#include "str.h"

static size_t CountComma(const char* s) {
  size_t comma = 0;
  for (size_t i = 0; s[i]; ++i) {
    comma += s[i] == ',';
  }
  return comma;
}

int main(int argc, char** argv) {
  char host[BUFSIZE];
  uint16_t port = 6000;
  char servers[BUFSIZE];
  strcpy(host, "localhost");
  servers[0] = '\0';
  for (int i = 1; i < argc; ++i) {
    if (Equals(argv[i], "n")) {
      ++i;
      strcpy(host, argv[i]);
    } else if (Equals(argv[i], "p")) {
      ++i;
      sscanf(argv[i], "%" SCNu16, &port);
    } else if (Equals(argv[i], "s")) {
      ++i;
      strcpy(servers, argv[i]);
    }
  }

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

  size_t len = CountComma(servers) + 1;
  char buf[BUFSIZE];
  snprintf(buf, sizeof(buf), "%zu\n", len);
  WriteAll(sock, buf, strlen(buf));
  for (size_t i = 0; len > 0; ++i) {
    if (servers[i] == ',' || servers[i] == '\0') {
      write(sock, &"\n", sizeof(char));
      --len;
      continue;
    }
    if (servers[i] == ':') {
      write(sock, &"\n", sizeof(char));
      continue;
    }
    write(sock, &servers[i], sizeof(servers[i]));
  }

  struct Str* st = StrInit();
  StrRead(st, sock);
  size_t st_len;
  sscanf(StrS(st), "%zu", &st_len);
  for (size_t i = 0; i < st_len; ++i) {
    StrReadNl(st, sock);
    printf("%s\n", StrS(st));
  }
  StrReset(&st);
  close(sock);

  return 0;
}
