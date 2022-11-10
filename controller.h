#pragma once

#include "board.h"

class Controller
{
protected:
    Board* board;
public:
    Controller(){}
    Controller(Board* board_){board=board_;}
    virtual void play(){}
};


class Random: public Controller
{
public:
    Random(){};
    Random(Board* board_): Controller(board_){};
    void play();
};


class Heuristic: public Controller
{
public:
    Heuristic(){};
    Heuristic(Board* board_): Controller(board_){};
    void play();
};

/// Play an entire game given the players
void play(Board* board, Controller* players);
