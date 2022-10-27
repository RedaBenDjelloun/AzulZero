#pragma once

int nb_color  = 5;

class Board{

    int nb_players;
    int current_player;

    // score
    int* scores;

    // tiles in the bag
    int* bag;

    // tiles in the discard pil
    int* discard_box;

    // tiles on the board
    int* board;

public:

    Board(int nb_players_);
    ~Board();

    int nb_fabrics();
    // Check if
    int winner();
};
