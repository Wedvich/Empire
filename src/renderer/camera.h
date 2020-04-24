#pragma once

using namespace DirectX;

class Camera {
public:
  XMFLOAT3 m_eye = {0.0f, 0.5f, -3.5f};
  XMFLOAT3 m_at  = {0.0f, 0.1f, 0.0f};
  XMFLOAT3 m_up  = {0.0f, 1.0f, 0.0f};
  XMFLOAT4 m_rotation = {0.0f, 0.0f, 0.0f, 0.0f};
};
