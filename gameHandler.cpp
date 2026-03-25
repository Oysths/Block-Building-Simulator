#include "gameHandler.h"

gameModes newestGameMode = gameModes::mainMenu;

void changeToEditor() {
    newestGameMode = gameModes::editor;
}

void changeDrone(int& droneIdx, bool direction) {
    if (direction) {
        droneIdx++;
    } else {
        droneIdx--;
    }
}


//MainMenu-class--------------------------------------------------------------
MainMenu::MainMenu(AnimationWindow& window): window(window)
{}

void MainMenu::render() {
    window.setBackgroundColor(Color::blue);
    player.droneIdxCheck;
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
    world.renderBlocks(window);
    window.draw_circle(midten, 3, Color::black);
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




    if (input.LMouse && input.RMouse) { //cant place and destroy a block at the same time
        input.LMouse = false;
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


void PlayerInput::getPlayerInput() {
    //cout << "Sjekker spillerinput" << endl;
    
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
    bool spacePressed = window.is_key_down(KeyboardKey::SPACE);
    space = spacePressed;
    bool leftShiftPressed = window.is_key_down(KeyboardKey::LEFT_SHIFT);
    LShift = leftShiftPressed;
    bool escPressed = window.is_key_down(KeyboardKey::ESCAPE);
    esc = escPressed;
    bool leftMousePressed = window.is_left_mouse_button_down();
    LMouse = leftMousePressed;
    bool rightMousePressed = window.is_right_mouse_button_down();
    RMouse = rightMousePressed;
}


//updateScreen-class----------------------------------------------------------

GameHandler::GameHandler(AnimationWindow& window, gameModes& gameMode): window(window), gameMode(gameMode), menu{window}, editor{window}, playerInput{window}
{
    menu.addButton(0.44*windowWidth, 0.44*windowHeight, 0.12*windowWidth, 0.12*windowHeight, "Play", changeToEditor);
    menu.addButton(0.45*windowWidth, 0.6*windowHeight, 0.1*windowWidth, 0.1*windowHeight, "Editor", changeToEditor);
    menu.addButton(0.35*windowWidth, 0.2*windowHeight, 0.05*windowWidth, 0.05*windowHeight, "=>", changeDrone(menu.player::droneIdx, 1));
    menu.addButton(0.05*windowWidth, 0.2*windowHeight, 0.05*windowWidth, 0.05*windowHeight, "<=", changeDrone(menu.player::droneIdx, 0));
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
    playerInput.getPlayerInput();
    checkForGameModeChange();
    if (gameMode == gameModes::editor) {
        editor.handlePlayerInput(playerInput);
        //cout << "Ber om player input" << endl;
    }
    //cout << "renderer" << endl;
    render();
}

void GameHandler::checkForGameModeChange() {
    if (newestGameMode != gameMode) {
        if (newestGameMode == gameModes::editor && gameMode == gameModes::mainMenu) { //then the game is transitioning from menu to editor
            for (auto& b : menu.buttons) { //går gjennom alle knappene i main menu og skjuler dem
                b.setVisible(false);
                editor.mouse = window.get_mouse_coordinates(); //So that the start reference mouse pointer value is the actual start value and not (0, 0)
                editor.paused = false;
            }
        }
        gameMode = newestGameMode;
        //cout << "byttet gamemode" << endl;
    }
}

void GameHandler::play() {

}