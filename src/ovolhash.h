#ifndef ovol_hash_h
#define ovol_hash_h

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* Open hash algorithm for finding neighbours withing a pre-defined
 * distance in the domain [-1, 1] x [-1,1] x [-1,1]. Using any
 * coordinates outside this range will result in undefined behaviour.
 *
 * Aiming for a load factor of about 1/3 which seems to work quite
 * well for uniformly random data. Possibly a lower might be better
 * for other data types.
 *
 * Most likely a KD-Tree is a better datastructure for non-uniform data.
 * For really big datasets, and especially when working with double precision
 * an alternative would be to store indexes or pointers to the original data...
 * all up to the application to tune :)
 * Storing data instead of pointers might be good for the cache locality.
 * Version 0.0.1
 * Erik Wernersson
 * 2023-04-27
*/

/* Choose what floating point precision to use */
typedef float ofloat;
// typedef double ofloat;

/* define to store the hash for each item.  This reduces run time by
 * about 50% at the cost of 8 bytes extra per item */
#define ovolhash_store

typedef struct {
/* Used as a marker to know when we reached the end of a bucket
 * you can use it for anything but 0 which has a special meaning
 * to the algorithm */
    ofloat r;
    ofloat x;
    ofloat y;
    ofloat z;
    #ifdef ovolhash_store
    size_t hash;
    #endif
} ovol_item_t;

typedef struct{
    /* Most elements to be inserted */
    size_t N_construct;
    size_t N_inserted;
    /* Data storage */
    ovol_item_t * items;
    size_t N_alloc;

    /* Geometry */
    ofloat rc;
    ofloat rc2; /* rc*rc */
    ofloat dx1; // bin size per dimension
    size_t nbin1; // bins per dimension
    size_t nbin; // Total number of bins pow(nbin1, 3);

    /* Elements per bucket */
    size_t bucket_size;

    /* For queries populates this  */
    ovol_item_t * result;
    /* Keep track of size  */
    size_t N_result_alloc;
} ovol_t;

/* Create a new ovl that will store N elements */
ovol_t *
ovol_new(size_t N, ofloat capture_radius);

int
ovol_insert(ovol_t *, ofloat r, ofloat x, ofloat y, ofloat z);

/* Get a list of N elements with all points withing
 *  the capture radius from (x,y,z)
 * The returned list is owned by the ovol and should not be freed.
 * It is invalid after the next call to ovol_get_neighbours
 * and is freed together with the ovol at ovol_free();
*/
ovol_item_t *
ovol_get_neighbours(const ovol_t * ovol,
                    ofloat x, ofloat y, ofloat z,
                    size_t * N);


void ovol_free(ovol_t * ovol);

#endif
