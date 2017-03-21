#ifndef CDR_H_
#define CDR_H_

struct Cdr* CdrInit(const char* str);
void CdrReset(struct Cdr** cdr);
const struct Num* CdrCaller(const struct Cdr* cdr);
const struct Num* CdrCallee(const struct Cdr* cdr);
const char* CdrId(const struct Cdr* cdr);

#endif  // CDR_H_
