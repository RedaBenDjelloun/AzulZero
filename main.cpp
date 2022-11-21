#include <Imagine/Graphics.h>
using namespace Imagine;
#include <iostream>
using namespace std;
#include <chrono>
using namespace std::chrono;
#include "board.h"
#include "controller.h"
#include "GUI.h"

int main(){

    srand((unsigned int) time(0));

    int total_points[NB_PLAYERS] = {0,0};
    int max_points[NB_PLAYERS] = {0,0};
    int nb_iterations = 1;
    int winner[3] = {0,0,0};
    Controller** players = new Controller*[NB_PLAYERS];
    Heuristic smart_player1(0);
    MinMax smart_player2(20);
    Random random_player;
    Human player("Hector");
    players[0] = &random_player;
    players[1] = &player;

    auto start = high_resolution_clock::now();

    for(int i=0; i<nb_iterations; i++){

        Board board;
        board.init();

        play_game(&board,players);
        for(int player=0; player<NB_PLAYERS; player++){
            total_points[player] += board.getScore(player);
            max_points[player] = max(max_points[player],int(board.getScore(player)));
        }
        winner[board.winner()]+=1;
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout<<duration.count()<<endl;
    cout<<nb_iterations<<" parties jouees"<<endl;
    cout<<"score moyen joueur 1: "<<float(total_points[0])/nb_iterations<<endl;
    cout<<"score moyen joueur 2: "<<float(total_points[1])/nb_iterations<<endl;
    cout<<"meilleur score joueur 1: "<<max_points[0]<<endl;
    cout<<"meilleur score joueur 2: "<<max_points[1]<<endl;
    cout<<"nombre de parties gagnees par le joueur 1: "<<winner[0]<<endl;
    cout<<"nombre de parties nulles: "<<winner[1]<<endl;
    cout<<"nombre de parties gagnees par le joueur 2: "<<winner[2]<<endl;


    return 0;

}

