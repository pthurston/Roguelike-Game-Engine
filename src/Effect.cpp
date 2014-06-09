#include "Effect.hpp"
#include "Actor.hpp"
#include "Destructible.hpp"
#include "AI.hpp"
#include "Engine.hpp"
#include "Map.hpp"
#include "GUI.hpp"

Effect *Effect::create(TCODZip &zip){
    EffectType type = (EffectType)zip.getInt();
    Effect *effect = NULL;
    switch(type){
        case HEALTH : effect = new HealthEffect(0,NULL); break;
        case AICHANGE : effect = new AIChangeEffect(NULL, NULL); break;
        case TELEPORT : effect = new TeleportEffect(); break;
    }
    effect->load(zip);
    return effect;
}

HealthEffect::HealthEffect(float amount, const char *message) : amount(amount), message(message){
    targetType = ACTOR;
}

bool HealthEffect::applyTo(Actor *actor){
    if(!actor->destructible)
        return false;
    if(amount > 0){
        float pointsHealed = actor->destructible->heal(amount);
        if(pointsHealed > 0){
            if(message)
                engine.gui->message(TCODColor::lightGrey, message, actor->name, pointsHealed);
            return true;
        }
    }else{
        if(message && -amount - actor->destructible->getDefenseRating() > 0){
            engine.gui->message(TCODColor::lightGrey, message, actor->name, -amount - actor->destructible->getDefenseRating());
        }
        if(actor->destructible->takeDamage(actor, -amount) > 0){
            return true;
        }
    }

    return false;
}

void HealthEffect::save(TCODZip &zip){
    zip.putInt(HEALTH);
    zip.putFloat(amount);
    zip.putString(message);
}

void HealthEffect::load(TCODZip & zip){
    amount = zip.getFloat();
    message = zip.getString();
}

AIChangeEffect::AIChangeEffect(TemporaryAI *newAI, const char *message) : newAI(newAI), message(message){
    targetType = ACTOR;
}

bool AIChangeEffect::applyTo(Actor *actor){
    newAI->applyTo(actor);
    if(message){
        engine.gui->message(TCODColor::lightGrey, message, actor->name);
    }
    return true;
}

void AIChangeEffect::save(TCODZip &zip){
    zip.putInt(AICHANGE);
    zip.putString(message);
    zip.putInt(newAI != NULL);
    if(newAI)
        newAI->save(zip);
}

void AIChangeEffect::load(TCODZip & zip){
    message = zip.getString();
    bool hasNewAI = zip.getInt();
    if(hasNewAI)
        newAI = TemporaryAI::create(zip);
}

TeleportEffect::TeleportEffect(){
    targetType = POSITION;
}

bool TeleportEffect::applyTo(int x, int y){
    if(!engine.map->isInBounds(x,y))
        return false;
    engine.player->x = x;
    engine.player->y = y;
    engine.map->computeFOV();
}

void TeleportEffect::save(TCODZip &zip){
    zip.putInt(TELEPORT);
}
