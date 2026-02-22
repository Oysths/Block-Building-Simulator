#pragma once
#include "std_lib_facilities.h"
#include "render3d.h"
//#include "getData.h"


enum class gameModes {mainMenu, editor, fpv};

//extern gameModes gameMode;
void changeToFPV(); //changes to the fpv-gamemode

class MainMenu {
    AnimationWindow& window; //window object as reference
    //gameModes& gameMode;
    public:
        list<Button> buttons {}; //We need a list to store buttons. This is because when we add a button to buttons, AnimationWindow saves a pointer to that address. When we add a new button, every object is moved, so the pointer is no longer valid. A list fixes this
        MainMenu(AnimationWindow& window);
        void render();
        void addButton(double x, double y, double width, double height, string label, function<void ()>);
};

class FPV {
    public:
        AnimationWindow& window;
        Player player;
        World world;
        Point mouse;
        bool leftMouseDownLastFrame;
        bool rightMouseDownLastFrame;
        FPV(AnimationWindow& window);
        void render();
        void getPlayerInput();
};

class GameHandler {
    AnimationWindow& window; //this is the window that is being drawn to every frame
    gameModes& gameMode; //tracks the current gamemode
    MainMenu menu; //the mainmenu-object
    public:
        FPV fpv;
        GameHandler(AnimationWindow& window, gameModes& gameMode);
        void update(); //the most important method, should be called every frame from main. Checks for updates and then renders the appropriate gamemode
        void checkForGameModeChange(); //checks for changes in gamemode - called be the update method
        void render(); //checks which gamemode is active and renders the appropriate gamemode - called by the update method
        void play();
};