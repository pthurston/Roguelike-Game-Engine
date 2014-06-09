#include "TargetSelector.hpp"
#include "Destructible.hpp"
#include "Actor.hpp"
#include "Engine.hpp"
#include "Map.hpp"
#include "GUI.hpp"

TargetSelector::TargetSelector(SelectorType type, float range) : type(type), range(range){}

void TargetSelector::selectTargets(Actor *wearer, TCODList<Actor *> &list){
    switch(type){
        case CLOSEST_MONSTER:
        {
            Actor *closestMonster = engine.getClosestMonster(wearer->x, wearer->y, range);
            if(closestMonster){
                list.push(closestMonster);
            }
        }break;
        case SELECTED_MONSTER:
        {
            int x,y;
            engine.gui->message(TCODColor::cyan, "Left-click to select an enemy,\nor right-click to cancel.");
            if(engine.pickATile(&x, &y, range)){
                Actor *actor = engine.getActor(x,y);
                if(actor)
                    list.push(actor);
            }
        }break;
        case WEARER_RANGE:
        for(Actor **iterator = engine.map->actors.begin(); iterator != engine.map->actors.end(); iterator++){
            Actor *actor = *iterator;
            if(actor != wearer && actor->destructible && !actor->destructible->isDead()
            && actor->getDistance(wearer->x, wearer->y) <= range){
                list.push(actor);
            }
        }
        break;
        case SELECTED_RANGE:
            int x,y;
            engine.gui->message(TCODColor::cyan, "Left-click to select a tile,\nor right-click to cancel.");
            if(engine.pickATile(&x, &y)){
                for(Actor **iterator = engine.map->actors.begin(); iterator != engine.map->actors.end(); iterator++){
                    Actor *actor = *iterator;
                    if(actor->destructible && !actor->destructible->isDead() && actor->getDistance(x,y) <= range){
                        list.push(actor);
                    }
                }
            }
        break;
    }
    if(list.isEmpty()){
        engine.gui->message(TCODColor::lightGrey, "No enemy is close enough");
    }
}

bool TargetSelector::selectPosition(int &x, int &y){
    engine.gui->message(TCODColor::cyan, "Left-click to select an empty tile,\nor right-click to cancel.");
    if(engine.pickATile(&x, &y)){
        if(engine.player->x == x && engine.player->y == y)
            return false;

        for(Actor **it = engine.map->actors.begin(); it != engine.map->actors.end(); it++){
            Actor *actor = *it;
            if(actor->x == x && actor->y == y)
                return false;
        }
        return true;
    }
    return false;
}

void TargetSelector::save(TCODZip &zip){
    zip.putInt(type);
    zip.putFloat(range);
}

void TargetSelector::load(TCODZip &zip){
    type = (SelectorType)zip.getInt();
    range = zip.getFloat();
}
