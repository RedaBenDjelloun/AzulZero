#pragma once

#include "board.h"

class Controller
{
public:
    Controller(){}
    virtual void play_move(Board* board){}
};


class Random: public Controller
{
public:
    Random(){};
    void play_move(Board* board);
};


class Heuristic: public Controller
{
    double par[8] ={0,0,0,0,0,0,0,0};
public:
    Heuristic(){}
    Heuristic(int preoptimization);
    void play_move(Board* board);
    void optimize(Controller* opponent);
    double line_complete_reward(int line,int nb){return par[0] + par[1]*line + par[2]*nb;}
    double non_complete_line_reward(int line, int nb){return par[3] + par[4]*line + par[5]*nb;}
    double discard_reward(int nb){return par[6] + par[7]*nb;}
    double reward(int line, int nb, int in_the_floor);
};

/// Play an entire game given the players
void play_game(Board* board, Controller** players);
