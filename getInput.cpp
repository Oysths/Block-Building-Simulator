#include "getInput.h"
#include <SDL2/SDL.h>
#include "std_lib_facilities.h"

SDL_Joystick* js = SDL_JoystickOpen(0);
SDL_Event e;

void getJoystickInputs(double& yaw, double& pitch, double& roll, double& throttle, SDL_Event e) {
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_JOYAXISMOTION) {
            switch ((int)e.jaxis.axis) {
                case 0:
                    roll = (static_cast<double>(e.jaxis.value + 32768) / 65535);
                    break;
                case 1:
                    pitch = (static_cast<double>(e.jaxis.value + 32768) / 65535);
                    break;
                case 2:
                    throttle = (static_cast<double>(e.jaxis.value + 32768) / 65535);
                    break;
                case 3:
                    yaw = (static_cast<double>(e.jaxis.value + 32768) / 65535);
                    break;
            }
        }
    }
    SDL_Delay(10);
}
