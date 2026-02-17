#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "std_lib_facilities.h"
#include "AnimationWindow.h"

int windowWidth = 1000;
int windowHeight = 650;
 
int main(int argc, char** argv)
{
    double roll = 0.5;
    double pitch = 0.5;
    double yaw = 0.5;
    double throttle = 0;
    Point square1_point {150, 150};
    int square_width = 200;
    Point square2_point {(square1_point.x + square_width + 100), 150};
    Point circle1 {(square1_point.x + yaw * square_width), (square1_point.y + (1 - throttle) * square_width)};
    Point circle2 {(square2_point.x + roll * square_width), (square2_point.y + (1 - pitch) * square_width)};
    

    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
        return 1;
    }
 
    int count = SDL_NumJoysticks();
    std::cout << "Antall joysticker funnet: " << count << "\n";
 
    if (count == 0) {
        std::cout << "Ingen joysticker funnet. Avslutter.\n";
        SDL_Quit();
        return 0;
    }
 
    SDL_Joystick* js = SDL_JoystickOpen(0);
    if (!js) {
        std::cerr << "Feil ved åpning av joystick: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }
 
    std::cout << "Åpnet joystick: " << SDL_JoystickName(js) << "\n";
    std::cout << "Aksler: "   << SDL_JoystickNumAxes(js)
<< "  Knapper: " << SDL_JoystickNumButtons(js)
<< "  Hatter: "  << SDL_JoystickNumHats(js) << "\n";
 
    SDL_Event e;

    AnimationWindow window(50, 50, windowWidth, windowHeight, SDL_JoystickName(js));

    while (!window.should_close()) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_JOYAXISMOTION) {
                switch ((int)e.jaxis.axis) {
                    case 0:
                        roll = (static_cast<double>(e.jaxis.value + 32768) / 65536);
                        break;
                    case 1:
                        pitch = (static_cast<double>(e.jaxis.value + 32768) / 65536);
                        break;
                    case 2:
                        throttle = (static_cast<double>(e.jaxis.value + 32768) / 65536);
                        break;
                    case 3:
                        yaw = (static_cast<double>(e.jaxis.value + 32768) / 65536);
                        break;
                }
            }
        }
        window.draw_rectangle(square1_point, square_width, square_width, Color::gray);
        window.draw_rectangle(square2_point, square_width, square_width, Color::gray);
        circle1.x = (square1_point.x + yaw * square_width);
        circle1.y = (square1_point.y + (1 - throttle) * square_width);
        circle2.x = (square2_point.x + roll * square_width);
        circle2.y = (square2_point.y + (1 - pitch) * square_width);
        window.draw_circle(circle1, 10, Color::black);
        window.draw_circle(circle2, 10, Color::black);
        window.next_frame();
        SDL_Delay(5);
    }
 
    SDL_JoystickClose(js);
    SDL_Quit();
    return 0;
}