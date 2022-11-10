#include <Imagine/Graphics.h>
using namespace Imagine;
#include <iostream>
using namespace std;
#include "board.h"
#include "controller.h"
#include "GUI.h"

int main(){


    Board board;
    Random* players = new Random[NB_PLAYERS];
    players[0] = Random(&board);
    players[1] = Random(&board);

    play(&board,players);
    return 0;

}

