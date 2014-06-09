#ifndef _DEST_H_INCLUDED
#define _DEST_H_INCLUDED

class Actor;

#include <stdio.h>
#include "Persistent.hpp"

class Destructible : public Persistent{
public :
    int constitution;
    int agility;
    float maxHp;
    float hp;
    char *corpseName;
    int xp;

    Destructible(float maxHp, int agility, const char *corpseName, int xp);
    ~Destructible();
    inline bool isDead(){
        return hp <= 0;
    }
    float takeDamage(Actor *owner, float damage);
    float heal(float amount);
    float getDefenseRating();
    virtual void die(Actor *owner);
    void load(TCODZip &zip);
    void save(TCODZip &zip);
    static Destructible *create(TCODZip &zip);
protected:
    enum DestructibleType{
        MONSTER,
        PLAYER
    };
};

class MonsterDestructible : public Destructible{
public:
    MonsterDestructible(float maxHp, float defense, const char *corpseName, int xp);
    void die(Actor *owner);
    void save(TCODZip &zip);
};

class PlayerDestructible : public Destructible{
public:
    PlayerDestructible(float maxHp, float defense, const char *corpseName);
    void die(Actor *owner);
    void save(TCODZip &zip);
};

#endif // _DEST_H_INCLUDED
