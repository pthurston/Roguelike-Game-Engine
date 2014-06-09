#ifndef _MAP_H_INCLUDED
#define _MAP_H_INCLUDED

class Actor;

#include <math.h>
#include "libtcod.hpp"

struct Tile{
    bool explored;
    bool walkable;
    bool inFOV;
    Tile() : explored(false), walkable(false), inFOV(false) {}
};

class Map{
public:
    enum NodeType{
        ROOT,
        ABOVE_ROOT,
        BELOW_ROOT
    };
    int width, height;
    long seed;
    Tile *tiles;
    //Actor *player;
    TCODColor ambientLight;
    TCODList<Actor *> actors;
    Map *nextLevel;
    Map *prevLevel;
    Map(int width, int height, Map *nextLevel, Map *prevLevel);
    ~Map();
    void init(bool withActors);
    void deleteMaps(NodeType type = ROOT);
    void load(TCODZip &zip, NodeType = ROOT);
    void save(TCODZip &zip, NodeType = ROOT);
    bool isInBounds(int x, int y) const;
    bool isWall(int x, int y) const;
    bool canWalk(int x, int y) const;
    bool isInFOV(int x, int y) const;
    bool isExplored(int x, int y) const;
    void computeFOV();
    void render() const;
};

#endif // _MAP_H_INCLUDED
