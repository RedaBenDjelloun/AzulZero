#pragma once

#include <iostream>
using namespace std;
#include <Imagine/Graphics.h>
#include <Imagine/Images.h>
using namespace Imagine;

#include "board.h"

// GUI Parameters

const double zoom = 0.8;
const int PLAYERBOARD_WIDTH = int(740*zoom);
const int PLAYERBOARD_HEIGHT = int(484*zoom);
const int FACTORY_SIDE = int(250*zoom);
const int WINDOW_HEIGHT = 2*PLAYERBOARD_HEIGHT; // Recommended : 968 px
const int WINDOW_WIDTH = PLAYERBOARD_WIDTH+WINDOW_HEIGHT; // Recommended : 1480 px

// Images

const string GAMEBOX = stringSrcPath("Images/game_box.png");
const string BAG = stringSrcPath("Images/bag.png");
const string PLAYERBOARD = stringSrcPath("Images/playerboard.png");
const string FACTORY = stringSrcPath("Images/factory.png");
const string BLUE_TILE = stringSrcPath("Images/blue.svg");
const string YELLOW_TILE = stringSrcPath("Images/yellow.svg");
const string RED_TILE = stringSrcPath("Images/red.svg");
const string BLACK_TILE = stringSrcPath("Images/black.svg");
const string CYAN_TILE = stringSrcPath("Images/cyan.svg");
const string FIRST_PLAYER_TILE = stringSrcPath("Images/first-player.svg");

const vector<string> IMAGE_PATHS = {
    GAMEBOX,
    BAG,
    PLAYERBOARD,
    FACTORY,
    BLUE_TILE,
    YELLOW_TILE,
    RED_TILE,
    BLACK_TILE,
    CYAN_TILE,
    FIRST_PLAYER_TILE
};

class GUI{
    /// Window size
    int window_width, window_height;
    /// Images for GUI
    Image<AlphaColor>* images;
public:
    /// Constructor
    GUI(vector<string> image_paths = IMAGE_PATHS, int w = WINDOW_WIDTH, int h = WINDOW_HEIGHT);
    /// Destructor
    ~GUI();

    /// Load with error reporting
    void safeLoad(Image<AlphaColor> &image, string path);
    /// Load all images
    void loadImages(vector<string> image_paths);

    /// Display image i
    void displayImage(int i, int x = 0, int y = 0, double fact = zoom);
    /// Shortcut : Display gamebox image
    void displayGamebox(int x = 0, int y = 0, double fact = zoom) { displayImage(0,x,y,fact); }
    /// Shortcut : Display bag image
    void displayBag(int x = 0, int y = 0, double fact = zoom) { displayImage(1,x,y,fact); }
    /// Shortcut : Display playerboard image
    void displayPlayerboard(int x = 0, int y = 0, double fact = zoom) { displayImage(2,x,y,fact); }
    /// Shortcut : Display factory image
    void displayFactory(int x = 0, int y = 0, double fact = zoom) { displayImage(3,x,y,fact); }
    /// Shortcut : Display tile of color j (0: blue, 1: yellow, 2: red, 3: black, 4: cyan, 5: first player)
    void displayTile(int j, int x = 0, int y = 0, double fact = zoom) { displayImage(4+j,x,y,fact); }

    /// Display image i
    void displayImage(int i, IntPoint2 P, double fact = zoom){ displayImage(i,P.x(),P.y(),fact); }
    /// Shortcut : Display gamebox image
    void displayGamebox(IntPoint2 P, double fact = zoom) { displayGamebox(P.x(),P.y(),fact); }
    /// Shortcut : Display bag image
    void displayBag(IntPoint2 P, double fact = zoom) { displayBag(P.x(),P.y(),fact); }
    /// Shortcut : Display playerboard image
    void displayPlayerboard(IntPoint2 P, double fact = zoom) { displayPlayerboard(P.x(),P.y(),fact); }
    /// Shortcut : Display factory image
    void displayFactory(IntPoint2 P, double fact = zoom) { displayFactory(P.x(),P.y(),fact); }
    /// Shortcut : Display tile of color j (0: blue, 1: yellow, 2: red, 3: black, 4: cyan, 5: first player)
    void displayTile(int j, IntPoint2 P, double fact = zoom) { displayTile(j,P.x(),P.y(),fact); }

    /// Initialize GUI
    Window init();

    /// Display NB_PLAYERS = 2 playerboards
    void displayAllPlayerboards();
    /// Display NB_FACTORIES factories in a circle
    void displayFactoryCircle(double scale = 1);

};




