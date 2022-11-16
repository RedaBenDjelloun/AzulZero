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
    double par[9] ={0,0,0,0,0,0,0,0,0};
public:
    Heuristic(){}
    Heuristic(int preoptimization);
    void play_move(Board* board);
    void optimize(Controller* opponent, int nb_test_game=100, int nb_evolve_game=100);
    double line_complete_reward(int line,int nb){return par[0] + par[1]*line + par[2]*nb;}
    double non_complete_line_reward(int line, int nb, int missing){return par[3] + par[4]*line + par[5]*nb + par[6]*missing;}
    double discard_reward(int nb){return par[7] + par[8]*nb;}
    double reward(int line, int nb, int in_the_floor);
};

class MinMax: public Controller
{
    byte max_depth;
public:
    MinMax(){}
    MinMax(byte max_depth_){max_depth= max_depth_;}
    int play_move(Board* board, byte depth);
    void play_move(Board* board);
};


/// Play an entire game given the players
void play_game(Board* board, Controller** players);
