#ifndef _TEST_H_
#define _TEST_H_

#include <stdbool.h>

#define fBOLD "\x1b[1m"
#define fTITLE "\e[1m\x1B[32m"
#define fWARN "\x1b[31m"
#define fRESET "\x1B[0m"

typedef struct Test_result {
    bool passed;
    char* comment;
} Test_result;

#endif
