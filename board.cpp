#include <Imagine/Graphics.h>
using namespace Imagine;
#include "board.h"
#include <assert.h>
using namespace std;


int wallColumnToColor(int column, int line){
    return (column - line + NB_COLORS) % NB_COLORS;
}


int wallColorToColumn(int color, int line){
    return (line + color) % NB_COLORS;
}


Board::Board(){

    current_player = 0;

    for(int i=0; i<NB_COLORS; i++){
        bag[i] = NB_TILES_PER_COLOR;
    }

    for(int i=0; i<NB_COLORS; i++){
        discard[i] = 0; // no tiles discarded yet
    }

    for(int i=0; i<(NB_FACTORIES+1)*NB_COLORS; i++){
        factories[i] = 0;   // factories are empty
    }

    for(int i=0; i<NB_PLAYERS*WALL_HEIGHT*2; i++){
        pattern_lines[i] = 0; // pattern lines are empty
    }

    for(int i=0; i<NB_PLAYERS*FLOOR_SIZE; i++){
        floor_lines[i] = NB_COLORS+1; // there is no tile
    }

    for(int i=0; i<NB_PLAYERS*WALL_SIZE; i++){
        walls[i] = false;   // the wall is empty
    }
}


bool Board::endOfTheRound(){
    int factories_size = (NB_FACTORIES+1)*NB_COLORS;
    for(int i=0; i<factories_size; i++){
        if(factories[i]!=0)
            return false;
    }
    return true;
}


bool Board::endOfTheGame(){

    for(int player=0; player<NB_PLAYERS; player++){
        for(int line=0; line<WALL_HEIGHT; line++){
            for(int column=0; column<WALL_WIDTH; column++){
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
}


void Board::updateWall(){

    // for each player
    for(int player=0; player<NB_PLAYERS; player++){

        // update wall
        for(int line=0; line<WALL_HEIGHT; line++){
            // if the pattern line is complete
            if(pattern_lines[player*2*WALL_HEIGHT+line*2] == line+1){

                int color = pattern_lines[player*2*WALL_HEIGHT+line*2+1];
                int column = wallColorToColumn(color, line);

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
    for(int player=0; player<NB_PLAYERS; player++){
        // update floor
        for(int i=0; i<FLOOR_SIZE; i++){
            int color = floor_lines[player*FLOOR_SIZE+i];
            // if there is no tile: stop
            if(color == NB_COLORS+1)
                break;
            // if there is the "1" tile: player become the current player
            if(color == NB_COLORS)
                current_player = player;
            // clear the tile
            floor_lines[player*FLOOR_SIZE+i]=NB_COLORS+1;
            // add the malus
            scores[player] += FLOOR[i];
            // discard the tile
            discard[color] ++;
        }
    }
}


void Board::updateFactories(){

    int tiles_remaining = 0;
    for(int col=0; col<NB_COLORS; col++){
        tiles_remaining += bag[col];
    }

    // for each factory (except the center of the table)
    for(int factory=0; factory<NB_FACTORIES; factory++){
        for(int tile=0; tile<NB_TILES_PER_FACTORY; tile++){
            if(tiles_remaining == 0){
                refillBag();
                for(int col=0; col<NB_COLORS; col++){
                    tiles_remaining += bag[col];
                }
            }
            // choose a random tile
            int col = chooseRandomTile(tiles_remaining);
            // remove the color from the bag
            bag[col] --;
            tiles_remaining --;
            // add the tile to the factory
            factories[factory*NB_COLORS + col] ++;
        }
    }
}


int Board::chooseRandomTile(int tiles_remaining){
    assert(tiles_remaining>0);
    int random = rand()%tiles_remaining;
    int nb_tiles = 0;
    for (int col=0; col<NB_COLORS; col++){
        nb_tiles += bag[col];
        if(nb_tiles>random){
            return col;
        }
    }
    return -1; // not normal
}


void Board::refillBag(){
    for(int col=0; col<NB_COLORS; col++){
        bag[col] = discard[col];
        discard[col] = 0;
    }
}


void Board::addTileToWall(int player, int line, int column){
    int horizontal_points = 1;
    int vertical_points = 1;
    for(int i=line-1; i>=0; i--){
        if(walls[player*WALL_SIZE + i*WALL_WIDTH + column])
            vertical_points += 1;
        else
            break;
    }
    for(int i=line+1; i<WALL_HEIGHT; i++){
        if(walls[player*WALL_SIZE + i*WALL_WIDTH + column])
            vertical_points += 1;
        else
            break;
    }
    for(int j=column-1; j>=0; j--){
        if(walls[player*WALL_SIZE + line*WALL_WIDTH + j])
            horizontal_points += 1;
        else
            break;
    }
    for(int j=column+1; j<WALL_HEIGHT; j++){
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
    for(int player=0; player<NB_PLAYERS; player++){

        // horizontal line bonus
        for(int i=0; i< WALL_HEIGHT; i++){
            for(int j=0; j< WALL_WIDTH; j++){
                // if there is no tile: the line isn't complete (stop)
                if(!walls[player*WALL_SIZE+i*WALL_WIDTH+j])
                    break;
                // if we checked that all tiles in line i are here: add bonus
                if(j==WALL_WIDTH-1)
                    scores[player] += HORIZONTAL_LINE_BONUS;
            }
        }

        // vertical line bonus
        for(int j=0; j< WALL_WIDTH; j++){
            for(int i=0; i< WALL_HEIGHT; i++){
                // if there is no tile: the column isn't complete (stop)
                if(!walls[player*WALL_SIZE+i*WALL_WIDTH+j])
                    break;
                // if we checked that all tiles in column j are here: add bonus
                if(i==WALL_HEIGHT-1)
                    scores[player] += VERTICAL_LINE_BONUS;
            }
        }

        int j;
        // color bonus
        for(int col=0; col<NB_COLORS; col++){
            for(int i=0; i< WALL_HEIGHT; i++){
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
