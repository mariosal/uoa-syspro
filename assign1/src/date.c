#include "date.h"
#include <stdio.h>

long long Num(const char* str) {
  long long prefix;
  long long num;
  sscanf(str, "%3lld-%10lld", &prefix, &num);
  return prefix * 10000000000 + num;
}

int Time(const char* str) {
  int hours;
  int mins;
  sscanf(str, "%d:%d", &hours, &mins);
  return hours * 100 + mins;
}

int Date(const char* str) {
  int day;
  int month;
  int year;
  sscanf(str, "%2d%2d%4d", &day, &month, &year);
  return year * 10000 + month * 100 + day;
}
