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

float RGB2XYZ[D][D] ={
  {0.5141, 0.3239, 0.1604},
  {0.2651, 0.6702, 0.0641},
  {0.0241, 0.1228, 0.8444},
};

float XYZ2LMS[D][D] = {
  {0.3897,  0.6890, -0.0787},
  {-0.2298, 1.1834, 0.0464},
  {0.f,     0.f,    1.f},
};


float RGB2LMS[D][D] = {
  {0.3811, 0.5783, 0.0402}, 
  {0.1967, 0.7244, 0.0782},  
  {0.0241, 0.1288, 0.8444},
};

float LMS2LAB[D][D] = {
  {0.5773,  0.5773,  0.5773},
  {0.4082,  0.4082, -0.8164},
  {0.7071, -0.7071,  0},
};

float LAB2LMS[D][D] = {
  {0.5774,  0.4082,  0.7071},
  {0.5774,  0.4082, -0.7071},
  {0.5774, -0.8164,  0},
};

float LMS2RGB[D][D] = {
  {4.4679,  -3.5873,  0.1193},
  {-1.2186,  2.3809, -0.1624},
  {0.0497,  -0.2439,  1.2045},
};


void
process(char *ims, char *imt, char* imd){
  pnm ims = pnm_load(ims);
  pnm imt = pnm_load(imt);
  pnm imd = pnm_load(imd);


  pnm_free(ims);
  pnm_free(imt);
  pnm_free(imd);
}

void
usage (char *s){
  fprintf(stderr, "Usage: %s <ims> <imt> <imd> \n", s);
  exit(EXIT_FAILURE);
}

#define PARAM 3
int
main(int argc, char *argv[]){
  if (argc != PARAM+1) usage(argv[0]);
  process(argv[1], argv[2], argv[3]);
  return EXIT_SUCCESS;
}
