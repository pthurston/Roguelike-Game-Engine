#include "main.hpp"

Rect::Rect(int x1, int y1, int x2, int y2) : x1(x1), y1(y1), x2(x2), y2(y2){}

int Rect::width(){
    return x2 - x1 + 1;
}

int Rect::height(){
    return y2 - y1 + 1;
}

void Rect::printRect(){
    printf("%d %d %d %d\n", x1, y1, x2, y2);
}
