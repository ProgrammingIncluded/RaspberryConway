# RaspberryConway
Efficient Conway's Game of Life Implementation for Raspberry Pi. Implementation will try to
take advantage of hardware specific optimizations to squeeze as much of the Pi as possible.
The source is kept as light weight as possible with little reliance on heavy libraries except an
optional OpenGLES.

Implementation currently supports direct write to framebuffer using native kernel calls.
GPU OpenGLES support is on hold until OpenGLES API is more solified for Rapsberry Pi 4.
A few lines of source code is available for testing.

Due to the size of the project, there are no plans to support CLI interface unless there is
sufficient request.

# Controls
`w`, `a`, `s`, and `d` to move view finder around. `x` to zoom out and `z` to zoom in. `p` to pause simulation.

# Implementations
## Naive Implementation
With the basic make function, the project builds a naive implementation. 
The naive implementation currently has runtime tied to the board size (basic matrix simulation.)
You can change the board size in `main.cpp` with `BOARD_X` and `BOARD_Y`.

Due to naive approach and computation limits, no support for fast forwarding generations.

## Hashlife Implementation
Not currently supported, there is a Hashlife Implementation found in `hashlife.hpp`. You can build this using `run_hashlife` make command. This will eventually support fast forwarding of generations.

# How to Run
There are two modes supported, render with CPU or render with GPU via OpenGLES v2.

### CPU Render
```
cd source
make run
```

A binary will also be produced in `bin`.

### GPU Render
This feature is currently on hold until OpenGLES is more stable for Raspberry Pi 4.

```
cd source
make OPENGLES=1 run
```

If not using vanilla OpenGL files, make sure that `OPENGL_FLAGS` are set correctly in Makefile.
See dependencies on how to install OpenGL on Rapsberry Pi 4.

## Load RLE Files
Use the provided `rle_loader.hpp` to load common life game files.

# Dependencies
- Mesa (OpenGL Build Only)
    - `sudo apt-get install libgles2-mesa-dev`
- GBM (OpenGL Build Only)
    - `sudo apt-get install libgbm-dev`

Non-OpenGL build should only require a Raspberry Pi default Linux and C++11 standards.

## Platforms
Any Raspberry Pi with latest Raspbian installation should work. OpenGLES build will require Raspberry Pi 4.
Tested on Raspberry Pi 4 with 1Gb Ram with Raspbian 4.19.75.

# Future Work
As of Decemeber 30th, 2019, there are no known Raspberry Pi 4 accelerated graphics computing library.
As a result, there is no implementation for game of life using hardware acceleration.
This might be a good future milestone.

# Example
Running Turing Machine rle file.

![Turing Machine](screenshot/demo.jpg)