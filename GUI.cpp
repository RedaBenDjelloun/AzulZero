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

void GUI::displayAllPlayerboards(){
    displayPlayerboard(WINDOW_WIDTH-PLAYERBOARD_WIDTH);
    displayPlayerboard(WINDOW_WIDTH-PLAYERBOARD_WIDTH,WINDOW_HEIGHT/2);
}

void GUI::displayFactoryCircle(){
    int radius = (WINDOW_WIDTH-PLAYERBOARD_WIDTH)/2 - FACTORY_SIDE/2;
    IntPoint2 center = IntPoint2((WINDOW_WIDTH-PLAYERBOARD_WIDTH)/2,WINDOW_HEIGHT/2);
    IntPoint2 centering = IntPoint2(FACTORY_SIDE/2, FACTORY_SIDE/2);
    IntPoint2 unityroot = IntPoint2(1,0);
    drawCircle(center,radius-FACTORY_SIDE/2,ORANGE);
    drawCircle(center,radius+FACTORY_SIDE/2,ORANGE);
    drawCircle(center,radius,BLACK);
    for (int k = 0; k < NB_FACTORIES; k++){
        unityroot = IntPoint2(int(radius*cos(- M_PI/2 + 2*M_PI*k/NB_FACTORIES)),int(radius*sin(- M_PI/2 + 2*M_PI*k/NB_FACTORIES)));
        displayFactory(center+unityroot-centering);
    }
}
