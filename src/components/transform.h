#pragma once

#include "component.h"

using namespace DirectX;

class TransformComponent : public Component {
public:
  XMFLOAT3 position = {};
  XMFLOAT3 rotation = {};
  XMFLOAT3 scale = {};
};
