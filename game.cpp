#include "game.h"

void Game::nextState(){
    if(current_state.endOfTheRound()){
        if(current_state.endOfTheGame())
            return;
        current_state = states[++round_index];
    }
    else
        current_state.play(moves[++move_index]);
}


void Game::init(){
    current_state = states[0];
    round_index = 0;
    move_index = -1;
}


double Game::valuation(int choice){
    Controller* controller;
    if(choice==0)
        controller = new MinMax(20,true,1);
    else
        controller = new MCTS(1);
    controller->play_move(&current_state,false);
    double result = controller->giveEvaluation();
    delete controller;
    return result;
}
