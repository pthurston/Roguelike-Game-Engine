#ifndef _PICK_H_INCLUDED
#define _PICK_H_INCLUDED

class Actor;
class TargetSelector;
class Effect;

#include "libtcod.hpp"
#include "Persistent.hpp"

class Pickable : public Persistent{
public:
    Pickable(TargetSelector *selector, Effect *effect);
    ~Pickable();
    bool pick(Actor *owner, Actor *wearer);
    void drop(Actor *owner, Actor *wearer);
    virtual bool use(Actor *owner, Actor *wearer);
    void save(TCODZip &zip);
    void load(TCODZip &zip);
protected:
    TargetSelector *selector;
    Effect *effect;
};

#endif // _PICK_H_INCLUDED

