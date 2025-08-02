#include <stdlib.h>
extern const double ZERO;

static inline unsigned long get_size_e16_m8(size_t remain)
{
    size_t vl;
    asm volatile(
      "vsetvli %0, %1, e16, m8\n\t"
      : "=r"(vl)
      : "r"(remain)
    );
    return vl;
}

static inline unsigned long get_size_e16_m4(size_t remain)
{
    size_t vl;
    asm volatile(
      "vsetvli %0, %1, e16, m4\n\t"
      : "=r"(vl)
      : "r"(remain)
    );
    return vl;
}

static inline unsigned long get_size_e16_m2(size_t remain)
{
    size_t vl;
    asm volatile(
      "vsetvli %0, %1, e16, m2\n\t"
      : "=r"(vl)
      : "r"(remain)
    );
    return vl;
}

static inline unsigned long get_size_e16_m1(size_t remain)
{
    size_t vl;
    asm volatile(
      "vsetvli %0, %1, e16, m1\n\t"
      : "=r"(vl)
      : "r"(remain)
    );
    return vl;
}

static inline unsigned long get_size_e64_m8(size_t remain)
{
    size_t vl;
    asm volatile(
      "vsetvli %0, %1, e64, m8\n\t"
      : "=r"(vl)
      : "r"(remain)
    );
    return vl;
}

static inline unsigned long get_size_e64_m4(size_t remain)
{
    size_t vl;
    asm volatile(
      "vsetvli %0, %1, e64, m4\n\t"
      : "=r"(vl)
      : "r"(remain)
    );
    return vl;
}

static inline unsigned long get_size_e64_m2(size_t remain)
{
    size_t vl;
    asm volatile(
      "vsetvli %0, %1, e64, m2\n\t"
      : "=r"(vl)
      : "r"(remain)
    );
    return vl;
}

static inline unsigned long get_size_e64_m1(size_t remain)
{
    size_t vl;
    asm volatile(
      "vsetvli %0, %1, e64, m1\n\t"
      : "=r"(vl)
      : "r"(remain)
    );
    return vl;
}

static inline size_t rvv_config_e16_m8(size_t remain)
{
    size_t vl = get_size_e16_m8(remain);
    asm volatile (     
        "mv t0, %[cols]\n\t"                 
        "vsetvli t1, t0, e16, m8\n\t"
        :
        :
          [cols] "r" (vl)
        : "t0","memory"
    );

    return vl;
}


static inline size_t rvv_config_e16_m4(size_t remain)
{
    size_t vl = get_size_e16_m4(remain);
    asm volatile (     
        "mv t0, %[cols]\n\t"                 
        "vsetvli t1, t0, e16, m4\n\t"
        :
        :
          [cols] "r" (vl)
        : "t0","memory"
    );

    return vl;
}


static inline size_t rvv_config_e16_m2(size_t remain)
{
    size_t vl = get_size_e16_m2(remain);
    asm volatile (     
        "mv t0, %[cols]\n\t"                 
        "vsetvli t1, t0, e16, m2\n\t"
        :
        :
          [cols] "r" (vl)
        : "t0","memory"
    );

    return vl;
}


static inline size_t rvv_config_e16_m1(size_t remain)
{
    size_t vl = get_size_e16_m1(remain);
    asm volatile (     
        "mv t0, %[cols]\n\t"                 
        "vsetvli t1, t0, e16, m1\n\t"
        :
        :
          [cols] "r" (vl)
        : "t0","memory"
    );

    return vl;
}

static inline size_t rvv_config_e64_m8(size_t remain)
{
    size_t vl = get_size_e64_m8(remain);
    asm volatile (     
        "mv t0, %[cols]\n\t"                 
        "vsetvli t1, t0, e64, m8\n\t"
        :
        :
          [cols] "r" (vl)
        : "t0","memory"
    );

    return vl;
}

static inline size_t rvv_config_e64_m4(size_t remain)
{
    size_t vl = get_size_e64_m4(remain);
    asm volatile (     
        "mv t0, %[cols]\n\t"                 
        "vsetvli t1, t0, e64, m4\n\t"
        :
        :
          [cols] "r" (vl)
        : "t0","memory"
    );

    return vl;
}

static inline size_t rvv_config_e64_m2(size_t remain)
{
    size_t vl = get_size_e64_m2(remain);
    asm volatile (     
        "mv t0, %[cols]\n\t"                 
        "vsetvli t1, t0, e64, m2\n\t"
        :
        :
          [cols] "r" (vl)
        : "t0","memory"
    );

    return vl;
}

static inline size_t rvv_config_e64_m1(size_t remain)
{
    size_t vl = get_size_e64_m1(remain);
    asm volatile (     
        "mv t0, %[cols]\n\t"                 
        "vsetvli t1, t0, e64, m1\n\t"
        :
        :
          [cols] "r" (vl)
        : "t0","memory"
    );

    return vl;
}

static inline void rvv_reset_m8()
{
    asm volatile (
        "vsetvli t1, t0, e64, m8\n\t"
        :
        :
        : "t1","t0","memory"
    );
}

static inline void rvv_reset_m4()
{
    asm volatile (
        "vsetvli t1, t0, e64, m4\n\t"
        :
        :
        : "t1","t0","memory"
    );
}

static inline void rvv_reset_m2()
{
    asm volatile (
        "vsetvli t1, t0, e64, m2\n\t"
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
        "vfredusum.vs v24, v16, v24\n\t"    
        "vfmv.f.s  ft1, v24\n\t"             
        "fsd        ft1, 0(%[dst])\n\t"
        :
        : [dst]  "r"(sum),
          [base] "r"(ZERO)
        : "ft0","ft1","v16","v24","memory"
    );
}

static inline void rvv_opt_m8()
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

static inline void rvv_opt_m4()
{
    asm volatile(
        "vsetvli t1, x0, e64, m1\n\t"
        "vfadd.vv  v16, v16, v17\n\t"
        "vfadd.vv  v18, v18, v19\n\t"
        "vfadd.vv  v16, v16, v18\n\t"
        :
        :
        : "t1","v16","v17","v18","v19","memory"
    );
}

static inline void rvv_opt_m2()
{
    asm volatile(
        "vsetvli t1, x0, e64, m1\n\t"
        "vfadd.vv  v16, v16, v17\n\t"
        :
        :
        : "t1","v16","v17","memory"
    );
}

static inline size_t rvv_config_e16(size_t remain, int lmul)
{
    switch (lmul) {
    case 1: return rvv_config_e16_m1(remain);
    case 2: return rvv_config_e16_m2(remain);
    case 4: return rvv_config_e16_m4(remain);
    case 8: return rvv_config_e16_m8(remain);
    default: return 0;
    }
}

static inline size_t rvv_config_e64(size_t remain, int lmul)
{
    switch (lmul) {
    case 1: return rvv_config_e64_m1(remain);
    case 2: return rvv_config_e64_m2(remain);
    case 4: return rvv_config_e64_m4(remain);
    case 8: return rvv_config_e64_m8(remain);
    default: return 0;
    }
}

static inline void rvv_opt(int lmul)
{
    switch (lmul) {
    case 1: return;
    case 2: return rvv_opt_m2();
    case 4: return rvv_opt_m4();
    case 8: return rvv_opt_m8();
    default: return;
    }   
}

static inline void rvv_reset(int lmul)
{
    switch (lmul) {
    case 1: return;
    case 2: return rvv_reset_m2();
    case 4: return rvv_reset_m4();
    case 8: return rvv_reset_m8();
    default: return;
    }   
}

static inline size_t rvv_load_and_config(size_t n, double *src1, int lmul)
{
    size_t vl = rvv_config_e64(n, lmul);
    asm volatile (
        "vle64.v v8, (%[ptr])\n\t"
        :
        : [ptr] "r"(src1)    
        : "v8","memory"
    );

    return vl;
}