#include "game.h"
#include <fstream>

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
    int x = random() % maxX, y = random() % maxY;
    if (!x) x = 2;
    if (!y) y = 2;
    coloron(RED);
    mvprintw(y, x, "@");
    coloroff(RED);
    setFoodPos(x, y);

    LANSendFoodCoordinates( getFoodX(), getFoodY());
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

void game::setClientsList(vector<int>& clientslist){
    clients = clientslist;
}

int game::getNoOfPlayers(){
    return noOfPlayers;
}

void game::resetMaxScreen(){
    int curX = 0, curY = 0;
    getmaxyx(stdscr, curY, curX);
    if (curX < maxX || curY < maxY){
        initConsoleScreen("off");
        cout << "\nConsole size is too small!!! Needed:" << maxX << "x" << maxY << endl << endl;
        sleep(1);
        exit(3);
    }
}

void game::setNoOfPlayers(int n){
    noOfPlayers  = n;
}

void game::initConsoleScreen(string state){
    if (state == "on"){
        initscr(); //Init screen
        noecho(); // Dont show any pressed char
        curs_set(false); // Don't show the cursor
        start_color();
        int curX = 0, curY = 0;
        getmaxyx(stdscr, curY, curX);
        if (curX < maxX || curY < maxY){
            initConsoleScreen("off");
            cout << "\nConsole size is too small!!! Needed:" << maxX << "x" << maxY << endl << endl;
            sleep(1);
            exit(3);
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

    checkSnakeOverlap(snk);

    //if (snk.getHeadX() == getFoodX() && snk.getHeadY() == getFoodY()){
    //    snk.addPart(getFoodX(), getFoodY());
    //    snk.setScore(snk.getScore() + 1);
    //    printFood("new");
    //}

    drawSnake(snk);
    refresh();
}

void game::moveAllSnakes(){
    for (int i = 0; i < allSnakes.size(); i++){
        moveSnake(allSnakes[i], allSnakes[i].getDirection());
    }
}

void game::handleMovementKeyPress(snake& snk, const int code){
    if(code == 17 || code == 103){
        if (snk.getDirection() != 3 && snk.getDirection() != 1)
            moveSnake(snk, 1);
    }
    else if(code == 31 || code == 108){
        if (snk.getDirection() != 1 && snk.getDirection() != 3)
            moveSnake(snk, 3);
    }
    else if(code == 30 || code == 106){
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
    int headX = snk.getHeadX(), headY  = snk.getHeadY();
    for (int i = 0; i < snk.getParts().size() - 1; i++)
        if (snk.getPart(i).getX() == headX && snk.getPart(i).getY() == headY){
            if (snk.getSocketDescriptor() > 0)
                return;
            server.sendData(snk.getSocketDescriptor(), "$" + std::to_string(snk.getScore()) + "$");
            gameOverHandler(snk);
        }
}

int game::getSnakeIndexFromID(int id){
    for (int i = 0; i < allSnakes.size(); i++){
        if (allSnakes[i].getId() == id)
            return i;
    }
    return -1;
}

void game::printScores(){
    int s  = 0;
    string temp;
    coloron(WHITE);
    for (int i = 0; i < allSnakes.size(); i++){
        temp = allSnakes[i].getName();
        mvprintw(0 , s, "%s:%d:%d", temp.c_str(), allSnakes[i].getScore(), allSnakes[i].getId());
        s += temp.length() + 4;
    }
    coloroff(WHITE);
}

void game::gameOverHandler(const snake& snk){
    clear();
    initConsoleScreen("off");
    system("clear");
    string gameovermessage = "\n\n\nGAME OVER FOR " + snk.getName() + " :(\n\n";
    gameovermessage += "Score : " + std::to_string(snk.getScore()) + "\nBetter Luck Next time :)\n\n";

    printAnimated(gameovermessage);

    if (snk.getSocketDescriptor() > 0){
        allSnakes.erase(allSnakes.begin() + getSnakeIndexFromDescriptor(snk.getSocketDescriptor()));
        server.closeSocket(snk.getSocketDescriptor());
    }
    else
        allSnakes.erase(allSnakes.begin() + getSnakeIndexFromID(snk.getId()));

    setNoOfPlayers(getNoOfPlayers() - 1);

    if (getNoOfPlayers() == 0){
        initConsoleScreen("off");
        cout << "\nNo Snakes Left to Play :(  Exiting Game ..." << endl << endl;
        sleep(1);
        exit(3);
    }

    printAnimated("\n\nGame will continue in few seconds.");
    sleep(1);
}

void game::drawSnake(snake& snk){
    int i;
    coloron(snk.getBodyColor());
    for(i =0; i < snk.getParts().size() - 1; i++){
        mvprintw(snk.getPart(i).getY(), snk.getPart(i).getX(), "o");
    }
    coloroff(snk.getBodyColor());
    mvprintw(snk.getPart(i).getY(), snk.getPart(i).getX(), "+");
}

void game::drawAllSnakes(){
    for (int i = 0; i < allSnakes.size(); i++){
        allSnakes[i].addPart(centerX + 5 , centerY);
        allSnakes[i].addPart(centerX + 6 , centerY);
        allSnakes[i].addPart(centerX + 7 , centerY);
        drawSnake(allSnakes[i]);
    }
}

void game::LANSendFoodCoordinates(int x, int y){
    for (int temp= 0; temp < allSnakes.size(); temp++){
        int sd  = allSnakes[temp].getSocketDescriptor();

        if (sd > 0){
            char foodcoord[20];
            sprintf(foodcoord, "!%03d,%03d", x, y);
            server.sendData(sd, string(foodcoord));
        }
    }
}

void game::initServerForMultiplayer(){
    server.bindServer();
    server.setupClientDescriptors();
    server.startServer();
}

void game::handleNewConnection(){
    int client_socket = server.handleNewConnection();
    snake* clientSnakePtr = new snake(prevClientId++, client_socket);
    allSnakes.push_back(*clientSnakePtr);
    setNoOfPlayers(getNoOfPlayers() + 1);

    allSnakes[allSnakes.size() - 1].addPart(centerX + 5, centerY);
    allSnakes[allSnakes.size() - 1].addPart(centerX + 6, centerY);
    allSnakes[allSnakes.size() - 1].addPart(centerX + 7, centerY);
    generateFood();
    string msg = "+";
    msg += to_string(clientSnakePtr -> getId());
    server.sendData(client_socket, msg);
}

void game::syncSnakeWithClient(snake& clientsnake){
    int x_diff = getFoodX() - clientsnake.getHeadX();
    int y_diff = getFoodY() - clientsnake.getHeadY();

    for (int i = 0; i < clientsnake.getParts().size(); i++){
        clientsnake.getPart(i).setX(clientsnake.getPart(i).getX() + x_diff);
        clientsnake.getPart(i).setY(clientsnake.getPart(i).getY() + y_diff);
    }
}

void game::handleIOActivity(){
    string msg ;
    int snake_index ;

    for (int i = 0 ; i < clients.size(); i++){
        snake_index = getSnakeIndexFromDescriptor(clients[i]);
        msg = server.handleIOActivity(clients[i]);

        if (msg == ""){
            initConsoleScreen("off");
            system("clear");
            cout << allSnakes[snake_index].getName() << " left the game.\n";
            cout << "Score : " << allSnakes[snake_index].getScore() << "\n\n";
            server.closeSocket(clients[i]);
            allSnakes.erase(allSnakes.begin() + snake_index);
            setNoOfPlayers(getNoOfPlayers() - 1);
            sleep(1);
            initConsoleScreen("on");
        }


        else if(msg.find("init~~") != string::npos){
            LANSendFoodCoordinates(foodObj.getX(), foodObj.getY()) ;
            string name="", sight="";
            int i;

            for (i = 6; msg[i] != '~'; i++){
                name += msg[i];
            }

            initConsoleScreen("off");
            system("clear");
            cout << "\n\n" << name << " joined the game.\n\n";
            sleep(1);
            system("clear");
            initConsoleScreen("on");
            allSnakes[snake_index].setName(name);
            allSnakes[snake_index].setBodyColor(int(msg[msg.find("&") + 1] - '0'));
        }


        else if(msg.find("$")!=string::npos){
            int start = msg.find("$");
            gameOverHandler(allSnakes[snake_index]);
        }

        else{
            for(int c = 0; c < msg.length(); c++){
                if (msg[c]=='#') {
                    allSnakes[snake_index].addPart(allSnakes[snake_index].getHeadX(),
                                                    allSnakes[snake_index].getHeadY());
                    allSnakes[snake_index].setScore(allSnakes[snake_index].getScore() + 1);

                    syncSnakeWithClient(allSnakes[snake_index]);

                    printFood("new");
                    LANSendFoodCoordinates(getFoodX(), getFoodY());
                }
                else if(msg[c] == 'A' or msg[c] == 'B' or msg[c] == 'C' or msg[c] == 'D') {
                    int code = 0;
                    if (msg[c]=='A') code = 17;
                    if (msg[c]=='B') code = 31;
                    if (msg[c]=='C') code = 30;
                    if (msg[c]=='D') code = 32;
                    handleMovementKeyPress(allSnakes[snake_index], code);
                }
            }
        }
    }
}

int game::getSnakeIndexFromDescriptor(int sd){
    for (int temp=0; temp < allSnakes.size(); temp++){
        if (sd == allSnakes[temp].getSocketDescriptor()){
            return temp;
        }
    }
}
std::string serializeSnakes(std::vector<snake> Snakes){
    std::string data;
    for (auto const s : Snakes){
        data.push_back('(');
        data += char('0' + s.getId());
        data += char('0' + s.getDirection());
        data += char('0' + s.getBodyColor());
        data += char('0' + s.getParts().size());
        for (const auto& part : s.getParts()){
            data += char('0' + part.getX());
            data += char('0' + part.getY());
        }
    }
    data.push_back(')');
    return data;
}

void game::handleActivity(){
    clients = server.handleActivity();

    if (clients[0] == -1){
        handleNewConnection();
    }
    else handleIOActivity();
    std::string serializedSnakes = serializeSnakes(allSnakes);
    for (int temp= 0; temp < allSnakes.size(); temp++){
        int sd  = allSnakes[temp].getSocketDescriptor();
        if (sd > 0){
            server.sendData(sd, serializedSnakes);
        }
    }
}

int game::checkClientActivity(){
    return server.checkClientActiviy();
}

void game::showInitialChoices(){
    system("clear");
    cout << "\n1. Start Game *";
    cout << "\n2. Exit";
    cout << "\n\nEnter choice : ";
    int ch;
    cin >> ch;

    switch(ch){
        case 1:
            return;
        case 2:
            exit(5);
        default:
            return;
    }
}

socketHandler& game::getServer(){
    return server;
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
