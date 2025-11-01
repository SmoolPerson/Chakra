#include <iostream>
#include "lodepng.h"
#include "cfstruct.hpp"
#include "fractalmath.hpp"
#include "meta.hpp"
#include "color.hpp"
#include "../metal/gpu_bridge.hpp"

// Main entry point, check all the other included header files for all the helper functions I use in this
int main(int argc, char *argv[]) {
    puts("Welcome to Mandelbrot! Use the argument -h or --help for usage. If you run it with no arguments it will run with the default ones!");
    std::flush (std::cout);
    Config config = get_default_config();
    parse_args(argc,  argv, &config);
    unsigned char *rgb_data;
    if (config.GPU) {
        std::cout << "Rendering with GPU acceleration enabled." << std::endl;
        gpu_init();
        rgb_data = fill_rgb_gpu(&config, true);
        if (rgb_data == nullptr) {
            std::cerr << "GPU rendering failed due to nullptr return, falling back to CPU rendering." << std::endl;
            rgb_data = generate_mandelbrot(&config, true);
        }
    } else {
        auto startTime = std::chrono::steady_clock::now();
        std::cout << "Rendering with CPU." << std::endl;
        rgb_data = generate_mandelbrot(&config, true);
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime);
        std::cout << "It took " << duration.count() << " seconds.\n";
    }
    puts("\n");

    unsigned error = lodepng_encode24_file(config.OUTPUT_FILENAME, rgb_data, config.WIDTH, config.HEIGHT);
    if (error) {
        printf("PNG encode error %u: %s\n", error, lodepng_error_text(error));
    }
    free(rgb_data); // prevents segfaults
    return 0;
}