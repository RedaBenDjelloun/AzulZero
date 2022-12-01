#pragma once

#include "board.h"
#include "time.h"
#include <unordered_map>
using namespace std;

class Controller
{
public:
    static bool display_game; // Activate GUI
    Controller(){}
    virtual ~Controller(){}
    virtual void play_move(Board* board)=0;
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

struct PositionValue{
    double value;
    byte depth;
    PositionValue(double value_,byte depth_){value=value_;depth=depth_;}
};

class MinMax: public Controller
{
protected:
    int nb_expect = 10;
    double time_limit; // in seconds
    byte depth_limit;   // limit of max_depth
    bool time_limited;
    double tol = 0;

    byte choosen_color;
    byte choosen_factory;
    byte choosen_line;
    unordered_map<Board,PositionValue> look_up_table;

    Timer chrono;
public:
    MinMax(){}
    MinMax(byte depth_limit_, bool time_limited_=true, double time_limit_ = 0.01);
    double DFS(Board* board, byte depth, byte max_depth, double alpha = -INFINITY, double beta = + INFINITY);
    void play_move(Board* board);
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
