#include <stdio.h>
#include <math.h>
#include <time.h>

#define NUM_DISTANCES 3
#define NUM_ANGLES 4

const double PI = 3.141592653589793;

const int SIN[NUM_DISTANCES][NUM_ANGLES] = {
    {0, 1, 1, 1},   // Distance 1
    {0, 4, 5, 4},   // Distance 5
    {0, 7, 10, 7}   // Distance 10
};

const int COS[NUM_DISTANCES][NUM_ANGLES] = {
    {1, 1, 0, -1},   // Distance 1
    {5, 4, 0, -4},   // Distance 5
    {10, 7, 0, -7}   // Distance 10
};

int max(int a, int b)
{
    return (a > b) ? a : b;
}

int min(int a, int b)
{
    return (a < b) ? a : b;
}

void offset(int angle, int distance, int* dx, int* dy)
{
    if ((angle != 0 || angle != 45 || angle != 90 || angle != 135) && (distance != 1 || distance != 5 || distance != 10))
    {
        double rad_angle = angle * (PI / 180.0);
        *dy = round(sin(rad_angle) * distance);
        *dx = round(cos(rad_angle) * distance);
        return;
    }
    else
    {
        int ix_angle = 0;
        int ix_distance = 0;
        switch (ix_angle)
        {
            case 0:   ix_angle = 0;   break;
            case 45:  ix_angle = 1;  break;
            case 90:  ix_angle = 2; break;
            case 135: ix_angle = 3; break;
            default: printf("Invalid angle! Use 0, 45, 90, or 135 degrees.\n");
            return;
        }
    
        switch (ix_distance)
        {
            case 1:   ix_distance = 0;   break;
            case 3:  ix_distance = 1;  break;
            case 5:  ix_distance = 2; break;
            default: printf("Invalid distance! Use 1, 5, 10.\n");
            return;
        }
    
        *dy = SIN[ix_distance][ix_angle];
        *dx = COS[ix_distance][ix_angle];
        return;
    }
}

static inline unsigned char* rgb_2_gray(unsigned char *h_imageIn, int width, int height, int cpp, int* maximum)
{
    unsigned char *gray_image = (unsigned char*)malloc(sizeof(unsigned char) * width * height);
    int max = 0;
    for (int i = 0; i < width * height; i++)
    {
        int idx = i * cpp;
        unsigned char r = h_imageIn[idx];     // Red
        unsigned char g = h_imageIn[idx + 1]; // Green
        unsigned char b = h_imageIn[idx + 2]; // Blue
        double gray = 0.299 * r + 0.587 * g + 0.114 * b;
        gray_image[i] = (unsigned char)(round(gray));
        if(gray_image[i] > max)
        {
            max = gray_image[i];
        }
    }
    *maximum = (max + 1);
    return gray_image;
}

static inline clock_t read_cycles(void)
{
    return clock();
}

static inline clock_t get_time(clock_t t1, clock_t t2)
{
    return labs(t2 - t1);
}