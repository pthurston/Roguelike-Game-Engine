#include "MapGenerator.hpp"
#include "BSPTree.hpp"
#include "BSPListener.hpp"
#include "Rect.hpp"
#include "Map.hpp"

using namespace std;

BSPGenerator::BSPGenerator(Map &map, long seed, WriteMode writeMode) : MapGenerator(map, seed, writeMode){}

BSPGenerator::~BSPGenerator(){
    delete bspTree;
}

void BSPGenerator::buildMap(bool withActors){
    Rect mapBounds(1, 1, map.width - 2, map.height - 2);
    bspTree = new BSPTree(mapBounds, ROOM_MIN_SIZE + 2, seed);
    bspTree->partition();

    BSPListener listener(*this);
    bspTree->traverseTree(&listener, withActors);
    //bspTree->printTree();
}
