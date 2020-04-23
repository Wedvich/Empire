#pragma once

class GameWindow {
public:
  void init(WNDPROC windowProc);
  void show(int command);

  const HWND getHandle() const noexcept { return m_handle; }

private:
  HWND m_handle = nullptr;
};
