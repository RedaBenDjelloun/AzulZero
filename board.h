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
const int WALL[WALL_HEIGHT*WALL_WIDTH] = {
    0,1,2,3,4,
    4,0,1,2,3,
    3,4,0,1,2,
    2,3,4,0,1,
    1,2,3,4,0
};

// Scoring
int FLOOR_SIZE;
const int FLOOR[FLOOR_SIZE] = {-1,-1,-2,-2,-2,-3,-3};
const int HORIZONTAL_LINE_BONUS = 2;
const int VERTICAL_LINE_BONUS = 7;
const int COLOR_BONUS = 10;


class Board{

    int nb_players;
    int current_player;

    // score for each player
    int* scores;

    // tiles in the bag
    int* bag;

    // tiles in the lid of the game box
    int* discard;

    // factories containing tiles and the center of the table
    int* factories;

    // tiles on the pattern lines for each players
    int* pattern_lines;

    // tiles on the floor line for each players
    int* floor_lines;

    //tiles on the wall for each player
    bool* walls;


public:

    Board(int nb_players_);
    ~Board();
};
