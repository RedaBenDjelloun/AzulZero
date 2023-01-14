#pragma once

#include "board.h"
#include "time.h"
#include "Tree.h"
using namespace std;

#include "GUI.h"

class Controller
{
protected:
    double evaluation;
public:
    Controller(){}
    virtual ~Controller(){}
    virtual Move play_move(Board* board, bool play = true)=0;
    double giveEvaluation(){return evaluation;}
};


class Random: public Controller
{
public:
    Random(){}
    Move play_move(Board* board, bool play = true);
};


class Heuristic: public Controller
{
    double par[9] ={0,0,0,0,0,0,0,0,0};
public:
    Heuristic(){}
    Heuristic(int preoptimization);
    Move play_move(Board* board, bool play = true);
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
    Heuristic heuristic = Heuristic(0);
    int nb_expect = 10;
    double time_limit; // in seconds
    byte depth_limit;   // limit of max_depth
    bool time_limited;

    Move next_move;

    Timer chrono;
public:
    MinMax(){}
    MinMax(byte depth_limit_, bool time_limited_=true, double time_limit_ = 1);
    double DFS(Board* board, byte depth, byte max_depth, double alpha = -INFINITY, double beta = + INFINITY);
    Move play_move(Board* board, bool play=true);
};

class Human: public Controller
{
    string name;
    GUI *gui;
public:
    Human(){}
    Human(string name_, GUI *gui_){name=name_ ; gui = gui_;}
    void clickPickableTile(Board *board, byte &factory, byte &color);
    bool clickPlaceableTile(Board *board, byte &line);
    Move play_move(Board* board, bool play=true);
};

struct State{
    int wins=0;
    int draws=0;
    int losses=0;

    State(){}
    State(int wins_, int draws_, int losses_){wins=wins_; draws=draws_; losses=losses_;}
    void operator+=(State s){wins+=s.wins; draws+=s.draws; losses+=s.losses;}
    State inverse(){return State(losses,draws,wins);};
    void update(byte player,byte score1, byte score2){
        if (score1==score2)
            draws++;
        else if ((score1>score2 and player==0) or (score1<score2 and player==1))
            wins++;
        else
            losses++;
    }
};


struct MCNode{
    Move move;
    State s;

    double UCT(int N_parent, double c=sqrt(2)) const{return (s.wins+0.5*s.draws)/(s.wins+s.draws+s.losses)+c*sqrt(log(N_parent)/(s.wins+s.draws+s.losses));}
    int N() const{return s.wins+s.draws+s.losses;}
    double value(){return (s.wins+0.5*s.draws)/double(N());}
    MCNode(){}
    MCNode(Move move_){move=move_;}
};


class MCTS: public Controller
{
    double time_limit=10;
    int nb_max_simul=10000;
    bool time_limited=true;
    bool nb_simul_limited=false;
    Timer chrono;
    Controller** random_players;
public:
    MCTS();
    ~MCTS();
    MCTS(double time_limit_){time_limit=time_limit_;}

    /// Add all possible nodes from a leaf
    State add_nodes(Board* board,Tree<MCNode>* tree);
    State tree_search(Board* board,Tree<MCNode>* tree);
    Move play_move(Board* board, bool play=true);
};

/// Play an entire game given the players
void play_game(Board* board, Controller** players);
