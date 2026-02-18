#pragma once
#include "std_lib_facilities.h"
#include "AnimationWindow.h"


enum class gameModes {mainMenu, editor, fpv};

class updateScreen {
    AnimationWindow& window; //dette blir vinduet som blir tegnet til hele tiden
    const gameModes& gameMode;
    public:
        updateScreen(const AnimationWindow& window, const gameModes& gameMode);
};