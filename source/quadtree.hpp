/***********************************************
 * Project: RaspberryConway
 * File: quadtree.hpp
 * By: ProgrammingIncluded
 * Website: ProgrammingIncluded.github.io
 * License: GNU GPLv3 (see LICENSE file)
***********************************************/
#ifndef QUADTREE_HPP
#define QUADTREE_HPP

struct GenData;

typedef long long int lint;
typedef unsigned long long int ulint;
typedef unsigned int uint;

struct Range {
    lint startX; // Inclusive
    lint startY; // Inclusive
    lint endX; // Exclusive
    lint endY; // Exlcusive
};

struct QuadNode {
    bool isLeaf;
    // Length of one side of the square
    uint id;
    uint pixelCount;
    lint sideLength;
    Range range;
    QuadNode *parent;
    QuadNode *children;
    GenData *gd;
};

class QuadTree {
    // Unique id for atleast 4 billion nodes
    ulint uidCounter;
    // std::unordered_map<string, QuadNode*> hashlife;

    public:
        // Defined max bounds as given by MAX_TREE_BOUND and MIN_TREE_BOUND
        Range bounds;
        QuadNode *root;
        GenData *nullData;

        QuadTree();
        ~QuadTree();

        // QuadNode* getNodeFromRange(lint startX, lint endX, lint startY, lint endY);

        void addPixel(lint x, lint y);
        void addPixel(bool *board, lint x, lint y);

        bool getPixel(lint x, lint y);
        bool getNextGenPixel(lint x, lint y);

        void removeNode(QuadNode *node);

        QuadNode* getChildFromPoint(lint x, lint y, QuadNode *node);

        void addChildrenForNode(QuadNode *node);

        void printNode(QuadNode *node);

        GenData *nextGeneration(QuadNode *node);

    private:
        // Disallow copy constructor
        QuadTree(const QuadTree&) = delete;
};

#endif /* QUADTREE_HPP */
