#ifndef _CTEST_H_
#define _CTEST_H_

#include <setjmp.h>
#include <stdio.h>

#define BEGIN_TEST(title) { jmp_buf ex_buf__; \
    printf("%-70s", title); \
    if (!setjmp(ex_buf__)) {
#define END_TEST printf("%s\n", "PASSED"); ++passed;} else {printf("%s\n", "FAILED"); ++failed;} }
#define CHECK(cond) if (!(cond)) longjmp(ex_buf__, 1);
#define CHECK_NOT(cond) CHECK(!(cond))
#define LENGTH(sa) (sizeof(sa)/sizeof(sa[0]))
#define USE_CTEST unsigned int passed = 0, failed = 0; 
#define SUMMARIZE printf("\n--------------------------------------------------------------------------------\n");\
    printf("Tests total/passed/failed: %d/%d/%d\n", (passed+failed), passed, failed);

#endif
