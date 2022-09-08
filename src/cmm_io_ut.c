#include "cmm_io.h"

void show_usage(char * prog_name)
{
    printf("Usage:\n");
    printf("%s file.cmm\n", prog_name);
    return;
}

void read_test(char * file)
{
    printf("Reading %s\n", file);
    cmm_data_t * cmm = cmm_read(file);
    if(cmm->n_markers < 40 && cmm->n_links < 40)
    {
        cmm_print(stdout, cmm);
    } else {
        printf("Too many links or markers, not showing anything\n");
    }
    cmm_free(cmm);
    return;
}

int cmm_marker_compare(cmm_marker_t A, cmm_marker_t B)
{
    int different = 0;
    if(memcmp(&A, &B, sizeof(cmm_marker_t) != 0))
    {
        printf("Markers differ:\n");
        cmm_marker_print(stdout, A);
        cmm_marker_print(stdout, B);
        different = 1;
    }
    return different;
}

int cmm_link_compare(cmm_link_t A, cmm_link_t B)
{
    int different = 0;
    if(memcmp(&A, &B, sizeof(cmm_link_t)) != 0)
    {
        printf("Links differ:\n");
        cmm_link_print(stdout, A);
        cmm_link_print(stdout, B);
        different = 1;
    }
    return different;
}

void cmm_compare(const cmm_data_t * A, const cmm_data_t * B)
{
    printf("-> cmm_compare\n");
    assert(A != NULL);
    assert(A != NULL);
    int cmp_markers = 1;
    int cmp_links = 1;
    if(A->n_markers != B->n_markers)
    {
        printf("! Different number of markers in the cmm objects\n"
               "  Will not compare markers\n");

        cmp_markers = 0;
    }
    if(B->n_links != B->n_links)
    {
        printf("! Different number of links in the cmm objects\n"
               "  Will not compare links\n");
        cmp_links = 0;
    }

    int different = 0;
    if(cmp_markers == 1)
    {
        for(uint32_t mm = 0; mm < A->n_markers; mm++)
        {
            different += cmm_marker_compare(A->markers[mm], B->markers[mm]);
        }
    }

    if(cmp_links == 1)
    {
        for(uint32_t ll = 0; ll < A->n_links; ll++)
        {
            different += cmm_link_compare(A->links[ll], B->links[ll]);
        }
    }
    if(different == 0)
    {
        printf("compared cmm objects are identical\n");
    } else {
        printf("! compared cmm objects are different\n");
    }
    printf("// cmm_compare\n");
    return;
}

void read_write_read_test(char * file)
{
    cmm_data_t * cmm = cmm_read(file);
    char template[] = "/tmp/cmm_io_ut_XXXXXX";
    if( mkstemp(template) == -1)
    {
        printf("Failed to find a temporary file name\n");
        return;
    }
    cmm_write(cmm, template);
    //printf("Temporary file: %s\n", template);
    cmm_data_t * cmm2 = cmm_read(template);
    remove(template);

    cmm_compare(cmm, cmm2);

    cmm_free(cmm);
    cmm_free(cmm2);
    return;

}

int main(int argc, char ** argv)
{
    if(argc == 1)
    {
        show_usage(argv[0]);
    }

    if(argc == 2)
    {
        printf("Trying to read %s\n", argv[1]);
        read_test(argv[1]);
        printf("Reading %s, writing and reading again to compare\n", argv[1]);
        read_write_read_test(argv[1]);
    }

    return EXIT_SUCCESS;
}
