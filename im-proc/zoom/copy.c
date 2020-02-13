/**
 * @file copy.c
 * @brief zoom the input image @param ims by factor of @param f
 * @param ims the input image
 * @param f the zoom factor
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <bcl.h>
#include <string.h>
#include <stdbool.h>
void usage(const char *s)
{
    fprintf(stderr, "Usage: %s <ims> \n", s);
    exit(EXIT_FAILURE);
}


/**
 * 
 * 
 * 
 **/

void process(int factor, pnm ims, char * filename)
{
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);

    int output_rows = rows * factor;
    int output_cols = cols * factor;
    pnm imd = pnm_new(output_cols, output_rows, PnmRawPpm);


    for (int i = 0; i < output_rows; i++)
    {
        for (int j = 0; j < output_cols; j++)
        { 
            for(int c = PnmRed; c<=PnmBlue;c++){
                unsigned short data =  pnm_get_component(ims,i/factor,j/factor,c);
                pnm_set_component(imd,i,j,c,data);
            }
        }
    }
    pnm_save(imd, PnmRawPpm, filename);
    free(imd);
}

#define PARAM 3
int main(int argc, char *argv[])
{
    if (argc != PARAM + 1)
        usage(argv[0]);

    //load pnm images
    int factor = atoi(argv[1]);
    pnm ims = pnm_load(argv[2]);
    char * filename = argv[3];
    process(factor, ims,filename);

    pnm_free(ims);
    return EXIT_SUCCESS;
}
