#include <stdlib.h>
#include <stdio.h>

#include<bcl.h>

void
usage(char* s)
{
  fprintf(stderr,"%s <se> <hs> <ims> <imd>\n",s);
  exit(EXIT_FAILURE);
}

void process(int min, int max, pnm ims, pnm imd, int rows, int cols,char * filename){
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      for (int k = 0; k < 3; k++)
      {
        unsigned short color = pnm_get_component(ims,i,j,k);
        unsigned short final = (color>=min && color<=max) ?255 : 0;
        pnm_set_component(imd, i, j, k, final);
      }
    }
  }
  pnm_save(imd,PnmRawPpm,filename);
  
}

#define PARAM 4
int
main(int argc, char* argv[])
{
  if(argc != PARAM+1) usage(argv[0]);
  int min = atoi(argv[1]);
  int max = atoi(argv[2]);
  pnm ims = pnm_load(argv[3]);
  int rows = pnm_get_height(ims);
  int cols = pnm_get_width(ims);
  pnm imd = pnm_new(cols,rows,PnmRawPpm);
  process(min,max,ims,imd,rows,cols,argv[4]);
  pnm_free(ims);
  pnm_free(imd);
  return EXIT_SUCCESS;
}
