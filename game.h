#pragma once
#include "board.h"
#include "controller.h"

class Game{
    /// All the moves of the saved game
    vector<Move> moves;

    /// State of the game at the start of each round
    vector<Board> states;

    int move_index=0;
    int round_index=0;

    /// Board
    Board current_state;
public:
    Game(){}

    /// Update the board by moving forward one move
    void nextState();

    /// Update the board returning at the beginning of the game
    void init();

    /// Valuation of the state of the game with a choosen AI
    double valuation(int choice=0);

};
