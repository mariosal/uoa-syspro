#ifndef DATE_H_
#define DATE_H_

struct Date* DateInit(int day, int month, int year);
void DateReset(struct Date** date);
struct Date* DateParse(const char* str);
int DateYear(const struct Date* date);

#endif  // DATE_H_
