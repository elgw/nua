#ifndef __vshape_h__
#define __vshape_h__

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
   Add color directly?
   Also add face normals?
   Then add a stride for the vertex data

   TODO: face splitting subdivision

   To generate sphere:
   HEALPix https://healpix.sourceforge.io/;
   icosphere / geosphere
   cubesphere
*/

typedef struct {
    uint32_t nvert; /* Number of vertices */
    uint32_t ntri; /* Number of triangles */
    float * vert; /* Vertices [3 x nvert]*/
    uint32_t * tri; /* Inidices [3 x ntri] */
} vshape_t;

/* A vshape with stride 6, a color to each vertex */
typedef struct {
    uint32_t nvert; /* Number of vertices */
    uint32_t ntri; /* Number of triangles */
    float * vert; /* Vertices [3 x nvert]*/
    uint32_t * tri; /* Inidices [3 x ntri] */
} cvshape_t;

/* Append a constant color to a vshape */
cvshape_t * cvshape_new(uint32_t nvert, uint32_t ntri);
cvshape_t * cvshape_from_vshape(vshape_t *, float r, float g, float b);


/* Allocate a new shape with nvert vertices and ntri triangles */
vshape_t * vshape_new(uint32_t nvert, uint32_t ntri);

/* Free a shape */
void vshape_free(vshape_t *);

/* Concatenate two shapes
* A := A + B
*/
void vshape_cat(vshape_t * A, const vshape_t * B);

/* Create a flat (in z) square */
vshape_t * vshape_square(void);

/* A 3D cube */
vshape_t * vshape_cube(void);
vshape_t * vshape_icosahedron(void);

/* A 3D tube of radius r and length l with s sides.
   As s increases it will be more smooth.
 */
vshape_t * vshape_tube(float r, float l, int s);

/* Create a 3D sphere based on a cube, then refined div times by
 * dyadic subdivision */
vshape_t * vshape_sphere(int div);

void vshape_show(FILE *, vshape_t *);

/* Preliminary mesh optimization status */
float vshape_mesh_status(vshape_t * , int cache_size, int verbose);

/* Show for copy and past to matlab */
void vshape_show_matlab(FILE * fid, vshape_t * s);

/* Construct a 8 x s->nvert list of edges, E, where
E[0 ... 7] are the edges going from vertex 0
E[8 ... 15] are the edges going from vertex 1 ...
E[8*k ... 8*(k+1)-1] are sorted. Unused possible edges are marked with (uint32_t) -1
*/
uint32_t * vshape_edgelist(vshape_t * s);

vshape_t *vshape_dyadic_subdivision(vshape_t * s);

/* Scale all vertices to a certain radius, for converting cubes to spheres */
void vshape_force_radius(vshape_t *, float );

/* mesh optimization */
void vshape_optimize(vshape_t * S);
#endif
