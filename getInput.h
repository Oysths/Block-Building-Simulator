#include <SDL2/SDL.h>
#include "std_lib_facilities.h"

extern SDL_Joystick* js;
extern SDL_Event e;

int joystickSetup();
void getJoystickInput(double& yaw, double& pitch, double& roll, double& throttle, SDL_Event e);