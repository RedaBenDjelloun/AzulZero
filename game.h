#pragma once
#include "board.h"
#include "controller.h"

class Game{
    /// All the moves of the saved game
    vector<Move> moves;

    /// States of the games
    vector<Board> states;

    unsigned int move_index=0;

public:
    Game(){}

    /// Update the board by moving forward one move
    void nextState();

    void previousState();

    void saveState(Board* board){states.push_back(Board(*board));}

    void saveMove(Move m){moves.push_back(m);}

    /// Update the board returning at the beginning of the game
    void init();

    Board* currentState(){return &states[move_index];};

    /// Valuation of the state of the game with a choosen AI
    double valuation(int choice=0);

    void review_game(GUI gui);
};

/// Play an entire game given the players with graphics
Game* playGameGraphics(Board* board, Controller** players, GUI &gui, bool save=true);
