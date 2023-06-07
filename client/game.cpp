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
    if (status == "new")
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

std::vector<snake> deserializeSnakes(const int startInd, const std::string& data) {
    std::vector<snake> Snakes;
    int i = startInd;
    while (data[i] != ')') {
        i += 1;
        snake s((int)(data[i] - '0'));
        i += 1;
        s.setDirection(data[i] - '0');
        i += 1;
        s.setBodyColor((int)(data[i] - '0'));
        i += 1;
        int partsSize = (int)(data[i] - '0');
        i += 1;
        for (int j = 0; j < partsSize; j++) {
            int x = int(data[i] - '0');
            i += 1;
            int y = int(data[i] - '0');
            i += 1;
            s.addPart(x, y);
        }
        Snakes.push_back(s);
    }
    return Snakes;
}

void game::handleMessageFromServer(string msg){
    if (msg.find("!") != string::npos){
        int start_colon = msg.find("!");

        int camma = msg.find(",");
        string str_x = msg.substr(start_colon + 1 , camma - 1);
        string str_y = msg.substr(camma + 1, 3);

        int x  = stoi(str_x), y = stoi(str_y);

        setFoodPos(x, y);
    }


    //Handle game over message sent from server
    if (msg.find("$") != string::npos){
        int start = msg.find("$");
        string num = "";
        for (int i = start + 1; msg[i] != '$'; i++){
            num += msg[i];
        }
        mainSnakePtr->setScore(stoi(num));
        gameOverHandler(*mainSnakePtr);
    }

    if (msg.find("(") != string::npos) {
        allSnakes = deserializeSnakes(msg.find("("), msg);
    }

    if (msg.find("+") != string::npos){
        mainSnakePtr->setId(atoi(&msg[msg.find("+") + 1]));
    }
}

int game::getFoodX(){
    return foodObj.getX();
}

int game::getFoodY(){
    return foodObj.getY();
}

int game::getCenterX(){
    return centerX;
}

int game::getCenterY(){
    return centerY;
}

food game::getFoodPos(){
    food obj = foodObj;
    return obj;
}

int game::getNoOfPlayers(){
    return noOfPlayers;
}

void game::resetMaxScreen(){
    int curX = 0, curY = 0;
    getmaxyx(stdscr, curY, curX);
    if (curX < maxX || curY < maxY){
        clear();
        initConsoleScreen("off");
        system("clear");
        cout << "\nConsole size is too small!!! Needed:" << maxX << "x" << maxY << endl << endl;
        sleep(5000);
    }
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
        int curX = 0, curY = 0;
        getmaxyx(stdscr, curY, curX);
        if (curX < maxX || curY < maxY){
            clear();
            initConsoleScreen("off");
            system("clear");
            cout << "\nConsole size is too small!!! Needed:" << maxX << "x" << maxY << endl << endl;
            sleep(5000);
        }
        centerX = maxX / 2, centerY = maxY / 2;
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
    return sockObj;
}

void game::gameOverHandler(const snake& snk){
    sockObj.sendData("$");
    clear();
    initConsoleScreen("off");
    system("clear");
    string gameovermessage = "\n\n\nGAME OVER " + snk.getName() + " :(\n\n";
    gameovermessage += "Score : " + std::to_string(snk.getScore()) + "\n";
    gameovermessage += "\n\nPress ctrl+c to exit.";
    printAnimated(gameovermessage);
    sockObj.closeSocket();
    sleep(5000);
}

void game::printScore(const snake& snk, string pos){
    if (pos == "right")
        mvprintw(0, 15, "Score = %d", snk.getScore());
    mvprintw(0, 0, "Score = %d", snk.getScore());
}

void game::drawSnake(const snake& snk){
    int i;
    coloron(snk.getBodyColor());
    for (i = 0; i < snk.getParts().size() - 1; i++){
        mvprintw(snk.getPart(i).getY(), snk.getPart(i).getX(), "o");
    }
    coloroff(snk.getBodyColor());
    mvprintw(snk.getPart(i).getY(), snk.getPart(i).getX(), "+");
}

void game::drawAllSnakes(){
    for (int i = 0; i < allSnakes.size(); i++){
        if (allSnakes[i].getId() != mainSnakePtr->getId()) {
            drawSnake(allSnakes[i]);
        }
    }
}

void game::initSnakeOnScreen(snake& snk){
    snk.addPart(getCenterX(), getCenterY());
    snk.addPart(getCenterX() + 1, getCenterY());
    snk.addPart(getCenterX() + 2, getCenterY());

    drawSnake(snk);
}

void game::moveSnake(snake& snk, int direction){
    snk.getParts().erase(snk.getParts().begin());
    snake_part last_part = snk.getParts().at(snk.getParts().size() - 1);

    if (direction == 2){
        snk.addPart(last_part.getX() % (maxX - 1) + 1, last_part.getY());
        snk.setDirection(2);
    }
    else if (direction == 0){
        snk.addPart((last_part.getX() - 1) < 1 ? maxX - 2 :(last_part.getX() - 1), last_part.getY());
        snk.setDirection(0);
    }
    else if (direction == 1){
        snk.addPart(last_part.getX() , (last_part.getY() - 1) < 1 ? maxY - 2:(last_part.getY() - 1));
        snk.setDirection(1);
    }
    else if (direction == 3){
        snk.addPart(last_part.getX(), last_part.getY() % (maxY - 1) + 1);
        snk.setDirection(3);
    }

    if (snk.getId() != mainSnakePtr->getId())
        drawSnake(snk);
    refresh();
}

void game::moveMainSnake(snake& snk, int direction){
    snk.getParts().erase(snk.getParts().begin());
    snake_part last_part = snk.getParts().at(snk.getParts().size() - 1);

    if (direction == 2){
        snk.addPart(last_part.getX() % (maxX - 1) + 1, last_part.getY());
        snk.setDirection(2);
    }
    else if (direction == 0){
        snk.addPart((last_part.getX() - 1) < 1 ? maxX - 2 :(last_part.getX() - 1), last_part.getY());
        snk.setDirection(0);
    }
    else if (direction == 1){
        snk.addPart(last_part.getX() , (last_part.getY() - 1) < 1 ? maxY - 2:(last_part.getY() - 1));
        snk.setDirection(1);
    }
    else if (direction == 3){
        snk.addPart(last_part.getX(), last_part.getY() % (maxY - 1) + 1);
        snk.setDirection(3);
    }

    checkSnakeOverlap(snk);

    if (snk.getHeadX() == getFoodX() && snk.getHeadY() == getFoodY() && snk.getId() == mainSnakePtr->getId()){
        snk.addPart(getFoodX(), getFoodY());
        snk.setScore(snk.getScore() + 1);
        sockObj.sendData("#");

        setFoodPos(-10, -10);
    }

    drawSnake(snk);
    refresh();
}

void game::moveAllSnakes(){
    for (int i = 0; i < allSnakes.size(); i++){
        moveSnake(allSnakes[i], allSnakes[i].getDirection());
    }
}
void game::handleMovementKeyPress(snake& snk, const int code){
    if (code == 17 || code == 103){
        if (snk.getDirection() != 3 && snk.getDirection() != 1)
            moveSnake(snk, 1);
    }
    else if (code == 31 || code == 108){
        if (snk.getDirection() != 1 && snk.getDirection() != 3)
            moveSnake(snk, 3);
    }
    else if (code == 30 || code == 106){
        if (snk.getDirection() != 0 && snk.getDirection() != 2)
            moveSnake(snk, 2);
    }
    else if(code == 32 || code == 105){
        if (snk.getDirection() != 2 && snk.getDirection() != 0)
            moveSnake(snk, 0);
    }
    else return;
}

void game::checkSnakeOverlap(snake& snk){
    int headX = snk.getHeadX(), headY = snk.getHeadY();
    for (int i = 0; i < snk.getParts().size() - 1; i++)
        if (snk.getPart(i).getX() == headX && snk.getPart(i).getY() == headY){
            if (socketDescriptor > 0)
                return;
            gameOverHandler(snk);
        }
}

void game::readData(){
    int val;
    FD_ZERO(&sockObj.getSet());
    FD_SET(sockObj.getSock(), &sockObj.getSet());

    val = select(sockObj.getSock() + 1, &sockObj.getSet(), NULL, NULL, &sockObj.getTimeout());

    if(val > 0){
        recv(sockObj.getSock(), sockObj.getBuffer(), 1024, 0);
        handleMessageFromServer(string(sockObj.getBuffer()));
    }

    else if(val == -1){
        perror("select");
    }
}

void game::drawBorderWindow(){
    int max_x = maxX, max_y = maxY;
    for (int i = 0, j = 1; (i < max_x || j < max_y);){
        if ( i < max_x)
            mvprintw(1, i++, "--");
        if (j < max_y)
            mvprintw(j++, 0, "|");
    }

    max_x -= 1; max_y -= 1;

    for (int i = max_x, j = max_y; (i > 0 || j > 0); ){
        if (i > 0)
            mvprintw(max_y, i--, "--");

        if (j > 0)
            mvprintw(j--, max_x, "|");
    }
}