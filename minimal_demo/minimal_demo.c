#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "libnua/nua.h"

int main(int argc, char ** argv)
{
    nua_t * nua = nua_new();
    nua_run(nua);
    nua_destroy(nua);
    return EXIT_SUCCESS;
}
