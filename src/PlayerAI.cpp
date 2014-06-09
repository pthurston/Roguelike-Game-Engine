#include "AI.hpp"
#include "Actor.hpp"
#include "Destructible.hpp"
#include "Attacker.hpp"
#include "Container.hpp"
#include "Pickable.hpp"
#include "Interactable.hpp"
#include "Engine.hpp"
#include "Map.hpp"
#include "GUI.hpp"

AI *AI::create(TCODZip &zip){
    AIType type = (AIType)zip.getInt();
    AI *ai = NULL;
    switch(type){
        case PLAYER : ai = new PlayerAI(); break;
        case MONSTER : ai = new MonsterAI(); break;
        case CONFUSED_MONSTER : ai = new ConfusedMonsterAI(0); break;
        default : break;
    }
    ai->load(zip);
    return ai;
}

const int LEVEL_UP_BASE = 200;
const int LEVEL_UP_FACTOR = 150;

PlayerAI::PlayerAI() : xpLevel(1){}

int PlayerAI::getNextLevelXP(){
    return LEVEL_UP_BASE + xpLevel * LEVEL_UP_FACTOR;
}

void PlayerAI::update(Actor *owner){
    int levelUpXp = getNextLevelXP();
    if(owner->destructible->xp > levelUpXp){
        xpLevel++;
        owner->destructible->xp -= levelUpXp;
        engine.gui->message(TCODColor::yellow, "Your battle skills grow stronger! You reached level %d", xpLevel);
        engine.gui->menu.clear();
        engine.gui->menu.addItem(Menu::CONSTITUTION, "Constitution (+20HP)");
        engine.gui->menu.addItem(Menu::STRENGTH, "Strength (+1 attack)");
        engine.gui->menu.addItem(Menu::AGILITY, "Agility (+1 defense)");
        Menu::MenuItemCode menuItem = engine.gui->menu.pick(Menu::PAUSE);
        switch(menuItem){
        case Menu::CONSTITUTION:
            owner->destructible->constitution++;
            owner->destructible->maxHp += 20;
            owner->destructible->hp += 20;
            break;
        case Menu::STRENGTH:
            owner->attacker->strength++;
            break;
        case Menu::AGILITY:
            owner->destructible->agility++;
            break;
        default : break;
        }
    }
    if(owner->destructible && owner->destructible->isDead()){
        return;
    }
    int dx = 0, dy = 0;
    switch(engine.lastKey.vk){
        case TCODK_UP: dy = -1; break;
        case TCODK_DOWN: dy = 1; break;
        case TCODK_LEFT: dx = -1; break;
        case TCODK_RIGHT: dx = 1; break;
        case TCODK_CHAR : handleActionKey(owner, engine.lastKey.c); break;
        default:break;
    }
    if(dx != 0 || dy != 0){
        engine.gameStatus = Engine::NEW_TURN;
        if(moveOrAttack(owner, owner->x + dx, owner->y + dy)){
            engine.map->computeFOV();
        }
    }
}

void PlayerAI::handleActionKey(Actor *owner, int ascii){
    switch(ascii){
    case 'g' :
        {
        bool found = false;
        for(Actor **iterator = engine.map->actors.begin(); iterator != engine.map->actors.end(); iterator++){
            Actor *actor = *iterator;
            if(actor->pickable && actor->x == owner->x && actor->y == owner->y){
                if(actor->pickable->pick(actor, owner)){
                    found = true;
                    engine.gui->message(TCODColor::lightGrey, "You pick the %s.", actor->name);
                    break;
                }else if(!found){
                    found = true;
                    engine.gui->message(TCODColor::red, "Your inventory is full.");
                }
            }
        }
        if(!found){
            engine.gui->message(TCODColor::lightGrey, "There's nothing here that you can pick.");
        }
        engine.gameStatus = Engine::NEW_TURN;
        }
        break;
    case 'i' :
        {
        Actor *actor = owner->container->chooseFromInventory(owner);
        if(actor){
            actor->pickable->use(actor, owner);
            engine.gameStatus = Engine::NEW_TURN;
        }
        }
        break;
    case 'o':{
        for(Actor **it = engine.map->actors.begin(); it != engine.map->actors.end(); it++){
            Actor *actor = *it;
            if(actor->container && actor->x == owner->x && actor->y == owner->y && actor != engine.player){
                Actor *pickedItem;
                while(true){
                    pickedItem = actor->container->chooseFromInventory(actor);
                    if(pickedItem){
                        actor->container->remove(pickedItem);
                        owner->container->add(pickedItem);
                    }else
                        break;
                }
            }
        }
    }break;
    case 'd':
        {
        Actor *actor = owner->container->chooseFromInventory(owner);
        if(actor){
            actor->pickable->drop(actor, owner);
            engine.gameStatus = Engine::NEW_TURN;
        }
        }
        break;
    case '>':
    case '<':
        {
        for(Actor **iterator = engine.map->actors.begin(); iterator != engine.map->actors.end(); iterator++){
            Actor *actor = *iterator;
            if(actor->interactable && actor->x == owner->x && actor->y == owner->y && (actor->ch == '>' || actor->ch == '<')){
                actor->interactable->interact(owner);
                break;
            }
        }
        }
        break;
    }

}

bool PlayerAI::moveOrAttack(Actor *owner, int targetX, int targetY){
    if(engine.map->isWall(targetX, targetY))
        return false;
    for(Actor **iterator = engine.map->actors.begin(); iterator != engine.map->actors.end(); iterator++){
        Actor *actor = *iterator;
        if(actor->destructible && !actor->destructible->isDead() && actor->x == targetX & actor->y == targetY){
            owner->attacker->attack(owner, actor);
            return false;
        }else if(actor->interactable && actor->blocks && actor->x == targetX && actor->y == targetY){
            actor->interactable->interact(owner);
        }
    }

    for (Actor **iterator=engine.map->actors.begin(); iterator != engine.map->actors.end(); iterator++) {
        Actor *actor=*iterator;
        bool corpseOrItem = (actor->destructible && actor->destructible->isDead()) || actor->pickable;
        if (corpseOrItem && actor->x == targetX && actor->y == targetY ) {
            engine.gui->message(TCODColor::lightGrey, "There's a %s here",actor->name);
        }
    }
    owner->x = targetX;
    owner->y = targetY;
    return true;
}

void PlayerAI::load(TCODZip &zip){

}

void PlayerAI::save(TCODZip &zip){
    zip.putInt(PLAYER);
}
