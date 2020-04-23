#include "game_window.h"
#include "constants.h"

constexpr auto c_windowClassName = L"Empire::GameWindow";
constexpr auto c_windowStyle     = WS_OVERLAPPED | WS_SYSMENU;
constexpr auto c_windowStyleEx   = WS_EX_APPWINDOW;
constexpr auto c_windowTitle     = L"Empire";

void GameWindow::init(WNDPROC windowProc) {
  const auto instance = GetModuleHandle(nullptr);
  assert(instance);

  WNDCLASSEX windowClass{};
  windowClass.cbSize        = sizeof(WNDCLASSEX);
  windowClass.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
  windowClass.lpszClassName = c_windowClassName;
  windowClass.lpfnWndProc   = windowProc;
  windowClass.hCursor       = LoadCursor(nullptr, IDC_ARROW);
  windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
  windowClass.hInstance     = instance;
  if (!RegisterClassEx(&windowClass))
    throw com_exception(HRESULT_FROM_WIN32(GetLastError()));

  RECT windowRect = {0, 0, c_defaultWidth, c_defaultHeight};
  if (!AdjustWindowRectEx(&windowRect, c_windowStyle, FALSE, c_windowStyleEx))
    throw com_exception(HRESULT_FROM_WIN32(GetLastError()));

  const auto windowWidth  = gsl::narrow_cast<int>(windowRect.right - windowRect.left);
  const auto windowHeight = gsl::narrow_cast<int>(windowRect.bottom - windowRect.top);

  const auto screenWidth  = GetSystemMetrics(SM_CXSCREEN);
  const auto screenHeight = GetSystemMetrics(SM_CYSCREEN);

  const auto windowLeft = std::max(screenWidth - windowWidth, 1) / 2;
  const auto windowTop  = std::max(screenHeight - windowHeight, 1) / 2;

  m_handle =
      CreateWindowEx(c_windowStyleEx, c_windowClassName, c_windowTitle, c_windowStyle, windowLeft,
                     windowTop, windowWidth, windowHeight, nullptr, nullptr, instance, nullptr);

  if (!m_handle)
    throw com_exception(HRESULT_FROM_WIN32(GetLastError()));
}

void GameWindow::show(int command) { ShowWindow(m_handle, command); }
