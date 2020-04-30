#include "input_mapper.h"

void InputMapper::clear() {}

void InputMapper::updateKeyState(WPARAM wParam, LPARAM lParam, bool pressed) {
  const auto key = mapKey(wParam, lParam);
  if (key == InputId::Unknown)
    return;

  const auto modifier = buttonToModifier(key);
  if (modifier) {
    for (auto& binding : m_activeButtons) {
      if (!(binding.second & modifier) && pressed) {
        binding.second |= modifier;
      } else if (binding.second & modifier && !pressed) {
        binding.second &= ~modifier;
      }
    }
    return;
  }

  const auto modifiers = getModifierKeys();
  if (pressed) {
    m_activeButtons.emplace(key, modifiers);
  } else {
    m_activeButtons.erase(key);
  }
}

int InputMapper::getModifierKeys() const {
  int modifiers = 0;

  if (GetKeyState(VK_SHIFT) & (1 << 31))
    modifiers |= InputModifier::Shift;

  if (GetKeyState(VK_CONTROL) & (1 << 31))
    modifiers |= InputModifier::Ctrl;

  if (GetKeyState(VK_MENU) & (1 << 31))
    modifiers |= InputModifier::Alt;

  return modifiers;
}

InputId InputMapper::mapKey(WPARAM wParam, LPARAM lParam) const {
  switch (wParam) {
    // Character keys
    case 0x41:
      return InputId::A;
    case 0x42:
      return InputId::B;
    case 0x43:
      return InputId::C;
    case 0x44:
      return InputId::D;
    case 0x45:
      return InputId::E;
    case 0x46:
      return InputId::F;
    case 0x47:
      return InputId::G;
    case 0x48:
      return InputId::H;
    case 0x49:
      return InputId::I;
    case 0x4A:
      return InputId::J;
    case 0x4B:
      return InputId::K;
    case 0x4C:
      return InputId::L;
    case 0x4D:
      return InputId::M;
    case 0x4E:
      return InputId::N;
    case 0x4F:
      return InputId::O;
    case 0x50:
      return InputId::P;
    case 0x51:
      return InputId::Q;
    case 0x52:
      return InputId::R;
    case 0x53:
      return InputId::S;
    case 0x54:
      return InputId::T;
    case 0x55:
      return InputId::U;
    case 0x56:
      return InputId::V;
    case 0x57:
      return InputId::W;
    case 0x58:
      return InputId::X;
    case 0x59:
      return InputId::Y;
    case 0x5A:
      return InputId::Z;

    // Modifier keys
    case VK_SHIFT:
      return InputId::Shift;
    case VK_CONTROL:
      return InputId::Ctrl;
    case VK_MENU:
      return InputId::Alt;
  }

  return InputId::Unknown;
}

InputModifier InputMapper::buttonToModifier(InputId inputId) const {
  switch (inputId) {
    case InputId::Shift:
      return InputModifier::Shift;
    case InputId::Ctrl:
      return InputModifier::Ctrl;
    case InputId::Alt:
      return InputModifier::Alt;
    default:
      return static_cast<InputModifier>(0);
  }
}
