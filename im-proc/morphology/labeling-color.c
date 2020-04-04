/**
 * @file  labeling.c
 * @brief count connected components in pnm binary image
*/

#include <stdlib.h>
#include <stdio.h>

#include <pnm.h>
#include <memory.h>
#include <time.h>
int _find(int p, int *roots)
{
  while (roots[p] != p)
    p = roots[p];
  return p;
}

int _union(int r0, int r1, int *roots)
{
  if (r0 == r1)
    return r0;
  if (r0 == -1)
    return r1;
  if (r1 == -1)
    return r0;
  if (r0 < r1)
  {
    roots[r1] = r0;
    return r0;
  }
  else
  {
    roots[r0] = r1;
    return r1;
  }
}

int _add(int p, int r, int *roots)
{
  if (r == -1)
    roots[p] = p;
  else
    roots[p] = r;
  return roots[p];
}

void process(pnm ims,pnm imd, char * filename)
{
  int w = pnm_get_width(ims);
  int h = pnm_get_height(ims);
  unsigned short *ps = pnm_get_channel(ims, NULL, PnmRed);
  int p = 0;
  int r = -1;
  int *roots = memory_alloc(w * h * sizeof(int));

  for (int i = 0; i < h; i++)
  {
    for (int j = 0; j < w; j++)
    {
      r = -1;
      if (j > 0 && (*(ps - 1) == (*ps)))
        r = _union(_find(p - 1, roots), r, roots);
      if ((i > 0 && j > 0) && (*(ps - 1 - w) == (*ps)))
        r = _union(_find(p - 1 - w, roots), r, roots);
      if (i > 0 && (*(ps - w) == (*ps)))
        r = _union(_find(p - w, roots), r, roots);
      if ((j < (w - 1) && i > 0) && (*(ps + 1 - w) == (*ps)))
        r = _union(_find(p + 1 - w, roots), r, roots);
      r = _add(p, r, roots);
      p++;
      ps++;
    }
  }
  for (p = 0; p < w * h; p++)
    roots[p] = _find(p, roots);
  int l = 0;
  for (p = 0; p < w * h; p++)
  {
    if (roots[p] == p)
      roots[p] = l++;
    else
      roots[p] = roots[roots[p]];
  }

  fprintf(stderr, "labeling: %d components found\n", l);

  unsigned short *componentsColorR = malloc(sizeof(unsigned short) * l);
  unsigned short *componentsColorG = malloc(sizeof(unsigned short) * l);
  unsigned short *componentsColorB = malloc(sizeof(unsigned short) * l);
  srand(time(NULL));
  for (int i = 0; i < l; i++)
  {
    componentsColorR[i] = rand() % 256;
    componentsColorG[i] = rand() % 256;
    componentsColorB[i] = rand() % 256;
  }
  for (int i = 0; i < h; i++)
  {
    for (int j = 0; j < w; j++)
    {
      int p = i * w + j;
      if (roots[p] < l && roots[p] > 0)
      {
        pnm_set_component(imd,i,j,PnmRed,componentsColorR[roots[p]]);
        pnm_set_component(imd,i,j,PnmGreen,componentsColorG[roots[p]]);
        pnm_set_component(imd,i,j,PnmBlue,componentsColorB[roots[p]]);
      }
    }
  }
  pnm_save(imd,PnmRawPpm,filename);
  memory_free(roots);
  // memory_free(ps);
}

void usage(char *s)
{
  fprintf(stderr, "%s <ims>\n", s);
  exit(EXIT_FAILURE);
}

#define PARAM 2
int main(int argc, char *argv[])
{
  if (argc != PARAM + 1)
    usage(argv[0]);
  pnm pnm_ims = pnm_load(argv[1]);
  int rows = pnm_get_height(pnm_ims);
  int cols = pnm_get_width(pnm_ims);
  pnm imd = pnm_new(cols,rows,PnmRawPpm);
  process(pnm_ims,imd,argv[2]);
  pnm_free(pnm_ims);
  pnm_free(imd);
  return EXIT_SUCCESS;
}
