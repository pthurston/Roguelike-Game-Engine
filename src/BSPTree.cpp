#include <stdio.h>
#include "BSPTree.hpp"
#include "BSPListener.hpp"

using namespace std;

BSPTree::BSPTree(Rect rootSize, int minSize, long seed) : minSize(minSize){
    mapRng = new TCODRandom(seed, TCOD_RNG_CMWC);
    root = new BinNode<Rect>(rootSize);
}

void BSPTree::partition(){
    splitNodes(root);
}

void BSPTree::traverseTree(BSPListener *listener, bool withActors){
    traverseTree(root, listener, withActors);
}

void BSPTree::traverseTree(BinNode<Rect> *node, BSPListener *listener, bool withActors){
    if(node == NULL)
        return;
    listener->visitNode(node, withActors);
    traverseTree(node->left, listener, withActors);
    traverseTree(node->right, listener, withActors);
}

void BSPTree::splitNodes(BinNode<Rect> *node){
    if(node == NULL)
        return;

    Rect area = node->getData();
    if(area.width() < 4 * minSize && area.height() < 4 * minSize){
        return;
    }
    bool cutVertical = area.width() > area.height() ? true : false;
    if(cutVertical){
        int cutPos = mapRng->getInt(area.x1 + minSize -1, area.x2 - minSize);
        Rect subArea1(area.x1, area.y1, cutPos, area.y2);
        Rect subArea2(cutPos + 1, area.y1, area.x2, area.y2);
        node->left = new BinNode<Rect>(subArea1);
        node->right = new BinNode<Rect>(subArea2);
    }else{
        int cutPos = mapRng->getInt(area.y1 + minSize -1, area.y2 - minSize);
        Rect subArea1(area.x1, area.y1, area.x2, cutPos);
        Rect subArea2(area.x1, cutPos + 1, area.x2, area.y2);
        node->left = new BinNode<Rect>(subArea1);
        node->right = new BinNode<Rect>(subArea2);
    }
    splitNodes(node->left);
    splitNodes(node->right);
}

void BSPTree::printTree(){
    printTree(0, root);
}

void BSPTree::printTree(int level, BinNode<Rect> *node){

    if(node == NULL)
        return;

    for(int i = 0; i < level; i++)
        printf("\t");
    node->data.printRect();
    printTree(level + 1, node->left);
    printTree(level + 1, node->right);
}

