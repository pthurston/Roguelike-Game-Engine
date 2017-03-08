#include "main.hpp"
#include "Camera.hpp"

/*
    Engine constructor
*/
Engine::Engine(int screenWidth, int screenHeight) : gameStatus(STARTUP),fovRadius(10), screenWidth(screenWidth), screenHeight(screenHeight), level(1), player(NULL), map(NULL){
    TCODConsole::initRoot(screenWidth,screenHeight, "libtcod C++ tutorial", false);
    gui = new GUI();

}

/*
    Engine destructor
*/
Engine::~Engine(){
    term();
    delete gui;
}

/*
    Terminate function
        This function is called before the actual destructor of the engine in 
        order to perform some minor cleanup before the actual pointers are destroyed
*/
void Engine::term(){
    if(map) //if a map exists
        map->deleteMaps(); //delete it
    if(camera)
        delete camera;
    gui->clear(); //clear all data stored by the gui
}

/*
    Function to go to the next level, takes no arguments
*/
void Engine::nextLevel(){
    level++; //increase our main level counter

    gui->message(TCODColor::lightViolet, "You take a moment to rest, and recover your strength."); //flavor text
    player->destructible->heal(player->destructible->maxHp / 2); //increase our players health
    gui->message(TCODColor::red, "After a rare moment of peace, you descend\ndeeper in to the heart of the dungeon...");

    if(map->nextLevel) //if a next level for our map exists
        map = map->nextLevel; //change to it
    else{ //otherwise...
        map->nextLevel = new Map(screenWidth, screenHeight, NULL, map); //we make it
        map->actors.remove(player); //remove the player from our current map
        map = map->nextLevel; //change the level to the newly created next one
        map->init(true); //intialize our new map
    }
    camera = new Camera(screenWidth, screenHeight - 7, map, player); //create a new camera to view our new map
                                                // -7 to save room for GUI
    gameStatus = STARTUP; //let the engine know we're in the startup state
}

/*
    Function to go to the previous level, takes no arguments
*/
void Engine::prevLevel(){
    level--;
    gui->message(TCODColor::red, "You climb back up to an old part of the dungeon...");

    if(map->prevLevel){ //if a previous level exists
        map = map->prevLevel; //change our map to it
        for(Actor **it = map->actors.begin(); it != map->actors.end(); it++){ //for each actor in our map
            Actor *actor = *it; //set actor to it
            if(actor->interactable && actor->ch == '<'){ // '<' symbolizes an entrance or stairs
                        //if the actor can be interacted with AND it's an entrance
                player->x = actor->x; //effectively set our players coordinates here
                player->y = actor->y; 
                break; //break out of the for loop since we've found what we're looking for
            }
        }
    }else{ //else if one doesn't exist
        map->prevLevel = new Map(screenWidth, screenHeight, NULL, map); //make one
        map = map->prevLevel; //go to it
        map->init(true); //intialize it
    }
    camera = new Camera(screenWidth, screenHeight - 7, map, player); //make a camera again
    gameStatus = STARTUP; //change game status
}

/*
    Function to initialize the engine. This includes:
        Creating the player and setting up all of his relevant attributes
        Making an initial map and camera
        Setting the status to STARTUP
*/
void Engine::init(){

    player = new Actor(40, 25, '@', "Player", TCODColor::white); //create a player
                    //args: player's x and y coords, his onscreen symbol, and the color of the symbol
    player->destructible = new PlayerDestructible(30, 2, "your cadaver"); //make him destructable
                                //args: maximum health points, defense stat, name of corpse left behind
    player->attacker = new Attacker(5); //make him able to attack
                                //args: player's strength attribute
    player->ai = new PlayerAI(); //give him some basic AI:
                                 // this takes care of all leveling up functions and handles movement
                                 // in the case of debuffs such as confusion it also does that
    player->container = new Container(26, "Inventory"); //give him a container to hold stuff and call it an 'inventory'
                                //   holds 26 slots

    map = new Map(screenWidth, screenHeight - 7, NULL, NULL); //create a new map
    camera = new Camera(screenWidth, screenHeight - 7, map, player); //create our camera
    map->init(true); //initialize our map

    gui->message(TCODColor::red, "Welcome stranger!\nPrepare to die"); //flavor text
    gameStatus = STARTUP; //change status
}

/*
    Function to take care of saving the:
        map, player (and all related attributes)
*/ 
void Engine::save(){
    if(player->destructible->isDead()){
        TCODSystem::deleteFile("game.sav"); //delete the save if a player dies
    }else{ //if we're saving and the player isn't dead'
        TCODZip zip; //make a Zip object
        zip.putInt(map->width); //store the map size in it
        zip.putInt(map->height);
        map->save(zip); //save the map into the zip
        player->save(zip); //save the player into the zip

        camera->save(zip); //save the camera...
        gui->save(zip); //save the gui...
        zip.saveToFile("game.sav"); //define the name of the save file
    }
}

/*
    Initial function called:
        First prompts the user for a choice in playing a new game
             AND if there exists a save file in the directory we can also prompt to continue that save file
             AND let them exit
*/
void Engine::load(){ 
    engine.gui->menu.clear(); //clear the menu
    engine.gui->menu.addItem(Menu::NEW_GAME, "New Game"); //add the new game prompt

    if(TCODSystem::fileExists("game.sav")){ //if there exists 
        engine.gui->menu.addItem(Menu::CONTINUE, "Continue"); //add the continue prompt
    }

    engine.gui->menu.addItem(Menu::EXIT, "Exit"); //add the exit prompt
    Menu::MenuItemCode menuItem = engine.gui->menu.pick(); //what menu item did the user pick

    if(menuItem == Menu::EXIT || menuItem == Menu::NONE){ //if user picks EXIT or nothing
        exit(0); //we exit
    }else if(menuItem == Menu::NEW_GAME){ //if they pick new game
        engine.term(); //terminate the engine
        engine.init(); //initialize the engine!
    }else{ //otherwise they picked load game
        TCODZip zip; //make a zip object
        engine.term(); //terminate the engine
        printf("open file->");
        zip.loadFromFile("game.sav"); //load data from file
        printf("load map->");
        int width = zip.getInt(); //use our files dimensions
        int height = zip.getInt();
        map = new Map(width, height, NULL, NULL); //make a new map based on those dimensions
        map->load(zip); //load the map

        player = new Actor(0,0,0,NULL, TCODColor::white); //create a new blank character
        player->load(zip); //load his attributes from the file

        camera = new Camera(0,0, map, player); //make a new default camera
        camera->load(zip); //load his attributes from the file
        printf("load actors->");

        gui->load(zip); //load the saved gui elements
        gameStatus = STARTUP;
    }
}

/*
    Function to send an actor to the back - AKA, we make him rendered first so other actors get rendered on top
        Takes a single argument: a pointer to the actor we wish to send back
*/
void Engine::sendToBack(Actor *actor){
    map->actors.remove(actor); //remove the actor entirely
    map->actors.insertBefore(actor, 0); //insert at the beginning
}

/*
    Function to pick a tile using left mouse button, used for abilities which require a target selection
    Takes 3 arguments: A pointer to an x variable 
                       A pointer to a y variable
                       A float for the maxium reach of an ability

*/
bool Engine::pickATile(int *x, int *y, float maxRange){
    while(!TCODConsole::isWindowClosed()){ //while the main window isn't closed
        render(); //render the scene
        for(int cx = 0; cx < map->width; cx++){ //iterate through whole map
            for(int cy = 0; cy < map->height; cy++){
                Vector2 screenPos = engine.camera->worldToScreenPos(cx, cy); //get screen position of world coordinates
                                                                             // aka - where on screen is this point?
                if(map->isInFOV(cx,cy) && (maxRange == 0 || player->getDistance(cx, cy) <= maxRange)){ //if our point is in the player's FOV
                    TCODColor col = TCODConsole::root->getCharBackground(screenPos.x, screenPos.y); //get the current color
                    col = col *1.2f; //multiply it by 1.2
                    TCODConsole::root->setCharBackground(screenPos.x, screenPos.y, col); //colour the point
                }
            }
        }
        TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE, &lastKey, &mouse); //check if we have a mouse click
        int mapX = mouse.cx + engine.player->x; //change our mapX and mapY by adding our player's current locations
        int mapY = mouse.cy + engine.player->y;
        if(map->isInFOV(mapX, mapY) && (maxRange == 0 || player->getDistance(mapX, mapY) < maxRange)){ //if this mouse click location is in our FOV
                                                                                                       // AND it's within the ability's range
            TCODConsole::root->setCharBackground(mouse.cx, mouse.cy, TCODColor::white); //change the colour to of the background to white
        }

        if(mouse.lbutton_pressed){ //if the mouse left button is pressed
            Vector2 mapPos = engine.camera->screenToWorldPos(mouse.cx, mouse.cy); 
            *x = mapPos.x; //store the locations of it
            *y = mapPos.y;
            return true;
        }else if(mouse.rbutton_pressed || lastKey.vk != TCODK_NONE){
            return false;
        }
        TCODConsole::flush();
    }
    return false;
}

/*
    Function to return the actor given a certain x and y as arguments
*/
Actor *Engine::getActor(int x, int y) const{
    for(Actor **iterator = map->actors.begin(); iterator != map->actors.end(); iterator++){ //iterate through all of our actors
        Actor *actor = *iterator;
        if(actor->x == x && actor->y == y && actor->destructible && !actor->destructible->isDead()){ //if the actor is there, destructible and not dead
            return actor; //return it
        }
    }
    return NULL; //otherwise, we found nothing
}

/*
    Return the closest monster to a position
    Takes 3 arguments: an x and y for the coordinates
                       a range for the distance to find a monster
*/
Actor *Engine::getClosestMonster(int x, int y, float range) const{
    Actor *closest = NULL;
    float bestDistance = 1E6f;

    for(Actor **iterator = map->actors.begin(); iterator != map->actors.end(); iterator++){ //iterate through all of our actors
        Actor *actor = *iterator;
        if(actor != player && actor->destructible && !actor->destructible->isDead()){ //if an actor isn't the player, is destructible and isn't dead
            float distance = actor->getDistance(x,y); //get the distance from the player to it
            if(distance < bestDistance && (distance <= range || range == 0.0f)){ //if it's closer than our best distance and is still in range OR on top of us
                bestDistance = distance; //this is the new best distance
                closest = actor; //this is the closest actor
            }
        }
    }
    return closest; //return it

}

/*
    This is the main update function, it is called every frame
    It is responsible for: checking game status and getting a new FOV for the player when necessary
                           check for mouse events
                           check if player presses escape (and thus saves and loads)
                           updates player position
                           updates camera position
*/
void Engine::update(){
    if(gameStatus == STARTUP) //if map is new
        map->computeFOV(); //get our FOV
    gameStatus = IDLE; //change status

    TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE, &lastKey, &mouse);
    if(lastKey.vk == TCODK_ESCAPE){
        save();
        load();
    }
    player->update(); 
    camera->update();
    static double coef = 0.0;
    if(gameStatus == NEW_TURN){ //happens when leaving a menu 
        map->ambientLight = TCODColor::lerp(TCODColor::lightYellow, TCODColor::blue, coef < 1 ? coef : 2 - coef);
                                                                                                //if coefficient < 1, then: coefficient = 2 - coefficient
        coef += 0.1; 
        if(coef > 2.0)
            coef -= 2.0;

        for (Actor **iterator = map->actors.begin(); iterator != map->actors.end(); iterator++) { //iterate through all actors
            Actor *actor=*iterator;
            if ( actor != player ) { //as long as it's not a player, update 
                actor->update();
            }
        }
    }
}

//Calls render functions for camera and gui
void Engine::render(){
    TCODConsole::root->clear();
    camera->render();
    gui->render();
}
