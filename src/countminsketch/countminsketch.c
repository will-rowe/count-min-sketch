#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>

#include "countminsketch.h"
#include "../utils/fnv.h"

/****
FUNCTION PROTOTYPES - PRIVATE
****/
static void cms__construct(CountMinSketch* self, double epsilon, double delta, uint32_t d, uint32_t g, uint64_t* matrix, double dr);
static int cms__scale(CountMinSketch* self);
cmsResult* cms__traverse(CountMinSketch* self, char* element, uint64_t increment);

/****
PUBLIC FUNCTIONS
****/
/* 
  newCMS will initialise a CountMinSketch (CMS) - this function allocates memory + constructs a CMS
  if decayRatio == 0.0, the CMS will have scaling set to false
*/
CountMinSketch* newCMS(double epsilon, double delta, double decayRatio) {
  /* try allocating the struct */
  CountMinSketch* cms = malloc(sizeof(CountMinSketch));
  if (cms == NULL) {
    fprintf(stderr, ERR_malloc);
    return NULL;
  }
  /* check the given epsilon and delta values */
  if (epsilon <= 0 || delta <= 0) {
    fprintf(stderr, ERR_epsilon_delta_0);
    return NULL;
  }
  /* calculate CMS dimensions */
  uint32_t g = ceil(2 / epsilon);
  uint32_t d = ceil(log(1 - delta) / log(0.5));
  /* allocate memory for the matrix */
  uint64_t* q = malloc(d * g * sizeof(uint64_t));
  if (q == NULL) {
    free(cms);
    fprintf(stderr, ERR_malloc);
    return NULL;
  }
   /* zero the matrix */
  for (uint32_t i = 0; i < d; i++) {
    for (uint32_t j = 0; j < g; j++) {
      q[(i * g) + j] = 0;
    }
  }
  /* construct the CMS */
  cms__construct(cms, epsilon, delta, d, g, q, decayRatio);
  return cms;
}

/*
  destroyCMS will clean a CMS from memory
  this safely assumes that CMS is either NULL or constructed
*/
void destroyCMS(CountMinSketch** cms) {
  if (*cms != NULL) {
    free((*cms)->q);
    free((*cms)->params);
    free(*cms);
    *cms = NULL;
  }
}

/*
  add2CMS will add an element to the CMS and return the estimated frequency
*/
cmsResult* add2CMS(CountMinSketch* cms, char* element, uint64_t increment) {
  /* check that the cms has been initialised */
  if (cms == NULL) {
    fprintf(stderr, ERR_add2CMS_uninit);
    return NULL;
  }
  /* check the increment is valid */
  /*double floatIncrement = (double)increment;
  if (floatIncrement <= 0.0) {
    fprintf(stderr, ERR_add2CMS_increment);
    return NULL;
  }*/
   
  /* apply scaling if flag set */
	if (cms->params->scaling == true) {
		// uniform scaling of all sketch counters
		cms__scale(cms);
	}
  /* traverse the CMS, add the element and return the new frequency estimate for the element */
	return cms__traverse(cms, element, increment);


}

/*
  estimateFreq will estimate the frequency of an element using a CMS
*/
cmsResult* estimateFreq(CountMinSketch* cms, char* element) {
  /* check that the cms has been initialised */
  if (cms == NULL) {
    fprintf(stderr, ERR_add2CMS_uninit);
    return NULL;
  }
  /* traverse the CMS and return the frequency estimate for the element */
	return cms__traverse(cms, element, 0);
}

/****
PRIVATE FUNCTIONS
****/
/*
  cms__construct will initialise a CMS
*/
static void cms__construct(CountMinSketch* cms, double epsilon, double delta, uint32_t d, uint32_t g, uint64_t* matrix, double dr) {
  cmsParams* params = malloc(sizeof(cmsParams));
  params->epsilon = epsilon;
  params->delta = delta;
  params->d = d;
  params->g = g;
  /* set the decay weight */
	if (dr > 0.0) {
		params->weightDecay = exp(-dr);
		params->scaling = true;
	} else {
    params->scaling = false;
  }
  cms->params = params;
  cms->q = matrix;
}

/*
  cms__scale will adjust each counter in q using a decay weight
*/
static int cms__scale(CountMinSketch* self) {
  for (uint32_t i = 0; i < self->params->d; i++) {
    for (uint32_t j = 0; j < self->params->g; j++) {
      self->q[(i * self->params->d) + j] = self->q[(i * self->params->d) + j] * self->params->weightDecay;
    }
  }
  // TODO: check for underflow
	return 0;
}

/*
  cms__traverse will is used to either increment or estimate the frequency of an element
*/
cmsResult* cms__traverse(CountMinSketch* self, char* element, uint64_t increment) {
  /* set the current minimum to max */
  uint64_t minimum = UINT64_MAX;
  /* get the base hash values */
  Fnv64_t hash = fnv_64_str(element, FNV1_64_INIT);
  uint32_t a = hash;
  uint32_t b = (hash >> 32);
  /* traverse each table */
  for (uint32_t i = 0; i < self->params->d; i++) {
    /* get the counter id for this table */
    uint64_t abHash = (a + ((uint64_t)b*i));
    uint32_t counterID = abHash % self->params->g;
    uint32_t counter = (i * self->params->g) + counterID;
    /* increment counter if requested */
    /* TODO: check that we wont over/underflow this counter with our increment */
    if (increment != 0) {
      self->q[counter] += increment;
    }
    /* evaluate if the current counter is the minimum */
    if (self->q[counter] < minimum) {
      minimum = self->q[counter];
    }
  }
  /* set up the result struct and return it */
  cmsResult* result = malloc(sizeof(cmsResult));
  if (result == NULL) {
    fprintf(stderr, ERR_malloc);
    return NULL;
  }
  result->minimum = minimum;    
  return result;
}
