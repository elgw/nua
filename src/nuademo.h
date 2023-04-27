#ifndef __nuademo_h__
#define __nuademo_h__

#include "nua.h"

#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include "ovolhash.h"

struct __attribute__((packed, aligned(__alignof__(float)))) bead_data
{
    float x;
    float y;
    float z;
    float R;
    float G;
    float B;
    float radius;
};

typedef struct bead_data bead_data_t;

struct __attribute__((packed, aligned(__alignof__(float)))) link_data
{
    float x1;
    float y1;
    float z1;
    float x2;
    float y2;
    float z2;
    float radius;
};
typedef struct link_data link_data_t;

typedef struct
{
    int n_beads;
    bead_data_t * beads;
    /* a to b, b to d, f to a, etc */
    uint32_t * link_ids;
    int n_links;
    link_data_t * links;
} demo_data_t;


typedef struct
{
    int cmmdump;
    int msaa_want;
    int oneframe;
    int validation_layers;
    int verbose;
    int n_beads;
    int n_links;
    int spherediv;
    int linkdiv;
    char * cmmfile;
    demo_data_t * D;
    nua_t * nua;

    float brown;

    int yellow_blue;
    float blue;

    int worker_quit;
    pthread_t worker_thread;
    pthread_t timeout_thread;
    pthread_cond_t worker_cond;
    pthread_mutex_t worker_mutex;

} nuademo_t;

/* Just to test/demonstrate that nua can be closed from a thread
 * Most likely your calculation thread should call this when done unless
 * you want the GUI to stay open.
 */
void * close_nua_from_thread(void * data);

/* Initialize data structures for the demo */
nuademo_t * nuademo_new(void);
/* Configure based on command line */
void nuademo_argparse(nuademo_t * p, int argc, char ** argv);
void nuademo_free(nuademo_t * conf);
/* Print help/usage to stdout */
void nuademo_help(nuademo_t * p);

/* Set parameters of the nua */
void nuademo_configure_nua(nuademo_t * nuad, nua_t * nua);

/* Callback for nua for user defined mouse/keyboard events */
void nua_demo_ui(SDL_Event e, void *data);

/* Create some random data for testing */
demo_data_t * demo_data_new(int n_beads, int n_links);
void demo_data_free(demo_data_t * D);
/* Read data from cmm file */
demo_data_t * demo_data_from_cmm(char * file);
/* Write to chimera marker .cmm file */
void demo_data_to_cmm(demo_data_t * D, char * name);

/* Send link data to nua */
void demo_data_update_link_data(demo_data_t * D);

/* For move around the markers
 * in a separate thread
 */
void nuademo_start_calc(void *data);
void nuademo_stop_calc(void *data);
void * nuademo_calc_thread(void * data);
void nuad_wiggle_markers(nuademo_t * nuad);

#endif
