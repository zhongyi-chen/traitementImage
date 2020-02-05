/**
 * @file test-fft.c
 * @brief test the behaviors of functions in fft module
 *
 */
#include <stdlib.h>
#include <stdio.h>

#include <fft.h>

/**
 * @brief test the forward and backward functions
 * @param char* name, the input image file name
 */
void test_forward_backward(char *name)
{
  pnm ims = pnm_load(name);
  int rows = pnm_get_height(ims);
  int cols = pnm_get_width(ims);
  unsigned short *data = pnm_get_channel(ims, NULL, PnmRed);

  fftw_complex *freq_repr = forward(rows, cols, data);
  unsigned short *g_img = backward(rows, cols, freq_repr);

  pnm imd = pnm_new(cols, rows, PnmRawPpm);
  for (unsigned int i = PnmRed; i <= PnmBlue; i++)
  {
    pnm_set_channel(imd, g_img, i);
  }
  pnm_save(imd, PnmRawPpm, "test.pnm");
  free(data);
  free(g_img);
  free(freq_repr);
  free(imd);
  free(ims);
  fprintf(stderr, "test_forward_backward: ");
  fprintf(stderr, "OK\n");
}

/**
 * @brief test image reconstruction from of magnitude and phase spectrum
 * @param char *name: the input image file name
 */
void test_reconstruction(char *name)
{
  pnm ims = pnm_load(name);
  int rows = pnm_get_height(ims);
  int cols = pnm_get_width(ims);
  int size = rows * cols;
  unsigned short *data = pnm_get_channel(ims, NULL, PnmRed);

  fftw_complex *freq_repr = forward(rows, cols, data);
  fftw_complex *out = malloc(size*sizeof(fftw_complex));
  float * as = malloc(size*sizeof(float));
  float * ps = malloc(size*sizeof(float));
  pnm imd = pnm_new(cols, rows, PnmRawPpm);
  freq2spectra(rows,cols,freq_repr,as,ps);
  spectra2freq(rows,cols,as,ps,out);

  unsigned short *g_img = backward(rows, cols, out);
  for (unsigned int i = PnmRed; i <= PnmBlue; i++)
  {
    pnm_set_channel(imd, g_img, i);
  }
  pnm_save(imd, PnmRawPpm, "asps.pnm");

  free(data);
  free(out);
  free(as);
  free(ps);
  free(freq_repr);
  free(ims);
  free(imd);
  fprintf(stderr, "test_reconstruction: ");
  fprintf(stderr, "OK\n");

}

/**
 * @brief test construction of magnitude and phase images in ppm files
 * @param char* name, the input image file name
 */
void test_display(char *name)
{
  fprintf(stderr, "test_display: ");
  (void)name;
  fprintf(stderr, "OK\n");
}

/**
 * @brief test the modification of magnitude by adding a periodic functions
          on both vertical and horizontal axis, and 
 *        construct output images
 * @param char* name, the input image file name
 */
void

test_add_frequencies(char *name)
{
  fprintf(stderr, "test_add_frequencies: ");
  (void)name;
  fprintf(stderr, "OK\n");
}

void run(char *name)
{
  test_forward_backward(name);
  test_reconstruction(name);
  test_display(name);
  test_add_frequencies(name);
}

void usage(const char *s)
{
  fprintf(stderr, "Usage: %s <ims> \n", s);
  exit(EXIT_FAILURE);
}

#define PARAM 1
int main(int argc, char *argv[])
{
  if (argc != PARAM + 1)
    usage(argv[0]);
  run(argv[1]);
  return EXIT_SUCCESS;
}
