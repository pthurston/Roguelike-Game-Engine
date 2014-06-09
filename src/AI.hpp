#ifndef _AI_H_INCLUDED
#define _AI_H_INCLUDED

class Actor;

#include "libtcod.hpp"
#include "Persistent.hpp"

class AI : public Persistent{
public:
    virtual void update(Actor *owner) = 0;
    static AI *create(TCODZip &zip);
protected:
    enum AIType{
        MONSTER, CONFUSED_MONSTER, PLAYER
    };
};

class PlayerAI : public AI{
public:
    int xpLevel;
    PlayerAI();
    int getNextLevelXP();
    void update(Actor *owner);
    void load(TCODZip &zip);
    void save(TCODZip &zip);
protected:
    void handleActionKey(Actor *owner, int ascii);
    void displayStats(Actor *owner);
    bool moveOrAttack(Actor *owner, int targetX, int targetY);
};

class MonsterAI : public AI{
public:
    MonsterAI();
    void update(Actor *owner);
    void load(TCODZip &zip);
    void save(TCODZip &zip);
protected:
    int moveCount;
    void moveOrAttack(Actor *owner, int targetX, int targetY);
};

class TemporaryAI : public AI{
public:
    TemporaryAI(int nbTurns);
    void update(Actor *owner);
    void applyTo(Actor *actor);
    void load(TCODZip &zip);
    void save(TCODZip &zip);
    static TemporaryAI *create(TCODZip &zip);
protected:
    int nbTurns;
    AI *oldAI;
};

class ConfusedMonsterAI : public TemporaryAI{
public:
    ConfusedMonsterAI(int nbTurns);
    void update(Actor *owner);
    void save(TCODZip &zip);
};

#endif // _AI_H_INCLUDED
