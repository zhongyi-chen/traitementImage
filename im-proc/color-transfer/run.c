#include <stdlib.h>

int main()
{
    system("./color-transfer ../data/ocean.ppm ../data/forest.ppm ocean_forest.ppm");
    system("./color-transfer ../data/flower.ppm ../data/mountain.ppm flower_moutain.ppm");
    system("pvisu *.ppm");
    exit (EXIT_SUCCESS);
}
