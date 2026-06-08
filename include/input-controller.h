#pragma once

class InputController
{
public:
  bool GetKey(int keyCode);     // true for single frame keyCode is pressed
  bool GetKeyDown(int keyCode); // true as long as keyCode is pressed
  bool GetKeyUp(int keyCode);   // true for single frame keyCode is lifted

private:
};
