#ifndef _cmm_io_h_
#define _cmm_io_h_

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libxml/parser.h>
#include <zlib.h>


/* Read and write chimera marker files (.cmm)
   see:
   https://www.cgl.ucsf.edu/chimera/docs/ContributedSoftware/volumepathtracer/volumepathtracer.html#markerfiles

Example data in data/mini.cmm

*/

typedef struct {
    uint32_t id;
    float x;
    float y;
    float z;
    float R;
    float G;
    float B;
    float radius;
} cmm_marker_t;

typedef struct {
    uint32_t id1;
    uint32_t id2;
    float R;
    float G;
    float B;
    float radius;
} cmm_link_t;

typedef struct {
    uint32_t n_markers;
    uint32_t n_links;
    cmm_marker_t * markers;
    cmm_link_t * links;
} cmm_data_t;

/* Read a .cmm file. Warning: might crash for corrupt files when
 * NDEBUG is defined.
 * Use cmm_free to free the returned object.
 */
cmm_data_t * cmm_read(char * file_name);

/* Write to a file.
 * Returns either EXIT_SUCCESS or EXIT_FAILURE
 */
int cmm_write(cmm_data_t * cmm_data, char * filename);
void cmm_print(FILE *, cmm_data_t * cmm);
void cmm_free(cmm_data_t *);

void cmm_marker_print(FILE * fid, cmm_marker_t M);
void cmm_link_print(FILE * fid, cmm_link_t L);

/* Interface used by chromflock */
int cmmwrite(char * fname,
    double * D, // 3xnD list with dot coordinates
    size_t nD, // number of beads/dots
    double radius, // bead radius
    uint32_t * P, // 2xnP list of pairs
    size_t nP, // Number of pairs
    uint8_t * L); // nD long list of labels

// Same as above, but writes a compressed file using zlib
int cmmwritez(char * fname,
              double * D,
              size_t nD,
              double radius,
              uint32_t * P,
              size_t nP,
              uint8_t * L);

#endif
