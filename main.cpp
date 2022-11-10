#include <Imagine/Graphics.h>
using namespace Imagine;
#include <iostream>
using namespace std;
#include "board.h"
#include "GUI.h"



int main(){
    int nb_player = 2;
    Board board(nb_player);

    GUI gui;
    gui.init();
    gui.displayPlayerboard(WINDOW_WIDTH/2);
    gui.displayPlayerboard(WINDOW_WIDTH/2,WINDOW_HEIGHT/2);
    gui.displayBag();

    int x,y;
    getMouse(x,y);
    gui.displayTile(0,x,y);

    endGraphics();
    return 0;

}

