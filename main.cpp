#include <Imagine/Graphics.h>
using namespace Imagine;
#include <iostream>
using namespace std;
#include "board.h"
#include "GUI.h"



int main(){
    Board board;

    GUI gui;
    gui.init();

    gui.displayAllPlayerboards();
    gui.displayBag();
    gui.displayFactoryCircle();

//    int x,y;
//    getMouse(x,y);
//    gui.displayTile(0,x,y);

    endGraphics();
    return 0;

}

