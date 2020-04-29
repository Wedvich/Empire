#pragma once

#include "input_constants.h"

class InputMapper {
public:
  void clear();
  void updateKeyState(WPARAM wParam, LPARAM lParam, bool pressed);

private:
  int      getModifierKeys() const;
  InputId mapKey(WPARAM wParam, LPARAM lParam);
  InputModifier buttonToModifier(InputId buttonInputId) const;

private:
  friend class Game;
  std::map<InputId, int> m_activeButtons;
};
