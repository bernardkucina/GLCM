#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "glcmvec.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#define MAX_ANGLES 32

static const char* image_name[4] = {
    "./images/frame_0249_jpg.rf.d33720f1304d383c749e18cc08dea6db.jpg",
    "./images/frame_0055_jpg.rf.440b9803a9766de971934b5a891db20f.jpg",
    "./images/frame_0057_jpg.rf.88cee5058d48da760320392c6967ddde.jpg",
    "./images/frame_0252_jpg.rf.f685d90adb983dc8dafa4ae700342521.jpg"
};

// COMPILE FOR C FILES: riscv64-unknown-elf-gcc -o hello glcmVec.c -lm
// RUN (PROXY KERNEL FOR RISC-V): spike --varch=vlen:256,elen:64 --isa=rv64gcv pk ./hello --distance 1 --angles 0 45 90 --normed 1 --optimized 1 --image 0 > test.out
// HELP:  echo | riscv64-unknown-elf-gcc -dM -E -     riscv64-unknown-elf-objdump -d hello > log.out
int main(int argc, char *argv[]) {

    int distance  = 1;
    int normed    = 0;
    int optimized = 0;
    int angles[MAX_ANGLES], nAngles = 0;
    int image = 0;

    if (argc < 7) {
        printf("Usage: %s width height --distance N --angles A1 A2 … --normed B --optimized B --image B\n", argv[0]);
        return 1;
    }

    // Parse flags
    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "--distance") == 0 && i+1 < argc) {
            distance = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--angles") == 0 && i+1 < argc) {
            nAngles = 0;
            while (i+1 < argc && argv[i+1][0] != '-' && nAngles < MAX_ANGLES) {
                angles[nAngles++] = atoi(argv[++i]);
            }
        }
        else if (strcmp(argv[i], "--normed") == 0 && i+1 < argc) {
            normed = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--optimized") == 0 && i+1 < argc) {
            optimized = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--image") == 0 && i+1 < argc) {
            image = atoi(argv[++i]);
        }
    }

    // Report
    printf("Vectorized%s: ", optimized ? "Opt" : "Normal");
    for(int i = 0; i < nAngles; i++){
        printf("%d ", angles[i]);
    }
    printf("\n");

    int width, height, cpp;
    unsigned char *h_imageIn = stbi_load(image_name[image], &width, &height, &cpp, STBI_rgb);
    if (h_imageIn == NULL)
    {
        printf("Error reading loading image!\n");
        exit(EXIT_FAILURE);
    }
    
    
    int* offSize =(int*) calloc(nAngles, sizeof(int));
    double* sum =(double*) calloc(nAngles, sizeof(double));
    
    int maxOff = 0;
    int maximum = 0;

    unsigned char* gray_image = rgb_2_gray(h_imageIn, width, height, cpp, &maximum);
    double* histogram = (double*) calloc(nAngles * maximum * maximum, sizeof(double));
    double** addr = (double**) calloc(nAngles, sizeof(double*));
    uint16_t** offets = index_calloc(angles, width, height, distance, nAngles);
    double* glcm_feat = (double*) calloc(nAngles * FEAUTERS, sizeof(double));

    clock_t t1G = read_cycles();
    offets = glcm_index(gray_image, angles, offSize, &maxOff, sum, offets, width, height, distance, nAngles, maximum);
    glcm_vec(histogram, sum, offets, offSize, addr,  maxOff, nAngles, normed, maximum);
    clock_t t2G = read_cycles();
    
    if(normed)
    {
        clock_t t1F = read_cycles();
        for(int i = 0; i < nAngles; i++)
        {
            glcm_feauters(&histogram[i * maximum * maximum], &glcm_feat[i * FEAUTERS], maximum, optimized);
        }
        clock_t t2F = read_cycles();

        for(int i = 0; i < nAngles; i++)
        {
            // printf("Contrast: %.5f\n", glcm_feat[i * FEAUTERS + CONTRAST]);
            // printf("Dissimilarity: %.5f\n", glcm_feat[i * FEAUTERS + DISSIMILARITY]);
            // printf("Homogeneity: %.5f\n", glcm_feat[i * FEAUTERS + HOMOGENITY]);
            // printf("ASM: %.5f\n", glcm_feat[i * FEAUTERS + ASM]);
            // printf("Energy: %.5f\n", glcm_feat[i * FEAUTERS + ENERGY]);
            // printf("\n");
        }
        printf("TimeFeauters: %ld µs\n", get_time(t1F, t2F));
    }
    printf("TimeGLCM: %ld µs\n", get_time(t1G, t2G));
    /*
    printf("Original image:\n");
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            printf("%d ", image[i * width + j]);
            }
            printf("\n");
            }
            printf("\n");
            */
           /*
           for(int k = 0; k < nAngles; k++)
           {
            printf("GLCM matrix:\n");
            int K = k * maximum * maximum;
            for (int i = 0; i < maximum; i++)
            {
                for (int j = 0; j < maximum; j++)
                {
                    printf("%0.4f ", histogram[K + i * maximum + j]);
                    }
                    printf("\n");
                    }
                    printf("\n");
                    }
                    */

    for(int i = 0; i < nAngles; i++)
    {
        free(offets[i]);
    }
    free(offets);
    free(histogram);
    free(gray_image);
    free(offSize);
    free(sum);
    free(addr);
    free(glcm_feat);
    return 0;
}