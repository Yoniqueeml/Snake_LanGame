#include "snake.h"

#define coloron(a) attron(COLOR_PAIR(a))
#define coloroff(a) attroff(COLOR_PAIR(a))

int snake::getHeadX() const{
    return parts.at(parts.size()-1).getX();
}

int snake::getHeadY() const{
    return parts.at(parts.size()-1).getY();
}

int snake::getDirection() const{
    return snakeDirection;
}

void snake::setDirection(int newDirection) {
    snakeDirection = newDirection;
}

int snake::getScore() const{
    return score;
}

int snake::getBodyColor() const{
    return bodycolor;
}

void snake::add_part(int x , int y){
    snake_part obj(x , y) ;
    parts.push_back(obj);
}

int snake::setScore(int s){
    score = s;
    return score;
}


const vector<snake_part>& snake::getParts() const{
    return parts;
}

vector<snake_part>& snake::getParts(){
    return parts;
}

const snake_part& snake::getPart(const int i) const{
    return parts[i];
}


string snake::getName() const{
    return player_name;
}