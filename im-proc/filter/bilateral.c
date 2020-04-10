#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <bcl.h>
#define T 0.1

void process(int sigma_s, int sigma_g, char *source, char *filename)
{
    int squared_s2 = sigma_s * sigma_s * 2;
    int squared_g2 = sigma_g * sigma_g * 2;
    int hs = (sigma_s * sqrt(-log(T) * 2));
    int windowSize = 2 * hs+1;
    pnm ims = pnm_load(source);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    pnm imd = pnm_new(cols, rows, PnmRawPpm);
    int size = rows * cols;
    unsigned short *data = pnm_get_channel(ims, NULL, PnmRed);
    float *imsBuffer = calloc(sizeof(float), size);
    float *GsTable = calloc(sizeof(float), 256);
    float *GgTable = calloc(sizeof(float), 256);
    for (int k = 0; k < 256; k++)
    {
        GsTable[k] = exp(-(k * k) / (float)squared_s2);
        GgTable[k] = exp(-(k * k) / (float)squared_g2);
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            float dividend = 0;
            float divisor = 0;
            int index = i * cols + j;
            for (int x = 0; x < windowSize; x++)
            {
                for (int y = 0; y < windowSize; y++)
                {
                    int new_i = i + x - hs;
                    int new_j = j + y - hs;
                    if (new_i >= 0 && new_i < rows && new_j >= 0 && new_j < cols)
                    {
                        int pixelDist = abs(i - new_i)  + abs(j - new_j);
                        int newIndex = new_i * cols + new_j;
                        float G_s_g = GsTable[pixelDist] * GgTable[abs(data[index] - data[newIndex])];
                        dividend += G_s_g * data[newIndex];
                        divisor += G_s_g;
                    }
                }
            }
            imsBuffer[index] = dividend / divisor;
        }
    }

    for (int s = 0; s < size; s++)
    {
        float res = imsBuffer[s];
        if (res > 255)
            res = 255.f;
        if (res < 0)
            res = 0.f;
        data[s] = (unsigned short)res;
    }

    pnm_set_channel(imd, data, PnmRed);
    pnm_set_channel(imd, data, PnmGreen);
    pnm_set_channel(imd, data, PnmBlue);

    pnm_save(imd, PnmRawPpm, filename);
    pnm_free(ims);
    pnm_free(imd);
    free(data);
    free(GsTable);
    free(GgTable);
    free(imsBuffer);
}

void usage(char *s)
{
    fprintf(stderr, "Usage: %s <n> <ims> <imd>\n", s);
    exit(EXIT_FAILURE);
}

#define PARAM 4
int main(int argc, char *argv[])
{
    if (argc != PARAM + 1)
        usage(argv[0]);
    int sigma_s = atoi(argv[1]);
    int sigma_g = atoi(argv[2]);
    process(sigma_s, sigma_g, argv[3], argv[4]);
    return EXIT_SUCCESS;
}
