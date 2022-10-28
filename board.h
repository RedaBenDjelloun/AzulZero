#pragma once

#include <Imagine/Graphics.h>
using namespace Imagine;
#include <iostream>
#include <map>
using namespace std;

// Parameters
const int NB_PLAYERS = 2;
const int NB_COLORS  = 5;
const int NB_TILES_PER_COLOR = 20;
const int NB_FACTORIES = 2*NB_PLAYERS+1;
const int NB_TILES_PER_FACTORY = 4;

// Geometry
const int WALL_HEIGHT = 5;
const int WALL_WIDTH = 5;
const int WALL_SIZE = WALL_HEIGHT*WALL_WIDTH;

// Scoring
const int FLOOR_SIZE = 7;
const int FLOOR[FLOOR_SIZE] = {-1,-1,-2,-2,-2,-3,-3};
const int HORIZONTAL_LINE_BONUS = 2;
const int VERTICAL_LINE_BONUS = 7;
const int COLOR_BONUS = 10;


int wallColumnToColor(int column, int line);

int wallColorToColumn(int color, int line);



class Board{
    byte current_player;

    // Score of each player
    // [player] -> score
    byte scores[NB_PLAYERS];

    // tiles in the bag
    // [color] -> nb of tiles of this color
    byte bag[NB_COLORS];

    // tiles in the lid of the game box
    // [color] -> nb of tiles of this color
    byte discard[NB_COLORS];

    // factories containing tiles and the center of the table
    // [factory,color] -> nb of tiles of this color
    // factory = NB_FACTORIES -> center of the table
    byte factories[(NB_FACTORIES+1)*NB_COLORS];

    // tiles on the pattern lines for each player
    // [player,line,(nb,color)] ->
    byte pattern_lines[NB_PLAYERS*WALL_HEIGHT*2];

    // tiles on the floor line for each player
    // [player,square] -> color or NB_COLORS if "1" tile or NB_COLOR+1 if no tile
    byte floor_lines[NB_PLAYERS*FLOOR_SIZE];

    // tiles on the wall for each player
    // [player, line, column] -> is there a tile ?
    bool walls[NB_PLAYERS*WALL_SIZE];


public:

    Board();
    ~Board(){};

    /// Checks if it is the end of the round
    bool endOfTheRound();

    /// Checks if it is the end of the game
    bool endOfTheGame();

    /// Update score and set up the tiles for the next round
    void nextRound();

    /// Add a tile to the wall of one player and update his score
    void addTileToWall(int player, int line, int column);

    /// Add the right tiles to the wall of each player, clean pattern lines and update score
    void updateWall();

    /// Clear the floor of each player (uptade also the score)
    void updateFloor();

    /// Replace new tiles on the factory
    void updateFactories();

    /// Pick a random tile in the bag
    int chooseRandomTile(int tiles_remaining);

    /// Put discarded tiles back in the bag
    void refillBag();

    /// Add bonus for every horizontal/vertical line and color complete
    void addEndgameBonus();
};
