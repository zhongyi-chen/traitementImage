#include <stdlib.h>

int main()
{
    // system("./dilation 2 10 ../data/lena-gray.ppm dilatation_2_10.ppm");
    // system("./dilation 7 10 ../data/lena-gray.ppm dilatation_7_10.ppm");
    // system("./erosion 2 10 ../data/lena-gray.ppm erosion_2_10.ppm");
    // system("./erosion 7 10 ../data/lena-gray.ppm erosion_7_10.ppm");
    // system("./opening 2 10 ../data/lena-gray.ppm opening_2_10.ppm");
    // system("./opening 7 10 ../data/lena-gray.ppm opening_7_10.ppm");
    // system("./closing 2 10 ../data/lena-gray.ppm closing_2_10.ppm");
    // system("./closing 7 10 ../data/lena-gray.ppm closing_7_10.ppm");

    system("./dilation 2 2 ../data/lena-gray.ppm a.ppm");
    system("./erosion 2 2 ../data/lena-gray.ppm b.ppm");
    system("./subtract a.ppm b.ppm c.ppm");

    system("./add a.ppm b.ppm d.ppm");
    system("./subtract d.ppm ../data/lena-gray.ppm e.ppm");
    system("./subtract e.ppm ../data/lena-gray.ppm f.ppm");
    system("pvisu *.ppm");
    exit(EXIT_SUCCESS);
}
