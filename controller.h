#pragma once

#include "board.h"

class Controller
{
public:
    Controller(){}
    virtual void play(Board* board){}
};


class Random: public Controller
{
public:
    Random(){};
    void play(Board* board);
};


class Heuristic: public Controller
{
    double k1=9;
    double k2=5;
    double k3=2;
    double k4=-2;
public:
    Heuristic(){};
    void play(Board* board);
    double line_complete_reward(int line,int nb){return line+1+nb+k1;}
    double non_complete_line_reward(int line, int nb){return k2+k3*(2*nb-line-1);}
    double discard_reward(int nb){return k4*nb;}
    double reward(int line, int nb, int in_the_floor);
};

/// Play an entire game given the players
void play(Board* board, Controller** players);
