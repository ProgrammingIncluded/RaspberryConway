# RaspberryConway
Efficient Conway's Game of Life Implementation for Raspberry Pi. Implementation will try to
take advantage of hardware specific optimizations to squeeze as much of the Pi as possible.
The source is kept as light weight as possible with little reliance on heavy input libraries.

Implementation currently supports direct write to framebuffer using native kernel calls.
QPU support is under development.

## How to Run
```
cd source
make run
```

A binary will also be produced in `bin`.

## Controls
`w`, `a`, `s`, and `d` to move view finder around. `x` to zoom out and `z` to zoom in.

## Dependencies
None. Should only require a Raspberry Pi default Linux binaries.

## Platforms
Any Raspberry Pi with latest Raspbian installation should work.
Tested on Raspberry Pi 4 with 1Gb Ram with Raspbian 4.19.75.