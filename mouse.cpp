#include <iostream>
#include <GL/freeglut.h>
#include "keyboard.hpp"

int lastX = 1024 / 2;
int mouseDeltaX;

void mouseMovementHandler(int x, int y) {
    int centerX = 1024 / 2;
    int centerY = 512 / 2;

    int deltaBuffer = x - centerX;

    mouseDeltaX = deltaBuffer;

    if (!key.x) {
        glutWarpPointer(centerX, centerY);
        glutSetCursor(GLUT_CURSOR_NONE);
    }
    else
    {
        mouseDeltaX = 0;
        glutSetCursor(GLUT_CURSOR_CROSSHAIR);
    }
    
    lastX = centerX;
}