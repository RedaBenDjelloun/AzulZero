#include <Imagine/Graphics.h>
using namespace Imagine;
#include <iostream>
#include <chrono>
using namespace std;
using namespace std::chrono;
#include "board.h"

int main() {

    srand((unsigned int) time(0));

    int total_nb_round = 0;
    int nb_games = 10000;

    auto start = high_resolution_clock::now();
    for(int i=0; i<nb_games; i++){

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

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken for playing " << nb_games << " games: "
         << duration.count() << " microseconds" << endl;
    cout << "Total number of round played: "
         << total_nb_round << endl;
    cout << "Average time for a round: "
         << duration.count()/float(total_nb_round) << " microseconds" << endl;

    return 0;
}

