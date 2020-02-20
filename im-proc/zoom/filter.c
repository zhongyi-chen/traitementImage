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


float tent(float x){return (-1.f <= x && x<=1.f) ? 1.f-fabsf(x) : 0.f;}

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


pnm flip(pnm source, int cols, int rows,bool clock){
    pnm imd = pnm_new(rows,cols,PnmRawPpm);
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            for(int c = PnmRed;c<=PnmBlue;c++){
                unsigned short color = pnm_get_component(source,i,j,c);
                (!clock) ?  pnm_set_component(imd,cols-j-1,i,c,color):  pnm_set_component(imd,j,rows-i-1,c,color);
            }
        }

    }
    return imd;
}



void interpolation(pnm source, pnm dst, int input_rows, int input_cols,int interpolation, char * filter_name, int factor){

    float filter_domain = get_filter_domain(filter_name);

    for(int i = 0; i<input_rows; i++){
        for(int j =0; j<interpolation; j++){
            float newj = j/(float) factor;
            int left = floor(newj -filter_domain);
            left = left>0 ? left : 0;
            int right = floor(newj + filter_domain);
            right = right <= interpolation-1 ? right : interpolation-1;

          
            float S[3]= {0,0,0};
            for(int k = left; k<=right;k++){
                int tmpj = (k>=input_cols) ? input_cols-1: k;
                S[PnmRed]+=pnm_get_component(source,i,tmpj,PnmRed) * calcul_filter(filter_name, k-newj);
                S[PnmGreen]+=pnm_get_component(source,i,tmpj,PnmGreen) * calcul_filter(filter_name, k-newj);
                S[PnmBlue]+=pnm_get_component(source,i,tmpj,PnmBlue) * calcul_filter(filter_name, k-newj);
            }

            for(int channel = PnmRed; channel<=PnmBlue;channel ++){
                pnm_set_component(dst,i,j,channel,(unsigned short)S[channel]);
            }
        }
    }
}

void process(int factor, char * filter_name, pnm ims, char * filename)
{
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);

    int output_rows = rows * factor;
    int output_cols = cols * factor;

    pnm buffer = pnm_new(output_cols, output_rows, PnmRawPpm);
    pnm imd = pnm_new(output_rows,output_cols , PnmRawPpm);

    interpolation(ims,buffer,rows,cols,output_cols,filter_name,factor);
    pnm fliped_buffer = flip(buffer,output_cols,output_rows,false);
    interpolation(fliped_buffer,imd,output_cols,rows,output_rows,filter_name,factor);

    pnm final = flip(imd,output_rows,output_cols,true);

    pnm_save(final, PnmRawPpm, filename);
    pnm_free(imd);
    pnm_free(buffer);
    pnm_free(fliped_buffer);
    pnm_free(final);
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
