#include "cdr.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "date.h"

struct Cdr {
  char* id;
  long long caller;
  long long callee;
  int date;
  int time;
  int duration;
  int type;
  int tarrif;
  int fault;
};

struct Cdr* CdrInit(const char* str) {
  struct Cdr* cdr = malloc(sizeof(*cdr));
  if (cdr == NULL) {
    exit(EXIT_FAILURE);
  }

  size_t len = 0;
  size_t cap = 1;
  cdr->id = malloc(sizeof(*cdr->id) * cap);
  cdr->id[len] = '\0';
  while (str[len] != ';') {
    if (len == cap - 1) {
      cap *= 2;
      cdr->id = realloc(cdr->id, sizeof(*cdr->id) * cap);
    }
    cdr->id[len] = str[len];
    cdr->id[len + 1] = '\0';
    ++len;
  }

  char caller[15];
  char callee[15];
  char date[9];
  char time[6];
  sscanf(str + len + 1, "%14s;%14s;%8s;%5s;%d;%d;%d;%d", caller, callee, date,
         time, &cdr->duration, &cdr->type, &cdr->tarrif, &cdr->fault);
  cdr->caller = Num(caller);
  cdr->callee = Num(callee);
  cdr->date = Date(date);
  cdr->time = Time(time);
  CdrPrint(cdr);
  return cdr;
}

void CdrReset(struct Cdr** cdr) {
  free((*cdr)->id);
  free(*cdr);
  *cdr = NULL;
}

long long CdrCaller(const struct Cdr* cdr) {
  return cdr->caller;
}

long long CdrCallee(const struct Cdr* cdr) {
  return cdr->callee;
}

int CdrDate(const struct Cdr* cdr) {
  return cdr->date;
}

int CdrTime(const struct Cdr* cdr) {
  return cdr->time;
}

void CdrPrint(const struct Cdr* cdr) {
  printf("%s;%lld;%lld;%d;%d;%d;%d;%d;%d\n", cdr->id, cdr->caller, cdr->callee, cdr->date, cdr->time, cdr->duration, cdr->type, cdr->tarrif, cdr->fault);
}

const char* CdrId(const struct Cdr* cdr) {
  return cdr->id;
}
