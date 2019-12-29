#include "quadtree.hpp"

#include <vector>
#include <iostream>
#include <climits>

// Though QuadTrees can be infinite in nature
// simplify implementation by assuming a very large board space
// 2^16 roughly 16 deep
static const lint MAX_TREE_BOARD = 65536;
static const lint MIN_TREE_BOARD = -65536;

QuadTree::QuadTree() {
    this->uidCounter = 0;
    QuadNode *root = new QuadNode();
    // Make range the largest it can be
    Range range = {MIN_TREE_BOARD, MIN_TREE_BOARD, MAX_TREE_BOARD, MAX_TREE_BOARD};
    root->id = (this->uidCounter)++;
    root->range = range;
    root->parent = nullptr;
    root->children = nullptr;
    root->isLeaf = false;
    // Set to atleast one
    root->pixelCount = 1;

    this->root = root;
    this->bounds = range;
}

QuadTree::QuadTree(bool *board, uint boardX, uint boardY) : QuadTree() {
    addPixel(board, boardX, boardY);
}

QuadTree::~QuadTree() {
    removeNode(this->root);
}

void QuadTree::addPixel(lint x, lint y) {
    QuadNode *cur = this->root;
    Range *range;
    for(;;) {
        range = &cur->range;

        // Don't traverse if we've hit the single digit which is a leaf
        if (cur->isLeaf) {
            // Set pixel count to 1 if node already exists
            cur->pixelCount = 1;
            return ;
        }

        // Keep traversing until a leaf node is hit
        // Add the children if they don't exist yet
        if (cur->children == nullptr) {
            addChildrenForNode(cur);
        }

        // Pick the appropriate child depending on range
        cur = getChildFromPoint(x, y, cur);
    }
}

bool QuadTree::getPixel(lint x, lint y) {
    QuadNode *cur = this->root;
    Range *range;
    for(;;) {
        range = &cur->range;

        if (cur->isLeaf) {
            // This is only true if we've hit a leaf node
            return (cur->pixelCount == 1);
        } else if (cur->children == nullptr) {
            // This is only true if we've exhausted our search
            return false;
        }
        // Pick the appropriate child depending on range
        cur = getChildFromPoint(x, y, cur);
    }
}

QuadNode* QuadTree::getChildFromPoint(lint x, lint y, QuadNode *node) {
    Range *range = &node->range;
    // Check out of bounds of node
    if (x < range->startX || x >= range->endX || y < range->startY || y >= range->endY) {
        return nullptr;
    }

    // Keep adding nodes so long as there are pixels
    lint borderX = range->startX + (range->endX - range->startX) / 2;
    lint borderY = range->startY + (range->endY - range->startY) / 2;

    if (x < borderX && y < borderY) {
        return node->children;
    }
    else if (x < borderX && y >= borderY) {
        return (node->children + 1);
    }
    else if (x >= borderX && y < borderY) {
        return (node->children + 2);
    }

    return (node->children + 3);
}

void QuadTree::addChildrenForNode(QuadNode *node) {
    if (node->children != nullptr) {
        return;
    }

    Range *range = &node->range;
    // Keep adding nodes so long as there are pixels
    lint halfX = (range->endX - range->startX) / 2;
    lint halfY = (range->endY - range->startY) / 2;

    node->children = new QuadNode[4];
    QuadNode *newNode;
    // Add new children with new ranges
    for (uint i = 0; i < 4; ++i) {
        newNode = (node->children + i);
        newNode->id = (this->uidCounter)++;
        // Set atleast one but not zero so that next loop can check
        newNode->pixelCount = 1;
        newNode->parent = node;
        newNode->children = nullptr;
        newNode->isLeaf = false;

        Range r;
        // round down
        // Notice the order
        switch(i) {
            case 0: {
                r.startX = range->startX;
                r.startY = range->startY;
                r.endY = halfY + range->startY;
                r.endX = halfX + range->startX;
                break;
            }
            case 1: {
                r.startX = range->startX;
                r.startY = halfY + range->startY;
                r.endY = range->endY;
                r.endX = halfX + range->startX;
                break;
            }
            case 2: {
                r.startX = halfX + range->startX;
                r.startY = range->startY;
                r.endY = halfY + range->startY;
                r.endX = range->endX;
                break;
            }
            default: {
                r.startX = halfX + range->startX;
                r.startY = halfY + range->startY;
                r.endY = range->endY;
                r.endX = range->endX;
                break;
            }
        }

        // Constrain end points
        // Always have the range be of difference one if halfX is already one
        // This only occurs at end points. This allows for consistent range
        // such that r = [a, b) where a < b and b - a = 1
        // and leaf nodes to exist
        if (halfX < 1) {
            r.endX = 1 - r.startX;
        }

        if (halfY < 1) {
            r.endY = 1 - r.startY;
        }

        if (r.endX - r.startX == 1 && r.endY - r.startY == 1) {
            newNode->isLeaf = true;
        }

        newNode->range = r;
    }
}

void QuadTree::addPixel(bool *board, uint boardX, uint boardY) {
    for (uint y = 0; y < boardY; ++y) {
        for (uint x = 0; x < boardX; ++x) {
            if (board[x + (y * boardX)]) {
                this->addPixel(x, y);
            }
        }
    }
}

uint QuadTree::countPixels(bool *board, Range *range, uint boardX) {
    uint count = 0;
    for (uint y = range->startY; y < range->endY; ++y) {
        for (uint x = range->startX; x < range->endX; ++x) {
            count += board[x + (y * boardX)];
        }
    }
    return count;
}

void QuadTree::generateTree(bool *board, uint boardX) {
    std::vector<QuadNode*> nodes;

    QuadNode *cur;
    Range *range;
    nodes.push_back(root);
    while (!nodes.empty()) {
        cur = nodes.back();
        nodes.pop_back();
        range = &cur->range;

        // Keep adding nodes so long as there are pixels
        uint pixelCount = countPixels(board, range, boardX);
        lint halfX = (range->endX - range->startX) / 2;
        lint halfY = (range->endY - range->startY) / 2;

        cur->pixelCount = pixelCount;

        // Don't traverse if there are no pixels
        if (pixelCount == 0 || (halfX == 0 && halfY == 0)) {
            continue;
        }

        addChildrenForNode(cur);
        for (uint i = 0; i < 4; ++i) {
            nodes.push_back(cur->children + i);
        }
    }
}

void QuadTree::removeNode(QuadNode *node) {
    std::vector<QuadNode*> nodes;
    nodes.push_back(node);

    QuadNode *cur;
    while (!nodes.empty()) {
        cur = nodes.back();
        nodes.pop_back();

        if (cur->children != nullptr) {
            for (uint i = 0; i < 4; ++i) {
                nodes.push_back(&cur->children[i]);
            }
        }

        delete cur;
    }
}

void QuadTree::printNode(QuadNode *node) {
    printf("Start X: %lld \n", node->range.startX);
    printf("End X: %lld \n", node->range.endX);
    printf("Start Y: %lld \n", node->range.startY);
    printf("End Y: %lld \n", node->range.endY);
    printf("IS LEAF: %d \n", node->isLeaf);
}