#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <bcl.h>
/**
 * Load an image from parameter ims
 * Extract a portion of ims from pixel(i,j) of dimension row x cols
 * The extracted image will be save in imd 
 **/ 
void usage (char *s);
void process(const size_t i, const size_t j, const size_t rows, const size_t cols, pnm ims, char * filename)
{ 
  if(i > rows)
    usage("Position i must be LESSER than rows\n");
  if(j > cols)
    usage("Position j muse be LESSER than cols\n");
  unsigned int height = pnm_get_height(ims);
  unsigned int width = pnm_get_width(ims);
  if(rows > height)
    usage("Dimension rows must be LESSER than ims's height\n");
  if(cols > width)
    usage("Dimension cols must be LESSER than ims's width\n");

  pnm imd = pnm_new(cols-j, rows-i, PnmRawPpm);
  // unsigned short *data = pnm_get_image(ims);
  unsigned short p;
  for(unsigned int x = i; x < rows; x++){
    for(unsigned int y = j;y< cols;y++){

        p = pnm_get_component(ims, x, y,PnmRed); // get the red component
        pnm_set_component(imd, x-i, y-j, PnmRed, p); // put pixtel to imd 
        
        p = pnm_get_component(ims, x, y, PnmGreen); // get the green component
        pnm_set_component(imd, x-i, y-j, PnmGreen, p);

        p = pnm_get_component(ims, x, y, PnmBlue); // get the blue component
        pnm_set_component(imd, x-i, y-j, PnmBlue, p);
        
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

#define PARAM 6
int 
main(int argc, char *argv[])
{
  if (argc != PARAM+1) usage(argv[0]);

  size_t i = atoi(argv[1]);
  size_t j = atoi(argv[2]);
  size_t rows = atoi(argv[3]);
  size_t cols = atoi(argv[4]);
  //load pnm image
  pnm ims = pnm_load(argv[5]);

  char* filename =argv[6];
  // add pnm as extention to filename
  strcat(filename,".pnm");
  process( i, j, rows, cols, ims, filename);
  pnm_free(ims);

  return EXIT_SUCCESS;
}
