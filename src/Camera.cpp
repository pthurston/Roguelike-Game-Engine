#include "Camera.hpp"
#include "Actor.hpp"
#include "Engine.hpp"
#include "Map.hpp"

Camera::Camera(int camWidth, int camHeight, Map *map, Actor *target) : camWidth(camWidth), camHeight(camHeight), map(map), target(target){
    origin = Vector2(0,0);
}

Camera::~Camera(){
    /*if(map)
        delete map;
    if(target)
        delete target;*/
}

Vector2 Camera::screenToWorldPos(int x, int y){
    Vector2 pos(x,y);
    return pos + origin;
}

Vector2 Camera::screenToWorldPos(Vector2 pos){
    return pos + origin;
}

Vector2 Camera::worldToScreenPos(int x, int y){
    Vector2 pos(x,y);
    return pos - origin;
}

Vector2 Camera::worldToScreenPos(Vector2 pos){
    return pos - origin;
}

bool Camera::isOnScreen(Vector2 pos){
    Vector2 screenPos = worldToScreenPos(pos);
    if(screenPos.x < 0 || screenPos.x >= camWidth || screenPos.y < 0 || screenPos.y >= camHeight)
        return false;
    return true;
}

void Camera::update(){
    if(engine.player->x - origin.x < 10)
        origin.x = engine.player->x - 10;
    else if(origin.x + camWidth - engine.player->x < 10)
        origin.x = engine.player->x + 10 - camWidth;

    if(engine.player->y - origin.y < 10)
        origin.y = engine.player->y - 10;
    else if(origin.y + camHeight - engine.player->y < 10)
        origin.y = engine.player->y + 10 - camHeight;

}

void Camera::render(){
    engine.map->render();
    for(Actor **iterator= engine.map->actors.begin(); iterator != engine.map->actors.end(); iterator++){
        Actor *actor = *iterator;
        if(actor != engine.player && ((!actor->fovOnly && map->isExplored(actor->x, actor->y)) || map->isInFOV(actor->x, actor->y))){
            actor->render();
        }
    }
    engine.player->render();
}

void Camera::save(TCODZip &zip){
    zip.putInt(camWidth);
    zip.putInt(camHeight);
    zip.putInt(origin.x);
    zip.putInt(origin.y);
}

void Camera::load(TCODZip &zip){
    camWidth = zip.getInt();
    camHeight = zip.getInt();
    int x = zip.getInt();
    int y = zip.getInt();
    origin = Vector2(x,y);
}
