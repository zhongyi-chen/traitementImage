#include <stdlib.h>
#include <stdio.h>

#include <morphology.h>

void
usage(char* s)
{
  fprintf(stderr,"%s <se> <hs> <ims> <imd>\n",s);
  exit(EXIT_FAILURE);
}

#define PARAM 4
int
main(int argc, char* argv[])
{
  if(argc != PARAM+1) usage(argv[0]);
  int shape = atoi(argv[1]);
  int hs = atoi(argv[2]);
  pnm ims = pnm_load(argv[3]);
  int rows = pnm_get_height(ims);
  int cols = pnm_get_width(ims);
  pnm imd = pnm_new(cols,rows,PnmRawPpm);

  process(shape, hs, ims, imd, maximum);
  pnm_save(imd,PnmRawPpm,argv[4]);
  pnm_free(ims);
  pnm_free(imd);
  return EXIT_SUCCESS;
}
