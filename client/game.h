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

    const int maxX = 120;
    const int maxY = 40;

    SocketHandler sockObj;
public:
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
    void initSnakeOnScreen(snake&);
    void moveSnake(snake&, int);
    void handleMovementKeyPress(snake&, int);
    void checkSnakeOverlap(snake&);
    void readData();
    void drawBorderWindow();
};

