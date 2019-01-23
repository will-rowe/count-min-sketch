#ifndef TEST_CMS
#define TEST_CMS

#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include "minunit.h"
#include "../src/countminsketch/countminsketch.h"
#include "../src/countminsketch/countminsketch.c"
#include "../src/utils/helpers.h"
#include "../src/utils/helpers.c"
#include "../src/utils/fnv.h"
#include "../src/utils/fnv.c"

#define ERR_cmsInit1 "CMS shouldn't accept epsilon or delta == 0.0"
#define ERR_cmsInit2 "CMS failed to initialise"
#define ERR_cmsInit3 "failed to set scaling flag"
#define ERR_cmsDest1 "could not destroy CMS"
#define ERR_cmsScale1 "counter did not undergo correct scaling"
#define ERR_cmsAdd1 "cmsAdd failed to catch un-initialised CMS"
#define ERR_cmsAdd2 "failed to add element to CMS"
#define ERR_cmsAdd3 "incorrect minimum returned from add2CMS function"
#define ERR_cmsAdd4 "CMS failed to catch counter overflow"
#define ERR_estFreq1 "incorrect frequency estimate"

int tests_run = 0;

/*
  test the CMS initialisation
*/
static char* test_cmsInit() {
  double epsilon = 0.0, delta = 0.0, noDR = 0.0;
  // make sure epsilon and delta != 0.0
  CountMinSketch* cms = newCMS(epsilon, delta, DECAY_RATIO);
  if (cms != NULL) {
    return ERR_cmsInit1;
  }
  // try CMS init without scaling set
  cms = newCMS(EPSILON, DELTA, noDR);
  if (cms == NULL) {
    return ERR_cmsInit2;
  }
  if (cms->params->scaling != false) {
    return ERR_cmsInit3;
  }
  // try CMS init with scaling set
  cms = newCMS(EPSILON, DELTA, DECAY_RATIO);
  if (cms == 0) {
    return ERR_cmsInit2;
  }
  if (cms->params->scaling != true) {
    return ERR_cmsInit3;
  }
  return 0;
}

/*
  test the CMS destruction
*/
static char* test_cmsDestroy() {
  CountMinSketch* cms = newCMS(EPSILON, DELTA, DECAY_RATIO);
  if (cms == NULL) {
    return ERR_cmsInit2;
  }
  destroyCMS(&cms);
  if (cms != NULL) {
    return ERR_cmsDest1;
  }
  return 0;
}

/*
  test the CMS scaling
*/
/*
static char* test_cmsScale() {
  CountMinSketch* cms = newCMS(EPSILON, DELTA, DECAY_RATIO);
  if (cms == NULL) {
    return ERR_cmsInit2;
  }
  cms->q[0] = 100;
  cms__scale(cms);
  if (cms->q[0] != (100 * cms->params->weightDecay)) {
    return ERR_cmsScale1;
  }
  destroyCMS(&cms);
  return 0;
}
*/

/*
  test adding an element to the CMS
*/
static char* test_cmsAdd() {
  CountMinSketch* cms;
  void* gpPointer;
  uint64_t increment = 10;

  char stringElement[] = "bin-1";
  int intElement = 1;

  /* check that you can't add to an un-initialised cms */
  gpPointer = &intElement;
  cmsResult* result = add2CMS(cms, gpPointer, increment);
  if (result != NULL) {
    return ERR_cmsAdd1;
  }
  /* now add to an initialised cms and check that the counter has increased */
  cms = newCMS(EPSILON, DELTA, 0.0);
  if (cms == NULL) {
    return ERR_cmsInit2;
  }
  result = add2CMS(cms, gpPointer, increment);
  if (result == NULL) {
    return ERR_cmsAdd2;
  }
  if (result->minimum != increment) {
    return ERR_cmsAdd3;
  }
  free(result);
  /* check that you can add different element types to the CMS */
  gpPointer = &stringElement;
  result = add2CMS(cms, gpPointer, increment);
  if (result == NULL) {
    return ERR_cmsAdd2;
  }
  if (result->minimum != increment) {
    return ERR_cmsAdd3;
  }
  free(result);
  /* check that overflows are caught */
  /**
  uint32_t g = cms->params->g;
  for (uint32_t i = 0; i < g; i++) {
    cms->q[i] = UINT64_MAX;
  }
  result = add2CMS(cms, gpPointer, (uint64_t)UINT64_MAX);
  if (result != NULL) {
    return ERR_cmsAdd4;
  }
  */
  destroyCMS(&cms);
  return 0;
}

/*
  test estimating frequency an element
*/
static char* test_cmsEstimateFreq() {
  CountMinSketch* cms = newCMS(EPSILON, DELTA, 0.0);
  if (cms == NULL) {
    return ERR_cmsInit2;
  }
  void* gpPointer;
  uint64_t increment1 = 10;
  uint64_t increment2 = 999;
  char stringElement1[] = "b1";
  char stringElement2[] = "b2";
  gpPointer = &stringElement1;
  add2CMS(cms, gpPointer, increment1);
  gpPointer = &stringElement2;
  add2CMS(cms, gpPointer, increment2);

  cmsResult* result = estimateFreq(cms, gpPointer);
  if (result->minimum != (double)increment2) {
    return ERR_estFreq1;
  }
  free(result);
  gpPointer = &stringElement1;
  result = estimateFreq(cms, gpPointer);
  if (result->minimum != (double)increment1) {
    return ERR_estFreq1;
  }
  return 0;
}


/*
  test accuracy of CMS (thanks to: https://github.com/shenwei356/countminsketch)
*/
static char* test_cmsAccuracy() {
  printf("accuracy of CMS:\n");
  CountMinSketch* cms = newCMS(EPSILON, DELTA, 0.0);
  if (cms == NULL) {
    return ERR_cmsInit2;
  }
  /* based on https://github.com/jehiah/countmin/blob/master/sketch_test.go */
	uint32_t iterations = 5500;
	uint32_t diverged = 0;
	for (uint32_t i = 1; i < iterations; i++) {
    uint64_t v = mod(i, 50);
    char str1[ENOUGH];
    sprintf(str1, "%d", i);
    cmsResult* result = add2CMS(cms, str1, v);
    if (result == NULL) {
      return ERR_cmsAdd2;
    }
		if (result->minimum > v) {
			diverged++;
		}
    free(result);
	}
	uint32_t miss = 0;
	for (uint32_t i = 1; i < iterations; i++) {
    uint64_t v = mod(i, 50);
    char str2[ENOUGH];
    sprintf(str2, "%d", i);
    cmsResult* result = estimateFreq(cms, str2);
		if (v != result->minimum) {
			printf(" - miss recorded (real: %llu, estimate: %llu)\n", v, (uint64_t)result->minimum);
			miss++;
		}
	}
	printf(" - missed %u of %u (%u diverged during adds)\n", miss, iterations, diverged);
  return 0;
}


/*
  helper function to run all the tests
*/
static char* all_tests() {
  mu_run_test(test_cmsInit);
  mu_run_test(test_cmsDestroy);
  /*mu_run_test(test_cmsScale);*/
  mu_run_test(test_cmsAdd);
  mu_run_test(test_cmsEstimateFreq);
  mu_run_test(test_cmsAccuracy);
  return 0;
}

/*
  entrypoint
*/
int main(int argc, char **argv) {
  fprintf(stderr, "\ntesting unit...\n\n");
  char *result = all_tests();
  if (result != 0) {
    fprintf(stderr, "\ntest function %d failed:\n", tests_run);
    fprintf(stderr, "%s\n", result);
  }
  return result != 0;
}

#endif