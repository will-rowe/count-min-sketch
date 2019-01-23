# CountMinSketch

This implementation of the [Count Min Sketch](http://www.cse.unsw.edu.au/~cs9314/07s1/lectures/Lin_CS9314_References/cm-latin.pdf) is my first stab at programming in C...

It just works as a library at the moment.

## Background

>In computing, the Count Min Sketch (CMS) is a probabilistic data structure that serves as a frequency table of events in a stream of data. It uses hash functions to map events to frequencies, but unlike a hash table uses only sub-linear space, at the expense of overcounting some events due to collisions.

>CMSs are essentially the same data structure as the counting Bloom filters introduced in 1998 by Fan et al. However, they are used differently and therefore sized differently: a CMS typically has a sublinear number of cells, related to the desired approximation quality of the sketch, while a counting Bloom filter is more typically sized to match the number of elements in the set.

## Unit tests

Just run:

```bash
make clean && make test
```

## Usage

```c
/* create CMS */
CountMinSketch* cms = newCMS(EPSILON, DELTA, 0.0);
if (cms == NULL) {
    /* handle error */
}

/* add a string to CMS and return updated frequency estimate */
int frequency = 42;
cmsResult* estimate = add2CMS(cms, string, frequency);
if (estimate == NULL) {
  /* handle error */
}
printf("frequency estimate: %llu\n", result->minimum);
```
