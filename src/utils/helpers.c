#include <float.h>
#include "helpers.h"

/*
  floatCheck makes sure that an increment will not overflow or underflow the variable being modified
*/
int floatCheck(double a, double b) {
  if (b == DBL_MAX) {
    return 1;
   }
  if (a > 0 && b > DBL_MAX - a) {
    /* overflow error = 1 (ERR_overflow) */
	  return 1;
  } else if (a < 0 && b < DBL_MIN - a) {
    /* underflow error = 2 (ERR_underflow) */
	  return 2;
  }
  return 0;
}

/*
  itoa converts integer to string
*/
char* itoa (int value, char *result, int base) {
  // check that the base if valid
  if (base < 2 || base > 36) { *result = '\0'; return result; }
  char* ptr = result, *ptr1 = result, tmp_char;
  int tmp_value;
  do {
    tmp_value = value;
    value /= base;
    *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
  } while ( value );
  // Apply negative sign
  if (tmp_value < 0) *ptr++ = '-';
  *ptr-- = '\0';
  while (ptr1 < ptr) {
    tmp_char = *ptr;
    *ptr--= *ptr1;
    *ptr1++ = tmp_char;
  }
  return result;
}

/*
  mod will 
*/
int mod(int a, int b) {
  int m = a % b;
  if (m < 0) {
    // m += (b < 0) ? -b : b; // avoid this form: it is UB when b == INT_MIN
    m = (b < 0) ? m - b : m + b;
  }
  return m;
}