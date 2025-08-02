#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "glcmmath.h"
#include "rvvconf.h"

#define FEAUTERS 5
#define CONTRAST 0
#define DISSIMILARITY 1
#define HOMOGENITY 2
#define ENERGY 3
#define ASM 4

#define MIN_SIZE -1
const double ONE = 1.0;
const double ZERO = 0.0;

static inline uint16_t** index_calloc(int* angles, int x, int y, int distance, int Nangle)
{
    uint16_t** indexes = (uint16_t**) calloc(Nangle, sizeof(uint16_t));
    for(int k = 0; k < Nangle; k++)
    {
        int dy = 0;
        int dx = 0;
        
        offset(angles[k], distance, &dx, &dy);
        
        int b_i = max(0, -dy);
        int e_i = min(y, y - dy);
        int b_j = max(0, -dx);
        int e_j = min(x, x - dx);

        indexes[k] = (uint16_t*) calloc((e_i - b_i) * (e_j - b_j), sizeof(uint16_t));
    }
    return indexes;
}

static inline uint16_t* index_calloc_opt(int* angles, int x, int y, int distance, int Nangle)
{
    int maximum = 0;
    for(int k = 0; k < Nangle; k++)
    {
        int dy = 0;
        int dx = 0;
        
        offset(angles[k], distance, &dx, &dy);
        
        int b_i = max(0, -dy);
        int e_i = min(y, y - dy);
        int b_j = max(0, -dx);
        int e_j = min(x, x - dx);

        int curr = (e_i - b_i) * (e_j - b_j);
        if(maximum < curr) {
            maximum = curr;
        }

    }
    uint16_t* indexes = (uint16_t*) calloc(maximum, sizeof(uint16_t));
    return indexes;
}

static inline uint16_t** glcm_index(uint16_t *image_1d, int* angles, int* offSize, int* maxOff, double* sum, uint16_t** indexes ,int x, int y, int distance, int Nangle, int maxValue, int lmul)
{
    int MaxSize = MIN_SIZE;
    for(int k = 0; k < Nangle; k++)
    {

        size_t vl;
        int dy = 0;
        int dx = 0;
        
        offset(angles[k], distance, &dx, &dy);
        
        int b_i = max(0, -dy);
        int e_i = min(y, y - dy);
        int b_j = max(0, -dx);
        int e_j = min(x, x - dx);
        
        int size = (e_i - b_i) * (e_j - b_j);
        offSize[k] = size;
        MaxSize = max(MaxSize, size);
        
        uint16_t* ponter = indexes[k];
        int n_j = b_j + dx;
        for(int i = b_i; i < e_i; i++)
        {
            int n_i = i + dy;
            uint16_t *org_pix = &image_1d[i * x + b_j];
            uint16_t *next_pix = &image_1d[n_i * x + n_j];

            size_t remain = e_j - b_j;
            sum[k] += (double) remain;
            while(remain > 0)
            {
                size_t vl = rvv_config_e16(remain, lmul);
                asm volatile (           
                    "vle16.v v0, (%[src1])\n\t"               
                    "vle16.v v8, (%[src2])\n\t"
                    "mv    t2, %[sh]\n\t"
                    "vmadd.vx  v0, t2, v8\n\t"
                    "vse16.v v0, (%[dst])\n\t"
                    :
                    : [src1] "r" (org_pix),
                      [src2] "r" (next_pix),
                      [dst]  "r" (ponter),
                      [sh]   "r"(maxValue)
                    : "t0", "t1", "t2","v0", "v8", "memory"
                );
                remain -= vl;
                ponter += vl;
                org_pix += vl;
                next_pix += vl;
            }    
        }
    }
    *maxOff = MaxSize;
    return indexes;
}

static inline void normed_vec(double* histogram, double sum, int maxValue, int i, int lmul)
{
    if(sum > 0)
    {
        size_t remain = maxValue * maxValue;
        double* ponter = &histogram[i * maxValue * maxValue];
        while(remain > 0)
        {
            size_t vl = rvv_config_e64(remain, lmul);
            asm volatile (
                "vle64.v v0, (%[src])\n\t"
                "fmv.d.x   ft0, %[one]\n\t"
                
                "vfdiv.vf v0, v0, ft0\n\t"
                "vse64.v v0, (%[src])\n\t"
                :
                : [src] "r" (ponter),
                [one] "r" (sum)
                : "t1","ft0","v0", "v1","memory"
            );
            remain -= vl;
            ponter += vl;
         }
    }
    return;
}

static inline void glcm_vec(double* histogram, double* sum, uint16_t** offsets, int* offSize, double** addr, int maxOff, int Nangles, int normed, int maxValue, int lmul)
{
    int ix = 0;
    double** pointer = addr;
    for(int i = 0; i < maxOff; i++)
    {
        int ix = 0;
        pointer = addr;
        for(int j = 0; j < Nangles; j++)
        {
            int J = j * maxValue * maxValue;
            if(i < offSize[j])
            {
                pointer[ix] = &(histogram[J + offsets[j][i]]);
                ix += 1;
            }
        }

        size_t remain = ix;
        while(remain > 0)
        {
            size_t vl = rvv_config_e64(remain, lmul);
            asm volatile (
                "vle64.v v0, (%[src])\n\t"
                "fmv.d.x   ft0, %[one]\n\t"
                
                "vluxei64.v v8, (x0), v0\n\t"
                "vfadd.vf v8, v8, ft0\n\t"
                "vsuxei64.v v8, (x0), v0\n\t"
                :
                : [src] "r" (pointer),
                  [one] "r" (ONE)
                : "t1","ft0","v0","v8","memory"
            );
            
            remain -= vl;
            pointer += vl;
        }
    }

    if(normed) {
        for(int i = 0; i < Nangles; i++)
        {
            normed_vec(histogram, sum[i], maxValue, i, lmul);
        }
    }
    return;
    
}

void glcm_norm_add(double* histogram, uint16_t* offsets, int size, double sum, int maxOff, int normed, int maxValue, int angle, int lmul)
{
    for(int i = 0; i < maxOff; i++)
    {
        int J = angle * maxValue * maxValue;
        histogram[J + offsets[i]] += 1;
        offsets[i] = 0;
    }
    if(normed) {
        normed_vec(histogram, sum, maxValue, angle, lmul);
    }
}

static inline void glcm_vec_opt(double* histogram, uint16_t *image_1d, int* angles, double* sum, uint16_t* indexes ,int x, int y, int distance, int Nangle, int maxValue, int normed, int lmul)
{
    for(int k = 0; k < Nangle; k++)
    {

        size_t vl;
        int dy = 0;
        int dx = 0;
        
        offset(angles[k], distance, &dx, &dy);
        
        int b_i = max(0, -dy);
        int e_i = min(y, y - dy);
        int b_j = max(0, -dx);
        int e_j = min(x, x - dx);
        
        int size = (e_i - b_i) * (e_j - b_j);

        uint16_t* ponter = indexes;
        int n_j = b_j + dx;
        for(int i = b_i; i < e_i; i++)
        {
            int n_i = i + dy;
            uint16_t *org_pix = &image_1d[i * x + b_j];
            uint16_t *next_pix = &image_1d[n_i * x + n_j];

            size_t remain = e_j - b_j;
            sum[k] += (double) remain;
            while(remain > 0)
            {
                size_t vl = rvv_config_e16(remain, lmul);
                asm volatile (           
                    "vle16.v v0, (%[src1])\n\t"               
                    "vle16.v v8, (%[src2])\n\t"
                    "mv    t2, %[sh]\n\t"
                    "vmadd.vx  v0, t2, v8\n\t"
                    "vse16.v v0, (%[dst])\n\t"
                    :
                    : [src1] "r" (org_pix),
                      [src2] "r" (next_pix),
                      [dst]  "r" (ponter),
                      [sh]   "r"(maxValue)
                    : "t1", "t2","v0", "v8", "memory"
                );
                remain -= vl;
                ponter += vl;
                org_pix += vl;
                next_pix += vl;
            }    
        }

        glcm_norm_add(histogram, indexes, size, sum[k], size, normed, maxValue, k, lmul);
    }
    return;
}

static inline void rvv_index(long off, long i)
{
    asm volatile (
        "vid.v   v0\n\t"
        "mv    t2, %[sh]\n\t"
        "mv    t3, %[base]\n\t"
        "vadd.vx v0, v0, t2\n\t"
        "vrsub.vx v0, v0, t3\n\t"
        "vfcvt.f.x.v v0, v0\n\t"
        :
        : [sh]  "r"(off),       
          [base]"r"(i)
        : "t0","t1","t2","ft0","v0","memory"
    );
}

static inline double rvv_contrast(int optimized, int lmul)
{
    double sum = 0.0;
    asm volatile (
        "vfmul.vv  v16, v0, v0\n\t"         
        "vfmul.vv  v16, v8, v16\n\t"   
        :
        :
        : "v0","v8","v16","memory"
    );

    if(optimized){
        rvv_opt(lmul);
    }
    rvv_store_sum(&sum);
    return sum;
}

static inline double rvv_dissimilarity(int optimized, int lmul)
{
    double sum = 0.0;
     if(optimized){
        rvv_reset(lmul);
    }

    asm volatile (
        "vfsgnjx.vv  v16, v0, v0\n\t"         
        "vfmul.vv  v16, v8, v16\n\t"     
        :
        : 
        : "v0","v8","v16","memory"
    );

    if(optimized){
        rvv_opt(lmul);
    }
    rvv_store_sum(&sum);
    return sum;
}

static inline double rvv_homogenity(int optimized, int lmul)
{
    double sum = 0.0;
    if(optimized){
        rvv_reset(lmul);
    }

    asm volatile (        
        "vfmul.vv  v16, v0, v0\n\t"
        "fmv.d.x   ft0, %[one]\n\t"
        "vfadd.vf  v16, v16, ft0\n\t"
        "vfrdiv.vf  v16, v16, ft0\n\t"         
        "vfmul.vv  v16, v8, v16\n\t"     
        :
        :
          [one] "r" (ONE)
        : "ft0","v0","v8","v16",    "memory"
    );

    if(optimized){
        rvv_opt(lmul);
    }
    rvv_store_sum(&sum);
    return sum;
}

static inline double rvv_ASM(int optimized, int lmul)
{
    double sum = 0.0;
    if(optimized){
        rvv_reset(lmul);
    }

    asm volatile (
        "vfmul.vv  v16, v8, v8\n\t"     
        :
        :
        : "v8","v16","memory"
    );

    if(optimized){
        rvv_opt(lmul);
    }
    rvv_store_sum(&sum);
    return sum;
}

static inline void glcm_feauters(double* histogram, double* feauters, int max_value, int optimized, int lmul)
{
    for (int i = 0; i < max_value; ++i)
    {
        size_t remain = max_value;
        double* pointer = &histogram[i * max_value];
        long off = 0;
        while(remain > 0)
        {
            size_t vl = rvv_load_and_config(remain, pointer, lmul);
            rvv_index(off, (long) i);

            feauters[CONTRAST] += rvv_contrast(optimized, lmul);
            feauters[DISSIMILARITY] += rvv_dissimilarity(optimized, lmul);
            feauters[HOMOGENITY] += rvv_homogenity(optimized, lmul);
            feauters[ASM] += rvv_ASM(optimized, lmul);

            remain -= vl;
            pointer += vl;
            off += (long) vl;
        }
    }

    feauters[ENERGY] = sqrt(feauters[ASM]);
    return;
}