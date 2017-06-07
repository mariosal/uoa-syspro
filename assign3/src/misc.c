#include "misc.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

bool Equals(const char* a, const char* b) {
  if (a[0] == '-') {
    return Equals(a + 1, b);
  }
  if (b[0] == '-') {
    return Equals(a, b + 1);
  }
  if (tolower(a[0]) == tolower(b[0])) {
    return a[0] == '\0' || Equals(a + 1, b + 1);
  }
  return false;
}

void Error(const char* msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

size_t WriteAll(int fd, const void* buff, size_t count) {
  size_t written = 0;
  for (size_t i = 0; written < count; written += i) {
    i = (size_t)write(fd, (const unsigned char*)buff + written, count - written);
  }
  return written;
}

void WriteCmd(int fd, const char* cmd) {
  FILE* pipe = popen(cmd, "r");
  if (pipe == NULL) {
    Error("popen");
  }
  while (true) {
    char c;
    if (fread(&c, sizeof(c), 1, pipe) == 0) {
      break;
    }
    write(fd, &c, sizeof(c));
  }
  pclose(pipe);
}
