#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "werhauz.h"

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

static void Read(struct Werhauz* werhauz, FILE* in) {
  char cmd[512];
  char buf1[512];
  char buf2[512];
  char buf3[512];
  char buf4[512];
  while (fscanf(in, "%512s", cmd) != EOF) {
    if (Equals(cmd, "insert")) {
      fscanf(in, "%s", buf1);
      WerhauzInsert(werhauz, buf1);
    } else if (Equals(cmd, "delete")) {
      fscanf(in, "%s%s", buf1, buf2);
      WerhauzDelete(werhauz, buf1, buf2);
    } else if (Equals(cmd, "find")) {
      fscanf(in, "%512s%*[ \t\n\v\f\r]%512[^\n]", buf1, buf2);
      WerhauzFind(werhauz, buf1, buf2);
    } else if (Equals(cmd, "lookup")) {
      fscanf(in, "%512s%*[ \t\n\v\f\r]%512[^\n]", buf1, buf2);
      WerhauzLookup(werhauz, buf1, buf2);
    } else if (Equals(cmd, "indist1")) {
      fscanf(in, "%512s%512s", buf1, buf2);
      WerhauzIndist1(werhauz, buf1, buf2);
    } else if (Equals(cmd, "topdest")) {
      fscanf(in, "%512s", buf1);
      WerhauzTopdest(werhauz, buf1);
    } else if (Equals(cmd, "top")) {
      int k;
      fscanf(in, "%d", &k);
      WerhauzTop(werhauz, k);
    } else if (Equals(cmd, "bye")) {
      WerhauzBye(werhauz);
    } else if (Equals(cmd, "print")) {
      fscanf(in, "%512s", buf1);
      WerhauzPrint(werhauz, buf1);
    } else if (Equals(cmd, "exit")) {
      fclose(in);
      WerhauzReset(&werhauz);
      exit(EXIT_SUCCESS);
    }
  }
}

int main(int argc, char** argv) {
  setbuf(stdout, NULL);
  FILE* op = NULL;
  size_t h1_num_entries = 11;
  size_t h2_num_entries = 17;
  size_t bucket_size = 16;
  char buf[512];
  for (int i = 1; i < argc; ++i) {
    if (Equals(argv[i], "o")) {
      ++i;
      sscanf(argv[i], "%s", buf);
      op = fopen(buf, "r");
    } else if (Equals(argv[i], "h1")) {
      ++i;
      sscanf(argv[i], "%zu", &h1_num_entries);
    } else if (Equals(argv[i], "h2")) {
      ++i;
      sscanf(argv[i], "%zu", &h2_num_entries);
    } else if (Equals(argv[i], "s")) {
      ++i;
      sscanf(argv[i], "%zu", &bucket_size);
    }
  }

  FILE* charge_file = fopen("etc/charge.txt", "r");
  struct Werhauz* werhauz = WerhauzInit(h1_num_entries, h2_num_entries, bucket_size, charge_file);
  fclose(charge_file);

  if (op != NULL) {
    Read(werhauz, op);
    fclose(op);
  }
  Read(werhauz, stdin);
  return EXIT_SUCCESS;
}
