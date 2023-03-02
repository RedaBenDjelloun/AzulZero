#include "controller.h"


struct TimeOutException{};

//////////////////// RANDOM ////////////////////


Move Random::choose_move(Board* board){
    int nb_choices = 0;
    for(byte color=0; color<NB_COLORS; color++){
        for(byte line=0; line<=WALL_HEIGHT; line++){
            if(board->placeableTile(color,line)){
                for(byte factory=0; factory<=NB_FACTORIES; factory++){
                    nb_choices += board->pickableTile(factory,color);
                }
            }
        }
    }

    int choice = rand()%nb_choices;
    int n=0;

    for(byte line=WALL_HEIGHT; line!=255; line--){
        for(byte color=0; color<NB_COLORS; color++){
            if(board->placeableTile(color,line)){
                for(byte factory=NB_FACTORIES; factory!=255; factory--){
                    n += board->pickableTile(factory,color);
                    if(n>choice){
                        return Move(factory,color,line);
                    }
                }
            }
        }
    }
    assert(false);
    return Move(255,255,255); // not normal
}


//////////////////// PseudoRandom ////////////////////


Move PseudoRandom::choose_move(Board* board){
    double nb_choices = 0;
    for(byte factory=0; factory<=NB_FACTORIES; factory++){
        for(byte color=0; color<NB_COLORS; color++){
            if(board->pickableTile(factory,color)){
                for(byte line=0; line<=WALL_HEIGHT; line++){
                    nb_choices += (1+coeff*(line!=WALL_HEIGHT))*board->placeableTile(color,line);
                }
            }
        }
    }

    double choice = doubleRandom()*nb_choices;
    double n=0;

    for(byte factory=0; factory<=NB_FACTORIES; factory++){
        for(byte color=0; color<NB_COLORS; color++){
            if(board->pickableTile(factory,color)){
                for(byte line=0; line<=WALL_HEIGHT; line++){
                    n += (1+coeff*(line!=WALL_HEIGHT))*board->placeableTile(color,line);
                    if(n>=choice){
                        return Move(factory,color,line);
                    }
                }
            }
        }
    }
    assert(false);
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


Move Heuristic::choose_move(Board* board){

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
    return Move(best_factory,best_col,best_line);
}

Heuristic::Heuristic(int preoptimize){
    switch(preoptimize){

    // optimized vs random (94-8.5 in average)
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
    int best_result = -INT32_MAX; // = score(player)-score(opponent)
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
        Move m = next_move;
        board_copy.play(m);
        best_response = -DFS(&board_copy,depth+1,max_depth,-beta,-alpha);
        alpha = best_response;
        best_factory = m.factory;
        best_col = m.col;
        best_line = m.line;
    }

    if(depth==max_depth){
        byte player = board->currentPlayer();
        board->nextRound();
        board->addEndgameBonus();
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
            if(i==0 and board_copy.endOfTheGame()){
                byte player = board_copy.currentPlayer();
                board_copy.addEndgameBonus();
                // exagerate the result so if it is a win he takes it
                // and if it is a loss he tries to force a next round to have a chance to win
                return 1000*(1-2*change_sign)*(board_copy.getScore(player) - board_copy.getScore(1-player));
            }
            total_expected += DFS(&board_copy,depth,max_depth);
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
        valuation = best_response;
    }
    return best_response;
}

Move MinMax::choose_move(Board* board){
    next_move = heuristic.choose_move(board);
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

IntPoint2 computeFactoryCenter(byte factoryIndex){
    IntPoint2 unityroot = IntPoint2(int(FACTORY_RADIUS*cos(- M_PI/2 + 2*M_PI*factoryIndex/NB_FACTORIES)),
                                    int(FACTORY_RADIUS*sin(- M_PI/2 + 2*M_PI*factoryIndex/NB_FACTORIES)));
    return FACTORY_CENTER + unityroot;
}

void highlightFactory(byte factory){
    IntPoint2 factoryCenter = computeFactoryCenter(factory);
    drawCircle(factoryCenter,FACTORY_SIDE/2 + HIGHLIGHT_PEN, GREEN, HIGHLIGHT_PEN);
}

void highlightMiddle(){
    drawRect(MIDDLE_P0 - IntPoint2(HIGHLIGHT_PEN, HIGHLIGHT_PEN), 5*WALL_SPACING + WALL_MARGIN + 2*HIGHLIGHT_PEN, BAG_SIDE+2*HIGHLIGHT_PEN, GREEN, HIGHLIGHT_PEN);
}

void highlightMiddleTile(byte tile){
    drawRect(MIDDLE_P0 + IntPoint2(WALL_MARGIN + tile*WALL_SPACING - HIGHLIGHT_PEN,WALL_MARGIN/2 - HIGHLIGHT_PEN), TILE_SIDE +2*HIGHLIGHT_PEN, TILE_SIDE+2*HIGHLIGHT_PEN, GREEN, HIGHLIGHT_PEN);
}

void highlightFactoryTilesOfColor(byte factory, byte factoryTiles[NB_TILES_PER_FACTORY], byte color){
    IntPoint2 factoryCenter = computeFactoryCenter(factory);
    for (int xHighlight = -1; xHighlight < 1; xHighlight++){
        for (int yHighlight = -1; yHighlight < 1; yHighlight++){
            if (factoryTiles[(1+xHighlight)*2+(1+yHighlight)] == color){
                drawRect((factoryCenter-FACTORY_CENTERING).x() + FACTORY_SIDE/2 + xHighlight*(FACTORY_MARGIN+TILE_SIDE) + FACTORY_MARGIN/2 - HIGHLIGHT_PEN,
                         (factoryCenter-FACTORY_CENTERING).y() + FACTORY_SIDE/2 + yHighlight*(FACTORY_MARGIN+TILE_SIDE) + FACTORY_MARGIN/2 - HIGHLIGHT_PEN,
                         TILE_SIDE + 2*HIGHLIGHT_PEN, TILE_SIDE + 2*HIGHLIGHT_PEN, GREEN, HIGHLIGHT_PEN);
            }
        }
    }
}

void highlightPatternLine(byte player, byte line){ // Too slow to use yet
    if(line<WALL_WIDTH){
        for (int xPattern = WALL_HEIGHT - line- 1; xPattern < WALL_HEIGHT; xPattern++){
            drawRect(PLAYERBOARD_X0 + PATTERN_X0 + xPattern*WALL_SPACING - HIGHLIGHT_PEN,
                     player*PLAYERBOARD_HEIGHT + PATTERN_Y0 + line*WALL_SPACING - HIGHLIGHT_PEN,
                     TILE_SIDE+2*HIGHLIGHT_PEN, TILE_SIDE+2*HIGHLIGHT_PEN, GREEN, HIGHLIGHT_PEN);
        }
    }
    else{
        for(int i=0; i<FLOOR_SIZE; i++){
            drawRect(PLAYERBOARD_X0 + FLOOR_X0 + i*FLOOR_SPACING - HIGHLIGHT_PEN,
                     player*PLAYERBOARD_HEIGHT + FLOOR_Y0 - HIGHLIGHT_PEN,
                     TILE_SIDE+2*HIGHLIGHT_PEN, TILE_SIDE+2*HIGHLIGHT_PEN, GREEN, HIGHLIGHT_PEN);
        }
    }
}

void highlighFloor(){ // Too slow to use yet
    drawRect(PLAYERBOARD_X0 + FLOOR_X0 - FLOOR_MARGIN/4 - HIGHLIGHT_GAP, FLOOR_Y0 - HIGHLIGHT_GAP, FLOOR_SIZE*FLOOR_SPACING + FLOOR_MARGIN/2 - TILE_SIDE/2 + 2*HIGHLIGHT_GAP, TILE_SIDE + FLOOR_MARGIN/4 + 2*HIGHLIGHT_GAP, GREEN, 4);
}

bool isInsideBox(IntPoint2 point, int x, int y, int w, int h){
    return (point.x() >= x and point.x() < x + w and
            point.y() >= y and point.y() < y + h);
}

byte findClickedFactory(Board *board, IntPoint2 clickedPoint){
    IntPoint2 factoryCenter(0,0);
    byte factory = 0;
    // Examine all factories in factory circle
    while (factory < NB_FACTORIES){
        factoryCenter = computeFactoryCenter(factory);
        if (euclidianDistance(clickedPoint, factoryCenter) < FACTORY_SIDE/2 and not isEmptyFactory(board, factory)){
            highlightFactory(factory);
            return factory;
        }
        factory++;
    }
    // Examine middle
    if (factory == NB_FACTORIES){
        if (isInsideBox(clickedPoint, MIDDLE_P0.x(), MIDDLE_P0.y(), WALL_MARGIN + NB_COLORS*WALL_SPACING, BAG_SIDE) and not isEmptyFactory(board, factory)){
            return factory;
        }
    }
    // not found
    return 255;
}

byte findClickedTileColor(Board *board, byte factory, IntPoint2 clickedPoint){
    byte color = 0;
    // Examine all factories in factory circle
    if (factory < NB_FACTORIES){
        IntPoint2 factoryCenter = computeFactoryCenter(factory);
        byte factoryTiles[NB_TILES_PER_FACTORY];
        board->fillFactoryTilesArray(factoryTiles, factory);
        int xTile = (factoryCenter.x() < clickedPoint.x())? 1 : 0;
        int yTile = (factoryCenter.y() < clickedPoint.y())? 1 : 0;
        if (factoryTiles[xTile*2 + yTile] < 255){
            color = factoryTiles[xTile*2 + yTile];
            highlightFactoryTilesOfColor(factory, factoryTiles, color);
            return color;
        }
    }
    // Examine center
    else if (factory == NB_FACTORIES){
        byte xMiddle = byte((clickedPoint.x() - MIDDLE_P0.x() - WALL_MARGIN)/WALL_SPACING);
        if (board->getFactoryTile(NB_FACTORIES, xMiddle) > 0){
            highlightMiddle();
            highlightMiddleTile(xMiddle);
            color = byte(xMiddle);
            return color;
        }
    }
    // not found
    return 255;
}

void  Human::clickPickableTile(Board *board, byte &factory, byte &color){
    factory = 255;
    color = 255;
    int x = 0, y = 0;
    IntPoint2 clickedPoint(x,y);
    while (factory == 255){
        getMouse(x,y);
        clickedPoint = IntPoint2(x,y);
        factory = findClickedFactory(board, clickedPoint);
    }
    color = findClickedTileColor(board, factory, clickedPoint);
}

bool Human::clickPlaceableTile(Board *board, byte &line){
    int x = 0, y = 0;
    line = 255;
    IntPoint2 clickedPoint(x,y);
    getMouse(x,y);
    clickedPoint = IntPoint2(x,y);
    byte player = board->currentPlayer();
    if (isInsideBox(clickedPoint, PLAYERBOARD_X0 + PATTERN_X0, player*PLAYERBOARD_HEIGHT + PATTERN_Y0,
                    WALL_HEIGHT*WALL_SPACING,  WALL_HEIGHT*WALL_SPACING)){
        line = byte((y -  player*PLAYERBOARD_HEIGHT - PATTERN_Y0)/WALL_SPACING);
        return true;
    }
    else if(isInsideBox(clickedPoint, PLAYERBOARD_X0 + FLOOR_X0 - FLOOR_MARGIN/4, player*PLAYERBOARD_HEIGHT + FLOOR_Y0,
                        FLOOR_SIZE*FLOOR_SPACING + FLOOR_MARGIN/2 - TILE_SIDE/2, TILE_SIDE + FLOOR_MARGIN/4)){
        line = WALL_HEIGHT;
        return true;
    }
    else{
        return false;
    }
}

Move Human::choose_move(Board *board){
    byte factory = 255;
    byte color = 255;
    byte line = 255;
    bool foundTile = true;
    bool isPlaceableTile = true;
    do{
        do{
            if (foundTile == false or isPlaceableTile == false){
                gui->updateBoardState(board);
            }
            clickPickableTile(board, factory, color);
        }while(!board->pickableTile(factory,color));
        foundTile = clickPlaceableTile(board, line);
        isPlaceableTile = foundTile and board->placeableTile(color,line);
    } while (not foundTile or not isPlaceableTile);
    return Move(factory,color,line);
}


//////////////////// MCTS ////////////////////


MCTS::MCTS(double coeff){
    random_players = new Controller*[NB_PLAYERS];
    random_players[0] = new PseudoRandom(coeff);
    random_players[1] = new PseudoRandom(coeff);
}

MCTS::~MCTS(){
    delete random_players[0];
    delete random_players[1];
    delete random_players;
}

State MCTS::add_nodes(Board* board,Tree<MCNode> *tree){
    assert(!board->endOfTheRound());
    assert(tree->isLeaf());
    State delta_state;
    for(int factory=0; factory<=NB_FACTORIES; factory++){
        for(int col=0; col<NB_COLORS; col++){
            if(board->pickableTile(factory,col)){
                for(int line=0; line<=WALL_HEIGHT; line++){
                    if(board->placeableTile(col,line)){
                        tree->addAsLastChild(new Tree<MCNode>(MCNode(Move(factory,col,line))));
                        Board board_copy(*board);
                        board_copy.play(factory,col,line);
                        byte player = board->currentPlayer();
                        play_game(&board_copy,random_players);
                        tree->getLastChild()->getDataRef().s.update(player,board_copy.getScore(0),board_copy.getScore(1));
                        delta_state += tree->getLastChild()->getDataRef().s;
                    }
                }
            }
        }
    }
    return delta_state;
}


State MCTS::tree_search(Board* board,Tree<MCNode>* tree){
    if(tree->isLeaf()){
        // expansion
        if(!board->endOfTheRound()){
            State delta_state(add_nodes(board,tree).inverse());
            tree->getDataRef().s += delta_state;
            return delta_state;
        }
        // simulation
        else{
            byte player = 1-board->currentPlayer();
            play_game(board,random_players);
            State delta_state;
            delta_state.update(player,board->getScore(0),board->getScore(1));
            tree->getDataRef().s += delta_state;
            return delta_state;
        }
    }
    else{
        // selection
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
        board->play(tree->getChild(child_index)->getData().move);
        State delta_state = tree_search(board,child).inverse();
        tree->getDataRef().s += delta_state;
        return delta_state;
    }
}


Move MCTS::choose_move(Board *board){
    assert(time_limited or nb_simul_limited);
    chrono.reset();
    MCNode root;
    Tree<MCNode> tree(root);
    Board board1(*board);
    board1.addBonusToAll(50);
    while((chrono.lap()<time_limit or !time_limited) and (tree.getData().N()<nb_max_simul or !nb_simul_limited)){
        Board board_copy(board1);
        tree_search(&board_copy,&tree);
    }
    // selection
    double best_value = -INFINITY;
    int child_index = -1;
    for(int i=0; i<tree.nbChildren(); i++){
        if(best_value < tree.getChild(i)->getData().value()){
            best_value = tree.getChild(i)->getData().value();
            child_index = i;
        }
    }
    return tree.getChild(child_index)->getData().move;
}


void play_game(Board* board, Controller **players){
    while(!board->endOfTheGame()){
        while(!board->endOfTheRound()){
            board->play(players[board->currentPlayer()]->choose_move(board));
        }
        board->nextRound();
    }
    board->addEndgameBonus();
}
