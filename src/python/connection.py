from ctypes import Structure, c_int, c_bool, c_double, c_char_p, CDLL, POINTER, c_ubyte, cast, c_uint
import numpy as np;
import pygame
import time

class Config(Structure):
    _fields_ = [
        ("ANTI_ALIASING_NUM_PTS", c_uint),
        ("COLOR_STEP_MULTIPLIER", c_double),
        ("COLOR_OFFSET", c_int),
        ("OUTPUT_FILENAME", c_char_p),
        ("MAX_ITER", c_uint),
        ("CUTOFF", c_int),
        ("width_min", c_double),
        ("width_max", c_double),
        ("height_min", c_double),
        ("height_max", c_double),
        ("WIDTH", c_uint),
        ("HEIGHT", c_uint),
        ("ITERATION_CHECK", c_bool),
        ("GPU", c_bool)  # new field added
    ]
lib = CDLL('./build/libmandelbrot.so')
def init_gpu():
    lib.gpu_init.restype = None
    lib.gpu_init()

def get_mandelbrot(config, res):
    # load mandelbrot compiled lib
    # set arguments and returns of all functions we need to call in c
    lib.generate_mandelbrot.argtypes = [POINTER(Config), c_bool]
    lib.generate_mandelbrot.restype = POINTER(c_ubyte)

    lib.free_buf.argtypes = [POINTER(c_ubyte)]
    lib.free_buf.restype = None

    lib.fill_rgb_gpu.argtypes = [POINTER(Config), c_bool]
    lib.fill_rgb_gpu.restype = POINTER(c_ubyte)

    lib.get_default_config.restype = Config

    # Get default config if the user doesn't provide a config
    if config is not None:
        cfg = config
    else:
        cfg = lib.get_default_config()

    start_time = time.time()
    ptr = None
    print(cfg.GPU)
    if not cfg.GPU:
        ptr = lib.generate_mandelbrot(cfg, False)
    else:
        ptr = lib.fill_rgb_gpu(cfg, False)
    width = cfg.WIDTH
    height = cfg.HEIGHT

    # Multiply by three since there are three channels in the output
    buffer_size = width * height * 3

    # We need to cast the ptr to array the function returns to an actual array
    byte_array = cast(ptr, POINTER(c_ubyte * buffer_size)).contents

    # Convert to numpy for compatibility with pygame
    np_array = np.ctypeslib.as_array(byte_array)
    end_time = time.time()

    # Reshape it as a 3d tensor and make surface
    np_array = np.reshape(np_array, (height, width, 3))
    before_rotate_surf = pygame.surfarray.make_surface(np_array)

    # Rotate surf by -90 cuz for some strange reason pygame renders it sideways
    surf = pygame.transform.rotate(before_rotate_surf, 90)
    # flip if gpu is true, dont ask me why this works
    if cfg.GPU:
        surf = pygame.transform.flip(surf, False, True)

    # append to result object if there is a res value; useful for using threading.Thread
    res.append(surf)
    end_time = time.time()
    print(f"Done rendering: took {round((end_time - start_time)*100)/100} seconds")
    print(f"Bounds: width_min: {config.width_min}, width_max: {config.width_max}, height_min: {config.height_min}, height_max: {config.height_max}")
    
def get_default_config():
    import os
    cwd = os.getcwd()
    try: 
        lib = CDLL(f'{cwd}/build/libmandelbrot.so')
    except:
        raise Exception("Failed to load libmandelbrot.so")
    lib.get_default_config.restype = Config
    cfg = lib.get_default_config()
    return cfg