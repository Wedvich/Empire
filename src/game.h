#pragma once

#include "components/transform.h"
#include "game_window.h"
#include "renderer/renderer.h"
#include "world.h"
#include "input_state.h"
#include <input_state.h>

class Game {
public:
  void run();

private:
  GameWindow m_window;
  Renderer   m_renderer;
  InputState m_inputState;

  std::unique_ptr<World> m_world;

  LARGE_INTEGER m_currentTime{};
  float         m_tickFrequency = 1000000.0f;
  float         m_elapsedTime   = 0.0f;
  float         m_cubeState     = 0.0f;

  TransformComponent m_cubeTransform = {};
  float              m_cameraAngle   = 0.0f;

  void init();
  void tick();

  static LRESULT CALLBACK WindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};
