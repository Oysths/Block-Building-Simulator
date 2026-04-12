#include "gameHandler.h"

gameModes newestGameMode = gameModes::mainMenu;

int mapIdx = 0;

filesystem::path mapsFilePath{"Data/Maps"};

void changeToEditor() {
    newestGameMode = gameModes::editor;
}

string getMapNameFromIndex(int idx) {
    auto map = mapsFilePath.begin();
    int counter = 0;
    string mapFileName = "";
    for (auto& map : filesystem::directory_iterator(mapsFilePath)) {
        if (counter == idx) {
            mapFileName = map.path().stem().string();
        }
        counter++;
    }
    return mapFileName;
}

TextBox mapName {
{static_cast<int>(round(0.2*windowWidth)), static_cast<int>(round(0.4*windowHeight))},
static_cast<int>(round(0.2*windowWidth)), 
static_cast<int>(round(0.05*windowHeight)),
getMapNameFromIndex(0)};

void nextMap() {
    int numberOfMaps = distance(filesystem::directory_iterator(mapsFilePath), filesystem::directory_iterator{});
    if (mapIdx == numberOfMaps - 1) {
        mapIdx = 0;
    } else {
        mapIdx++;
    }
    mapName.setText(getMapNameFromIndex(mapIdx));
}

void previousMap() {
    int numberOfMaps = distance(filesystem::directory_iterator(mapsFilePath), filesystem::directory_iterator{});
    if (mapIdx == 0) {
        mapIdx = numberOfMaps - 1;
    } else {
        mapIdx--;
    }
    mapName.setText(getMapNameFromIndex(mapIdx));
}

//MainMenu-class--------------------------------------------------------------
MainMenu::MainMenu(AnimationWindow& window): window(window), mapNameLagtTil(false)
{}

void MainMenu::render() {
    window.setBackgroundColor(Color::blue);
    if (!mapNameLagtTil) { //adds the mapName to window if it hasn't done so yet (only executed first frame)
        window.add(mapName);
        mapNameLagtTil = true;
    }
    //cout << "Prøver å rendere mainmenu" << endl;
    
}

void MainMenu::addButton(double x, double y, double width, double height, string label, function<void ()> function) {
    Point p {
        static_cast<int>(round(x)), 
        static_cast<int>(round(y))
    };
    Button b{p,
        static_cast<int>(width),
        static_cast<int>(height),
        label
        };
    b.setCallback(function);
    buttons.emplace_back(b);
    window.add(buttons.back());
}


//Editor-class------------------------------------------------------------------
Editor::Editor(AnimationWindow& window): window{window}, leftMouseDownLastFrame{false}
{}

void Editor::render() {
    window.setBackgroundColor(Color::white);
    player.getTrigValues();
    world.transformCoords(player);
    world.sortBlocks(); //makes the blocks render in the correct order
    //world.renderLines(window);
    //world.renderSurfaces(window);
    Point midten {windowWidth/2, windowHeight/2};
    try { //tries to render the world's (jojo ref) blocks
        world.renderBlocks(window);
    } catch(...) { //catches all exeptions
        cout << "Could not render blocks." << endl;
    }
    window.draw_circle(midten, 6, player.activeColorColor, Color::white);
}

void Editor::handlePlayerInput(PlayerInput& input) {
    bool leftMouseverdiHolder = input.LMouse; //this ensures you can't hold the left mouse for more than one frame
    if (input.LMouse && leftMouseDownLastFrame) { 
        input.LMouse = false;
    }
    leftMouseDownLastFrame = leftMouseverdiHolder;


    bool rightMouseverdiHolder = input.RMouse; //same thing for the right side
    if (input.RMouse && rightMouseDownLastFrame) { 
        input.RMouse = false;
    }
    rightMouseDownLastFrame = rightMouseverdiHolder;


    bool leftArrowverdiHolder = input.LArrow; //same thing for the arrow buttons
    if (input.LArrow && leftArrowDownLastFrame) { 
        input.LArrow = false;
    }
    leftArrowDownLastFrame = leftArrowverdiHolder;

    bool rightArrowverdiHolder = input.RArrow; //same thing for the arrow buttons
    if (input.RArrow && rightArrowDownLastFrame) { 
        input.RArrow = false;
    }
    rightArrowDownLastFrame = rightArrowverdiHolder;  



    if (input.LMouse && input.RMouse) { //cant place and destroy a block at the same time
        input.LMouse = false;
    }

    if (input.LArrow && input.RArrow) { //cant do both at the same time
        input.LArrow = false;
    }

    if (input.w) {
        player.move("W");
    } if (input.a) {
        player.move("A");
    } if (input.s) {
        player.move("S");
    } if (input.d) {
        player.move("D");
    } if (input.space) {
        player.move("SPACE");
    } if (input.LShift) {
        player.move("LSHIFT"); 
    } if (input.q) {
        player.move("Q");
    } if (input.e) {
        player.move("E");
    } if (input.RMouse) { //for meg blir leftmousebutton høyre og motsatt, litt cursed men
        //cout << "Place";
        world.placeBlock(player);
    } if (input.LMouse) {
        //cout << "Break";
        world.breakBlock(player);
    } if (input.m) {
        fov -= 0.01;
    } if (input.p) {
        fov += 0.01;
    } if (input.LArrow) {
        player.previousColor();
    } if (input.RArrow) {
        player.nextColor();
    } 
    if (input.mouseWheel) { //if mouseWheel is non-zero (means it has moved), then iterate through color the corresponding amount of times
        if (input.mouseWheel > 0) {
            for (int i = 0; i < input.mouseWheel; i++) {
                player.nextColor();
            }
        } else {
            for (int i = 0; i < -input.mouseWheel; i++) {
                player.previousColor();
            }
        }
    }
    Point newmouse = window.get_mouse_coordinates();
    double dXZ = newmouse.x - mouse.x;
    double dYZ = newmouse.y - mouse.y;
    mouse = newmouse;
    player.angles.at(0) -= dXZ/130; //-= since when moving mouse pointer, you move the blocks away, not with it
    player.angles.at(1) -= dYZ/500;


    //cout << "Spillerinput sjekket" << endl;
}


//PlayerInput-class------------------------------------------------------------
PlayerInput::PlayerInput(AnimationWindow& window): window{window}, esc{false}, w{false}, a{false}, s{false}, d{false}, q{false}, e{false}, m{false}, p{false}, space{false}, LShift{false}, LMouse{false}, RMouse{false}
{}


void PlayerInput::setPlayerInput() {
    //cout << "Sjekker spillerinput" << endl;
    mouseWheel = round(window.get_delta_mouse_wheel());
    bool wPressed = window.is_key_down(KeyboardKey::W);
    w = wPressed;
    bool aPressed = window.is_key_down(KeyboardKey::A);
    a = aPressed;
    bool sPressed = window.is_key_down(KeyboardKey::S);
    s = sPressed;
    bool dPressed = window.is_key_down(KeyboardKey::D);
    d = dPressed;
    bool qPressed = window.is_key_down(KeyboardKey::Q);
    q = qPressed;
    bool ePressed = window.is_key_down(KeyboardKey::E);
    e = ePressed;
    bool mPressed = window.is_key_down(KeyboardKey::M);
    m = mPressed;
    bool pPressed = window.is_key_down(KeyboardKey::P);
    p = pPressed;
    bool ctrlPressed = window.is_key_down(KeyboardKey::LEFT_CTRL);
    ctrl = ctrlPressed;
    bool spacePressed = window.is_key_down(KeyboardKey::SPACE);
    space = spacePressed;
    bool escPressed = window.is_key_down(KeyboardKey::ESCAPE);
    esc = escPressed;
    bool leftShiftPressed = window.is_key_down(KeyboardKey::LEFT_SHIFT);
    LShift = leftShiftPressed;
    bool leftArrowPressed = window.is_key_down(KeyboardKey::LEFT);
    LArrow = leftArrowPressed;
    bool rightArrowPressed = window.is_key_down(KeyboardKey::RIGHT);
    RArrow = rightArrowPressed;
    bool leftMousePressed = window.is_left_mouse_button_down();
    LMouse = leftMousePressed;
    bool rightMousePressed = window.is_right_mouse_button_down();
    RMouse = rightMousePressed;
}


//updateScreen-class----------------------------------------------------------

GameHandler::GameHandler(AnimationWindow& window, gameModes& gameMode): window(window), gameMode(gameMode), menu{window}, editor{window}, playerInput{window}, escDownLastFrame(false)
{
    //menu.addButton(0.44*windowWidth, 0.44*windowHeight, 0.12*windowWidth, 0.12*windowHeight, "Play", changeToEditor); //will be added later when drones are added
    menu.addButton(0.45*windowWidth, 0.6*windowHeight, 0.1*windowWidth, 0.1*windowHeight, "Editor", changeToEditor);
    menu.addButton(0.2*windowWidth, 0.3*windowHeight, 0.06*windowWidth, 0.06*windowHeight, "<-", previousMap);
    menu.addButton(0.3*windowWidth, 0.3*windowHeight, 0.06*windowWidth, 0.06*windowHeight, "->", nextMap);
    //menu.addButton(0.7*windowWidth, 0.3*windowHeight, 0.06*windowWidth, 0.06*windowHeight, "<-", changeToEditor);
    //menu.addButton(0.8*windowWidth, 0.3*windowHeight, 0.06*windowWidth, 0.06*windowHeight, "->", changeToEditor);
    //TextBox d {{300, 300}, 300, 300, "Drone selector"};
    //window.add(d);
    //TextBox m {"Map selector"};
    //cout << "Knappen er lagt til" << endl;
}

void GameHandler::render() {
    switch (gameMode) { //Checks which gameMode the game is in to decide which render function to call
        case gameModes::mainMenu:
            menu.render();
            break;
        case gameModes::editor:
            editor.render();
            break;
    }
}

void GameHandler::update() {
    playerInput.setPlayerInput();
    checkForGameModeChange();
    if (gameMode == gameModes::editor) {
        editor.handlePlayerInput(playerInput);
        //cout << "Ber om player input" << endl;
    }
    if (playerInput.s && playerInput.ctrl) { //save with ctrl s
        editor.world.saveMapData();
    }
    handlePlayerInput();
    //cout << "renderer" << endl;
    render();
}

void GameHandler::checkForGameModeChange() {
    if (newestGameMode != gameMode) {
        if (newestGameMode == gameModes::editor && gameMode == gameModes::mainMenu) { //then the game is transitioning from menu to editor
            for (auto& b : menu.buttons) { //går gjennom alle knappene i main menu og skjuler dem
                b.setVisible(false);
                mapName.setVisible(false);
            }
            editor.world.loadMap(mapName.getText()); //initialize stuff so everythings ready
            editor.player.resetPlayer();
            editor.mouse = window.get_mouse_coordinates(); //So that the start reference mouse pointer value is the actual start value and not (0, 0)
            editor.paused = false;
        }
        if (newestGameMode == gameModes::mainMenu && gameMode == gameModes::editor) {
            for (auto& b : menu.buttons) {
                b.setVisible(true);
                mapName.setVisible(true);
            }
        }
        gameMode = newestGameMode;
        //cout << "byttet gamemode" << endl;
    }
}

void GameHandler::handlePlayerInput() {
    bool escVerdiHolder = playerInput.esc;
    if (playerInput.esc && escDownLastFrame) {
        playerInput.esc = false;
    }
    escDownLastFrame = escVerdiHolder;


    if (playerInput.esc) {
        switch (gameMode)
        {
        case (gameModes::editor):
            newestGameMode = gameModes::mainMenu;
            break;
        
        case (gameModes::mainMenu):
            window.close();
            break;
        }
    }
}

void GameHandler::confirmMaps() {
    int numberOfMaps = distance(filesystem::directory_iterator(mapsFilePath), filesystem::directory_iterator{});
    if (!numberOfMaps) {
        filesystem::path fileName = mapsFilePath;
        string mapNameString = "an_unnamed_map";
        fileName += "/" + mapNameString + ".txt";
        ofstream outputStream{fileName}; //ofstream automatically creates file if it doesn't exist
        outputStream << "";
        mapName.setText(mapNameString);
    }
}

void GameHandler::play() {

}