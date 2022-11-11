#include "controller.h"




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


double Heuristic::reward(int line, int nb, int in_the_floor){
    if(line==WALL_HEIGHT)
        return discard_reward(nb);
    else if(in_the_floor>=0)
        return line_complete_reward(line,nb)+discard_reward(max(0,in_the_floor));
    else
        return non_complete_line_reward(line,nb);
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
            if(board->factories[factory*NB_COLORS+col]>0){
                possible_draw[col*NB_TILES_PER_COLOR+board->factories[factory*NB_COLORS+col]-1] = true;
                arg_fact[col*NB_TILES_PER_COLOR+board->factories[factory*NB_COLORS+col]-1] = factory;
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
                        int in_the_floor = (nb+board->pattern_lines[board->current_player*2*WALL_HEIGHT + line*2]-line-1);
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

    // best score(player)-score(opponent) vs random
    case 0:
        par[0] = 0.990938;
        par[1] = 3.58249;
        par[2] = 2.18165;
        par[3] = -2.9019;
        par[4] = 1.33669;
        par[5] = -2.5848;
        par[6] = -3.2952;
        par[7] = -3.16817;

    // best winrate vs random (but an awful avg score)
    case 1:
        par[0] = -3.54123;
        par[1] = -0.238941;
        par[2] = -2.58024;
        par[3] = 1.84378;
        par[4] = 2.85989;
        par[5] = -2.76286;
        par[6] = -1.99341;
        par[7] = -4.08354;
    }

}

void Heuristic::optimize(Controller *opponent){
    int total_result;
    int best_result = -INFINITY; // = score(player)-score(opponent)
    double old_par[8];
    double best_par[8];

    Controller** players = new Controller*[NB_PLAYERS];
    players[0] = this;
    players[1] = opponent;

    for(int i=0; i<100; i++){
        total_result=0;
        for(int j=0; j<8; j++){
            old_par[j] = par[j];
            par[j] += min(i,1)*10*(doubleRandom()-0.5)/(i+10);
        }

        for(int j=0; j<100; j++){
            Board board;

            play_game(&board,players);
            total_result += board.scores[0]-board.scores[1];
        }

        if(total_result>best_result or doubleRandom()<exp(double(total_result-best_result)/50)){
            best_result = total_result;
            for(int j=0; j<8; j++){
                best_par[j] = par[j];
            }
        }

        for(int j=0; j<8; j++){
            par[j] = best_par[j];
        }
    }
    /*
    for(int j=0; j<8; j++){
        cout<<best_par[j]<<endl;
    }
    */
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
