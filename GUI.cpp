#include "GUI.h"

GUI::GUI(vector<string> image_paths, int w, int h){
    window_width = w;
    window_height = h;
    images = new Image<AlphaColor>[2*image_paths.size()];
    loadImages(image_paths);

}

GUI::~GUI(){
    delete [] images;
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
    displayImage(4+j,x,y,fact);
    drawRect(x-1,y-1,TILE_SIDE+2,TILE_SIDE+2,BLACK);
    drawRect(x-2,y-2,TILE_SIDE+4,TILE_SIDE+4,WHITE);
    drawRect(x-3,y-3,TILE_SIDE+6,TILE_SIDE+6,BLACK);
}

void GUI::displayPlayerboards(){
    displayPlayerboard(PLAYERBOARD_X0);
    displayPlayerboard(PLAYERBOARD_X0,WINDOW_HEIGHT/2);
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

void GUI::displayWalls(){
    for (int x = 0; x < NB_COLORS; x++){
        for (int y = 0; y < NB_COLORS; y++){
            displayTile(rand()%NB_COLORS, PLAYERBOARD_X0 + WALL_X0 + x*WALL_SPACING, WALL_Y0 + y*WALL_SPACING);
            displayTile(rand()%NB_COLORS, PLAYERBOARD_X0 + WALL_X0 + x*WALL_SPACING, PLAYERBOARD_HEIGHT + WALL_Y0 + y*WALL_SPACING);
        }
    }
}

void GUI::displayPatterns(){
    for (int x = 0; x < NB_COLORS; x++){
        for (int y = NB_COLORS-x-1; y < NB_COLORS; y++){
            displayTile(rand()%NB_COLORS, PLAYERBOARD_X0 + PATTERN_X0 + x*WALL_SPACING, PATTERN_Y0 + y*WALL_SPACING);
            displayTile(rand()%NB_COLORS, PLAYERBOARD_X0 + PATTERN_X0 + x*WALL_SPACING, PLAYERBOARD_HEIGHT + PATTERN_Y0 + y*WALL_SPACING);
        }
    }
}

void GUI::displayFloors(){
    for (int x = 0; x < FLOOR_SIZE; x++){
        displayTile(rand()%NB_COLORS, PLAYERBOARD_X0 + FLOOR_X0 + x*FLOOR_SPACING, FLOOR_Y0);
        displayTile(rand()%NB_COLORS, PLAYERBOARD_X0 + FLOOR_X0 + x*FLOOR_SPACING, PLAYERBOARD_HEIGHT + FLOOR_Y0);
    }
}

void GUI::displayFactoryTiles(IntPoint2 factory_p0){
    for (int x = -1; x < 1; x++){
        for (int y = -1; y < 1; y++){
            displayTile(rand()%NB_COLORS,factory_p0.x() + FACTORY_SIDE/2 + x*(FACTORY_MARGIN+TILE_SIDE)+FACTORY_MARGIN/2,
                        factory_p0.y() + FACTORY_SIDE/2 + y*(FACTORY_MARGIN+TILE_SIDE) + FACTORY_MARGIN/2);
        }
    }

}

void GUI::displayAllFactoryTiles(){
    IntPoint2 unityroot = IntPoint2(1,0);
    for (int k = 0; k < NB_FACTORIES; k++){
        unityroot = IntPoint2(int(FACTORY_RADIUS*cos(- M_PI/2 + 2*M_PI*k/NB_FACTORIES)),
                              int(FACTORY_RADIUS*sin(- M_PI/2 + 2*M_PI*k/NB_FACTORIES)));
        displayFactoryTiles(FACTORY_CENTER+unityroot-FACTORY_CENTERING);
    }
}

void GUI::displayTileInfo(IntPoint2 P0, Color bg_color, int tiles[NB_COLORS]){
    fillRect(P0, 5*WALL_SPACING + WALL_MARGIN, BAG_SIDE, bg_color);
    for (int x = 0; x < NB_COLORS; x++){
        if (tiles[x] > 0){
            displayTile(x, P0 + IntPoint2(WALL_MARGIN + x*WALL_SPACING,WALL_MARGIN/2));
            if (tiles[x] < 10){
                drawString(P0 + IntPoint2(WALL_MARGIN + x*WALL_SPACING + TILE_SIDE/2 - TEXT_SIZE/4,WALL_SPACING + int(2.5*WALL_MARGIN)), to_string(tiles[x]), BLACK, TEXT_SIZE);
            }
            else{
                drawString(P0 + IntPoint2(WALL_MARGIN + x*WALL_SPACING + TILE_SIDE/2 - TEXT_SIZE/2,WALL_SPACING + int(2.5*WALL_MARGIN)), to_string(tiles[x]), BLACK, TEXT_SIZE);
            }
        }
    }
}

void GUI::displayMiddleTiles(int tiles[NB_COLORS], bool first_not_claimed){
    displayTileInfo(MIDDLE_P0,MIDDLE_COLOR, tiles);
    if (first_not_claimed){
        displayTile(NB_COLORS, WINDOW_HEIGHT/2 - TILE_SIDE/2, WINDOW_HEIGHT/2 - TILE_SIDE/2);
    }
}

void GUI::displayBagContent(int tiles[NB_COLORS]){
    displayTileInfo(BAG_CONTENT_P0, BAG_CONTENT_COLOR, tiles);
}

void GUI::displayDiscard(int tiles[NB_COLORS]){
    displayTileInfo(DISCARD_P0, DISCARD_COLOR, tiles);
}


void GUI::displayGrid(){
    drawLine(WINDOW_HEIGHT/2, 0, WINDOW_HEIGHT/2, WINDOW_HEIGHT, BLACK);
    drawLine(0, WINDOW_HEIGHT/2, WINDOW_HEIGHT, WINDOW_HEIGHT/2, BLACK);
    drawLine(0, BAG_SIDE, WINDOW_HEIGHT, BAG_SIDE, BLACK);
    drawLine(0, WINDOW_HEIGHT - BAG_SIDE, WINDOW_HEIGHT, WINDOW_HEIGHT - BAG_SIDE, BLACK);
    drawLine(BAG_SIDE, 0, BAG_SIDE, WINDOW_HEIGHT, BLACK);
    drawLine(WINDOW_HEIGHT - BAG_SIDE, 0, WINDOW_HEIGHT - BAG_SIDE, WINDOW_HEIGHT, BLACK);
}

