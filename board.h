#pragma once

#include <map>
using namespace std;

// Parameters
const int NB_COLORS  = 5;
const int NB_TILES_PER_COLOR = 20;
const map<int, int> NB_FACTORIES = {
    {2, 5},
    {3, 7},
    {4, 9}
};
const int NB_TILES_PER_FACTORY = 4;

// Geometry
const int WALL_HEIGHT = 5;
const int WALL_WIDTH = 5;
const int WALL_SIZE = WALL_HEIGHT*WALL_WIDTH;

// Scoring
const int FLOOR_SIZE=7;
const int FLOOR[FLOOR_SIZE] = {-1,-1,-2,-2,-2,-3,-3};
const int HORIZONTAL_LINE_BONUS = 2;
const int VERTICAL_LINE_BONUS = 7;
const int COLOR_BONUS = 10;

// Output the color
int wallColumnToColor(int column, int line){
    return (column - line + NB_COLORS) % NB_COLORS;
}

int wallColorToColumn(int color, int line){
    return (line + color) % NB_COLORS;
}


class Board{

    int nb_players;
    int current_player;

    // score for each player
    // [player] -> score
    int* scores;

    // tiles in the bag
    // [color] -> nb of tiles of this color
    int* bag;

    // tiles in the lid of the game box
    // [color] -> nb of tiles of this color
    int* discard;

    // factories containing tiles and the center of the table
    // [factory,color] -> nb of tiles of this color
    // factory = 0 -> center of the table
    int* factories;

    // tiles on the pattern lines for each player
    // [player,line,(nb,color)] ->
    int* pattern_lines;

    // tiles on the floor line for each player
    // [player,square] -> color or -1 if no tile or NB_COLORS if "1" tile
    int* floor_lines;

    // tiles on the wall for each player
    // [player, line, column] -> is there a tile ?
    bool* walls;


public:

    Board(int nb_players_);
    ~Board();

    bool endOfTheRound();
    bool endOfTheGame();
    void nextRound();
    // add a tile to the wall of one player
    void addTileToWall(int player, int line, int column);
    void updateWall();
    void updateFloor();
    void updateFactories();
    int chooseRandomTile(int tiles_remaining);
    void refillBag();
};
