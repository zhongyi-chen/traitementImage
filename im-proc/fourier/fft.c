#include <float.h>
#include <stdlib.h>
#include <math.h>

#include <fft.h>

fftw_complex *
forward(int rows, int cols, unsigned short* g_img)
{
  int size = rows * cols;
  // convert spatial image to complex image
  fftw_complex * g_img_complex = malloc(size*sizeof(fftw_complex));

  for (int i  = 0; i<size;i++){
    g_img_complex[i] = g_img[i];
  }

  //center sample 
  for(int i =0;i<rows;i++){
    for(int j =0;j<cols;j++){
      g_img_complex[i*cols +j] = g_img_complex[i*cols +j] * pow((-1),(i+j));
    }
  }
  fftw_complex * out = malloc(size*sizeof(fftw_complex));
  fftw_plan plan = fftw_plan_dft_2d(rows,cols,g_img_complex,out,FFTW_FORWARD,FFTW_ESTIMATE);
  fftw_execute(plan);

  free(g_img_complex);
  fftw_destroy_plan(plan);
  return out;
}

unsigned short *
backward(int rows, int cols, fftw_complex* freq_repr)
{
  int size = rows * cols;
  unsigned short * g_img = malloc(size*sizeof(unsigned short));

  fftw_complex * out = malloc(size*sizeof(fftw_complex));
  fftw_plan plan = fftw_plan_dft_2d(rows,cols,freq_repr,out,FFTW_BACKWARD,FFTW_ESTIMATE);
  fftw_execute(plan);
  //center back sample 
  for(int i =0;i<rows;i++){
    for(int j =0;j<cols;j++){
      out[i*cols +j] = out[i*cols +j] * pow((-1),(i+j));
    }
  }

  // normalize
  for(int i =0;i<size;i++){
    g_img[i]= creal(out[i])/size;
    printf("g_img %hu\n",g_img[i]);
    printf("real nm %f\n", creal(out[i]));
  }
  
  free(out);
  fftw_destroy_plan(plan);
  return g_img;
}

void
freq2spectra(int rows, int cols, fftw_complex* freq_repr, float* as, float* ps) 
{
  (void)rows;
  (void)cols;
  (void)freq_repr;
  (void)as;
  (void)ps;
}

void 
spectra2freq(int rows, int cols, float* as, float* ps, fftw_complex* freq_repr)
{
  (void)rows;
  (void)cols;
  (void)as;
  (void)ps;
  (void)freq_repr;
}
