#include "cdr.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "date.h"
#include "num.h"

struct Cdr {
  char* id;
  struct Num* caller;
  struct Num* callee;
  struct Date* date;
  int time;
  int duration;
  int type;
  int tarrif;
  int fault;
};

static int Time(const char* str) {
  int hours;
  int mins;
  sscanf(str, "%d-%d", &hours, &mins);
  return hours * 100 + mins;
}

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
  cdr->caller = NumInit(caller);
  cdr->callee = NumInit(callee);
  cdr->date = DateInit(date);
  cdr->time = Time(time);
  printf("%s;%s;%s;%s;%s;%d;%d;%d;%d\n", cdr->id, caller, callee, date, time, cdr->duration, cdr->type, cdr->tarrif, cdr->fault);
  return cdr;
}

void CdrReset(struct Cdr** cdr) {
  free((*cdr)->id);
  NumReset(&(*cdr)->caller);
  NumReset(&(*cdr)->callee);
  DateReset(&(*cdr)->date);
  free(*cdr);
  *cdr = NULL;
}

const struct Num* CdrCaller(const struct Cdr* cdr) {
  return cdr->caller;
}

const struct Num* CdrCallee(const struct Cdr* cdr) {
  return cdr->callee;
}

const char* CdrId(const struct Cdr* cdr) {
  return cdr->id;
}
