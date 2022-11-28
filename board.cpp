#include <Imagine/Graphics.h>
using namespace Imagine;
#include "board.h"
#include <assert.h>
using namespace std;


byte wallColumnToColor(byte column, byte line){
    return (column - line + NB_COLORS) % NB_COLORS;
}


byte wallColorToColumn(byte color, byte line){
    return (line + color) % NB_COLORS;
}


void Board::init(){

    current_player = 0;
    tile1 = NB_PLAYERS;

    for(byte player=0; player<NB_PLAYERS; player++){
        scores[player] = 0;
    }

    for(byte i=0; i<NB_COLORS; i++){
        bag[i] = NB_TILES_PER_COLOR;
    }

    for(byte i=0; i<NB_COLORS; i++){
        discard[i] = 0; // no tiles discarded yet
    }

    for(byte i=0; i<(NB_FACTORIES+1)*NB_COLORS; i++){
        factories[i] = 0;   // factories are empty
    }

    for(byte i=0; i<NB_PLAYERS*WALL_HEIGHT*2; i++){
        pattern_lines[i] = 0; // pattern lines are empty
    }

    for(byte i=0; i<NB_PLAYERS*FLOOR_SIZE; i++){
        floor_lines[i] = NB_COLORS+1; // there is no tile
    }

    for(byte i=0; i<NB_PLAYERS*WALL_SIZE; i++){
        walls[i] = false;   // the wall is empty
    }
}


byte Board::nbFloorTiles() const{
    for(int pos=0; pos<FLOOR_SIZE; pos++){
        if(getFloorTile(pos)==NB_COLORS+1)
            return pos+1;
    }
    return FLOOR_SIZE;
}


bool Board::endOfTheRound() const{
    for(byte i=0; i<(NB_FACTORIES+1)*NB_COLORS; i++){
        if(factories[i]!=0)
            return false;
    }
    return true;
}


bool Board::endOfTheGame() const{

    for(byte player=0; player<NB_PLAYERS; player++){
        for(byte line=0; line<WALL_HEIGHT; line++){
            for(byte column=0; column<WALL_WIDTH; column++){
                if(!walls[player*WALL_SIZE + line*WALL_WIDTH + column])
                    break;
                else if(column == WALL_WIDTH-1)
                    return true;
            }
        }
    }
    return false;
}


void Board::nextRound(){
    updateWall();
    updateFloor();
    updateFactories();

    // checks if we are in the first round
    if(tile1 != NB_PLAYERS)
        current_player = tile1;
    tile1 = NB_PLAYERS;
}


void Board::updateWall(){

    // for each player
    for(byte player=0; player<NB_PLAYERS; player++){

        // update wall
        for(byte line=0; line<WALL_HEIGHT; line++){
            // if the pattern line is complete
            if(pattern_lines[player*2*WALL_HEIGHT+line*2] == line+1){

                byte color = pattern_lines[player*2*WALL_HEIGHT+line*2+1];
                byte column = wallColorToColumn(color, line);

                // clear the tiles on the pattern line
                pattern_lines[player*2*WALL_HEIGHT+line*2] = 0;

                // add the tile to the wall
                addTileToWall(player, line, column);

                // discard the rest of the tiles
                discard[color] += line;
            }
        }
    }
}


void Board::updateFloor(){
    // for each player
    for(byte player=0; player<NB_PLAYERS; player++){
        // update floor
        for(byte i=0; i<FLOOR_SIZE; i++){
            byte color = floor_lines[player*FLOOR_SIZE+i];
            // if there is no tile: stop
            if(color == NB_COLORS+1)
                break;
            // clear the tile
            floor_lines[player*FLOOR_SIZE+i]=NB_COLORS+1;
            // add the malus
            addMalus(FLOOR[i],player);

            // discard the tile
            if(color != NB_COLORS)
                discard[color] ++;
        }
    }
}


void Board::updateFactories(){

    byte tiles_remaining = 0;
    for(byte col=0; col<NB_COLORS; col++){
        tiles_remaining += bag[col];
    }

    // for each factory (except the center of the table)
    for(byte factory=0; factory<NB_FACTORIES; factory++){
        for(byte tile=0; tile<NB_TILES_PER_FACTORY; tile++){
            if(tiles_remaining == 0){
                refillBag();
                for(byte col=0; col<NB_COLORS; col++){
                    tiles_remaining += bag[col];
                }
            }
            // choose a random tile
            byte col = chooseRandomTile(tiles_remaining);
            // remove the color from the bag
            bag[col] --;
            tiles_remaining --;
            // add the tile to the factory
            factories[factory*NB_COLORS + col] ++;
        }
    }
}


byte Board::chooseRandomTile(byte tiles_remaining){
    assert(tiles_remaining>0);
    byte random = rand()%tiles_remaining;
    byte nb_tiles = 0;
    for (byte col=0; col<NB_COLORS; col++){
        nb_tiles += bag[col];
        if(nb_tiles>random){
            return col;
        }
    }
    return -1; // not normal
}


void Board::refillBag(){
    for(byte col=0; col<NB_COLORS; col++){
        bag[col] = discard[col];
        discard[col] = 0;
    }
}


void Board::addTileToWall(byte player, byte line, byte column){

    walls[player*WALL_SIZE+line*WALL_WIDTH+column] = true;

    byte horizontal_points = 1;
    byte vertical_points = 1;
    for(byte i=line-1; i!=255; i--){
        if(walls[player*WALL_SIZE + i*WALL_WIDTH + column])
            vertical_points += 1;
        else
            break;
    }
    for(byte i=line+1; i<WALL_HEIGHT; i++){
        if(walls[player*WALL_SIZE + i*WALL_WIDTH + column])
            vertical_points += 1;
        else
            break;
    }
    for(byte j=column-1; j!=255; j--){
        if(walls[player*WALL_SIZE + line*WALL_WIDTH + j])
            horizontal_points += 1;
        else
            break;
    }
    for(byte j=column+1; j<WALL_HEIGHT; j++){
        if(walls[player*WALL_SIZE + line*WALL_WIDTH + j])
            horizontal_points += 1;
        else
            break;
    }

    if(vertical_points==1)
        scores[player] += horizontal_points;
    else if(horizontal_points==1)
        scores[player] += vertical_points;
    else
        scores[player] += horizontal_points + vertical_points;
}


void Board::addEndgameBonus(){
    for(byte player=0; player<NB_PLAYERS; player++){

        // horizontal line bonus
        for(byte i=0; i< WALL_HEIGHT; i++){
            for(byte j=0; j< WALL_WIDTH; j++){
                // if there is no tile: the line isn't complete (stop)
                if(!walls[player*WALL_SIZE+i*WALL_WIDTH+j])
                    break;
                // if we checked that all tiles in line i are here: add bonus
                if(j==WALL_WIDTH-1)
                    scores[player] += HORIZONTAL_LINE_BONUS;
            }
        }

        // vertical line bonus
        for(byte j=0; j< WALL_WIDTH; j++){
            for(byte i=0; i< WALL_HEIGHT; i++){
                // if there is no tile: the column isn't complete (stop)
                if(!walls[player*WALL_SIZE+i*WALL_WIDTH+j])
                    break;
                // if we checked that all tiles in column j are here: add bonus
                if(i==WALL_HEIGHT-1)
                    scores[player] += VERTICAL_LINE_BONUS;
            }
        }

        byte j;
        // color bonus
        for(byte col=0; col<NB_COLORS; col++){
            for(byte i=0; i< WALL_HEIGHT; i++){
                j = wallColorToColumn(col,i);
                // if there is no tile: the color isn't complete (stop)
                if(!walls[player*WALL_SIZE+i*WALL_WIDTH+j])
                    break;
                // if we checked that all tiles of color col are here: add bonus
                if(i==WALL_HEIGHT-1)
                    scores[player] += COLOR_BONUS;
            }
        }
    }
}


void Board::play(byte factory, byte color, byte line){
    assert(playable(factory,color,line));

    int nb_tiles = factories[factory*NB_COLORS+color];
    factories[factory*NB_COLORS+color] = 0;
    // if the choosen tile is not in the center
    if(factory != NB_FACTORIES){
        for(byte col=0; col<NB_COLORS; col++){
            // move the tiles to center of the table
            factories[NB_FACTORIES*NB_COLORS+col] += factories[factory*NB_COLORS+col];
            factories[factory*NB_COLORS+col] = 0;
        }
    }
    // if the choosen tile is in the center and the "1" tile is still here
    else if(tile1 == NB_PLAYERS){
        tile1 = current_player;
        // place the "1" tile in the floor if possible
        for(byte i=0; i<FLOOR_SIZE ; i++){
            // si la case est vide
            if(floor_lines[current_player*FLOOR_SIZE+i] == NB_COLORS+1){
                floor_lines[current_player*FLOOR_SIZE+i] = NB_COLORS;
                break;
            }
        }
    }

    byte index = current_player*WALL_HEIGHT*2 + line*2;
    byte nb_tiles_line;
    byte nb_tiles_floor;
    if(line < WALL_HEIGHT){
        nb_tiles_line = min(line-pattern_lines[index]+1, nb_tiles);
        nb_tiles_floor = nb_tiles-nb_tiles_line;
        // fill the pattern line
        if(pattern_lines[index]==0)
            pattern_lines[index+1] = color;
        pattern_lines[index] += nb_tiles_line;
    }
    else{
        nb_tiles_line = 0;
        nb_tiles_floor = nb_tiles;
    }

    // fill the floor lines
    for(byte i=0; i<FLOOR_SIZE and nb_tiles_floor>0; i++){
        // si la case est vide
        if(floor_lines[current_player*FLOOR_SIZE+i] == NB_COLORS+1){
            floor_lines[current_player*FLOOR_SIZE+i] = color;
            nb_tiles_floor--;
        }
    }

    // discard the extra tiles
    discard[current_player*NB_COLORS+color] += nb_tiles_floor;

    // the player end his turn
    nextPlayer();
}


bool Board::pickableTile(byte factory, byte color) const{
    // That color is available in this factory ?
    return factories[factory*NB_COLORS+color]>0;
}

bool Board::placeableTile(byte color, byte line) const{
    // if the player wants to place the tiles on the floor line (always possible)
    if(line == WALL_HEIGHT)
        return true;

    int index = current_player*WALL_HEIGHT*2+line*2;

    // if the line is empty... check if the color has already be done
    if(pattern_lines[index] == 0)
        return !walls[current_player*WALL_SIZE+line*WALL_WIDTH+wallColorToColumn(color,line)];

    // if the line is not empty... colors need to match and the line needs to be not full
    return (pattern_lines[index+1] == color and pattern_lines[index]<line+1);
}

bool Board::playable(byte factory, byte color, byte line){
    return pickableTile(factory,color) and placeableTile(color,line);
}

void Board::addMalus(byte malus, byte player){
    if(malus>scores[player])
        scores[player] = 0;
    else
        scores[player] -= malus;
}


// To test if the game works
void randomGameTest(){

    srand((unsigned int) time(0));

    int total_nb_round = 0;

    Board game;

    while(!game.endOfTheGame()){

        game.nextRound();

        total_nb_round++;

        while(!game.endOfTheRound()){

            int nb_pickeable = 0;
            for(int factory=0; factory<=NB_FACTORIES; factory++){
                for(int color=0; color<NB_COLORS; color++){
                    nb_pickeable += game.pickableTile(factory,color);
                }
            }

            int pick_choice = rand()%nb_pickeable;
            int factory_choice;
            int color_choice;
            int acc = 0;
            for(int factory=0; factory<=NB_FACTORIES; factory++){
                for(int color=0; color<NB_COLORS; color++){
                    acc += game.pickableTile(factory,color);
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
                nb_placeable += game.placeableTile(color_choice, line);
            }

            nb_placeable = max(1,nb_placeable-1);
            int spot_choice = rand()%nb_placeable;
            acc=0;

            for(int line=0; line<=WALL_HEIGHT; line++){
                acc += game.placeableTile(color_choice, line);
                if(spot_choice<acc){
                    game.play(factory_choice,color_choice,line);
                    break;
                }
            }
        }
    }
    game.nextRound();
    game.addEndgameBonus();
}


void Board::display() const{
    for(int factory=0; factory<=NB_FACTORIES; factory++){
        if(factory == NB_FACTORIES)
            cout<<"Center"<<endl;
        else
            cout<<"Factory "<<factory<<endl;
        for(int col=0; col<NB_COLORS; col++){
            if(getFactoryTile(factory,col)!=0){
                cout<<"Col "<<col<<": "<<int(getFactoryTile(factory,col))<<endl;
            }
        }
        cout<<endl;
    }
    if(tile1==NB_PLAYERS)
        cout<<"tile 1"<<endl;
}


size_t Board::board_hash() const{
    unsigned int key = 127320773;
    size_t output =0;
    output ^= current_player;
    output *= key;
    output ^= tile1;
    output *= key;

    for(byte player=0; player<NB_PLAYERS; player++){
        output ^= scores[player];
        output *= key;
    }

    for(byte i=0; i<NB_COLORS; i++){
        output ^= bag[i];
        output *= key;
    }

    for(byte i=0; i<NB_COLORS; i++){
        output ^= discard[i];
        output *= key;
    }

    for(byte i=0; i<(NB_FACTORIES+1)*NB_COLORS; i++){
        output ^= factories[i];
        output *= key;
    }

    for(byte i=0; i<NB_PLAYERS*WALL_HEIGHT*2; i++){
        output ^= pattern_lines[i];
        output *= key;
    }

    for(byte i=0; i<NB_PLAYERS*FLOOR_SIZE; i++){
        output ^= floor_lines[i];
        output *= key;
    }

    for(byte i=0; i<NB_PLAYERS*WALL_SIZE; i++){
        output ^= walls[i];
        output *= key;
    }
    return output;
}



