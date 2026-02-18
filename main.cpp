#include "std_lib_facilities.h"
#include "gameMode.h"

const int windowWidth = 1000;
const int windowHeight = 650;
const int fov = 1;
const int frameScaling = 1000;
array<double, 2> trigXZ; //yaw
array<double, 2> trigYZ; //pitch
array<double, 2> trigXY; //roll


int main() {
    AnimationWindow window(50, 50, windowWidth, windowHeight, "3d-renderer");
    gameModes gameMode = gameModes::mainMenu;
    updateScreen screen = updateScreen(window, gameMode);


    cout << "Hello, World!" << endl;

    return 0;
}
