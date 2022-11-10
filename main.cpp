#include <Imagine/Graphics.h>
using namespace Imagine;
#include <iostream>
using namespace std;
#include "board.h"
#include "controller.h"
#include "GUI.h"

int main(){

    srand((unsigned int) time(0));

    Board board;
    Controller** players = new Controller*[NB_PLAYERS];
    players[0] = new Random();
    players[1] = new Heuristic();

    play(&board,players);

    cout<<"score joueur 1: "<<int(board.scores[0])<<endl;
    cout<<"score joueur 2: "<<int(board.scores[1])<<endl;
    return 0;

}

