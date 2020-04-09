#include <stdlib.h>
#include <stdio.h>

#include <bcl.h>
#define T 0.25f
void process(int n, char *source, char *filename)
{
  pnm ims = pnm_load(source);
  int rows = pnm_get_height(ims);
  int cols = pnm_get_width(ims);
  pnm imd = pnm_new(cols, rows, PnmRawPpm);
  int size = rows * cols;
  unsigned short *data = pnm_get_channel(ims, NULL, PnmRed);
  float *imsBuffer = calloc(sizeof(float), size);
  float *tmp = calloc(sizeof(float), size);
  for (int i = 0; i < size; i++)
    imsBuffer[i] = data[i];

  for (int c = 0; c < n; c++)
  {

    for (int i = 0; i < rows; i++)
    {
      for (int j = 0; j < cols; j++)
      {
        float pixel_top, pixel_bottom, pixel_left, pixel_right;
        float pixel_center = imsBuffer[i * cols + j];
        pixel_top = i == 0 ? pixel_center : imsBuffer[(i - 1) * cols + j];
        pixel_bottom = i == rows - 1 ? pixel_center : imsBuffer[(i + 1) * cols + j];
        pixel_left = j == 0 ? pixel_center : imsBuffer[i * cols + (j - 1)];
        pixel_right = j == cols - 1 ? pixel_center : imsBuffer[i * cols + (j + 1)];
        float laplacien = pixel_top + pixel_bottom + pixel_left + pixel_right - 4 * pixel_center;
        tmp[i * cols + j] = laplacien;
      }
    }
    for (int s = 0; s < size; s++)
      imsBuffer[s] += T * tmp[s];
  }

  for (int s = 0; s < size; s++)
  {
    float res = imsBuffer[s];
    if (res > 255)
      res = 255.f;
    if (res < 0)
      res = 0.f;
    data[s] = (unsigned short)res;
  }

  pnm_set_channel(imd, data, PnmRed);
  pnm_set_channel(imd, data, PnmGreen);
  pnm_set_channel(imd, data, PnmBlue);

  pnm_save(imd, PnmRawPpm, filename);
  pnm_free(ims);
  pnm_free(imd);
  free(data);
  free(imsBuffer);
  free(tmp);
}

void usage(char *s)
{
  fprintf(stderr, "Usage: %s <n> <ims> <imd>\n", s);
  exit(EXIT_FAILURE);
}

#define PARAM 3
int main(int argc, char *argv[])
{
  if (argc != PARAM + 1)
    usage(argv[0]);
  int n = atoi(argv[1]);
  process(n, argv[2], argv[3]);
  return EXIT_SUCCESS;
}
