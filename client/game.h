#include "food.h"
#include "snake.h"
#include "server.h"

class game{
private:
    int noOfPlayers;
    food foodObj;
    int center_x, center_y;
    snake * mainSnakePtr;

    int socket_descriptor =-1;

    int maxX = 0;
    int maxY = 0;

    SocketHandler sock_obj;
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
    void reset_max_screen();
    void setNoOfPlayers(int n);
    void initConsoleScreen(string);
    SocketHandler& getSockObj();
    void gameOverHandler(const snake& snk);
    void printScore(const snake&, string pos="left");
    void draw_snake(const snake&);
    void init_snake_on_screen(snake&);
    void move_snake(snake&, int);
    void handleMovementKeyPress(snake&, int);
    void check_snake_overlap(snake&);
    void readData();
};

