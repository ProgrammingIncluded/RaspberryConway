/***********************************************
 * Project: RaspberryConway
 * File: main.cpp
 * By: ProgrammingIncluded
 * Website: ProgrammingIncluded.github.io
 * License: GNU GPLv3 (see LICENSE file)
***********************************************/

#include "app.hpp"
#include "quadtree.hpp"
#include "rle_loader.hpp"

#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>

#define DEBUG 1

// Where the render window should be relative to screen coordinates
static lint POS_X = 0;
static lint POS_Y = 0;

// Set by main to allow scaling to be dynamic from screen size
static uint BOARD_TIMES_X = 1;
static uint BOARD_TIMES_Y = 1;

// ms per frame
static const double MSPF = 200.0;


// TODO: Probably want graphics card to do this
// Virtualboard size + padding should be a multiple of screen size 
inline void drawBoard(QuadTree &field) {
    // Draw only based off screen
    for (uint y = 0; y < SCREEN_Y; ++y) {
        for (uint x = 0; x < SCREEN_X; ++x) {
            // Transform to game space, negative bias by one for even pixel screens
            lint transX = POS_X + x - SCREEN_X / 2;
            lint transY = POS_Y + y - SCREEN_Y / 2;
            DISPLAY[x + (y * SCREEN_X)] = 0xFFFFFFFF * field.getNextGenPixel(transX, transY);
        }
    }
}


// Runs one iteration of the board game
inline void updateBoard() {
    // Naive implementation
    // for (uint y=0; y < BOARD_Y; y += 1) {
    //     for(uint x=0; x < BOARD_X; x += 1) {
    //         // Current cell
    //         bool cur = virtualBoard[x + (y * BOARD_X)];
    //         // Get neighbors of tiles
    //         bool n0 = getTile(x + 1, y, virtualBoard);
    //         bool n1 = getTile(x + 1, y + 1, virtualBoard);
    //         bool n2 = getTile(x + 1, y - 1, virtualBoard);
    //         bool n3 = getTile(x - 1, y + 1, virtualBoard);
    //         bool n4 = getTile(x - 1, y - 1, virtualBoard);
    //         bool n5 = getTile(x - 1, y, virtualBoard);
    //         bool n6 = getTile(x, y + 1, virtualBoard);
    //         bool n7 = getTile(x, y - 1, virtualBoard);

    //         // Check if alive
    //         uint res = n0 + n1 + n2 + n3 + n4 + n5 + n6 + n7;

    //         if (cur) {
    //             bool alive = (2 <= res && res <= 3);
    //             BOARD_BUFFER[x + (y * BOARD_X)] = alive * 0xFFFFFFFF;
    //         } else {
    //             bool alive = (res == 3);
    //             BOARD_BUFFER[x + (y * BOARD_X)] = alive * 0xFFFFFFFF;
    //         }
    //     }
    // }
    // // Update previous virtualBoard to new buffer
    // memcpy(virtualBoard, BOARD_BUFFER, sizeof(bool) * BOARD_X * BOARD_Y);
    // memset(BOARD_BUFFER, 0, BOARD_X * BOARD_Y * sizeof(bool));
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
}

void spawnGlider(QuadTree &field, lint x, lint y) {
    field.addPixel(x, y);
    field.addPixel(x + 1, y);
    field.addPixel(x - 1, y);
}

int main(int argc, char *argv[]) {
    startApp();

    std::cout << "Screen Size X: " << SCREEN_X << std::endl;
    std::cout << "Screen Size Y: " << SCREEN_Y << std::endl;

    QuadTree field;

    // loadRLE("gosperglidergun.rle", &field);
    spawnGlider(field, 0, 0);
    field.nextGeneration(field.root);


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
        // Update board
        updateBoard();
        // Draw the board
        drawBoard(field);
    }

    // This should never reach
    closeApp(9);
}
