#include "MapGenerator.hpp"
#include "Actor.hpp"
#include "Destructible.hpp"
#include "Attacker.hpp"
#include "AI.hpp"
#include "Pickable.hpp"
#include "Container.hpp"
#include "Effect.hpp"
#include "Interactable.hpp"
#include "TargetSelector.hpp"
#include "Map.hpp"
#include "Engine.hpp"

MapGenerator::MapGenerator(Map &map, long seed, WriteMode writeMode) : map(map), seed(seed), writeMode(writeMode){
    mapRng = new TCODRandom(seed, TCOD_RNG_CMWC);
}

MapGenerator::~MapGenerator(){
    delete mapRng;
}

void MapGenerator::buildMap(bool withActors){

    int roomWidth = mapRng->getInt(ROOM_MIN_SIZE, ROOM_MAX_SIZE);
    int roomHeight = mapRng->getInt(ROOM_MIN_SIZE, ROOM_MAX_SIZE);
    int x = mapRng->getInt(1, map.width - 2 - roomWidth);
    int y = mapRng->getInt(1, map.height - 2 - roomHeight);
    createRoom(true, x, y, roomWidth, roomHeight, withActors);
}

void MapGenerator::dig(int x, int y, int width, int height){
    //printf("digging -> ");
    for(int tileX = x; tileX < x + width; tileX++){
        for(int tileY = y; tileY < y + height; tileY++){
            if(writeMode == OVERWRITE)
                map.tiles[tileX + tileY * map.width].walkable = true;
            else
                map.tiles[tileX + tileY * map.width].walkable |= true;
        }
    }
}

void MapGenerator::createRoom(bool first, int x, int y, int width, int height, bool withActors){
    dig(x, y, width, height);

    if(!withActors)
        return;

    if(first){
        engine.player->x = x + width / 2;
        engine.player->y = y + height / 2;
        addStair(x + width/2, y + height / 2, true);
        if(map.prevLevel)
            addStair(x + width/2 + 2, y + height / 2, false);
    }else{
        TCODRandom *countRng = TCODRandom::getInstance();
        int nbItems = countRng->getInt(1, MAX_ROOM_ITEMS);
        int chestX;
        int chestY;
        do{
            chestX = countRng->getInt(x, x + width -1);
            chestY = countRng->getInt(y, y+ height - 1);
        }while(!map.canWalk(chestX, chestY));
        printf("%d %d\n", chestX, chestY);
        addChest(chestX, chestY, nbItems);
        /*while(nbItems > 0){
            int aX = countRng->getInt(x, x + width - 1);
            int aY = countRng->getInt(y, y + height - 1);
            if(map.canWalk(aX,aY)){
                addItem(aX,aY);
            }
            nbItems--;
        }*/
        int nbMonsters = countRng->getInt(0, MAX_ROOM_MONSTERS);
        while(nbMonsters > 0){
            int aX = countRng->getInt(x, x + width - 1);
            int aY = countRng->getInt(y, y + height - 1);
            if(map.canWalk(aX,aY)){
                addMonster(aX,aY);
            }
            nbMonsters--;
        }
    }
}

void MapGenerator::addMonster(int x, int y){
    TCODRandom *tmpRng = TCODRandom::getInstance();
    if(tmpRng->getInt(0, 100) < 80){
        Actor *orc = new Actor(x, y, 'o', "orc", TCODColor::desaturatedGreen);
        orc->destructible = new MonsterDestructible(10, 0, "dead orc", 50);
        orc->attacker = new Attacker(3);
        orc->ai = new MonsterAI();
        map.actors.push(orc);
    }else{
        Actor *troll = new Actor(x, y, 'T', "troll", TCODColor::darkerGreen);
        troll->destructible = new MonsterDestructible(16, 1, "troll carcass", 150);
        troll->attacker = new Attacker(4);
        troll->ai = new MonsterAI();
        map.actors.push(troll);
    }
}

void MapGenerator::addItem(int x, int y){
    Actor *newItem = chooseRandomItem();
    map.actors.push(newItem);
    newItem->x = x;
    newItem->y = y;
}

Actor * MapGenerator::chooseRandomItem(){
    TCODRandom *tmpRng = TCODRandom::getInstance();
    int dice = tmpRng->getInt(0, 100);
    Actor *newItem;
    if(dice < HEALTH_POTION_PROB){
        newItem = new Actor(0, 0, '*', "teleport scroll", TCODColor::violet);
        newItem->pickable = new Pickable(new TargetSelector(TargetSelector::SELECTED_RANGE, 5),
                                                new TeleportEffect());
        //Actor *healthPotion = new Actor(x, y, '!', "health potion", TCODColor::violet);
        //healthPotion->pickable = new Pickable(NULL, new HealthEffect(4, NULL));
    }else if(dice < HEALTH_POTION_PROB + LIGHTING_SPELL_PROB){
        newItem = new Actor(0, 0, '#', "scroll of lightning", TCODColor::lightYellow);
        newItem->pickable = new Pickable(new TargetSelector(TargetSelector::CLOSEST_MONSTER, 5),
                                                       new HealthEffect(-20, "A lightning bolt strikes the %s with a loud thunder!\nThe damage is %g hit points."));
    }else if(dice < HEALTH_POTION_PROB + LIGHTING_SPELL_PROB + FIREBALL_SPELL_PROB){
        newItem = new Actor(0, 0, '#', "scroll of fireball", TCODColor::orange);
        newItem->pickable = new Pickable(new TargetSelector(TargetSelector::SELECTED_RANGE, 3),
                                                  new HealthEffect(-12, "The %s gets burned for %g hit points."));
    }else{
        newItem = new Actor(0, 0, '#', "scroll of confusion", TCODColor::lightGrey);
        newItem->pickable = new Pickable(new TargetSelector(TargetSelector::SELECTED_MONSTER, 5),
                                                   new AIChangeEffect(new ConfusedMonsterAI(10), "The eyes of the %s look vacant,\nas he starts to stumble around!"));
    }
    newItem->blocks = false;
    return newItem;

}

void MapGenerator::addChest(int x, int y, int itemCount){
    Actor *chest = new Actor(x,y, '=', "Chest", TCODColor::copper);
    chest->container = new Container(26, "Chest");
    chest->blocks = false;
    for(int i = 0; i < itemCount; i++){
        Actor *newItem = chooseRandomItem();
        chest->container->add(newItem);
    }
    map.actors.push(chest);
}

void MapGenerator::addDoor(int x, int y){
    Actor *door = new Actor(x,y,'+', "door", TCODColor::green);
    //Door *doorInt = new Door();
    door->interactable = new Door(door);
    door->blocks = true;

    map.actors.push(door);
}

void MapGenerator::addStair(int x, int y, bool goingDown){
    char ch = goingDown ? '<' : '>';
    Actor *stairs = new Actor(x,y, ch, "stairs", TCODColor::grey);
    stairs->blocks = false;
    stairs->interactable = new Stair(stairs);
    map.actors.push(stairs);
}


