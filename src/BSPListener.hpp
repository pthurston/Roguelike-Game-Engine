#ifndef _LISTENER_H_INCLUDED
#define _LISTENER_H_INCLUDED

#include "MapGenerator.hpp"

class BSPListener{
public:
    BSPListener(BSPGenerator &bspGen) : bspGen(bspGen), roomNum(0){}
    void visitNode(BinNode<Rect> *node, bool withActors){
        if(node->isLeaf()){
            Rect area = node->getData();
            int roomWidth = bspGen.mapRng->getInt(ROOM_MIN_SIZE, area.width() - 2);
            int roomHeight = bspGen.mapRng->getInt(ROOM_MIN_SIZE, area.height() - 2);
            int x = bspGen.mapRng->getInt(area.x1, area.width() - roomWidth + area.x1);
            int y = bspGen.mapRng->getInt(area.y1, area.height() - roomHeight + area.y1);
            //printf("Room: %d %d %d %d\n", x, y, roomWidth, roomHeight);
            bspGen.createRoom(roomNum == 0, x, y, roomWidth, roomHeight, withActors);
            roomNum++;
        }else if(node->left != NULL && node->right != NULL){
            Rect leftArea = node->left->getData();
            Rect rightArea = node->right->getData();
            int cx1 = (leftArea.x1 + leftArea.x2) / 2;
            int cy1 = (leftArea.y1 + leftArea.y2) / 2;
            int cx2 = (rightArea.x1 + rightArea.x2) / 2;
            int cy2 = (rightArea.y1 + rightArea.y2) / 2;
            //bspGen.addDoor(cx1, cy1);
            if(cx2 <= cx1 && cy2 <= cy1){
                bspGen.dig(cx2, cy2, cx1 - cx2 + 1, cy1 - cy2 + 1);
            }else{
                bspGen.dig(cx1, cy1, cx2 - cx1 + 1, cy2 - cy1 + 1);
            }
        }
    }
private:
    BSPGenerator &bspGen;
    int roomNum;
};

#endif // _LISTENER_H_INCLUDED
