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

    // Choose players
    players[0] = &minmax_player;
    players[1] = &heuristic_player;

    Board board;
    board.init();

    GUI gui;
    gui.init();
    playGameGraphics(&board,players,gui);

    endGraphics();

    return 0;

}

