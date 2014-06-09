#ifndef _RECT_H_INCLUDED
#define _RECT_H_INCLUDED

class Rect{
public:
    int x1;
    int y1;
    int x2;
    int y2;
    Rect(int x1, int y1, int x2, int y2);
    int width();
    int height();
    void printRect();
};

#endif // _RECT_H_INCLUDED
