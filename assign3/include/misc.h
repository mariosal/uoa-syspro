#ifndef MISC_H_
#define MISC_H_

#include <stdbool.h>
#include <stddef.h>

#define BUFSIZE 1024

bool Equals(const char* a, const char* b);
void Error(const char* msg) __attribute__ ((noreturn));
size_t WriteAll(int fd, const void* buff, size_t count);
void WriteCmd(int fd, const char* cmd);

#endif  // MISC_H_
