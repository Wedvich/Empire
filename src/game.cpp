#include "game.h"
#include "constants.h"
#include "renderer/renderer.h"
#include <utils\transform.h>

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
    constexpr auto resetBinding       = std::make_pair(InputId::R, 0);
    constexpr auto moveForwardBinding = std::make_pair(InputId::W, 0);
    constexpr auto moveBackBinding    = std::make_pair(InputId::S, 0);
    constexpr auto moveUpBinding      = std::make_pair(InputId::W, InputModifier::Shift);
    constexpr auto moveDownBinding    = std::make_pair(InputId::S, InputModifier::Shift);
    constexpr auto strafeLeftBinding  = std::make_pair(InputId::A, 0);
    constexpr auto strafeRightBinding = std::make_pair(InputId::D, 0);

    auto camera = m_renderer.getCamera();

    auto isBindingActive = [&](std::pair<InputId, int> binding) {
      return std::find_if(m_inputMapper.m_activeButtons.begin(),
                          m_inputMapper.m_activeButtons.end(),
                          [&](std::pair<InputId, int> activeBinding) {
                            return activeBinding == binding;
                          }) != m_inputMapper.m_activeButtons.end();
    };

    if (isBindingActive(resetBinding)) {
      camera->m_eye = {0.0f, 0.5f, -5.0f};
      camera->m_at  = {0.0f, 0.1f, 0.0f};
    }

    if (isBindingActive(moveForwardBinding)) {
      camera->m_eye.z += 2.0f * deltaTime;
      camera->m_at.z += 2.0f * deltaTime;
    }

    if (isBindingActive(moveBackBinding)) {
      camera->m_eye.z -= 2.0f * deltaTime;
      camera->m_at.z -= 2.0f * deltaTime;
    }

    if (isBindingActive(moveUpBinding)) {
      camera->m_eye.y += 2.0f * deltaTime;
      camera->m_at.y += 2.0f * deltaTime;
    }

    if (isBindingActive(moveDownBinding)) {
      camera->m_eye.y -= 2.0f * deltaTime;
      camera->m_at.y -= 2.0f * deltaTime;
    }

    if (isBindingActive(strafeLeftBinding)) {
      camera->m_eye.x -= 2.0f * deltaTime;
      camera->m_at.x -= 2.0f * deltaTime;
    }

    if (isBindingActive(strafeRightBinding)) {
      camera->m_eye.x += 2.0f * deltaTime;
      camera->m_at.x += 2.0f * deltaTime;
    }

#pragma endregion

    m_world->update(deltaTime);

    frameTime -= deltaTime;
    m_elapsedTime += deltaTime;
  }

  TransformProxy cubeTransform{};
  Utils::copyTransform(m_cubeTransform, &cubeTransform);

  m_renderer.render(&cubeTransform);
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
        game->m_inputMapper.clear();
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
    case WM_SYSKEYDOWN:
      if (game) {
        game->m_inputMapper.updateKeyState(wParam, lParam, true);
      }
      break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
      if (game) {
        game->m_inputMapper.updateKeyState(wParam, lParam, false);
      }
      break;

    case WM_MOUSEMOVE:
      if (game) {
        // TODO: Update mouse state
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
