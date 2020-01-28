#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <bcl.h>
/**
 * Generate an image from three gray level images 
 * Order of images is important! 
 * \param ims0 gray level generated from red channel
 * \param ims1 gray level generated from green channel
 * \param ims2 gray level generated from blue channel
 * 
 * we want the dimension of three images are the same
 **/ 
void usage (char *s);
void process(pnm ims0, pnm ims1, pnm ims2, char * filename)
{ 
    unsigned int width0 = pnm_get_width(ims0);
    unsigned int width1 = pnm_get_width(ims1);
    unsigned int width2 = pnm_get_width(ims2);

    unsigned int height0 = pnm_get_height(ims0);
    unsigned int height1 = pnm_get_height(ims1);
    unsigned int height2 = pnm_get_height(ims2);

    if(!(width0==width1 && width1==width2))
        usage("Width of images are not the same\n");
    if(!(height0==height1 && height1==height2))
        usage("Height of images are not the same\n");

    pnm imd = pnm_new(width0, height0, PnmRawPpm);

    unsigned short * data0 = pnm_get_channel(ims0,NULL,PnmRed);
    unsigned short * data1 = pnm_get_channel(ims1,NULL,PnmGreen);
    unsigned short * data2 = pnm_get_channel(ims2,NULL,PnmBlue);


    pnm_set_channel(imd,data0,PnmRed);
    pnm_set_channel(imd,data1,PnmGreen);
    pnm_set_channel(imd,data2,PnmBlue);
    
    pnm_save(imd, PnmRawPpm, filename);
    free(data0);
    free(data1);
    free(data2);
    pnm_free(imd);

}
void 
usage (char *s)
{
  fprintf(stderr,"Usage: %s %s", s, "<i> <j> <rows> <cols> <ims> <imd>\n");
  exit(EXIT_FAILURE);
}

#define PARAM 4
int 
main(int argc, char *argv[])
{
  if (argc != PARAM+1) usage(argv[0]);

  //load pnm images
  pnm ims0 = pnm_load(argv[1]);
  pnm ims1 = pnm_load(argv[2]);
  pnm ims2 = pnm_load(argv[3]);

  char* filename =argv[4];
  // add pnm as extention to filename
  strcat(filename,".pnm");
  process(ims0, ims1, ims2, filename);
  pnm_free(ims0);
  pnm_free(ims1);
  pnm_free(ims2);

  return EXIT_SUCCESS;
}
