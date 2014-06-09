#include "Attacker.hpp"
#include "Destructible.hpp"
#include "Actor.hpp"
#include "Engine.hpp"
#include "GUI.hpp"

Attacker::Attacker(int strength) : strength(strength){}

float Attacker::getAttackRating(Actor *owner){
    return (float)strength;
}

void Attacker::attack(Actor *owner, Actor *target){
    if(target->destructible && !target->destructible->isDead()){
        float AR = getAttackRating(owner);
        float DR = target->destructible->getDefenseRating();
        if(AR - DR > 0){
            engine.gui->message(TCODColor::white, "%s attacks %s for %g hit points.", owner->name, target->name, AR - DR);
        }else{
            engine.gui->message(TCODColor::white, "%s attacks, %s but it has no effect!", owner->name, target->name);
        }
        target->destructible->takeDamage(target, AR);
    }else
        engine.gui->message(TCODColor::white, "%s attacks %s in vain.", owner->name, target->name);
}

void Attacker::load(TCODZip &zip){
    strength =zip.getInt();
}

void Attacker::save(TCODZip &zip){
    zip.putInt(strength);
}
