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
#include <stdbool.h>

// concatenate the prefix with the desire name
void create_filename(char*filename, char *prefix, char *name)
{
  char  tmp[256];
  snprintf(tmp, sizeof(tmp), "%s",name );
  char *token = strtok(tmp, "/");
  char *tmp2;
  // loop through the string to extract all other tokens
  while (token != NULL)
  {
    tmp2 = token;
    token = strtok(NULL, "/");
  }
  snprintf(filename, 256, "%s%s", prefix, tmp2);
  
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
  
  char  filename[256];
  create_filename(filename,"FB-", name);
  pnm_save(imd, PnmRawPpm, filename);
  // memory management
  pnm_free(ims);
  free(data);
  free(freq_repr);
  free(g_img);
  pnm_free(imd);
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
  
  char  filename[256];
  create_filename(filename,"FB-ASPS-", name);
  pnm_save(imd, PnmRawPpm, filename);

  pnm_free(ims);
  free(data);
  free(out);
  free(as);
  free(ps);
  free(freq_repr);
  free(g_img);
  pnm_free(imd);
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
  char  as_filename[256];
  create_filename(as_filename,"AS-", name);
  char ps_filename[256];
  create_filename(ps_filename,"PS-", name);
  pnm_save(imd_as, PnmRawPpm, as_filename);
  pnm_save(imd_ps, PnmRawPpm, ps_filename);

  // Memory management
  pnm_free(ims);
  free(data);
  free(as);
  free(ps);
  pnm_free(imd_as);
  pnm_free(imd_ps);
  free(bufferAs);
  free(bufferPs);
  free(freq_repr);
  fprintf(stderr, "OK\n");
}


/**
 * @brief 
 **/ 
bool is_color_image(pnm ims){
   unsigned short *datar = pnm_get_channel(ims, NULL, PnmRed);
   unsigned short *datag = pnm_get_channel(ims, NULL, PnmGreen);
   unsigned short *datab = pnm_get_channel(ims, NULL, PnmBlue);
   bool res = datar[0]==datag[0] && datag[0]==datab[0];
   free(datar);
   free(datag);
   free(datab);
   return !res;
}

void test_add_frequencies_color(pnm ims, int freq,char*name);
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

  if(is_color_image(ims)){
    test_add_frequencies_color(ims,freq,name);
    return;
  }
  int rows = pnm_get_height(ims);
  int cols = pnm_get_width(ims);
  int size = rows * cols;
  unsigned short *data = pnm_get_channel(ims, NULL, PnmRed);

    
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
  unsigned short * backward_data = backward(rows, cols, out);

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
  char imdFreq_filename[256];
  create_filename(imdFreq_filename,"FREQ-", name);
  char imdAs_filename[256];
  create_filename(imdAs_filename,"FAS-", name);

  pnm_save(imdFreq, PnmRawPpm, imdFreq_filename);
  pnm_save(imdAs, PnmRawPpm, imdAs_filename);

  // memory management
  pnm_free(ims);
  free(data);
  free(backward_data);
  free(as);
  free(ps);
  free(out);
  pnm_free(imdFreq);
  pnm_free(imdAs);
  free(freq_repr);
  free(bufferAs);
  fprintf(stderr, "OK\n");
}

/**
 * @brief add frequencies and color image
 * @param char* name, the input image file name
 * @param int freq, the frequence to be modify
 */
void
test_add_frequencies_color(pnm ims,int freq,char*name)
{
  fprintf(stderr, "test_add_frequencies_color: ");
  int rows = pnm_get_height(ims);
  int cols = pnm_get_width(ims);
  int size = rows * cols;
   unsigned short *datar = pnm_get_channel(ims, NULL, PnmRed);
   unsigned short *datag = pnm_get_channel(ims, NULL, PnmGreen);
   unsigned short *datab = pnm_get_channel(ims, NULL, PnmBlue);

    
  float *as_r = malloc(size * sizeof(float));
  float *ps_r = malloc(size * sizeof(float));    
  float *as_g = malloc(size * sizeof(float));
  float *ps_g = malloc(size * sizeof(float));    
  float *as_b = malloc(size * sizeof(float));
  float *ps_b = malloc(size * sizeof(float));
  fftw_complex *out = malloc(size * sizeof(fftw_complex));
  pnm imdFreq = pnm_new(cols, rows, PnmRawPpm);
  pnm imdAs = pnm_new(cols, rows, PnmRawPpm);

  fftw_complex *freq_repr_r = forward(rows, cols, datar);
  fftw_complex *freq_repr_g = forward(rows, cols, datag);
  fftw_complex *freq_repr_b = forward(rows, cols, datab);
  freq2spectra(rows, cols, freq_repr_r, as_r, ps_r);
  freq2spectra(rows, cols, freq_repr_g, as_g, ps_g);
  freq2spectra(rows, cols, freq_repr_b, as_b, ps_b);

  float amax = getMax(as_r, size);
  float desire_amp = amax * 0.25;

  // get center index for both dimensions
  int rows_center_index = rows / 2;
  int cols_center_index = cols / 2;

  // access the frequence to be modify by shifting freq index in four directions
  // replace with the desire amplitude for all channels 

  // horizontal synmmetry left & right
  as_r[rows_center_index * cols + cols_center_index - freq] = desire_amp;
  as_r[rows_center_index * cols + cols_center_index + freq] = desire_amp;
  // vertical synmmetry top & bottom
  as_r[rows_center_index * cols + cols_center_index - freq * cols] = desire_amp;
  as_r[rows_center_index * cols + cols_center_index + freq * cols] = desire_amp;  

  // horizontal synmmetry left & right
  as_g[rows_center_index * cols + cols_center_index - freq] = desire_amp;
  as_g[rows_center_index * cols + cols_center_index + freq] = desire_amp;
  // vertical synmmetry top & bottom
  as_g[rows_center_index * cols + cols_center_index - freq * cols] = desire_amp;
  as_g[rows_center_index * cols + cols_center_index + freq * cols] = desire_amp;  

  // horizontal synmmetry left & right
  as_b[rows_center_index * cols + cols_center_index - freq] = desire_amp;
  as_b[rows_center_index * cols + cols_center_index + freq] = desire_amp;
  // vertical synmmetry top & bottom
  as_b[rows_center_index * cols + cols_center_index - freq * cols] = desire_amp;
  as_b[rows_center_index * cols + cols_center_index + freq * cols] = desire_amp;
   
  spectra2freq(rows, cols, as_r, ps_r, out);
  unsigned short * backward_data_r = backward(rows, cols, out);
  spectra2freq(rows, cols, as_g, ps_g, out);
  unsigned short * backward_data_g = backward(rows, cols, out);
  spectra2freq(rows, cols, as_b, ps_b, out);
  unsigned short * backward_data_b = backward(rows, cols, out);

  unsigned short *bufferAs_r = malloc(size * sizeof(unsigned short));
  //center sample
  float k = 0.2;
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      float data_as_r = as_r[i * cols + j];
      bufferAs_r[i * cols + j] = pow((data_as_r / amax), k) * 255;
    }
  }


  pnm_set_channel(imdFreq, backward_data_r, PnmRed);
  pnm_set_channel(imdFreq, backward_data_g, PnmGreen);
  pnm_set_channel(imdFreq, backward_data_b, PnmBlue);
  for (unsigned int i = PnmRed; i <= PnmBlue; i++)
  {
    pnm_set_channel(imdAs, bufferAs_r, i);
  }

  // save output images
  char imdFreq_filename[256];
  create_filename(imdFreq_filename,"FREQ-", name);
  char imdAs_filename[256];
  create_filename(imdAs_filename,"FAS-", name);

  pnm_save(imdFreq, PnmRawPpm, imdFreq_filename);
  pnm_save(imdAs, PnmRawPpm, imdAs_filename);

  // memory management
  pnm_free(ims);
  free(datar);
  free(datag);
  free(datab);
  free(as_r);
  free(ps_r);
  free(as_g);
  free(ps_g);
  free(as_b);
  free(ps_b);
  free(out);
  pnm_free(imdFreq);
  pnm_free(imdAs);
  free(freq_repr_r);
  free(freq_repr_g);
  free(freq_repr_b);
  free(backward_data_r);
  free(backward_data_g);
  free(backward_data_b);
  free(bufferAs_r);
  fprintf(stderr, "OK\n");

}


void run(char *name)
{
  test_forward_backward(name);
  test_reconstruction(name);
  test_display(name);
  test_add_frequencies(name);
  fftw_cleanup();
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
