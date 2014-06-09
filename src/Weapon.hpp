#ifndef _WEAPON_H_INCLUDED
#define _WEAPON_H_INCLUDED

#include "libtcod.hpp"

class Weapon{
public:
    Weapon(int dieCount, int dieSize);
    float getRandomRoll();
    void save(TCODZip &zip);
    void load(TCODZip &zip);
protected:
    dieCount;
    dieSize;
};

#endif // _WEAPON_H_INCLUDED
