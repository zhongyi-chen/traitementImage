/**
 * @file filter.c
 * @brief zoom the input image @param ims by factor of @param f by using interpolation filter
 * @param factor the zoom factor
 * @param filter_name 
 * @param ims the input image
 * @param imd output image
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <bcl.h>
#include <string.h>
#include <stdbool.h>
#include <fft.h>

void usage(const char *s)
{
    fprintf(stderr, "Usage: %s <ims> \n", s);
    exit(EXIT_FAILURE);
}

float box(float x){return (x>=-0.5 && x<0.5) ? 1 : 0;}


float tent(float x){return (-1 <= x && x<=1) ? 1.f-fabs(x) : 0.f;}

float bell(float x){
    float absx = fabs(x);
    if(absx<=0.5f)
        return -(x*x) + 0.75f;
    else if(0.5<absx && absx <= 1.5f)
        return 0.5 * pow((absx - 1.5f),2);
    return 0.f;
}

float mitch(float x){
    float absx = fabs(x);
    if(-1<=x  && x <=1)
        return (7.f / 6.f) * powf(absx,3) - 2 * pow(x,2) + 8.f/9.f;
    else if ((-2<=x && x<= -1) || (1<=x && x<=2))
        return (-7.f / 18.f) * powf(absx,3) + 2* pow(x,2)
                - (10.f/3.f) *absx + (16.f /9.f);
    return 0.f;
}


float calcul_filter(char * name,float x){
    if(!strcmp(name,"box"))
        return box(x);
    else if(!strcmp(name,"tent"))
        return tent(x);
    else if(!strcmp(name,"bell"))
        return bell(x);
    else if(!strcmp(name,"mitch"))
        return mitch(x);
    else{
        usage("Unknown filter\n");
        exit(EXIT_FAILURE);
    }
        
}

float get_filter_domain(char *name){
    if(!strcmp(name,"box"))
        return 0.5f;
    else if(!strcmp(name,"tent"))
        return 1.f;
    else if(!strcmp(name,"bell"))
        return 1.5f;
    else if(!strcmp(name,"mitch"))
        return 2.f;
    else{
        usage("Unknown filter\n");
        exit(EXIT_FAILURE);
    }
}



void process(int factor, char * filter_name, pnm ims, char * filename)
{
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);

    int output_rows = rows * factor;
    int output_cols = cols * factor;
    int output_size = output_rows * output_cols;

    pnm imd = pnm_new(output_cols, output_rows, PnmRawPpm);
    unsigned short * data = pnm_get_channel(ims,NULL,PnmRed);

    unsigned short * output_data = malloc(output_size *  sizeof(unsigned short));

    // interpolation filter in cols 
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < output_cols-1; j++)
        {
            float newj = j/(float)factor;
            float left = fabs(newj-get_filter_domain(filter_name));
            left = left>0 ? left : 0;
            float right = fabs(newj + get_filter_domain(filter_name));
            right = right < output_cols-1 ? right : output_cols-1;
            
            float S = 0;
            for (int k = left; k <=right; k++)
            {
                
                S+=data[ i*cols + k] * calcul_filter(filter_name, k-newj);
            }
            // printf("S in cols is %d\n",(unsigned short)S);
            output_data[i*output_cols+j] = (unsigned short) S; 
        } 
    }

    // interpolation filter in rows 
    for (int j = 0; j < output_cols; j++)
    {
        for (int i = 0; i < output_rows-1; i++)
        {
            float newi = i/(float)factor;
            float left = fabs(newi-get_filter_domain(filter_name));
            left = left>0 ? left : 0;
            float right = fabs(newi + get_filter_domain(filter_name));
            printf("right is %f\n",floor(right));
            right = right < output_rows-1 ? right : output_rows-1;
            float S = 0;
            printf("right is %f\n",floor(right));
            for (int k = left; k <=right; k++)
            {
                int tmpi = k;
                if (k>=rows) 
                    tmpi=rows-1;
                int tmpj = j/factor;
                S+= pnm_get_component(ims,tmpi,tmpj,0) * calcul_filter(filter_name, k-newi);
            }
            output_data[i*output_cols+j] = S; 
        } 
    }

    

    for (int c = PnmRed; c <=PnmBlue; c++)
    {
        pnm_set_channel(imd, output_data, c);
    }

    pnm_save(imd, PnmRawPpm, filename);
    pnm_free(imd);
    free(data);
    free(output_data);
}

#define PARAM 4
int main(int argc, char *argv[])
{
    if (argc != PARAM + 1)
        usage(argv[0]);

    //load pnm images
    int factor = atoi(argv[1]);
    if(factor<=1)
        usage(argv[1]);
    char * filter_name = argv[2];
    pnm ims = pnm_load(argv[3]);
    char * filename = argv[4];
    process(factor, filter_name,ims, filename);

    pnm_free(ims);
    return EXIT_SUCCESS;
}
