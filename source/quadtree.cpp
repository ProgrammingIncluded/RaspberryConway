#include "quadtree.hpp"

#include <vector>
#include <iostream>
#include <climits>
#include "game.hpp"

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
    root->sideLength = range.endX - range.startX;
    // Set to atleast one
    root->pixelCount = 1;
    root->gd = nullptr;

    this->root = root;
    this->bounds = range;

    this->nullData = setupNullData();
}

QuadTree::~QuadTree() {
    removeNode(this->root);
}

GenData* QuadTree::nextGeneration(QuadNode *node) {
    if (node == nullptr) {
        return this->nullData;
    } else if (node->gd != nullptr) {
        return node->gd;
    }
    else if (node->sideLength == 2) {
        // Return all dead cells
        if (node->children == nullptr) {
            return this->nullData;
        }

        // Just set the data for future iterations
        char data;
        data |= ((bool) (node->children[0].pixelCount)) << 3; // nw
        data |= ((bool) (node->children[2].pixelCount)) << 2; // ne
        data |= ((bool) (node->children[1].pixelCount)) << 1; // sw
        data |= ((bool) (node->children[1].pixelCount)); // se

        GenData *cached = getBasis(data);
        node->gd = cached;
        return cached;
    }
    else if (node->sideLength == 4) {
        if (node->children == nullptr) {
            return this->nullData;
        }

        GenData d = {0};

        // Get gen data from length of side 4
        d.nw = nextGeneration(node->children);
        d.sw = nextGeneration(node->children + 1);
        d.ne = nextGeneration(node->children + 2);
        d.se = nextGeneration(node->children + 3);

        GenData *cached;
        if (!getGenData(&cached, d)) {
            // Set data bits for new center if not cached
            cached->data = life_4(d.nw, d.ne, d.sw, d.se);
        }

        // Set to node for easier retrieval
        node->gd = cached;
        return cached;
    }

    // Everything else, go two t
    if(node->children == nullptr) {
        return this->nullData;
    }

    QuadNode *nwNode = node->children;
    QuadNode *swNode = node->children + 1;
    QuadNode *neNode = node->children + 2;
    QuadNode *seNode = node->children + 3;

    // Grab length 4 centers
    GenData result = {0};
    result.nw = nextGeneration(nwNode);
    result.sw = nextGeneration(swNode);
    result.ne = nextGeneration(neNode);
    result.se = nextGeneration(seNode);

    GenData *cached;

    // Generate missing sides
    GenData nn = {0};
    nn.nw = result.nw;
    nn.ne = result.ne;
    nn.sw = result.nw;
    nn.se = result.ne;

    // Check cache
    if(!getGenData(&cached, nn)) {
        cached->data = life_4(result.nw, result.ne, result.nw, result.ne);
    }

    result.nn = cached;

    GenData ss = {0};
    nn.nw = result.sw;
    nn.ne = result.se;
    nn.sw = result.sw;
    nn.se = result.se;

    // Check cache
    if(!getGenData(&cached, ss)) {
        cached->data = life_4(result.sw, result.se, result.sw, result.se);
    }

    result.ss = cached;

    GenData cc = {0};
    nn.nw = result.nw;
    nn.ne = result.ne;
    nn.sw = result.sw;
    nn.se = result.se;

    // Check cache
    if(!getGenData(&cached, cc)) {
        cached->data = life_4(result.nw, result.ne, result.sw, result.se);
    }

    result.cc = cached;

    GenData ee = {0};
    nn.nw = result.nw;
    nn.ne = result.nw;
    nn.sw = result.sw;
    nn.se = result.sw;

    // Check cache
    if(!getGenData(&cached, ee)) {
        cached->data = life_4(result.ne, result.ne, result.se, result.se);
    }

    result.ee = cached;

    GenData ww = {0};
    nn.nw = result.nw;
    nn.ne = result.ne;
    nn.sw = result.sw;
    nn.se = result.se;

    // Check cache
    if(!getGenData(&cached, ww)) {
        cached->data = life_4(result.nw, result.nw, result.sw, result.sw);
    }

    result.ww = cached;

    GenData *resultCached;
    getGenData(&resultCached, result);
    return resultCached;

    // nn->data |= (bool) (result->nw->ne->data & 0b00000001) << 3; // nw
    // nn->data |= (bool) (result->ne->nw->data & 0b00000010) << 2; // ne
    // nn->data |= (bool) (result->nw->se->data & 0b00000100) << 1; // sw
    // nn->data |= (bool) (result->ne->sw->data & 0b00001000); // se

    // ss->data |= (bool) (result->sw->ne->data & 0b00000001) << 3; // nw
    // ss->data |= (bool) (result->se->nw->data & 0b00000010) << 2; // ne
    // ss->data |= (bool) (result->sw->se->data & 0b00000100) << 1; // sw
    // ss->data |= (bool) (result->se->sw->data & 0b00001000); // se

    // ee->data |= (bool) (result->ne->sw->data & 0b00000001) << 3; // nw
    // ee->data |= (bool) (result->ne->se->data & 0b00000010) << 2; // ne
    // ee->data |= (bool) (result->se->nw->data & 0b00000100) << 1; // sw
    // ee->data |= (bool) (result->se->ne->data & 0b00001000); // se

    // ww->data |= (bool) (result->nw->sw->data & 0b00000001) << 3; // nw
    // ww->data |= (bool) (result->nw->se->data & 0b00000010) << 2; // ne
    // ww->data |= (bool) (result->sw->nw->data & 0b00000100) << 1; // sw
    // ww->data |= (bool) (result->sw->ne->data & 0b00001000); // se

    // cc->data |= (bool) (result->nw->se->data & 0b00000001) << 3; // nw
    // cc->data |= (bool) (result->ne->sw->data & 0b00000010) << 2; // ne
    // cc->data |= (bool) (result->sw->ne->data & 0b00000100) << 1; // sw
    // cc->data |= (bool) (result->se->nw->data & 0b00001000); // se
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

void QuadTree::addPixel(bool *board, lint boardX, lint boardY) {
    for (uint x = 0; x < boardX; ++x) {
        for (uint y = 0; y < boardY; ++y) {
            if (board[x + (y * boardY)]) {
                this->addPixel(x, y);
            }
        }
    }
}

bool QuadTree::getNextGenPixel(lint x, lint y) {
    QuadNode *cur = this->root;
    Range *range;
    for(;;) {
        range = &cur->range;

        if (cur->sideLength == 2) {
            lint borderX = range->startX + (range->endX - range->startX) / 2;
            lint borderY = range->startY + (range->endY - range->startY) / 2;
            if (x < borderX && y < borderY)
                return nw(cur->gd);
            else if (x >= borderX && y < borderY)
                return ne(cur->gd);
            else if (x < borderX && y >= borderY)
                return sw(cur->gd);
            else
                return se(cur->gd);

        } else if (cur->children == nullptr) {
            // This is only true if we've exhausted our search
            return false;
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
        newNode->gd = nullptr;
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
        newNode->sideLength = r.endX - r.startX;
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