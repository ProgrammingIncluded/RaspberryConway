/***********************************************
 * Project: RaspberryConway
 * File: app.cpp
 * By: ProgrammingIncluded
 * Website: ProgrammingIncluded.github.io
 * License: GNU GPLv3 (see LICENSE file)
***********************************************/

#include "app.hpp"

#include <iostream>

// Linux
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>

// KD
#include <linux/kd.h>

// Sig
#include <stdio.h>
#include <cstdlib>
#include <signal.h>
#include <unistd.h>
#include <cstring>

// IOCTL Libaries
#include <termios.h>

#include <linux/types.h>

static const char *FRAMEBUFFER = "/dev/fb0";
static const char *INPUT = nullptr;
static const char *TERMINAL = "/dev/tty0";

int FDSCREEN = 0;

// File descriptor for default render
int FDTERM = 0;

// File descriptor associated with key tty
// Maybe different or same as FDTERM
int FDKEY = 0;

uint *DISPLAY = nullptr;
uint SCREEN_X = 0;
uint SCREEN_Y = 0;

void setupFrameBuffer() {
    // Prep buffer for writing
    FDSCREEN = open(FRAMEBUFFER, O_RDWR);
    fb_var_screeninfo varInfo;
    ioctl(FDSCREEN, FBIOGET_VSCREENINFO, &varInfo);

    // Print Screen Info
    SCREEN_X = varInfo.xres;
    SCREEN_Y = varInfo.yres;

    // Write back file description
    ioctl(FDSCREEN,FBIOPUT_VSCREENINFO, &varInfo);
    DISPLAY = (uint *) mmap(
                            0,
                            SCREEN_X * SCREEN_Y * sizeof(uint),
                            PROT_WRITE | PROT_READ,
                            MAP_SHARED,
                            FDSCREEN,
                            0
                        );
}

void setupKeyInputs() {
    // Get the terminal process, may not be necessary
    INPUT = ttyname(STDIN_FILENO);

    // Set terminal mode for listening to input
    termios termios_p;
    tcgetattr(STDIN_FILENO, &termios_p);
    // Set non-canonical mode and remove echo
    termios_p.c_lflag = termios_p.c_lflag & !ICANON & !ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &termios_p);

    // Open for reading
    FDKEY = open(INPUT, O_RDONLY | O_NONBLOCK);
}

void closeKeyInputs() {
    // Set canonical back
    termios termios_p;
    tcgetattr(STDIN_FILENO, &termios_p);
    // Set non-canonical mode and remove echo
    termios_p.c_lflag = termios_p.c_lflag | ICANON | ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &termios_p);
    close(FDKEY);
}

void closeFrameBuffer() {
    close(FDSCREEN);
    munmap(DISPLAY, SCREEN_X * SCREEN_Y * sizeof(uint));
}

// Shutdown command
void closeApp(int s) {
    // Revert back to text mode
    ioctl(FDTERM, KDSETMODE, KD_TEXT);
    close(FDTERM);
    closeFrameBuffer();
    closeKeyInputs();
    exit(s);
}

// start the application
void startApp() {
    // Setup sigint close action
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = closeApp;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    // Setupt the framebuffer file descriptors
    setupFrameBuffer();

    // Setup the key inputs
    setupKeyInputs();

    // Lock down screen with draw mode
    FDTERM = open(TERMINAL, O_RDWR);
    ioctl(FDTERM, KDSETMODE, KD_GRAPHICS);
}

// Draw the application
void draw(RGBA *vdisplay) {
    memcpy(DISPLAY, vdisplay, sizeof(uint) * SCREEN_X * SCREEN_Y);
}

char getKeyPress() {
    char buffer[1];

    // Very basic single char read
    ssize_t res = read(FDKEY, &buffer, 1);

    if (res <= 0) {
        return 0;
    }

    // Check sigkill from terminal
    if (buffer[0] == 0x03) {
        closeApp(0);
    }

    return buffer[0];
}