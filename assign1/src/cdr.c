#include "cdr.h"
#include <stdio.h>
#include <stdlib.h>
#include "date.h"
#include "num.h"

struct Cdr {
  char id[11];
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
  sscanf(str, "%d%d", &hours, &mins);
  return hours * 100 + mins;
}

struct Cdr* CdrInit(const char* str) {
  struct Cdr* cdr = malloc(sizeof(*cdr));
  if (cdr == NULL) {
    exit(EXIT_FAILURE);
  }
  char caller[15];
  char callee[15];
  char date[9];
  char time[6];
  sscanf(str, "%10s;%14s;%14s;%8s;%5s;%d%d%d%d", cdr->id, caller,callee, date,
         time, &cdr->duration, &cdr->type, &cdr->tarrif, &cdr->fault);
  cdr->caller = NumInit(caller);
  cdr->callee = NumInit(callee);
  cdr->date = DateInit(date);
  cdr->time = Time(time);
  return cdr;
}

void CdrReset(struct Cdr** cdr) {
  NumReset(&(*cdr)->caller);
  NumReset(&(*cdr)->callee);
  DateReset(&(*cdr)->date);
  free(*cdr);
  *cdr = NULL;
}
