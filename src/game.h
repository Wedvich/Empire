#pragma once

#include "game_window.h"
#include "renderer/renderer.h"
#include "world.h"
#include "components/transform.h"

class Game {
public:
  void run();

private:
  GameWindow m_window;
  Renderer   m_renderer;

  std::unique_ptr<World> m_world;

  LARGE_INTEGER m_currentTime{};
  float         m_tickFrequency = 1000000.0f;
  float         m_elapsedTime   = 0.0f;
  float         m_cubeState     = 0.0f;

  TransformComponent m_cubeTransform = {};

  void init();
  void tick();

  static LRESULT CALLBACK WindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};
