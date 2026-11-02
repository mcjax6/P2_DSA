#include "SplayTree.h"

SplayTree::SplayTree() : root(nullptr) {}

SplayTree::~SplayTree() {
    destroyTree(root);
}

void SplayTree::destroyTree(Node* node) {
    if (node) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

Node* SplayTree::rightRotate(Node* x) {
    Node* y = x->left;
    x->left = y->right;
    y->right = x;
    return y;
}

Node* SplayTree::leftRotate(Node* x) {
    Node* y = x->right;
    x->right = y->left;
    y->left = x;
    return y;
}

Node* SplayTree::splay(Node* root, const std::string& key) {
    if (root == nullptr || root->data.name == key)
        return root;

    if (root->data.name > key) {
        if (root->left == nullptr) return root;

        //Zig Zig Rotation (Left Left Rotation)
        if (root->left->data.name > key) {
            root->left->left = splay(root->left->left, key);
            root = rightRotate(root);
        }
        else if (root->left->data.name < key) {
            root->left->right = splay(root->left->right, key);
            if (root->left->right != nullptr)
                root->left = leftRotate(root->left);
        }

        return (root->left == nullptr) ? root : rightRotate(root);
    }
    else {
        if (root->right == nullptr) return root;

        //Zig Zag Rotation (Right Left Rotation)
        if (root->right->data.name > key) {
            root->right->left = splay(root->right->left, key);
            if (root->right->left != nullptr)
                root->right = rightRotate(root->right);
        }
        //Zig Zig Rotation (Right Right Rotation)
        else if (root->right->data.name < key) {
            root->right->right = splay(root->right->right, key);
            root = leftRotate(root);
        }

        return (root->right == nullptr) ? root : leftRotate(root);
    }
}