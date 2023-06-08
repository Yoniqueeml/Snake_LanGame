#include "food.h"
#include "snake.h"
#include "server.h"

#include <ncurses.h>
#include <iostream>
#include <csignal>

class game{
private:
    int noOfPlayers;
    food foodObj;
    int centerX, centerY;

    const int maxX = 80;
    const int maxY = 30;

    socketHandler server;
    vector<int> clients;

    int prevClientId = 0;
public:
    vector <snake> allSnakes;

    game(int players);
    void generateFood();
    int getSnakeIndexFromDescriptor(int);

    void showInitialChoices();
    void sendSnakes();
    void syncSnakeWithClient(snake &);
    int getSnakeIndexFromID(int);
    void resetMaxScreen();
    void printAnimated(string, int);
    void drawSnake(snake&);
    void drawAllSnakes();
    void printFood(string);
    void moveSnake(snake&, int direction);
    void moveAllSnakes();
    void LANSendFoodCoordinates(int, int );
    void setFoodPos(int, int);
    void printScores();
    void handleNewConnection();
    void handleIOActivity();
    int checkClientActivity(); //returns activity number
    void initServerForMultiplayer();
    void handleActivity();
    int getFoodX();
    int getFoodY();
    int getCenterX();
    int getCenterY();
    void setClientsList(vector<int>&);
    food getFoodPos();
    int getNoOfPlayers();
    void setNoOfPlayers(int n);
    void initConsoleScreen(string);
    void initSnakeOnScreen(snake&);
    void checkSnakeOverlap(snake&);
    void handleMovementKeyPress(snake& snk, const int code);
    void gameOverHandler(const snake& snk);

    void initColors();
    void readData();
    socketHandler& getServer();
    void drawBorderWindow();
};