#include "gameHandler.h"
#include "joystickOverlay.h"
#include "getInput.h"

gameModes newestGameMode = gameModes::mainMenu;

void changeToFPV() {
    newestGameMode = gameModes::fpv;
}

//MainMenu-class--------------------------------------------------------------
MainMenu::MainMenu(AnimationWindow& window): window(window)
{}

void MainMenu::render() {
    window.setBackgroundColor(Color::blue);
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
        "Play"
        };
    b.setCallback(function);
    buttons.emplace_back(b);
    window.add(buttons.back());
}


//FPV-class------------------------------------------------------------------
FPV::FPV(AnimationWindow& window): window{window}, leftMouseDownLastFrame{false}
{}

void FPV::render() {
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

void FPV::getPlayerInput() {
    //cout << "Sjekker spillerinput" << endl;
    
    bool wPressed = window.is_key_down(KeyboardKey::W);
    bool aPressed = window.is_key_down(KeyboardKey::A);
    bool sPressed = window.is_key_down(KeyboardKey::S);
    bool dPressed = window.is_key_down(KeyboardKey::D);
    bool qPressed = window.is_key_down(KeyboardKey::Q);
    bool ePressed = window.is_key_down(KeyboardKey::E);
    bool mPressed = window.is_key_down(KeyboardKey::M);
    bool pPressed = window.is_key_down(KeyboardKey::P);
    bool spacePressed = window.is_key_down(KeyboardKey::SPACE);
    bool leftShiftPressed = window.is_key_down(KeyboardKey::LEFT_SHIFT);
    bool escPressed = window.is_key_down(KeyboardKey::ESCAPE);
    bool leftMousePressed = window.is_left_mouse_button_down();
    bool rightMousePressed = window.is_right_mouse_button_down();


    bool leftMouseverdiHolder = leftMousePressed; //this ensures you can't hold the left mouse for more than one frame
    if (leftMousePressed && leftMouseDownLastFrame) { 
        leftMousePressed = false;
    }
    leftMouseDownLastFrame = leftMouseverdiHolder;

    bool rightMouseverdiHolder = rightMousePressed; //same thing for the right side
    if (rightMousePressed && rightMouseDownLastFrame) { 
        rightMousePressed = false;
    }
    rightMouseDownLastFrame = rightMouseverdiHolder;


    if (leftMousePressed && rightMousePressed) { //cant place and destroy a block at the same time
        leftMousePressed = false;
    }

    if (wPressed || aPressed || sPressed || dPressed || spacePressed || leftShiftPressed || qPressed || ePressed || leftMousePressed|| rightMousePressed || mPressed || pPressed) {
        if (wPressed) {
            player.move("W");
        } if (aPressed) {
            player.move("A");
        } if (sPressed) {
            player.move("S");
        } if (dPressed) {
            player.move("D");
        } if (spacePressed) {
            player.move("SPACE");
        } if (leftShiftPressed) {
            player.move("LSHIFT"); 
        } if (qPressed) {
            player.move("Q");
        } if (ePressed) {
            player.move("E");
        } if (rightMousePressed) { //for meg blir leftmousebutton høyre og motsatt, litt cursed men
            //cout << "Place";
            world.placeBlock(player);
        } if (leftMousePressed) {
            //cout << "Break";
            world.breakBlock(player);
        } if (mPressed) {
            fov -= 0.01;
        } if (pPressed) {
            fov += 0.01;
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


//updateScreen-class----------------------------------------------------------

GameHandler::GameHandler(AnimationWindow& window, gameModes& gameMode): window(window), gameMode(gameMode), menu{window}, fpv{window}
{
    menu.addButton(0.45*windowWidth, 0.45*windowHeight, 0.1*windowWidth, 0.1*windowHeight, "Play", changeToFPV);
    //cout << "Knappen er lagt til" << endl;
}

double roll = 0.5;
double pitch = 0.5;
double yaw = 0.5;
double throttle = 0;

void GameHandler::render() {
    switch (gameMode) { //Checks which gameMode the game is in to decide which render function to call
        case gameModes::mainMenu:
            menu.render();
            break;
        case gameModes::fpv:
            fpv.render();
            getJoystickInput(yaw, pitch, roll, throttle, e);
            showJoystickOverlay(yaw, pitch, roll, throttle, window, windowWidth, windowHeight);
            break;
    }
}

void GameHandler::update() {
    checkForGameModeChange();
    if (gameMode == gameModes::fpv) {
        fpv.getPlayerInput();
        //cout << "Ber om player input" << endl;
    }
    //cout << "renderer" << endl;
    render();
}

void GameHandler::checkForGameModeChange() {
    if (newestGameMode != gameMode) {
        if (newestGameMode == gameModes::fpv && gameMode == gameModes::mainMenu) { //then the game is transitioning from menu to fpv
            for (auto& b : menu.buttons) { //går gjennom alle knappene i main menu og skjuler dem
                b.setVisible(false);
                fpv.mouse = window.get_mouse_coordinates(); //So that the start reference mouse pointer value is the actual start value and not (0, 0)
            }
        }
        gameMode = newestGameMode;
        //cout << "byttet gamemode" << endl;
    }
}

void GameHandler::play() {

}