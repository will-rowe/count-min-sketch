#ifndef HELPERS_HEADER
#define HELPERS_HEADER

#include <string.h>

#ifndef min
#define min(a,b) ((a < b)? a : b)
#endif

/****
ERRORS
****/
#define ERR_overflow "cannot add element: increment will cause overflow of float\n"
#define ERR_underflow "cannot add element: increment will cause underflow of float\n"

/****
TYPES
****/
typedef enum boolean { false, true } bool;

/****
FUNCTION PROTOTYPES
****/
extern int floatCheck(double a, double b);
extern char* itoa(int value, char *result, int base);
extern int mod (int a, int b);

#endif