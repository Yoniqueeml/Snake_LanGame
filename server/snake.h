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
    int socketDescriptor;

public:
    snake(int snakeid, int socket_descriptor, string name="Name"){
        score = 0;
        snakeDirection = 2;
        id = snakeid;
        playerName = name;
        this->socketDescriptor = socket_descriptor;
        bodyColor = rand() % 5 + 1;
    }

    int getHeadX() const;
    int getHeadY() const;
    int getDirection() const;
    void setDirection(int);
    int getScore() const;
    int getId() const;
    int getSocketDescriptor() const;
    int getBodyColor() const;
    void setBodyColor(int);
    int setScore(int s);
    void addPart(int, int);
    const vector<snake_part>& getParts() const;
    vector<snake_part>& getParts();
    snake_part& getPart(const int);
    string getName() const;
    void setName(string);
};