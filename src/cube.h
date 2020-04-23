#pragma once

typedef struct VertexPositionColor_ {
  DirectX::XMFLOAT3 position;
  DirectX::XMFLOAT3 color;
} VertexPositionColor;

struct Cube {
  const VertexPositionColor vertices[8] = {
      {
          DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f),
          DirectX::XMFLOAT3(0, 0, 0),
      },
      {
          DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f),
          DirectX::XMFLOAT3(0, 0, 1),
      },
      {
          DirectX::XMFLOAT3(-0.5f, 0.5f, -0.5f),
          DirectX::XMFLOAT3(0, 1, 0),
      },
      {
          DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f),
          DirectX::XMFLOAT3(0, 1, 1),
      },

      {
          DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f),
          DirectX::XMFLOAT3(1, 0, 0),
      },
      {
          DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f),
          DirectX::XMFLOAT3(1, 0, 1),
      },
      {
          DirectX::XMFLOAT3(0.5f, 0.5f, -0.5f),
          DirectX::XMFLOAT3(1, 1, 0),
      },
      {
          DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f),
          DirectX::XMFLOAT3(1, 1, 1),
      },
  };

  const unsigned short indices[36] = {
      0, 2, 1, // -x
      1, 2, 3,

      4, 5, 6, // +x
      5, 7, 6,

      0, 1, 5, // -y
      0, 5, 4,

      2, 6, 7, // +y
      2, 7, 3,

      0, 4, 6, // -z
      0, 6, 2,

      1, 3, 7, // +z
      1, 7, 5,
  };
};