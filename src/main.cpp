#include "renderer.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  }

  return DefWindowProc(hwnd, message, wParam, lParam);
}

_Use_decl_annotations_
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR, int showCommand) {
  WNDCLASSEX wcex{};
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_DBLCLKS;
  wcex.lpszClassName = L"Empire";
  wcex.lpfnWndProc = WndProc;
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  RegisterClassEx(&wcex);

  constexpr auto windowStyle = WS_OVERLAPPED | WS_SYSMENU;

  RECT windowRect = {0, 0, 1024, 768};
  AdjustWindowRectEx(&windowRect, windowStyle, FALSE, WS_EX_APPWINDOW);

  const auto windowWidth = gsl::narrow_cast<int>(windowRect.right - windowRect.left);
  const auto windowHeight = gsl::narrow_cast<int>(windowRect.bottom - windowRect.top);

  const auto screenWidth = GetSystemMetrics(SM_CXSCREEN);
  const auto screenHeight = GetSystemMetrics(SM_CYSCREEN);

  const auto windowLeft = std::max(screenWidth - windowWidth, 1) / 2;
  const auto windowTop = std::max(screenHeight - windowHeight, 1) / 2;

  auto hwnd =
      CreateWindowEx(WS_EX_APPWINDOW, wcex.lpszClassName, L"Empire", windowStyle, windowLeft,
                     windowTop, windowWidth, windowHeight, nullptr, nullptr, instance, nullptr);

  ShowWindow(hwnd, showCommand);

  Renderer renderer{};
  renderer.init(hwnd);

  MSG msg{};
  while (msg.message != WM_QUIT) {
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
      renderer.render();
    }
  }

  return 0;
}
