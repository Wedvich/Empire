#include "game.h"
#include "constants.h"
#include "renderer/renderer.h"

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

#pragma region TODO : Move to systems
    m_cubeTransform.rotation.y = std::fmod(m_cubeTransform.rotation.y + 45 * deltaTime, 360.0f);
#pragma endregion

#pragma region TODO : Move to systems
    auto camera = m_renderer.getCamera();
    if (m_inputState.R) {
      camera->m_eye = {0.0f, 0.7f, 1.5f};
      camera->m_at  = {0.0f, 0.1f, 0.0f};
    }

    if (m_inputState.A) {
      camera->m_eye.x -= 2.0f * deltaTime;
      camera->m_at.x -= 2.0f * deltaTime;
    }

    if (m_inputState.D) {
      camera->m_eye.x += 2.0f * deltaTime;
      camera->m_at.x += 2.0f * deltaTime;
    }

    if (!m_inputState.Shift) {
      if (m_inputState.W) {
        camera->m_eye.z -= 2.0f * deltaTime;
        camera->m_at.z -= 2.0f * deltaTime;
      }

      if (m_inputState.S) {
        camera->m_eye.z += 2.0f * deltaTime;
        camera->m_at.z += 2.0f * deltaTime;
      }
    } else {
      if (m_inputState.W) {
        camera->m_eye.y += 2.0f * deltaTime;
        camera->m_at.y += 2.0f * deltaTime;
      }

      if (m_inputState.S) {
        camera->m_eye.y -= 2.0f * deltaTime;
        camera->m_at.y -= 2.0f * deltaTime;
      }
    }
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
      if (game) {
        game->m_inputState.reset();
      }
      SetTimer(handle, c_sizeMoveTimerId, USER_TIMER_MINIMUM, nullptr);
      break;

    case WM_EXITSIZEMOVE:
      s_inSizeMove = false;
      KillTimer(handle, c_sizeMoveTimerId);
      // TODO: GetClientRect and resize
      // if (game) {}
      break;

    case WM_KEYDOWN:
      if (game) {
        switch (wParam) {
          case VK_SHIFT:
            game->m_inputState.Shift = true;
            break;
          case 0x57:
            game->m_inputState.W = true;
            break;
          case 0x41:
            game->m_inputState.A = true;
            break;
          case 0x53:
            game->m_inputState.S = true;
            break;
          case 0x44:
            game->m_inputState.D = true;
            break;
          case 0x52:
            game->m_inputState.R = true;
            break;
        }
      }
      break;

    case WM_KEYUP:
      if (game) {
        switch (wParam) {
          case VK_SHIFT:
            game->m_inputState.Shift = false;
            break;
          case 0x57:
            game->m_inputState.W = false;
            break;
          case 0x41:
            game->m_inputState.A = false;
            break;
          case 0x53:
            game->m_inputState.S = false;
            break;
          case 0x44:
            game->m_inputState.D = false;
            break;
          case 0x52:
            game->m_inputState.R = false;
            break;
        }
      }
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
