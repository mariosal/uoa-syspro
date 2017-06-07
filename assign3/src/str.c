#include "str.h"

#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#include "misc.h"

struct Str {
  size_t len;
  size_t size;
  char* s;
};

size_t StrLen(const struct Str* str) {
  return str->len;
}

const char* StrS(const struct Str* str) {
  return str->s;
}

struct Str* StrInit(void) {
  struct Str* str = malloc(sizeof(*str));
  if (str == NULL) {
    Error("malloc");
  }
  str->len = 0;
  str->size = 1;
  str->s = malloc(sizeof(*str->s) * str->size);
  str->s[0] = '\0';
  return str;
}

void StrReset(struct Str** str) {
  free((*str)->s);
  free(*str);
  *str = NULL;
}

void StrZero(struct Str* str) {
  str->len = 0;
  str->s[0] = '\0';
}

void StrInsert(struct Str* str, char c) {
  if (str->len + 1 == str->size) {
    str->size *= 2;
    str->s = realloc(str->s, sizeof(*str->s) * str->size);
  }
  str->s[str->len] = c;
  str->s[str->len + 1] = '\0';
  ++str->len;
}

void StrRead(struct Str* str, int fd) {
  StrZero(str);
  bool init = true;
  while (true) {
    char c;
    if (read(fd, &c, sizeof(c)) <= 0) {
      break;
    }
    if ((isspace(c) || iscntrl(c)) && init) {
      continue;
    }
    if (isspace(c) || iscntrl(c)) {
      break;
    }
    init = false;
    StrInsert(str, c);
  }
}

void StrReadNl(struct Str* str, int fd) {
  StrZero(str);
  bool init = true;
  while (true) {
    char c;
    if (read(fd, &c, sizeof(c)) <= 0) {
      break;
    }
    if ((isspace(c) || iscntrl(c)) && init) {
      continue;
    }
    if (c == '\n') {
      break;
    }
    init = false;
    StrInsert(str, c);
  }
}
