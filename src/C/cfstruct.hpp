#include <iostream>

#ifndef STRUCT_H
#define STRUCT_H

typedef struct {
    bool ANTI_ALIASING;
    unsigned int ANTI_ALIASING_NUM_PTS;
    double COLOR_STEP_MULTIPLIER;
    int COLOR_OFFSET;
    const char *OUTPUT_FILENAME;
    unsigned int MAX_ITER;
    int CUTOFF;
    double width_min;
    double width_max;
    double height_min;
    double height_max;
    unsigned int WIDTH;
    unsigned int HEIGHT;
    bool ITERATION_CHECK;
} Config;

typedef struct {
    float r;
    float g;
    float b;
} rgb_type;
#endif