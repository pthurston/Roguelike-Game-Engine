#include "Container.hpp"
#include "Actor.hpp"
#include "Engine.hpp"

using namespace std;

Container::Container(int size, const char *name) : size(size), name(name){}

Container::~Container(){
    for(int i = 0; i < inventory.size(); i++){
        delete inventory[i];
    }
}

bool Container::add(Actor *actor){
    if(size > 0 && inventory.size() >= size){
        return false;
    }
    inventory.push_back(actor);
    itemCount.push_back(1);
    return true;
}

void Container::remove(Actor *actor){
    for(int i = 0; i < inventory.size(); i++){
        inventory.erase(inventory.begin() + i);
    }
}

Actor *Container::chooseFromInventory(Actor *owner){
    static const int INVENTORY_WIDTH = 50;
    static const int INVENTORY_HEIGHT = 28;
    static TCODConsole con(INVENTORY_WIDTH, INVENTORY_HEIGHT);

    con.setDefaultForeground(TCODColor(200, 180, 50));
    con.printFrame(0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, true, TCOD_BKGND_DEFAULT, name);

    con.setDefaultForeground(TCODColor::white);
    int shortcut = 'a';
    int y = 1;
    for(int i = 0; i < inventory.size(); i++){
        Actor *actor = inventory[i];
        con.print(2, y, "(%c) %s", shortcut, actor->name);
        y++;
        shortcut++;
    }

    TCODConsole::blit(&con, 0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, TCODConsole::root,
                      engine.screenWidth/2 - INVENTORY_WIDTH/2, engine.screenHeight/2 - INVENTORY_HEIGHT/2);

    TCODConsole::flush();

    TCOD_key_t key;
    TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
    if(key.vk == TCODK_CHAR){
        int actorIndex = key.c - 'a';
        if(actorIndex >= 0 && actorIndex < inventory.size()){
            return inventory[actorIndex];
        }
    }
    return NULL;
}

void Container::load(TCODZip &zip){
    size = zip.getInt();
    name = zip.getString();
    int nbActors = zip.getInt();
    while(nbActors > 0){
        Actor *actor = new Actor(0,0,0,NULL, TCODColor::white);
        actor->load(zip);
        inventory.push_back(actor);
        nbActors--;
    }
}

void Container::save(TCODZip &zip){
    zip.putInt(size);
    zip.putString(name);
    zip.putInt(inventory.size());
    for(int i = 0; i < inventory.size(); i++){
        inventory[i]->save(zip);
    }
}


