#pragma once

using namespace DirectX;

typedef struct TransformProxy_ {
  XMFLOAT3 scale{};
  XMFLOAT4 rotate{};
  XMFLOAT3 translate{};
} TransformProxy;
