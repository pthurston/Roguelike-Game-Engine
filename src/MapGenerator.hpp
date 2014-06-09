#ifndef _MAPGEN_H_INCLUDED
#define _MAPGEN_H_INCLUDED

class Map;
class Actor;
class Rect;
class BSPTree;

#include "libtcod.hpp"
#include "BSPTree.hpp"
#include <stack>

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;
static const int MAX_ROOM_MONSTERS = 3;
static const int MAX_ROOM_ITEMS = 2;

static const int HEALTH_POTION_PROB = 70;
static const int LIGHTING_SPELL_PROB = 10;
static const int FIREBALL_SPELL_PROB = 10;
static const int CONFUSION_SPELL_PROB = 10;

class MapGenerator{
public:
    enum WriteMode{
        OVERWRITE, ADDTO
    }writeMode;
    MapGenerator(Map &map, long seed, WriteMode writeMode = OVERWRITE);
    virtual ~MapGenerator();
    virtual void buildMap(bool withActors);
protected:
    Map &map;
    long seed;
    TCODRandom *mapRng;
    void dig(int x, int y, int width, int height);
    void createRoom(bool first, int x, int y, int width, int height, bool withActors);
    void addDoor(int x, int y);
    void addStair(int x, int y, bool goingDown);
    void addChest(int x, int y, int itemCount);
    void addMonster(int x, int y);
    void addItem(int x, int y);
    Actor * chooseRandomItem();
};

class BSPGenerator : public MapGenerator{
friend class BSPListener;
public:
    BSPGenerator(Map &map, long seed, WriteMode writeMode = OVERWRITE);
    ~BSPGenerator();
    void buildMap(bool withActors);
protected:
    BSPTree *bspTree;
};

class CellularGenerator : public MapGenerator{
public:
    CellularGenerator(Map &map, long seed, WriteMode writeMode = OVERWRITE);
    ~CellularGenerator();
    void buildMap(bool withActors);
protected:
    bool *mapData;
    void applyRules(int survivalMin, int deathMin);
    int getNeighborCount(int x, int y);
};

#endif // _MAPGEN_H_INCLUDED
