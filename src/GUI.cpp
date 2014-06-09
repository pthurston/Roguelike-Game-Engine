#include "GUI.hpp"
#include "Engine.hpp"
#include "Actor.hpp"
#include "Destructible.hpp"
#include "AI.hpp"
#include "Map.hpp"
#include "Camera.hpp"
#include "Vector2.hpp"
#include <stdio.h>

static const int PANEL_HEIGHT = 7;
static const int BAR_WIDTH = 20;

static const int MSG_X = BAR_WIDTH + 2;
static const int MSG_HEIGHT = PANEL_HEIGHT - 1;

GUI::Message::Message(const char *text, const TCODColor &col) :
    text(strdup(text)), col(col){}

GUI::Message::~Message(){
    free(text);
}


GUI::GUI(){
    int nHeight = PANEL_HEIGHT;
    con = new TCODConsole(engine.screenWidth, nHeight);
}

GUI::~GUI(){
    delete con;
    clear();
}

void GUI::clear(){
    log.clearAndDelete();
}

void GUI::render(){
    con->setDefaultBackground(TCODColor::black);
    con->clear();

    renderBar(1, 1, BAR_WIDTH, "HP", engine.player->destructible->hp,
             engine.player->destructible->maxHp, TCODColor::lightRed, TCODColor::darkerRed);

    PlayerAI *ai = (PlayerAI *)engine.player->ai;
    char xpTxt[128];
    sprintf(xpTxt, "XP(%d)", ai->xpLevel);
    renderBar(1,2,BAR_WIDTH, xpTxt, engine.player->destructible->xp, ai->getNextLevelXP(), TCODColor::lightViolet, TCODColor::darkerViolet);

    int y = 1;
    float colorCoef = 0.4f;
    for(Message **it = log.begin(); it != log.end(); it++){
        Message *message = *it;
        con->setDefaultForeground(message->col * colorCoef);
        con->print(MSG_X, y, message->text);
        y++;
        if(colorCoef < 1.0f)
            colorCoef += 0.3f;
    }

    renderMouseLook();

    con->setDefaultForeground(TCODColor::white);
    con->print(3,5,"Dungeon level %d", engine.level);

    TCODConsole::blit(con, 0, 0, engine.screenWidth, PANEL_HEIGHT,
                      TCODConsole::root, 0, engine.screenHeight - PANEL_HEIGHT);

}

void GUI::renderBar(int x, int y, int width, const char *name,
                    float value, float maxValue, const TCODColor &barColor,
                    const TCODColor &backColor){

    con->setDefaultBackground(backColor);
    con->rect(x,y,width,1,false, TCOD_BKGND_SET);

    int barWidth = (int)(value / maxValue * width);
    if(barWidth > 0){
        con->setDefaultBackground(barColor);
        con->rect(x,y,barWidth,1,false, TCOD_BKGND_SET);
    }

    con->setDefaultForeground(TCODColor::white);
    //printf("Print bar(%d,%d) %s : %g/%g\n", x + width/2, y, name, value, maxValue);
    con->printEx(x+width/2, y, TCOD_BKGND_NONE, TCOD_CENTER, "%s : %g/%g", name, value, maxValue);
}

void GUI::renderMouseLook(){
    Vector2 mapPos = engine.camera->screenToWorldPos(engine.mouse.cx, engine.mouse.cy);
    if(!engine.map->isInFOV(mapPos.x, mapPos.y)){
        return;
    }
    char buf[128]="";
    bool first = true;
    for(Actor **it = engine.map->actors.begin(); it != engine.map->actors.end(); it++){
        Actor *actor = *it;
        if(actor->x == mapPos.x && actor->y == mapPos.y){
            if(!first){
                strcat(buf, ", ");
            }else {
                first = false;
            }
            strcat(buf, actor->name);
            if(actor->destructible && !actor->destructible->isDead()){
                char hp[10];
                sprintf(hp, "(%d/%d)", (int)actor->destructible->hp, (int)actor->destructible->maxHp);
                strcat(buf, hp);
            }
        }
    }
    con->setDefaultForeground(TCODColor::lightGrey);
    con->print(1,0,buf);
}

void GUI::message(const TCODColor &col, const char *text, ...){
    va_list ap;
    char buf[128];
    va_start(ap, text);
    vsprintf(buf, text, ap);
    va_end(ap);

    char *lineBegin = buf;
    char *lineEnd;

    do{
        if(log.size() == MSG_HEIGHT){
            Message *toRemove = log.get(0);
            log.remove(toRemove);
            delete toRemove;
        }
        lineEnd = strchr(lineBegin, '\n');
        if(lineEnd){
            *lineEnd = '\0';
        }

        Message *msg = new Message(lineBegin, col);
        log.push(msg);

        lineBegin = lineEnd +1;
    }while(lineEnd);
}

void GUI::save(TCODZip &zip){
    zip.putInt(log.size());
    for(Message **it = log.begin(); it != log.end(); it++){
        zip.putString((*it)->text);
        zip.putColor(&(*it)->col);
    }
}

void GUI::load(TCODZip &zip){
    int nbMessages = zip.getInt();
    while(nbMessages > 0){
        const char *text = zip.getString();
        TCODColor col = zip.getColor();
        message(col, text);
        nbMessages--;
    }
}

Menu::~Menu(){
    clear();
}

void Menu::clear(){
    items.clearAndDelete();
}

void Menu::addItem(MenuItemCode code, const char *label){
    MenuItem *item = new MenuItem();
    item->code = code;
    item->label = label;
    items.push(item);
}
const int PAUSE_MENU_WIDTH = 30;
const int PAUSE_MENU_HEIGHT = 15;

Menu::MenuItemCode Menu::pick(DisplayMode mode){

    int selectedItem = 0;
    int menuX, menuY;

    if(mode == PAUSE){
        menuX = engine.screenWidth / 2 - PAUSE_MENU_WIDTH / 2;
        menuY = engine.screenHeight / 2 - PAUSE_MENU_HEIGHT / 2;
        TCODConsole::root->setDefaultForeground(TCODColor(200, 180, 50));
        TCODConsole::root->printFrame(menuX, menuY, PAUSE_MENU_WIDTH, PAUSE_MENU_HEIGHT, true, TCOD_BKGND_ALPHA(70), "menu");
        menuX += 2;
        menuY += 3;
    }else{
        static TCODImage img("menu_background1.png");
        img.blit2x(TCODConsole::root,0,0);
        menuX = 10;
        menuY = TCODConsole::root->getHeight()/3;
    }
    while(!TCODConsole::isWindowClosed()){
        int currentItem = 0;
        for(MenuItem **it = items.begin(); it != items.end(); it++){
            if(currentItem == selectedItem){
                TCODConsole::root->setDefaultForeground(TCODColor::lighterOrange);
            }else{
                TCODConsole::root->setDefaultForeground(TCODColor::lightGrey);
            }
            TCODConsole::root->print(menuX, menuY + currentItem * 3, (*it)->label);
            currentItem++;
        }
        TCODConsole::flush();
        TCOD_key_t key;
        TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL);
        switch(key.vk){
        case TCODK_UP:
            selectedItem--;
            if(selectedItem < 0)
                selectedItem = items.size() - 1;
            break;
        case TCODK_DOWN:
            selectedItem = (selectedItem + 1) % items.size();
            break;
        case TCODK_ENTER:
            return items.get(selectedItem)->code;
        default : break;
        }
    }
    return NONE;
}
