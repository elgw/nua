#include "cmm_io.h"

static void parse_link2(xmlTextReaderPtr r, cmm_link_t * l)
{
    memset(l, 0, sizeof(cmm_link_t));
    {
        xmlChar * val = xmlTextReaderGetAttribute(r, (xmlChar *) "id1");
        assert(val != NULL);
        l->id1 = atol((const char*) val);
        xmlFree(val);
    }
    {
        xmlChar * val = xmlTextReaderGetAttribute(r, (xmlChar *) "id2");
        assert(val != NULL);
        l->id2 = atol((const char*) val);
        xmlFree(val);
    }
    {
        xmlChar * val = xmlTextReaderGetAttribute(r, (xmlChar *) "r");
        assert(val != NULL);
        l->R = atof((const char*) val);
        xmlFree(val);
    }
    {
        xmlChar * val = xmlTextReaderGetAttribute(r, (xmlChar *) "g");
        assert(val != NULL);
        l->G = atof((const char*) val);
        xmlFree(val);
    }
    {
        xmlChar * val = xmlTextReaderGetAttribute(r, (xmlChar *) "b");
        assert(val != NULL);
        l->B = atof((const char*) val);
        xmlFree(val);
    }
    {
        xmlChar * val = xmlTextReaderGetAttribute(r, (xmlChar *) "radius");
        assert(val != NULL);
        l->radius = atof((const char*) val);
        xmlFree(val);
    }

}

static void parse_marker2(xmlTextReaderPtr r, cmm_marker_t * m)
{
    memset(m, 0, sizeof(cmm_marker_t));
    /* Default color: white (as in chimera) */
    m->R = 1; m->G = 1; m->B = 1;

    {
        xmlChar * val = xmlTextReaderGetAttribute(r, (xmlChar *) "id");
        assert(val != NULL);
        m->id = atol((const char*) val);
        xmlFree(val);
    }
    {
        xmlChar * val = xmlTextReaderGetAttribute(r, (xmlChar *) "x");
        assert(val != NULL);
        m->x = atof((const char*) val);
        xmlFree(val);
    }
    {
        xmlChar * val = xmlTextReaderGetAttribute(r, (xmlChar *) "y");
        assert(val != NULL);
        m->y = atof((const char*) val);
        xmlFree(val);
    }
    {
        xmlChar * val = xmlTextReaderGetAttribute(r, (xmlChar *) "z");
        assert(val != NULL);
        m->z = atof((const char*) val);
        xmlFree(val);
    }
    {
        xmlChar * val = xmlTextReaderGetAttribute(r, (xmlChar *) "r");
        if(val != NULL)
        {
            m->R = atof((const char*) val);
            xmlFree(val);
        }
    }
    {
        xmlChar * val = xmlTextReaderGetAttribute(r, (xmlChar *) "g");
        if(val != NULL)
        {
            m->G = atof((const char*) val);
            xmlFree(val);
        }
    }
    {
        xmlChar * val = xmlTextReaderGetAttribute(r, (xmlChar *) "b");
        if(val != NULL){
            m->B = atof((const char*) val);
            xmlFree(val);
        }
    }
    {
        xmlChar * val = xmlTextReaderGetAttribute(r, (xmlChar *) "radius");
        if(val != NULL){
            m->radius = atof((const char*) val);
            xmlFree(val);
        }
    }
    return;
}


cmm_data_t * cmm_read(char * file)
{
    cmm_data_t * cmm = NULL;
    int ret;
    xmlTextReaderPtr reader;

    //doc = xmlParseFile(file);
    reader = xmlNewTextReaderFilename(file);
    if (reader == NULL )
    {
        fprintf(stderr,"Document not parsed successfully. \n");
        return cmm;
    }

    size_t nmarkers = 0;
    size_t nlinks = 0;

    size_t nmarkers_alloc = 1000;
    size_t nlinks_alloc = 1000;
    cmm = malloc(sizeof(cmm_data_t));
    cmm->markers = malloc(nmarkers_alloc * sizeof(cmm_marker_t));
    cmm->links = malloc(nlinks_alloc * sizeof(cmm_link_t));

    ret = xmlTextReaderRead(reader);
    while (ret == 1) {
        const xmlChar * name = xmlTextReaderConstName(reader);
        if (strcmp((char*) name, "marker") == 0)
        {
            if(nmarkers+1 == nmarkers_alloc)
            {
                nmarkers_alloc = 1.2*nmarkers_alloc;
                cmm->markers = realloc(cmm->markers, nmarkers_alloc*sizeof(cmm_marker_t));
            }
            //xmlNodePtr cur;
            parse_marker2(reader, cmm->markers+nmarkers);
            nmarkers++;
        }
        else if (strcmp((char*) name, "link") == 0)
        {
            if(nlinks+1 == nlinks_alloc)
            {
                nlinks_alloc = 1.2*nlinks_alloc;
                cmm->links = realloc(cmm->links, nlinks_alloc*sizeof(cmm_link_t));
            }
            parse_link2(reader, cmm->links+nlinks);
            nlinks++;
        }

        ret = xmlTextReaderRead(reader);
    }
    xmlFreeTextReader(reader);
    if (ret != 0) {
        printf("%s : failed to parse\n", file);
    }

    printf("Found %zu markers and %zu links\n", nmarkers, nlinks);

    cmm->n_markers = nmarkers;

    if(nmarkers == 0)
    {
        free(cmm->markers);
        cmm->markers = NULL;
    } else {
        cmm->markers = realloc(cmm->markers, cmm->n_markers*sizeof(cmm_marker_t));
    }

    cmm->n_links = nlinks;
    if(nlinks == 0)
    {
        free(cmm->links);
        cmm->links = NULL;
    } else {
        cmm->links = realloc(cmm->links, cmm->n_links*sizeof(cmm_link_t));
    }

    return cmm;
}


static void parse_link(xmlNodePtr cur, cmm_link_t * l)
{
    memset(l, 0, sizeof(cmm_link_t));
    {
        xmlChar * id1 = xmlGetProp(cur, (const xmlChar*) "id1");
        assert(id1 != NULL);
        l->id1 = atol((const char*) id1);
        xmlFree(id1);
    }
    {
        xmlChar * id2 = xmlGetProp(cur, (const xmlChar*) "id2");
        assert(id2 != NULL);
        l->id2 = atol((const char*) id2);
        xmlFree(id2);
    }
    {
        xmlChar * R = xmlGetProp(cur, (const xmlChar*) "r");
        if(R != NULL)
        {
            l->R = atof((const char*) R);
            xmlFree(R);
        }
    }
    {
        xmlChar * G = xmlGetProp(cur, (const xmlChar*) "g");
        if(G != NULL)
        {
            l->G = atof((const char*) G);
            xmlFree(G);
        }
    }
    {
        xmlChar * B = xmlGetProp(cur, (const xmlChar*) "b");
        if(B != NULL)
        {
            l->B = atof((const char*) B);
            xmlFree(B);
        }
    }
    {
        xmlChar * r = xmlGetProp(cur, (const xmlChar*) "radius");
        if(r != NULL)
        {
            l->radius = atof((const char*) r);
            xmlFree(r);
        }
    }
    return;
}


static void parse_marker(xmlNodePtr cur, cmm_marker_t * m)
{
    memset(m, 0, sizeof(cmm_marker_t));
    /* Default color: white (as in chimera) */
    m->R = 1; m->G = 1; m->B = 1;
    {
        xmlChar * id = xmlGetProp(cur, (const xmlChar*) "id");
        assert(id != NULL);
        m->id = atol((const char*) id);
        xmlFree(id);
    }
    {
        xmlChar * x = xmlGetProp(cur, (const xmlChar*) "x");
        assert(x != NULL);
        m->x = atof((const char*) x);
        xmlFree(x);
    }
    {
        xmlChar * y = xmlGetProp(cur, (const xmlChar*) "y");
        assert(y != NULL);
        m->y = atof((const char*) y);
        xmlFree(y);
    }
    {
        xmlChar * z = xmlGetProp(cur, (const xmlChar*) "z");
        assert(z != NULL);
        m->z = atof((const char*) z);
        xmlFree(z);
    }
    {
        xmlChar * R = xmlGetProp(cur, (const xmlChar*) "r");
        if(R != NULL)
        {
            m->R = atof((const char*) R);
            xmlFree(R);
        }
    }
    {
        xmlChar * G = xmlGetProp(cur, (const xmlChar*) "g");
        if(G != NULL)
        {
            m->G = atof((const char*) G);
            xmlFree(G);
        }
    }
    {
        xmlChar * B = xmlGetProp(cur, (const xmlChar*) "b");
        if(B != NULL)
        {
            m->B = atof((const char*) B);
            xmlFree(B);
        }
    }
    {
        xmlChar * r = xmlGetProp(cur, (const xmlChar*) "radius");
        assert(r != NULL);
        m->radius = atof((const char*) r);
        xmlFree(r);
    }
    return;
}

cmm_data_t * cmm_read_parser(char * file)
{
    cmm_data_t * cmm = NULL;

    xmlDocPtr doc;
    xmlNodePtr cur;

    doc = xmlParseFile(file);
    if (doc == NULL )
    {
        fprintf(stderr,"Document not parsed successfully. \n");
        return cmm;
    }
    cur = xmlDocGetRootElement(doc);

    if (cur == NULL)
    {
        fprintf(stderr,"empty document\n");
        xmlFreeDoc(doc);
        return cmm;
    }

    if (xmlStrcmp(cur->name, (const xmlChar *) "marker_set"))
    {
        fprintf(stderr,"document of the wrong type, root node != marker_set");
        xmlFreeDoc(doc);
        return cmm;
    }

    size_t nmarkers = 0;
    size_t nlinks = 0;

    size_t nmarkers_alloc = 1000;
    size_t nlinks_alloc = 1000;
    cmm = malloc(sizeof(cmm_data_t));
    cmm->markers = malloc(nmarkers_alloc * sizeof(cmm_marker_t));
    cmm->links = malloc(nlinks_alloc * sizeof(cmm_link_t));

    cur = cur->xmlChildrenNode;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, (const xmlChar *) "marker")))
        {
            if(nmarkers+1 == nmarkers_alloc)
            {
                nmarkers_alloc = 1.2*nmarkers_alloc;
                cmm->markers = realloc(cmm->markers, nmarkers_alloc*sizeof(cmm_marker_t));
            }
            parse_marker(cur, cmm->markers+nmarkers);
            nmarkers++;
        }
        else if ((!xmlStrcmp(cur->name, (const xmlChar *) "link")))
        {
            if(nlinks+1 == nlinks_alloc)
            {
                nlinks_alloc = 1.2*nlinks_alloc;
                cmm->links = realloc(cmm->links, nlinks_alloc*sizeof(cmm_link_t));
            }
            parse_link(cur, cmm->links+nlinks);
            nlinks++;
        }

	cur = cur->next;
    }

    printf("Found %zu markers and %zu links\n", nmarkers, nlinks);

    cmm->n_markers = nmarkers;

    if(nmarkers == 0)
    {
        free(cmm->markers);
        cmm->markers = NULL;
    } else {
        cmm->markers = realloc(cmm->markers, cmm->n_markers*sizeof(cmm_marker_t));
    }

    cmm->n_links = nlinks;
    if(nlinks == 0)
    {
        free(cmm->links);
        cmm->links = NULL;
    } else {
        cmm->links = realloc(cmm->links, cmm->n_links*sizeof(cmm_link_t));
    }


    xmlFreeDoc(doc);
    return cmm;
}

int cmm_write(cmm_data_t * cmm, char * filename)
{
    if(cmm == NULL)
    {
        return EXIT_FAILURE;
    }
    if(cmm->n_markers == 0 && cmm->n_links == 0)
    {
        return EXIT_FAILURE;
    }
    FILE * f = fopen(filename, "w");
    if(f == NULL)
    {
        return EXIT_FAILURE;
    }

    fprintf(f, "<marker_set name=\"dump\">\n");

    for(uint32_t mm = 0; mm < cmm->n_markers; mm++)
    {
        cmm_marker_t * M = &cmm->markers[mm];
        fprintf(f, "<marker id=\"%u\" x=\"%.3f\" y=\"%.3f\" z=\"%.3f\" r=\"%f\" g=\"%f\" b=\"%f\" radius=\"%f\" />\n",
                M->id,
                M->x, M->y, M->z,
                M->R, M->G, M->B,
                M->radius);
    }
    for(uint32_t ll = 0; ll < cmm->n_links; ll++)
    {
        cmm_link_t * L = &cmm->links[ll];
        fprintf(f, "<link id1=\"%u\" id2=\"%u\" r=\"%f\" g=\"%f\" b=\"%f\" radius=\"%f\"/>\n",
                L->id1, L->id2,
                L->R, L->G, L->B,
                L->radius);

    }
    fprintf(f, "</marker_set>\n");
    fclose(f);
    return EXIT_FAILURE;
}

void cmm_marker_print(FILE * fid, cmm_marker_t M)
{
    fprintf(fid, "id: %u x: %f y: %f, z: %f, R: %f G: %f B: %f radius: %f\n",
            M.id,
            M.x, M.y, M.z,
            M.R, M.G, M.B,
            M.radius);
    return;
}

void cmm_link_print(FILE * fid, cmm_link_t L)
{
    fprintf(fid, "id1: %u, id2: %u, R: %f, G: %f, B: %f, r: %f\n",
            L.id1, L.id2,
            L.R, L.G, L.B,
            L.radius);
    return;
}

void cmm_print(FILE * fid, cmm_data_t * cmm)
{
    if(cmm == NULL)
    {
        fprintf(fid, "cmm_print: got NULL for data\n");
        return;
    }
    fprintf(stdout, "marker data with %u markers and %u links\n",
            cmm->n_markers, cmm->n_links);
    for(uint32_t mm = 0; mm<cmm->n_markers; mm++)
    {
        printf("MARKER %u ", mm);
        cmm_marker_print(fid, cmm->markers[mm]);
    }
    for(uint32_t ll = 0; ll < cmm->n_links; ll++)
    {
        printf("LINK %u ", ll);
        cmm_link_print(fid, cmm->links[ll]);
    }
}

void cmm_free(cmm_data_t * cmm)
{
    if(cmm == NULL)
    {
        return;
    }
    free(cmm->markers);
    free(cmm->links);
    free(cmm);
}

uint8_t cmap[] = {255,255,255,
    240,163,255,
    0,117,220,
    153,63,0,
    76,0,92,
    25,25,25,
    0,92,49,
    43,206,72,
    255,204,153,
    128,128,128,
    148,255,181,
    143,124,0,
    157,204,0,
    194,0,136,
    0,51,128,
    255,164,5,
    255,168,187,
    66,102,0,
    255,0,16,
    94,241,242,
    0,153,143,
    224,255,102,
    116,10,255,
    153,0,0,
    255,255,128,
    255,255,0,
    255,80,5};


int cmmwritez(char * fname, double * D, size_t nD, double radius, uint32_t * P, size_t NP, uint8_t * L)
{

    gzFile zf = gzopen(fname, "wb");
    char * line = malloc(1024*sizeof(char));

    if(zf == NULL)
    {
        fprintf(stderr, "Unable to gzopen %s\n", fname);
        return -1;
    }

    sprintf(line, "<marker_set name=\"dump\">\n");
    gzwrite(zf, line, strlen(line));


    for(size_t kk = 0; kk<nD; kk++)
    {

        double r = 1;
        double g = 0;
        double b = 0;

        if(L != NULL)
        {
            uint8_t chr = L[kk];
            chr = chr % 32; // use 33, 34, ... for the second copy
            // Avoid reading unallocated memory
            if(chr>26)
            {
                printf("Warning: Does not know how to color label %u\n", L[kk]);
                chr = 26;
            }
            r = (double) cmap[3*chr]/255.0;
            g = (double) cmap[3*chr+1]/255.0;
            b = (double) cmap[3*chr+2]/255.0;
        }

        sprintf(line, "<marker id=\"%zu\" x=\"%.3f\" y=\"%.3f\" z=\"%.3f\" r=\"%f\" g=\"%f\" b=\"%f\" radius=\"%f\" />\n",
                kk,
                D[kk*3], D[kk*3+1], D[kk*3+2],
                r, g, b,
                radius);
        gzwrite(zf, line, strlen(line));
    }

    for(size_t kk = 0; kk<NP; kk++)
    {

        size_t A = P[kk*2];
        size_t B = P[kk*2+1];

        double r = .5;
        double g = .5;
        double b = .5;

        if(A==B+1 || A+1==B)
        { // adjacent
            r = .8;
            g = .8;
            b = 0;
        }

        sprintf(line, "<link id1=\"%u\" id2=\"%u\" r=\"%f\" g=\"%f\" b=\"%f\" radius=\"%f\"/>\n",
                P[2*kk], P[2*kk+1],
                r, g, b,
                radius/3);
        gzwrite(zf, line, strlen(line));
    }


    sprintf(line, "</marker_set>\n");
    gzwrite(zf, line, strlen(line));

    gzclose(zf);

    free(line);
    return(0);
}




int cmmwrite(char * fname, double * D, size_t nD, double radius, uint32_t * P, size_t NP, uint8_t * L)
{

    FILE * f = fopen(fname, "w");


    if(f == NULL)
    {
        fprintf(stderr, "Unable to open %s\n", fname);
        return -1;
    }

    fprintf(f, "<marker_set name=\"dump\">\n");


    for(size_t kk = 0; kk<nD; kk++)
    {

        double r = 1;
        double g = 0;
        double b = 0;

        if(L != NULL)
        {
            uint8_t chr = L[kk];
            chr = chr % 32; // use 33, 34, ... for the second copy
            // Avoid reading unallocated memory
            if(chr>26)
            {
                printf("Warning: Does not know how to color label %u\n", L[kk]);
                chr = 26;
            }
            r = (double) cmap[3*chr]/255.0;
            g = (double) cmap[3*chr+1]/255.0;
            b = (double) cmap[3*chr+2]/255.0;
        }


        fprintf(f, "<marker id=\"%zu\" x=\"%.3f\" y=\"%.3f\" z=\"%.3f\" r=\"%f\" g=\"%f\" b=\"%f\" radius=\"%f\" />\n",
                kk,
                D[kk*3], D[kk*3+1], D[kk*3+2],
                r, g, b,
                radius);
    }

    for(size_t kk = 0; kk<NP; kk++)
    {

        size_t A = P[kk*2];
        size_t B = P[kk*2+1];

        double r = .5;
        double g = .5;
        double b = .5;

        if(A==B+1 || A+1==B)
        { // adjacent
            r = .8;
            g = .8;
            b = 0;
        }


        fprintf(f, "<link id1=\"%u\" id2=\"%u\" r=\"%f\" g=\"%f\" b=\"%f\" radius=\"%f\"/>\n",
                P[2*kk], P[2*kk+1],
                r, g, b,
                radius/3);
    }

    fprintf(f, "</marker_set>\n");

    fclose(f);
    return(0);
}
