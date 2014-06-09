#include "Interactable.hpp"
#include "Engine.hpp"
#include "Actor.hpp"

Stair::Stair(Actor *owner) : Interactable(owner){}

void Stair::save(TCODZip &zip){
    zip.putInt(STAIR);
}

void Stair::interact(Actor *user){
    if(owner->ch == '<'){
        engine.nextLevel();
    }else if(owner->ch == '>'){
        engine.prevLevel();
    }
}
