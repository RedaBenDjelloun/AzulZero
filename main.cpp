#include <Imagine/Graphics.h>
using namespace Imagine;
#include <iostream>
using namespace std;

const char* default_image_file=srcPath("Images/red.svg");

int main(int argc, char **argv) {
    string image_file = (argc>1)? argv[1]: default_image_file;
    AlphaColor* image;
    int width, height;
    cout << "Loading image: " << image_file << endl;
    if(! loadAlphaColorImage(default_image_file, image, width, height)) {
        cerr << "Error loading image file " << image_file << endl;
        return 1;
    }

    openWindow(width, height);
    putAlphaColorImage(0,0,image,width,height);

    endGraphics();

    return 0;
}

