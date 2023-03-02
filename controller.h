#pragma once

#include "board.h"
#include "time.h"
#include "Tree.h"
#include "GUI.h"

class Controller
{
public:
    Controller(){}
    virtual ~Controller(){}
    virtual Move choose_move(Board* board)=0;
};

class Random: public Controller
{
public:
    Random(){}
    Move choose_move(Board* board);
};


class PseudoRandom: public Controller
{
    double coeff=3;
public:
    PseudoRandom(){}
    PseudoRandom(double coeff_){coeff=coeff_;}
    Move choose_move(Board* board);
};


class Heuristic: public Controller
{
protected:
    double par[9] ={0,0,0,0,0,0,0,0,0};
public:
    Heuristic(){}
    Heuristic(int preoptimization);
    virtual Move choose_move(Board* board);
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
    int nb_expect = 10; // for expecti-MinMax
    double time_limit; // in seconds
    byte depth_limit;   // limit of max_depth
    bool time_limited;

    Move next_move;

    Timer chrono;
public:
    double valuation;
    MinMax(){}
    MinMax(byte depth_limit_, bool time_limited_=true, double time_limit_ = 1);
    double DFS(Board* board, byte depth, byte max_depth, double alpha = -INFINITY, double beta = + INFINITY);
    Move choose_move(Board* board);
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
    Move choose_move(Board* board);
};

void highlightFactoryTilesOfColor(byte factory, byte factoryTiles[NB_TILES_PER_FACTORY], byte color);
void highlightMiddleTile(byte tile);
void highlightMiddle();
void highlightPatternLine(byte player, byte line);

struct State{
    unsigned int wins=0;
    unsigned int draws=0;
    unsigned int losses=0;

    State(){}
    State(int wins_, int draws_, int losses_){wins=wins_; draws=draws_; losses=losses_;}
    int total(){return wins+draws+losses;}
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
    bool operator<(MCNode other){return value()<other.value();}
    MCNode(){}
    MCNode(Move move_){move=move_;}
};


class MCTS: public Controller
{
    double time_limit=1;
    int nb_max_simul=10000;
    bool time_limited=true;
    bool nb_simul_limited=false;
    Timer chrono;
    Controller** random_players;
public:
    MCTS(double coeff=3);
    ~MCTS();

    /// Add all possible nodes from a leaf
    State add_nodes(Board* board,Tree<MCNode>* tree);
    State tree_search(Board* board,Tree<MCNode>* tree);
    Move choose_move(Board* board);
};

/// Play an entire game given the players
void play_game(Board* board, Controller** players);
