#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "glcmmath.h"

#define FEAUTERS 5
#define CONTRAST 0
#define DISSIMILARITY 1
#define HOMOGENITY 2
#define ENERGY 3
#define ASM 4

#define MIN_SIZE -1
const double ONE = 1.0;
const double ZERO = 0.0;

unsigned long get_size_e16_m8(size_t remain)
{
    size_t vl;
    asm volatile(
      "vsetvli %0, %1, e16, m8\n\t"
      : "=r"(vl)
      : "r"(remain)
    );
    return vl;
}

unsigned long get_size_e16_m1(size_t remain)
{
    size_t vl;
    asm volatile(
      "vsetvli %0, %1, e16, m1\n\t"
      : "=r"(vl)
      : "r"(remain)
    );
    return vl;
}

unsigned long get_size_e64_m8(size_t remain)
{
    size_t vl;
    asm volatile(
      "vsetvli %0, %1, e64, m8\n\t"
      : "=r"(vl)
      : "r"(remain)
    );
    return vl;
}

unsigned long get_size_e64_m1(size_t remain)
{
    size_t vl;
    asm volatile(
      "vsetvli %0, %1, e64, m1\n\t"
      : "=r"(vl)
      : "r"(remain)
    );
    return vl;
}

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

static inline uint16_t** glcm_index(unsigned char *image_1d, int* angles, int* offSize, int* maxOff, double* sum, uint16_t** indexes ,int x, int y, int distance, int Nangle, int maxValue)
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
            unsigned char *org_pix = &image_1d[i * x + b_j];
            unsigned char *next_pix = &image_1d[n_i * x + n_j];

            size_t remain = e_j - b_j;
            sum[k] += (double) remain;
            while(remain > 0)
            {
                size_t vl = get_size_e16_m8(remain);
                asm volatile (     
                    "mv t0, %[cols]\n\t"                 
                    "vsetvli t1, t0, e8,  m8\n\t"             
                    "vle8.v v0, (%[src1])\n\t"               
                    "vle8.v v8, (%[src2])\n\t"

                    "vsetvli x0, x0, e16, m8\n\t"
                    "vzext.vf2 v16, v0\n\t"
                    "vzext.vf2 v24, v8\n\t"
                    "mv    t2, %[sh]\n\t"
                    "vmadd.vx  v16, t2, v24\n\t"
                    "vse16.v v16, (%[dst])\n\t"
                    :
                    : [src1] "r" (org_pix),
                      [src2] "r" (next_pix),
                      [dst]  "r" (ponter),
                      [cols] "r" (vl),
                      [sh]   "r"(maxValue)
                    : "t0", "t1", "t2","v0", "v8", "v16", "v24","memory"
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

static inline void normed_vec(double* histogram, double* sum, int Nangles, int maxValue)
{
    for(int i = 0; i < Nangles; i++)
    {
        if(sum[i] > 0)
        {
            size_t remain = maxValue * maxValue;
            double* ponter = &histogram[i * maxValue * maxValue];
            while(remain > 0)
            {
                size_t vl = get_size_e64_m8(remain);
                asm volatile (
                    "mv    t0, %[n]\n\t"
                    "vsetvli t1, t0, e64, m8\n\t"
                    "vle64.v v0, (%[src])\n\t"
                    "fmv.d.x   ft0, %[one]\n\t"
                    
                    "vfdiv.vf v0, v0, ft0\n\t"
                    "vse64.v v0, (%[src])\n\t"
                    :
                    : [n]"r"(vl),
                    [src] "r" (ponter),
                    [one] "r" (sum[i])
                    : "t0","t1","ft0","v0", "v1","memory"
                );
                remain -= vl;
                ponter += vl;
            }
        }
    }
    return;
}

static inline void glcm_vec(double* histogram, double* sum, uint16_t** offsets, int* offSize, double** addr, int maxOff, int Nangles, int normed, int maxValue)
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
            size_t vl = get_size_e64_m8(remain);
            asm volatile (
                "mv    t0, %[n]\n\t"
                "vsetvli t1, t0, e64, m8\n\t"
                "vle64.v v0, (%[src])\n\t"
                "fmv.d.x   ft0, %[one]\n\t"
                
                "vluxei64.v v8, (x0), v0\n\t"
                "vfadd.vf v8, v8, ft0\n\t"
                "vsuxei64.v v8, (x0), v0\n\t"
                :
                : [n]"r"(vl),
                  [src] "r" (pointer),
                  [one] "r" (ONE)
                : "t0","t1","ft0","v0","v8","memory"
            );
            
            remain -= vl;
            pointer += vl;
        }
    }

    if(normed) {
        normed_vec(histogram, sum, Nangles, maxValue);
    }
    return;
    
}

static inline void rvv_reset()
{
    asm volatile (
        "vsetvli t1, t0, e64, m8\n\t"
        :
        :
        : "t1","t0","memory"
    );
}

static inline void rvv_store_sum(double* sum)
{
    asm volatile (
        "fmv.d.x   ft0, %[base]\n\t"
        "vfmv.v.f  v24, ft0\n\t"           
        "vfredosum.vs v24, v16, v24\n\t"    
        "vfmv.f.s  ft1, v24\n\t"             
        "fsd        ft1, 0(%[dst])\n\t"
        :
        : [dst]  "r"(sum),
          [base] "r"(ZERO)
        : "ft0","ft1","v16","v24","memory"
    );
}

static inline void rvv_opt()
{
    asm volatile(
        "vsetvli t1, x0, e64, m1\n\t"
        "vfadd.vv  v16, v16, v17\n\t"
        "vfadd.vv  v18, v18, v19\n\t"
        "vfadd.vv  v20, v20, v21\n\t"
        "vfadd.vv  v22, v22, v23\n\t"
        "vfadd.vv  v16, v16, v18\n\t"
        "vfadd.vv  v20, v20, v22\n\t"
        "vfadd.vv  v16, v16, v20\n\t"
        :
        :
        : "t1","v16","v17","v18","v19","v20","v21","v22","v23","memory"
    );
}

static inline void rvv_load_and_config(size_t n, double *src1)
{
    asm volatile (
        "mv    t0, %[n]\n\t"
        "vsetvli t1, t0, e64, m8\n\t"
        "vle64.v v8, (%[ptr])\n\t"
        :
        : [n]   "r"(n),      
          [ptr] "r"(src1)    
        : "t1", "t0", "v8", "memory"
    );
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

static inline double rvv_contrast(int optimized)
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
        rvv_opt();
    }
    rvv_store_sum(&sum);
    return sum;
}

static inline double rvv_dissimilarity(int optimized)
{
    double sum = 0.0;
     if(optimized){
        rvv_reset();
    }

    asm volatile (
        "vfsgnjx.vv  v16, v0, v0\n\t"         
        "vfmul.vv  v16, v8, v16\n\t"     
        :
        : 
        : "v0","v8","v16","memory"
    );

    if(optimized){
        rvv_opt();
    }
    rvv_store_sum(&sum);
    return sum;
}

static inline double rvv_homogenity(int optimized)
{
    double sum = 0.0;
    if(optimized){
        rvv_reset();
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
        rvv_opt();
    }
    rvv_store_sum(&sum);
    return sum;
}

static inline double rvv_ASM(int optimized)
{
    double sum = 0.0;
    if(optimized){
        rvv_reset();
    }

    asm volatile (
        "vfmul.vv  v16, v8, v8\n\t"     
        :
        :
        : "v8","v16","memory"
    );

    if(optimized){
        rvv_opt();
    }
    rvv_store_sum(&sum);
    return sum;
}

static inline void glcm_feauters(double* histogram, double* feauters, int max_value, int optimized)
{
    for (int i = 0; i < max_value; ++i)
    {
        size_t remain = max_value;
        double* pointer = &histogram[i * max_value];
        long off = 0;
        while(remain > 0)
        {
            size_t vl = get_size_e64_m8(remain);
            rvv_load_and_config(vl, pointer);
            rvv_index(off, (long) i);

            feauters[CONTRAST] += rvv_contrast(optimized);
            feauters[DISSIMILARITY] += rvv_dissimilarity(optimized);
            feauters[HOMOGENITY] += rvv_homogenity(optimized);
            feauters[ASM] += rvv_ASM(optimized);

            remain -= vl;
            pointer += vl;
            off += (long) vl;
        }
    }

    feauters[ENERGY] = sqrt(feauters[ASM]);
    return;
}