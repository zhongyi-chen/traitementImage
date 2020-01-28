#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <bcl.h>
/**
 * Load an image from parameter ims
 * Extract the \param num channel from ims
 * The result will be save as gray level in imd file
 **/ 
void usage (char *s);
void process(const size_t num,  pnm ims, char * filename)
{ 
    if(num >2)
        usage("Invalide channel\n");
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    pnm imd = pnm_new(cols, rows, PnmRawPpm);

    unsigned short * data = pnm_get_channel(ims,NULL,num);
    for(unsigned int i = PnmRed;i<=PnmBlue;i++){
            pnm_set_channel(imd,data,i);
    }
    pnm_save(imd, PnmRawPpm, filename);
    free(data);
    pnm_free(imd);

}
void 
usage (char *s)
{
  fprintf(stderr,"Usage: %s %s", s, "<i> <j> <rows> <cols> <ims> <imd>\n");
  exit(EXIT_FAILURE);
}

#define PARAM 3
int 
main(int argc, char *argv[])
{
  if (argc != PARAM+1) usage(argv[0]);

  size_t num = atoi(argv[1]);
  //load pnm image
  pnm ims = pnm_load(argv[2]);

  char* filename =argv[3];
  // add pnm as extention to filename
  strcat(filename,".pnm");
  process(num, ims, filename);
  pnm_free(ims);

  return EXIT_SUCCESS;
}
