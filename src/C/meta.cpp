#include "cfstruct.hpp"
#include <cstring>
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
    printf("  --pixels <int>          Pixels per unit (default: 4096)\n");
    printf("  --iterations <int>      Max iterations (default: 675)\n");
    printf("  --output <filename>     Output filename (default: mandelbrot.png)\n");
    printf("  --anti-aliasing <0|1>   Enable anti-aliasing (default: false)\n");
    printf("  --aa-points <int>       Anti-aliasing sample points (default: 12)\n");
    printf("  --color-mult <float>    Color step multiplier (default: 0.5)\n");
    printf("  --color-offset <int>    Color offset (default: 240)\n");
    printf("  --width <int>           Image width in pixels (default: 256)\n");
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
        else if (strcmp(argv[i], "--anti-aliasing") == 0 && i + 1 < argc) {
            config->ANTI_ALIASING = atoi(argv[++i]);
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
        .ANTI_ALIASING = true,
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
        .ITERATION_CHECK = false
    };
    return config;
} }