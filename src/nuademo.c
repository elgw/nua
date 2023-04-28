#include "nuademo.h"

demo_data_t * demo_data_from_cmm(char * file)
{
    cmm_data_t * cmm = cmm_read(file);
    if(cmm == NULL)
    {
        printf("Could not read from %s\n", file);
        exit(EXIT_FAILURE);
    }

    demo_data_t * D = calloc(1, sizeof(demo_data_t));
    D->n_beads = cmm->n_markers;
    D->beads = malloc(sizeof(bead_data_t)*D->n_beads);
    D->n_links = cmm->n_links;
    D->links = malloc(sizeof(link_data_t)*D->n_links);
    D->link_ids = malloc(2*sizeof(uint32_t)*D->n_links);
    size_t offset = cmm->markers[0].id;

    for(uint32_t mm = 0; mm < (uint32_t) D->n_beads; mm++)
    {
        bead_data_t * B = &D->beads[mm];
        B->x = cmm->markers[mm].x;
        B->y = cmm->markers[mm].y;
        B->z = cmm->markers[mm].z;
        B->R = cmm->markers[mm].R;
        B->G = cmm->markers[mm].G;
        B->B = cmm->markers[mm].B;
        B->radius = cmm->markers[mm].radius;
    }

    for(uint32_t ll = 0; ll < (uint32_t) D->n_links; ll++)
    {
        //printf("ll=%u\n", ll);
        link_data_t * L = &D->links[ll];
        uint32_t id1 = cmm->links[ll].id1 - offset;
        uint32_t id2 = cmm->links[ll].id2 - offset;
        assert(id1 < (uint32_t) D->n_beads);
        assert(id2 < (uint32_t) D->n_beads);
        //printf("id1=%u, id2=%u\n", id1, id2); fflush(stdout);
        L->x1 = cmm->markers[id1].x;
        L->y1 = cmm->markers[id1].y;
        L->z1 = cmm->markers[id1].z;
        L->x2 = cmm->markers[id2].x;
        L->y2 = cmm->markers[id2].y;
        L->z2 = cmm->markers[id2].z;
        L->radius = cmm->links[ll].radius;
        D->link_ids[2*ll] = id1;
        D->link_ids[2*ll+1] = id2;
    }

    cmm_free(cmm);

    demo_data_update_link_data(D);

    return D;
}

void demo_data_update_link_data(demo_data_t * D)
{
    for(uint32_t kk = 0; kk < (uint32_t) D->n_links; kk++)
    {
        link_data_t * L = &D->links[kk];
        uint32_t b1 = D->link_ids[2*kk];
        uint32_t b2 = D->link_ids[2*kk+1];

        bead_data_t * B1 = &D->beads[b1];
        bead_data_t * B2 = &D->beads[b2];

        memcpy(&L->x1, B1, 3*sizeof(float));
        memcpy(&L->x2, B2, 3*sizeof(float));
        float r_bead = (B1->radius + B2->radius)/2.0;
        L->radius = r_bead/5.0;
    }
    return;
}

demo_data_t * demo_data_new(int n_beads, int n_links)
{
    int verbose = 0;
    if(verbose > 1)
    {
        printf("Creating demo data with %d beads and %d links\n",
               n_beads, n_links);
    }
    if(n_beads < 2)
    {
        if(n_links > 0)
        {
            printf("Impossible to have links with < 2 beads\n");
            exit(EXIT_FAILURE);
        }
    }
    demo_data_t * D = calloc(1, sizeof(demo_data_t));
    if(D == NULL)
    {
        return NULL;
    }
    D->n_beads = n_beads;
    D->n_links = n_links;
    D->link_ids = malloc(2*n_links*sizeof(uint32_t));
    D->beads = malloc(7*sizeof(float)*n_beads);
    D->links = malloc(7*sizeof(float)*n_links);

    float vol_tot = 4.0/3.0*M_PI;
    //float vol_rem = vol_tot;

    float vol_occ = 0.005; /* Volume occupancy */
    float vol_bead = vol_tot/n_beads*vol_occ;
    float r_bead = cbrt(vol_bead*M_PI*3.0/4.0);
    float vol_beads = 0;
    for(int kk = 0; kk<n_beads; kk++)
    {
        bead_data_t * B = &D->beads[kk];
        sphererand((float*) B);
        B->R = urand();
        B->G = urand();
        B->B = urand();
        B->radius = r_bead + 0.2*r_bead*midrand();
        f3_mul_scalar((float*) B, 1.0 - B->radius); // completely inside
        vol_beads += 4.0/3.0*M_PI*pow(B->radius, 3);
    }

    if(verbose > 0)
    {
        printf("Domain volume: %f Bead volume: %f, vol_occ = %f\n",
               vol_tot, vol_beads, vol_occ/vol_beads);
    }

    /*
     * Make some connections prefer short ones
     * Note: this will generate some duplicates and
     * chimera does not like that
     */

    int n_links_written = 0;
    while(n_links_written < n_links)
    {
        int b1 = rand() % n_beads;
        float dmin = 1e9;
        int b2 = 0;

        for(int ll = b1+1; ll < n_beads; ll++)
        {
            int p = ll; //rand() % n_beads;
            if(p != b1)
            {
                float d = f3_dist_f3((float*) &D->beads[b1],
                                     (float*) &D->beads[p]);
                if(d < dmin)
                {
                    dmin = d;
                    b2 = p;
                }
            }
        }
        if(b2 != 0)
        {
            int kk = n_links_written;
            D->link_ids[2*kk] = b1;
            D->link_ids[2*kk+1] = b2;
            n_links_written++;
        }
    }

    demo_data_update_link_data(D);

    return D;
}

void demo_data_free(demo_data_t * D)
{
    assert(D != NULL);
    free(D->beads);
    free(D->links);
    free(D->link_ids);
    free(D);
}

void demo_data_to_cmm(demo_data_t * D, char * name)
{
    cmm_data_t * cmm = malloc(sizeof(cmm_data_t));
    cmm->n_markers = D->n_beads;
    cmm->n_links = 0;
    cmm->markers = malloc(D->n_beads*sizeof(cmm_marker_t));
    cmm->n_links = D->n_links;
    cmm->links = malloc(D->n_links*sizeof(cmm_link_t));
    for(int kk = 0; kk < D->n_beads; kk++)
    {
        cmm_marker_t * m = &cmm->markers[kk];
        bead_data_t * b = &D->beads[kk];;
        m->id = kk+1;
        m->x = b->x;
        m->y = b->y;
        m->z = b->z;
        m->R = b->R;
        m->G = b->G;;
        m->B = b->B;
        m->radius = b->radius;
    }
    for(int ll = 0; ll < D->n_links; ll++)
    {
        cmm_link_t * l = &cmm->links[ll];
        uint32_t a = D->link_ids[2*ll];
        uint32_t b = D->link_ids[2*ll+1];
        l->id1 = a+1;
        l->id2 = b+1;
        l->R = 1;
        l->G = 1;
        l->B = 0;
        l->radius = D->links[ll].radius;
    }
    cmm_write(cmm, name);
    free(cmm);
    return;
}

/* Default settings for the demo */
nuademo_t * nuademo_new(void)
{
    nuademo_t * conf = calloc(1, sizeof(nuademo_t));
    if(conf == NULL)
    {
        return NULL;
    }

    conf->cmmdump = 0;
    conf->msaa_want = 8;
    conf->oneframe = 0;
    conf->validation_layers = 1;
    conf->verbose = 1;
    conf->n_beads = 300;
    conf->n_links = 100;
    conf->spherediv = -1;
    conf->linkdiv = -1;
    conf->cmmfile = NULL;
    conf->nua = NULL;
    return conf;
}

void nuademo_free(nuademo_t * conf)
{
    demo_data_free(conf->D);
    free(conf);
}

void nuademo_help(nuademo_t * p)
{
    printf("--help\n\t show this help and quit\n");
    printf("--cmmdump\n\t write bead coordinates to cmmdump.cmm\n");
    printf("--cmm file\n\t read a chimera marker file (.cmm)\n");
    printf("--nbeads b\n\t set the number of beads\n");
    printf("--nlinks l\n\t set the number of links\n");
    printf("--beadsize\n\t set the number of basepairs per bead\n");
    printf("--frag frag.spv\n\t specify fragment shader\n");
    printf("--vert vert.spv\n\t specify vertex shader\n");
    printf("--qball s\n\t set the number of subdivisions for the "
           "spheres/balls. Current = %d. Min: 0, Max: 6\n",
           p->spherediv);
    printf("--qlink s\n\t set the number of sides for the connectors. "
           "Current = %d. Min: 3\n",
           p->linkdiv);

    printf("--msaa n \n\t set the number of wanted MSAA bits. Current = %d\n",
           p->msaa_want);
    printf("--verbose n\n\t set verbosity level. Current = %d\n",
           p->verbose);
    printf("--novalidation\n\t disable any validation layers\n");
    printf("--oneframe\n\t show one frame and then quit, for debugging\n");
    exit(EXIT_SUCCESS);
}

void set_yellow_blue(demo_data_t * D, float blue)
{
    // Set the proportion blue beads to color blue
    assert(D != NULL);
    assert(blue <= 1.0);
    assert(blue >= 0.0);
    for(size_t kk = 0; kk < (size_t) D->n_beads; kk++)
    {
        D->beads[kk].R = 1;
        D->beads[kk].G = 1;
        D->beads[kk].B = 0;
    }

    #if 0
    // Random initialization
    size_t nset = 0;
    while( (double) nset / (double) D->n_beads < blue)
    {
        size_t idx = rand() % D->n_beads;
        if(D->beads[idx].B == 0)
        {
            D->beads[idx].R=0;
            D->beads[idx].G=1;
            D->beads[idx].B=1;
            nset++;
        }
    }
    #endif

    int start = (0.5 -0.5*blue)*D->n_beads;
    for(int kk = start; kk < start + blue*D->n_beads; kk++)
    {
        D->beads[kk].R=0;
        D->beads[kk].G=1;
        D->beads[kk].B=1;
    }

    for(size_t kk = 0; kk + 1< (size_t) D->n_beads; kk++)
    {
        D->link_ids[2*kk] = kk;
        D->link_ids[2*kk+1] = kk+1;
    }
    demo_data_update_link_data(D);
    return;
}

void nuademo_argparse(nuademo_t * p, int argc, char ** argv)
{
    struct option longopts[] = {
        { "nbeads", required_argument, NULL, 'b'},
        { "cmm", required_argument, NULL, 'C'},
        { "cmmdump", no_argument, NULL, 'c'},
        { "blue",  required_argument, NULL, '0'},
        { "beadsize", required_argument, NULL, 'B'},
        { "frag", required_argument, NULL, 'f'},
        { "nlinks", required_argument, NULL, 'l'},
        { "nvert", required_argument, NULL, 'v'},
        { "oneframe", no_argument, NULL, '1'},
        { "verbose", required_argument, NULL, 'V'},
        { "version", no_argument, NULL, 'w'},
        { "msaa", required_argument, NULL, 'm'},
        { "qsphere", required_argument, NULL, 's'},
        { "qlink", required_argument, NULL, 'L'},
        { "help", no_argument, NULL, 'h'},
        { "line", no_argument, NULL, 'l'},
        { "point", no_argument, NULL, 'p'},
        { "novalidation", no_argument, NULL, 'n'},
        { NULL, 0, NULL, 0 }
    };
    int ch;

    while((ch = getopt_long(argc, argv, "0:1b:cCb:BfhlLmnpsvVw", longopts, NULL)) != -1)
    {
        switch(ch)
        {
        case '1':
            p->oneframe = 1;
            break;
        case '0':
            p->blue = atof(optarg);
            p->yellow_blue = 1;
            break;
        case 'B':
            p->n_beads = 3400000000.0 / (double) atol(optarg);
            break;
        case 'b':
            p->n_beads = atol(optarg);
            break;
        case 'c':
            p->cmmdump = 1;
            break;
        case 'C':
            p->cmmfile = strdup(optarg);
            break;
        case 'f':
            //free(p->frag_shader_file);
            //p->frag_shader_file = strdup(optarg);
            break;
        case 'h':
            nuademo_help(p);
            break;
        case 'l':
            p->n_links = atoi(optarg);
            break;
        case 'L':
            p->linkdiv = atoi(optarg);
            break;
        case 'n':
            p->validation_layers = 0;
            break;
        case 'p':
            //p->polygon_mode = VK_POLYGON_MODE_POINT;
            break;
        case 'v':
            //free(p->vert_shader_file);
            //p->vert_shader_file = strdup(optarg);
            break;
        case 'w':
            printf("nuademo using ");
            nua_print_version(stdout);
            exit(EXIT_SUCCESS);
            break;
        case 'm':
            p->msaa_want = atoi(optarg);
            break;
        case 's':
            p->spherediv = atoi(optarg);
            break;
        case 'V':
            p->verbose = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Does not understand that option\n");
            exit(EXIT_FAILURE);
        }
    }


    if(p->cmmfile == NULL)
    {
        printf("Using %d beads (%.1f kb per bead on haploid HG)\n",
               p->n_beads,
               (double) 3400000000 / (double) p->n_beads / 1000);

        if(p->yellow_blue)
        {
            p->n_links = p->n_beads-1;
            p->D = demo_data_new(p->n_beads, p->n_links);
            set_yellow_blue(p->D, p->blue);


        } else {
            p->D = demo_data_new(p->n_beads, p->n_links);
        }

    } else {
        p->D = demo_data_from_cmm(p->cmmfile);
        p->n_beads = p->D->n_beads;
        p->n_links = p->D->n_links;
    }

    if(p->cmmdump == 1)
    {
        demo_data_to_cmm(p->D, "cmmdump.cmm");
    }

    return;
}

/* Transfer settings to nua */
void nuademo_configure_nua(nuademo_t * nuad, nua_t * nua)
{
    nuad->nua = nua;
    nua->oneframe = nuad->oneframe;
    nua->validation_layers = nuad->validation_layers;
    nua->verbose = nuad->verbose;

    if(nuad->spherediv > -1)
    {
        nua->spherediv = nuad->spherediv;
    } else {
        nua->spherediv = 5;
        int base = 2500;
        if(nuad->n_beads > base)
        {
            nua->spherediv = 4;
        }
        if(nuad->n_beads > 4*base)
        {
            nua->spherediv = 3;
        }
        if(nuad->n_beads > 16*base)
        {
            nua->spherediv = 2;
        }
        if(nuad->n_beads > 64*base)
        {
            nua->spherediv = 1;
        }
        if(nuad->n_beads > 256*base)
        {
            nua->spherediv = 0;
        }
    }
    if(nuad->linkdiv > -1)
    {
        nua->linkdiv = nuad->linkdiv;
    }
    else
    {
        if(nuad->n_beads == 0)
        {
            nua->linkdiv = 29;
        }
        else
        {
            nua->linkdiv = round(30.0*(1.0 - (float) nuad->n_beads/20000.0));
            nua->linkdiv < 3 ? nua->linkdiv = 3 : 0;
        }
    }

    if(nuad->verbose > 1)
    {
        printf("nua->linkdiv = %d, nua->spherediv = %d\n",
               nua->linkdiv, nua->spherediv);
    }

    if(nuad->verbose > 1)
    {
        printf("Transferring pointers to %d beads and %d links\n",
               nuad->D->n_beads, nuad->D->n_links);
    }
    assert(nuad->D != NULL);
    assert(nuad->D->beads != NULL);
    assert(nuad->D->links != NULL);

    nua->nbeads = nuad->D->n_beads;


    nua->bead_data = (float*) nuad->D->beads;

    nua->nlinks = nuad->D->n_links;

    nua->link_data = (float*) nuad->D->links;

#ifndef NDEBUG
    float sum = 0;
    for(int kk = 0; kk< nua->nbeads*7; kk++)
    {
        sum+=nua->bead_data[kk];
    }
    printf("Sum of bead data: %f\n", sum);
    sum = 0;
    for(int kk = 0; kk< nua->nlinks*7; kk++)
    {
        sum+=nua->link_data[kk];
    }
    printf("Sum of link data: %f\n", sum);
#endif

    switch(nuad->msaa_want)
    {
    case 1:
        nua->msaaSamples = VK_SAMPLE_COUNT_1_BIT;
        break;
    case 2:
        nua->msaaSamples = VK_SAMPLE_COUNT_2_BIT;
        break;
    case 4:
        nua->msaaSamples = VK_SAMPLE_COUNT_4_BIT;
        break;
    case 8:
        nua->msaaSamples = VK_SAMPLE_COUNT_8_BIT;
        break;
    case 16:
        nua->msaaSamples = VK_SAMPLE_COUNT_16_BIT;
        break;
    case 32:
        nua->msaaSamples = VK_SAMPLE_COUNT_32_BIT;
        break;
    case 64:
        nua->msaaSamples = VK_SAMPLE_COUNT_64_BIT;
        break;

    default:
        printf("Can't set MSAA to %d bit, try 1, 2, 4, 8, 16, 32, or, 64\n",
               nua->msaa_want);
    }
}

void nua_demo_ui(SDL_Event e, void *data)
{
    nuademo_t * nuad = (nuademo_t *) data;

    if( e.type == SDL_KEYDOWN )
    {
        char keyDown = e.key.keysym.scancode;
        if(keyDown == SDL_SCANCODE_R)
        {
            if(nuad->brown == 0)
            {
                if(nuad->verbose > 1)
                {
                    printf("Main: signaling worker to start\n");
                }
                pthread_cond_signal(&nuad->worker_cond);
            }
            nuad->brown *= 1.5;
            if(nuad->brown < 1e-3)
            {
                nuad->brown = 1e-3;
            }
        }
        if(keyDown == SDL_SCANCODE_F)
        {
            nuad->brown *= 1.0/1.5;
            if(nuad->brown < 1e-4)
            {
                nuad->brown = 0;
            }
        }
        if(keyDown == SDL_SCANCODE_C)
        {
            pthread_create(&nuad->timeout_thread,
                           NULL,
                           close_nua_from_thread,
                           nuad);
        }
    }
    return;
}

void nuad_wiggle_markers(nuademo_t * nuad)
{
    demo_data_t * dd = nuad->D;
    for(uint32_t kk = 0; kk < (uint32_t) dd->n_beads; kk++)
    {
        bead_data_t * B = &dd->beads[kk];
        B->x += nuad->brown*0.01*midrand();
        B->y += nuad->brown*0.01*midrand();
        B->z += nuad->brown*0.01*midrand();
        float n = f3_norm((float*) B);
        if(n > 1.0-B->radius)
        {
            f3_mul_scalar((float*) B, (1.0-B->radius)/n);
        }
    }
    demo_data_update_link_data(dd);
    return;
}

static float norm3(const float * X)
{
    return sqrt(pow(X[0], 2)
                + pow(X[1], 2)
                + pow(X[2], 2));
}

static float
eudist3(const float *A, const float *B)
{
    return sqrt(
                pow(A[0]-B[0],2)
                + pow(A[1]-B[1],2)
                + pow(A[2]-B[2],2));
}


void steric_hindrance(const float * restrict X, float * restrict g,
                      const size_t N, // Nbeads
                      const float r0, // Bead radius
                      const float F) // Force
{

    ovol_t * ovol = ovol_new(N, r0);

    for(size_t kk = 0; kk< N; kk++)
    {
        ovol_insert(ovol, 1, X[3*kk], X[3*kk+1], X[3*kk+2]);
    }

    for(size_t kk = 0; kk<N; kk++)
    {
        size_t nnb = 0;
        if(norm3(X+3*kk) > 1)
            continue;

        ovol_item_t * nb
            = ovol_get_neighbours(ovol,
                                  X[3*kk], X[3*kk+1], X[3*kk+2],
                                  &nnb);
        float did = 1.0;
        for(size_t nn = 0; nn < nnb; nn++)
        {
            if(norm3(&nb[nn].x) > 1)
                continue;

            float dx = nb[nn].x - X[3*kk];
            g[3*kk] += F*2*(dx)*did;
            float dy = nb[nn].y - X[3*kk+1];
            g[3*kk+1] += F*2*(dy)*did;
            float dz = nb[nn].z - X[3*kk+2];
            g[3*kk+2] += F*2*(dz)*did;
        }
    }
    ovol_free(ovol);
    return;
}

// r0: bead radius
// r1: capture distance
void attraction(float * X, uint8_t * u, float * g, size_t N, float r0, float r1, float F)
{

    ovol_t * ovol = ovol_new(N, r1);

    for(size_t kk = 0; kk< N; kk++)
    {
        if(u[kk])
            ovol_insert(ovol, 1, X[3*kk], X[3*kk+1], X[3*kk+2]);
    }

    for(size_t kk = 0; kk<N; kk++)
    {
        if(!u[kk])
            continue;

        if(norm3(X+3*kk) > 1)
            continue;

        size_t nnb = 0;
        ovol_item_t * nb
            = ovol_get_neighbours(ovol,
                                  X[3*kk], X[3*kk+1], X[3*kk+2],
                                  &nnb);

        for(size_t nn = 0; nn < nnb; nn++)
        {
            if(norm3(&nb[nn].x) > 1)
               continue;

            double di = eudist3(X+3*kk, &nb[nn].x);
            if(di < 2*r0)
                continue;

            double did = (di - 0.5*(r1+r0))/di;

            float dx = nb[nn].x - X[3*kk];
            g[3*kk] -= F*2*(dx)*did;
            float dy = nb[nn].y - X[3*kk+1];
            g[3*kk+1] -= F*2*(dy)*did;
            float dz = nb[nn].z - X[3*kk+2];
            g[3*kk+2] -= F*2*(dz)*did;
        }
    }
    ovol_free(ovol);
    return;
}

static void brownian_force(float * g, size_t N, float Fb)
{
    for(size_t pp = 0; pp < 3*N; pp++)
    {
        g[pp] = Fb*
            (
             ( (double) rand() / (double) RAND_MAX ) - 0.5);
    }
}


static void
in_domain(float * g, float * X, size_t N, float r0, float fDom)
{
    // r0: bead radius

    for(size_t kk = 0; kk< N; kk++)
    {
        double r = norm3(X+kk*3);
        if(r > 1-r0)
        {
            assert(fabs(r) > 0);
            double re = 2.0 / r * (r-(1-r0));
            for(int idx = 0; idx<3; idx++)
            {
                g[3*kk+idx] += fDom*X[kk*3+idx]*re;
            }
        }
    }
}

static void
links(float * g, uint32_t *L, size_t NL, float * X, float d0, float F)
{  for(size_t pp = 0; pp < NL; pp++)
    {
        size_t kk = L[pp*2];
        size_t ll = L[pp*2+1];

        double d = eudist3(X+3*kk, X+3*ll);

        if(d > d0 && d > 1e-6 && fabs(d) > 1e-5)
        {
            for(int idx = 0; idx<3; idx++)
            {
                g[3*kk+idx] += F*2*(X[3*kk+idx] - X[3*ll+idx])/d*(d - d0);
                g[3*ll+idx] -= F*2*(X[3*kk+idx] - X[3*ll+idx])/d*(d - d0);
            }
        }
    }
}


/*
 * - Start in a paused state
 * - Start working when signaled.
 * - Pause working when nuad->brown == 0
 * - Quit when nuad->worker_quit = 1
 *   requires signaling to discover if paused.
 * see https://docs.oracle.com/cd/E19455-01/806-5257/6je9h032r/index.html
 */
void * nuademo_calc_thread(void * data)
{
    nuademo_t * nuad = (nuademo_t*) data;
    nua_t * nua = nuad->nua;

    while(nuad->worker_quit == 0)
    {

        //printf("Thread: waiting\n");
        int status = pthread_cond_wait(&nuad->worker_cond,
                                       &nuad->worker_mutex);
        if(status != 0)
        {
            fprintf(stderr, "ERROR: pthread_cond_wait returned %d on line %d\n", status, __LINE__);
            exit(EXIT_FAILURE);
        }
        //printf("Thread unlocked (i.e. mutex locked)\n");

#if 1
        size_t N = nuad->D->n_beads;
        float * X = calloc(3*N+3, sizeof(float));
        float * Xm = calloc(3*N+3, sizeof(float));
        float * g = calloc(3*N+3, sizeof(float));
        uint8_t * u = calloc(N+1, sizeof(uint8_t));
        size_t NL =  nuad->D->n_links;
        uint32_t * L = calloc(2*NL+2, sizeof(uint32_t));

        if(X == NULL || Xm == NULL || g == NULL || u == NULL || L == NULL)
        {
            fprintf(stderr, "Memory allocation failure\n");
            exit(EXIT_FAILURE);
        }

        for(size_t kk = 0 ; kk<NL; kk++)
        {
            L[2*kk] = nuad->D->link_ids[2*kk];
            L[2*kk+1] = nuad->D->link_ids[2*kk+1];
        }

        for(size_t kk = 0; kk < N; kk++)
        {
            X[3*kk] = nuad->D->beads[kk].x;
            X[3*kk+1] = nuad->D->beads[kk].y;
            X[3*kk+2] = nuad->D->beads[kk].z;
            if(nuad->D->beads[kk].B > 0.5)
            {
                u[kk] = 1;
            }
            Xm[3*kk] = X[3*kk];
            Xm[3*kk+1] = X[3*kk+1];
            Xm[3*kk+2] = X[3*kk+2];
        }
        float dt = 0.01;

        float Fdomain = 10;
        float Flinks = 10;
        float Fattraction = 20;
        float Frepulsion = 40;

        float w = 1e-4;
        size_t it = 0;
        while(nuad->brown > 0 && nuad->worker_quit == 0)
        {
            printf("\r %zu", it++); fflush(stdout);
            w*=1.01;
            w > 1 ? w = 1 : 0;

            /* 3. Update X */

            // Brownian force

            float Fb = 0.01*nuad->brown;
            float r0 = nuad->D->beads[0].radius;
            // printf("Fb=%f\n", Fb);
            brownian_force(g, N, w*Fb);
            steric_hindrance(X, g, N, 2.0*r0, w*Frepulsion);
            attraction(X, u, g, N, r0, 3.0*r0, w*Fattraction);
            in_domain(g, X, N, r0, w*Fdomain);
            links(g, L, NL, X, 3*r0, w*Flinks);

            for(size_t pp = 0 ; pp < 3*N ; pp++)
            {
                double xt = X[pp];
                X[pp] = X[pp] + 0.999*(X[pp] - Xm[pp]) - g[pp]*pow(dt,2);
                Xm[pp] = xt; // Update Xm to reflect the previous X-value
            }

            for(size_t kk = 0; kk<N; kk++)
            {
                nuad->D->beads[kk].x = X[3*kk];
                nuad->D->beads[kk].y = X[3*kk+1];
                nuad->D->beads[kk].z = X[3*kk+2];
            }

            demo_data_update_link_data(nuad->D);
            nua_data_changed(nua);
        }

        free(u);
        free(X);
        free(Xm);
        free(g);
        free(L);
#else

        while(nuad->brown > 0 && nuad->worker_quit == 0)
        {
            nuad_wiggle_markers(nuad);
            nua_data_changed(nua);
        }
#endif

        //printf("Locking thread\n");
        pthread_mutex_unlock(&nuad->worker_mutex);
    }

    pthread_mutex_unlock(&nuad->worker_mutex);
    return NULL;
}

void nuademo_start_calc(void *data)
{
    nuademo_t * nuad = (nuademo_t*) data;
    int status = pthread_create(&nuad->worker_thread,
                                NULL,
                                nuademo_calc_thread,
                                nuad);
    if(status != 0)
    {
        fprintf(stderr, "nuademo_start_calc: Failed to create a worker thread "
                "on line %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }
    return;
}

void nuademo_stop_calc(void *data)
{
    nuademo_t * nuad = (nuademo_t*) data;

    nuad->worker_quit = 1;
    pthread_cond_signal(&nuad->worker_cond);
    pthread_join(nuad->worker_thread, NULL);

    return;
}

void * close_nua_from_thread(void * data)
{
    nuademo_t * nuad = (nuademo_t *) data;
    printf("Will close nua from a thread in 5 s\n");
    sleep(5);
    nua_close(nuad->nua);
    return NULL;
}

void nuad_print_usage(UNUSED nuademo_t * nuad)
{
    printf(" Keyboard shortcuts enabled in nuademo:\n");
    printf("      <r> increase brownian force      <f> decrease brownian force\n");
    printf("      <c> ask a thread to close nua in 5 s\n");
}

int main(int argc, char ** argv)
{
    srand(time(NULL));

    nuademo_t * nuad = nuademo_new();
    assert(nuad != NULL);

    nuademo_argparse(nuad, argc, argv);

    nua_t * nua = nua_new();
    nuademo_configure_nua(nuad, nua);

    /* Capture some user inputs, we add 't' and 'g' to control
     * Brownian movements. */
    nua->user_handle = nua_demo_ui;
    nua->user_data = nuad;
    /* show extra user interactions enabled */
    nuad_print_usage(nuad);

    /* Start some calculations in a thread */
    nuademo_start_calc(nuad);

    /* Show the window */
    nua_run(nua);

    /* We get here if the window was closed */
    nua_destroy(nua);

    /* Stop your calculations */
    nuademo_stop_calc(nuad);

    /* Wait for this thread if it is running */
    pthread_join(nuad->timeout_thread, NULL);

    nuademo_free(nuad);

    printf("Peak memory: %zu kb\n", get_peakMemoryKB());
    return EXIT_SUCCESS;
}
