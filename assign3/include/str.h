#ifndef STR_H_
#define STR_H_

#include <stddef.h>

struct Str* StrInit(void);
size_t StrLen(const struct Str* str);
const char* StrS(const struct Str* str);
void StrReset(struct Str** str);
void StrZero(struct Str* str);
void StrInsert(struct Str* str, char c);
void StrRead(struct Str* str, int fd);
void StrReadNl(struct Str* str, int fd);
void StrReadAllNl(struct Str* str, int fd);

#endif  // STR_H_
