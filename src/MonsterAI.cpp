#include <math.h>
#include "AI.hpp"
#include "Actor.hpp"
#include "Destructible.hpp"
#include "Attacker.hpp"
#include "Engine.hpp"
#include "Map.hpp"

static const int TRACKING_TURNS = 3;

MonsterAI::MonsterAI(){}

void MonsterAI::update(Actor *owner){
    if(owner->destructible && owner->destructible->isDead()){
        return;
    }

    if(engine.map->isInFOV(owner->x, owner->y)){
        moveCount = TRACKING_TURNS;
    }else{
        moveCount--;
    }

    if(moveCount > 0)
        moveOrAttack(owner, engine.player->x, engine.player->y);
}

void MonsterAI::moveOrAttack(Actor *owner, int targetX, int targetY){
    int dx = targetX - owner->x;
    int dy = targetY - owner->y;
    int stepdx = (dx > 0 ? 1 : -1);
    int stepdy = (dy > 0 ? 1 : -1);
    float distance = sqrtf(dx*dx + dy*dy);
    if(distance >= 2){
        dx = (int)(round(dx/distance));
        dy = (int)(round(dy/distance));
        if(engine.map->canWalk(owner->x + dx, owner->y + dy)){
            owner->x += dx;
            owner->y += dy;
        }else if(engine.map->canWalk(owner->x + stepdx, owner->y)){
            owner->x += stepdx;
        }else if(engine.map->canWalk(owner->x, owner->y + stepdy)){
            owner->y += stepdy;
        }
    }else if(owner->attacker){
        owner->attacker->attack(owner, engine.player);
    }
}

void MonsterAI::load(TCODZip &zip){
    moveCount = zip.getInt();
}

void MonsterAI::save(TCODZip &zip){
    zip.putInt(MONSTER);
    zip.putInt(moveCount);
}
