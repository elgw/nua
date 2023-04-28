#include "ovolhash.h"

static size_t
ovol_hash1(const ovol_t * ovol, ofloat x)
{
    /* Silently project back points outside of the domain
    *  to avoid invalid hash values
    */
    x < -1.0 ? x = -1.0 : 0;
    x > 1.0 ? x = 1.0 : 0;
    return round((x+1.0)/ovol->dx1);
}

static size_t
ovol_hash(const ovol_t * ovol, const ofloat x, const ofloat y, const ofloat z)
{
    size_t h1 = ovol_hash1(ovol, x);
    size_t h2 = ovol_hash1(ovol, y);
    size_t h3 = ovol_hash1(ovol, z);

    assert(h1 < ovol->nbin1);
    assert(h2 < ovol->nbin1);
    assert(h3 < ovol->nbin1);
    size_t h = h1 + ovol->nbin1*h2 + ovol->nbin1*ovol->nbin1*h3;
    // printf("(%f, %f, %f) -> [%zu, %zu, %zu]\n", x, y, z, h1, h2, h3);
    return h*ovol->bucket_size;
}

ovol_t * ovol_new(const size_t N, const ofloat rc)
{
    assert(N > 0);
    ovol_t * ovol = calloc(1, sizeof(ovol_t));
    if(ovol == NULL)
    {
        return NULL;
    }
    ovol->N_construct = N;
    ovol->rc = rc;
    ovol->rc2 = pow(ovol->rc, 2);

    /* The radio beteen dx1/rc determines much of the performance.
     * aorund 1.5 seems to be good.
     *
     */
    ovol->nbin1 = floor(2.0 / rc* 0.7);
    ovol->dx1 = 2.0 / (double) (ovol->nbin1 - 1);

    //printf("dx/rc = %f\n", ovol->dx1/rc);

    ovol->nbin = pow(ovol->nbin1, 3);
    ovol->bucket_size = 1;
    ovol->bucket_size = ceil(2.0*N/(ovol->nbin));
    ovol->bucket_size < 2 ? ovol->bucket_size = 2 : 0;
    ovol->bucket_size *= 4;
    if(ovol->nbin*ovol->bucket_size < N)
    {
        printf("Creating room for %zu elements\n", N);
        printf("But only allocating for %zux%zu=%zu elements\n", ovol->nbin, ovol->bucket_size,
               ovol->nbin*ovol->bucket_size);
        printf("Would want at least 2x%zu = %zu\n", N, 2*N);
        printf("This is an implementation problem, please file a bug report\n");
        exit(EXIT_FAILURE);
    }

    if(0)
    {
        printf("Initialized a bucket structure:\n");
        printf("dx1=%f r_c=%f\n", ovol->dx1, rc);
        printf("nbin1=%zu\n", ovol->nbin1);
        printf("nbin: %zu\n", ovol->nbin);
        printf("bucket size: %zu\n", ovol->bucket_size);
        printf("load factor: %f\n", (double) N
               / (double) ovol->bucket_size
               / (double) ovol->nbin);
    }

    ovol->N_alloc = ovol->bucket_size*ovol->nbin;
    ovol->items = calloc(ovol->N_alloc, sizeof(ovol_item_t));
    ovol->result = calloc(ovol->bucket_size*27, sizeof(ovol_item_t));

    assert(ovol_hash(ovol, 1.0, 1.0, 1.0) < ovol->nbin*ovol->bucket_size);
    return ovol;
}


static size_t
ovol_find_pos(const ovol_t * ovol, size_t h)
{
    assert(h < ovol->N_alloc);
    while(ovol->items[h].r != 0)
    {
        h++;

        /* Turn around if we hit the end */
        if(h == ovol->N_alloc)
        { h = 0; }

    }
    return h;
}

int
ovol_insert(ovol_t * ovol, const ofloat r,
            const ofloat x, const ofloat y, const ofloat z)
{
    assert(r != 0);
    size_t hash = ovol_hash(ovol, x, y, z);

    size_t idx = ovol_find_pos(ovol, hash);

    ovol->items[idx].r = r;
    ovol->items[idx].x = x;
    ovol->items[idx].y = y;
    ovol->items[idx].z = z;
#ifdef ovolhash_store
    ovol->items[idx].hash = hash;
#endif
    ovol->N_inserted++;
    return EXIT_SUCCESS;
}


ovol_item_t *
ovol_get_neighbours(const ovol_t * ovol,
                    const ofloat x, const ofloat y, const ofloat z,
                    size_t * _N)
{
    size_t N = 0; /* N found */
    const double rc2 = ovol->rc2; /* Radius of capture squared */

    /* Check up to 27 possible bins */
    size_t h1min = ovol_hash1(ovol, x - ovol->rc);
    size_t h1max = ovol_hash1(ovol, x + ovol->rc);
    size_t h2min = ovol_hash1(ovol, y - ovol->rc);
    size_t h2max = ovol_hash1(ovol, y + ovol->rc);
    size_t h3min = ovol_hash1(ovol, z - ovol->rc);
    size_t h3max = ovol_hash1(ovol, z + ovol->rc);

    for(size_t h3 = h3min; h3 <= h3max ; h3++){
        for(size_t h2 = h2min; h2 <= h2max ; h2++){
            for(size_t h1 = h1min; h1 <= h1max ; h1++){
                /* Hash value for the bucket of interest, note that the search
                 * might continue to other buckets if bh is full. */
                size_t bh = ovol->bucket_size
                    *(h1 + ovol->nbin1*h2 + ovol->nbin1*ovol->nbin1*h3);

                size_t h = bh; /* Current hash table index */

                while(ovol->items[h].r != 0) /* Unless empty */
                {
                    /* To make sure that the items are from the
                     *  "correct bucket" we must calculate their hash
                     *  value in order to avoid multiple hits for the
                     *  same item most likely only a problem between
                     *  h1min and h1max Alternatively we could store
                     *  this with the elements.  or use another
                     *  strategy to keep the risk low.
                     *  This check consumes quite much time .. :(
                    */
#ifdef ovolhash_store
                    if(ovol->items[h].hash == bh)
#else
                    if(ovol_hash(ovol, ovol->items[h].x, ovol->items[h].y, ovol->items[h].z) == bh)
                        #endif
                    {
                    ofloat dx2 = (x-ovol->items[h].x)*(x-ovol->items[h].x);
                    ofloat dy2 = (y-ovol->items[h].y)*(y-ovol->items[h].y);
                    ofloat dz2 = (z-ovol->items[h].z)*(z-ovol->items[h].z);
                    if(dx2 + dy2 + dz2 < rc2)
                    {
                        ovol->result[N].r = ovol->items[h].r;
                        ovol->result[N].x = ovol->items[h].x;
                        ovol->result[N].y = ovol->items[h].y;
                        ovol->result[N].z = ovol->items[h].z;
                        N++;
                    }
                    }
                    h++;
                    /* Start over from the beginning if we hit the end */
                    if(h == ovol->nbin*ovol->bucket_size)
                        h = 0;
                }
            }
        }
    }

    *_N = N;
    return ovol->result;
}

void
ovol_free(ovol_t * ovol)
{
    free(ovol->items);
    free(ovol->result);
    free(ovol);
}
