#include "Destructible.hpp"
#include "Actor.hpp"
#include "Engine.hpp"
#include "GUI.hpp"

Destructible::Destructible(float maxHp, int agility, const char *corpseName, int xp) :
                            constitution(1), agility(1),
                            maxHp(maxHp), hp(maxHp), xp(xp){
    this->corpseName = strdup(corpseName);
}

Destructible::~Destructible(){
    free(corpseName);
}

float Destructible::takeDamage(Actor *owner, float damage){
    float DR = getDefenseRating();
    damage -= DR;
    if(damage > 0){
        hp -= damage;
        if(hp <= 0)
            die(owner);
    }else
        damage = 0;
    return damage;
}

float Destructible::heal(float amount){
    hp += amount;
    if(hp > maxHp){
        amount -= hp - maxHp;
        hp = maxHp;
    }
    return amount;
}

float Destructible::getDefenseRating(){
    return (float)agility;
}

void Destructible::die(Actor *owner){
    owner->ch = '%';
    owner->col = TCODColor::darkRed;
    owner->name = corpseName;
    owner->blocks = false;
    engine.sendToBack(owner);
}

void Destructible::load(TCODZip &zip){
    maxHp = zip.getFloat();
    hp = zip.getFloat();
    agility = zip.getInt();
    corpseName = strdup(zip.getString());
}

void Destructible::save(TCODZip &zip){
    zip.putFloat(maxHp);
    zip.putFloat(hp);
    zip.putInt(agility);
    zip.putString(corpseName);
}

Destructible *Destructible::create(TCODZip &zip){
    DestructibleType type=  (DestructibleType)zip.getInt();
    Destructible *destructible = NULL;
    switch(type){
        case MONSTER : destructible = new MonsterDestructible(0,0,NULL, 0); break;
        case PLAYER : destructible = new PlayerDestructible(0,0,NULL); break;
    }
    destructible->load(zip);
    return destructible;
}



MonsterDestructible::MonsterDestructible(float maxHp, float defense, const char *corpseName, int xp) :
    Destructible(maxHp,defense,corpseName, xp) {
}

void MonsterDestructible::die(Actor *owner){
    engine.gui->message(TCODColor::lightGrey, "%s is dead. You gain %d up", owner->name, xp);
    engine.player->destructible->xp += xp;
    Destructible::die(owner);
}

void MonsterDestructible::save(TCODZip &zip){
    zip.putInt(MONSTER);
    Destructible::save(zip);
}

PlayerDestructible::PlayerDestructible(float maxHp, float defense, const char *corpseName) :
    Destructible(maxHp,defense,corpseName, 0) {
}

void PlayerDestructible::die(Actor *owner){
    engine.gui->message(TCODColor::red, "You died!");
    Destructible::die(owner);
    engine.gameStatus = Engine::DEFEAT;
}

void PlayerDestructible::save(TCODZip &zip){
    zip.putInt(PLAYER);
    Destructible::save(zip);
}

