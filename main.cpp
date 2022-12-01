#include <Imagine/Graphics.h>
using namespace Imagine;
#include <iostream>
using namespace std;
#include <chrono>
using namespace std::chrono;
#include "board.h"
#include "controller.h"
#include "GUI.h"

int main(){
    srand((unsigned int) time(0));

    Controller** players = new Controller*[NB_PLAYERS];
    Heuristic heuristic_player(0);
    MinMax minmax_player(20);
    Random random_player;
    Human player("Hector");
    
    
    GUI gui;
    gui.init();
    
    // Choose players
    players[0] = &minmax_player;
    players[1] = &heuristic_player;

    Board board;
    board.init();

    playGameGraphics(&board, players, gui);
    cout<<"score joueur 1: "<<int(board.getScore(0))<<endl;
    cout<<"score joueur 2: "<<int(board.getScore(1))<<endl;


    endGraphics();
    return 0;

}

