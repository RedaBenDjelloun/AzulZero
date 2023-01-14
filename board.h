#pragma once

#include <Imagine/Graphics.h>
using namespace Imagine;
#include <iostream>
using namespace std;

// Parameters
const byte NB_PLAYERS = 2;
const byte NB_COLORS  = 5;
const byte NB_TILES_PER_COLOR = 20;
const byte NB_FACTORIES = 2*NB_PLAYERS+1;
const byte NB_TILES_PER_FACTORY = 4;

// Geometry
const byte WALL_HEIGHT = 5;
const byte WALL_WIDTH = 5;
const byte WALL_SIZE = WALL_HEIGHT*WALL_WIDTH;

// Scoring
const byte FLOOR_SIZE = 7;
const byte FLOOR[FLOOR_SIZE] = {1,1,2,2,2,3,3};
const byte HORIZONTAL_LINE_BONUS = 2;
const byte VERTICAL_LINE_BONUS = 7;
const byte COLOR_BONUS = 10;


byte wallColumnToColor(byte column, byte line);

byte wallColorToColumn(byte color, byte line);

struct Move{
    byte factory;
    byte col;
    byte line;
    Move(){}
    Move(byte factory_, byte col_, byte line_){
        factory = factory_;
        col = col_;
        line = line_;
    }
};

class Board{

    byte current_player;

    // Score of each player
    // [player] -> score
    byte scores[NB_PLAYERS];

    // tiles in the bag
    // [color] -> nb of tiles of this color
    byte bag[NB_COLORS];

    // tiles in the lid of the game box
    // [color] -> nb of tiles of this color
    byte discard[NB_COLORS];

    // factories containing tiles and the center of the table
    // [factory,color] -> nb of tiles of this color
    // factory = NB_FACTORIES -> center of the table
    byte factories[(NB_FACTORIES+1)*NB_COLORS];

    // indicates where the "1" tile
    // player or NB_PLAYER -> center of the table
    byte tile1;

    // tiles on the pattern lines for each player
    // [player,line,(nb,color)] ->
    byte pattern_lines[NB_PLAYERS*WALL_HEIGHT*2];

    // tiles on the floor line for each player
    // [player,color] -> color or NB_COLORS if "1" tile or NB_COLOR+1 if no tile
    byte floor_lines[NB_PLAYERS*FLOOR_SIZE];

    // tiles on the wall for each player
    // [player, line, column] -> is there a tile ?
    bool walls[NB_PLAYERS*WALL_SIZE];

public:

    Board(){};
    ~Board(){};
    void init();

    bool operator ==(const Board& b) const;

    // Accessors (and basic informations)
    byte currentPlayer() const {return current_player;}
    byte getScore(byte player) const {return scores[player];}
    byte getBagTile(byte color) const {return bag[color];}
    byte getDiscardedTile(byte color) const {return discard[color];}
    byte getFactoryTile(byte factory, byte color) const{return factories[factory*NB_COLORS+color];}
    byte getTile1() const{return tile1;}
    byte getPatternLineNb(byte player, byte line) const{return pattern_lines[player*WALL_HEIGHT*2 + line*2];}
    byte getPatternLineColor(byte player, byte line) const{return pattern_lines[player*WALL_HEIGHT*2 + line*2 + 1];}
    byte getFloorTile(byte pos) const{return floor_lines[pos];}
    byte nbFloorTiles() const;
    inline bool wallTileFilled(byte player, byte line, byte column) const{return walls[player*WALL_SIZE+line*WALL_HEIGHT+column];}

    /// Set the first player randomly
    void random_first_player(){current_player = rand()%2;}

    /// returns 0 if the first player wins 1 if the scaond wins and 1 if its a draw
    byte winner() const{return (scores[0]<scores[1])+(scores[0]<=scores[1]);}

    /// Checks if it is the end of the round
    bool endOfTheRound() const;

    /// Checks if it is the end of the game
    bool endOfTheGame() const;

    /// Update score and set up the tiles for the next round
    void nextRound();

    /// Add a tile to the wall of one player and update his score
    void addTileToWall(byte player, byte line, byte column);

    /// Add the right tiles to the wall of each player, clean pattern lines and update score
    void updateWall();

    /// Clear the floor of each player (uptade also the score)
    void updateFloor();

    /// Replace new tiles on the factory
    void updateFactories();

    /// Pick a random tile in the bag
    byte chooseRandomTile(byte tiles_remaining);

    /// Put discarded tiles back in the bag
    void refillBag();

    /// Add bonus for every horizontal/vertical line and color complete
    void addEndgameBonus();

    /// Play a move without checking if it is possible (Unknown behaviour if the move is not possible)
    void play(byte factory, byte color, byte line);
    void play(Move m){play(m.factory,m.col,m.line);}

    ///  Checks if a tile exists
    bool pickableTile(byte factory, byte color) const;

    /// Checks if a tile is placeable on a particular line
    bool placeableTile(byte color, byte line) const;

    /// Checks if the move is possible
    bool playable(byte factory, byte color, byte line) const;
    bool playable(Move m) const {return playable(m.factory,m.col,m.line);}

    /// Add a malus to a player
    void addMalus(byte malus, byte player);

    /// End the turn of the current player
    void nextPlayer(){current_player = (current_player+1)%NB_PLAYERS;}

    /// In terminal display of the factories
    void display() const;

    /// Construct the list of all the possible moves
    vector<Move> moveList();

    void addBonusToAll(byte bonus){scores[0]+=bonus; scores[1]+=bonus;}

    void setScoreToAll(byte score){scores[0]=score; scores[1]=score;}
};

void randomGameTest();

