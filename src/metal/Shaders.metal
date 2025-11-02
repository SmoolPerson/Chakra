#include <metal_stdlib>
using namespace metal;
// This file actually does the mandelbrot computation on the GPU
constant float3 colors_to_interpolate[5] = {
                float3(0.0, 32.0, 128.0),    // deep blue
                float3(32.0, 107.0, 203.0),  // bright blue
                float3(237.0, 255.0, 255.0), // white/cyan highlight
                float3(255.0, 170.0, 0.0),   // orange
                float3(128.0, 0.0, 128.0)    // purple
            };
float lerp(float a, float b, float t) {
    return a + (b-a) * t;
}

float3 get_gradient(float value, constant float3 *gradient, int len) {
    float3 color;

    float sined_val = 0.5 * (sin(value*0.02)+1);
    // Scale the value by the length and mod it by 1 so it matches what the interpolation value should be
    float scaled_val = fmod((sined_val * len), 1);
    int index = (int)(sined_val * len);
    // mod it so it wraps around
    int next_index = (index + 1) % len;
    // interpolate between the indices
	color.x = lerp(gradient[index].r, gradient[next_index].r, scaled_val);
    color.y = lerp(gradient[index].g, gradient[next_index].g, scaled_val);
    color.z = lerp(gradient[index].b, gradient[next_index].b, scaled_val);
	return color;
}

float mandelbrot_point(float cx, float cy) {
    float zx = 0;
    float zy = 0;

    float new_zy = 0; // Temp vars to make sure we dont rewrite stuff

    // Bulb checking: https://en.wikipedia.org/wiki/Plotting_algorithms_for_the_Mandelbrot_set#Cardioid_/_bulb_checking
    float cx14 = cx - 0.25;
    float cy2 = cy * cy;
    float q = (cx14)*(cx14) + cy2;
    if (sqrt(q) - 2 * q + 0.25 >= cx) {
        return 0;
    }
    if ((cx + 1)*(cx + 1) + cy2 <= 0.0625) {
        return 0;
    }
    float magnitude;
    for (size_t i = 1; i < 300; ++i) {
        // This is bounds check; if the distance of x and y from origin is > 32, then point escaped
        if (zx*zx + zy*zy > 1024) {
            magnitude = sqrt(zx*zx + zy*zy);
            // http://www.imajeenyus.com/mathematics/20121112_distance_estimates/distance_estimation_method_for_fractals.pdf
            //dest = (magnitude / magnitude_d) * log(magnitude);
            float value_to_color = i - log(log(magnitude))/log(2.0f); // Smooth coloring
            return value_to_color;
        }
        // Instead of using a single complex object, we split it up into two real equations for slight optimization
        new_zy = (zx + zx) * zy + cy; // add zx to itself for a speedup
        zx = zx*zx - zy*zy + cx;
        zy = new_zy;
    }
    // if point does not escape, then its in the set, so return 0
    return 0;
}

kernel void mandelbrot(device unsigned char *outBuffer [[ buffer(0) ]], const device float2* inBuffer [[buffer(1)]], const device int* aa_num_pts [[buffer(2)]], const device float* step [[buffer(3)]], uint index [[thread_position_in_grid]]) {
    // anti aliasing code
    float total_mandel_value = 0;
    for (int k = 0; k < aa_num_pts[0]; ++k) {
        for (int j = 0; j < aa_num_pts[0]; ++j) {
            float offset_x = step[0] * k * 1/aa_num_pts[0];
            float offset_y = step[0] * j * 1/aa_num_pts[0]; // scale it down by the step so we don't get points too far away
            total_mandel_value += mandelbrot_point(inBuffer[index].x + offset_x, inBuffer[index].y + offset_y);
        }
    }
    float average = total_mandel_value / (aa_num_pts[0] * aa_num_pts[0]);
    if (abs(average) > 1e-2) {
        float3 color = get_gradient(average, colors_to_interpolate, 5);
        outBuffer[index * 3] = color.x;
        outBuffer[index * 3 + 1] = color.y;
        outBuffer[index * 3 + 2] = color.z;
    }
    else {
        outBuffer[index * 3] = 0;
        outBuffer[index * 3 + 1] = 0;
        outBuffer[index * 3 + 2] = 0;
    }
    return;
}