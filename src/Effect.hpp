#ifndef _EFFECT_H_INCLUDED
#define _EFFECT_H_INCLUDED

class Actor;
class TemporaryAI;

#include "libtcod.hpp"
#include "Persistent.hpp"

class Effect : public Persistent{
public:
    enum TargetType{
        ACTOR,
        POSITION
    }targetType;
    virtual bool applyTo(Actor *actor){}
    virtual bool applyTo(int x, int y){}
    static Effect *create(TCODZip &zip);
protected:
    enum EffectType{
        HEALTH,
        AICHANGE,
        TELEPORT
    };
};

class HealthEffect : public Effect{
public:
    float amount;
    const char *message;

    HealthEffect(float amount, const char *message);
    bool applyTo(Actor *actor);
    void load(TCODZip &zip);
    void save(TCODZip &zip);
};

class AIChangeEffect : public Effect{
public:
    TemporaryAI *newAI;
    const char *message;

    AIChangeEffect(TemporaryAI *newAI, const char *message);
    bool applyTo(Actor *actor);
    void load(TCODZip &zip);
    void save(TCODZip &zip);
};

class TeleportEffect : public Effect{
public:
    TeleportEffect();
    bool applyTo(int x, int y);
    void load(TCODZip &zip){}
    void save(TCODZip &zip);
};

#endif // _EFFECT_H_INCLUDED
