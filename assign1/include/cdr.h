#ifndef CDR_H_
#define CDR_H_

struct Cdr* CdrInit(const char* str);
void CdrReset(struct Cdr** cdr);
long long CdrCaller(const struct Cdr* cdr);
long long CdrCallee(const struct Cdr* cdr);
int CdrDate(const struct Cdr* cdr);
int CdrTime(const struct Cdr* cdr);
void CdrPrint(const struct Cdr* cdr);
const char* CdrId(const struct Cdr* cdr);

#endif  // CDR_H_
