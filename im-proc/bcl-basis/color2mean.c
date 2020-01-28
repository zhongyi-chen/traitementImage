#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <bcl.h>
/**
 * Convert an image to an image where every pixel is the mean of the channels RGB
 * \param ims image to be convert
 * \param filename 
 * 
 **/ 
void usage (char *s);
void process(pnm ims, char * filename)
{ 
    unsigned int width = pnm_get_width(ims);
    unsigned int height = pnm_get_height(ims);

    pnm imd = pnm_new(width, height, PnmRawPpm);

    
    for(unsigned int i=0; i < height; i++){
        for(unsigned int j=0; j<width;j++){
            unsigned int mean = (pnm_get_component(ims,i,j,PnmRed) + 
                                pnm_get_component(ims,i,j,PnmGreen) + 
                                pnm_get_component(ims,i,j,PnmBlue)) / 3;
            for(int c =PnmRed; c<=PnmBlue;c++){
                pnm_set_component(imd,i,j,c,mean);
            }
        }
    }

    pnm_save(imd, PnmRawPpm, filename);

    pnm_free(imd);

}
void 
usage (char *s)
{
  fprintf(stderr,"Usage: %s %s", s, "<i> <j> <rows> <cols> <ims> <imd>\n");
  exit(EXIT_FAILURE);
}

#define PARAM 2
int 
main(int argc, char *argv[])
{
  if (argc != PARAM+1) usage(argv[0]);

  //load pnm images
  pnm ims = pnm_load(argv[1]);

  char* filename =argv[2];
  // add pnm as extention to filename
  strcat(filename,".pnm");
  process(ims, filename);

  pnm_free(ims);

  return EXIT_SUCCESS;
}
