#pragma once

#include <Imagine/Graphics.h>
using namespace Imagine;
#include <iostream>
using namespace std;
#include <chrono>
using namespace std::chrono;
#include "board.h"
#include "controller.h"
#include "GUI.h"
#include "game.h"

Color darken(Color col);


const int NB_CHOICES_AI = 5;
const int BUTTON_WIDTH = 350*ZOOM;
const int H_SPACING = 80*ZOOM;
const int W_SPACING = 20*ZOOM;
const int INDENT = 50*ZOOM;
const int BUTTON_HEIGHT = 100*ZOOM;
const int MENU_TEXT_SIZE = 40*ZOOM;
const Color COLORS_BUTTON[10]{
    RED_TILE_COLOR,darken(RED_TILE_COLOR),
            BLUE_TILE_COLOR,darken(BLUE_TILE_COLOR),
            YELLOW_TILE_COLOR,YELLOW_TILE_COLOR2,
            DARK_TILE_COLOR, DARK_TILE_COLOR2,
            LIGHT_TILE_COLOR, LIGHT_TILE_COLOR2};

const IntPoint2 P0{BUTTON_WIDTH/2,BUTTON_HEIGHT};

const string choices_names[NB_CHOICES_AI] = {"Human","Random","Heuristic","MinMax","MCTS"};

Color brighten(Color col);

void displaySelection(int choices[NB_PLAYERS]);

void user_friendly_interface();

