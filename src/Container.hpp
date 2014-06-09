#ifndef _CONT_H_INCLUDED
#define _CONT_H_INCLUDED

class Actor;

#include <vector>
#include "libtcod.hpp"
#include "Persistent.hpp"

class Container : public Persistent{
public:
    int size;
    const char *name;
    std::vector<Actor *> inventory;
    std::vector<int> itemCount;

    Container(int size, const char *name);
    ~Container();
    bool add(Actor *actor);
    void remove(Actor *actor);
    Actor *chooseFromInventory(Actor *owner);
    void load(TCODZip &zip);
    void save(TCODZip &zip);
};

#endif // _CONT_H_INCLUDED
