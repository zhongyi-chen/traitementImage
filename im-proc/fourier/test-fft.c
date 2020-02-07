/**
 * @file test-fft.c
 * @brief test the behaviors of functions in fft module
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <fft.h>
#include <string.h>

// concatenate the prefix with the desire name
char *create_filename(char *prefix, char *name)
{
  char *tmp = (char *)malloc(40 * sizeof(char));
  char *filename = (char *)malloc(60 * sizeof(char));
  tmp = strcpy(tmp, name);
  char *token = strtok(tmp, "/");
  // loop through the string to extract all other tokens
  while (token != NULL)
  {
    tmp = token;
    token = strtok(NULL, "/");
  }
  strcat(filename, prefix);
  strcat(filename, tmp);
  return filename;
}

/**
 * @brief test the forward and backward functions
 * @param char* name, the input image file name
 */
void test_forward_backward(char *name)
{
  fprintf(stderr, "test_forward_backward: ");
  pnm ims = pnm_load(name);
  int rows = pnm_get_height(ims);
  int cols = pnm_get_width(ims);
  unsigned short *data = pnm_get_channel(ims, NULL, PnmRed);

  fftw_complex *freq_repr = forward(rows, cols, data);
  unsigned short *g_img = backward(rows, cols, freq_repr);

  pnm imd = pnm_new(cols, rows, PnmRawPpm);
  for (unsigned int i = PnmRed; i <= PnmBlue; i++)
    pnm_set_channel(imd, g_img, i);
  
  char * filename = create_filename("FB-", name);
  pnm_save(imd, PnmRawPpm, filename);
  // memory management
  free(ims);
  free(data);
  free(freq_repr);
  free(g_img);
  free(imd);
  free(filename);
  fprintf(stderr, "OK\n");
}

/**
 * @brief test image reconstruction from of magnitude and phase spectrum
 * @param char *name: the input image file name
 */
void test_reconstruction(char *name)
{
  fprintf(stderr, "test_reconstruction: ");
  pnm ims = pnm_load(name);
  int rows = pnm_get_height(ims);
  int cols = pnm_get_width(ims);
  int size = rows * cols;
  unsigned short *data = pnm_get_channel(ims, NULL, PnmRed);

  fftw_complex *out = malloc(size * sizeof(fftw_complex));
  float *as = malloc(size * sizeof(float));
  float *ps = malloc(size * sizeof(float));

  fftw_complex *freq_repr = forward(rows, cols, data);
  freq2spectra(rows, cols, freq_repr, as, ps);
  spectra2freq(rows, cols, as, ps, out);
  unsigned short *g_img = backward(rows, cols, out);

  pnm imd = pnm_new(cols, rows, PnmRawPpm);
  for (unsigned int i = PnmRed; i <= PnmBlue; i++)
    pnm_set_channel(imd, g_img, i);
  
  char * filename = create_filename("FB-ASPS-", name);
  pnm_save(imd, PnmRawPpm, filename);

  free(ims);
  free(data);
  free(out);
  free(as);
  free(ps);
  free(freq_repr);
  free(imd);
  free(filename);
  fprintf(stderr, "OK\n");
}

float getMax(float *a, int size)
{
  float max = a[0];
  for (int i = 0; i < size; i++)
    if (a[i] > max)
      max = a[i];
  return max;
}

/**
 * @brief test construction of magnitude and phase images in ppm files
 * @param char* name, the input image file name
 */
void test_display(char *name)
{
  fprintf(stderr, "test_display: ");

  pnm ims = pnm_load(name);
  int rows = pnm_get_height(ims);
  int cols = pnm_get_width(ims);
  int size = rows * cols;
  unsigned short *data = pnm_get_channel(ims, NULL, PnmRed);

  float *as = malloc(size * sizeof(float));
  float *ps = malloc(size * sizeof(float));
  pnm imd_as = pnm_new(cols, rows, PnmRawPpm);
  pnm imd_ps = pnm_new(cols, rows, PnmRawPpm);
  unsigned short *bufferAs = malloc(size * sizeof(unsigned short));
  unsigned short *bufferPs = malloc(size * sizeof(unsigned short));
  
  fftw_complex *freq_repr = forward(rows, cols, data);
  freq2spectra(rows, cols, freq_repr, as, ps);

  float amax = getMax(as, size);
  float pmax = getMax(ps, size);
  //center sample
  float k = 0.155;
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      float data_as = as[i * cols + j];
      bufferAs[i * cols + j] = pow((data_as / amax), k) * 255;

      float data_ps = ps[i * cols + j];
      bufferPs[i * cols + j] = pow((data_ps / pmax), k) * 255;
    }
  }

  for (unsigned int i = PnmRed; i <= PnmBlue; i++)
  {
    pnm_set_channel(imd_as, bufferAs, i);
    pnm_set_channel(imd_ps, bufferPs, i);
  }
  // save output images
  char * as_filename = create_filename("AS-", name);
  char * ps_filename =  create_filename("PS-", name);
  pnm_save(imd_as, PnmRawPpm, as_filename);
  pnm_save(imd_ps, PnmRawPpm, ps_filename);

  // Memory management
  free(ims);
  free(data);
  free(as);
  free(ps);
  free(imd_as);
  free(imd_ps);
  free(bufferAs);
  free(bufferPs);
  free(freq_repr);
  free(as_filename);
  free(ps_filename);

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
  // the frequence to be modify
  int freq = 8;

  pnm ims = pnm_load(name);
  int rows = pnm_get_height(ims);
  int cols = pnm_get_width(ims);
  int size = rows * cols;
  unsigned short *data = pnm_get_channel(ims, NULL, PnmRed);

  unsigned short *backward_data = malloc(size * sizeof(unsigned short));
  float *as = malloc(size * sizeof(float));
  float *ps = malloc(size * sizeof(float));
  fftw_complex *out = malloc(size * sizeof(fftw_complex));
  pnm imdFreq = pnm_new(cols, rows, PnmRawPpm);
  pnm imdAs = pnm_new(cols, rows, PnmRawPpm);

  fftw_complex *freq_repr = forward(rows, cols, data);
  freq2spectra(rows, cols, freq_repr, as, ps);

  float amax = getMax(as, size);
  float desire_amp = amax * 0.25;

  // get center index for both dimensions
  int rows_center_index = rows / 2;
  int cols_center_index = cols / 2;

  // access the frequence to be modify by shifting freq index in four directions
  // replace with the desire amplitude
  // horizontal synmmetry left & right
  as[rows_center_index * cols + cols_center_index - freq] = desire_amp;
  as[rows_center_index * cols + cols_center_index + freq] = desire_amp;
  // vertical synmmetry top & bottom
  as[rows_center_index * cols + cols_center_index - freq * cols] = desire_amp;
  as[rows_center_index * cols + cols_center_index + freq * cols] = desire_amp;
   
  spectra2freq(rows, cols, as, ps, out);
  backward_data = backward(rows, cols, out);

  unsigned short *bufferAs = malloc(size * sizeof(unsigned short));
  //center sample
  float k = 0.2;
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      float data_as = as[i * cols + j];
      bufferAs[i * cols + j] = pow((data_as / amax), k) * 255;
    }
  }

  for (unsigned int i = PnmRed; i <= PnmBlue; i++)
  {
    pnm_set_channel(imdFreq, backward_data, i);
    pnm_set_channel(imdAs, bufferAs, i);
  }

  // save output images
  char *imdFreq_filename = create_filename("FREQ-", name);
  char *imdAs_filename = create_filename("FAS-", name);

  pnm_save(imdFreq, PnmRawPpm, imdFreq_filename);
  pnm_save(imdAs, PnmRawPpm, imdAs_filename);

  // memory management
  free(ims);
  free(data);
  free(backward_data);
  free(as);
  free(ps);
  free(out);
  free(imdFreq);
  free(imdAs);
  free(freq_repr);
  free(bufferAs);
  free(imdFreq_filename);
  free(imdAs_filename);
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
