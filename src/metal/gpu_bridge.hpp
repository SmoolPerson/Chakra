#include "../C/cfstruct.hpp"
#ifndef GPU_BRIDGE_H
#define GPU_BRIDGE_H
extern "C" {
unsigned char *fill_rgb_gpu(Config *config, bool logging);
void gpu_init();
}
#endif