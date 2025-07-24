#include <stdbool.h>
#include <stdint.h>

#ifndef STRUCT_H
#define STRUCT_H

typedef struct {
    bool ANTI_ALIASING;
    int ANTI_ALIASING_NUM_PTS;
    double COLOR_STEP_MULTIPLIER;
    int COLOR_OFFSET;
    double SATURATION;
    double BRIGHTNESS;
    char *OUTPUT_FILENAME;
    int MAX_ITER;
    int CUTOFF;
    double width_min;
    double width_max;
    double height_min;
    double height_max;
    int WIDTH;
    int HEIGHT;
    int ITERATION_CHECK;
} Config;

typedef struct {
    float r;
    float g;
    float b;
} rgb_type;
#endif