#include "GUI.h"

void safeLoad(Image<AlphaColor> &image, string path){
    if(! load(image, path)) {
        cerr << "Error loading image file " << path << endl;
    }
}

void loadAllImages(const vector<string> &IMAGE_PATHS, vector<Image<AlphaColor> > &IMAGES){
    for (unsigned int i = 0; i < IMAGE_PATHS.size(); i++){
        Image<AlphaColor> image;
        safeLoad(image,IMAGE_PATHS[i]);
        IMAGES.push_back(image);
    }
}
