#ifndef _INTERACTABLE_H_INCLUDED
#define _INTERACTABLE_H_INCLUDED

class Actor;
class Map;

#include "Persistent.hpp"
#include "libtcod.hpp"

class Interactable : public Persistent{
public:
    Interactable(Actor *owner);
    virtual void interact(Actor *user) = 0;
    static Interactable * create(Actor *, TCODZip &zip);
    //Interactable* create(TCODZip &zip);
protected:
    Actor *owner;
    enum InteractableType{
        DOOR,
        STAIR
    };
};

class Door : public Interactable{
public:
    Door(Actor *owner);
    void interact(Actor *user);
    void save(TCODZip &zip);
};

class Stair : public Interactable{
public:
    Stair(Actor *owner);
    void interact(Actor *user);
    void save(TCODZip &zip);
protected:
    Map *nextLevel;
};

#endif // _INTERACTABLE_H_INCLUDED
