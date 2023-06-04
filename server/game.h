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

    SocketHandler server;
    vector<int> clients;
public:
    vector <snake> allSnakes; // todo add to private

    game(int players);
    void generateFood();
    int getSnakeIndexFromDescriptor(int) ;

    void showInitialChoices();
    void syncSnakeWithClient(snake &);
    int getSnakeIndexFromID(int);
    void reset_max_screen();
    void printAnimated(string, int);
    void draw_snake(snake&);
    void draw_all_snakes();
    void KeyPressHandler();
    void printFood(string);
    void MainEventLoop() ;
    void ask_no_players(string) ; // Virezat nado
    void move_snake(snake&, int direction);
    void moveAllSnakes() ;
    void LAN_sendFoodCoordinates(int , int ) ;
    void setFoodPos(int , int) ;
    void printScores() ;
    void handleNewConnection() ;
    void handleIOActivity() ;
    int checkClientActivity() ; //returns activity number
    void initServerForMultiplayer();
    void setClientsList();
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
    void init_snake_on_screen(snake&);
    void check_snake_overlap(snake&);
    void handleMovementKeyPress(snake& snk, const int code);
    void gameOverHandler(const snake& snk);

    void initColors();

    void readData();
};

