#pragma once

using namespace DirectX;

typedef struct TransformProxy {
  XMFLOAT3 scale{};
  XMFLOAT4 rotate{};
  XMFLOAT3 translate{};
} TransformProxy;
