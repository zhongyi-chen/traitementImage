/**
 * @file color-transfert
 * @brief transfert color from source image to target image.
 *        Method from Reinhard et al. : 
 *        Erik Reinhard, Michael Ashikhmin, Bruce Gooch and Peter Shirley, 
 *        'Color Transfer between Images', IEEE CGA special issue on 
 *        Applied Perception, Vol 21, No 5, pp 34-41, September - October 2001
 */

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <bcl.h>

#define D 3

static float RGB2LMS[D][D] = {
    {0.3811, 0.5783, 0.0402},
    {0.1967, 0.7244, 0.0782},
    {0.0241, 0.1288, 0.8444},
};

static float LMS2LAB[D][D] = {
    {0.5773, 0.5773, 0.5773},
    {0.4082, 0.4082, -0.8164},
    {0.7071, -0.7071, 0},
};

static float LAB2LMS[D][D] = {
    {0.5774, 0.4082, 0.7071},
    {0.5774, 0.4082, -0.7071},
    {0.5774, -0.8164, 0},
};

static float LMS2RGB[D][D] = {
    {4.4679, -3.5873, 0.1193},
    {-1.2186, 2.3809, -0.1624},
    {0.0497, -0.2439, 1.2045},
};


void multiply(float *input, float *output, float matrix[D][D], int rows, int cols)
{
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      for (int c = 0; c < 3; c++)
      {
        float tmp = 0.f;
        for (int n = 0; n < 3; n++)
          tmp += matrix[c][n] * input[i * cols * 3 + j * 3 + n];
        output[i * cols * 3 + j * 3 + c] = tmp;
      }
}

float *getMeans(float *data, int rows, int cols)
{
  int size = rows * cols;
  float *means = calloc(sizeof(float), 3);
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      for (int c = PnmRed; c <= PnmBlue; c++)
        means[c] += data[i * cols * 3 + j * 3 + c];

  for (int c = PnmRed; c <= PnmBlue; c++)
  {
    means[c] /= (float)size;
  }
  return means;
}

float *getStandardDeviations(float *data, float *means, int rows, int cols)
{
  float *standardDeviationsns = calloc(sizeof(float), 3);
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      for (int c = PnmRed; c <= PnmBlue; c++)
        standardDeviationsns[c] += (data[i * cols * 3 + j * 3 + c] - means[c]) * (data[i * cols * 3 + j * 3 + c] - means[c]);
  for (int c = PnmRed; c <= PnmBlue; c++)
    standardDeviationsns[c] = sqrt(standardDeviationsns[c]);
  return standardDeviationsns;
}

unsigned short troncature(float data)
{
  if (data < 0)
    return 0;
  else if (data > 255)
    return 255;
  return (unsigned short)data;
}

void rgb2lms(float *rgb, float *lms, int rows, int cols)
{
  multiply(rgb, lms, RGB2LMS, rows, cols);
  int size = rows * cols * 3;
  //convert data to log space
  for (int i = 0; i < size; i++)
    lms[i] = (lms[i] <= 0) ? 0 : log10(lms[i]);
}

void lms2rgb(float *lms, float *rgb, int rows, int cols)
{
  int size = rows * cols * 3;
  float *rgb_tmp = calloc(sizeof(float), size);
  for (int i = 0; i < size; i++)
    lms[i] = pow(10, lms[i]);
  multiply(lms, rgb_tmp, LMS2RGB, rows, cols);
  for (int i = 0; i < size; i++)
    rgb[i] = troncature(rgb_tmp[i]);

  free(rgb_tmp);
}

float *colorCorrection(float *labTarget, float *meansSource, float *meansTarget, float *dSource, float *dTarget, int rows, int cols)
{
  float *labFinal = calloc(sizeof(float), rows * cols * 3);
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      for (int c = PnmRed; c <= PnmBlue; c++)
      {
        float data = labTarget[i * cols * 3 + j * 3 + c] - meansSource[c];
        labFinal[i * cols * 3 + j * 3 + c] = data * (dSource[c] / ((float)dTarget[c])) + meansSource[c] + meansTarget[c]*0;
      }
  return labFinal;
}

void process(char *ims_path, char *imt_path, char *imd)
{
  pnm ims = pnm_load(ims_path);
  pnm imt = pnm_load(imt_path);
  unsigned short *imgSourcedata = pnm_get_image(ims);
  unsigned short *imgTargetdate = pnm_get_image(imt);

  int rows = pnm_get_height(ims);
  int cols = pnm_get_width(ims);
  int size = rows * cols * 3;
  float *imgSource = calloc(sizeof(float), size);
  float *imgTarget = calloc(sizeof(float), size);
  float *imgFinal = calloc(sizeof(float), size);
  for (int i = 0; i < size; i++)
  {
    imgSource[i] = imgSourcedata[i];
    imgTarget[i] = imgTargetdate[i];
  }

  float *lmsSource = calloc(sizeof(float), size);
  float *lmsTarget = calloc(sizeof(float), size);
  float *lmsFinal = calloc(sizeof(float), size);
  float *labSource = calloc(sizeof(float), size);
  float *labTarget = calloc(sizeof(float), size);
  //transfer rgb to lms
  rgb2lms(imgSource, lmsSource, rows, cols);
  rgb2lms(imgTarget, lmsTarget, rows, cols);
  //transfer lms to lab
  multiply(lmsSource, labSource, LMS2LAB, rows, cols);
  multiply(lmsTarget, labTarget, LMS2LAB, rows, cols);
  //color correction
  float *meansSource = getMeans(labSource, rows, cols);
  float *meansTarget = getMeans(labTarget, rows, cols);

  float *dSource = getStandardDeviations(labSource, meansSource, rows, cols);
  float *dTarget = getStandardDeviations(labTarget, meansTarget, rows, cols);

  float *labFinal = colorCorrection(labTarget, meansSource, meansTarget, dSource, dTarget, rows, cols);

  //transfer lab to lms
  multiply(labFinal, lmsFinal, LAB2LMS, rows, cols);

  //transfer lms to rgb
  lms2rgb(lmsFinal, imgFinal, rows, cols);
  
  pnm imf = pnm_new(cols, rows, PnmRawPpm);
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      for (int c = PnmRed; c <= PnmBlue; c++)
      {
        unsigned short val = (unsigned short)imgFinal[i * cols * 3 + j * 3 + c];
        pnm_set_component(imf, i, j, c, val);
      }
  pnm_save(imf, PnmRawPpm, imd);

  pnm_free(ims);
  pnm_free(imt);
  pnm_free(imf);
  free(imgSource);
  free(imgTarget);
  free(imgFinal);
  free(lmsSource);
  free(lmsTarget);
  free(lmsFinal);
  free(labSource);
  free(labTarget);
  free(labFinal);
  free(meansSource);
  free(meansTarget);
  free(dSource);
  free(dTarget);
}

void usage(char *s)
{
  fprintf(stderr, "Usage: %s <ims> <imt> <imd> \n", s);
  exit(EXIT_FAILURE);
}

#define PARAM 3
int main(int argc, char *argv[])
{
  if (argc != PARAM + 1)
    usage(argv[0]);
  process(argv[1], argv[2], argv[3]);
  return EXIT_SUCCESS;
}
