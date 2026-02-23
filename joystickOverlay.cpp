#include "joystickOverlay.h"

int squareWidth = 100;
int spaceBetweenSquares = 50;
int circleRadius = 5;

Color c{128, 128, 128};
Color c2{20, 20, 20};


void showJoystickOverlay(double yaw, double pitch, double roll, double throttle, AnimationWindow& window, int windowWidth, int windowHeight) {
    Point square1_point {((windowWidth - 2 * squareWidth - spaceBetweenSquares) / 2), ((windowHeight - squareWidth) * 0.9)};
    Point square2_point {(square1_point.x + squareWidth + spaceBetweenSquares), square1_point.y};
    Point circle1 {(square1_point.x + yaw * squareWidth), (square1_point.y + (1 - throttle) * squareWidth)};
    Point circle2 {(square2_point.x + roll * squareWidth), (square2_point.y + (1 - pitch) * squareWidth)};
    window.draw_rectangle(square1_point, squareWidth, squareWidth, c);
    window.draw_rectangle(square2_point, squareWidth, squareWidth, c);
    circle1.x = (square1_point.x + yaw * squareWidth);
    circle1.y = (square1_point.y + (1 - throttle) * squareWidth);
    circle2.x = (square2_point.x + roll * squareWidth);
    circle2.y = (square2_point.y + (1 - pitch) * squareWidth);
    window.draw_circle(circle1, circleRadius, c2);
    window.draw_circle(circle2, circleRadius, c2);
}
