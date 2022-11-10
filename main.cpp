#include <Imagine/Graphics.h>
using namespace Imagine;
#include <iostream>
using namespace std;
#include "board.h"
#include "controller.h"
#include "GUI.h"

int main(){


    Board board;
    Controller** players = new Controller*[NB_PLAYERS];
    players[0] = new Random();
    players[1] = new Random();

    play(&board,players);
    return 0;

}

