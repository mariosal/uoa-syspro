#include "werhauz.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "date.h"
#include "hash.h"
#include "array.h"

struct Werhauz {
  struct Hash* h1;
  struct Hash* h2;
};

struct Werhauz* WerhauzInit(size_t h1_num_entries, size_t h2_num_entries, size_t bucket_size, FILE* charge_file) {
  struct Werhauz* werhauz = malloc(sizeof(*werhauz));
  if (werhauz == NULL) {
    exit(EXIT_FAILURE);
  }
  werhauz->h1 = HashInit(h1_num_entries, bucket_size);
  werhauz->h2 = HashInit(h2_num_entries, bucket_size);
  return werhauz;
}

void WerhauzInsert(struct Werhauz* werhauz, const char* str) {
  struct Cdr* cdr = CdrInit(str);
  HashInsert(werhauz->h1, CdrCaller(cdr), cdr);
  HashInsert(werhauz->h2, CdrCallee(cdr), cdr);
}

void WerhauzDelete(struct Werhauz* werhauz, const char* caller, const char* id) {
  HashDelete(werhauz->h1, Num(caller), id);
}

void WerhauzFind(struct Werhauz* werhauz, const char* caller, const char* str) {
  int num_time = 0;
  for (size_t i = 0; str[i] != '\0'; ++i) {
    num_time += str[i] == ':';
  }
  int num_date = 0;
  int cont_char = 0;
  for (size_t i = 0; str[i] != '\0'; ++i) {
    cont_char = !isspace(str[i]) * (cont_char + 1);
    num_date += cont_char == 8;
  }

  char t1[6] = "00:00";
  char t2[6] = "99:99";
  char d1[9] = "00000000";
  char d2[9] = "99999999";
  char tmp[9];
  tmp[0] = '\0';
  int len = 0;
  bool is_time = false;
  bool sec = false;
  if (num_time == 2) {
    for (size_t i = 0; str[i] != '\0'; ++i) {
      if (isspace(str[i])) {
        tmp[0] = '\0';
        len = 0;
        continue;
      }
      tmp[len] = str[i];
      tmp[len + 1] = '\0';
      ++len;
      if (tmp[len - 1] == ':') {
        is_time = true;
      }
      if (len == 5 && is_time) {
        if (sec == false) {
          strcpy(t1, tmp);
          is_time = false;
          sec = true;
        } else {
          strcpy(t2, tmp);
          is_time = false;
        }
      }
    }
  }
  sec = false;
  if (num_date == 2) {
    for (size_t i = 0; str[i] != '\0'; ++i) {
      if (isspace(str[i])) {
        tmp[0] = '\0';
        len = 0;
        continue;
      }
      tmp[len] = str[i];
      tmp[len + 1] = '\0';
      ++len;
      if (len == 8) {
        if (sec == false) {
          strcpy(d1, tmp);
          sec = true;
        } else {
          strcpy(d2, tmp);
        }
      }
    }
  }
  HashFind(werhauz->h1, Num(caller), Time(t1), Time(t2), Date(d1), Date(d2));
}

void WerhauzLookup(struct Werhauz* werhauz, const char* callee, const char* str) {
  int num_time = 0;
  for (size_t i = 0; str[i] != '\0'; ++i) {
    num_time += str[i] == ':';
  }
  int num_date = 0;
  int cont_char = 0;
  for (size_t i = 0; str[i] != '\0'; ++i) {
    cont_char = !isspace(str[i]) * (cont_char + 1);
    num_date += cont_char == 8;
  }

  char t1[6] = "00:00";
  char t2[6] = "99:99";
  char d1[9] = "00000000";
  char d2[9] = "99999999";
  char tmp[9];
  tmp[0] = '\0';
  int len = 0;
  bool is_time = false;
  bool sec = false;
  if (num_time == 2) {
    for (size_t i = 0; str[i] != '\0'; ++i) {
      if (isspace(str[i])) {
        tmp[0] = '\0';
        len = 0;
        continue;
      }
      tmp[len] = str[i];
      tmp[len + 1] = '\0';
      ++len;
      if (tmp[len - 1] == ':') {
        is_time = true;
      }
      if (len == 5 && is_time) {
        if (sec == false) {
          strcpy(t1, tmp);
          is_time = false;
          sec = true;
        } else {
          strcpy(t2, tmp);
          is_time = false;
        }
      }
    }
  }
  sec = false;
  if (num_date == 2) {
    for (size_t i = 0; str[i] != '\0'; ++i) {
      if (isspace(str[i])) {
        tmp[0] = '\0';
        len = 0;
        continue;
      }
      tmp[len] = str[i];
      tmp[len + 1] = '\0';
      ++len;
      if (len == 8) {
        if (sec == false) {
          strcpy(d1, tmp);
          sec = true;
        } else {
          strcpy(d2, tmp);
        }
      }
    }
  }
  HashFind(werhauz->h2, Num(callee), Time(t1), Time(t2), Date(d1), Date(d2));
}

void WerhauzIndist(struct Werhauz* werhauz, const char* caller1, const char* caller2) {
  struct Array* a1 = ArrayInit();
  HashGetCdrs(werhauz->h1, Num(caller1), a1);
  HashGetCdrs(werhauz->h2, Num(caller1), a1);

  struct Array* a2 = ArrayInit();
  HashGetCdrs(werhauz->h1, Num(caller2), a2);
  HashGetCdrs(werhauz->h2, Num(caller2), a2);

  struct Array* a3 = ArrayInit();
  for (size_t i = 0; i < ArrayLen(a1); ++i) {
    for (size_t j = 0; j < ArrayLen(a2); ++j) {
      if (ArrayGet(a1, i) == ArrayGet(a2, j)) {
        bool dupl = false;
        for (size_t k = 0; k < ArrayLen(a3); ++k) {
          if (ArrayGet(a3, k) == ArrayGet(a1, i)) {
            dupl = true;
            break;
          }
        }
        if (!dupl) {
          ArrayInsert(a3, ArrayGet(a1, i));
        }
      }
    }
  }

  bool* map = malloc(sizeof(*map) * ArrayLen(a3));
  memset(map, 0, sizeof(*map) * ArrayLen(a3));
  for (size_t i = 0; i < ArrayLen(a3); ++i) {
    for (size_t j = 0; j < ArrayLen(a3); ++j) {
      if (ArrayGet(a3, i) == ArrayGet(a3, j)) {
        continue;
      }
      if (HashContains(werhauz->h1, ArrayGet(a3, i), ArrayGet(a3, j)) ||
          HashContains(werhauz->h1, ArrayGet(a3, j), ArrayGet(a3, i)) ||
          HashContains(werhauz->h2, ArrayGet(a3, i), ArrayGet(a3, j)) ||
          HashContains(werhauz->h2, ArrayGet(a3, j), ArrayGet(a3, i))) {
        map[i] = true;
        map[j] = true;
      }
    }
  }

  printf("Indist:\n");
  for (size_t i = 0; i < ArrayLen(a3); ++i) {
    if (!map[i] && ArrayGet(a3, i) != Num(caller1) && ArrayGet(a3, i) != Num(caller2)) {
      printf("%03lld-%010lld\n", ArrayGet(a3, i) / 10000000000LL, ArrayGet(a3, i) % 10000000000LL);
    }
  }
  printf("\n");
  ArrayReset(&a1);
  ArrayReset(&a2);
  ArrayReset(&a3);
  free(map);
}

void WerhauzTopdest(struct Werhauz* werhauz, const char* caller) {
  HashTopdest(werhauz->h1, Num(caller));
}

void WerhauzTop(struct Werhauz* werhauz, int k) {
}

void WerhauzBye(struct Werhauz* werhauz) {
  HashBye(werhauz->h1, false);
  HashBye(werhauz->h2, true);
}

void WerhauzPrint(struct Werhauz* werhauz, const char* str) {
  if (str[9] == '1') {
    HashPrint(werhauz->h1);
  } else {
    HashPrint(werhauz->h2);
  }
}

void WerhauzReset(struct Werhauz** werhauz) {
  HashReset(&(*werhauz)->h1, false);
  HashReset(&(*werhauz)->h2, true);
  free(*werhauz);
  *werhauz = NULL;
}
