#include <stdio.h>
#include <stdlib.h>
#include "glcmmath.h"

#define FEAUTERS 5
#define CONTRAST 0
#define DISSIMILARITY 1
#define HOMOGENITY 2
#define ENERGY 3
#define ASM 4

static inline void glcm_seq(unsigned char *h_imageIn, double* histogram, int* angels, double* sum, int x, int y, int distance, int Nangle, int max_value,int normed)
{
    for(int k = 0; k < Nangle; k++)
    {
        int dy = 0, dx = 0;
        offset(angels[k], distance, &dx, &dy);
        
        int b_i = max(0, -dy);
        int e_i = min(y, y - dy);
        int b_j = max(0, -dx);
        int e_j = min(x, x - dx);

        int K = k * max_value * max_value;
        for(int i = b_i; i < e_i; i++)
        {
            int n_i = i + dy;
            for(int j = b_j; j < e_j; j++)
            {
                
                int n_j = j + dx;
                unsigned char org_pix = h_imageIn[i * x + j]; //<-- ti grejo v v1 (spraviš te podatke v v0,...,v3)
                unsigned char next_pix = h_imageIn[n_i * x + n_j]; // <-- ti grejo v v2
                histogram[K + org_pix * max_value + next_pix] += 1;
                sum[k] += 1;
            }
            
        }
            
        if(normed && sum[k] > 0.0)
        {
            for(int i = 0; i < max_value; i++)
            {
                for(int j = 0; j < max_value; j++)
                {
                    histogram[K + i * max_value + j] = histogram[K + i * max_value + j] / sum[k];
                }
            }
        }
    }
    return;
}


static inline void glcm_feauters(double* histogram, double* feauters, int max_value)
{
    for (int i = 0; i < max_value; ++i)
    {
        for (int j = 0; j < max_value; ++j)
        {
            int idx = i * max_value + j;

            double diff = (double)i - (double)j;
            double weight_con = diff * diff;
            feauters[CONTRAST] += histogram[idx] * weight_con;

            double weight_hom = 1.0 / (1.0 + weight_con);
            feauters[HOMOGENITY] += histogram[idx] * weight_hom;
            
            double weight_dis = fabs((diff));
            feauters[DISSIMILARITY] += histogram[idx] * weight_dis;
            
            
            feauters[ASM] += histogram[idx] * histogram[idx];
        }
    }
    
    feauters[ENERGY] = sqrt(feauters[ASM]);
    return;
}