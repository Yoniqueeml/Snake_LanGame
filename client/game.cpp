#include "game.h"

#include <ncurses.h>
#include <iostream>
#include <csignal>

#define RED 7
#define GREEN 2
#define YELLOW 3
#define BLUE 1
#define BLACK 0
#define CYAN 4
#define WHITE 8
#define MAGENTA 5

#define coloron(a) attron(COLOR_PAIR(a))
#define coloroff(a) attroff(COLOR_PAIR(a))

game::game(int players){
    noOfPlayers= players;
}

void game::generateFood(){
    int x = random() % maxX , y = random() % maxY;
    if (!x) x = 2;
    if (!y) y = 2;
    coloron(RED);
    mvprintw(y, x,"@");
    coloroff(RED);
    setFoodPos(x , y);
}

void game::setMainSnakePtr(snake* ptr){
    mainSnakePtr = ptr;
}

void game::printFood(string status){
    if (status=="new")
        generateFood();

    if (!getFoodX() && !getFoodY())
        generateFood();
    coloron(RED);
    mvprintw(foodObj.getY(), foodObj.getX(), "@");
    coloroff(RED);
}

void game::printAnimated(string msg, int speed = 6000){
    for (int c = 0; msg[c]; c++){
        cout << msg[c];
        cout.flush();
        usleep(speed);
    }
}

void game::initColors(){
    init_pair(RED, COLOR_RED, COLOR_BLACK);
    init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(WHITE, COLOR_WHITE, COLOR_BLACK);
    init_pair(MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(CYAN, COLOR_CYAN, COLOR_BLACK);
}

void game::setFoodPos(int x, int y){
    foodObj.setX(x);
    foodObj.setY(y);
}

void game::handleMessageFromServer(string msg){
    if (msg.find(":") != string::npos){
        int start_colon = msg.find(":");

        int camma = msg.find(",");
        string str_x = msg.substr(start_colon + 1 , camma - 1);
        string str_y = msg.substr(camma+1, 3);

        int x  = stoi(str_x), y = stoi(str_y);

        setFoodPos(x, y);
    }

    //Handle game over message sent from server
    if (msg.find("$")!=string::npos){
        int start = msg.find("$");
        string num = "";
        for (int i = start + 1; msg[i] != '$'; i++){
            num += msg[i];
        }
        mainSnakePtr->setScore(stoi(num));
        gameOverHandler(*mainSnakePtr);
    }
}

int game::getFoodX(){
    return foodObj.getX();
}

int game::getFoodY(){
    return foodObj.getY();
}

int game::getCenterX(){
    return center_x;
}

int game::getCenterY(){
    return center_y;
}

food game::getFoodPos(){
    food obj = foodObj;
    return obj;
}

int game::getNoOfPlayers(){
    return noOfPlayers;
}

void game::reset_max_screen(){
    getmaxyx(stdscr, maxY, maxX);
}

void game::setNoOfPlayers(int n){
    noOfPlayers  = n;
}

void game::initConsoleScreen(string state){
    if(state=="on"){
        initscr(); //Init screen
        noecho(); // Dont show any pressed char
        curs_set(false); // Don't show the cursor
        start_color();
        getmaxyx(stdscr, maxY, maxX);
        center_x = maxX / 2, center_y = maxY / 2;
        cbreak(); //Dont wait for enter to be pressed when using getch
        nodelay(stdscr, 1);  //Use non blocking input for getch which just returns ERR if there is no input (ERR=-1)
    }

    else if(state == "off"){
        clear();
        flushinp();
        fflush(stdin);
        use_default_colors();
        endwin();
        cout.flush();
        system("clear");
    }
}

SocketHandler& game::getSockObj(){
    return sock_obj;
}

void game::gameOverHandler(const snake& snk){
    sock_obj.sendData("$");
    clear();
    initConsoleScreen("off");
    system("clear");
    string gameovermessage = "\n\n\nGAME OVER " + snk.getName() + " :(\n\n";
    gameovermessage += "Score : " + std::to_string(snk.getScore()) + "\n";
    gameovermessage += "\n\nPress ctrl+c to exit.";
    printAnimated(gameovermessage);
    sock_obj.closeSocket();
    sleep(5000);
}

void game::printScore(const snake& snk, string pos){
    if (pos == "right")
        mvprintw(0, 15, "Score = %d", snk.getScore());
    mvprintw(0, 0, "Score = %d", snk.getScore());
}

void game::draw_snake(const snake& snk){
    int i;
    coloron(snk.getBodyColor());
    for(i =0; i < snk.getParts().size() - 1; i++){
        mvprintw(snk.getPart(i).getY(), snk.getPart(i).getX(), "o");
    }
    coloroff(snk.getBodyColor());
    mvprintw(snk.getPart(i).getY(), snk.getPart(i).getX(), "+");
}

void game::init_snake_on_screen(snake& snk){
    snk.add_part(getCenterX(), getCenterY());
    snk.add_part(getCenterX() + 1, getCenterY());
    snk.add_part(getCenterX() + 2,getCenterY());

    draw_snake(snk);
}

void game::move_snake(snake& snk, int direction){

    snk.getParts().erase(snk.getParts().begin());
    snake_part last_part = snk.getParts().at(snk.getParts().size() - 1);

    if(direction == 2){
        snk.add_part((last_part.getX() + 1) % maxX, last_part.getY());
        snk.setDirection(2);
    }
    else if(direction == 0){
        snk.add_part((last_part.getX() - 1) < 0 ? maxX - 1 :(last_part.getX() - 1), last_part.getY());
        snk.setDirection(0);
    }
    else if(direction == 1){
        snk.add_part(last_part.getX() , (last_part.getY() - 1) < 0 ? maxY - 1:(last_part.getY() - 1));
        snk.setDirection(1);
    }
    else if(direction == 3){
        snk.add_part(last_part.getX(), (last_part.getY() + 1) % maxY);
        snk.setDirection(3);
    }

    check_snake_overlap(snk);

    if (snk.getHeadX() == getFoodX() && snk.getHeadY() == getFoodY()){
        snk.add_part(getFoodX(), getFoodY());
        snk.setScore(snk.getScore() + 1);
        sock_obj.sendData("#");

        setFoodPos(-10, -10);
    }
    draw_snake(snk);
    refresh();
}

void game::handleMovementKeyPress(snake& snk, const int code){
    if(code==17 || code == 103){
        if (snk.getDirection() != 3 && snk.getDirection() != 1)
            move_snake(snk, 1);
    }
    else if(code==31 || code==108){
        if (snk.getDirection() != 1 && snk.getDirection() != 3)
            move_snake(snk, 3);
    }
    else if(code==30 || code==106){
        if (snk.getDirection() != 0 && snk.getDirection() != 2)
            move_snake(snk, 2);
    }
    else if(code==32 || code==105){
        if (snk.getDirection() != 2 && snk.getDirection() != 0)
            move_snake(snk, 0);
    }
    else return;
}

void game::check_snake_overlap(snake& snk){
    int headX = snk.getHeadX(), headY  = snk.getHeadY();
    for (int i = 0; i < snk.getParts().size() - 1; i++)
        if (snk.getPart(i).getX() == headX && snk.getPart(i).getY() == headY){
            if (socket_descriptor > 0)
                return;
            gameOverHandler(snk);
        }
}

void game::readData(){
    int val;
    FD_ZERO(&sock_obj.getSet());
    FD_SET(sock_obj.getSock(), &sock_obj.getSet());

    val = select(sock_obj.getSock() + 1, &sock_obj.getSet(), NULL, NULL, &sock_obj.getTimeout());

    if(val > 0){
        recv(sock_obj.getSock(), sock_obj.getBuffer(), 1024, 0);
        handleMessageFromServer(string(sock_obj.getBuffer()));
    }

    else if(val == -1){
        perror("select");
    }
}