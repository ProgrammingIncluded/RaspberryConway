/***********************************************
 * Project: RaspberryConway
 * File: rle_loader.cpp
 * By: ProgrammingIncluded
 * Website: ProgrammingIncluded.github.io
 * License: GNU GPLv3 (see LICENSE.txt)
***********************************************/

#include "rle_loader.hpp"
#include "quadtree.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// Change this to point to a different folder relative to binary execution
const std::string RLE_FOLDER = "rle";

void loadRLE(std::string filename, bool *board, uint board_x, uint board_y, uint offset_x, uint offset_y) {
    // TODO: File integrity should be checked by summing rows
    std::ifstream rleFile(RLE_FOLDER + "/" + filename);

    // A few flag checking
    bool loadedMeta = false;
    bool set_size_x = false;
    bool set_size_y = false;

    // Size of loaded file
    uint size_x = board_x + 1;
    uint size_y = board_y + 1;

    // Needed variables used for reading compressed lines
    std::string buf = "";
    uint write_x = 0;
    uint write_y = 0;
    bool finished = false;

    std::string line;
    while(std::getline(rleFile, line) && !finished) {
        // Skip any and all comments
        if (line[0] == '#') {
            continue;
        }

        // Load metadata to check board size
        if (!loadedMeta) {
            std::string token;
            std::stringstream split(line);
            while(std::getline(split, token, ',')) {
                // Strip any starting spaces
                if (token[0] == ' ') {
                    token.erase(0, 1);
                }

                // Format will always v = <num>
                if (token[0] == 'x') {
                    size_x = std::stoi(token.substr(4));
                    set_size_x = true;
                } else if (token[0] == 'y') {
                    size_y = std::stoi(token.substr(4));
                    set_size_y = true;
                } else {
                    std::cout << "Ignoring metadata: " << token << std::endl;
                }
            }

            // Verify if able to continue with proper size
            if (!set_size_x || !set_size_y) {
                std::cout << "Invalid file given, no size found. Cannot load: " << filename << std::endl;
            } else if (size_x > board_x || size_y > board_y) {
                std::cout << "Given model is larger than allocated memory. Cannot load: " << filename << std::endl;
                std::cout << "Required Size: " << size_x << ", " << size_y << std::endl;
            }

            loadedMeta = true;
            continue;
        }

        // Load the compressed data
        for (char &c : line) {
            switch(c) {
                case '!':
                    finished = true;
                    break;
                case '$':
                    // Assume dead cells if any number before eol
                    // with out a tag. Reset x write head and increment y.
                    buf = "";
                    write_x = 0;
                    write_y += 1;
                    break;
                case ' ':
                    break;
                case 'b': {
                    // Preceeding integers are for deadcells
                    uint value;
                    if (buf == "") {
                        value = 1; // Default one as defined in standard
                    } else {
                        value = std::stoi(buf);
                    }

                    write_x += value;
                    buf = "";
                    break;
                }
                case 'o': {
                    // Preceeding integers are for livecells
                    uint value;
                    if (buf == "") {
                        value = 1; // Default one as defined in standard
                    } else {
                        value = std::stoi(buf);
                    }

                    // Write all the alive cells
                    for (uint x = write_x; x < write_x + value; ++x) {
                        board[x + (write_y * board_x)] = true;
                    }

                    write_x += value;
                    buf = "";
                    break;
                }
                default:
                    // Integer
                    buf += c;
                    break;
            }
        }
    }
}

void loadRLE(std::string filename, QuadTree *qtree) {
       // TODO: File integrity should be checked by summing rows
    std::ifstream rleFile(RLE_FOLDER + "/" + filename);

    // A few flag checking
    bool loadedMeta = false;
    bool set_size_x = false;
    bool set_size_y = false;

    // Size of loaded file
    uint size_x = 0;
    uint size_y = 0;

    // Needed variables used for reading compressed lines
    std::string buf = "";
    uint write_x = 0;
    uint write_y = 0;
    bool finished = false;
    bool *board;

    std::string line;
    while(std::getline(rleFile, line) && !finished) {
        // Skip any and all comments
        if (line[0] == '#') {
            continue;
        }

        // Load metadata to check board size
        if (!loadedMeta) {
            std::string token;
            std::stringstream split(line);
            while(std::getline(split, token, ',')) {
                // Strip any starting spaces
                if (token[0] == ' ') {
                    token.erase(0, 1);
                }

                // Format will always v = <num>
                if (token[0] == 'x') {
                    size_x = std::stoi(token.substr(4)) + 10;
                    set_size_x = true;
                } else if (token[0] == 'y') {
                    size_y = std::stoi(token.substr(4)) + 10;
                    set_size_y = true;
                } else {
                    std::cout << "Ignoring metadata: " << token << std::endl;
                }
            }

            // Verify if able to continue with proper size
            if (!set_size_x || !set_size_y) {
                std::cout << "Invalid file given, no size found. Cannot load: " << filename << std::endl;
            }

            loadedMeta = true;
            board = new bool[size_x * size_y]();
            continue;
        }

        // Load the compressed data
        for (char &c : line) {
            switch(c) {
                case '!':
                    finished = true;
                    break;
                case '$':
                    // Assume dead cells if any number before eol
                    // with out a tag. Reset x write head and increment y.
                    buf = "";
                    write_x = 0;
                    write_y += 1;
                    break;
                case ' ':
                    break;
                case 'b': {
                    // Preceeding integers are for deadcells
                    uint value;
                    if (buf == "") {
                        value = 1; // Default one as defined in standard
                    } else {
                        value = std::stoi(buf);
                    }

                    write_x += value;
                    buf = "";
                    break;
                }
                case 'o': {
                    // Preceeding integers are for livecells
                    uint value;
                    if (buf == "") {
                        value = 1; // Default one as defined in standard
                    } else {
                        value = std::stoi(buf);
                    }

                    // Write all the alive cells
                    for (uint x = write_x; x < write_x + value; ++x) {
                        board[x + (write_y * size_x)] = true;
                    }

                    write_x += value;
                    buf = "";
                    break;
                }
                default:
                    // Integer
                    buf += c;
                    break;
            }
        }
    }

    qtree->addPixel(board, size_x, size_y);
    delete board;
}