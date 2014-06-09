#ifndef _CAMERA_H_INCLUDED
#define _CAMERA_H_INCLUDED

class Map;
class Actor;

#include "libtcod.hpp"
#include "Persistent.hpp"
#include "Vector2.hpp"

class Camera : public Persistent{
public:
    int camWidth;
    int camHeight;
    Vector2 origin;
    Camera(int camWidth, int camHeight, Map *map, Actor *target);
    ~Camera();
    Vector2 screenToWorldPos(int x, int y);
    Vector2 screenToWorldPos(Vector2 pos);
    Vector2 worldToScreenPos(int x, int y);
    Vector2 worldToScreenPos(Vector2 pos);
    bool isOnScreen(Vector2 pos);
    void update();
    void render();
    void load(TCODZip &zip);
    void save(TCODZip &zip);
protected:
    Map *map;
    Actor *target;
};

#endif // _CAMERA_H_INCLUDED
