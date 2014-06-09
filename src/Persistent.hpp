#ifndef _PERS_H_INCLUDED
#define _PERS_H_INCLUDED

#include "libtcod.hpp"

class Persistent{
public:
    virtual void load(TCODZip &zip){};
    virtual void save(TCODZip &zip){};
};

#endif // _PERS_H_INCLUDED
