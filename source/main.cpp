/***********************************************
 * Project: RaspberryConway
 * File: main.cpp
 * By: ProgrammingIncluded
 * Website: ProgrammingIncluded.github.io
 * License: GNU GPLv3 (see LICENSE file)
***********************************************/

#include "app.hpp"
#include "rle_loader.hpp"

#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>

// Kyeboard
#include <ncurses.h>

#define DEBUG 0

// Framebuffer location
static uint BOARD_X = 100;
static uint BOARD_Y = 100;

// Where the render window should be relative to screen coordinates
static uint POS_X = 0;
static uint POS_Y = 0;

// Set by main to allow scaling to be dynamic from screen size
static uint BOARD_TIMES_X = 1;
static uint BOARD_TIMES_Y = 1;

// ms per frame
static const double MSPF = 20.0;

// TODO: Probably want graphics card to do this
// Virtualboard size + padding should be a multiple of screen size 
inline void drawBoard(bool *virtualBoard) {
    // Assume screen_x >= board_x, screen_y >= board_y
    // Display buffer
    uint buffer[SCREEN_X * SCREEN_Y];

    // Iterate through the viewport which is screen size adjusted by scale and view shift
    for (uint y = POS_Y; y < POS_Y + SCREEN_Y / BOARD_TIMES_Y; y += 1) {
        for(uint x = POS_X; x < POS_X + SCREEN_X / BOARD_TIMES_X; x += 1) {
            // Get pixel from boardspace
            bool cur = virtualBoard[x + (y * BOARD_X)];

            // Convert board to screen space
            uint trans_x = (x - POS_X) * BOARD_TIMES_X;
            uint trans_y = (y - POS_Y) * BOARD_TIMES_Y;

            // Draw entire pixel in screen space with enlarging in mind
            for (uint new_y = trans_y; new_y < trans_y + BOARD_TIMES_Y; ++new_y) {
                for (uint new_x = trans_x; new_x < trans_x + BOARD_TIMES_X; ++new_x) {
                    buffer[new_x + (new_y * SCREEN_X)] = 0xFFFFFFFF * cur;
                }
            }
        }
    }

    draw(reinterpret_cast<RGBA *>(buffer));
}

// Soft wrapper to deal with edge cases
// Starts with 0 index
inline bool getTile(uint x, uint y, bool *virtualBoard) {
    if (x >= BOARD_X || y >= BOARD_Y || x < 0 || y < 0) {
        return 0;
    }

    return virtualBoard[x + (y * BOARD_X)];
}

// Runs one iteration of the board game
inline void updateBoard(bool *virtualBoard) {
    // Naive implementation
    bool buffer[BOARD_X * BOARD_Y];
    for (uint y=0; y < BOARD_Y; y += 1) {
        for(uint x=0; x < BOARD_X; x += 1) {
            // Current cell
            bool cur = virtualBoard[x + (y * BOARD_X)];
            // Get neighbors of tiles
            bool n0 = getTile(x + 1, y, virtualBoard);
            bool n1 = getTile(x + 1, y + 1, virtualBoard);
            bool n2 = getTile(x + 1, y - 1, virtualBoard);
            bool n3 = getTile(x - 1, y + 1, virtualBoard);
            bool n4 = getTile(x - 1, y - 1, virtualBoard);
            bool n5 = getTile(x - 1, y, virtualBoard);
            bool n6 = getTile(x, y + 1, virtualBoard);
            bool n7 = getTile(x, y - 1, virtualBoard);

            // Check if alive
            uint res = n0 + n1 + n2 + n3 + n4 + n5 + n6 + n7;

            if (cur) {
                bool alive = (2 <= res && res <= 3);
                buffer[x + (y * BOARD_X)] = alive * 0xFFFFFFFF;
            } else {
                bool alive = (res == 3);
                buffer[x + (y * BOARD_X)] = alive * 0xFFFFFFFF;
            }
        }
    }
    // Update previous virtualBoard to new buffer
    memcpy(virtualBoard, buffer, sizeof(bool) * BOARD_X * BOARD_Y);
}

void parseInput(char key) {
    if (key == 0) {
        return;
    }

    switch(key) {
        case 'z':
            BOARD_TIMES_X *= 2;
            BOARD_TIMES_Y *= 2;
            break;
        case 'x':
            BOARD_TIMES_X /= 2;
            BOARD_TIMES_Y /= 2;
            BOARD_TIMES_X = BOARD_TIMES_X == 0 ? 1 : BOARD_TIMES_X;
            BOARD_TIMES_Y = BOARD_TIMES_Y == 0 ? 1 : BOARD_TIMES_Y;
            break;
        case 'w':
            // UP
            POS_Y = (POS_Y == 0) ? POS_Y : POS_Y - 10;
            break;
        case 's':
            // DOWN
            POS_Y += 10;
            break;
        case 'd':
            // RIGHT
            POS_X += 10;
            break;
        case 'a':
            // LEFT
            POS_X = (POS_X == 0) ? POS_X : POS_X - 10;
            break;
    }

    // Check if we have viewed greater than board space
    if ((POS_Y + SCREEN_Y / BOARD_TIMES_Y) > BOARD_Y) {
        POS_Y = SCREEN_Y - (BOARD_Y / BOARD_TIMES_Y);
    }
    if ((POS_X + SCREEN_X / BOARD_TIMES_X) > BOARD_X) {
        POS_X = SCREEN_X - (BOARD_X / BOARD_TIMES_X);
    }
}

void spawnGlider(uint x, uint y, bool *virtualBoard) {
    if (x + 3 >= BOARD_X || y + 3 >= BOARD_Y) {
        std::cout << "Unable to set glider at: " << x << ", " << y << std::endl;
        return;
    }

    virtualBoard[x + (y * BOARD_X)] = 1;
    virtualBoard[x + 1 + (y + 1 * BOARD_X)] = 1;
    virtualBoard[x + 2 + (y + 1 * BOARD_X)] = 1;
    virtualBoard[x + (y + 2 * BOARD_X)] = 1;
    virtualBoard[x + 1 + (y + 2 * BOARD_X)] = 1;
}

int main(int argc, char *argv[]) {
    startApp();

    std::cout << "Screen Size X: " << SCREEN_X << std::endl;
    std::cout << "Screen Size Y: " << SCREEN_Y << std::endl;

    // Default Board size should always be greater or equal to screen size
    BOARD_X = BOARD_X < SCREEN_X ? SCREEN_X : BOARD_X;
    BOARD_Y = BOARD_Y < SCREEN_Y ? SCREEN_Y : BOARD_Y;

    // Allocate board space
    bool virtualBoard[BOARD_X * BOARD_Y] = {0};

    loadRLE("gosperglidergun.rle", virtualBoard, BOARD_X, BOARD_Y);

    // Basic intiailization
    //spawnGlider(10, 0, virtualBoard);

    // FPS Logic: https://stackoverflow.com/questions/38730273/how-to-limit-fps-in-a-loop-with-c
    std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
    std::chrono::system_clock::time_point b = std::chrono::system_clock::now();

    // Game Loop
    for(;;) {
        a = std::chrono::system_clock::now();
        std::chrono::duration<double, std::milli> work_time = a - b;

        if (work_time.count() < MSPF)
        {
            std::chrono::duration<double, std::milli> delta_ms(MSPF - work_time.count());
            auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
            std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
        }

        b = std::chrono::system_clock::now();

        #if DEBUG
            std::chrono::duration<double, std::milli> sleep_time = b - a;
            std::cout << "MS per Frame: " << sleep_time.count() << std::endl;
        #endif

        // Map board to display
        parseInput(getKeyPress());
        updateBoard(virtualBoard);
        drawBoard(virtualBoard);
    }

    // This should never reach
    closeApp(9);
}
