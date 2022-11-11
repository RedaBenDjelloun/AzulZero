#include <Imagine/Graphics.h>
using namespace Imagine;
#include <iostream>
using namespace std;
#include "board.h"
#include "controller.h"
#include "GUI.h"

int main(){

    srand((unsigned int) time(0));

    int total_points[NB_PLAYERS] = {0,0};
    int max_points[NB_PLAYERS] = {0,0};
    int nb_iterations = 10000;
    int winner[3] = {0,0,0};
    Controller** players = new Controller*[NB_PLAYERS];
    Heuristic smart_player1;
    Heuristic smart_player2(0);
    Random random_player;
/*
    for(int i=0; i<10; i++){
        smart_player1.optimize(&smart_player2);
        smart_player2.optimize(&smart_player1);
    }
*/
    players[0] = &smart_player1;
    players[1] = &random_player;

    smart_player1.optimize(&random_player,100,100);
    for(int i=0; i<nb_iterations; i++){

        Board board;

        play_game(&board,players);
        for(int player=0; player<NB_PLAYERS; player++){
            total_points[player] += board.scores[player];
            max_points[player] = max(max_points[player],int(board.scores[player]));
        }
        winner[(board.scores[0]<board.scores[1])+(board.scores[0]<=board.scores[1])]+=1;
    }

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

