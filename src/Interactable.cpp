#include "Interactable.hpp"

Interactable::Interactable(Actor *owner) : owner(owner){}

Interactable * Interactable::create(Actor *owner, TCODZip &zip){
    InteractableType type = (InteractableType)zip.getInt();
    Interactable *inter = NULL;
    switch(type){
        case DOOR : inter = new Door(owner); break;
        case STAIR : inter = new Stair(owner); break;
        default : break;
    }
    inter->load(zip);
    return inter;
}
