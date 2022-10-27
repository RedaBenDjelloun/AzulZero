#include <Imagine/Graphics.h>
using namespace Imagine;
#include <iostream>
using namespace std;

#include "GUI.h"

int main(){
    vector<Image<AlphaColor>> images;
    loadAllImages(IMAGE_PATHS,images);

//    Image<AlphaColor> I;
//    safeLoad(I,PLAYERBOARD);

    openWindow(window_width, window_height);
    display(images[6],0,0);

    endGraphics();
    return 0;

}

