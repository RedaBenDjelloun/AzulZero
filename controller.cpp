#include "controller.h"


struct TimeOutException{};

//////////////////// RANDOM ////////////////////


void Random::play_move(Board* board){
    int nb_pickeable = 0;
    for(int factory=0; factory<=NB_FACTORIES; factory++){
        for(int color=0; color<NB_COLORS; color++){
            nb_pickeable += board->pickableTile(factory,color);
        }
    }

    int pick_choice = rand()%nb_pickeable;
    int factory_choice;
    int color_choice;
    int acc = 0;
    for(int factory=0; factory<=NB_FACTORIES; factory++){
        for(int color=0; color<NB_COLORS; color++){
            acc += board->pickableTile(factory,color);
            if(pick_choice<acc){
                factory_choice = factory;
                color_choice = color;
                factory = NB_FACTORIES;
                color = NB_COLORS;
            }
        }
    }

    int nb_placeable = 0;
    for(int line=0; line<=WALL_HEIGHT; line++){
        nb_placeable += board->placeableTile(color_choice, line);
    }

    nb_placeable = max(1,nb_placeable-1); // do not play in the floor if he doesn't have to
    int spot_choice = rand()%nb_placeable;
    acc=0;

    for(int line=0; line<=WALL_HEIGHT; line++){
        acc += board->placeableTile(color_choice, line);
        if(spot_choice<acc){
            board->play(factory_choice,color_choice,line);
            break;
        }
    }
}



//////////////////// HEURISTIC ////////////////////


double Heuristic::reward(int line, int nb, int in_the_floor){
    if(line==WALL_HEIGHT)
        return discard_reward(nb);
    if(in_the_floor>0)
        return line_complete_reward(line,nb)+discard_reward(in_the_floor);
    if(in_the_floor==0)
        return line_complete_reward(line,nb);
    return non_complete_line_reward(line,nb,-in_the_floor);
}


void Heuristic::play_move(Board* board){

    // [color,nb] -> is it possible to have this ?
    bool possible_draw[NB_COLORS*NB_TILES_PER_COLOR];
    int arg_fact[NB_COLORS*NB_TILES_PER_COLOR];

    for(int i=0; i<NB_COLORS*NB_TILES_PER_COLOR; i++){
        possible_draw[i] = false;
        arg_fact[i] = -1;
    }

    for(int factory=0; factory<=NB_FACTORIES; factory++){
        for(int col=0; col<NB_COLORS; col++){
            if(board->getFactoryTile(factory,col)>0){
                possible_draw[col*NB_TILES_PER_COLOR+board->getFactoryTile(factory,col)-1] = true;
                arg_fact[col*NB_TILES_PER_COLOR+board->getFactoryTile(factory,col)-1] = factory;
            }
        }
    }
    double best_reward = -INFINITY;
    int best_col=-1;
    int best_factory=-1;
    int best_line=-1;
    for(int col=0; col<NB_COLORS; col++){
        for(int nb=1; nb<=NB_TILES_PER_COLOR; nb++){
            if(possible_draw[col*NB_TILES_PER_COLOR+nb-1]){
                double max_reward = -INFINITY;
                int max_line = WALL_HEIGHT;
                for(int line=0; line<WALL_HEIGHT; line++){
                    if(board->placeableTile(col,line)){
                        int in_the_floor = nb+board->getPatternLineNb(board->currentPlayer(),line) -line-1;
                        if(max_reward<reward(line,nb,in_the_floor)){
                            max_reward = reward(line,nb,in_the_floor);
                            max_line = line;
                        }
                    }
                    if(max_reward<reward(WALL_HEIGHT,nb,nb)){
                        max_reward = reward(line,nb,nb);
                        max_line = WALL_HEIGHT;
                    }
                }
                if(max_reward>best_reward){
                    best_col = col;
                    best_factory = arg_fact[col*NB_TILES_PER_COLOR+nb-1];
                    best_line = max_line;
                    best_reward = max_reward;
                }
            }

        }
    }
    assert(board->playable(best_factory,best_col,best_line));
    board->play(best_factory,best_col,best_line);
}

Heuristic::Heuristic(int preoptimize){
    switch(preoptimize){

    // optimize vs random (53-2 in average)
    case 0:
        par[0] = 0.06558;
        par[1] = 0.4635;
        par[2] = 0.2758;
        par[3] = -0.5508;
        par[4] = 0.03573;
        par[5] = -0.2471;
        par[6] = 0.1930;
        par[7] = -0.2121;
        par[8] = -0.5065;
    }
}

void Heuristic::optimize(Controller *opponent, int nb_test_game, int nb_evolve_game){
    int total_result;
    int best_result = -INFINITY; // = score(player)-score(opponent)
    double old_par[9];
    double best_par[9];

    Controller** players = new Controller*[NB_PLAYERS];
    players[0] = this;
    players[1] = opponent;

    for(int i=0; i<nb_evolve_game; i++){
        total_result=0;
        double squared_norm = 0;
        for(int j=0; j<9; j++){
            old_par[j] = par[j];
            par[j] += 10*min(i,1)*(doubleRandom()-0.5)/(i+10);
            squared_norm += par[j]*par[j];
        }
        double norm = sqrt(squared_norm);
        if(squared_norm>0){
            for(int j=0; j<9; j++){
                par[j] /= norm;
            }
        }

        for(int j=0; j<nb_test_game; j++){
            Board board;
            play_game(&board,players);
            total_result += board.getScore(0)-board.getScore(1);
        }

        if(total_result>best_result or doubleRandom()<exp(double(total_result-best_result)/nb_test_game)){
            best_result = total_result;
            for(int j=0; j<9; j++){
                best_par[j] = par[j];
            }
        }

        for(int j=0; j<9; j++){
            par[j] = best_par[j];
        }
    }
    for(int j=0; j<9; j++){
        cout<<par[j]<<endl;
    }
}



//////////////////// MINMAX ////////////////////


int MinMax::play_move(Board *board, byte depth){
    // ensure that we didn't run out of time and that the algorithm has time to compute first depth
    if(time_limited and max_depth>1 and chrono.lap()>time_limit)
        throw TimeOutException();

    // cannot happened if depth==0 (the end of the round would have been called)
    if(depth==max_depth or board->endOfTheRound()){
        byte player = board->currentPlayer();
        board->nextRound();
        board->addEndgameBonus();
        return board->getScore(player) - board->getScore(1-player);
    }

    int best_response = INT32_MIN;
    int response;
    byte best_factory=255, best_col, best_line;

    // when the depth is max_depth-1 we (almost) don't care about the factory
    if(depth==max_depth-1){

        bool already_done[NB_COLORS*NB_TILES_PER_COLOR];
        for(int i=0; i<NB_COLORS*NB_TILES_PER_COLOR; i++){
            already_done[i]=false;
        }

        for(byte factory=0; factory<=NB_FACTORIES; factory++){
            for(byte col=0; col<NB_COLORS; col++){
                if(board->pickableTile(factory,col) and !already_done[col*NB_TILES_PER_COLOR+board->getFactoryTile(factory,col)]){
                    already_done[col*NB_TILES_PER_COLOR+board->getFactoryTile(factory,col)] = true;
                    for(byte line=0; line<=WALL_HEIGHT; line++){
                        if(board->placeableTile(col,line)){
                            Board board_copy(*board);
                            board_copy.play(factory,col,line);
                            response = -play_move(&board_copy,depth+1);
                            if(response>best_response){
                                best_response = response;
                                best_factory = factory;
                                best_col = col;
                                best_line = line;
                            }
                        }
                    }
                }
            }
        }
    }
    else{
        for(byte factory=0; factory<=NB_FACTORIES; factory++){
            for(byte col=0; col<NB_COLORS; col++){
                if(board->pickableTile(factory,col)){
                    for(byte line=0; line<=WALL_HEIGHT; line++){
                        if(board->placeableTile(col,line)){
                            Board board_copy(*board);
                            board_copy.play(factory,col,line);
                            response = -play_move(&board_copy,depth+1);
                            if(response>best_response){
                                best_response = response;
                                best_factory = factory;
                                best_col = col;
                                best_line = line;
                            }
                        }
                    }
                }
            }
        }
    }
    assert(best_factory!=255);
    // choose the best move
    if(depth==0){
        choosen_factory = best_factory;
        choosen_color = best_col;
        choosen_line=best_line;
    }
    return best_response;
}

void MinMax::play_move(Board *board){
    if(time_limited){
    chrono.reset();
    try{
        for(max_depth=1; max_depth<=depth_limit; max_depth++){
            play_move(board,0);
        }
    }
    catch(TimeOutException&e){}
    }
    else{
        max_depth=depth_limit;
        play_move(board,0);
    }
    board->play(choosen_factory,choosen_color,choosen_line);
}


//////////////////// HUMAN ////////////////////

void Human::play_move(Board *board){
    byte factory;
    byte color;
    byte line;

    board->display();

    do{
        cout<<"factory ?"<<endl;
        cin>>factory;
        cout<<"color ?"<<endl;
        cin>>color;
    }while(board->pickableTile(factory,color));

    do{
        cout<<"line ?"<<endl;
        cin>>line;
    }while(board->placeableTile(color,line));

    board->play(factory,color,line);
}




void play_game(Board* board, Controller **players){
    while(!board->endOfTheGame()){
        board->nextRound();
        while(!board->endOfTheRound()){
            players[board->currentPlayer()]->play_move(board);
        }
    }
    board->nextRound();
    board->addEndgameBonus();
}
