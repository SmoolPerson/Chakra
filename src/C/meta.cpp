#include "cfstruct.hpp"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "meta.hpp"

// Meta stuff like config and parsing args
void print_help() {
    printf("Mandelbrot Set Generator\n");
    printf("Usage: ./mandelbrot [options]\n\n");
    printf("Options:\n");
    printf("  --width-min <float>     Left boundary (default: -2.0)\n");
    printf("  --width-max <float>     Right boundary (default: 0.5)\n");
    printf("  --height-min <float>    Bottom boundary (default: -1.0)\n");
    printf("  --height-max <float>    Top boundary (default: 1.0)\n");
    printf("  --iterations <int>      Max iterations (default: 675)\n");
    printf("  --output <filename>     Output filename (default: mandelbrot.png)\n");
    printf("  --aa-points <int>       Anti-aliasing sample points (default: 12)\n");
    printf("  --color-mult <float>    Color step multiplier (default: 0.5)\n");
    printf("  --color-offset <int>    Color offset (default: 240)\n");
    printf("  --width <int>           Image width in pixels (default: 256)\n");
    printf("  --gpu <bool>            Whether to use the GPU to render the image (default: false)\n");
    printf("  --height <int>          Image height in pixels (default: 256)\n");
    printf("  --help, -h              Show this help message\n");
    exit(0);
}

void parse_args(int argc, char *argv[], Config *config) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_help();
        }
        else if (strcmp(argv[i], "--width-min") == 0 && i + 1 < argc) {
            config->width_min = atof(argv[++i]);
        }
        else if (strcmp(argv[i], "--width-max") == 0 && i + 1 < argc) {
            config->width_max = atof(argv[++i]);
        }
        else if (strcmp(argv[i], "--height-min") == 0 && i + 1 < argc) {
            config->height_min = atof(argv[++i]);
        }
        else if (strcmp(argv[i], "--height-max") == 0 && i + 1 < argc) {
            config->height_max = atof(argv[++i]);
        }
        else if (strcmp(argv[i], "--iterations") == 0 && i + 1 < argc) {
            config->MAX_ITER = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc) {
            config->OUTPUT_FILENAME = argv[++i];
        }
        else if (strcmp(argv[i], "--aa-points") == 0 && i + 1 < argc) {
            config->ANTI_ALIASING_NUM_PTS = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--color-mult") == 0 && i + 1 < argc) {
            config->COLOR_STEP_MULTIPLIER = atof(argv[++i]);
        }
        else if (strcmp(argv[i], "--color-offset") == 0 && i + 1 < argc) {
            config->COLOR_OFFSET = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--width") == 0 && i + 1 < argc) {
            config->WIDTH = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--height") == 0 && i + 1 < argc) {
            config->HEIGHT = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--gpu") == 0 && i + 1 < argc) {
            std::string gpu_arg = argv[++i];
            if (gpu_arg == "true" || gpu_arg == "1") {
                config->GPU = true;
            } 
            else {
                config->GPU = false;
            }
        }
        else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            fprintf(stderr, "Use --help for usage information\n");
            exit(1);
        }
    }
}

// Extern "C" makes it callable from C code, like Python's ctypes
extern "C" {Config get_default_config() {
    Config config = {
        .ANTI_ALIASING_NUM_PTS = 12,
        .COLOR_STEP_MULTIPLIER = 10,
        .COLOR_OFFSET = 240,
        .OUTPUT_FILENAME = "mandelbrot.png",
        .MAX_ITER = 300,
        .CUTOFF = 1024,
        .width_min = -2,
        .width_max = 2,
        .height_min = -2,
        .height_max = 2,
        .WIDTH = 256,
        .HEIGHT = 256,
        .ITERATION_CHECK = false,
        .GPU = false
    };
    return config;
} }