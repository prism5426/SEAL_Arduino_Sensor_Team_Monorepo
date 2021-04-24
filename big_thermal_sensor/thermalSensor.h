
#include "Adafruit_AMG88xx.h"

#define AMG_COLS 8
#define AMG_ROWS 8

#define HD_COLS 24
#define HD_ROWS 24

typedef struct thermalSensorData {
    Adafruit_AMG88xx* amg;
    float* pixels[AMG_COLS * AMG_ROWS];
    float* HDTemp[AMG_COLS * AMG_ROWS]; // this just doesn't work, need help
} thermalSensorData;

void thermalSensorTask (void*);

float get_point(float *p, uint8_t rows, uint8_t cols, int8_t x, int8_t y);
void set_point(float *p, uint8_t rows, uint8_t cols, int8_t x, int8_t y, float f);
void get_adjacents_1d(float *src, float *dest, uint8_t rows, uint8_t cols, int8_t x, int8_t y);
void get_adjacents_2d(float *src, float *dest, uint8_t rows, uint8_t cols, int8_t x, int8_t y);
float cubicInterpolate(float p[], float x);
float bicubicInterpolate(float p[], float x, float y);
void interpolate_image(float *src, uint8_t src_rows, uint8_t src_cols, 
                       float *dest, uint8_t dest_rows, uint8_t dest_cols);
