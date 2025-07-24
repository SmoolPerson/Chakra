from ctypes import Structure, c_int, c_bool, c_double, c_char_p, CDLL, POINTER, c_ubyte, cast
import numpy as np;
import pygame
import time

class Config(Structure):
    _fields_ = [
        ("ANTI_ALIASING", c_bool),
        ("ANTI_ALIASING_NUM_PTS", c_int),
        ("COLOR_STEP_MULTIPLIER", c_double),
        ("COLOR_OFFSET", c_int),
        ("SATURATION", c_double),
        ("BRIGHTNESS", c_double),
        ("OUTPUT_FILENAME", c_char_p),
        ("MAX_ITER", c_int),
        ("CUTOFF", c_int),
        ("width_min", c_double),
        ("width_max", c_double),
        ("height_min", c_double),
        ("height_max", c_double),
        ("WIDTH", c_int),
        ("HEIGHT", c_int),
        ("ITERATION_CHECK", c_int)
    ]
def get_mandelbrot(config, res):
    # load mandelbrot compiled lib
    lib = CDLL('./build/libmandelbrot.so')
    # set arguments and returns of all functions we need to call in c
    lib.generate_mandelbrot.argtypes = [POINTER(Config), c_bool]
    lib.generate_mandelbrot.restype = POINTER(c_ubyte)

    lib.free_buf.argtypes = [POINTER(c_ubyte)]
    lib.free_buf.restype = None

    lib.get_default_config.restype = Config

    # Get default config if the user doesn't provide a config
    if config is not None:
        cfg = config
    else:
        cfg = lib.get_default_config()

    start_time = time.time()
    ptr = lib.generate_mandelbrot(cfg, False)
    width = cfg.WIDTH
    height = cfg.HEIGHT

    # Multiply by three since there are three channels in the output
    buffer_size = width * height * 3

    # We need to cast the ptr to array the function returns to an actual array
    byte_array = cast(ptr, POINTER(c_ubyte * buffer_size)).contents

    # Convert to numpy for compatibility with pygame
    np_array = np.ctypeslib.as_array(byte_array)
    end_time = time.time()

    # Reshape it as a 3d tensor
    np_array = np.reshape(np_array, (height, width, 3))
    before_rotate_surf = pygame.surfarray.make_surface(np_array)

    # Rotate surf by 90 cuz for some strange reason pygame renders it sideways
    surf = pygame.transform.rotate(before_rotate_surf, 90)

    # append to result object if there is a res value; useful for using threading.Thread
    res.append(surf)
    end_time = time.time()
    print(f"Done rendering: took {round((end_time - start_time)*100)/100} seconds")
    print(f"Bounds: {config.width_min}, {config.width_max}, {config.height_min}, {config.height_max}")
    
def get_default_config():
    import os
    cwd = os.getcwd()
    
    lib = CDLL(f'{cwd}/build/libmandelbrot.so')
    lib.get_default_config.restype = Config
    cfg = lib.get_default_config()
    return cfg