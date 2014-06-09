#include "Pickable.hpp"
#include "TargetSelector.hpp"
#include "Effect.hpp"
#include "Container.hpp"
#include "Actor.hpp"
#include "Engine.hpp"
#include "Map.hpp"
#include "GUI.hpp"

Pickable::Pickable(TargetSelector *selector, Effect *effect) : selector(selector), effect(effect){}

Pickable::~Pickable(){
    if(selector) delete selector;
    if(effect) delete effect;
}

bool Pickable::pick(Actor *owner, Actor *wearer){
    if(wearer->container && wearer->container->add(owner)){
        engine.map->actors.remove(owner);
        return true;
    }
    return false;
}

bool Pickable::use(Actor *owner, Actor *wearer){
    bool succeed = false;
    if(!effect){
        engine.gui->message(TCODColor::lightGrey, "This item does nothing");
        return false;
    }

    if(effect->targetType == Effect::ACTOR){
        TCODList<Actor *> list;
        if(selector){
            selector->selectTargets(wearer, list);
        }else{
            list.push(wearer);
        }
        for(Actor **it = list.begin(); it != list.end(); it++){
            if(effect->applyTo(*it)){
                succeed = true;
            }
        }
    }else if(effect->targetType == Effect::POSITION){
        int x,y;
        printf("%d, %d", x, y);
        if(selector){
            if(selector->selectPosition(x,y)){
                if(effect->applyTo(x,y))
                    succeed = true;
            }
        }
    }
    if(succeed){
        if(wearer->container){
            wearer->container->remove(owner);
            delete owner;
        }
    }
    return succeed;
}

void Pickable::drop(Actor *owner, Actor *wearer){
    if(wearer->container){
        wearer->container->remove(owner);
        engine.map->actors.push(owner);
        owner->x = wearer->x;
        owner->y = wearer->y;
        engine.gui->message(TCODColor::lightGrey, "%s drops a %s.", wearer->name, owner->name);
    }
}

void Pickable::save(TCODZip &zip){
    zip.putInt(selector != NULL);
    zip.putInt(effect != NULL);
    if(selector)
        selector->save(zip);
    if(effect)
        effect->save(zip);
}

void Pickable::load(TCODZip &zip){
    bool hasTargetSelector = zip.getInt();
    bool hasEffect = zip.getInt();
    if(hasTargetSelector){
        selector = new TargetSelector(TargetSelector::CLOSEST_MONSTER,0);
        selector->load(zip);
    }
    if(hasEffect){
        effect = Effect::create(zip);
    }
}

