#include <stdlib.h>

int main()
{
    system("./colorization ../data/tree-c.ppm ../data/tree-g.ppm tree.ppm");
    system("./colorization ../data/insect-c.ppm ../data/insect-g.ppm insect.ppm");
    system("./colorization ../data/sat-c.ppm ../data/sat-g.ppm sat.ppm");
    system("pvisu *.ppm");
    exit (EXIT_SUCCESS);

}
