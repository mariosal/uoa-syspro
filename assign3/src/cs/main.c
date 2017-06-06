#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

static bool Equals(const char* a, const char* b) {
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

int main(int argc, char** argv) {
  char r_name[128];
  char w_name[128];
  char op_name[128];
  r_name[0] = w_name[0] = op_name[0] = '\0';
  for (int i = 1; i < argc; ++i) {
    if (Equals(argv[i], "w")) {
      ++i;
      sscanf(argv[i], "%s", w_name);
    } else if (Equals(argv[i], "r")) {
      ++i;
      sscanf(argv[i], "%s", r_name);
    } else if (Equals(argv[i], "o")) {
      ++i;
      sscanf(argv[i], "%s", op_name);
    }
  }

  mkfifo(w_name, 0700);
  mkfifo(r_name, 0700);

  FILE* w = fopen(w_name, "w");
  FILE* r = fopen(r_name, "r");

  FILE* op = NULL;
  if (op_name[0] != '\0') {
    op = fopen(op_name, "r");
    Read(op);
  }
  Read(stdin);

  return 0;
}
