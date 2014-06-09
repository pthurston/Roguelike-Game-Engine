#include "main.hpp"
#include "Camera.hpp"

Engine::Engine(int screenWidth, int screenHeight) : gameStatus(STARTUP),fovRadius(10), screenWidth(screenWidth), screenHeight(screenHeight), level(1), player(NULL), map(NULL){
    TCODConsole::initRoot(screenWidth,screenHeight, "libtcod C++ tutorial", false);
    gui = new GUI();

}

Engine::~Engine(){
    term();
    delete gui;
}

void Engine::term(){
    if(map)
        map->deleteMaps();
    if(camera)
        delete camera;
    gui->clear();
}

void Engine::nextLevel(){
    level++;

    gui->message(TCODColor::lightViolet, "You take a moment to rest, and recover your strength.");
    player->destructible->heal(player->destructible->maxHp / 2);
    gui->message(TCODColor::red, "After a rare moment of peace, you descend\ndeeper in to the heart of the dungeon...");

    //delete map;
    /*for(Actor **it = actors.begin(); it != actors.end(); it++){
        if(*it != player && *it != stairs){
            delete *it;
            it = actors.remove(it);
        }
    }*/
    if(map->nextLevel)
        map = map->nextLevel;
    else{
        map->nextLevel = new Map(screenWidth, screenHeight, NULL, map);
        map->actors.remove(player);
        map = map->nextLevel;
        map->init(true);
    }
    camera = new Camera(screenWidth, screenHeight - 7, map, player);
    gameStatus = STARTUP;
}

void Engine::prevLevel(){
    level--;

    //gui->message(TCODColor::lightViolet, "You take a moment to rest, and recover your strength.");
    //player->destructible->heal(player->destructible->maxHp / 2);
    gui->message(TCODColor::red, "You climb back up to an old part of the dungeon...");

    //delete map;
    /*for(Actor **it = actors.begin(); it != actors.end(); it++){
        if(*it != player && *it != stairs){
            delete *it;
            it = actors.remove(it);
        }
    }*/
    if(map->prevLevel){
        map = map->prevLevel;
        for(Actor **it = map->actors.begin(); it != map->actors.end(); it++){
            Actor *actor = *it;
            if(actor->interactable && actor->ch == '<'){
                player->x = actor->x;
                player->y = actor->y;
                break;
            }
        }
    }else{
        map->prevLevel = new Map(screenWidth, screenHeight, NULL, map);
        map = map->prevLevel;
        map->init(true);
    }
    camera = new Camera(screenWidth, screenHeight - 7, map, player);
    gameStatus = STARTUP;
}

void Engine::init(){

    player = new Actor(40, 25, '@', "Player", TCODColor::white);
    player->destructible = new PlayerDestructible(30, 2, "your cadaver");
    player->attacker = new Attacker(5);
    player->ai = new PlayerAI();
    player->container = new Container(26, "Inventory");

    map = new Map(screenWidth, screenHeight - 7, NULL, NULL);
    camera = new Camera(screenWidth, screenHeight - 7, map, player);
    map->init(true);

    /*stairs = new Actor(0,0, '>', "stairs", TCODColor::white);
    stairs->blocks = false;
    stairs->fovOnly = false;
    map->actors.push(stairs);*/

    //MapGenerator *mGen = new MapGenerator(*this, seed);

    gui->message(TCODColor::red, "Welcome stranger!\nPrepare to die");
    gameStatus = STARTUP;
}

void Engine::save(){
    //printf("saving->");
    if(player->destructible->isDead()){
        TCODSystem::deleteFile("game.sav");
    }else{
        TCODZip zip;
        zip.putInt(map->width);
        zip.putInt(map->height);
        //printf("save map->");
        map->save(zip);
        player->save(zip);
        //stairs->save(zip);

        camera->save(zip);
        gui->save(zip);
        zip.saveToFile("game.sav");
    }
    //printf("done\n");
}

void Engine::load(){
    engine.gui->menu.clear();
    engine.gui->menu.addItem(Menu::NEW_GAME, "New Game");

    if(TCODSystem::fileExists("game.sav")){
        engine.gui->menu.addItem(Menu::CONTINUE, "Continue");
    }

    engine.gui->menu.addItem(Menu::EXIT, "Exit");
    Menu::MenuItemCode menuItem = engine.gui->menu.pick();

    if(menuItem == Menu::EXIT || menuItem == Menu::NONE){
        exit(0);
    }else if(menuItem == Menu::NEW_GAME){
        engine.term();
        engine.init();
    }else{
        TCODZip zip;
        engine.term();
        printf("open file->");
        zip.loadFromFile("game.sav");
        printf("load map->");
        int width = zip.getInt();
        int height = zip.getInt();
        map = new Map(width, height, NULL, NULL);
        map->load(zip);

        player = new Actor(0,0,0,NULL, TCODColor::white);
        player->load(zip);
        //stairs = new Actor(0,0,0, NULL, TCODColor::white);
        //stairs->load(zip);
        //actors.push(stairs);
        //actors.push(player);

        camera = new Camera(0,0, map, player);
        camera->load(zip);
        printf("load actors->");

        gui->load(zip);
        gameStatus = STARTUP;
    }
}

void Engine::sendToBack(Actor *actor){
    map->actors.remove(actor);
    map->actors.insertBefore(actor, 0);
}

bool Engine::pickATile(int *x, int *y, float maxRange){
    while(!TCODConsole::isWindowClosed()){
        render();
        for(int cx = 0; cx < map->width; cx++){
            for(int cy = 0; cy < map->height; cy++){
                Vector2 screenPos = engine.camera->worldToScreenPos(cx, cy);
                if(map->isInFOV(cx,cy) && (maxRange == 0 || player->getDistance(cx, cy) <= maxRange)){
                    TCODColor col = TCODConsole::root->getCharBackground(screenPos.x, screenPos.y);
                    col = col *1.2f;
                    TCODConsole::root->setCharBackground(screenPos.x, screenPos.y, col);
                }
            }
        }
        TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE, &lastKey, &mouse);
        int mapX = mouse.cx + engine.player->x;
        int mapY = mouse.cy + engine.player->y;
        if(map->isInFOV(mapX, mapY) && (maxRange == 0 || player->getDistance(mapX, mapY) < maxRange)){
            TCODConsole::root->setCharBackground(mouse.cx, mouse.cy, TCODColor::white);
        }

        if(mouse.lbutton_pressed){
            Vector2 mapPos = engine.camera->screenToWorldPos(mouse.cx, mouse.cy);
            *x = mapPos.x;
            *y = mapPos.y;
            return true;
        }else if(mouse.rbutton_pressed || lastKey.vk != TCODK_NONE){
            return false;
        }
        TCODConsole::flush();
    }
    return false;
}

Actor *Engine::getActor(int x, int y) const{
    for(Actor **iterator = map->actors.begin(); iterator != map->actors.end(); iterator++){
        Actor *actor = *iterator;
        if(actor->x == x && actor->y == y && actor->destructible && !actor->destructible->isDead()){
            return actor;
        }
    }
    return NULL;
}

Actor *Engine::getClosestMonster(int x, int y, float range) const{
    Actor *closest = NULL;
    float bestDistance = 1E6f;

    for(Actor **iterator = map->actors.begin(); iterator != map->actors.end(); iterator++){
        Actor *actor = *iterator;
        if(actor != player && actor->destructible && !actor->destructible->isDead()){
            float distance = actor->getDistance(x,y);
            if(distance < bestDistance && (distance <= range || range == 0.0f)){
                bestDistance = distance;
                closest = actor;
            }
        }
    }
    return closest;

}

void Engine::update(){
    if(gameStatus == STARTUP)
        map->computeFOV();
    gameStatus = IDLE;

    TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE, &lastKey, &mouse);
    if(lastKey.vk == TCODK_ESCAPE){
        save();
        load();
    }
    player->update();
    camera->update();
    static double coef = 0.0;
    if(gameStatus == NEW_TURN){
        map->ambientLight = TCODColor::lerp(TCODColor::lightYellow, TCODColor::blue, coef < 1 ? coef : 2 - coef);
        coef += 0.1;
        if(coef > 2.0)
            coef -= 2.0;

        for (Actor **iterator = map->actors.begin(); iterator != map->actors.end(); iterator++) {
            Actor *actor=*iterator;
            if ( actor != player ) {
                actor->update();
            }
        }
    }
}

void Engine::render(){
    TCODConsole::root->clear();
    camera->render();
    gui->render();
    //TCODConsole::root->print(1, screenHeight-2, "HP: %d/%d", (int)player->destructible->hp, (int)player->destructible->maxHp);
}
