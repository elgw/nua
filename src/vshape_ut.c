#include "vshape.h"

int main(int argc, char ** argv)
{

    printf("-> tube, r=0.1, l=1.0\n");
    vshape_t * tube = vshape_tube(0.1, 1.0, 13);
    vshape_show(stdout, tube);
    vshape_show_matlab(stdout, tube);
    vshape_free(tube);
    return 0;

    printf("->square\n");
    vshape_t * square = vshape_square();
    vshape_show(stdout, square);
    vshape_free(square);

    printf("->icosahedron\n");
    vshape_t * ico = vshape_icosahedron();
    vshape_show(stdout, ico);
    vshape_show_matlab(stdout, ico);
    uint32_t * E = vshape_edgelist(ico);
    free(E);
    return 0;

    printf("->cube\n");
    vshape_t * cube = vshape_cube();
    vshape_show(stdout, cube);
    vshape_show_matlab(stdout, cube);
    E = vshape_edgelist(cube);
    free(E);

    printf("Dyadic subdivision\n");
    vshape_t * cube2 = vshape_dyadic_subdivision(cube);
    vshape_show_matlab(stdout, cube2);
    vshape_t * cube3 = vshape_dyadic_subdivision(cube2);

    vshape_force_radius(cube3, 1);
    vshape_show_matlab(stdout, cube3);

    vshape_mesh_status(cube, 20, 1);
    vshape_mesh_status(cube2, 20, 1);
    vshape_mesh_status(cube3, 20, 1);
    printf("Optimizing ...\n");
    vshape_optimize(cube3);
    vshape_mesh_status(cube3, 20, 1);

    vshape_free(cube);

    vshape_free(cube2);


    cvshape_t * ccube3 = cvshape_from_vshape(cube3, 1.0, 0.4, 0.0);
    vshape_free( (vshape_t * ) ccube3);

    vshape_free(cube3);



    return EXIT_SUCCESS;
}
