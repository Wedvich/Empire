#pragma once

struct Cube {
  const VertexPositionColor vertices[8] = {
      {
          DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f),
          DirectX::XMFLOAT4(0, 0, 0, 1),
      },
      {
          DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f),
          DirectX::XMFLOAT4(0, 0, 1, 1),
      },
      {
          DirectX::XMFLOAT3(-0.5f, 0.5f, -0.5f),
          DirectX::XMFLOAT4(0, 1, 0, 1),
      },
      {
          DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f),
          DirectX::XMFLOAT4(0, 1, 1, 1),
      },

      {
          DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f),
          DirectX::XMFLOAT4(1, 0, 0, 1),
      },
      {
          DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f),
          DirectX::XMFLOAT4(1, 0, 1, 1),
      },
      {
          DirectX::XMFLOAT3(0.5f, 0.5f, -0.5f),
          DirectX::XMFLOAT4(1, 1, 0, 1),
      },
      {
          DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f),
          DirectX::XMFLOAT4(1, 1, 1, 1),
      },
  };

  const unsigned short indices[36] = {
      1, 2, 0, // -x
      3, 2, 1,

      6, 5, 4, // +x
      6, 7, 5,

      5, 1, 0, // -y
      4, 5, 0,

      7, 6, 2, // +y
      3, 7, 2,

      6, 4, 0, // -z
      2, 6, 0,

      7, 3, 1, // +z
      5, 7, 1,
  };
};
