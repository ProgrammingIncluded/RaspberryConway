/***********************************************
 * Project: RaspberryConway
 * File: app.hpp
 * By: ProgrammingIncluded
 * Website: ProgrammingIncluded.github.io
 * License: GNU GPLv3 (see LICENSE.txt)
***********************************************/

#ifndef APP_HPP
#define APP_HPP

typedef unsigned int uint;

struct RGBA {
    char B;
    char G;
    char R;
    char A;
};

extern uint SCREEN_X;
extern uint SCREEN_Y;

void setupFrameBuffer();
void closeFrameBuffer();

void closeApp(int s);
void startApp();
void draw(RGBA *vdisplay);

// Keyboard related IO
void setupKeyInputs();
char getKeyPress();

#endif /* APP_HPP */
