#include <cmath>
#include "cfstruct.hpp"
#include "fractalmath.hpp"

// File for things that are related to math

// func to calc whether point is part of mandelbrot
// returns num of iters if not in the set, and returns 0 if in the set
// takes cx (real part of c) and cy (complex part of c) as params
double mandelbrot(double cx, double cy, Config *config) {
    double zx = 0;
    double zy = 0;
    double new_zx = 0; // Temp vars to make sure we dont rewrite stuff
    double new_zy = 0;
    double cycle_check_zx = 0;
    double cycle_check_zy = 0;

    // calculating squared versions ahead of time so that we don't need to calculate them twice
    double zx2 = zx * zx;
    double zy2 = zy * zy;

    // Bulb checking: https://en.wikipedia.org/wiki/Plotting_algorithms_for_the_Mandelbrot_set#Cardioid_/_bulb_checking
    double cx14 = cx - 0.25;
    double cy2 = cy * cy;
    double q = (cx14)*(cx14) + cy2;
    if (sqrt(q) - 2 * q + 0.25 >= cx) {
        return 0;
    }
    if ((cx + 1)*(cx + 1) + cy2 <= 0.0625) {
        return 0;
    }
    double magnitude;
    for (size_t i = 1; i < config->MAX_ITER; ++i) {
        // This is bounds check; if the distance of x and y from origin is > 32, then point escaped
        if (zx2 + zy2 > config->CUTOFF) {
            magnitude = sqrt(zx2 + zy2);
            // http://www.imajeenyus.com/mathematics/20121112_distance_estimates/distance_estimation_method_for_fractals.pdf
            //dest = (magnitude / magnitude_d) * log(magnitude);
            return i - log(log(magnitude))/log(2); // Smooth coloring
        }
        // Instead of using a single complex object, we split it up into two real equations for slight optimization
        new_zy = 2 * (zx) * zy + cy;
        new_zx = zx2 - zy2 + cx;

        zx = new_zx;
        zy = new_zy;

        if (config->ITERATION_CHECK) {
            // Every 20 iterations, store a copy of zx and zy, and every iteration check if the values approximately equal. If they do, it's a high chance that there's a cycle.
            if ((i + 1) % 20 == 0) {
                cycle_check_zx = zx;
                cycle_check_zy = zy;
            }
            // Check if zx and zy ~ cycle check
            else if (fabs(zx - cycle_check_zx) < 1e-10 && fabs(zy - cycle_check_zy) < 1e-10) {
                return 0;
            }
        }
        zx2 = zx * zx;
        zy2 = zy * zy;
    }
    // if point does not escape, then its in the set, so return 0
    return 0;
}

double lerp(double a, double b, double t) {
    return a + (b-a) * t;
}

rgb_type get_gradient(double value, rgb_type *gradient, int len) {
    rgb_type color;

    if (value > 1) {
        value = 1;
    }
    if (value < 0) {
        value = 0;
    }
    // Scale the value by the length and mod it by 1 so it matches what the interpolation value should be
    double scaled_val = fmod((value * len), 1);
    int index = (int)(value * len);
    // mod it so it wraps around
    int next_index = (index + 1) % len;
    // interpolate between the indices
	color.r = lerp(gradient[index].r, gradient[next_index].r, scaled_val);
    color.g = lerp(gradient[index].g, gradient[next_index].g, scaled_val);
    color.b = lerp(gradient[index].b, gradient[next_index].b, scaled_val);
	return color;
}