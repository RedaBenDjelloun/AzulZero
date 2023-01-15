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


MCNode Game::valuation(int nb_simul,Tree<MCNode>* tree){
    State s;
    while(tree->getData().N()<nb_simul){
        Board board_copy(*currentState());
        evaluator.tree_search(&board_copy,tree);
    }
    double best_bound = -INFINITY;
    int child_index = -1;
    int N = tree->getData().N();
    for(int i=0; i<tree->nbChildren(); i++){
        if(best_bound < tree->getChild(i)->getData().UCT(N)){
            best_bound = tree->getChild(i)->getData().UCT(N);
            child_index = i;
        }
    }
    Tree<MCNode>* child(tree->getChild(child_index));
    return child->getDataRef();
}


int keyboard(){
    Event e;
    do {
        bool deja_vu;
        int j;
        getEvent(0,e);

        if(e.type==EVT_KEY_ON){
            if(e.key==KEY_LEFT or e.key==KEY_RIGHT or e.key==KEY_ESCAPE){
                return e.key;
            }
        }

    } while(e.type!=EVT_NONE);
    return -1;
}

void Game::review_game(GUI gui){
    init();
    while(true){
        gui.updateBoardState(currentState());
        MCNode root;
        Tree<MCNode> tree(root);
        byte factory=255;
        byte color=255;
        byte line =255;
        int key=-1;
        if(currentState()->endOfTheGame()){

        }
        else{
            for(int i=1; i<=100; i++){
                MCNode best_child = valuation(10000*i,&tree);
                State eval = best_child.s;
                Move best_move = best_child.move;
                if(currentState()->currentPlayer()==0)
                    eval = eval.inverse();
                double wins = double(eval.wins)/eval.total();
                double draws = double(eval.draws)/eval.total();
                double losses = double(eval.losses)/eval.total();
                gui.displayEvaluationBar(wins,draws,losses);
                byte factoryTiles[NB_TILES_PER_FACTORY];
                if(i%10==0 and (factory!=best_move.factory or color!=best_move.col or line!=best_move.line)){
                    gui.displayFactories();
                    gui.displayAllFactoryTiles(currentState());
                    gui.displayMiddleTiles(currentState());
                    factory = best_move.factory;
                    color = best_move.col;
                    line = best_move.line;
                    if(factory<NB_FACTORIES){
                        currentState()->fillFactoryTilesArray(factoryTiles, factory);
                        highlightFactoryTilesOfColor(factory, factoryTiles, color);
                    }
                    else{
                        highlightMiddle();
                        highlightMiddleTile(color);
                    }
                }
                key = keyboard();
                if(key!=-1)
                    break;
            }
        }
        while(!(key==KEY_RIGHT or key==KEY_LEFT or key==KEY_ESCAPE))
            key = keyboard();
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
        milliSleep(50);
        while(!board->endOfTheRound()){
            if(save)
                game->saveState(board);
            // Play move and update board
            Move m = players[board->currentPlayer()]->play_move(board);
            if(save)
                game->saveMove(m);
            gui.updateBoardState(board);
            milliSleep(50);
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
