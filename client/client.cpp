#include <string.h>
#include <stdlib.h>
#include <ncurses.h>
#include<iostream>
#include<bits/stdc++.h>
#include<unistd.h>
#include <sys/time.h>
#include <sys/fcntl.h>
#include <thread>
#include "game.h"

using namespace std;

game GameObj(1);

void HANDLE_EVERYTHING_TILL_EVENT_LOOP(){
    srand(time(NULL));
    GameObj.initConsoleScreen("on");
    GameObj.initColors();

    flushinp();
    GameObj.initConsoleScreen("off");
    system("clear");
    GameObj.setNoOfPlayers(1);

    GameObj.drawAllSnakes();
}

void signalHandler(int code){
    GameObj.initConsoleScreen("off");
    GameObj.printAnimated("\nBye!", 6000);
    GameObj.getSockObj().closeSocket();
    exit(1);
}

struct inputEvent {
    struct timeval time;
    unsigned short type;
    unsigned short code;
    unsigned int value;
};

void drawUI(){
    while (true){
        if(GameObj.readData() == 0){
            continue;
        }
        flushinp();
        clear();
        GameObj.drawBorderWindow();
        GameObj.moveAllSnakes();
        GameObj.printFood("old");
        GameObj.drawAllSnakes();
    }
}
void getServerData(int fd){
    inputEvent buff[5];
    while(true){
        GameObj.resetMaxScreen();
        int r = read(fd, &buff, sizeof(inputEvent) * 5);
        if (r > 0) {
            for (int i = 0; i < 5; ++i) {
                if (buff[i].type == 1 && buff[i].value == 1) {
                    if (buff[i].code == 17 || buff[i].code == 103)
                        GameObj.getSockObj().sendData(string(1, 'A'));
                    if (buff[i].code == 31 || buff[i].code == 108)
                        GameObj.getSockObj().sendData(string(1, 'B'));
                    if (buff[i].code == 30 || buff[i].code == 106)
                        GameObj.getSockObj().sendData(string(1, 'C'));
                    if (buff[i].code == 32 || buff[i].code == 105)
                        GameObj.getSockObj().sendData(string(1, 'D'));
                }
            }
        }
        memset(buff, 0, sizeof(inputEvent) * 5);
        refresh();
        usleep(70000);
    }
}

int main(){
    int fd = open("/dev/input/event2", O_RDONLY | O_NONBLOCK);
    if (fd < 0){
        std::cout << "Error keyboard logging" << std::endl;
        throw "Error keyboard logging";
    }

    signal(SIGINT, signalHandler);
    srand(time(NULL));
    system("clear");
    string serverAddress, player_name;
    cout << "Enter your name : ";

    getline(cin , player_name);


    cout << "Enter the IP address of the Controlling Server : ";
    cin >> serverAddress;

    int connection = GameObj.getSockObj().connectToServer(serverAddress , 8888);
    if(connection == 2){
        close(fd);
        cout << "Failed to connect to the server" << endl;
        GameObj.getSockObj().closeSocket();
        exit(1);
    }
    snake first_snake(0, player_name);
    GameObj.setMainSnakePtr(&first_snake);
    GameObj.getSockObj().sendData("init~~" + player_name+"~~"+"c"+"~~"+"&"+to_string(first_snake.getBodyColor())+"&");
    GameObj.initConsoleScreen("on");
    GameObj.initColors();
    GameObj.initSnakeOnScreen(first_snake);

    std::thread gameThread(getServerData,fd);
    std::thread uiThread(drawUI);
    gameThread.join();
    uiThread.join();
}