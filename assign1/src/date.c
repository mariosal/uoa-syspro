#include "date.h"
#include <stdio.h>
#include <stdlib.h>

struct Date {
  int day;
  int month;
  int year;
};

struct Date* DateInit(const char* str) {
  struct Date* date = malloc(sizeof(*date));
  if (date == NULL) {
    exit(EXIT_FAILURE);
  }
  sscanf(str, "%2d%2d%4d", &date->day, &date->month, &date->year);
  return date;
}

void DateReset(struct Date** date) {
  free(*date);
  *date = NULL;
}
