//
// Created by mcafe on 11/4/2026.
//

#ifndef P2_DSA_REDBLACKTREE_H
#define P2_DSA_REDBLACKTREE_H
#include "Competitor.h"
struct RedBlackNode {

    RedBlackNode *parent, *left, *right;
    bool red = false; //0 black 1 red
    bool isLeft = false; //0 left 1 right child
    Competitor data;

    RedBlackNode(Competitor comp) : data(comp), parent(nullptr), left(nullptr), right(nullptr) {}

};



class RedBlackTree {

    RedBlackNode *root = nullptr;

public:

    void insert(Competitor comp);
    Competitor search(int id);

    void colorFlip(RedBlackNode *node); //flips parent and uncle
    void rotateLeft(RedBlackNode *node); //rotates left about the point
    void rotateRight(RedBlackNode *node); //rotates right about the point


};


#endif //P2_DSA_REDBLACKTREE_H
