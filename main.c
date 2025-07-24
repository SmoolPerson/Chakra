#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include "lodepng.h"
#include "cfstruct.h"
#include "fractalm.h"
#include "meta.h"
#include "color.h"

// Main entry point, check all the other included header files for all the helper functions I use in this
int main(int argc, char *argv[]) {
    puts("Welcome to Mandelbrot! Use the argument -h or --help for usage. If you run it with no arguments it will run with the default ones!");

    Config config = get_default_config();
    parse_args(argc,  argv, &config);

    unsigned char *rgb_data = generate_mandelbrot(&config, true);
    puts("\n");

    unsigned error = lodepng_encode24_file(config.OUTPUT_FILENAME, rgb_data, config.WIDTH, config.HEIGHT);
    if (error) printf("PNG encode error %u: %s\n", error, lodepng_error_text(error));

    free(rgb_data);
    return 0;
}