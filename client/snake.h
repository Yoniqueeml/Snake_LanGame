#include <string>
#include <vector>

#include "snake_part.h"

using namespace std;

class snake{
private:
    vector <snake_part> parts;
    int snakeDirection;
    string playerName;
    int score, id, bodyColor;

public:
    snake(int snakeid, string name="Unknown"){
        score = 0;
        snakeDirection = 2;
        id = snakeid;
        playerName = name;
        bodyColor = rand() % 5 + 1;
    }
    int getHeadX() const;
    int getHeadY() const;
    int getDirection() const;
    void setDirection(int);
    int getScore() const;
    int getBodyColor() const;
    void setBodyColor(int bodyColor) {this->bodyColor = bodyColor;}
    int setScore(int s);
    void addPart(int x, int y){
        snake_part obj(x,y);
        parts.push_back(obj);
    }
    const vector<snake_part>& getParts() const;
    vector<snake_part>& getParts();
    const snake_part& getPart(const int) const;
    void setParts(const std::vector<snake_part> _parts){ parts = _parts;}
    string getName() const;
};