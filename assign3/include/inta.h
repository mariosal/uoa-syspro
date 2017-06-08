#ifndef INTA_H_
#define INTA_H_

#include <stddef.h>

struct Inta* IntaInit(void);
size_t IntaLen(const struct Inta* inta);
const size_t* IntaS(const struct Inta* inta);
void IntaReset(struct Inta** inta);
void IntaInsert(struct Inta* inta, size_t c);

#endif  // INTA_H_
