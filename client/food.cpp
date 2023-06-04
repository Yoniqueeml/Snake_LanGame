#include "food.h"

food::food(): x(0), y(0) {}
food::food(int x, int y): x(x), y(y) {}
int food::getX() const{ return x;};
int food::getY() const {return y;};
void food::setX(const int x){
    this->x = x;
}
void food::setY(const int y){
    this->y = y;
}
