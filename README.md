# RaspberryConway
Efficient Conway's Game of Life Implementation for Raspberry Pi 4. Implementation will try to
take advantage of hardware specific optimizations to squeeze as much of the Pi as possible.
The source is kept as light weight as possible with little reliance on heavy libraries except an
optional OpenGLES.

Implementation currently supports direct write to framebuffer using native kernel calls.
GPU OpenGLES support is on hold until OpenGLES API is more solified for Rapsberry Pi 4.
A few lines of source code is available for testing.

## How to Run
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

If not, make sure that `OPENGL_FLAGS` are set correctly in Makefile.
See dependencies on how to install OpenGL on Rapsberry Pi 4.

## Controls
`w`, `a`, `s`, and `d` to move view finder around. `x` to zoom out and `z` to zoom in.

## Dependencies
- Mesa (OpenGL Build Only)
    - `sudo apt-get install libgles2-mesa-dev`
- GBM (OpenGL Build Only)
    - `sudo apt-get install libgbm-dev`

Non-OpenGL build should only require a Raspberry Pi default Linux and C++11 standards.

## Platforms
Any Raspberry Pi with latest Raspbian installation should work.
Tested on Raspberry Pi 4 with 1Gb Ram with Raspbian 4.19.75.

## Future Work
As of Decemeber 30th, 2019, there are no known Raspberry Pi 4 accelerated graphics computing library.
As a result, there is no implementation for game of life using hardware acceleration.
This might be a good future milestone.