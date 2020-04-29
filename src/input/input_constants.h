#pragma once

enum class InputId {
  Unknown = -1,
  A,
  B,
  C,
  D,
  E,
  F,
  G,
  H,
  I,
  J,
  K,
  L,
  M,
  N,
  O,
  P,
  Q,
  R,
  S,
  T,
  U,
  V,
  W,
  X,
  Y,
  Z,
  Shift,
  Ctrl,
  Alt
};

enum InputModifier { Shift = 1 << 0, Ctrl = 1 << 1, Alt = 1 << 2 };
