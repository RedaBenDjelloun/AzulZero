#include "interface.h"


Color darken(Color col){
    return Color(col.r()/2,col.g()/2,col.b()/2);
}

Color brighten(Color col){
    return Color(col.r()/2+127,col.g()/2+127,col.b()/2+127);
}

void displaySelection(int choices[NB_PLAYERS]){
    for(int player=0; player<NB_PLAYERS; player++){
        if(choices[player]==-1){
        for(int choice=0; choice<NB_CHOICES_AI;choice++){
            IntPoint2 P{P0.x()+player*WINDOW_WIDTH/2 + choice*INDENT,P0.y()+choice*(BUTTON_HEIGHT+H_SPACING)};
            drawRect(P.x()-1,P.y()-1,BUTTON_WIDTH+1,BUTTON_HEIGHT+1,BLACK);
            drawRect(P.x()-2,P.y()-2,BUTTON_WIDTH+3,BUTTON_HEIGHT+3,WHITE);
            drawRect(P.x()-3,P.y()-3,BUTTON_WIDTH+5,BUTTON_HEIGHT+5,BLACK);
            fillRect(P,BUTTON_WIDTH,BUTTON_HEIGHT,COLORS_BUTTON[2*choice]);
            IntPoint2 Q{P.x()+W_SPACING,P.y()+BUTTON_HEIGHT/2+3*MENU_TEXT_SIZE/4};
            drawString(Q,choices_names[choice],COLORS_BUTTON[2*choice+1],MENU_TEXT_SIZE);
        }
        }else{
            int choice = choices[player];
            IntPoint2 P{P0.x()+player*WINDOW_WIDTH/2 + choice*INDENT,P0.y()+choice*(BUTTON_HEIGHT+H_SPACING)};
            drawRect(P.x()-1,P.y()-1,BUTTON_WIDTH+1,BUTTON_HEIGHT+1,BLACK);
            drawRect(P.x()-2,P.y()-2,BUTTON_WIDTH+3,BUTTON_HEIGHT+3,WHITE);
            drawRect(P.x()-3,P.y()-3,BUTTON_WIDTH+5,BUTTON_HEIGHT+5,BLACK);
            fillRect(P,BUTTON_WIDTH,BUTTON_HEIGHT,COLORS_BUTTON[2*choice]);
            IntPoint2 Q{P.x()+W_SPACING,P.y()+BUTTON_HEIGHT/2+3*MENU_TEXT_SIZE/4};
            drawString(Q,choices_names[choice],COLORS_BUTTON[2*choice+1],MENU_TEXT_SIZE);
        }
    }
}

void user_friendly_interface(){
    srand((unsigned int) time(nullptr));

    GUI gui;
    gui.init();
    fillRect(0,0,WINDOW_WIDTH,WINDOW_HEIGHT,Color(238,215,191));
    drawLine(WINDOW_WIDTH/2-3,0,WINDOW_WIDTH/2-3,WINDOW_HEIGHT,BLACK,3);
    drawLine(WINDOW_WIDTH/2,0,WINDOW_WIDTH/2,WINDOW_HEIGHT,WHITE,3);
    drawLine(WINDOW_WIDTH/2+3,0,WINDOW_WIDTH/2+3,WINDOW_HEIGHT,BLACK,3);
    int choices[NB_PLAYERS]={-1,-1};


    enableMouseTracking(true);
    Event ev;
    displaySelection(choices);
    bool button_activated = false;
    do {
        getEvent(-1,ev); // ev becomes the next event
        if(ev.type == EVT_MOTION){
            bool nothing_happened = true;
            int x = ev.pix.x();   // cursor's x coord
            int y = ev.pix.y();   // cursor's y coord
            for(int player=0; player<NB_PLAYERS; player++){
                if(choices[player]==-1){
                    for(int choice=0; choice<NB_CHOICES_AI;choice++){
                        IntPoint2 P{P0.x()+player*WINDOW_WIDTH/2 + choice*INDENT,P0.y()+choice*(BUTTON_HEIGHT+H_SPACING)};
                        if(P.x()<=x && x<P.x()+BUTTON_WIDTH && P.y()<=y && y<P.y()+BUTTON_HEIGHT){
                            fillRect(P,BUTTON_WIDTH,BUTTON_HEIGHT,brighten(COLORS_BUTTON[2*choice]));
                            IntPoint2 Q{P.x()+W_SPACING,P.y()+BUTTON_HEIGHT/2+3*MENU_TEXT_SIZE/4};
                            drawString(Q,choices_names[choice],brighten(COLORS_BUTTON[2*choice+1]),MENU_TEXT_SIZE);
                            nothing_happened = false;
                            button_activated = true;
                        }
                    }
                }
            }
            if(nothing_happened and button_activated){
                displaySelection(choices);
                button_activated = false;
            }
        }else if(ev.type == EVT_BUT_ON){
            int x = ev.pix.x();   // cursor's x coord
            int y = ev.pix.y();   // cursor's y coord
            for(int player=0; player<NB_PLAYERS; player++){
                for(int choice=0; choice<NB_CHOICES_AI;choice++){
                    IntPoint2 P{P0.x()+player*WINDOW_WIDTH/2,P0.y()+choice*(BUTTON_HEIGHT+H_SPACING)};
                    if(P.x()<=x && x<P.x()+BUTTON_WIDTH && P.y()<=y && y<P.y()+BUTTON_HEIGHT){
                        choices[player] = choice;
                        fillRect(0,0,WINDOW_WIDTH,WINDOW_HEIGHT,Color(238,215,191));
                        drawLine(WINDOW_WIDTH/2-3,0,WINDOW_WIDTH/2-3,WINDOW_HEIGHT,BLACK,3);
                        drawLine(WINDOW_WIDTH/2,0,WINDOW_WIDTH/2,WINDOW_HEIGHT,WHITE,3);
                        drawLine(WINDOW_WIDTH/2+3,0,WINDOW_WIDTH/2+3,WINDOW_HEIGHT,BLACK,3);
                        displaySelection(choices);
                    }
                }
            }
        }
    }while(choices[0]==-1 || choices[1]==-1);
    Controller** players = new Controller*[2];
    for(int player=0; player<NB_PLAYERS; player++){
        switch(choices[player]){
        case(0):
            players[player] = new Human("Joueur",&gui);
            break;
        case(1):
            players[player] = new Random();
            break;
        case(2):
            players[player] = new Heuristic(0);
            break;
        case(3):
            players[player] = new MinMax;
            break;
        case(4):
            players[player] = new MCTS;
            break;
        }
    }
    milliSleep(800);
    Board board;
    board.init();
    Game game{*playGameGraphics(&board,players,gui)};
    game.review_game();
    endGraphics();
}

