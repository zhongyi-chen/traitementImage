#include <math.h>
#include <bcl.h>
#include <stdlib.h>
#include <stdio.h>
#include <se.h>

void maximum(unsigned int *val, unsigned int *max)
{
  if (*val > *max)
    *max = *val;
}

void minimum(unsigned int *val, unsigned int *min)
{
  if (*val < *min)
    *min = *val;
}

void process(int s,
             int hs,
             pnm ims,
             pnm imd,
             void (*pf)(unsigned int *, unsigned int *))
{
  pnm shape = se(s, hs);
  if (shape == NULL)
  {
    fprintf(stderr, "shape is null\n");
    exit(EXIT_FAILURE);
  }
  int rows = pnm_get_height(ims);
  int cols = pnm_get_width(ims);
  int size = 2 * hs + 1;
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      unsigned short origin[3];
      for (int k = 0; k < 3; k++)
      {
        origin[k] = pnm_get_component(ims, i, j, k);
      }
      unsigned int originC;
      originC = origin[PnmRed] * pow(10, 6) + origin[PnmGreen] * pow(10, 3) + origin[PnmBlue];
      for (int shape_i = 0; shape_i < size; shape_i++)
      {
        for (int shape_j = 0; shape_j < size; ++shape_j)
        {
          if (pnm_get_component(shape, shape_i, shape_j, 0) == 255)
          {
            //center sharp with origin
            int new_i = i + shape_i - hs;
            int new_j = j + shape_j - hs;
            if (new_i >= 0 && new_i < rows && new_j >= 0 && new_j < cols)
            {
              unsigned short newPixel[3];
              for (int k = 0; k < 3; k++)
              {
                newPixel[k] = pnm_get_component(ims, new_i, new_j, k);
              }
              unsigned int newPixelC = newPixel[PnmRed] * pow(10, 6) + newPixel[PnmGreen] * pow(10, 3) + newPixel[PnmBlue];
              pf(&newPixelC, &originC);
              
            }
          }
        }
        
      }
      unsigned short r = originC / pow(10, 6);
      unsigned short g = (originC - r * pow(10, 6)) / pow(10, 3);
      unsigned short b = originC - r * pow(10, 6) - g * pow(10, 3);
      pnm_set_component(imd, i, j, PnmRed, r);
      pnm_set_component(imd, i, j, PnmGreen, g);
      pnm_set_component(imd, i, j, PnmBlue, b);
    }
  }
  pnm_free(shape);
}
