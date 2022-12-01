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

    int total_points[NB_PLAYERS] = {0,0};
    int max_points[NB_PLAYERS] = {0,0};
    int nb_iterations = 1;
    int winner[3] = {0,0,0};
    Controller** players = new Controller*[NB_PLAYERS];
    Heuristic heuristic_player(0);
    MinMax minmax_player(7,false);
    Random random_player;
    Human player("Hector");
    
    
    GUI gui;
    gui.init();
    
    // Choose players
    players[0] = &minmax_player;
    players[1] = &heuristic_player;

    for(int i=0; i<nb_iterations; i++){

        Board board;
        board.init();

    playGameGraphics(&board, players, gui);
    cout<<"score joueur 1: "<<total_points[0]<<endl;
    cout<<"score joueur 2: "<<total_points[1]<<endl;


    endGraphics();
    return 0;

}

