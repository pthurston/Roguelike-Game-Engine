#include "AI.hpp"
#include "Actor.hpp"

TemporaryAI::TemporaryAI(int nbTurns) : nbTurns(nbTurns), oldAI(NULL){}

void TemporaryAI::update(Actor *owner){
    nbTurns--;
    if(nbTurns == 0){
        owner->ai = oldAI;
        delete this;
    }
}

void TemporaryAI::applyTo(Actor *actor){
    oldAI = actor->ai;
    actor->ai = this;
}

void TemporaryAI::save(TCODZip &zip){
    zip.putInt(nbTurns);
    zip.putInt(oldAI != NULL);
    if(oldAI)
        oldAI->save(zip);
}

void TemporaryAI::load(TCODZip &zip){
    nbTurns = zip.getInt();
    bool hasOldAI = zip.getInt();
    if(hasOldAI)
        oldAI = AI::create(zip);
}


TemporaryAI *TemporaryAI::create(TCODZip &zip){
    AIType type = (AIType)zip.getInt();
    TemporaryAI *ai = NULL;
    switch(type){
        case CONFUSED_MONSTER : ai = new ConfusedMonsterAI(0); break;
        default : break;
    }
    ai->load(zip);
    return ai;
}
