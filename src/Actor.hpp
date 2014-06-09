#ifndef _ACTOR_H_INCLUDED
#define _ACTOR_H_INCLUDED

class Attacker;
class Destructible;
class AI;
class Pickable;
class Container;
class Interactable;
class Component;

#include <stdio.h>
#include <math.h>
#include "libtcod.hpp"
#include "Persistent.hpp"

class Actor : Persistent{

    public:
        int x,y;    //Position
        int ch;     //Ascii code
        const char *name;
        TCODColor col;//Color

        bool blocks;
        bool fovOnly;
        Attacker *attacker;
        Destructible *destructible;
        AI *ai;
        Pickable *pickable;
        Container *container;
        Interactable *interactable;

        Actor(int x, int y, int ch, const char *name, const TCODColor &col);
        ~Actor();
        void load(TCODZip &zip);
        void save(TCODZip &zip);
        void update();
        float getDistance(int cx, int cy) const;
        bool moveOrAttack(int x, int y);
        void render() const;
};

#endif // _ACTOR_H_INCLUDED
