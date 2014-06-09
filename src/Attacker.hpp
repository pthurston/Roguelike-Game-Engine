#ifndef _ATTACKER_H_INCLUDED
#define _ATTACKER_H_INCLUDED

class Actor;

#include <stdio.h>
#include "Persistent.hpp"
#include "libtcod.hpp"

class Attacker : public Persistent{
public:
    int strength;
    int agility;
    Attacker(int strength);
    float getAttackRating(Actor *owner);
    void attack(Actor *owner, Actor *target);
    void load(TCODZip &zip);
    void save(TCODZip &zip);
};

#endif // _ATTACKER_H_INCLUDED
