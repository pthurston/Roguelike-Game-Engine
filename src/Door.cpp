#include "Interactable.hpp"
#include "Engine.hpp"
#include "Actor.hpp"
#include "GUI.hpp"

Door::Door(Actor *owner) : Interactable(owner){}

void Door::interact(Actor *user){
    if(user == engine.player){
        owner->ch = ' ';
        owner->blocks = false;
        engine.gui->message(TCODColor::lightGrey, "You unlocked the door");
    }
}

void Door::save(TCODZip &zip){
    zip.putInt(DOOR);
}

