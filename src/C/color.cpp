#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "cfstruct.hpp"
#include "fractalmath.hpp"

// color a hue based on how many iterations it took
rgb_type calculate_hue(double mandelbrot_val, Config *config) {
    rgb_type rgb;
    // using sine so it wrap smoothly between 0 and 1
    double hue_val = 0.5 * (sin((mandelbrot_val)*config->COLOR_STEP_MULTIPLIER + config->COLOR_OFFSET)+1);
    // Hoping to make this a setting soon instead of hardcoding it
    rgb_type grad[5] = {
{0, 32, 128},   // deep blue
{32, 107, 203}, // bright blue
{237, 255, 255},// white/cyan highlight
{255, 170, 0},  // orange
{128, 0, 128}   // deep purple instead of black
};
    int len = (int)(sizeof(grad)/sizeof(grad[0]));
    rgb = get_gradient(hue_val, grad, len);
    return rgb;
}

void get_and_set_pixel_color(int imagex, int imagey, unsigned char *rgb_data, Config *config) {
    rgb_type rgb;

    // Where should we write the rgb color inside the buffer
    int rgb_write_index = (imagey * config->WIDTH + imagex) * 3;

    double step = (config->width_max - config->width_min)/config->WIDTH;

    double cx = config->width_min + imagex * step;
    double cy = config->height_min + imagey * step;

    double mandelbrot_val;
    // We do some anti-aliasing here, so we can get a smoother image
    // The implementation here is to select some values at a random offset, and avg them
    double anti_aliasing_sum = 0.0;
    for (size_t i = 0; i < config->ANTI_ALIASING_NUM_PTS; ++i) {
        // Divide by randmax to normalize, and multiply by step
        double rand_x_offset = ((double)rand()/(double)RAND_MAX) * step;
        double rand_y_offset = ((double)rand()/(double)RAND_MAX) * step;
        double randomized_cx = cx + rand_x_offset;
        double randomized_cy = cy + rand_y_offset;
        anti_aliasing_sum += mandelbrot(randomized_cx, randomized_cy, config);
    }
    mandelbrot_val = anti_aliasing_sum/config->ANTI_ALIASING_NUM_PTS;
    // If mandelbrot is part of set, color black, else, color a hue based on how many iterations it took
    if (mandelbrot_val == 0) {
        rgb_data[rgb_write_index] = 0;
        rgb_data[rgb_write_index+1] = 0;
        rgb_data[rgb_write_index+2] = 0;
    } else {
        rgb = calculate_hue(mandelbrot_val, config);

        rgb_data[rgb_write_index] = (unsigned char)rgb.r;
        rgb_data[rgb_write_index+1] = (unsigned char)rgb.g;
        rgb_data[rgb_write_index+2] = (unsigned char)rgb.b;
    }
}

// This is useful for the python renderer, having a generate_mandelbrot to just write to a pixel buffer, and makes the code in main neater overall.
// Extern "C" makes it callable from C code, like Python's ctypes
extern "C" {unsigned char *generate_mandelbrot(Config *config, bool logging) {
    double mandelbrot_width = config->width_max - config->width_min;
    double mandelbrot_height = config->height_max - config->height_min;
    double screen_ratio = (double)config->WIDTH/(double)config->HEIGHT;
    double mandelbrot_ratio = mandelbrot_width / mandelbrot_height;
    if (fabs(screen_ratio - mandelbrot_ratio) > 1e-3) {
        fprintf(stderr, "%u %u\n", config->WIDTH, config->HEIGHT);
        fprintf(stderr, "WARNING: WIDTH and HEIGHT should be the same aspect ratio as the borders of the mandelbrot image or unexpected results can occur!\n");
        fprintf(stderr, "Floating point precision errors can also cause this.\n");
    }

    // Set RNG gen
    srand(time(NULL));

    // it is an unsigned char since rgb data is one byte array with digits 0-255, and we multiply by 4 for the rgba values
    unsigned char *rgb_data = (unsigned char *) malloc(config->WIDTH * config->HEIGHT * 3);
    if (!rgb_data) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    int percent = 0;
    int past_percent = -1;
    for (size_t y = 0; y < config->HEIGHT; ++y) {
        // formula to convert to percent is a/b * 100
        percent = (y*100/config->HEIGHT);
        if (percent > past_percent) {
            past_percent = percent;
            if (logging) {
                printf("\r%d percent done generating", percent);
                fflush(stdout);
            }
        }
        for (size_t x = 0; x < config->WIDTH; ++x) {
            get_and_set_pixel_color(x, y, rgb_data, config);
        }
    }
    return rgb_data;
}}

// Helper function to free the buffer allocated by generate_mandelbrot via python
extern "C" {
void free_buf(unsigned char *buf) {
    free(buf);
}}