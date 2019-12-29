/***********************************************
 * Project: RaspberryConway
 * File: rle_load.hpp
 * By: ProgrammingIncluded
 * Website: ProgrammingIncluded.github.io
 * License: GNU GPLv3 (see LICENSE file)
***********************************************/
#ifndef RLE_LOADER_HPP
#define RLE_LOADER_HPP

#include <string>

class QuadTree;

extern const std::string RLE_FOLDER;

// Function to load RLE into an array.
// RLE file should bin inside folder defined by REL_FOLDER.
// offset allows an offset to where the file should be loaded in array.
// Currently does not check file integrity
void loadRLE(std::string filename, bool *board, uint board_x, uint board_y, uint offset_x = 0, uint offset_y = 0);
void loadRLE(std::string filename, QuadTree *qtree);

#endif /* RLE_LOADER_HPP */
