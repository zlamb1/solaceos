#ifndef FAIL_H
#define FAIL_H

void __attribute__ ((noreturn, format (printf, 1, 2))) fail (const char *fmt,
                                                             ...);

#endif