//a drone simulator, made by Axel and Ludvig:)
//with zero blocks and zero points, the time it takes to run 60 frames on Ludvig's PC is ~362ms

#include "std_lib_facilities.h"
#include "gameHandler.h"
#include <chrono> //library for testing code speed

const int windowWidth = 1000;
const int windowHeight = 650;
double fov = 1;
const int frameScaling = 1000;
gameModes gameMode = gameModes::mainMenu; //This is global so we can change it fram gameHandler.cpp (by pressing buttons)

auto starttid = chrono::steady_clock::now();
auto sluttid = chrono::steady_clock::now();
auto varighet = chrono::duration_cast<chrono::milliseconds>(sluttid-starttid);
auto* startptr = &starttid;

int main() {
    AnimationWindow window(50, 50, windowWidth, windowHeight, "3d-renderer");
    gameModes gameMode = gameModes::mainMenu;
    GameHandler screen = GameHandler(window, gameMode);
    //for (int i = -30; i < 30; i++) {
    //    for (int j = 0; j < 1; j++) {
    //        for (int k = -30; k < 30; k++) {
    //            screen.editor.world.addBlock(i, j, k);
    //        }
    //    }
    //}
    //screen.fpv.world.addBlock(2, 2, 5);
    //screen.fpv.world.addBlock(2, 2, 6);
    int i = 0;
    while (!window.should_close()) {
        i += 1;
        if (i == 1) {
            starttid = chrono::steady_clock::now();
        }
        
        screen.update(); 
        //cout << "Ferdig med å rendere" << endl;
        window.next_frame();
        //cout << screen.fpv.player.angles[0] << endl;
        //cout << screen.fpv.player.angles[1] << endl;
        //cout << "Neste frame" << endl;
        if (i == 60) {
            i = 0;
            sluttid = chrono::steady_clock::now();
            varighet = chrono::duration_cast<chrono::milliseconds>(sluttid-starttid);
            //cout << "Tid mellom hvert sekstiende frame: " << varighet << endl;
        }
        
    }

    return 0;
}
