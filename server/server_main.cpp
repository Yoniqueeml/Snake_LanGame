#include<iostream>
#include<unistd.h>
#include<string>
#include<ncurses.h>
#include<cstdlib>
#include<regex>
using namespace std ;

#include "game.h"

game GameObj(1) ;

void HANDLE_EVERYTHING_TILL_EVENT_LOOP(){
    srand(time(NULL)) ;
    GameObj.initConsoleScreen("on") ;
    GameObj.initColors();

    flushinp();
    GameObj.initConsoleScreen("off") ;
    system("clear")  ;
    GameObj.setNoOfPlayers(0);

    GameObj.draw_all_snakes() ; 
}


void signalHandler(int code){
    GameObj.initConsoleScreen("off") ; 
    cout.flush() ;
    stringstream out ; 
    out<<"1.Continue" <<endl; 
    out<<"2.Exit" <<endl;
    out<<"\nEnter choice : " <<endl; 
    cout<<out.str() ; 
    int ch ; 
    cin>>ch ; 
    string credits="" ; 

    switch(ch){
        case 2:
            GameObj.getServer().stopServer();
            GameObj.printAnimated("\nBye", 6000);
            exit(1);
            break ; 
        case 1:
            GameObj.initConsoleScreen("on");
            break ; 
        default:
            GameObj.getServer().stopServer();
            exit(1) ;
    }
}

int main(int argc , char * argv[]){
    GameObj.showInitialChoices();

    signal(SIGINT, signalHandler);
    HANDLE_EVERYTHING_TILL_EVENT_LOOP();
    int activity;

    GameObj.initServerForMultiplayer();

    for(;;){
        GameObj.reset_max_screen();
        activity = GameObj.checkClientActivity();

        if (activity >= 1){
            GameObj.handleActivity() ;
        }


        flushinp();
        clear();

        GameObj.moveAllSnakes();
        GameObj.printScores();

        GameObj.printFood("old");
        refresh() ;
        usleep(70000);
    }
}