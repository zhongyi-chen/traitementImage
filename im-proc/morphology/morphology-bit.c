#include <math.h>
#include <bcl.h>
#include <stdlib.h>
#include <stdio.h>
#include <se.h>

unsigned long long dec2intertwinedBinaryRGB(int r, int g, int b)
{
  int **binaryRGB = malloc(sizeof(int *) * 3);
  for (int i = 0; i < 3; i++)
  {
    binaryRGB[i] = calloc(sizeof(int), 8);
  }
  int rgb[3] = {r, g, b};
  for (int i = 0; i < 3; i++)
  {
    int n = rgb[i];
    int k = 0;
    while (n > 0)
    {
      // storing remainder in binary array
      binaryRGB[i][k] = n % 2;
      n = n / 2;
      k++;
    }
  }
  unsigned long long bit = 0;
  for (int p = 23; p >= 2; p = p - 3)
  {
    unsigned long long rbit = binaryRGB[0][(p - 2) / 3];
    unsigned long long gbit = binaryRGB[1][(p - 2) / 3];
    unsigned long long bbit = binaryRGB[2][(p - 2) / 3];
    bit = bit | rbit << p | gbit << (p - 1) | bbit << (p - 2);
  }
  for (int i = 0; i < 3; i++)
  {
    free(binaryRGB[i]);
  }
  free(binaryRGB);
  return bit;
}

void maximum(unsigned short *val, unsigned short *max)
{
  int new_r = val[PnmRed];
  int new_g = val[PnmGreen];
  int new_b = val[PnmBlue];  
  int origin_r = max[PnmRed];
  int origin_g = max[PnmGreen];
  int origin_b = max[PnmBlue];
  unsigned long long newBit = dec2intertwinedBinaryRGB(new_r,new_g,new_b);
  unsigned long long originBit = dec2intertwinedBinaryRGB(origin_r,origin_g,origin_b);
  if (newBit > originBit){
    max[PnmRed] = new_r;
    max[PnmGreen] = new_g;
    max[PnmBlue] = new_b;
  }  
}

void minimum(unsigned short *val, unsigned short *min)
{
  int new_r = val[PnmRed];
  int new_g = val[PnmGreen];
  int new_b = val[PnmBlue];
  int origin_r = min[PnmRed];
  int origin_g = min[PnmGreen];
  int origin_b = min[PnmBlue];
  unsigned long long newBit = dec2intertwinedBinaryRGB(new_r, new_g, new_b);
  unsigned long long originBit = dec2intertwinedBinaryRGB(origin_r, origin_g, origin_b);
  if (newBit < originBit)
  {
    min[PnmRed] = new_r;
    min[PnmGreen] = new_g;
    min[PnmBlue] = new_b;
  }
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
      unsigned short origin[3];
      for (int k = 0; k < 3; k++)
      {
        origin[k] = pnm_get_component(ims, i, j, k);
      }
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
              pf(newPixel, origin);
            }
          }
        }
      }
      pnm_set_component(imd, i, j, PnmRed, origin[PnmRed]);
      pnm_set_component(imd, i, j, PnmGreen, origin[PnmGreen]);
      pnm_set_component(imd, i, j, PnmBlue, origin[PnmBlue]);
    }
  }
  pnm_free(shape);
}
