#pragma once

#include <iostream>
using namespace std;
#include <Imagine/Graphics.h>
#include <Imagine/Images.h>
using namespace Imagine;

#include "board.h"

// GUI Parameters

const int window_width = 500;
const int window_height = 500;

// Images

const string BLUE_TILE = stringSrcPath("Images/blue.svg");
const string YELLOW_TILE = stringSrcPath("Images/yellow.svg");
const string RED_TILE = stringSrcPath("Images/red.svg");
const string BLACK_TILE = stringSrcPath("Images/black.svg");
const string CYAN_TILE = stringSrcPath("Images/cyan.svg");
const string FIRST_PLAYER_TILE = stringSrcPath("Images/first-player.svg");
const string FACTORY = stringSrcPath("Images/factory.png");
const string BAG = stringSrcPath("Images/bag.png");
const string PLAYERBOARD = stringSrcPath("Images/playerboard.png");
const string GAMEBOX = stringSrcPath("Images/game_box.png");

const vector<string> IMAGE_PATHS = {
    BLUE_TILE,
    YELLOW_TILE,
    RED_TILE,
    BLACK_TILE,
    CYAN_TILE,
    FIRST_PLAYER_TILE,
    FACTORY,
    BAG,
    PLAYERBOARD,
    GAMEBOX
};

void safeLoad(Image<AlphaColor> &image, string path);

void loadAllImages(const vector<string> &IMAGE_PATHS, vector<Image<AlphaColor> > &IMAGES);



