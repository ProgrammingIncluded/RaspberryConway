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
#include <vector>

#define DEBUG 1

// Framebuffer location
static uint BOARD_X = 512;
static uint BOARD_Y = 512;

// Where the render window should be relative to screen coordinates
static uint POS_X = 0;
static uint POS_Y = 0;

// Set by main to allow scaling to be dynamic from screen size
static uint BOARD_TIMES_X = 1;
static uint BOARD_TIMES_Y = 1;

// Other Controls
static bool PAUSE = false;

// ms per frame
static const double MSPF = 20.0;

static bool *BOARD_BUFFER;
static bool *VIRTUAL_BOARD;

static uint THREADS = 4;

// TODO: Probably want graphics card to do this
// Virtualboard size + padding should be a multiple of screen size 
inline void drawBoard() {
    // Assume screen_x >= board_x, screen_y >= board_y
    // Display buffer
    // Iterate through the viewport which is screen size adjusted by scale and view shift
    for (uint y = POS_Y; y < POS_Y + SCREEN_Y / BOARD_TIMES_Y; y += 1) {
        for(uint x = POS_X; x < POS_X + SCREEN_X / BOARD_TIMES_X; x += 1) {
            // Get pixel from boardspace
            bool cur = VIRTUAL_BOARD[x + (y * BOARD_X)];

            // Convert board to screen space
            uint trans_x = (x - POS_X) * BOARD_TIMES_X;
            uint trans_y = (y - POS_Y) * BOARD_TIMES_Y;

            // Draw entire pixel in screen space with enlarging in mind
            for (uint new_y = trans_y; new_y < trans_y + BOARD_TIMES_Y; ++new_y) {
                for (uint new_x = trans_x; new_x < trans_x + BOARD_TIMES_X; ++new_x) {
                    DISPLAY[new_x + (new_y * SCREEN_X)] = 0xFFFFFFFF * cur;
                }
            }
        }
    }
}

// Soft wrapper to deal with edge cases
// Starts with 0 index
bool getTile(uint x, uint y, bool *vb, uint board_x, uint board_y) {
    if (x >= board_x || y >= board_y || x < 0 || y < 0) {
        return 0;
    }

    return vb[x + (y * board_x)];
}

// Runs one iteration of the board game
// Threadable function
void updateBoard(bool *vb, bool *bb, uint board_x, uint board_y, uint offsetX, uint offsetY) {
    // Naive implementation
    uint boundY = board_y + offsetY;
    for (uint y=offsetY; y < boundY; y += 1) {
        for(uint x=offsetX; x < board_x + offsetX; x += 1) {
            // Current cell
            bool cur = vb[x + (y * BOARD_X)];
            // Get neighbors of tiles
            bool n0 = getTile(x + 1, y, vb, board_x, boundY);
            bool n1 = getTile(x + 1, y + 1, vb, board_x, boundY);
            bool n2 = getTile(x + 1, y - 1, vb, board_x, boundY);
            bool n3 = getTile(x - 1, y + 1, vb, board_x, boundY);
            bool n4 = getTile(x - 1, y - 1, vb, board_x, boundY);
            bool n5 = getTile(x - 1, y, vb, board_x, boundY);
            bool n6 = getTile(x, y + 1, vb, board_x, boundY);
            bool n7 = getTile(x, y - 1, vb, board_x, boundY);

            // Check if alive
            uint res = n0 + n1 + n2 + n3 + n4 + n5 + n6 + n7;

            if (cur) {
                bool alive = (2 <= res && res <= 3);
                bb[x + (y * board_x)] = alive * 0xFFFFFFFF;
            } else {
                bool alive = (res == 3);
                bb[x + (y * board_x)] = alive * 0xFFFFFFFF;
            }
        }
    }
    // Update previous VIRTUAL_BOARD to new buffer
    memcpy(vb + board_x* offsetY, bb + board_x * offsetY, sizeof(bool) * board_x * board_y);
}

void parseInput(char key) {
    if (key == 0) {
        return;
    }

    bool move = false;
    switch(key) {
        case 'z':
            BOARD_TIMES_X *= 2;
            BOARD_TIMES_Y *= 2;
            move=true;
            break;
        case 'x':
            BOARD_TIMES_X /= 2;
            BOARD_TIMES_Y /= 2;
            BOARD_TIMES_X = BOARD_TIMES_X == 0 ? 1 : BOARD_TIMES_X;
            BOARD_TIMES_Y = BOARD_TIMES_Y == 0 ? 1 : BOARD_TIMES_Y;
            move=true;
            break;
        case 'w':
            // UP
            POS_Y = (POS_Y == 0) ? POS_Y : POS_Y - 10;
            move=true;
            break;
        case 's':
            // DOWN
            POS_Y += 10;
            move=true;
            break;
        case 'd':
            // RIGHT
            POS_X += 10;
            move=true;
            break;
        case 'a':
            // LEFT
            POS_X = (POS_X == 0) ? POS_X : POS_X - 10;
            move=true;
            break;
        case 'p':
            PAUSE ^= true;
            move=true;
            break;
    }

    if (move) {
        // Check if we have viewed greater than board space
        if ((POS_Y + SCREEN_Y / BOARD_TIMES_Y) > BOARD_Y) {
            POS_Y = SCREEN_Y - (BOARD_Y / BOARD_TIMES_Y);
        }
        if ((POS_X + SCREEN_X / BOARD_TIMES_X) > BOARD_X) {
            POS_X = SCREEN_X - (BOARD_X / BOARD_TIMES_X);
        }
    }
}

void spawnGlider(uint x, uint y, bool *VIRTUAL_BOARD) {
    if (x + 3 >= BOARD_X || y + 3 >= BOARD_Y) {
        std::cout << "Unable to set glider at: " << x << ", " << y << std::endl;
        return;
    }

    VIRTUAL_BOARD[x + (y * BOARD_X)] = 1;
    VIRTUAL_BOARD[x + 1 + (y + 1 * BOARD_X)] = 1;
    VIRTUAL_BOARD[x + 2 + (y + 1 * BOARD_X)] = 1;
    VIRTUAL_BOARD[x + (y + 2 * BOARD_X)] = 1;
    VIRTUAL_BOARD[x + 1 + (y + 2 * BOARD_X)] = 1;
}

int main(int argc, char *argv[]) {
    startApp();

    std::cout << "Screen Size X: " << SCREEN_X << std::endl;
    std::cout << "Screen Size Y: " << SCREEN_Y << std::endl;

    // Default Board size should always be greater or equal to screen size
    BOARD_X = BOARD_X < SCREEN_X ? SCREEN_X : BOARD_X;
    BOARD_Y = BOARD_Y < SCREEN_Y ? SCREEN_Y : BOARD_Y;

    // Allocate board space
    VIRTUAL_BOARD = new bool[BOARD_X * BOARD_Y]();
    BOARD_BUFFER = new bool[BOARD_X * BOARD_Y]();

    // loadRLE("turingmachine.rle", VIRTUAL_BOARD, BOARD_X, BOARD_Y);


    // Basic intiailization
    spawnGlider(10, 0, VIRTUAL_BOARD);

    // FPS Logic: https://stackoverflow.com/questions/38730273/how-to-limit-fps-in-a-loop-with-c
    std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
    std::chrono::system_clock::time_point b = std::chrono::system_clock::now();

    // Multithreading
    uint threadBoardY = BOARD_Y / THREADS;


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

        if (!PAUSE) {
            // Update board
            std::vector<std::thread> threads;
            for (uint t = 0; t < THREADS; ++t) {
                // Create overlap
                std::thread res(
                    updateBoard,
                    VIRTUAL_BOARD,
                    BOARD_BUFFER,
                    BOARD_X,
                    threadBoardY,
                    0,
                    (BOARD_Y / THREADS) * t
                );
                threads.push_back(std::move(res));
            }

            for (uint t = 0; t < THREADS; ++t) {
                threads.at(t).join();
            }
            drawBoard();
        }
    }

    // This should never reach
    closeApp(9);
}
