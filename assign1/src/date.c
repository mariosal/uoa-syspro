#include "date.h"
#include <stdio.h>
#include <stdlib.h>

struct Date {
  int day;
  int month;
  int year;
};

struct Date* DateInit(int day, int month, int year) {
  struct Date* date = malloc(sizeof(*date));
  if (date == NULL) {
    exit(EXIT_FAILURE);
  }
  date->day = day;
  date->month = month;
  date->year = year;
  return date;
}

void DateReset(struct Date** date) {
  free(*date);
  *date = NULL;
}

struct Date* DateParse(const char* str) {
  struct Date* date = malloc(sizeof(*date));
  if (date == NULL) {
    exit(EXIT_FAILURE);
  }
  sscanf(str, "%2d%2d%4d", &date->day, &date->month, &date->year);
  return date;
}

int DateYear(const struct Date* date) {
  if (date == NULL) {
    return -1;
  }
  return date->year;
}
