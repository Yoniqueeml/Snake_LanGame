#include<bits/stdc++.h>
#include<iostream>
#include<unistd.h>
#include<string>
#include<ncurses.h>
#include<cstdlib>
#include "./server/server.h"
#include<fstream>
#include<regex>
#include <ncurses.h>

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

using namespace std ; 

class snake;
class Game;

#include "server/game.h"

//global variables for the program 
int max_x  = 0 , max_y = 0  ;  //Make max_x and max_y as global since the values are used by many methods

std::ostringstream ss;



game GameObj(1) ;












void draw_border_window(int, int) ;
void draw_food(void) ; 

void draw_border_window( int max_x , int max_y)
{
    for(int i =0 , j = 1 ; (i<max_x || j<max_y ) ; )
    {
        if(i<max_x)
            mvprintw(1,i++,"--") ;

        if(j<max_y)
            mvprintw(j++ , 0 , "|") ; 
    }

    max_x -=1 ; max_y-=1 ; 

    for(int i =max_x , j = max_y ; (i>0 || j>0) ; )
    {
        if(i>0)
            mvprintw( max_y , i--  , "--") ; 

        if(j>0)
            mvprintw(j--,max_x  , "|") ; 
        
    }
}


/*
void ask_no_players(string player="single"){
    char ch ; 

    for(;;)
    {
    //Needed only this branch
    if(GameObj.getGameMode()=="multi"){
        flushinp() ;
        GameObj.initConsoleScreen("off") ;
        system("clear")  ;
        int no_of_players ;
        cout<<"Enter the number of players on this device (any number of LAN clients can join this play ) : "  ;
        cin>> no_of_players;
        GameObj.setNoOfPlayers(no_of_players) ;
    }


    //Add the first snake object
    if(GameObj.getNoOfPlayers()){
        snake first_snake  = snake('A' , 'B' , 'C' , 'D' , 0 ) ;
        first_snake.setPlayerName("Default" ) ;
        GameObj.allSnakes.push_back(first_snake) ;
    }

    //Turn on the coordinate kind of behaviour for the terminal
    GameObj.initConsoleScreen("on") ; 
}
*/

//Right now this is the MAIN KEYS HANDLING FUNCTION Which will handle all the keystrokes entered
//not needed
/*
void Game::KeyPressHandler()
{
    char ch ; 
    if((ch = getch())!=ERR)
    {
        if(ch==27)
        {
            getch() ; // clear and reject 91 from buffer
            ch = getch() ;//Now store the actual value of arrow key pressed               getch() ; 
            if(GameObj.getNoOfPlayers())
                allSnakes[0].handleMovementKeyPress(ch) ;
        }

        if (GameObj.getGameMode()=="multi"){
            for(int i =0 ; i<allSnakes.size() ; i++)
            {
                //only move if the key press occurs for offline snakes
                if(allSnakes[i].getSocketDescriptor()<0 && allSnakes[i].getID()<0)
                    allSnakes[i].handleMovementKeyPress(ch) ; 
            }
        }


        //Decrease the snake speed
        if(ch==45)
        {
            GameObj.setSpeed(GameObj.getSpeed()+3000) ;
            if(GameObj.getGameMode()=="multi")
            {
                for(int temp =0 ; temp<allSnakes.size() ; temp++)
                {
                    int sd = allSnakes[temp].getSocketDescriptor() ; 
                    if(sd>0)
                    {
                        server.sendData(sd , "-") ; 
                    }
                }
            }
        }

    }
}
*/

//THIS IS A VERY IMPORTANT FUNCTION WHICH SHOULD HAVE BEEN INCLUDED IN THE MAIN FUNCTION , BUT FOR CONVENIENCE , ITS INCLUDED HERE . 
//HANDLES EVERYTHING UNTILL THE MAIN EVENT LOOP STARTS IN THE main function . 
void HANDLE_EVERYTHING_TILL_EVENT_LOOP(){
    srand(time(NULL)) ;
    GameObj.initConsoleScreen("on") ; 

    init_pair(RED , COLOR_RED , COLOR_BLACK) ; 
    init_pair(YELLOW , COLOR_YELLOW , COLOR_BLACK) ; 
    init_pair(GREEN , COLOR_GREEN , COLOR_BLACK) ;
    init_pair(WHITE , COLOR_WHITE , COLOR_BLACK) ; 
    init_pair(MAGENTA, COLOR_MAGENTA, COLOR_BLACK) ;
    init_pair(BLUE, COLOR_BLUE, COLOR_BLACK) ;
    init_pair(CYAN , COLOR_CYAN , COLOR_BLACK) ; 

    //Asks the number of players who want to play  (single or multiplayer (with 2 players )) ; 
    GameObj.ask_no_players();

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
            GameObj.server.stopServer() ;
            GameObj.printAnimated("\nBye") ;
            exit(1) ;
            break ; 
        case 1:
            GameObj.initConsoleScreen("on") ; 
            break ; 
        default:
            GameObj.server.stopServer() ; 
            exit(1) ;
    }
}

int main(int argc , char * argv[]) 
{
    GameObj.showInitialChoices() ;

    signal(SIGINT, signalHandler);
    HANDLE_EVERYTHING_TILL_EVENT_LOOP() ; 
    int activity ; 

    if(GameObj.getGameMode()=="multi")
    {
        GameObj.initServerForMultiplayer() ; 
    }

    for(;;)
    {
        GameObj.reset_max_screen() ; 
        GameObj.KeyPressHandler() ; //Handles key presses 

        if(GameObj.getGameMode()=="multi")
        {
            activity = GameObj.checkClientActivity() ;

            if(activity>=1)
            {
                GameObj.handleActivity() ; 
            }
            
        }

        flushinp();
        clear() ;

        GameObj.moveAllSnakes() ; 
        GameObj.printScores() ;

        GameObj.printFood("old");
        refresh() ;
        usleep(GameObj.getSpeed()) ;
    }
 
}
