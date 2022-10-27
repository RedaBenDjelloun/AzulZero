#include "GUI.h"

int main(){
    Image<AlphaColor> image;

    if(! load(image, RED_TILE)) {
        cerr << "Error loading image file " << RED_TILE << endl;
        return 1;
    }

    openWindow(window_width, window_height);
    display(image,0,0);

    endGraphics();
    return 0;

}

