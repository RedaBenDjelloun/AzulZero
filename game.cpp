#include "game.h"

void Game::nextState(){
    if(move_index<moves.size())
        move_index++;
}

void Game::previousState(){
    if(move_index>0)
        move_index--;
}

void Game::init(){
    move_index = 0;
}


double Game::valuation(int choice){
    Controller* controller;
    if(choice==0)
        controller = new MinMax(20,true,1);
    else
        controller = new MCTS(1);
    controller->play_move(&states[move_index],false);
    double result = 2*(0.5-states[move_index].currentPlayer())*controller->giveEvaluation();
    delete controller;
    return result;
}


void Game::review_game(GUI gui){
    init();
    while(true){
        gui.updateBoardState(currentState());
        double evaluation = valuation();
        gui.displayEvaluationBar(evaluation);
        int key = getKey();
        if(key == KEY_RIGHT)
            nextState();
        else if(key == KEY_LEFT)
            previousState();
        else if(key == KEY_ESCAPE)
            break;
    }
}

Game* playGameGraphics(Board* board, Controller **players, GUI &gui, bool save){
    Game* game = new Game();
    // Display initial board state
    gui.updateBoardState(board);
    // Go to the first round after click
    click();
    board->nextRound();
    // Loop on rounds
    while(!board->endOfTheGame()){
        // Display board at the beginning of the round
        gui.updateBoardState(board);
        milliSleep(500);
        while(!board->endOfTheRound()){
            if(save)
                game->saveState(board);
            // Play move and update board
            Move m = players[board->currentPlayer()]->play_move(board);
            if(save)
                game->saveMove(m);
            gui.updateBoardState(board);
            milliSleep(500);
        }
        // Go to the next round after click
        click();
        board->nextRound();
    }
    // Display final board state and update scores
    gui.updateBoardState(board);
    board->addEndgameBonus();
    if(save)
        game->saveState(board);
    return game;
}
