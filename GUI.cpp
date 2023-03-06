#include "GUI.h"

GUI::GUI(vector<string> image_paths, int w, int h){
    window_width = w;
    window_height = h;
    images = new Image<AlphaColor>[2*image_paths.size()];
    loadImages(image_paths);
}

GUI::~GUI(){
    delete images;
}

void GUI::safeLoad(Image<AlphaColor> &image, string path){
    if(! load(image, path)) {
        cerr << "Error loading image file " << path << endl;
    }
}

void GUI::loadImages(vector<string> image_paths){
    for (unsigned int i = 0; i < image_paths.size(); i++){
        Image<AlphaColor> image;
        safeLoad(image,image_paths[i]);
        images[i] = image;
    }
}

void GUI::displayImage(int i, int x, int y, double fact){
    display(images[i], x, y,false,fact);
}

Window GUI::init(){
    return openWindow(window_width, window_height);
}

void GUI::displayTile(int j, int x, int y, double fact) {
    displayImage(5+j,x,y,fact);

    drawRect(x-1,y-1,TILE_SIDE+2,TILE_SIDE+2,BLACK);
    drawRect(x-2,y-2,TILE_SIDE+4,TILE_SIDE+4,WHITE);
    drawRect(x-3,y-3,TILE_SIDE+6,TILE_SIDE+6,BLACK);
}

void GUI::displayPlayerboards(Board *board){
    byte player = board->currentPlayer();
    displayPlayerboard(PLAYERBOARD_X0);
    displayPlayerboard(PLAYERBOARD_X0,WINDOW_HEIGHT/2);
    if (player == 0){
        drawRect(PLAYERBOARD_X0, 0, PLAYERBOARD_WIDTH, PLAYERBOARD_HEIGHT, HIGHLIGHT_SELECT_COLOR, HIGHLIGHT_PEN);
    }
    else{
        drawRect(PLAYERBOARD_X0, PLAYERBOARD_HEIGHT, PLAYERBOARD_WIDTH, PLAYERBOARD_HEIGHT, HIGHLIGHT_SELECT_COLOR, HIGHLIGHT_PEN);
    }

}

void GUI::displayFactories(bool circles){
    IntPoint2 unityroot = IntPoint2(1,0);
    if (circles){
        drawCircle(FACTORY_CENTER,FACTORY_RADIUS-FACTORY_SIDE/2,ORANGE);
        drawCircle(FACTORY_CENTER,FACTORY_RADIUS+FACTORY_SIDE/2,ORANGE);
        drawCircle(FACTORY_CENTER,FACTORY_RADIUS,BLACK);
    }
    for (int k = 0; k < NB_FACTORIES; k++){
        unityroot = IntPoint2(int(FACTORY_RADIUS*cos(- M_PI/2 + 2*M_PI*k/NB_FACTORIES)),int(FACTORY_RADIUS*sin(- M_PI/2 + 2*M_PI*k/NB_FACTORIES)));
        displayFactory(FACTORY_CENTER+unityroot-FACTORY_CENTERING);
    }
}

void GUI::displayFactory(byte factory){
    IntPoint2 unityroot = IntPoint2(int(FACTORY_RADIUS*cos(- M_PI/2 + 2*M_PI*factory/NB_FACTORIES)),int(FACTORY_RADIUS*sin(- M_PI/2 + 2*M_PI*factory/NB_FACTORIES)));
    displayFactory(FACTORY_CENTER+unityroot-FACTORY_CENTERING);
}

void GUI::displayWalls(Board *board){
    for (byte p = 0; p < NB_PLAYERS; p++){
        for (byte x = 0; x < NB_COLORS; x++){
            for (byte y = 0; y < NB_COLORS; y++){
                if (board->wallTileFilled(p, y, x)){
                    displayTile(wallColumnToColor(x,y),
                                PLAYERBOARD_X0 + WALL_X0 + x*WALL_SPACING,
                                p*PLAYERBOARD_HEIGHT + WALL_Y0 + y*WALL_SPACING);
                }
            }
        }
    }
}

void GUI::displayPatterns(Board *board){
    for(byte p = 0; p < NB_PLAYERS; p++){
        for (byte y = 0; y < WALL_HEIGHT; y++){
            for (byte x = WALL_HEIGHT - board->getPatternLineNb(p,y); x < WALL_HEIGHT; x++){
                displayTile(board->getPatternLineColor(p,y),
                            PLAYERBOARD_X0 + PATTERN_X0 + x*WALL_SPACING,
                            p*PLAYERBOARD_HEIGHT + PATTERN_Y0 + y*WALL_SPACING);
            }
        }
    }
}

void GUI::displayFloors(Board *board){
    for(byte player=0; player<NB_PLAYERS; player++){
        for(byte x=0; x<FLOOR_SIZE; x++){
            byte color = board->getFloorTile(player*FLOOR_SIZE+x);
            if(color == NB_COLORS+1)
                break;
            displayTile(color, PLAYERBOARD_X0 + FLOOR_X0 + x*FLOOR_SPACING, player*PLAYERBOARD_HEIGHT + FLOOR_Y0);
        }
    }
}

void GUI::displayFactoryTiles(byte factory, Board *board){
    IntPoint2 unityroot = IntPoint2(int(FACTORY_RADIUS*cos(- M_PI/2 + 2*M_PI*factory/NB_FACTORIES)),
                          int(FACTORY_RADIUS*sin(- M_PI/2 + 2*M_PI*factory/NB_FACTORIES)));
    IntPoint2 factory_p0 = FACTORY_CENTER+unityroot-FACTORY_CENTERING;

    byte tiles_to_place[NB_TILES_PER_FACTORY];
    board->fillFactoryTilesArray(tiles_to_place, factory);

    for (int x = -1; x < 1; x++){
        for (int y = -1; y < 1; y++){
            if (tiles_to_place[(1+x)*2+(1+y)] < 255){
                displayTile(tiles_to_place[(1+x)*2+(1+y)],
                            factory_p0.x() + FACTORY_SIDE/2 + x*(FACTORY_MARGIN+TILE_SIDE) + FACTORY_MARGIN/2,
                            factory_p0.y() + FACTORY_SIDE/2 + y*(FACTORY_MARGIN+TILE_SIDE) + FACTORY_MARGIN/2);
            }
        }
    }

}

void GUI::displayAllFactoryTiles(Board *board){
    for (byte factory = 0; factory < NB_FACTORIES; factory++){
        displayFactoryTiles(factory, board);
    }
}

void GUI::displayTileInfo(IntPoint2 P0, Color bg_color, byte tiles[NB_COLORS]){
    fillRect(P0, 5*WALL_SPACING + WALL_MARGIN, BAG_SIDE, bg_color);
    for (int x = 0; x < NB_COLORS; x++){
        if (tiles[x] > 0){
            displayTile(x, P0 + IntPoint2(WALL_MARGIN + x*WALL_SPACING,WALL_MARGIN/2));
            if (tiles[x] < 10){
                drawString(P0 + IntPoint2(WALL_MARGIN + x*WALL_SPACING + TILE_SIDE/2 - TEXT_SIZE/2,WALL_SPACING + int(2.5*WALL_MARGIN)), to_string(tiles[x]), BLACK, TEXT_SIZE);
            }
            else{
                drawString(P0 + IntPoint2(WALL_MARGIN + x*WALL_SPACING + TILE_SIDE/2 - TEXT_SIZE,WALL_SPACING + int(2.5*WALL_MARGIN)), to_string(tiles[x]), BLACK, TEXT_SIZE);
            }
        }
    }
}

void GUI::displayMiddleTiles(Board* board){
    byte tiles[NB_COLORS];
    for (byte color = 0; color < NB_COLORS; color++){
        tiles[color] = board->getFactoryTile(NB_FACTORIES, color);
    }
    displayTileInfo(MIDDLE_P0,MIDDLE_COLOR, tiles);
    if (board->getTile1() == NB_PLAYERS){
        displayTile(NB_COLORS, WINDOW_HEIGHT/2 - TILE_SIDE/2, WINDOW_HEIGHT/2 - TILE_SIDE/2);
    }
}

void GUI::displayBagContent(Board *board){
    byte tiles[NB_COLORS];
    for (byte color = 0; color < NB_COLORS; color++){
        tiles[color] = board->getBagTile(color);
    }
    displayTileInfo(BAG_CONTENT_P0, BAG_CONTENT_COLOR, tiles);
}

void GUI::displayDiscard(Board *board){
    byte tiles[NB_COLORS];
    for (byte color = 0; color < NB_COLORS; color++){
        tiles[color] = board->getDiscardedTile(color);
    }
    displayTileInfo(DISCARD_P0, DISCARD_COLOR, tiles);
}

void GUI::displayEvaluationBar(double wins, double draws, double losses){
    int x0 = WINDOW_WIDTH-PLAYERBOARD_WIDTH-EVALUATION_BAR_WIDTH-EVALUATION_BAR_MARGIN;
    fillRect(x0,(WINDOW_HEIGHT-EVALUATION_BAR_HEIGHT)/2,EVALUATION_BAR_WIDTH,losses*EVALUATION_BAR_HEIGHT,BLACK);
    fillRect(x0,(WINDOW_HEIGHT-EVALUATION_BAR_HEIGHT)/2+losses*EVALUATION_BAR_HEIGHT,EVALUATION_BAR_WIDTH,draws*EVALUATION_BAR_HEIGHT,Color(128,128,128));
    fillRect(x0,(WINDOW_HEIGHT-EVALUATION_BAR_HEIGHT)/2+(losses+draws)*EVALUATION_BAR_HEIGHT,EVALUATION_BAR_WIDTH,wins*EVALUATION_BAR_HEIGHT,WHITE);
    drawRect(x0,(WINDOW_HEIGHT-EVALUATION_BAR_HEIGHT)/2,EVALUATION_BAR_WIDTH,EVALUATION_BAR_HEIGHT,BLACK,2);
    int val = 100-int(100*(wins+0.5*draws)/(wins+draws+losses)+0.5); // +0.5 for the rounding
    string disp = ".";
    if(val<10)
        disp+="0";
    disp+=to_string(val);
    if(val==100)
        disp="1.";
    bool ex = val>=50;
    drawString(x0,(WINDOW_HEIGHT-EVALUATION_BAR_HEIGHT)/2+ex*losses*EVALUATION_BAR_HEIGHT+(1-ex)*((1-wins)*EVALUATION_BAR_HEIGHT+20),disp,RED,EVAL_BAR_TEXT_SIZE);
}


void GUI::displayGrid(){
    drawLine(WINDOW_HEIGHT/2, 0, WINDOW_HEIGHT/2, WINDOW_HEIGHT, BLACK);
    drawLine(0, WINDOW_HEIGHT/2, WINDOW_HEIGHT, WINDOW_HEIGHT/2, BLACK);
    drawLine(0, BAG_SIDE, WINDOW_HEIGHT, BAG_SIDE, BLACK);
    drawLine(0, WINDOW_HEIGHT - BAG_SIDE, WINDOW_HEIGHT, WINDOW_HEIGHT - BAG_SIDE, BLACK);
    drawLine(BAG_SIDE, 0, BAG_SIDE, WINDOW_HEIGHT, BLACK);
    drawLine(WINDOW_HEIGHT - BAG_SIDE, 0, WINDOW_HEIGHT - BAG_SIDE, WINDOW_HEIGHT, BLACK);
}

void GUI::displayBoardState(Board *board){
    displayPlayerboards(board);
    //displayBag(0, WINDOW_HEIGHT-BAG_SIDE);
    //displayGamebox(WINDOW_HEIGHT-BAG_SIDE, WINDOW_HEIGHT-BAG_SIDE);
    displayFactories();
    displayWalls(board);
    displayPatterns(board);
    displayFloors(board);
    displayAllFactoryTiles(board);
    displayMiddleTiles(board);
    displayBagContent(board);
    displayDiscard(board);
    displayScores(board);
}

void GUI::updateBoardState(Board *board){
    noRefreshBegin();
    clearWindow();
    displayBoardState(board);
    noRefreshEnd();
}

void GUI::displayScores(Board *board){
    int score0 = board->getScore(0);
    int score1 = board->getScore(1);
    drawString(WINDOW_WIDTH-0.858*PLAYERBOARD_WIDTH-TEXT_SIZE/2*(1+(score0>=10)+(score0>=100)),PLAYERBOARD_HEIGHT*0.2,to_string(score0),BLACK,TEXT_SIZE);
    drawString(WINDOW_WIDTH-0.858*PLAYERBOARD_WIDTH-TEXT_SIZE/2*(1+(score1>=10)+(score1>=100)),PLAYERBOARD_HEIGHT*1.2,to_string(score1),BLACK,TEXT_SIZE);
}

// set of moves are separated by moves = {255,255,255}
void GUI::displayBestMoves(vector<Move> moves,vector<double> valuations){
    IntPoint2 corner = MIDDLE_P0 + IntPoint2(5*WALL_SPACING + WALL_MARGIN,0);
    fillRect(corner,WINDOW_WIDTH-PLAYERBOARD_WIDTH-corner.x(),WINDOW_HEIGHT-corner.y(),WHITE);
    size_t move_index = 0;
    for(size_t i=0; i<valuations.size(); i++){
        int val = 100-int(100*valuations[i]); // +0.5 for the rounding
        string disp = ".";
        if(val<10)
            disp+="0";
        disp+=to_string(val);
        if(val==100)
            disp="1.";
        int nb_moves=0;
        while(moves[move_index].factory!=255) {
            if(nb_moves==7) disp += "...";
            if(nb_moves<7) disp += " " + moves[move_index].acronym();
            move_index++;
            nb_moves++;
        }
        move_index++;
        drawString(0.4*WINDOW_WIDTH, (0.9+0.02*i)*WINDOW_HEIGHT,disp,RED,BEST_MOVES_TEXT_SIZE);
    }
}
