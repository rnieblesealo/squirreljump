#include "input-controller.h"
#include <raylib.h>

bool InputController::GetKey(int keyCode) { return IsKeyPressed(keyCode); }
bool InputController::GetKeyDown(int keyCode) { return IsKeyDown(keyCode); }
bool InputController::GetKeyUp(int keyCode) { return IsKeyUp(keyCode); }
