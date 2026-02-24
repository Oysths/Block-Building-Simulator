#pragma once
#include <SDL2/SDL.h>
#include "std_lib_facilities.h"

bool initJoystick();  // call once at startup
void updateJoystick(double& yaw, double& pitch, double& roll, double& throttle);
void closeJoystick(); // call at shutdown