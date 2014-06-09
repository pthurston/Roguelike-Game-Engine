#ifndef _ENG_H_INCLUDED
#define _ENG_H_INCLUDED

class GUI;
class Actor;
class Map;
class Camera;

#include "libtcod.hpp"

class Engine{
public:
    enum GameStatus{
        STARTUP,
        IDLE,
        NEW_TURN,
        VICTORY,
        DEFEAT
    } gameStatus;

    int fovRadius;

    int screenWidth;
    int screenHeight;
    int level;
    GUI *gui;

    TCOD_key_t lastKey;
    TCOD_mouse_t mouse;


    Actor *player;
    //Actor *stairs;
    Map *map;
    Camera *camera;

    Engine(int screenWidth, int screenHeight);
    ~Engine();

    bool pickATile(int *x, int *y, float maxRange = 0.0f);

    void init();
    void term();
    void load();
    void save();
    void nextLevel();
    void prevLevel();

    Actor *getClosestMonster(int x, int y, float range) const;
    Actor *getActor(int x, int y) const;
    void sendToBack(Actor *actor);
    void update();
    void render();

};

extern Engine engine;

#endif // _ENG_H_INCLUDED
