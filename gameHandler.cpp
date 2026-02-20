#include "gameHandler.h"

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
FPV::FPV(AnimationWindow& window): window{window}
{}

void FPV::render() {
    window.setBackgroundColor(Color::white);
    world.transformCoords(player);
    world.renderPoints(window);
}


//updateScreen-class----------------------------------------------------------

GameHandler::GameHandler(AnimationWindow& window, gameModes& gameMode): window(window), gameMode(gameMode), menu{window}, fpv{window}
{
    menu.addButton(0.45*windowWidth, 0.45*windowHeight, 0.1*windowWidth, 0.1*windowHeight, "Play", changeToFPV);
    //cout << "Knappen er lagt til" << endl;
}

void GameHandler::render() {
    switch (gameMode) { //Checks which gameMode the game is in to decide which render function to call
        case gameModes::mainMenu:
            menu.render();
            break;
        case gameModes::fpv:
            fpv.render();
            break;
    }
}

void GameHandler::update() {
    checkForGameModeChange();
    render();
}

void GameHandler::checkForGameModeChange() {
    if (newestGameMode != gameMode) {
        if (newestGameMode == gameModes::fpv && gameMode == gameModes::mainMenu) { //then the game is transitioning from menu to fpv
            for (auto& b : menu.buttons) { //går gjennom alle knappene i main menu og skjuler dem
                b.setVisible(false);
                cout << "byttet gamemode" << endl;
            }
        }
        gameMode = newestGameMode;
    }
}

void GameHandler::play() {

}