#ifndef __nua_util_h__
#define __nua_util_h__

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

size_t get_peakMemoryKB(void);
float timespec_diff(struct timespec* end, struct timespec * start);

/* Handle char ** arrays */
const char ** vec_char_add(const char ** vec, uint32_t *n, char * new);
void vec_char_free(char ** vec, int n);

float float_max(float a, float b);
float float_min(float a, float b);
void showMat4(const char * name, float * M);

/* Uniform random in [-.5, .5] */
float midrand(void);

/* Uniform random in [0, 1] */
float urand(void);

/* 3-Vector L2 norm */
float f3_norm(float * p);
/* scale a 3D float vector by k */
void f3_mul_scalar(float * p, float k);
/* A 3D point in a sphere */
void sphererand(float * p);
/* Euclidean distance, || a - b||  */
float f3_dist_f3(float * a, float * b);
#endif
