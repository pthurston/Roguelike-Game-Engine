#include "Vector2.hpp"
#include <math.h>

Vector2::Vector2() : x(0), y(0){}

Vector2::Vector2(int x, int y) : x(x), y(y){}

const Vector2 & Vector2::operator=(const Vector2 &other){
    x = other.x;
    y = other.y;
    return *this;
}

Vector2 Vector2::operator+(const Vector2 &other){
    return Vector2(other.x + x, other.y + y);
}

Vector2 Vector2::operator-(const Vector2 &other){
    return Vector2(x - other.x, y - other.y);
}

double Vector2::magnitude(){
    return sqrt(x*x + y*y);
}

double Vector2::sqrMagnitude(){
    return x*x + y*y;
}

