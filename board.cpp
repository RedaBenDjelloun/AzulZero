#include <Imagine/Graphics.h>
using namespace Imagine;
#include "board.h"

Board::Board(int nb_players_){
    nb_players = nb_players_;
    current_player = 0;

    bag = new int[NB_COLORS];
    for(int i=0; i<NB_COLORS; i++){
        bag[i] = NB_TILES_PER_COLOR;
    }

    discard = new int[NB_COLORS];
    for(int i=0; i<NB_COLORS; i++){
        discard[i] = 0;
    }

    int factories_size = (NB_FACTORIES.at(nb_players)+1)*5;
    factories = new int[factories_size];
    for(int i=0; i<factories_size; i++){
        factories[i] = 0;
    }

    int pattern_lines_size = nb_players*2*WALL_HEIGHT;
    pattern_lines = new int[pattern_lines_size];
    for(int i=0; i<pattern_lines_size; i++){
        pattern_lines[i] = 0;
    }

    int floor_lines_size = FLOOR_SIZE*nb_players;
    floor_lines = new int[floor_lines_size];
    for(int i=0; i<floor_lines_size; i++){
        floor_lines[i] = -1;
    }

    int wall_size = WALL_HEIGHT*WALL_WIDTH;
    walls = new bool[wall_size * nb_players];
}

Board::~Board(){
    delete[] bag;
    delete[] discard;
    delete [] factories;
    delete [] pattern_lines;
    delete [] floor_lines;
    delete [] walls;
}
