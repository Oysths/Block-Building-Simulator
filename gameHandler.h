#pragma once
#include "std_lib_facilities.h"
#include "render3d.h"
#include <list>
//#include "getData.h"

//these are the gamemodes you can switch between. Note that fpv is not implemented in the code at the moment, that is because the plan was originally to add a drone gamemode as well.
enum class gameModes {mainMenu, editor, fpv};

//extern gameModes gameMode;
void changeToEditor(); //changes to the editor-gamemode
void nextMap(); //changes to the editor-gamemode
void previousMap(); //changes to the editor-gamemode
string getMapNameFromIndex(int idx);

//class for main menu
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

//class for the playerinput, whether or not a button is pressed is stored in these variables
struct PlayerInput {
    AnimationWindow& window;
    bool esc;
    bool w;
    bool a;
    bool s;
    bool ctrl;
    bool d;
    bool q;
    bool e;
    bool m;
    bool p;
    bool space;
    bool LShift;
    bool LMouse;
    bool RMouse;
    bool LArrow;
    bool RArrow;
    int mouseWheel;
    PlayerInput(AnimationWindow& window);
    void setPlayerInput();
};

//class for the editor
class Editor {
    public:
        AnimationWindow& window;
        Player player;
        World world;
        Point mouse;
        bool paused;
        bool leftMouseDownLastFrame;
        bool rightMouseDownLastFrame;
        bool leftArrowDownLastFrame;
        bool rightArrowDownLastFrame;
        Editor(AnimationWindow& window);
        void render();    
        void handlePlayerInput(PlayerInput& input);    
};

//keeps track of practically everything in the game
class GameHandler {
    AnimationWindow& window; //this is the window that is being drawn to every frame
    gameModes& gameMode; //tracks the current gamemode
    MainMenu menu; //the mainmenu-object
    PlayerInput playerInput;
    bool escDownLastFrame; //gamehandler handles esc- which should take you one step "back"; from editor to menu, and from menu to quitting the game
    public:
        Editor editor;
        GameHandler(AnimationWindow& window, gameModes& gameMode);
        void update(); //the most important method, should be called every frame from main. Checks for updates and then renders the appropriate gamemode
        void checkForGameModeChange(); //checks for changes in gamemode - called be the update method
        void render(); //checks which gamemode is active and renders the appropriate gamemode - called by the update method
        //void play();
        void handlePlayerInput();
        void confirmMaps(); //makes sure that there is at least one map in the map folder, will create an empty map if not
};