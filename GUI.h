#pragma once

#include <iostream>
using namespace std;
#include <Imagine/Graphics.h>
#include <Imagine/Images.h>
using namespace Imagine;

#include "board.h"

// GUI Parameters

const double ZOOM = 1.;

const int HIGHLIGHT_PEN = 4;
const int HIGHLIGHT_GAP = HIGHLIGHT_PEN-1;
const Color HIGHLIGHT_SELECT_COLOR = GREEN;
const Color HIGHLIGHT_HISTORY_COLOR = BLUE;

const int PLAYERBOARD_WIDTH = int(740*ZOOM);
const int PLAYERBOARD_HEIGHT = int(484*ZOOM);

const int WINDOW_HEIGHT = 2*PLAYERBOARD_HEIGHT; // Recommended : 968 px
const int WINDOW_WIDTH = PLAYERBOARD_WIDTH+WINDOW_HEIGHT; // Recommended : 1480 px

const int FACTORY_SIDE = int(250*ZOOM);
const int TILE_SIDE = int(53*ZOOM);
const int BAG_SIDE = int(128*ZOOM);

const int PLAYERBOARD_X0 = WINDOW_HEIGHT;

const int WALL_X0 = int(0.5307*PLAYERBOARD_WIDTH);
const int WALL_Y0 = int(0.0406*PLAYERBOARD_HEIGHT);
const int WALL_MARGIN = int(0.31*TILE_SIDE);
const int WALL_SPACING = TILE_SIDE + WALL_MARGIN;

const int PATTERN_X0 = WALL_MARGIN;
const int PATTERN_Y0 = int(0.045*PLAYERBOARD_HEIGHT);

const int FLOOR_X0 = int(0.018*PLAYERBOARD_WIDTH);
const int FLOOR_Y0 = int(0.86*PLAYERBOARD_HEIGHT);
const int FLOOR_MARGIN = int(0.43*TILE_SIDE);
const int FLOOR_SPACING = FLOOR_MARGIN + TILE_SIDE;

const int EVALUATION_BAR_HEIGHT = int(700*ZOOM);
const int EVALUATION_BAR_WIDTH = int(35*ZOOM);
const int EVALUATION_BAR_MARGIN = int(5*ZOOM);

const double FACTORY_SCALE = 0.64;
const int FACTORY_RADIUS = int((WINDOW_HEIGHT-FACTORY_SIDE)*FACTORY_SCALE/2);
const IntPoint2 FACTORY_CENTER = IntPoint2((WINDOW_WIDTH-PLAYERBOARD_WIDTH)/2,WINDOW_HEIGHT/2);
const int FACTORY_MARGIN = int(0.15*FACTORY_SIDE);
const IntPoint2 FACTORY_CENTERING = IntPoint2(FACTORY_SIDE/2, FACTORY_SIDE/2);

const int TEXT_SIZE = int(20*ZOOM);
const int EVAL_BAR_TEXT_SIZE = int(12*ZOOM);
const int BEST_MOVES_TEXT_SIZE = int(8*ZOOM);

const IntPoint2 MIDDLE_P0 = IntPoint2(WINDOW_HEIGHT/2 - (5*WALL_SPACING + WALL_MARGIN)/2, WINDOW_HEIGHT-BAG_SIDE);
const Color MIDDLE_COLOR = Color(230,230,230);

const IntPoint2 DISCARD_P0 = IntPoint2(WINDOW_HEIGHT - 5*WALL_SPACING - WALL_MARGIN - HIGHLIGHT_PEN,0);
const Color DISCARD_COLOR = Color(255,225,225);

const IntPoint2 BAG_CONTENT_P0 = IntPoint2(0,0);
const Color BAG_CONTENT_COLOR = Color(212, 236, 247);


// Color

const Color RED_TILE_COLOR = Color(245,51,63);
const Color BLUE_TILE_COLOR = Color(0,131,173);
const Color YELLOW_TILE_COLOR = Color(255,191,60);
const Color YELLOW_TILE_COLOR2 = RED_TILE_COLOR;
const Color DARK_TILE_COLOR = BLACK;
const Color DARK_TILE_COLOR2 = Color(29,162,172);
const Color LIGHT_TILE_COLOR = Color(29,202,211);
const Color LIGHT_TILE_COLOR2 = WHITE;



// Images

const string GAMEBOX = stringSrcPath("Images/game_box.png");
const string BAG = stringSrcPath("Images/bag.png");
const string PLAYERBOARD = stringSrcPath("Images/playerboard.png");
const string PLAYERBOARD_TRANSPARENT = stringSrcPath("Images/playerboard_transparency.png");
const string FACTORY = stringSrcPath("Images/factory.png");
const string BLUE_TILE = stringSrcPath("Images/blue.svg");
const string YELLOW_TILE = stringSrcPath("Images/yellow.svg");
const string RED_TILE = stringSrcPath("Images/red.svg");
const string DARK_TILE = stringSrcPath("Images/black.svg");
const string LIGHT_TILE = stringSrcPath("Images/cyan.svg");
const string FIRST_PLAYER_TILE = stringSrcPath("Images/first-player.svg");

const vector<string> IMAGE_PATHS = {
    GAMEBOX,
    BAG,
    PLAYERBOARD,
    PLAYERBOARD_TRANSPARENT,
    FACTORY,
    BLUE_TILE,
    YELLOW_TILE,
    RED_TILE,
    DARK_TILE,
    LIGHT_TILE,
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
    void displayImage(int i, int x = 0, int y = 0, double fact = ZOOM);
    /// Shortcut : Display gamebox image
    void displayGamebox(int x = 0, int y = 0, double fact = ZOOM) { displayImage(0,x,y,fact); }
    /// Shortcut : Display bag image
    void displayBag(int x = 0, int y = 0, double fact = ZOOM) { displayImage(1,x,y,fact); }
    /// Shortcut : Display playerboard image
    void displayPlayerboard(int x = 0, int y = 0, double fact = ZOOM) { displayImage(2,x,y,fact); }
    /// Shortcut : Display transparent playerboard image
    void displayPlayerboardTransparent(int x = 0, int y = 0, double fact = ZOOM) { displayImage(3,x,y,fact); }
    /// Shortcut : Display factory image
    void displayFactory(int x = 0, int y = 0, double fact = ZOOM) { displayImage(4,x,y,fact); }
    /// Shortcut : Display tile of color j (0: blue, 1: yellow, 2: red, 3: black, 4: cyan, 5: first player)
    void displayTile(int j, int x = 0, int y = 0, double fact = ZOOM);

    /// Display image i
    void displayImage(int i, IntPoint2 P, double fact = ZOOM){ displayImage(i,P.x(),P.y(),fact); }
    /// Shortcut : Display gamebox image
    void displayGamebox(IntPoint2 P, double fact = ZOOM) { displayGamebox(P.x(),P.y(),fact); }
    /// Shortcut : Display bag image
    void displayBag(IntPoint2 P, double fact = ZOOM) { displayBag(P.x(),P.y(),fact); }
    /// Shortcut : Display playerboard image
    void displayPlayerboard(IntPoint2 P, double fact = ZOOM) { displayPlayerboard(P.x(),P.y(),fact); }
    /// Shortcut : Display transparent playerboard image
    void displayPlayerboardTransparent(IntPoint2 P, double fact = ZOOM) { displayPlayerboardTransparent(P.x(),P.y(),fact); }
    /// Shortcut : Display factory image
    void displayFactory(IntPoint2 P, double fact = ZOOM) { displayFactory(P.x(),P.y(),fact); }
    /// Display one factory
    void displayFactory(byte factory);
    /// Shortcut : Display tile of color j (0: blue, 1: yellow, 2: red, 3: black, 4: cyan, 5: first player)
    void displayTile(int j, IntPoint2 P, double fact = ZOOM) { displayTile(j,P.x(),P.y(),fact); }

    /// Initialize GUI
    Window init();

    /// Display NB_PLAYERS = 2 playerboards
    void displayPlayerboards(Board *board);
    /// Display NB_FACTORIES factories in a circle
    void displayFactories(bool circles = false);

    /// Display walls for both players
    void displayWalls(Board* board);
    /// Display patterns for both players
    void displayPatterns(Board *board);
    /// Display floors for both players
    void displayFloors(Board* board);
    /// Display tiles in one factories
    void displayFactoryTiles(byte factory, Board *board);
    /// Display tiles in all factories
    void displayAllFactoryTiles(Board *board);
    /// Display a box with all Tile colors and numbers
    void displayTileInfo(IntPoint2 P0, Color bg_color, byte tiles[]);
    /// Display tiles in the middle
    void displayMiddleTiles(Board *board);
    /// Display remaining tiles in bag
    void displayBagContent(Board *board);
    /// Display discard
    void displayDiscard(Board *board);
    /// Display evaluation bar (for review mode only)
    void displayEvaluationBar(double wins, double draws, double losses);

    /// Display everything
    void displayBoardState(Board *board);

    /// Clear window and display everything, with no refresh
    void updateBoardState(Board *board);

    /// Display grid for designing UI
    void displayGrid();

    /// Display the scores of each player
    void displayScores(Board *board);

    /// Display the best moves and their valuations
    void displayBestMoves(vector<Move> moves,vector<double> valuations);
};





