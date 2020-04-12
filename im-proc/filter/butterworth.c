#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <bcl.h>
#include <fft.h>

double dist(int u, int v)
{
  return sqrt(u * u + v * v);
}
float getMax(float *a, int size)
{
  float max = a[0];
  for (int i = 0; i < size; i++)
    if (a[i] > max)
      max = a[i];
  return max;
}

float lowpass(int u, int v, int d0, int n, int w, int u0, int v0)
{
  (void)w;
  (void)u0;
  (void)v0;
  double d = dist(u, v);
  return 1.f / (1 + powf(d / (float)d0, 2 * n));
}

float highpass(int u, int v, int d0, int n, int w, int u0, int v0)
{
  (void)w;
  (void)u0;
  (void)v0;
  return 1 - lowpass(u, v, d0, n, w, u0, v0);
}

float bandreject(int u, int v, int d0, int n, int w, int u0, int v0)
{
  (void)u0;
  (void)v0;
  double d = dist(u, v);
  double base = (d * w) / (d * d - d0 * d0);
  return 1.f / (1 + powf(base, 2 * n));
}

float bandpass(int u, int v, int d0, int n, int w, int u0, int v0)
{

  (void)u0;
  (void)v0;
  return 1 - bandreject(u, v, d0, n, w, u0, v0);
}

float notch(int u, int v, int d0, int n, int w, int u0, int v0)
{
  (void)w;

  double d1 = dist(u - u0, v - v0);
  double d2 = dist(u + u0, v + v0);
  double base = (d0 * d0) /(d1 * d2);
  float res = 1.f / (1 + pow(base,  2*n));
  return res;
}

void process(char *inp, char *out,
             int d0, int nx2, int ww, int u0, int v0,
             float (*apply)(int, int, int, int, int, int, int))
{
  pnm ims = pnm_load(inp);
  int rows = pnm_get_height(ims);
  int cols = pnm_get_width(ims);
  int size = rows * cols;
  pnm imd = pnm_new(cols, rows, PnmRawPpm);
  unsigned short *data = pnm_get_channel(ims, NULL, 0);
  unsigned short *data_as = malloc(sizeof(unsigned short) * size);
  fftw_complex *fft_data = forward(rows, cols, data);
  float *as = malloc(sizeof(float) * size);
  float *ps = malloc(sizeof(float) * size);
  freq2spectra(rows, cols, fft_data, as, ps);
  fftw_complex *buffer = malloc(sizeof(fftw_complex) * size);

  // get center index for both dimensions
  int rows_center_index = rows / 2;
  int cols_center_index = cols / 2;
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      int u = j - cols_center_index;
      int v = i - rows_center_index;
      as[i * cols + j] *= (*apply)(u, v, d0, nx2, ww, u0, v0);
    }
  }

  float amax = getMax(as, size);
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      float d_as = as[i * cols + j];
      data_as[i * cols + j] = pow((d_as / amax), 0.2) * 255;
    }
  }
  spectra2freq(rows, cols, as, ps, buffer);
  unsigned short *res = backward(rows, cols, buffer);
  for (int c = 0; c < 3; c++)
    pnm_set_channel(imd, res, c);

  pnm_save(imd, PnmRawPpm, out);

  pnm_free(ims);
  pnm_free(imd);
  free(data);
  fftw_free(fft_data);
  fftw_free(buffer);
  free(res);
}

void usage(char *s)
{
  fprintf(stderr, "Usage: %s <ims> <imd> <filter> ", s);
  fprintf(stderr, "<d0> <n> <w> <u0> <v0>\n");
  exit(EXIT_FAILURE);
}

#define param 8
int main(int argc, char *argv[])
{
  if (argc != param + 1)
    usage(argv[0]);
  char *ims = argv[1];
  char *imd = argv[2];
  char *function = argv[3];
  int d0 = atoi(argv[4]);
  int n = atoi(argv[5]);
  int w = atoi(argv[6]);
  int u0 = atoi(argv[7]);
  int v0 = atoi(argv[8]);
  if (!strcmp(function, "lp"))
  {
    process(ims, imd, d0, n, w, u0, v0, lowpass);
  }
  else if (!strcmp(function, "hp"))
  {
    process(ims, imd, d0, n, w, u0, v0, highpass);
  }
  else if (!strcmp(function, "br"))
  {
    process(ims, imd, d0, n, w, u0, v0, bandreject);
  }
  else if (!strcmp(function, "bp"))
  {
    process(ims, imd, d0, n, w, u0, v0, bandpass);
  }
  else if (!strcmp(function, "no"))
  {
    process(ims, imd, d0, n, w, u0, v0, notch);
  }
  else
  {
    usage("unknown function");
  }

  return EXIT_SUCCESS;
}
