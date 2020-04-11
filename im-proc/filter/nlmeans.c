#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <bcl.h>
#include <stdbool.h>
#define WINDOWSIZE 11
#define R 3
float normalizedDist(int p[], int q[], pnm ims, int rows, int cols)
{
    int n = 0;
    float dist = 0;
    for (int u = -R; u <= R; u++)
    {
        for (int v = -R; v <= R; v++)
        {
            int new_pi = p[0] + u;
            int new_qi = q[0] + u;
            int new_pj = p[1] + v;
            int new_qj = q[1] + v;
            // new_pi = new_pi >= 0 && new_pi ? new_pi : p[0];
            // new_qi = new_qi >= 0 && new_qi ? new_qi : q[0];
            // new_pj = new_pj >= 0 && new_pj ? new_pj : p[1];
            // new_qj = new_qj >= 0 && new_qj ? new_qj : q[1];
            bool check_p = new_pi >= 0 && new_pi < rows && new_pj >= 0 && new_pj < cols;
            bool check_q = new_qi >= 0 && new_qi < rows && new_qj >= 0 && new_qj < cols;
            if (check_p && check_q)
            {
                int data_p = check_p ? pnm_get_component(ims, new_pi, new_pj, 0) : 0;
                int data_q = check_q ? pnm_get_component(ims, new_qi, new_qj, 0) : 0;
                int data = data_p - data_q;
                dist += data * data;
                n++;
            }
        }
    }

    return dist / (float)n;
}

void process(int sigma, char *source, char *filename)
{
    int squared_s2 = sigma * sigma * 2;
    int hs = (WINDOWSIZE - 1) / 2;
    pnm ims = pnm_load(source);
    int rows = pnm_get_height(ims);
    int cols = pnm_get_width(ims);
    pnm imd = pnm_new(cols, rows, PnmRawPpm);
    int size = rows * cols;
    unsigned short *data = pnm_get_channel(ims, NULL, PnmRed);
    float *imsBuffer = calloc(sizeof(float), size);
    for (int i = 0; i < size; i++)
    {
        imsBuffer[i] = data[i];
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            float Cp = 0;
            float weights = 0;
            int index = i * cols + j;
            for (int x = 0; x < WINDOWSIZE; x++)
            {
                for (int y = 0; y < WINDOWSIZE; y++)
                {
                    int new_i = i + x - hs;
                    int new_j = j + y - hs;
                    if (new_i >= 0 && new_i < rows && new_j >= 0 && new_j < cols)
                    {
                        int p[2] = {i, j};
                        int q[2] = {new_i, new_j};
                        float k = normalizedDist(p, q, ims, rows, cols);
                        float weight = exp(-k / squared_s2);
                        Cp += weight;
                        weights += weight * data[new_i * cols + new_j];
                    }
                }
            }
            imsBuffer[index] = weights / Cp;
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
    int sigma = atoi(argv[1]);
    process(sigma, argv[2], argv[3]);
    return EXIT_SUCCESS;
}
