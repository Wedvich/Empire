#pragma once

// Temp input state thingy just to move camera around

class InputState {
public:
  bool A     = false;
  bool W     = false;
  bool S     = false;
  bool D     = false;
  bool R     = false;
  bool Shift = false;

  void reset();
};
