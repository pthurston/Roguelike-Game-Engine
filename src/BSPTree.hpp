#ifndef _BSPTREE_H_INCLUDED
#define _BSPTREE_H_INCLUDED

class BSPTree;
class BSPListener;

#include "Rect.hpp"
#include "libtcod.hpp"


template<typename T>
class BinNode{
    friend class BSPTree;
    friend class BSPListener;
public:
    BinNode(T);
    T getData();
    bool isLeaf();
private:
    T data;
    BinNode *left;
    BinNode *right;
};

template<typename T>
BinNode<T>::BinNode(T dataIn) : data(dataIn), left(0), right(0){}

template<typename T>
T BinNode<T>::getData(){
    return data;
}

template<typename T>
bool BinNode<T>::isLeaf(){
    return left == NULL && right == NULL;
}

class BSPTree{
public:
    BSPTree(Rect rootSize, int minSize, long seed);
    void partition();
    void traverseTree(BSPListener *listener, bool withActors);
    void printTree();
private:
    BinNode<Rect> *root;
    int minSize;
    TCODRandom *mapRng;
    void traverseTree(BinNode<Rect> *node, BSPListener *listener, bool withActors);
    void splitNodes(BinNode<Rect> *node);
    void printTree(int level, BinNode<Rect> *node);
};

#endif // _BSPTREE_H_INCLUDED
