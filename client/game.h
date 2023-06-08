#include "food.h"
#include "snake.h"
#include "server.h"

class game{
private:
    int noOfPlayers;
    food foodObj;
    int centerX, centerY;
    snake * mainSnakePtr;

    int socketDescriptor = -1;

    const int maxX = 80;
    const int maxY = 30;

    SocketHandler sockObj;
public:
    vector<snake> allSnakes;
    game(int players);
    void generateFood();
    void setMainSnakePtr(snake*);
    void printFood(string);
    void printAnimated(string, int);
    void initColors();
    void setFoodPos(int, int) ;
    void handleMessageFromServer(string);
    int getFoodX();
    int getFoodY();
    int getCenterX();
    int getCenterY();
    food getFoodPos();
    int getNoOfPlayers();
    void resetMaxScreen();
    void setNoOfPlayers(int n);
    void initConsoleScreen(string);
    SocketHandler& getSockObj();
    void gameOverHandler(const snake& snk);
    void printScore(const snake&, string pos = "left");
    void drawSnake(const snake&);
    void drawAllSnakes();
    void initSnakeOnScreen(snake&);
    void moveSnake(snake&, int);
    void moveMainSnake(snake&, int);
    void moveAllSnakes();
    void handleMovementKeyPress(snake&, int);
    void checkSnakeOverlap(snake&);
    int readData();
    void drawBorderWindow();
};