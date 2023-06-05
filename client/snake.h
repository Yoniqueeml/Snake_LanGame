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
    snake(int snakeid, string name="Name"){
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
    int setScore(int s);
    void addPart(int, int);
    const vector<snake_part>& getParts() const;
    vector<snake_part>& getParts();
    const snake_part& getPart(const int) const;
    string getName() const;
};