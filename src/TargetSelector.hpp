#ifndef _TS_H_INCLUDED
#define _TS_H_INCLUDED

class Actor;

#include "libtcod.hpp"
#include "Persistent.hpp"

class TargetSelector : public Persistent{
public :
    enum SelectorType{
        CLOSEST_MONSTER,
        SELECTED_MONSTER,
        WEARER_RANGE,
        SELECTED_RANGE
    };
    TargetSelector(SelectorType type, float range);
    void selectTargets(Actor *wearer, TCODList<Actor *> &list);
    bool selectPosition(int &x, int &y);
    void save(TCODZip &zip);
    void load(TCODZip &zip);
protected:
    SelectorType type;
    float range;
};

#endif // _TS_H_INCLUDED
