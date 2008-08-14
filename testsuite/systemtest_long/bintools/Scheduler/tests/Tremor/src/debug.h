#ifdef DEBUG_PRINT

#include <stdio.h>

#define DPRINTF(args) printf args

#define DPR(X__) \
    printf(__FILE__ ": " X__);\
    printf("\n")

#define DP DPRINT("")

#else

#define DPR(X__) 
#define DPRINT(X__)
#define DP
#define DPRINTF

#endif
