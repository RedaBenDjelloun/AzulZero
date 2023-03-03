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


void Game::valuation(double time_limit,Tree<MCNode>* tree){
    Timer t;
    t.reset();
    while(t.lap()<time_limit){
        Board board_copy(*currentState());
        evaluator.tree_search(&board_copy,tree);
    }
}


int keyboard(){
    Event e;
    do {
        getEvent(0,e);

        if(e.type==EVT_KEY_ON){
            if(e.key==KEY_LEFT or e.key==KEY_RIGHT or e.key==KEY_ESCAPE){
                return e.key;
            }
        }

    } while(e.type!=EVT_NONE);
    return -1;
}

void bestMoves(Tree<MCNode>* tree,vector<Move>& moves){
    moves.push_back(tree->getData().move);
    Tree<MCNode>* best_child = tree->max();
    if(!best_child) // check if nullptr
        return;
    bestMoves(best_child,moves);
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
        if(currentState()->endOfTheRound()){
            for(int i=1; i<=1000; i++){
                valuation(0.1,&tree);
                MCNode best_child = tree.getData();
                State eval = best_child.s;
                if(currentState()->currentPlayer()==1)
                    eval = eval.inverse();
                double wins = double(eval.wins)/eval.total();
                double draws = double(eval.draws)/eval.total();
                double losses = double(eval.losses)/eval.total();
                gui.displayEvaluationBar(wins,draws,losses);
                key = keyboard();
                if(key==KEY_RIGHT or key==KEY_LEFT or key==KEY_ESCAPE)
                    break;
            }
        }
        else{
            for(int i=0; i<1000; i++){
                valuation(0.1,&tree);
                MCNode best_child = tree.max()->getData();
                State eval = best_child.s;
                Move best_move = best_child.move;
                if(currentState()->currentPlayer()==0)
                    eval = eval.inverse();
                double wins = double(eval.wins)/eval.total();
                double draws = double(eval.draws)/eval.total();
                double losses = double(eval.losses)/eval.total();
                gui.displayEvaluationBar(wins,draws,losses);
                vector<Move> best_moves;
                auto best_childs = tree.firstN(5);
                vector<double> valuations;
                for(auto it = best_childs.begin(); it!= best_childs.end(); it++){
                    bestMoves(*it,best_moves);
                    best_moves.push_back(Move{255,255,255});
                    double value = (*it)->getData().value();
                    if(currentState()->currentPlayer()==0)
                        value = 1-value;
                    valuations.push_back(value);
                }
                gui.displayBestMoves(best_moves,valuations);
                key = keyboard();
                byte factoryTiles[NB_TILES_PER_FACTORY];
                if (i%10==0){
                    if(factory!=best_move.factory or color!=best_move.col or line!=best_move.line){
                        milliSleep(50);
                        noRefreshBegin();
                        clearWindow();
                        gui.displayBoardState(currentState());
                        factory = best_move.factory;
                        color = best_move.col;
                        line = best_move.line;
                        if(factory<NB_FACTORIES){
                            currentState()->fillFactoryTilesArray(factoryTiles, factory);
                            highlightFactoryTilesOfColor(factory, factoryTiles, color);
                        }
                        else{
                            highlightMiddleTile(color);
                        }
                        highlightPatternLine(currentState()->currentPlayer(),line = best_move.line);
                        gui.displayEvaluationBar(wins,draws,losses);
                        gui.displayBestMoves(best_moves,valuations);
                        noRefreshEnd();
                        milliSleep(50);
                    }
                }
                if(key==KEY_RIGHT or key==KEY_LEFT or key==KEY_ESCAPE)
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
            return;
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
            Move m = players[board->currentPlayer()]->choose_move(board);
            board->play(m);
            if(save)
                game->saveMove(m);
            gui.updateBoardState(board);
            milliSleep(50);
        }
        // Go to the next round after click
        click();
        if(save){
            game->saveState(board);
            game->saveMove(Move{255,255,255});
        }
        board->nextRound();
    }
    // Display final board state and update scores
    gui.updateBoardState(board);
    board->addEndgameBonus();
    if(save)
        game->saveState(board);
    milliSleep(50);
    gui.updateBoardState(board);
    milliSleep(50);
    click();
    return game;
}
