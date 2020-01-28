#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <bcl.h>
/**
 * Convert an image to an image where every pixel normalized
 * \param min pixel's minimum value
 * \param min pixel's maximum value
 * \param ims image to be convert
 * \param filename 
 * 
 **/
void usage(char *s);
typedef struct s_maxmin
{
    unsigned short max;
    unsigned short min;
}s_maxmin;

s_maxmin getMaxMin(const unsigned width, const unsigned height, pnm ims, pnmChannel channel)
{
    // set defaut max min value
    s_maxmin maxmin= {pnm_get_component(ims, 0, 0, channel), pnm_get_component(ims, 0, 0, channel)};
    for (unsigned int i = 0; i < height; i++)
    {
        for (unsigned int j = 0; j < width; j++)
        {
            unsigned short color = pnm_get_component(ims, i, j, channel);
            if (color > maxmin.max)
                maxmin.max = color;
            if (color < maxmin.min)
                maxmin.min = color;
        }
    }
    return maxmin;
}
void process(const unsigned int min, const unsigned int max, pnm ims, char *filename)
{
    if (min > 255 || max > 255)
        usage("Min or max out of range!\n");

    unsigned int width = pnm_get_width(ims);
    unsigned int height = pnm_get_height(ims);

    pnm imd = pnm_new(width, height, PnmRawPpm);

    for (int c = PnmRed; c <= PnmBlue; c++)
    {
        s_maxmin maxmin = getMaxMin(width, height, ims, c);
        unsigned short Max = maxmin.max;
        unsigned short Min = maxmin.min;
        printf("max is %d min is %d\n",Max,Min);
        for (unsigned int i = 0; i < height; i++)
        {
            for (unsigned int j = 0; j < width; j++)
            {
                unsigned short origin_color = pnm_get_component(ims,i,j,c);
                float new_color = ((max-min)/(float)(Max -Min)) * origin_color + (min*Max-max*Min) / (float)(Max-Min); 
                pnm_set_component(imd,i,j,c,new_color);
            }
        }
    }

    pnm_save(imd, PnmRawPpm, filename);

    pnm_free(imd);
}
void usage(char *s)
{
    fprintf(stderr, "Usage: %s %s", s, "<i> <j> <rows> <cols> <ims> <imd>\n");
    exit(EXIT_FAILURE);
}

#define PARAM 4
int main(int argc, char *argv[])
{
    if (argc != PARAM + 1)
        usage(argv[0]);
    unsigned int min = atoi(argv[1]);
    unsigned int max = atoi(argv[2]);
    //load pnm images
    pnm ims = pnm_load(argv[3]);

    char *filename = argv[4];
    // add pnm as extention to filename
    strcat(filename, ".pnm");
    process(min, max, ims, filename);

    pnm_free(ims);

    return EXIT_SUCCESS;
}
