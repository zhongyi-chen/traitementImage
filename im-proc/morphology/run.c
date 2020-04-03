#include <stdlib.h>

int main()
{
    system("./dilation 2 10 ../data/lena-gray.ppm dilatation_2_10.ppm");
    system("./dilation 7 10 ../data/lena-gray.ppm dilatation_7_10.ppm");
    system("./erosion 2 10 ../data/lena-gray.ppm erosion_2_10.ppm");
    system("./erosion 7 10 ../data/lena-gray.ppm erosion_7_10.ppm");
    system("pvisu *.ppm");
    exit(EXIT_SUCCESS);
}
