#include "snake_part.h"

snake_part::snake_part(const int x,const int y){
    this->x = x ; this->y = y ;
}

int snake_part::getX() const{
    return x;
}

void snake_part::setX(int x){
    this->x = x;
}

int snake_part::getY() const{
    return y;
}

void snake_part::setY(int y){
    this->y = y;
}