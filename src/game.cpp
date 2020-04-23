#include "game.h"
#include "renderer/renderer.h"
#include "constants.h"

void Game::run() {
  init();

  MSG msg{};
  while (msg.message != WM_QUIT) {
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
      tick();
    }
  }
}

void Game::init() {
  m_window.init(Game::WindowProc);
  const auto windowHandle = m_window.getHandle();
  SetWindowLongPtr(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

  m_renderer.init(windowHandle);
  m_window.show(SW_SHOW);

  LARGE_INTEGER frequency{};
  QueryPerformanceFrequency(&frequency);
  m_tickFrequency = gsl::narrow<float>(frequency.QuadPart);

  QueryPerformanceCounter(&m_currentTime);

  m_world = std::make_unique<World>();
  m_world->init();
}

void Game::tick() {
  // TODO: Update input state

  LARGE_INTEGER newTime{};
  QueryPerformanceCounter(&newTime);
  const auto ticks = gsl::narrow_cast<float>(newTime.QuadPart - m_currentTime.QuadPart);
  m_currentTime    = newTime;

  auto frameTime = ticks / m_tickFrequency;

  while (frameTime > 0.0) {
    float deltaTime = std::min(frameTime, c_timestep);

#pragma region TODO : Move to system
    m_cubeTransform.rotation.x = fmod(m_cubeTransform.rotation.x + 90 * deltaTime, 360.0f);
    m_cubeTransform.rotation.y = fmod(m_cubeTransform.rotation.y + 60 * deltaTime, 360.0f);
    m_cubeTransform.rotation.z = fmod(m_cubeTransform.rotation.z + 45 * deltaTime, 360.0f);
#pragma endregion
    m_world->update(deltaTime);

    frameTime -= deltaTime;
    m_elapsedTime += deltaTime;
  }

  m_renderer.render(&m_cubeTransform);
}

LRESULT Game::WindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
  constexpr auto c_sizeMoveTimerId = 1;

  static bool s_inSizeMove = false;

  const auto game = reinterpret_cast<Game*>(GetWindowLongPtr(handle, GWLP_USERDATA));

  switch (message) {
    case WM_PAINT:
      if (s_inSizeMove && game) {
        game->tick();
      } else {
        PAINTSTRUCT ps;
        BeginPaint(handle, &ps);
        EndPaint(handle, &ps);
      }
      break;

    case WM_ENTERSIZEMOVE:
      s_inSizeMove = true;
      SetTimer(handle, c_sizeMoveTimerId, USER_TIMER_MINIMUM, nullptr);
      break;

    case WM_EXITSIZEMOVE:
      s_inSizeMove = false;
      KillTimer(handle, c_sizeMoveTimerId);
      // TODO: GetClientRect and resize
      // if (game) {}
      break;

    case WM_TIMER:
      if (s_inSizeMove && wParam == c_sizeMoveTimerId && game) {
        game->tick();
      }
      break;

    case WM_DESTROY:
      PostQuitMessage(0);
      break;
  }

  return DefWindowProc(handle, message, wParam, lParam);
}
