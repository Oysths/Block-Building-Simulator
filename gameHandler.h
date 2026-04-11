#pragma once
#include "std_lib_facilities.h"
#include "render3d.h"
#include <list>
//#include "getData.h"


enum class gameModes {mainMenu, editor, fpv};

//extern gameModes gameMode;
void changeToEditor(); //changes to the editor-gamemode
void nextMap(); //changes to the editor-gamemode
void previousMap(); //changes to the editor-gamemode
string getMapNameFromIndex(int idx);

class MainMenu {
    AnimationWindow& window; //window object as reference
    //gameModes& gameMode;
    bool mapNameLagtTil; //maybe a kronglete solution to the problem, but at least it works
    public:
        list<Button> buttons {}; //We need a list to store buttons. This is because when we add a button to buttons, AnimationWindow saves a pointer to that address. When we add a new button, every object is moved, so the pointer is no longer valid. A list fixes this
        MainMenu(AnimationWindow& window);
        void render();
        void addButton(double x, double y, double width, double height, string label, function<void ()>);
};

struct PlayerInput {
    AnimationWindow& window;
    bool esc;
    bool w;
    bool a;
    bool s;
    bool Ctrl;
    bool d;
    bool q;
    bool e;
    bool m;
    bool p;
    bool space;
    bool LShift;
    bool LMouse;
    bool RMouse;
    PlayerInput(AnimationWindow& window);
    void getPlayerInput();
};

class Editor {
    public:
        AnimationWindow& window;
        Player player;
        World world;
        Point mouse;
        bool paused;
        bool leftMouseDownLastFrame;
        bool rightMouseDownLastFrame;
        Editor(AnimationWindow& window);
        void render();    
        void handlePlayerInput(PlayerInput& input);    
};


class GameHandler {
    AnimationWindow& window; //this is the window that is being drawn to every frame
    gameModes& gameMode; //tracks the current gamemode
    MainMenu menu; //the mainmenu-object
    PlayerInput playerInput;
    public:
        Editor editor;
        GameHandler(AnimationWindow& window, gameModes& gameMode);
        void update(); //the most important method, should be called every frame from main. Checks for updates and then renders the appropriate gamemode
        void checkForGameModeChange(); //checks for changes in gamemode - called be the update method
        void render(); //checks which gamemode is active and renders the appropriate gamemode - called by the update method
        void play();
};