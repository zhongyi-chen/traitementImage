#include <stdlib.h>
#include <stdio.h>

#include<bcl.h>

void
usage(char* s)
{
  fprintf(stderr,"%s <se> <hs> <ims> <imd>\n",s);
  exit(EXIT_FAILURE);
}

void process(pnm ims1, pnm ims2, pnm imd,int rows,int cols,char * filename){
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      for (int k = 0; k < 3; k++)
      {
        unsigned short color1 = pnm_get_component(ims1,i,j,k);
        unsigned short color2 = pnm_get_component(ims2,i,j,k);
        unsigned short final = (color1 + color2) > 255 ? 255 : (color1 + color2);
        pnm_set_component(imd, i, j, k, final);
      }
    }
  }
  pnm_save(imd,PnmRawPpm,filename);
  
}

#define PARAM 3
int
main(int argc, char* argv[])
{
  if(argc != PARAM+1) usage(argv[0]);
  pnm ims1 = pnm_load(argv[1]);
  pnm ims2 = pnm_load(argv[2]);
  int rows = pnm_get_height(ims1);
  int cols = pnm_get_width(ims1);
  pnm imd = pnm_new(cols,rows,PnmRawPpm);
  process(ims1,ims2,imd,rows,cols,argv[3]);
  pnm_free(ims1);
  pnm_free(ims2);
  pnm_free(imd);
  return EXIT_SUCCESS;
}
