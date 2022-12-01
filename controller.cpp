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

    //nb_placeable = max(1,nb_placeable-1); // do not play in the floor if he doesn't have to
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

    for(byte factory=0; factory<=NB_FACTORIES; factory++){
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

    // optimize vs random (94-8.5 in average)
    case 0:
        par[0] = 0.0053696;
        par[1] = 0.251656;
        par[2] = 0.144145;
        par[3] = 0.0812487;
        par[4] = 0.390732;
        par[5] = -0.165523;
        par[6] = -0.635363;
        par[7] = -0.519715;
        par[8] = -0.235377;
    }
}


// optmization of the parameters with the Monte-Carlo method
void Heuristic::optimize(Controller *opponent, int nb_test_game, int nb_evolve_game){
    int total_result;
    int best_result = -INFINITY; // = score(player)-score(opponent)
    double old_par[9];
    double best_par[9];

    Controller** players = new Controller*[NB_PLAYERS];
    players[0] = this;
    players[1] = opponent;
    Board board;
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
            board.init();
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

MinMax::MinMax(byte depth_limit_, bool time_limited_, double time_limit_){
    depth_limit = depth_limit_;
    time_limited = time_limited_;
    time_limit = time_limit_;
}



double MinMax::DFS(Board *board, byte depth, byte max_depth, double alpha, double beta){

    // ensure that we didn't run out of time and that the algorithm has time to compute first depth
    if(time_limited and max_depth>1 and chrono.lap()>time_limit)
        throw TimeOutException();

    // if the position has already been reached
    /*
    if(look_up_table.count(*board)>0){
        PositionValue pos_val(look_up_table.at(*board));
        if(pos_val.depth < max_depth-depth and pos_val.value >= alpha-tol){
            look_up_table.erase(*board);
            if(beta>=pos_val.value)
                alpha = max(alpha,pos_val.value);
        }
        else
            return pos_val.value;
    }
    */

    // cannot happened if depth==0 (the end of the round would have been called)
    if(board->endOfTheGame()){
        byte player = board->currentPlayer();
        board->addEndgameBonus();
        return board->getScore(player) - board->getScore(1-player);
    }

    if(depth==max_depth){
        byte player = board->currentPlayer();
        board->nextRound();
        board->addEndgameBonus();
        //look_up_table.insert({*board,PositionValue(board->getScore(player) - board->getScore(1-player),0)});
        return board->getScore(player) - board->getScore(1-player);
    }


    if(board->endOfTheRound()){
        // if the other player has to play the sign needs to change
        bool change_sign = board->currentPlayer()!=board->getTile1();
        double total_expected = 0.;
        for(int i=0; i<nb_expect; i++){
            Board board_copy(*board);
            board_copy.nextRound();
            total_expected += DFS(&board_copy,depth,max_depth,-beta,-alpha);
        }
        return (1-2*change_sign)*total_expected/nb_expect;
    }

    double best_response = -INFINITY;
    double response;
    byte best_factory=255, best_col, best_line;

    for(byte factory=0; factory<=NB_FACTORIES; factory++){
        for(byte col=0; col<NB_COLORS; col++){
            if(board->pickableTile(factory,col)){
                for(byte line=0; line<=WALL_HEIGHT; line++){
                    if(board->placeableTile(col,line)){
                        Board board_copy(*board);
                        board_copy.play(factory,col,line);
                        response = -DFS(&board_copy,depth+1,max_depth,-beta,-alpha);
                        if(response>best_response){
                            best_response=response;
                            if(depth==0){
                                best_factory = factory;
                                best_col = col;
                                best_line = line;
                            }
                            if(best_response>=beta)
                                return best_response;
                            alpha = max(alpha,best_response);
                        }
                    }
                }
            }
        }
    }
    // choose the best move
    if(depth==0){
        assert(best_factory!=255);
        choosen_factory = best_factory;
        choosen_color = best_col;
        choosen_line=best_line;
    }
    //look_up_table.insert({*board,PositionValue(best_response,max_depth-depth)});
    return best_response;
}

void MinMax::play_move(Board *board){
    look_up_table.clear();
    if(time_limited){
        chrono.reset();
        double alpha = -INFINITY;
        double beta = INFINITY;
        try{
            for(byte max_depth=1; max_depth<=depth_limit; max_depth++){
                DFS(board,0,max_depth,alpha,beta);
            }
        }
        catch(TimeOutException&e){}
    }
    else{
        DFS(board,0,depth_limit);
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
    board->nextRound();
    while(!board->endOfTheGame()){

        while(!board->endOfTheRound()){
            players[board->currentPlayer()]->play_move(board);
        }
        board->nextRound();
    }
    board->addEndgameBonus();
}
