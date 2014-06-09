#ifndef _VECTOR_H_INCLUDED
#define _VECTOR_H_INCLUDED

class Vector2{
public:
    int x,y;
    Vector2();
    Vector2(int x, int y);
    const Vector2 & operator=(const Vector2 &);
    Vector2 operator+(const Vector2 &);
    Vector2 operator-(const Vector2 &);
    double magnitude();
    double sqrMagnitude();
};

#endif // _VECTOR_H_INCLUDED
