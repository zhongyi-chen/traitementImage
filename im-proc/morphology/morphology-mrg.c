#include <math.h>
#include<bcl.h>
#include <stdlib.h>
#include <stdio.h>
#include <se.h>

void maximum(unsigned short *val, unsigned short *max)
{
  if (*val > *max)
    *max = *val;
}

void minimum(unsigned short *val, unsigned short *min)
{
  if (*val < *min)
    *min = *val;
}

void process(int s,
             int hs,
             pnm ims,
             pnm imd,
             void (*pf)(unsigned short *, unsigned short *))
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
      for (int k = 0; k < 3; k++)
      {
        unsigned short origin = pnm_get_component(ims, i, j, k);
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
                unsigned short newPixel = pnm_get_component(ims, new_i, new_j, k);
                pf(&newPixel, &origin);
              }
            }
          }
        }
        pnm_set_component(imd, i, j, k, origin);
      }
    }
  }
  pnm_free(shape);
}
