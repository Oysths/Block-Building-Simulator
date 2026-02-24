#include "getInput.h"

static SDL_Joystick* joystick = nullptr;

bool initJoystick() {

    if (SDL_NumJoysticks() < 1) {
        std::cout << "No joystick detected\n";
        return false;
    }

    joystick = SDL_JoystickOpen(0);

    if (!joystick) {
        std::cout << "Failed to open joystick\n";
        return false;
    }

    std::cout << "Joystick connected\n";
    return true;
}

static double applyDeadzone(double value, double deadzone = 0.05) {
    if (std::fabs(value) < deadzone)
        return 0.0;
    return value;
}

void updateJoystick(double& yaw, double& pitch, double& roll, double& throttle) {
    // if (!joystick)
    //     return;

    SDL_JoystickUpdate();

    roll  = (SDL_JoystickGetAxis(joystick, 0) + 32768.0) / 65535.0;
    pitch = (SDL_JoystickGetAxis(joystick, 1) + 32768.0) / 65535.0;
    yaw   = (SDL_JoystickGetAxis(joystick, 3) + 32768.0) / 65535.0;
    throttle = (SDL_JoystickGetAxis(joystick, 2) + 32768.0) / 65535.0;

    // roll  = applyDeadzone(roll);
    // pitch = applyDeadzone(pitch);
    // yaw   = applyDeadzone(yaw);
}

void closeJoystick() {
    if (joystick) {
        SDL_JoystickClose(joystick);
        joystick = nullptr;
    }
}