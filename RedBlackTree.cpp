//
// Created by mcafe on 11/4/2026.
//

#include "RedBlackTree.h"
#include <vector>

//only use if there is an uncle and is red
void RedBlackTree::colorFlip(RedBlackNode *node) {

    RedBlackNode *parent = node->parent;
    RedBlackNode *grandparent = parent->parent;
    RedBlackNode *uncle;
    if (parent->isLeft) {
        uncle = grandparent->right;
    }
    else {
        uncle = grandparent->left;
    }

    parent->red = false;
    grandparent->red = true;
    uncle->red = false;

}

//only use if uncle and uncle is black. node is the "grandparent"
void RedBlackTree::rotateLeft(RedBlackNode *node) {

    RedBlackNode* rightC = node->right;
    RedBlackNode* rightleftC = rightC->left; //could be null
    RedBlackNode* parent = node->parent; //could be null

    rightC->left = node;
    rightC->parent = parent;
    if (parent && node->isLeft) {
        parent->left = rightC;
        rightC->isLeft = true;
    }
    else if (parent) {
        parent->right = rightC;
        rightC->isLeft = false;
    }
    else {
        root = rightC;
    }

    node->right = rightleftC;
    if (!rightleftC) {
        rightleftC->parent = node;
        rightleftC->isLeft = false;
    }
    node->parent = rightC;
    node->isLeft = true;

    node->red = true;
    rightC->red = false;

}

//only use if uncle and uncle is black. node is grandparent
void RedBlackTree::rotateRight(RedBlackNode *node) {

    RedBlackNode* leftC = node->left;
    RedBlackNode* leftrightC = leftC->right; //could be null
    RedBlackNode* parent = node->parent; //could be null

    leftC->right = node;
    leftC->parent = parent;
    if (parent && node->isLeft) {
        parent->left = leftC;
        leftC->isLeft = true;
    }
    else if (parent) {
        parent->right = leftC;
        leftC->isLeft = false;
    }
    else {
        root = leftC;
    }

    node->left = leftrightC;
    if (!leftrightC) {
        leftrightC->parent = node;
        leftrightC->isLeft = true;
    }
    node->parent = leftC;
    node->isLeft = false;

    node->red = true;
    leftC->red = false;

}


void RedBlackTree::insert(Competitor comp) {
    RedBlackNode *node = new RedBlackNode(comp);
    RedBlackNode *current = root;
    RedBlackNode *next = nullptr;
    std::vector<RedBlackNode*> path;
    if(root) {
        path.push_back(root);
        if (current->data.composite_score > comp.composite_score) {
            next = current->left;
        }
        else {
            next = current->right;
        }
    }
    else {
        root = node;
        root->red = false;
        return;
    }

    while (next) {
        path.push_back(next);
        current = next;
        if (current->data.composite_score > comp.composite_score) {
            next = current->left;
        }
        else {
            next = current->right;
        }
    }

    if (current->data.composite_score > comp.composite_score) {
        current->left = node;
        node->parent = current;
        node->isLeft = true;
        node->red = true;

    }
    else {
        current->right = node;
        node->parent = current;
        node->isLeft = false;
        node->red = true;
    }

    bool lastRed = true; //true if the last element in path was red

    RedBlackNode* curr = node;

    while (curr != root && curr->parent->red == true) {
        RedBlackNode* parent = curr->parent;
        RedBlackNode* grandparent = parent->parent;

        //Parent is left child
        if (parent->isLeft) {
            RedBlackNode* uncle = grandparent->right;

            //Case 1: Uncle is red
            if (uncle != nullptr && uncle->red == true) {
                colorFlip(curr);
                curr = grandparent;
            }
            //Case 2 and 3: Uncle is black
            else {
                if (curr->isLeft == false) {
                    curr = parent;
                    rotateLeft(curr);

                    parent = curr->parent;
                    grandparent = parent->parent;
                }
                parent->red = false;
                grandparent->red = true;
                rotateRight(grandparent);
            }
        }
        //Parent is right child
        else {
            RedBlackNode* uncle = grandparent->left;

            //Case 1: Uncle is red
            if (uncle != nullptr && uncle->red == true) {
                colorFlip(curr);
                curr = grandparent;
            }
            //Case 2 and 3: Uncle is black
            else {
                if (curr->isLeft == true) {
                    curr = parent;
                    rotateRight(curr);

                    parent = curr->parent;
                    grandparent = parent->parent;
                }
                parent->red = false;
                grandparent->red = true;
                rotateLeft(grandparent);
            }
        }
    }

    root->red = false;
}

//blank if not found
Competitor RedBlackTree::search(int id) {

    RedBlackNode *current = root;
    while (current) {

        if (current->data.id == id) {

            return current->data;

        }

        if (current->data.id > id) {

            current = current->left;

        }
        else {
            current = current->right;
        }

    }

    return Competitor();

}