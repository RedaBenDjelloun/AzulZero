#include <Imagine/Graphics.h>
using namespace Imagine;
#include <iostream>
using namespace std;
#include "board.h"

const char* default_image_file=srcPath("Images/black.svg");

int main() {

    Board test(2);
    test.nextRound();

    return 0;
}

