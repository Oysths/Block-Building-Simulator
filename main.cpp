//a drone simulator, made by Axel and Ludvig:)

#include "std_lib_facilities.h"
#include "gameHandler.h"

const int windowWidth = 1000;
const int windowHeight = 650;
const int fov = 1;
const int frameScaling = 1000;
gameModes gameMode = gameModes::mainMenu; //This is global so we can change it fram gameHandler.cpp (by pressing buttons)


int main() {
    AnimationWindow window(50, 50, windowWidth, windowHeight, "3d-renderer");
    gameModes gameMode = gameModes::mainMenu;
    GameHandler screen = GameHandler(window, gameMode);
    screen.fpv.world.addBlock(0, 1, 6);


    while (!window.should_close()) {
        screen.update(); 
        //cout << "Ferdig med å rendere" << endl;
        window.next_frame();
        //cout << "Neste frame" << endl;
    }

    return 0;
}
