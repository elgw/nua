#include "nua_util.h"

float timespec_diff(struct timespec* end, struct timespec * start)
{
    double elapsed = (end->tv_sec - start->tv_sec);
    elapsed += (end->tv_nsec - start->tv_nsec) / 1000000000.0;
    return elapsed;
}


size_t get_peakMemoryKB(void)
{
#ifdef WINDOWS
    return 0;
#else
#ifdef __APPLE__
    struct rusage r_usage;
    getrusage(RUSAGE_SELF, &r_usage);
    return (size_t) round((double) r_usage.ru_maxrss/1024.0);
#endif

#ifndef __APPLE__

    char * statfile = malloc(100*sizeof(char));
    sprintf(statfile, "/proc/%d/status", getpid());
    FILE * sf = fopen(statfile, "r");
    if(sf == NULL)
    {
        fprintf(stderr, "Failed to open %s\n", statfile);
        free(statfile);
        return 0;
    }

    char * peakline = NULL;

    char * line = NULL;
    size_t len = 0;

    while( getline(&line, &len, sf) > 0)
    {
        if(strlen(line) > 6)
        {
            if(strncmp(line, "VmPeak", 6) == 0)
            {
                peakline = strdup(line);
            }
        }
    }
    free(line);
    fclose(sf);
    free(statfile);

    // Parse the line starting with "VmPeak"
    // Seems like it is always in kB
    // (reference: fs/proc/task_mmu.c)
    // actually in kiB i.e., 1024 bytes
    // since the last three characters are ' kb' we can skip them and parse in between
    size_t peakMemoryKB = 0;
    //  printf("peakline: '%s'\n", peakline);
    if(strlen(peakline) > 11)
    {
        peakline[strlen(peakline) -4] = '\0';

        //    printf("peakline: '%s'\n", peakline+7);
        peakMemoryKB = (size_t) atol(peakline+7);
    }

    free(peakline);
    return peakMemoryKB;
#endif
#endif
}

const char ** vec_char_add(const char ** vec, uint32_t *n, char * new)
{
    const int verbose = 0;
    // Check that new isn't already in the vector
    for(uint32_t kk = 0; kk< *n; kk++)
    {
        if(strcmp(vec[kk], new) == 0)
        {
            if(verbose > 2)
            {
                printf("vec_char_add: %s is already in the list\n", new);
            }
            return vec;
        }
    }
    if(verbose > 2)
    {
        printf("vec_char_add: %s was not in the list\n", new);
    }

    const char ** xvec = calloc((*n+1), sizeof(char*));
    for(uint32_t kk = 0; kk<*n; kk++)
    {
        xvec[kk] = vec[kk];
    }

    xvec[*n] = new;
    n[0]++;

    return xvec;
}

void vec_char_free(char ** vec, int n)
{
    for(int kk = 0; kk<n; kk++)
    {
        free(vec[kk]);
    }
    free(vec);
    return;
}

void showMat4(const char * name, float * M)
{
    if(name != NULL)
    {
        printf("%s\n", name);
    }
    for(int kk = 0; kk<4; kk++)
    {
        for(int ll = 0; ll<4; ll++)
        {
            printf("% .1f ", M[kk*4+ll]);
        }
        printf("\n");
    }
}

float float_min(float a, float b)
{
    if(a < b)
    {
        return a;
    }
    return b;
}

float float_max(float a, float b)
{
    if(a < b)
    {
        return b;
    }
    return a;
}

float f3_norm(float * p)
{
    return sqrt(pow(p[0], 2) + pow(p[1], 2) + pow(p[2], 2));
}

float urand(void)
{
    float r = (float) rand() / (float) RAND_MAX;
    return r;
}

float midrand(void)
{
    float r = (float) rand() / (float) RAND_MAX;
    r = (r-0.5)*2;
    return r;
}

void sphererand(float * p)
{
    do{
        p[0] = midrand(); // x
        p[1] = midrand(); // y
        p[2] = midrand(); // z
    } while(f3_norm(p) > 1.0);
    return;
}

void f3_mul_scalar(float * p, float k)
{
    for(int ii = 0; ii < 3; ii++)
    {
        p[ii]*=k;
    }
    return;
}

float f3_dist_f3(float * a, float * b)
{
    return sqrt(  pow(a[0]-b[0], 2)
                + pow(a[1]-b[1], 2)
                + pow(a[2]-b[2], 2) );
}

int nua_isfile(char * file_name)
{
    int isfile = 0;
    FILE * fid = fopen(file_name, "r");
    if(fid != NULL)
    {
        isfile = 1;
        fclose(fid);
    }
    return isfile;
}

void nua_print_version(FILE * fid)
{
    fprintf(fid, "nua version %s.%s.%s\n",
            NUA_VERSION_MAJOR,
            NUA_VERSION_MINOR,
            NUA_VERSION_PATCH);
}
