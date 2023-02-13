#pragma once
#include "board.h"
#include "controller.h"

class Game{
    /// All the moves of the saved game
    vector<Move> moves;

    /// States of the games
    vector<Board> states;

    MCTS evaluator = MCTS(0);

    unsigned int move_index=0;

public:
    Game(){}

    /// Update the board by moving one move forward
    void nextState();

    /// Update the board by moving one move bacward
    void previousState();

    /// Save the board
    void saveState(Board* board){states.push_back(Board(*board));}

    /// Save the the last move
    void saveMove(Move m){moves.push_back(m);}

    /// Update the board returning at the beginning of the game
    void init();

    /// return a pointer to the current state of the game
    Board* currentState(){return &states[move_index];};

    /// Valuation of the state of the game with a choosen AI
    MCNode valuation(double time_limit, Tree<MCNode> *tree);

    /// Launch a review of the game
    void review_game(GUI gui);

    /// Compute game stats
    void game_stats();
};

/// Play an entire game given the players with graphics
Game* playGameGraphics(Board* board, Controller** players, GUI &gui, bool save=true);
