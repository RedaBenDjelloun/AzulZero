#include <Imagine/Graphics.h>
using namespace Imagine;
#include "board.h"

Board::Board(int nb_players_){
    nb_players = nb_players_;
    current_player = 0;

    bag = new int[nb_color];
    for(int i=0; i<nb_color; i++){
        bag[i] = 20;
    }

    discard_box = new int[nb_color];
    for(int i=0; i<nb_color; i++){
        discard_box[i] = 0;
    }

    board = new int((nb_fabrics()+1)*5);
}

Board::~Board(){
    delete[] bag;
    delete[] discard_box;
    delete [] board;
}


int Board::nb_fabrics(){
    return 2*nb_players+1;
}
