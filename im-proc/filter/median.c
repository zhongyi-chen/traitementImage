#include <stdlib.h>
#include <stdio.h>

#include <bcl.h>
#define T 0.25f

float median(int size, float data[])
{
    int temp;
    int i, j;
    for (i = 0; i < size - 1; i++)
    {
        for (j = i + 1; j < size; j++)
        {
            if (data[j] < data[i])
            {
                // swap elements
                temp = data[i];
                data[i] = data[j];
                data[j] = temp;
            }
        }
    }
    return (size % 2 == 0) ? ((data[size / 2] + data[size / 2 - 1]) / 2.f) : data[size / 2];
}

void process(int hs, char *source, char *filename)
{
    int windowSize = (2 * hs + 1);
    pnm ims = pnm_load(source);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    pnm imd = pnm_new(cols, rows, PnmRawPpm);
    int size = rows * cols;
    unsigned short *data = pnm_get_channel(ims, NULL, PnmRed);
    float *imsBuffer = calloc(sizeof(float), size);
    float *window = calloc(sizeof(float), windowSize * windowSize);
    for (int i = 0; i < size; i++)
        imsBuffer[i] = data[i];

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int nb_data = 0;
            for (int x = 0; x < windowSize; x++)
            {
                for (int y = 0; y < windowSize; y++)
                {
                    int new_i = i + x - hs;
                    int new_j = j + y - hs;
                    if (new_i >= 0 && new_i < rows && new_j >= 0 && new_j < cols)
                    {
                        window[nb_data] = data[new_i * cols + new_j];
                        nb_data++;
                    }
                }
            }

            imsBuffer[i * cols + j] = median(nb_data, window);
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
    free(imsBuffer);
}

void usage(char *s)
{
    fprintf(stderr, "Usage: %s <n> <ims> <imd>\n", s);
    exit(EXIT_FAILURE);
}

#define PARAM 3
int main(int argc, char *argv[])
{
    if (argc != PARAM + 1)
        usage(argv[0]);
    int hs = atoi(argv[1]);
    process(hs, argv[2], argv[3]);
    return EXIT_SUCCESS;
}
