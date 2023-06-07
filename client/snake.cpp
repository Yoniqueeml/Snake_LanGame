#include "snake.h"

#define coloron(a) attron(COLOR_PAIR(a))
#define coloroff(a) attroff(COLOR_PAIR(a))

int snake::getHeadX() const{
    return parts.at(parts.size() - 1).getX();
}

int snake::getHeadY() const{
    return parts.at(parts.size() - 1).getY();
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
    return bodyColor;
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
    return playerName;
}

void snake::setBodyColor(int bodyColor){
    this->bodyColor = bodyColor;
}

void snake::setId(int newId){
    this->id = newId;
}

void snake::setParts(const std::vector<snake_part> newParts){
    this->parts = newParts;
}

int snake::getId() const {
    return id;
}