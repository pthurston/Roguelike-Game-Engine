#include "AI.hpp"
#include "Actor.hpp"
#include "Attacker.hpp"
#include "Engine.hpp"
#include "Map.hpp"

ConfusedMonsterAI::ConfusedMonsterAI(int nbTurns) : TemporaryAI(nbTurns){}

void ConfusedMonsterAI::update(Actor *owner){
    TCODRandom *rng = TCODRandom::getInstance();
    int dx = rng->getInt(-1, 1);
    int dy = rng->getInt(-1, 1);

    if(dx != 0 || dy != 0){
        int destX = owner->x + dx;
        int destY = owner->y + dy;
        if(engine.map->canWalk(destX, destY)){
            owner->x = destX;
            owner->y = destY;
        }else{
            Actor *actor = engine.getActor(destX, destY);
            if(actor){
                owner->attacker->attack(owner, actor);
            }
        }
    }
    TemporaryAI::update(owner);
}

void ConfusedMonsterAI::save(TCODZip &zip){
    zip.putInt(CONFUSED_MONSTER);
    TemporaryAI::save(zip);
}


