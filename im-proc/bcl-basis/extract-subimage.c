#include <stdlib.h>
#include <stdio.h>

#include <bcl.h>
/**
 * Load an image from parameter ims
 * 
 **/ 
void process(const size_t i, const size_t j, const size_t rows, const size_t cols, size_t ims, size_t imd)
{

}
void 
usage (char *s)
{
  fprintf(stderr,"Usage: %s %s", s, "<i> <j> <rows> <cols> <ims> <imd>\n");
  exit(EXIT_FAILURE);
}

#define PARAM 6
int 
main(int argc, char *argv[])
{
  if (argc != PARAM+1) usage(argv[0]);

  size_t i = atoi(argv[1]);
  size_t j = atoi(argv[2]);
  size_t rows = atoi(argv[3]);
  size_t cols = atoi(argv[4]);
  size_t ims = atoi(argv[5]);
  size_t imd = atoi(argv[6]);
  return EXIT_SUCCESS;
}
