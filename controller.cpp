#include "controller.h"


struct TimeOutException{};

//////////////////// RANDOM ////////////////////


Move Random::play_move(Board* board, bool play){
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
            if(play)
                board->play(factory_choice,color_choice,line);
            return Move(factory_choice,color_choice,line);
        }
    }
    return Move(-1,-1,-1); // not normal
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


Move Heuristic::play_move(Board* board, bool play){

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
    if(play)
        board->play(best_factory,best_col,best_line);
    return Move(best_factory,best_col,best_line);
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

    double response;
    byte best_factory=255, best_col, best_line;
    double best_response = -INFINITY;

    if(depth==0){
        Board board_copy(*board);
        Move m = heuristic.play_move(&board_copy);
        best_response = -DFS(&board_copy,depth+1,max_depth,-beta,-alpha);
        alpha = best_response;
        best_factory = m.factory;
        best_col = m.col;
        best_line = m.line;
    }

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
        // takes the average value of the expected outcome
        for(int i=0; i<nb_expect; i++){
            Board board_copy(*board);
            board_copy.nextRound();
            if(board_copy.endOfTheGame()){
                byte player = board_copy.currentPlayer();
                board_copy.addEndgameBonus();
                // exagerate the result so if it is a win he takes it
                // and if it is a loss he tries to force a next round
                return 1000*(1-2*change_sign)*(board_copy.getScore(player) - board_copy.getScore(1-player));
            }

            total_expected += DFS(&board_copy,depth,max_depth,-beta,-alpha);
        }
        return (1-2*change_sign)*total_expected/nb_expect;
    }


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
        next_move.factory = best_factory;
        next_move.col = best_col;
        next_move.line = best_line;
    }
    //look_up_table.insert({*board,PositionValue(best_response,max_depth-depth)});
    return best_response;
}

Move MinMax::play_move(Board* board, bool play){
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
    if(play)
        board->play(next_move);
    return next_move;
}


//////////////////// HUMAN ////////////////////

double euclidianNorm(IntPoint2 a){
    return sqrt(pow(a.x(),2) + pow(a.y(),2));
}

double euclidianDistance(IntPoint2 a, IntPoint2 b){
    return euclidianNorm(a-b);
}

bool computeFactoryCardinal(Board *board, byte factory){
    int total = 0;
    for (byte color = 0; color < NB_COLORS; color++){
        total += board->getFactoryTile(factory,color);
    }
    return total;
}

bool isEmptyFactory(Board *board, byte factory){
    return computeFactoryCardinal(board, factory) == 0;
}

void Human::clickPickableTile(Board *board, byte &factory, byte &color){
    int x = 0, y = 0;
    IntPoint2 clickPoint(x,y);
    factory = 255;
    color = 255;
    IntPoint2 unityroot(0,0);
    IntPoint2 factory_p0(0,0);
    while (factory == 255){
        getMouse(x,y);
        clickPoint = IntPoint2(x,y);
        byte factoryIndex = 0;
        while (factoryIndex < NB_FACTORIES and factory == 255){
            unityroot = IntPoint2(int(FACTORY_RADIUS*cos(- M_PI/2 + 2*M_PI*factoryIndex/NB_FACTORIES)),
                                  int(FACTORY_RADIUS*sin(- M_PI/2 + 2*M_PI*factoryIndex/NB_FACTORIES)));
            factory_p0 = FACTORY_CENTER+unityroot;
            if (euclidianDistance(clickPoint, factory_p0) < FACTORY_SIDE/2 and not isEmptyFactory(board, factoryIndex)){
                drawCircle(factory_p0,FACTORY_SIDE/2 + HIGHLIGHT_PEN, GREEN, HIGHLIGHT_PEN);
                factory = factoryIndex;
            }
            factoryIndex++;
        }
        if (factoryIndex == NB_FACTORIES){
            if (x > MIDDLE_P0.x() + WALL_MARGIN
            and x < MIDDLE_P0.x() + WALL_MARGIN + NB_COLORS*WALL_SPACING
            and y > MIDDLE_P0.y()){
                drawRect(MIDDLE_P0 - IntPoint2(HIGHLIGHT_PEN, HIGHLIGHT_PEN), 5*WALL_SPACING + WALL_MARGIN + 2*HIGHLIGHT_PEN, BAG_SIDE+2*HIGHLIGHT_PEN, GREEN, HIGHLIGHT_PEN);
                factory = factoryIndex;
            }
        }
    }
    if (factory < NB_FACTORIES){
        while(color == 255){
            byte factory_tiles[NB_TILES_PER_FACTORY];
            board->fillFactoryTilesArray(factory_tiles, factory);
            IntPoint2 relativeClickedPoint = factory_p0 - clickPoint;
            int xTile = (relativeClickedPoint.x() < 0)? 0 : -1;
            int yTile = (relativeClickedPoint.y() < 0)? 0 : -1;
            if (factory_tiles[(1+xTile)*2+(1+yTile)] < 255){
                color = factory_tiles[(1+xTile)*2+(1+yTile)];
                for (int xHighlight = -1; xHighlight < 1; xHighlight++){
                    for (int yHighlight = -1; yHighlight < 1; yHighlight++){
                        if (factory_tiles[(1+xHighlight)*2+(1+yHighlight)] == color){
                            drawRect((factory_p0-FACTORY_CENTERING).x() + FACTORY_SIDE/2 + xHighlight*(FACTORY_MARGIN+TILE_SIDE) + FACTORY_MARGIN/2 - HIGHLIGHT_PEN,
                                     (factory_p0-FACTORY_CENTERING).y() + FACTORY_SIDE/2 + yHighlight*(FACTORY_MARGIN+TILE_SIDE) + FACTORY_MARGIN/2 - HIGHLIGHT_PEN,
                                     TILE_SIDE + 2*HIGHLIGHT_PEN, TILE_SIDE + 2*HIGHLIGHT_PEN, GREEN, HIGHLIGHT_PEN);
                        }
                    }
                }
            }
        }
    }
    else if (factory == NB_FACTORIES){
        int xMiddle = (x - MIDDLE_P0.x() - WALL_MARGIN)/WALL_SPACING;
        drawRect(MIDDLE_P0 + IntPoint2(WALL_MARGIN + xMiddle*WALL_SPACING - HIGHLIGHT_PEN,WALL_MARGIN/2 - HIGHLIGHT_PEN), TILE_SIDE +2*HIGHLIGHT_PEN, TILE_SIDE+2*HIGHLIGHT_PEN, GREEN, HIGHLIGHT_PEN);
        color = byte(xMiddle);
    }

}

void Human::clickPlaceableTile(Board *board, byte &line){
    int x = 0, y = 0;
    line = 255;
    while (line == 255){
        getMouse(x,y);
        int p = board->currentPlayer();
        if (x > PLAYERBOARD_X0 + PATTERN_X0 and x < PLAYERBOARD_X0 + PATTERN_X0 + WALL_HEIGHT*WALL_SPACING
        and y > p*PLAYERBOARD_HEIGHT + PATTERN_Y0 and y < p*PLAYERBOARD_HEIGHT + PATTERN_Y0 + WALL_HEIGHT*WALL_SPACING){
            int yPattern = (y - p*PLAYERBOARD_HEIGHT - PATTERN_Y0)/WALL_SPACING;
//            for (int xPattern = WALL_HEIGHT - yPattern - 1; xPattern < WALL_HEIGHT; xPattern++){
//                drawRect(PLAYERBOARD_X0 + PATTERN_X0 + xPattern*WALL_SPACING - HIGHLIGHT_PEN,
//                         p*PLAYERBOARD_HEIGHT + PATTERN_Y0 + yPattern*WALL_SPACING - HIGHLIGHT_PEN,
//                         TILE_SIDE+2*HIGHLIGHT_PEN, TILE_SIDE+2*HIGHLIGHT_PEN, GREEN, HIGHLIGHT_PEN);
//            }
            line = byte(yPattern);
        }
    }
}

Move Human::play_move(Board *board, bool play){
    byte factory;
    byte color;
    byte line;

    do{
        clickPickableTile(board, factory, color);
    }while(!board->pickableTile(factory,color));

    do{
        clickPlaceableTile(board, line);
    }while(!board->placeableTile(color,line));
    if(play)
        board->play(factory,color,line);
    return Move(factory,color,line);
}

void play_game(Board* board, Controller **players){
    board->nextRound();
    board->random_first_player();
    while(!board->endOfTheGame()){

        while(!board->endOfTheRound()){
            players[board->currentPlayer()]->play_move(board);
        }
        board->nextRound();
    }
    board->addEndgameBonus();
}

void playGameGraphics(Board* board, Controller **players, GUI gui){
    gui.displayBoardState(board);
    click();
    board->nextRound();
    while(!board->endOfTheGame()){

        noRefreshBegin();
        clearWindow();
        gui.displayBoardState(board);
        noRefreshEnd();
        //milliSleep(500);
        //click();

        while(!board->endOfTheRound()){
            players[board->currentPlayer()]->play_move(board);

            noRefreshBegin();
            clearWindow();
            gui.displayBoardState(board);
            noRefreshEnd();
            milliSleep(500);

        }
        click();
        board->nextRound();
    }

    noRefreshBegin();
    clearWindow();
    gui.displayBoardState(board);
    noRefreshEnd();

    board->addEndgameBonus();
}
