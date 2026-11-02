#ifndef P2_DSA_SPLAYTREE_H
#define P2_DSA_SPLAYTREE_H
#include "Competitor.h"
#include <string>

struct Node {
    Competitor data;
    Node* left;
    Node* right;

    Node(Competitor comp) : data(comp), left(nullptr), right(nullptr) {}
};


class SplayTree {
private:
    Node* root;

    Node* rightRotate(Node* x);
    Node* leftRotate(Node* x);
    Node* splay(Node* root, const std::string& key);
    void destroyTree(Node* node);

public:
    SplayTree();
    ~SplayTree();

    void insert(const Competitor& comp);
    Competitor search(const std::string& key);
};


#endif
