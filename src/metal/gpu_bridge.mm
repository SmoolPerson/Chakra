#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#include <simd/simd.h>
#include "gpu_bridge.hpp"
#include "../C/cfstruct.hpp"
#include <chrono>
#include <iostream>
// global variables representing the gpu
id<MTLDevice> device;
id<MTLComputePipelineState> pipeline;
id<MTLCommandQueue> queue;
id<MTLLibrary> lib;        // correct type
id<MTLFunction> func;      // correct type
void gpu_init() {
    // 1. Get GPU device
        device = MTLCreateSystemDefaultDevice();
        if (!device) { NSLog(@"No GPU found"); return; }

        // 2. Load shader
        NSError *libraryError = nil;
    
        // Get the path to Shaders.metallib in the current working directory
        NSString *path = [[NSFileManager defaultManager] currentDirectoryPath];
        NSString *libraryPath = [path stringByAppendingPathComponent:@"build/Shaders.metallib"];
        
        lib = [device newLibraryWithFile:libraryPath error:&libraryError];

        if (!lib) {
            NSLog(@"Error: Could not load Metal library from path: %@", libraryPath);
            if (libraryError) { NSLog(@"Details: %@", libraryError); }
            return;
        }
        
        // Now get the function from the loaded library
        func = [lib newFunctionWithName:@"mandelbrot"];

        // 3. Create compute pipeline
        NSError *error = nil;
        pipeline = [device newComputePipelineStateWithFunction:func error:&error];
        if (error) { NSLog(@"Error: %@", error); return; }

        // 4. Create command queue
        queue = [device newCommandQueue];
}

unsigned char *fill_rgb_gpu(Config *config, bool logging) {
    @autoreleasepool {
        // 5. Create I/O buffers for the GPU
        // goal for the input is to create a grid of points for the GPU to originally transform into rgb colors
        int width = config->WIDTH;
        int height = config->HEIGHT;
        const int length = width * height;
        simd_float2 *inputData = new simd_float2[length];

        // step size is simply the difference in distance between the min and the max divided by number of pixels
        float step = (config->width_max - config->width_min)/config->WIDTH;
        for (int x = 0; x < width; x++) {
            for (int y = height - 1; y > -1; y--) {
                inputData[y * width + x] = simd_float2{(float) (config->width_min + x * step), (float) (config->height_max - y * step)}; // convert 2d array to 1d
            }
        };
        int supersampling = config->ANTI_ALIASING_NUM_PTS;
        id<MTLBuffer> buffer = [device newBufferWithLength:length * 3 options:MTLResourceStorageModeShared];
        id<MTLBuffer> inBuffer = [device newBufferWithBytes:inputData length:length * sizeof(simd_float2) options:MTLResourceStorageModeShared];
        id<MTLBuffer> aaBuffer = [device newBufferWithBytes:&supersampling length:sizeof(int) options:MTLResourceStorageModeShared];
        id<MTLBuffer> stepBuffer = [device newBufferWithBytes:&step length:sizeof(float) options:MTLResourceStorageModeShared];
        // 6. Create command buffer and encoder
        id<MTLCommandBuffer> cmdBuf = [queue commandBuffer];
        id<MTLComputeCommandEncoder> encoder = [cmdBuf computeCommandEncoder];
        [encoder setComputePipelineState:pipeline];
        [encoder setBuffer:buffer offset:0 atIndex:0]; // bind output buffer
        [encoder setBuffer:inBuffer offset:0 atIndex:1]; // bind input buffer
        [encoder setBuffer:aaBuffer offset:0 atIndex:2]; // bind input buffer
        [encoder setBuffer:stepBuffer offset:0 atIndex:3]; // bind input buffer

        MTLSize gridSize = MTLSizeMake(length, 1, 1);
        NSUInteger threadGroupSize = pipeline.maxTotalThreadsPerThreadgroup;
        if (threadGroupSize > length) threadGroupSize = length;
        MTLSize threadgroupSize = MTLSizeMake(threadGroupSize, 1, 1);
        [encoder dispatchThreads:gridSize threadsPerThreadgroup:threadgroupSize];
        [encoder endEncoding];
        [cmdBuf commit];
        [cmdBuf waitUntilCompleted];

        // 7. Read result
        unsigned char *out = (unsigned char*)malloc(length*3); // copy to cpu: cost is negligible but prevents errors in lodepng when i believe it tries to free gpu memory
        memcpy(out, buffer.contents, length*3);
        delete[] inputData;
        return out;
    }
    return nullptr;
}
