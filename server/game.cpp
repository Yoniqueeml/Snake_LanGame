#include "game.h"

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

    LAN_sendFoodCoordinates( getFoodX(), getFoodY());
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
    return center_x;
}

int game::getCenterY(){
    return center_y;
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

void game::init_snake_on_screen(snake& snk){
    snk.add_part(getCenterX(), getCenterY());
    snk.add_part(getCenterX() + 1, getCenterY());
    snk.add_part(getCenterX() + 2, getCenterY());

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
        printFood("new");
    }
    draw_snake(snk);
    refresh();
}

void game::moveAllSnakes(){
    for (int i = 0; i < allSnakes.size(); i++){
        move_snake(allSnakes[i], allSnakes[i].getDirection());
    }
}

void game::handleMovementKeyPress(snake& snk, const int code){
    if(code == 17 || code == 103){
        if (snk.getDirection() != 3 && snk.getDirection() != 1)
            move_snake(snk, 1);
    }
    else if(code == 31 || code == 108){
        if (snk.getDirection() != 1 && snk.getDirection() != 3)
            move_snake(snk, 3);
    }
    else if(code == 30 || code == 106){
        if (snk.getDirection() != 0 && snk.getDirection() != 2)
            move_snake(snk, 2);
    }
    else if(code == 32 || code == 105){
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
            server.sendData(socket_descriptor , "$" + std::to_string(snk.getScore()) + "$");
            gameOverHandler(snk);
        }
}

int game::getSnakeIndexFromID(int id){
    for(int i = 0; i < allSnakes.size(); i++){
        if (allSnakes[i].getId() == id)
            return i;
    }
    return -1;
}

void game::printScores(){
    int s  = 0;
    string temp;
    coloron(WHITE);
    for(int i = 0; i < allSnakes.size(); i++){
        temp = allSnakes[i].getName();
        mvprintw(0 , s, "%s:%d", temp.c_str(), allSnakes[i].getScore());
        s += temp.length() + 4;
    }
    coloroff(WHITE);
}

void game::gameOverHandler(const snake& snk){
    clear();
    initConsoleScreen("off");
    system("clear");
    string gameovermessage = "\n\n\nGAME OVER FOR " + snk.getName() + " :(\n\n";
    gameovermessage+="Score : " + std::to_string(snk.getScore()) + "\nBetter Luck Next time :)\n\n";

    printAnimated(gameovermessage);

    if (socket_descriptor > 0){
        allSnakes.erase(allSnakes.begin() + getSnakeIndexFromDescriptor(socket_descriptor));
        server.closeSocket(socket_descriptor);
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

void game::draw_snake(snake& snk){
    int i;
    coloron(snk.getBodyColor());
    for(i =0; i < snk.getParts().size() - 1; i++){
        mvprintw(snk.getPart(i).getY(), snk.getPart(i).getX(), "o");
    }
    coloroff(snk.getBodyColor());
    mvprintw(snk.getPart(i).getY(), snk.getPart(i).getX(), "+");
}

void game::draw_all_snakes(){
    for(int i =0; i<allSnakes.size(); i++){
        allSnakes[i].add_part(center_x + 5 , center_y);
        allSnakes[i].add_part(center_x + 6 , center_y);
        allSnakes[i].add_part(center_x + 7 , center_y);
        draw_snake(allSnakes[i]);
    }
}

void game::LAN_sendFoodCoordinates(int x, int y){
    for (int temp= 0; temp < allSnakes.size(); temp++){
        int sd  = allSnakes[temp].getSocketDescriptor();

        if (sd > 0){
            char foodcoord[20];
            sprintf(foodcoord, ":%03d,%03d", x, y);
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
    snake * clientSnakePtr = new snake(0, client_socket);
    allSnakes.push_back(*clientSnakePtr);
    setNoOfPlayers(getNoOfPlayers() + 1);

    allSnakes[allSnakes.size() - 1].add_part(center_x + 5, center_y);
    allSnakes[allSnakes.size() - 1].add_part(center_x + 6, center_y);
    allSnakes[allSnakes.size() - 1].add_part(center_x + 7, center_y);
    generateFood();
}

void game::syncSnakeWithClient(snake& clientsnake){
    int x_diff = getFoodX() - clientsnake.getHeadX();
    int y_diff = getFoodY() - clientsnake.getHeadY();

    for(int i = 0 ; i < clientsnake.getParts().size(); i++){
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
            LAN_sendFoodCoordinates(foodObj.getX() , foodObj.getY()) ;
            string name="", sight="";
            int pos = msg.find("init~~");
            int i;

            for (i = 6; msg[i] != '~'; i++){
                name+=msg[i];
            }

            sight = msg[i+2];

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
                    allSnakes[snake_index].add_part(allSnakes[snake_index].getHeadX(),
                                                    allSnakes[snake_index].getHeadY());
                    allSnakes[snake_index].setScore(allSnakes[snake_index].getScore() + 1);

                    syncSnakeWithClient(allSnakes[snake_index]);

                    printFood("new");
                    LAN_sendFoodCoordinates(getFoodX(), getFoodY());
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

void game::handleActivity(){
    clients = server.handleActivity();

    if (clients[0] == -1){
        handleNewConnection();
    }
    else handleIOActivity();
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