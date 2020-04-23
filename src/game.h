#pragma once

#include "game_window.h"
#include "renderer.h"

class Game {
public:
  void run();

private:
  GameWindow m_window;
  Renderer m_renderer;

  float m_tickFrequency = 1000000.0f;
  LARGE_INTEGER m_currentTime{};
  float m_state = 0.0f;

  void init();
  void tick();

  static LRESULT CALLBACK WindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};
