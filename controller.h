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
public:
    Heuristic(){};
    void play(Board* board);
};

/// Play an entire game given the players
void play(Board* board, Controller** players);
