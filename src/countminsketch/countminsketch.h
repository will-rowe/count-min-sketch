#ifndef CMS_HEADER
#define CMS_HEADER

#include <stdint.h>
#include "../utils/helpers.h"




#define ENOUGH ((CHAR_BIT * sizeof(uint32_t) - 1) / 3 + 2)




/****
DEFAULTS
****/
#define EPSILON 0.0001
#define DELTA 0.9999
#define DECAY_RATIO 0.002

/****
ERRORS
****/
#define ERR_malloc "cannot allocate memory\n"
#define ERR_epsilon_delta_0 "cannot create CMS: epsilon and delta must be > 0.0\n"
#define ERR_destroy "cannot destroy non-initialised CMS\n"
#define ERR_add2CMS_uninit "cannot add element: the CMS is not initialised\n"
#define ERR_add2CMS_increment "cannot add element: increment must be > 0\n"

/****
TYPES
****/
/*
  cmsParams:
  relative accuracy is within a factor of epsilon with probability delta - both of these parameters affect the space and time complexity.
  decay weight is included for uniform scaling of counters
*/
typedef struct {
  uint32_t d;  /* matrix depth (number of hash tables) */
  uint32_t g;  /* matrix width (number of counters per table) */
  double epsilon;  /* relative-accuracy factor */
  double delta;  /* relative-accuracy probability */ 
  bool scaling;  /* if true, uniform scaling will be applied to the counters using the decay weight */
  double weightDecay;  /* the decay weight for scaling */
} cmsParams;

/*
  CountMinSketch
*/
typedef struct {
  cmsParams* params;
  uint64_t* q;  /* the CMS matrix of counters (d x g) */
} CountMinSketch;

/*
  cmsResult is a struct to....
*/
typedef struct {
  double minimum;
} cmsResult;


/****
FUNCTION PROTOTYPES
****/
/* initialise a CMS */
CountMinSketch* newCMS(double epsilon, double delta, double decayRatio);
/* clean up memory used by a CMS */
void destroyCMS(CountMinSketch** cms);
/* add an element to the CMS */
cmsResult* add2CMS(CountMinSketch* cms, char* element, uint64_t increment);

#endif