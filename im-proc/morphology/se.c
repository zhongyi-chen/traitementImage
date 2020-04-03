#include <stdlib.h>
#include <math.h>

#include <bcl.h>
#include <se.h>

enum {SQUARE, DIAMOND, DISK, LINE_V, DIAG_R, LINE_H, DIAG_L, CROSS, PLUS};

pnm
se(int s, int hs){  
  int size = 2 * hs + 1;
  pnm imd = pnm_new(size,size,PnmRawPpm);

  switch (s)
  {
  case SQUARE:
    for (int i = 0; i < size; i++)
      for (int j = 0; j < size; j++)
        for (int k = 0; k < 3; k++)
          pnm_set_component(imd,i,j,k,255);
    break;
  case DIAMOND:
    for (int i = 0; i < size; i++)
    {
      int interval = (i<=hs) ? i : size-i;
      int start = hs-interval;
      int end = hs+ interval;
      for(int j = start; j<end;j++)
        for(int k =0; k<3; k++)
          pnm_set_component(imd,i,j,k,255);
    }
    break;
  case DISK:
    for (int i = 0; i < size; i++)
      for (int j = 0; j < size; j++){
        int d = sqrt((i-hs)*(i-hs)+(j-hs)*(j-hs));
        if(d<hs)
          for (int k = 0; k < 3; k++)
            pnm_set_component(imd,i,j,k,255);
      }
    break;
  case LINE_V:
    for (int i = 0; i < size; i++)
      for (int k = 0; k < 3; k++)
        pnm_set_component(imd,i,hs,k,255);
    break;
  case DIAG_R:
    for (int i = 0; i < size; i++)
      for (int j = 0; j < size; j++){
        if (i==size-j)
          for (int k = 0; k < 3; k++)
            pnm_set_component(imd, i, j, k, 255);
      }
    break;
  case LINE_H:
    for (int j = 0; j < size; j++)
      for (int k = 0; k < 3; k++)
        pnm_set_component(imd, hs, j, k, 255);
    break;
  case DIAG_L:
    for (int i = 0; i < size; i++)
      for (int j = 0; j < size; j++)
      {
        if (i ==  j)
          for (int k = 0; k < 3; k++)
            pnm_set_component(imd, i, j, k, 255);
      }
    break;
  case CROSS:
    for (int i = 0; i < size; i++)
      for (int j = 0; j < size; j++)
      {
        if (i == size - j)
          for (int k = 0; k < 3; k++)
            pnm_set_component(imd, i, j, k, 255);
      }
    for (int i = 0; i < size; i++)
      for (int j = 0; j < size; j++)
      {
        if (i == j)
          for (int k = 0; k < 3; k++)
            pnm_set_component(imd, i, j, k, 255);
      }
    break;
  case PLUS:
    for (int i = 0; i < size; i++)
      for (int k = 0; k < 3; k++)
        pnm_set_component(imd, i, hs, k, 255);
    for (int j = 0; j < size; j++)
      for (int k = 0; k < 3; k++)
        pnm_set_component(imd, hs, j, k, 255);
    break;
  
  default:
    break;
  }

  return imd;
}
