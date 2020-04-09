#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <bcl.h>
#define T 0.25f
static int fun = -1;
static int lambda = -1;
void usage(char *s)
{
  fprintf(stderr, "Usage: %s <n> <ims> <imd>\n", s);
  exit(EXIT_FAILURE);
}
float funC(float s)
{
  switch (fun)
  {
  case 0:
    return 1.f;
  case 1:
    return 1.f / (1.f + (s / (float)lambda) * (s / (float)lambda));
  case 2:
    return expf(-(s / (float)lambda) * (s / (float)lambda));

  default:
    usage("Input arguement function is invalid\n");
    return -1;
    break;
  }
}
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
  float *gradient_1 = calloc(sizeof(float), size);
  float *gradient_2 = calloc(sizeof(float), size);
  for (int i = 0; i < size; i++)
    imsBuffer[i] = data[i];

  for (int c = 0; c < n; c++)
  {

    for (int i = 0; i < rows; i++)
    {
      for (int j = 0; j < cols; j++)
      {
        float pixel_bottom, pixel_right;
        int index = i * cols + j;
        float pixel_center = imsBuffer[index];
        pixel_bottom = i == rows - 1 ? pixel_center : imsBuffer[(i + 1) * cols + j];
        pixel_right = j == cols - 1 ? pixel_center : imsBuffer[i * cols + (j + 1)];

        float d1 = pixel_bottom - pixel_center;
        float d2 = pixel_right - pixel_center;
        float norm = sqrtf(d1 * d1 + d2 * d2);
        float c = funC(norm);
        gradient_1[index] = d1 * c;
        gradient_2[index] = d2 * c;
      }
    }
    for (int i = 0; i < rows; i++)
    {
      for (int j = 0; j < cols; j++)
      {
        int index = i * cols + j;
        float p1 = i==0 ? 0 : gradient_1[index] - gradient_1[(i-1)*cols +j];
        float p2 = j==0 ? 0 : gradient_2[index] - gradient_2[i*cols+j-1];
        tmp[i * cols + j] = p1 + p2;
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

#define PARAM 5
int main(int argc, char *argv[])
{
  if (argc != PARAM + 1)
    usage(argv[0]);
  int n = atoi(argv[1]);
  lambda = atoi(argv[2]);
  fun = atoi(argv[3]);
  process(n, argv[4], argv[5]);
  return EXIT_SUCCESS;
}
