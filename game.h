#pragma once
#include "board.h"
#include "controller.h"

// naive implementation for saving a game :
// save all states and move of the game
// states of the board could be found from less informations
// (states of the begining of each round + moves)

class Game{
    /// All the moves of the saved game
    vector<Move> moves;

    /// States of the games
    vector<Board> states;

    MCTS evaluator = MCTS();

    unsigned int move_index=0;

    GUI* gui;

public:
    Game(GUI* gui_){gui=gui_;}

    /// Update the board by moving one move forward
    void nextState();

    /// Update the board by moving one move backward
    void previousState();

    /// Save the board
    void saveState(Board* board){states.push_back(Board(*board));}

    /// Save the the last move
    void saveMove(Move m){moves.push_back(m);}

    /// Update the board returning at the beginning of the game
    void init();

    /// return a pointer to the current state of the game
    Board* currentState(){return &states[move_index];};

    /// Update MC tree with limited time to compute it
    void valuation(double time_limit, Tree<MCNode> *tree);


    void evaluationIteration(Tree<MCNode>* tree, Move& current_best_move);

    /// Launch a review of the game
    void review_game();
};

/// Play an entire game given the players with graphics
Game* playGameGraphics(Board* board, Controller** players, GUI &gui, bool save=true);
