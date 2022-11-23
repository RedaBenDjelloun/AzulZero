#pragma once

#include "board.h"
#include "time.h"

class Controller
{
public:
    static bool display_game; // Activate GUI
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
protected:
    double time_limit = 0.01; // in seconds
    byte max_depth;     // current depth of the DFS
    byte depth_limit;   // limit of max_depth
    bool time_limited;

    byte choosen_color;
    byte choosen_factory;
    byte choosen_line;

    Timer chrono;
public:
    MinMax(){}
    MinMax(byte depth_limit_, bool time_limited_=true);
    virtual int DFS(Board* board, byte depth);
    virtual void play_move(Board* board);
};

class Human: public Controller
{
    string name;
public:
    Human(){}
    Human(string name_){name=name_;}
    void play_move(Board* board);
};


/// Play an entire game given the players
void play_game(Board* board, Controller** players);
