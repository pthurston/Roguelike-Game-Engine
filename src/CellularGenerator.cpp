#include "MapGenerator.hpp"
#include "Map.hpp"

CellularGenerator::CellularGenerator(Map &map, long seed, WriteMode writeMode) : MapGenerator(map, seed, writeMode){}

CellularGenerator::~CellularGenerator(){
    delete [] mapData;
}

void CellularGenerator::buildMap(bool withActors){
    mapData = new bool[map.width * map.height];

    for(int i = 0; i < map.width * map.height; i++){
        //True makes it a wall or non-walkable
        mapData[i] = mapRng->getFloat(0, 1) <= 0.45f ? true : false;
    }
    for(int i = 0; i < 4; i++){
        applyRules(4,6);
    }
    for(int i = 0; i < map.width * map.height; i++){
        if(writeMode == OVERWRITE)
            map.tiles[i].walkable = !mapData[i];
        else
            map.tiles[i].walkable |= !mapData[i];
    }
}

void CellularGenerator::applyRules(int survivalMin, int deathMin){
    bool *mapDataTmp = new bool[map.width * map.height];
    for(int x = 0; x < map.width; x++){
        for(int y = 0; y < map.height; y++){
            int neighborCount = getNeighborCount(x,y);
            int index = x + y * map.width;
            bool isWall = mapData[index];
            if(neighborCount >= deathMin && isWall)
                mapDataTmp[index] = false;
            if(neighborCount < survivalMin && isWall)
                mapDataTmp[index] = false;
            else if(neighborCount >= survivalMin && !isWall)
                mapDataTmp[index] = true;
            else
                mapDataTmp[index] = mapData[index];

        }
    }

    for(int i = 0; i < map.width * map.height; i++){
        mapData[i] = mapDataTmp[i];
    }
    delete[] mapDataTmp;
}

int CellularGenerator::getNeighborCount(int x, int y){
    int sum = 0;
    for(int dx = -1; dx <= 1; dx++){
        for(int dy = -1; dy <= 1; dy++){
            if(dx != 0 || dy != 0){
                int cx = x + dx, cy = y + dy;
                if(cx < 0 || cx >= map.width || cy < 0 || cy >= map.height)
                    sum++;
                else if(mapData[cx + cy * map.width]){
                    sum++;
                }
            }
        }
    }
    return sum;
}
