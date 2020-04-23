#pragma once

using namespace DirectX;

class Camera {
public:
  XMFLOAT3 m_eye = {0.0f, 0.7f, 1.5f};
  XMFLOAT3 m_at  = {0.0f, 0.1f, 0.0f};
  XMFLOAT3 m_up  = {0.0f, 1.0f, 0.0f};
};
