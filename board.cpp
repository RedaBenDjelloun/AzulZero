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


Board::Board(){

    current_player = 0;

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


bool Board::endOfTheRound(){
    byte factories_size = (NB_FACTORIES+1)*NB_COLORS;
    for(byte i=0; i<factories_size; i++){
        if(factories[i]!=0)
            return false;
    }
    return true;
}


bool Board::endOfTheGame(){

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
            // if there is the "1" tile: player become the current player
            if(color == NB_COLORS)
                current_player = player;
            // clear the tile
            floor_lines[player*FLOOR_SIZE+i]=NB_COLORS+1;
            // add the malus
            scores[player] -= FLOOR[i];
            // discard the tile
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

    byte horizontal_pobytes = 1;
    byte vertical_pobytes = 1;
    for(byte i=line-1; i>=0; i--){
        if(walls[player*WALL_SIZE + i*WALL_WIDTH + column])
            vertical_pobytes += 1;
        else
            break;
    }
    for(byte i=line+1; i<WALL_HEIGHT; i++){
        if(walls[player*WALL_SIZE + i*WALL_WIDTH + column])
            vertical_pobytes += 1;
        else
            break;
    }
    for(byte j=column-1; j>=0; j--){
        if(walls[player*WALL_SIZE + line*WALL_WIDTH + j])
            horizontal_pobytes += 1;
        else
            break;
    }
    for(byte j=column+1; j<WALL_HEIGHT; j++){
        if(walls[player*WALL_SIZE + line*WALL_WIDTH + j])
            horizontal_pobytes += 1;
        else
            break;
    }

    if(vertical_pobytes==1)
        scores[player] += horizontal_pobytes;
    else if(horizontal_pobytes==1)
        scores[player] += vertical_pobytes;
    else
        scores[player] += horizontal_pobytes + vertical_pobytes;
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
