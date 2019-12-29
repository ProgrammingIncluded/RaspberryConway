/***********************************************
 * Project: RaspberryConway
 * File: quadtree.hpp
 * By: ProgrammingIncluded
 * Website: ProgrammingIncluded.github.io
 * License: GNU GPLv3 (see LICENSE file)
***********************************************/
#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include <unordered_map>

typedef long long int lint;
typedef unsigned long long int ulint;

struct Range {
    lint startX; // Inclusive
    lint startY; // Inclusive
    lint endX; // Exclusive
    lint endY; // Exlcusive
};

struct QuadNode {
    bool isLeaf;
    uint id;
    uint pixelCount;
    Range range;
    QuadNode *parent;
    QuadNode *children;
    bool *center;
};

class QuadTree {
    // Unique id for atleast 4 billion nodes
    ulint uidCounter;
    QuadNode *root;
    std::unordered_map<string, QuadNode*> hashlife;

    public:
        // Defined max bounds as given by MAX_TREE_BOUND and MIN_TREE_BOUND
        Range bounds;

        QuadTree(bool *board, uint boardX, uint boardY);
        QuadTree();
        ~QuadTree();

        QuadNode* getNodeFromRange(lint startX, lint endX, lint startY, lint endY);

        uint countPixels(bool *board, Range *range, uint boardX);

        void addPixel(lint x, lint y);

        // Generate tree from given board size
        void addPixel(bool *board, uint boardX, uint boardY);

        bool getPixel(lint x, lint y);

        void removeNode(QuadNode *node);

        QuadNode* getChildFromPoint(lint x, lint y, QuadNode *node);

        void addChildrenForNode(QuadNode *node);

        void printNode(QuadNode *node);

        void generateTree(bool *board, uint boardX);

    private:
        // Disallow copy constructor
        QuadTree(const QuadTree&) = delete;
};

#endif /* QUADTREE_HPP */
