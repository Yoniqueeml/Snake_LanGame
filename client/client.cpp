#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <ncurses.h>
#include<fstream>
#include<iostream>
#include<bits/stdc++.h>
#include<iostream>
#include<unistd.h>
#include<ncurses.h>
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <sys/fcntl.h>

#include "game.h"

using namespace std ;

//global variables for the program
int max_x  = 0, max_y = 0;  //Make max_x and max_y as global since the values are used by many methods


game GameObj(1) ;

void draw_border_window(int  , int) ;
void draw_food(void) ;


void draw_border_window( int max_x , int max_y){
    for(int i =0 , j = 1 ; (i<max_x || j<max_y ) ; ){
        if(i<max_x)
            mvprintw(1,i++,"--") ;

        if(j<max_y)
            mvprintw(j++ , 0 , "|") ;
    }

    max_x -=1 ; max_y-=1 ;

    for(int i =max_x , j = max_y ; (i>0 || j>0) ; ){
        if(i>0)
            mvprintw( max_y , i--  , "--") ;

        if(j>0)
            mvprintw(j--,max_x  , "|") ;

    }
}

void signalHandler(int code){
    GameObj.initConsoleScreen("off");
    GameObj.printAnimated("\nBye!", 6000);
    GameObj.getSockObj().closeSocket();
    exit(1) ;
}

struct input_event {
    struct timeval time;
    unsigned short type;
    unsigned short code;
    unsigned int value;
};

int main(){
    int fd = open("/dev/input/event0", O_RDONLY | O_NONBLOCK);
    if (fd < 0){
        std::cout << "Error keyboard logging" << std::endl;
        throw "Error keyboard logging";
    }

    signal(SIGINT , signalHandler) ;
    srand(time(NULL)) ;
    system("clear") ;
    string serverAddress  , player_name;
    cout<<"Enter your name : " ;

    getline(cin , player_name) ;


    cout<<"Enter the IP address of the Controlling Server : "  ;
    cin>>serverAddress ;

    //Set some socket options
    int connection = GameObj.getSockObj().connectToServer(serverAddress , 8888);
    if(connection == 2){
        close(fd);
        cout<<"Failed to connect to the server" << endl;
        GameObj.getSockObj().closeSocket();
        exit(1);
    }
    //Initialize the snake object
    snake first_snake(0, player_name);
    GameObj.setMainSnakePtr(&first_snake);

    GameObj.getSockObj().sendData("init~~" + player_name+"~~"+"c"+"~~"+"&"+to_string(first_snake.getBodyColor())+"&") ;

    GameObj.initConsoleScreen("on");
    GameObj.initColors();

    GameObj.init_snake_on_screen(first_snake);

    char ch;

    input_event buff[5];
    timeval prevEventTime;

    for(;;){
        GameObj.reset_max_screen();
        int r = read(fd, &buff, sizeof(input_event)*5);
        if (r > 0) {
            for (int i = 0; i < 5; ++i) {
                if (buff[i].type == 1 && buff[i].value == 1) {
                    GameObj.getSockObj().sendData(string(1, buff[i].value));
                    GameObj.handleMovementKeyPress(first_snake, buff[i].code);
                }
            }
        }
        memset(buff, 0, sizeof(input_event) * 5);
        flushinp();
        clear() ;
        GameObj.move_snake(first_snake, first_snake.getDirection()) ;
        GameObj.printFood("old") ;
        GameObj.printScore(first_snake);


        refresh() ;
        GameObj.readData() ;

        usleep(70000) ;
    }
}
