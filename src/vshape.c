#include "vshape.h"

/* Forward declarations for the non-exported functions */

/* Vector norm */
static float vnorm(float * v);

/* returns norm(v) */
extern float vec3_norm(const float * v);

/* v = v*c */
extern void vec3_scalar_mult(float * v, const float c);

/* Cross product between a and b */
static void cross(float * cx, float * a, float *b);

/* Area of triangle # tri */
static float tri_area(vshape_t * s, uint32_t tri);

/* y = a - b */
static void vminus(float * y, float * a, float * b);


static float vnorm(float * v)
{
    return sqrt(
        pow(v[0], 2) + pow(v[1], 2) + pow(v[2], 2));
}


static void cross(float * cx, float * a, float *b)
{
    cx[0] = a[1]*b[2] - a[2]*b[1];
    cx[1] = a[2]*b[0] - a[0]*b[2];
    cx[2] = a[0]*b[1] - a[1]*b[0];
    return;
}


static void vminus(float * y, float * a, float * b)
{
    for(int kk = 0; kk < 3; kk++)
    {
        y[kk] = a[kk] - b[kk];
    }
    return;
}


static float tri_area(vshape_t * s, uint32_t tri)
{
    /* v1 = p1 - p0
     * v2 = p2 - p0
     */
    float * p0 = s->vert + 3*s->tri[3*tri + 0];
    float * p1 = s->vert + 3*s->tri[3*tri + 1];
    float * p2 = s->vert + 3*s->tri[3*tri + 2];

    float v1[3];
    vminus(v1, p1, p0);
    float v2[3];
    vminus(v2, p2, p0);
    float cx[3];
    cross(cx, v1, v2);
    return vnorm(cx)/2;
}


void vshape_free(vshape_t * s)
{
    if(s == NULL)
    {
        return;
    }
    if(s->vert != NULL)
    {
        free(s->vert);
    }

    if(s->tri != NULL)
    {
        free(s->tri);
    }
    free(s);
    return;
}


vshape_t * vshape_new(uint32_t nvert, uint32_t ntri)
{
    vshape_t * v = malloc(sizeof(vshape_t));
    v->nvert = nvert;
    v->ntri = ntri;

    v->vert = NULL;
    v->tri = NULL;
    if(nvert > 0)
    {
        v->vert = malloc(3*nvert*sizeof(float));
    }
    if(ntri > 0)
    {
        v->tri = malloc(3*ntri*sizeof(float));
    }
    return v;
}

cvshape_t * cvshape_new(uint32_t nvert, uint32_t ntri)
{
    cvshape_t * v = malloc(sizeof(cvshape_t));
    v->nvert = nvert;
    v->ntri = ntri;

    v->vert = NULL;
    v->tri = NULL;
    if(nvert > 0)
    {
        v->vert = malloc(6*nvert*sizeof(float));
    }
    if(ntri > 0)
    {
        v->tri = malloc(3*ntri*sizeof(float));
    }
    return v;
}

vshape_t * vshape_tube(float r, float l, int sides)
{
    if(sides < 3)
    {
        assert(0);
        return NULL;
    }
    vshape_t * S = vshape_new(2*sides, 2*sides);


    /* A tube going in the Z-direction of length l.
       radius r.
       vertices where Z>0 : 0, 2, 4, ...
       vertices where Z<0 : 1, 3, 5, ... */

/* Triangles */
    for(int ss = 0; ss < sides; ss++)
    {
        uint32_t * T = S->tri + 6*ss;
        uint32_t a = 2*ss; uint32_t b = 2*(ss+1);
        uint32_t c = 2*ss+1; uint32_t d = 2*ss+3;
        if(ss + 1 == sides)
        {
            b = 0;
            d = 1;
        }

        T[0] = a; T[1] = b; T[2] = c;
        T[3] = b; T[4] = d; T[5] = c;
    }

    /* Vertices */
    for(int ss = 0; ss < sides; ss++)
    {
        float * V = S->vert + 6*ss;
        float theta = (float) ss/(float) sides * 2.0*M_PI;
        V[0] = r*cos(theta); V[1] = r*sin(theta); V[2] = l;
        V[3] = V[0]; V[4] = V[1]; V[5] = 0;
    }
    return S;
}

vshape_t * vshape_square(void)
{
    vshape_t * s = vshape_new(4, 2);
    float * v = s->vert;
    float v0[3] = {-1, -1, 0};

    for(uint32_t kk = 0; kk < 4; kk++)
    {
        //printf("%f %f\n", v0[0], v0[1]);
        memcpy(v + 3*kk, v0, 3*sizeof(float));
        float v1 = -v0[1];
        float v2 = v0[0];
        v0[0] = v1;
        v0[1] = v2;

    }

    uint32_t * i = s->tri;
    i[0] = 0; i[1] = 1; i[2] = 2;
    i[3] = 2; i[4] = 3; i[5] = 0;
    return s;
}


vshape_t * vshape_cube(void)
{
    vshape_t * s = vshape_new(4*2, 2*6);

    { /* Vertices */
        float * v = s->vert;
        v[0] = -1; v[1] = -1; v[2] =  1; v += 3;
        v[0] =  1; v[1] = -1; v[2] =  1; v += 3;
        v[0] =  1; v[1] =  1; v[2] =  1; v += 3;
        v[0] = -1; v[1] =  1; v[2] =  1; v += 3;
        v[0] = -1; v[1] = -1; v[2] = -1; v += 3;
        v[0] =  1; v[1] = -1; v[2] = -1; v += 3;
        v[0] =  1; v[1] =  1; v[2] = -1; v += 3;
        v[0] = -1; v[1] =  1; v[2] = -1;
    }

    for(int kk = 0; kk<8*3 ; kk++)
    {
        s->vert[kk] *= 0.5;
    }

    { /* Trianges */
        uint32_t * t = s->tri;
        t[0] = 0; t[1] = 1; t[2] = 2; t += 3; // Top 1
        t[0] = 0; t[1] = 2; t[2] = 3; t += 3; // Top 2
        t[0] = 3; t[1] = 2; t[2] = 6; t += 3; // Front 1
        t[0] = 6; t[1] = 7; t[2] = 3; t += 3; // Front 2
        t[0] = 0; t[1] = 3; t[2] = 7; t += 3; // Left 1
        t[0] = 7; t[1] = 4; t[2] = 0; t += 3; // Left 2
        t[0] = 2; t[1] = 1; t[2] = 5; t += 3; // Right 1
        t[0] = 5; t[1] = 6; t[2] = 2; t += 3; // Right 2
        t[0] = 1; t[1] = 0; t[2] = 4; t += 3; // Back 1
        t[0] = 1; t[1] = 4; t[2] = 5; t += 3; // Back 2
        t[0] = 7; t[1] = 6; t[2] = 5; t += 3; // Bottom 1
        t[0] = 7; t[1] = 5; t[2] = 4; t += 3; // Bottom 2
    }
    return s;
}

vshape_t * vshape_sphere(int div)
{
    assert(div >= 0);
    //vshape_t * sphere = vshape_cube();
    vshape_t * sphere = vshape_icosahedron();

    for(int kk = 0; kk < div; kk++)
    {
        vshape_t * sphere2 = vshape_dyadic_subdivision(sphere);
        vshape_free(sphere);
        sphere = sphere2;
    }
    vshape_force_radius(sphere, 1);
    return sphere;
}

vshape_t * vshape_icosahedron(void)
{
    vshape_t * S = vshape_new(12, 20);
    { /* Trianges */
        uint32_t * t = S->tri;
        t[0] = 0; t[1] = 2; t[2] = 1; t += 3; // Top 1
        t[0] = 0; t[1] = 3; t[2] = 2; t += 3; // Top 2
        t[0] = 0; t[1] = 4; t[2] = 3; t += 3; // Top 3
        t[0] = 0; t[1] = 5; t[2] = 4; t += 3; // Top 4
        t[0] = 0; t[1] = 1; t[2] = 5; t += 3; // Top 5

        t[0] = 1; t[1] = 2; t[2] =  6; t += 3; // Upper Mid 1
        t[0] = 2; t[1] = 3; t[2] =  7; t += 3; // Upper Mid 2
        t[0] = 3; t[1] = 4; t[2] =  8; t += 3; // Upper Mid 3
        t[0] = 4; t[1] = 5; t[2] =  9; t += 3; // Upper Mid 4
        t[0] = 5; t[1] = 1; t[2] = 10; t += 3; // Upper Mid 5

        t[0] =  6; t[1] = 2; t[2] =  7; t += 3; // Lower Mid 1
        t[0] =  7; t[1] = 3; t[2] =  8; t += 3; // Lower Mid 2
        t[0] =  8; t[1] = 4; t[2] =  9; t += 3; // Lower Mid 3
        t[0] =  9; t[1] = 5; t[2] = 10; t += 3; // Lower Mid 4
        t[0] = 10; t[1] = 1; t[2] =  6; t += 3; // Lower Mid 5

        t[0] =  6; t[1] =  7; t[2] = 11; t += 3; // Bottom 1
        t[0] =  7; t[1] =  8; t[2] = 11; t += 3; // Bottom 2
        t[0] =  8; t[1] =  9; t[2] = 11; t += 3; // Bottom 3
        t[0] =  9; t[1] = 10; t[2] = 11; t += 3; // Bottom 4
        t[0] = 10; t[1] =  6; t[2] = 11;         // Bottom 5
    }
    { /* Vertices */
        float * v = S->vert;
        v[0] = 0; v[1] = 0; v[2] =  1; v += 3; // Top
        float k1 = cos(atan(0.5));
        float k2 = sin(atan(0.5));
        float a = 2.0*M_PI/5.0;
        for(float kk = 0; kk<5; kk++)
        {
            v[0] = k1*cos(a*kk);
            v[1] = k1*sin(a*kk);
            v[2] = k2;
            v += 3;
        }
        for(float kk = 0; kk<5; kk++)
        {
            v[0] = k1*cos(a*kk + 0.5*a);
            v[1] = k1*sin(a*kk + 0.5*a);
            v[2] = -k2;
            v += 3;
        }
        v[0] = 0; v[1] = 0; v[2] =  -1; // Bottom
    }
    return S;
}

void vshape_show_matlab(FILE * fid, vshape_t * s)
{
    fprintf(fid, "V=[");
    for(uint32_t kk = 0; kk<s->nvert; kk++)
    {
        fprintf(fid, "[");
        for(uint32_t ll = 0; ll < 3; ll++)
        {
            fprintf(fid, " % .2f",
                    s->vert[3*kk + ll]);
        }

        if(kk+1 == s->nvert)
        {
            fprintf(fid, "]]\n");
        } else {
            fprintf(fid, "]\n");
        }
    }

    fprintf(fid, "F=[");
    for(uint32_t kk = 0; kk < s->ntri; kk++)
    {
        fprintf(fid, "[");
        for(uint32_t ll = 0; ll<3; ll++)
        {
            fprintf(fid, " %u", s->tri[3*kk + ll]);
        }
        if(kk+1 == s->ntri)
        {
            fprintf(fid, "]]\n");
        } else {
            fprintf(fid, "]\n");
        }
    }
    return;
}

void vshape_show(FILE * fid, vshape_t * s)
{
    fprintf(fid, "vshape with %u vertices and %u triangles\n",
            s->nvert, s->ntri);
    for(uint32_t kk = 0; kk<s->nvert; kk++)
    {
        fprintf(fid, "V%d", kk);
        for(uint32_t ll = 0; ll < 3; ll++)
        {
            fprintf(fid, " % .1f",
                    s->vert[3*kk + ll]);
        }
        fprintf(fid, "\n");
    }


    float amin = tri_area(s, 0);
    float amax = amin;

    for(uint32_t kk = 0; kk < s->ntri; kk++)
    {
        fprintf(fid, "T%d", kk);
        float a = tri_area(s, kk);
        a > amax ? amax = a : 0;
        a < amin ? amin = a : 0;
        for(uint32_t ll = 0; ll<3; ll++)
        {
            fprintf(fid, " %u", s->tri[3*kk + ll]);
        }
        fprintf(fid, " (a = %.2f) \n", a);
    }

    printf("Largest signed triangle area: %f\n", amax);
    printf("Smallest signed triangle area: %f\n", amin);
    return;
}

static void edgelist_connect(uint32_t * E, uint32_t a, uint32_t b)
{
    if(a > b)
    {
        uint32_t t = a;
        a = b;
        b = t;
    }
    uint32_t * e = E+8*a;

    /* insert b in e, if not already there */
    while(*e != (uint32_t) -1)
    {
        if(e[0] == b)
        {
            return;
        }
        e++;
    }
    /* If not in the list, insert */
    e[0] = b;
}

void edgelist_show(uint32_t * E, size_t nvert)
{
    size_t nedge = 0;
    for(uint32_t kk = 0; kk < nvert; kk++)
    {
        uint32_t * e = E + 8*kk;
        for(uint32_t ll = 0; ll<8; ll++)
        {
            if(e[ll] != (uint32_t) -1)
            {
                printf("%zu (%u -> %u)\n", nedge++, kk, e[ll]);
            }
        }
    }
    printf("%zu edges\n", nedge);
}

uint32_t * vshape_edgelist(vshape_t * s)
{
    int max_edges_per_vertex = 8;
    size_t nE = max_edges_per_vertex * s->nvert;

    /* Initialize so that -1 means no edge present */
    uint32_t * E = malloc(nE*sizeof(uint32_t));
    for(size_t kk = 0; kk < nE; kk++)
    {
        E[kk] = -1;
    }

    /* Loop over the triangles and add their edges to the list */
    for(uint32_t tt = 0; tt < s->ntri; tt++)
    {
        uint32_t * T = s->tri + 3*tt;
        edgelist_connect(E, T[0], T[1]);
        edgelist_connect(E, T[1], T[2]);
        edgelist_connect(E, T[2], T[0]);
    }
    //    edgelist_show(E, s->nvert);
    return E;
}

uint32_t find_vertex(uint32_t * E, uint32_t * N, uint32_t a, uint32_t b)
{
    if(a > b)
    {
        uint32_t t = a;
        a = b;
        b = t;
    }
    assert(a < b);
    uint32_t * e = E + 8*a;
    uint32_t * n = N + 8*a;

    for(int kk =0 ; kk<8; kk++)
    {
        if(e[kk] == b)
        {
            return n[kk];
        }
    }
    assert(0);
    return -1;
}

vshape_t *  vshape_dyadic_subdivision(vshape_t * s)
{

    uint32_t * E = vshape_edgelist(s);
    uint32_t nedge = 0;
    for(size_t kk = 0; kk < 8*s->nvert; kk++)
    {
        nedge += E[kk] < (uint32_t) -1;
    }
    //printf("%u edges\n", nedge);

    /* Enumerate all the new vertices starting at s->nvert */
    uint32_t * N = malloc(8*s->nvert*sizeof(uint32_t));
    uint32_t n = s->nvert;
    for(uint32_t kk = 0; kk < 8*s->nvert; kk++)
    {
        if(E[kk] < (uint32_t) -1)
        {
            N[kk] = n++;
        } else {
            N[kk] = (uint32_t) -1;
        }
    }

    /* Create list of new triangles and vertices*/
    vshape_t * s2 = vshape_new((s->nvert+nedge), s->ntri*4);
    uint32_t * T2 = s2->tri;
    for(uint32_t tt = 0; tt < s->ntri; tt++)
    {
        uint32_t * T = s->tri + 3*tt;

        T2[0] = T[0];
        T2[1] = find_vertex(E, N, T[0], T[1]);
        T2[2] = find_vertex(E, N, T[2], T[0]);
        T2 += 3;
        T2[0] = find_vertex(E, N, T[0], T[1]);
        T2[1] = T[1];
        T2[2] = find_vertex(E, N, T[2], T[1]);
        T2 += 3;
        T2[0] = find_vertex(E, N, T[2], T[0]);
        T2[1] = find_vertex(E, N, T[0], T[1]);
        T2[2] = find_vertex(E, N, T[2], T[1]);
        T2 += 3;
        T2[0] = find_vertex(E, N, T[2], T[0]);
        T2[1] = find_vertex(E, N, T[2], T[1]);
        T2[2] = T[2];
        T2 += 3;
    }

    /* Copy the original vertices */
    memcpy(s2->vert, s->vert, 3*s->nvert*sizeof(uint32_t));


    for(uint32_t kk = 0; kk< s->nvert; kk++)
    {
        for(uint32_t ll = 0; ll< 8; ll++)
        {
            if(E[8*kk + ll] != (uint32_t) -1)
            {
                /* There is a new vertex in between vertex kk and vertex E[8*kk+ll] */
                uint32_t v1 = kk;
                uint32_t v2 = E[8*kk+ll];
                uint32_t n = N[8*kk+ll];
                for(uint32_t ii = 0; ii<3; ii++)
                {
                    s2->vert[3*n + ii] = (s->vert[3*v1 + ii] + s->vert[3*v2 + ii])/2.0;
                }
            }
        }
    }

    free(E);
    free(N);
    return s2;
}

extern float vec3_norm(const float * v)
{
    return sqrt( pow(v[0], 2) +
                 pow(v[1], 2) +
                 pow(v[2], 2));
}

extern void vec3_scalar_mult(float * v, const float c)
{
    for(uint32_t kk = 0; kk<3; kk++)
    {
        v[kk] *= c;
    }
    return;
}
void vshape_force_radius(vshape_t * s, float r)
{
    for(uint32_t kk = 0; kk < s->nvert; kk++)
    {
        float * v = s->vert+3*kk;
        float n = vec3_norm(v);
        vec3_scalar_mult(v, r/n);
    }
    return;
}

cvshape_t * cvshape_from_vshape(vshape_t * s, float r, float g, float b)
{
    cvshape_t * c = cvshape_new(s->nvert, s->ntri);
    memcpy(c->tri, s->tri, 3*sizeof(float)*s->ntri);
    for(uint32_t kk = 0; kk < s->nvert; kk++)
    {
        float * v1 = s->vert + 3*kk;
        float * v2 = c->vert + 6*kk;
        v2[0] = v1[0];
        v2[1] = v1[1];
        v2[2] = v1[2];
        v2[3] = r;
        v2[4] = g;
        v2[5] = b;
    }
    return c;
}

typedef struct{
    uint32_t * cache;
    uint32_t cache_size;
    uint32_t ncached;
} vertex_cache_t;

vertex_cache_t * vertex_cache_new(uint32_t cache_size)
{
    /* A First-In-First-Out - FIFO queue
     * Inputs are added at the bottom
     * Elements are removed from the top.
     */
    vertex_cache_t * C = malloc(sizeof(vertex_cache_t));
    C->cache_size = cache_size;
    // an extra element so that we can always add
    C->cache = malloc((C->cache_size +1)*sizeof(uint32_t));
    C->ncached = 0;
    return C;
}

void vertex_cache_remove(vertex_cache_t * C, uint32_t v)
{

    uint32_t writepos = 0;
    for(uint32_t readpos = 0; readpos < C->ncached; readpos++)
    {
        if(C->cache[readpos] != v)
        {
            C->cache[writepos++] = C->cache[readpos];
        }
    }
    C->ncached = writepos;
}

void vertex_cache_show(vertex_cache_t * C)
{
    for(uint32_t kk = 0; kk<C->ncached; kk++)
    {
        if(kk>0) printf(" ");
        printf("%u", C->cache[kk]);
    }
    printf("\n");
}
void vertex_cache_add(vertex_cache_t * C,
                      uint32_t v,
                      uint32_t * hit, uint32_t * miss)
{
    const int verbose = 0;
    if(verbose)
    {
        printf("Adding %u to \n", v);
        vertex_cache_show(C);
    }
    uint32_t size = C->ncached;
    vertex_cache_remove(C, v);
    if(size == C->ncached)
    {
        miss[0]++;
        if(verbose)
        {
            printf("miss\n");
        }
    } else {
        hit[0]++;
        if(verbose)
        {
            printf("hit\n");
        }
    }
    /* Add at the bottom */
    C->cache[C->ncached++] = v;

    /* If full, remove first element */
    if(C->ncached > C->cache_size)
    {
        for(uint32_t writepos = 0; writepos < C->cache_size ; writepos++)
        {
            C->cache[writepos] = C->cache[writepos+1];
        }
        C->ncached--;
    }

}

void vertex_cache_free(vertex_cache_t * C)
{
    free(C->cache);
    free(C);
    return;
}

float vshape_mesh_status(vshape_t * s, int cache_size, int verbose)
{
    /* Number of cached vertices */
    //int cache_size = 20;
    vertex_cache_t * cache = vertex_cache_new(cache_size);

    uint32_t cache_hit = 0;
    uint32_t cache_miss = 0;
    for(uint32_t tt = 0; tt<s->ntri; tt++)
    {
        for(uint32_t vv = 0 ; vv<3; vv++)
        {
            uint32_t V = s->tri[tt*3 + vv];
            vertex_cache_add(cache, V, &cache_hit, &cache_miss);
        }
    }
    if(verbose)
    {
    printf("-> Vertex post-transform cache analysis:\n");
    printf("   Using a cache size of %d vertices, hits: %u misses: %u\n",
           cache_size, cache_hit, cache_miss);
    }
    float ACMR = (float) cache_miss / (float) s->ntri;
    float ATVR = (float) cache_miss / (float) s->nvert;
// https://www.realtimerendering.com/blog/acmr-and-atvr/
    if(verbose)
    {
    printf("   Average Cache Miss Ratio (ACMR), misses/nTri\n   %f (optimal 0.5)\n"
           "   Average Transform to Vertex Ratio (ATVR), misses/nVert\n   %f (optimal: 1, worst: 6)\n",
           ACMR,
           ATVR);
    }
    vertex_cache_free(cache);
    return(ATVR);
}

void vshape_optimize(vshape_t * S)
{
    /* 1. Optimize triangle order */
    /* Optimal would be Morton order / Z-order
     * https://en.wikipedia.org/wiki/Z-order_curve
     *  or Hilbert order
     * https://en.wikipedia.org/wiki/Hilbert_curve
     */
    if(0) // Slower in benchmark
    {
    uint32_t * C0 = malloc(3*S->ntri*sizeof(uint32_t));
    memcpy(C0, S->tri, 3*S->ntri*sizeof(uint32_t));
    uint32_t * C1 = S->tri;
    uint32_t * buffer = malloc(3*sizeof(uint32_t));
    // Keep C0 as current best
    float best = vshape_mesh_status(S, 20, 0);
    for(int kk = 0; kk<100000; kk++)
    {
        S->tri = C1;

        uint32_t a = rand() % S->ntri;
        uint32_t b = rand() % S->ntri;
        // buffer = C1[a]
        memcpy(buffer, C1 + 3*a, 3*sizeof(uint32_t));
        // C1[a] = C1[b]
        memcpy(C1 + 3*a, C1 + 3*b, 3*sizeof(uint32_t));
        // C1[b] = buffer
        memcpy(C1 + 3*b, buffer, 3*sizeof(uint32_t));

        float v = vshape_mesh_status(S, 20, 0);
        if(v < best)
        {
            best = v;
            memcpy(C0, C1, 3*S->ntri*sizeof(uint32_t));
            printf("Iteration %d, %f\n", kk, best);
        } else {
            memcpy(C1, C0, 3*S->ntri*sizeof(uint32_t));
        }
    }

    S->tri = C0;
    free(C1);
    free(buffer);
    }

    /* 2. Optimize vertex locality */
    // Heuristics, place in the order that they appear
    // No difference in FPS
    uint32_t * vindex = malloc(S->nvert*sizeof(uint32_t));
    size_t writepos = 0;
    for(size_t kk = 0 ; kk < S->ntri*3; kk++)
    {
        uint32_t v = S->tri[kk];
        int found = 0;
        for(size_t ll = 0; ll < writepos; ll++)
        {
            if(vindex[ll] == v)
            {
                found = 1;
            }
        }
        if(found == 0)
        {
            vindex[writepos++] = v;
        }
    }

    /* Update triangles */
    // invert vindex
    uint32_t * rindex = malloc(S->nvert*sizeof(uint32_t));
    for(size_t kk = 0; kk < S->nvert; kk++)
    {
        rindex[vindex[kk]] = kk;
    }

    for(size_t kk = 0 ; kk < S->ntri*3; kk++)
    {
        S->tri[kk] = rindex[S->tri[kk]];
    }

    /* Reorder vertices */
    float * vert2 = malloc(S->nvert*3*sizeof(float));
    for(size_t kk = 0; kk < S->nvert; kk++)
    {
        memcpy(vert2 + 3*kk, S->vert + 3*vindex[kk], 3*sizeof(float));
    }
    free(S->vert);
    S->vert = vert2;

    free(rindex);
    free(vindex);
    return;
}
