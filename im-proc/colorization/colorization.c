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
#include <time.h>

#define D 3
#define SAMPLE_SIZE 3
#define NEIGHBORHOOD_SIZE 5

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
    means[c] /= (float)size;
  
  return means;
}

float getStandardDeviations(float *data, float means, int rows, int cols)
{
  float standardDeviationsns =0;
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
        standardDeviationsns += (data[i * cols * 3 + j * 3 ] - means) * (data[i * cols * 3 + j * 3] - means);
 
  return sqrt(standardDeviationsns/(rows*cols));
}

float getNeighborStandardDeviation(float *data, float mean, int size)
{
  float standardDeviationsns =0;
  for (int i = 0; i < size; i++)
        standardDeviationsns += (data[i] - mean) * (data[i] - mean);
 
  return sqrt(standardDeviationsns/(float)size);
}

float computeNeighborDev(float *labTarget, int rows,int cols, int i,int j){
    // compute neighborhood statics for current pixel
    int nb_neightbor =0;
    float *data =calloc(sizeof(float),NEIGHBORHOOD_SIZE *NEIGHBORHOOD_SIZE);
    int size = NEIGHBORHOOD_SIZE /2;
    float mean =0;
    for(int ip = 0; ip<NEIGHBORHOOD_SIZE;ip++){
      for(int jp = 0; jp<NEIGHBORHOOD_SIZE;jp++){
          int x = j-size +jp;
          int y = i-size + ip;
          if(x>=0 && x <cols && y>=0 && y<rows){
            data[nb_neightbor]=labTarget[y*cols*3 + x*3];
            mean+=data[nb_neightbor];
            nb_neightbor++;
          } 
      }
    }
    mean/=nb_neightbor;
    float dev = getNeighborStandardDeviation(data,mean,nb_neightbor);
    free(data);
    return dev;
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
  //normalize 
  float min[3];
  float max[3];
  min[PnmRed] = max[PnmRed] = rgb_tmp[PnmRed];
  min[PnmGreen] = max[PnmGreen] = rgb_tmp[PnmGreen];
  min[PnmBlue] = max[PnmBlue] = rgb_tmp[PnmBlue];

  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      for (int c = PnmRed; c <= PnmBlue; c++){
        float data = rgb_tmp[i * cols * 3 + j * 3 + c];
        if(data<min[c]) min[c]=data;
        if(max[c]<data) max[c]=data;
      }

  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      for (int c = PnmRed; c <= PnmBlue; c++){
        float data = rgb_tmp[i * cols * 3 + j * 3 + c];
        rgb[i * cols * 3 + j * 3 + c] =(unsigned short) (data - min[c]) *255.f /(float) (max[c] - min[c]);
      }

  free(rgb_tmp);
}

void luminanceRemapping(float *labSource, float *meansSource, float *meansTarget, float dSource, float dTarget, int rows, int cols)
{
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++){
        float data = labSource[i * cols * 3 + j * 3] - meansSource[0];
        labSource[i * cols * 3 + j * 3] = data * (dTarget / dSource) +meansTarget[0]/3.3 + meansSource[0]/1.5;
    }
}

void jitterSamples(float * labSource, float * samples, int rows, int cols){
  int sampleW[SAMPLE_SIZE] ={0};
  int sampleH[SAMPLE_SIZE] ={0};
  int countW =0;
  int countH =0;
  for (int i = 0; i < SAMPLE_SIZE; i++)
  {
    int rW = rand()%cols;
    int rH = rand()%rows;
    sampleW[countW]=rW;
    countW++;
    sampleH[countH]=rH;
    countH++;
  }
  
  // init samples 
  for (int i = 0; i < SAMPLE_SIZE; i++)
  {
    int y = sampleH[i];
    int x = sampleW[i];
    float dev =computeNeighborDev(labSource,rows,cols,y,x);
    samples[i*3] = (labSource[y*cols*3 + x*3] +dev) /2.f;
    samples[i*3+1] = labSource[y*cols*3 + x*3 +1];
    samples[i*3+2] = labSource[y*cols*3 + x*3 +2];
  }
  
}

void process(char *ims_path, char *imt_path, char *imd)
{
  pnm ims = pnm_load(ims_path);
  pnm imt = pnm_load(imt_path);
  unsigned short *imgSourcedata = pnm_get_image(ims);
  unsigned short *imgTargetdate = pnm_get_image(imt);

  int rowsSource = pnm_get_height(ims);
  int colsSource = pnm_get_width(ims);
  int sizeSource = rowsSource * colsSource * 3;  

  int rowsTarget = pnm_get_height(imt);
  int colsTarget = pnm_get_width(imt);
  int sizeTarget = rowsTarget * colsTarget * 3;

  float *imgSource = calloc(sizeof(float), sizeSource);
  float *imgTarget = calloc(sizeof(float), sizeTarget);
  float *imgFinal = calloc(sizeof(float), sizeTarget);
  for (int i = 0; i < sizeSource; i++)
    imgSource[i] = imgSourcedata[i];
    
  

  for (int i = 0; i < sizeTarget; i++)
    imgTarget[i] = imgTargetdate[i];

  float *lmsSource = calloc(sizeof(float), sizeSource);
  float *lmsTarget = calloc(sizeof(float), sizeTarget);
  float *lmsFinal = calloc(sizeof(float), sizeTarget);
  float *labSource = calloc(sizeof(float), sizeSource);
  float *labTarget = calloc(sizeof(float), sizeTarget);

  // convert source image to decorrelated lab space
  //transfer rgb to lms
  rgb2lms(imgSource, lmsSource, rowsSource, colsSource);
  rgb2lms(imgTarget, lmsTarget, rowsTarget, colsTarget);
  //transfer lms to lab
  multiply(lmsSource, labSource, LMS2LAB, rowsSource, colsSource);
  multiply(lmsTarget, labTarget, LMS2LAB, rowsTarget, colsTarget);

  // perform luminance remapping 
  float *meansSource = getMeans(labSource, rowsSource, colsSource);
  float *meansTarget = getMeans(labTarget, rowsTarget, colsTarget);

  float dSource = getStandardDeviations(labSource, meansSource[0], rowsSource, colsSource);
  float dTarget = getStandardDeviations(labTarget, meansTarget[0], rowsTarget, colsTarget);
  luminanceRemapping(labSource, meansSource, meansTarget, dSource, dTarget, rowsSource, colsSource);
  
  // jitter sampkers from the soucre image 
  float * samples = calloc(sizeof(float), SAMPLE_SIZE *3);
  jitterSamples(labSource,samples,rowsSource,colsSource);

  // find best matching source pixel and transfer color to target pixel
  for (int y = 0; y < rowsTarget; y++)
  {
    for (int x = 0; x < colsTarget; x++)
    {
      // compute neighborhood statics for current pixel
    float dev = computeNeighborDev(labTarget,rowsTarget,colsTarget,y,x);
    float weight =(labTarget[y*colsTarget*3 + x*3]+dev)/2.f;
    
    // get the best sample 
    int tmp=0;
    for (int i = 0; i < SAMPLE_SIZE; i++)
      if(fabs(weight-samples[i*3]) <fabs(weight-samples[tmp*3])) tmp=i;
        
    labTarget[y*colsTarget*3 + x*3+1] = samples[tmp*3+1];
    labTarget[y*colsTarget*3 + x*3+2] = samples[tmp*3+2];
    }
    
  }
  

  //transfer lab to lms
  multiply(labTarget, lmsFinal, LAB2LMS, rowsTarget, colsTarget);

  //transfer lms to rgb
  lms2rgb(lmsFinal, imgFinal, rowsTarget, colsTarget);
  
  pnm imf = pnm_new(colsTarget, rowsTarget, PnmRawPpm);
  for (int i = 0; i < rowsTarget; i++)
    for (int j = 0; j < colsTarget; j++)
      for (int c = PnmRed; c <= PnmBlue; c++)
      {
        unsigned short val = (unsigned short)imgFinal[i * colsTarget * 3 + j * 3 + c];
        pnm_set_component(imf, i, j, c, val);
      }
  pnm_save(imf, PnmRawPpm, imd);

  pnm_free(ims);
  pnm_free(imt);
  pnm_free(imf);
  free(samples);
  free(imgSource);
  free(imgTarget);
  free(imgFinal);
  free(lmsSource);
  free(lmsTarget);
  free(lmsFinal);
  free(labSource);
  free(labTarget);
  free(meansSource);
  free(meansTarget);
}

void usage(char *s)
{
  fprintf(stderr, "Usage: %s <ims> <imt> <imd> \n", s);
  exit(EXIT_FAILURE);
}

#define PARAM 3
int main(int argc, char *argv[])
{
  srand(time(NULL));
  if (argc != PARAM + 1)
    usage(argv[0]);
  process(argv[1], argv[2], argv[3]);
  return EXIT_SUCCESS;
}
