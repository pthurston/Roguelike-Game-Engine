#include "Map.hpp"
#include "MapGenerator.hpp"
#include "Actor.hpp"
#include "Engine.hpp"
#include "Camera.hpp"

Map::Map(int width, int height, Map *nextLevel, Map *prevLevel) : width(width), height(height), nextLevel(nextLevel), prevLevel(prevLevel){
    seed = TCODRandom::getInstance()->getInt(0, 0x7FFFFFFF);

}

Map::~Map(){
    actors.clearAndDelete();
    delete [] tiles;
}

void Map::deleteMaps(NodeType type){
    if(nextLevel && (type == ROOT || type == BELOW_ROOT))
        nextLevel->deleteMaps(BELOW_ROOT);
    if(prevLevel && (type == ROOT || type == ABOVE_ROOT))
        prevLevel->deleteMaps(ABOVE_ROOT);

    delete this;
}

void Map::init(bool withActors){
    tiles = new Tile[width * height];

    BSPGenerator bspGen(*this, seed);
    CellularGenerator caGen(*this, seed, MapGenerator::ADDTO);

    bspGen.buildMap(withActors);
    caGen.buildMap(withActors);
}

void Map::save(TCODZip &zip, NodeType type){
    zip.putInt(seed);
    for(int i = 0; i < width * height; i++){
        zip.putInt(tiles[i].explored);
        //zip.putInt(tiles[i].walkable);
        //zip.putInt(tiles[i].inFOV);
    }

    zip.putInt(actors.size());
    for(Actor **it = actors.begin(); it != actors.end(); it++){
        (*it)->save(zip);
    }

    zip.putInt(nextLevel != NULL);
    zip.putInt(prevLevel != NULL);

    if(nextLevel && (type == ROOT || type == BELOW_ROOT)){
        zip.putInt(nextLevel->width);
        zip.putInt(nextLevel->height);
        nextLevel->save(zip, BELOW_ROOT);
    }
    if(prevLevel && (type == ROOT || type == ABOVE_ROOT)){
        zip.putInt(prevLevel->width);
        zip.putInt(prevLevel->height);
        prevLevel->save(zip, ABOVE_ROOT);
    }
}

void Map::load(TCODZip &zip, NodeType type){
    seed = zip.getInt();
    init(false);
    for(int i = 0; i < width * height; i++){
        tiles[i].explored = zip.getInt();
        //tiles[i].walkable = zip.getInt();
        //tiles[i].inFOV = zip.getInt();
    }

    int nbActors = zip.getInt();
    while(nbActors > 0){
        Actor *actor = new Actor(0, 0, 0, NULL, TCODColor::white);
        actor->load(zip);
        actors.push(actor);
        nbActors--;
    }

    bool hasNextLevel = zip.getInt();
    bool hasPrevLevel = zip.getInt();

    if(hasNextLevel && (type == ROOT || type == BELOW_ROOT)){
        int newWidth = zip.getInt();
        int newHeight = zip.getInt();
        nextLevel = new Map(newWidth, newHeight, NULL, this);
        nextLevel->load(zip, BELOW_ROOT);
    }
    if(hasPrevLevel && (type == ROOT || type == ABOVE_ROOT)){
        int newWidth = zip.getInt();
        int newHeight = zip.getInt();
        prevLevel = new Map(newWidth, newHeight, this, NULL);
        prevLevel->load(zip, ABOVE_ROOT);
    }
}

bool Map::isInBounds(int x, int y) const{
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool Map::isWall(int x, int y) const{
    return !tiles[x + y*width].walkable;
}

bool Map::canWalk(int x, int y) const {
    if(isWall(x,y)){
        return false;
    }
    for(Actor **iterator = actors.begin(); iterator != actors.end(); iterator++){
        Actor *actor = *iterator;
        if(actor->blocks && actor->x == x && actor->y == y){
            return false;
        }
    }
    return true;
}

bool Map::isExplored(int x, int y) const{
    return tiles[x + y*width].explored;
}

bool Map::isInFOV(int x, int y) const {
    if(x < 0 || x >= width || y < 0 || y >= height)
        return false;

    if(tiles[x + y*width].inFOV)
        return true;

    return false;
}

void Map::computeFOV(){
    //Reset all tiles that were previously in the FOV and mark them as explored
    for(int x = 0; x < width; x++){
        for(int y = 0; y < height; y++){
            if(tiles[x + y*width].inFOV){
                tiles[x + y*width].explored = true;
                tiles[x + y*width].inFOV = false;
            }
        }
    }
    tiles[engine.player->x + engine.player->y * width].inFOV = true;
    for(float angle = 0; angle < 360.0f; angle += 0.18f){
        int dist = 0;
        float x = (float)engine.player->x + 0.5f;
        float y = (float)engine.player->y + 0.5f;
        float dx = cos(angle);
        float dy = sin(angle);
        while(dist < engine.fovRadius){
            x += dx;
            y += dy;
            if(x < 0 || x >= width || y < 0 || y >= height)
                break;

            int index = (int)x + (int)y * width;
            tiles[index].inFOV = true;
            if(isWall((int)x,(int)y))
                break;

            dist++;
        }
    }
}

void Map::render() const{
    static const TCODColor darkWall(0,0,100);
    static const TCODColor darkGround(50,50,150);
    static const TCODColor lightWall(130, 110, 50);
    static const TCODColor lightGround(200, 180, 50);



    for(int x = 0; x < engine.camera->camWidth; x++){
        for(int y = 0; y < engine.camera->camHeight; y++){
            Vector2 mapPos = engine.camera->screenToWorldPos(x,y);
            if(engine.map->isInBounds(mapPos.x, mapPos.y)){
                if(isInFOV(mapPos.x, mapPos.y))
                    TCODConsole::root->setCharBackground(x,y, (isWall(mapPos.x, mapPos.y) ? lightWall : lightGround));
                else if(isExplored(mapPos.x, mapPos.y)){
                    TCODConsole::root->setCharBackground(x,y, isWall(mapPos.x, mapPos.y) ? darkWall : darkGround);
                }
            }
        }
    }
}
