#include "Actor.hpp"
#include "Attacker.hpp"
#include "Destructible.hpp"
#include "AI.hpp"
#include "Pickable.hpp"
#include "Container.hpp"
#include "Interactable.hpp"
#include "Engine.hpp"
#include "Camera.hpp"
#include "Map.hpp"

Actor::Actor(int x, int y, int ch, const char *name, const TCODColor &col) :
    x(x), y(y), ch(ch), name(name), col(col), blocks(true), fovOnly(true),
    attacker(NULL), destructible(NULL), ai(NULL), pickable(NULL), container(NULL), interactable(NULL){}

Actor::~Actor(){
    if(attacker) delete attacker;
    if(destructible) delete destructible;
    if(ai) delete ai;
    if(pickable) delete pickable;
    if(container) delete container;
    if(interactable) delete interactable;
}

void Actor::save(TCODZip &zip){
    zip.putInt(x);
    zip.putInt(y);
    zip.putInt(ch);
    zip.putColor(&col);
    zip.putString(name);
    zip.putInt(blocks);

    zip.putInt(attacker != NULL);
    zip.putInt(destructible != NULL);
    zip.putInt(ai != NULL);
    zip.putInt(pickable != NULL);
    zip.putInt(container != NULL);
    zip.putInt(interactable != NULL);

    if(attacker) attacker->save(zip);
    if(destructible) destructible->save(zip);
    if(ai) ai->save(zip);
    if(pickable) pickable->save(zip);
    if(container) container->save(zip);
    if(interactable) interactable->save(zip);
}

void Actor::load(TCODZip &zip){
    x = zip.getInt();
    y = zip.getInt();
    ch = zip.getInt();
    col = zip.getColor();
    name = strdup(zip.getString());
    blocks = zip.getInt();

    bool hasAttacker = zip.getInt();
    bool hasDestructible = zip.getInt();
    bool hasAI = zip.getInt();
    bool hasPickable = zip.getInt();
    bool hasContainer = zip.getInt();
    bool hasInteractable = zip.getInt();

    if(hasAttacker){
        attacker = new Attacker(0);
        attacker->load(zip);
    }
    if(hasDestructible){
        destructible = Destructible::create(zip);
    }

    if(hasAI){
        ai = AI::create(zip);
    }

    if(hasPickable){
        pickable = new Pickable(NULL, NULL);
        pickable->load(zip);
    }

    if(hasContainer){
        container = new Container(0, NULL);
        container->load(zip);
    }

    if(hasInteractable){
        interactable = Interactable::create(this, zip);
    }
}

void Actor::update(){
    if(ai)
        ai->update(this);
}

float Actor::getDistance(int cx, int cy) const{
    int dx = x - cx;
    int dy = y - cy;
    return sqrtf(dx*dx + dy*dy);
}

bool Actor::moveOrAttack(int x, int y){
    if(engine.map->isWall(x,y))
        return false;
    for(Actor **iterator = engine.map->actors.begin(); iterator != engine.map->actors.end(); iterator++){
        Actor *actor = *iterator;
        if(actor->x == x && actor->y == y){
            printf("The %s laughs at your puny efforts to attack him!\n", actor->name);
            return false;
        }
    }
    this->x = x;
    this->y = y;
    return true;
}

void Actor::render() const{
    Vector2 screenPos = engine.camera->worldToScreenPos(x,y);
    TCODConsole::root->setChar(screenPos.x, screenPos.y,ch);
    TCODConsole::root->setCharForeground(screenPos.x, screenPos.y,col);
}
