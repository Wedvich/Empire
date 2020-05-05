#pragma once

struct Room {
  const VertexPositionColor vertices[12] = {
      // x+
      {
          DirectX::XMFLOAT3(-3.0f, -3.0f, -3.0f),
          DirectX::XMFLOAT4(1, 0, 0, 1),
      },
      {
          DirectX::XMFLOAT3(-3.0f, 3.0f, -3.0f),
          DirectX::XMFLOAT4(1, 0, 0, 1),
      },
      {
          DirectX::XMFLOAT3(-3.0f, -3.0f, 3.0f),
          DirectX::XMFLOAT4(1, 0, 0, 1),
      },
      {
          DirectX::XMFLOAT3(-3.0f, 3.0f, 3.0f),
          DirectX::XMFLOAT4(1, 0, 0, 1),
      },

      // y-
      {
          DirectX::XMFLOAT3(-3.0f, -3.0f, -3.0f),
          DirectX::XMFLOAT4(1, 1, 1, 1),
      },
      {
          DirectX::XMFLOAT3(-3.0f, -3.0f, 3.0f),
          DirectX::XMFLOAT4(1, 1, 1, 1),
      },
      {
          DirectX::XMFLOAT3(3.0f, -3.0f, -3.0f),
          DirectX::XMFLOAT4(1, 1, 1, 1),
      },
      {
          DirectX::XMFLOAT3(3.0f, -3.0f, 3.0f),
          DirectX::XMFLOAT4(1, 1, 1, 1),
      },

      // x+
      {
          DirectX::XMFLOAT3(3.0f, -3.0f, -3.0f),
          DirectX::XMFLOAT4(0, 1, 0, 1),
      },
      {
          DirectX::XMFLOAT3(3.0f, -3.0f, 3.0f),
          DirectX::XMFLOAT4(0, 1, 0, 1),
      },
      {
          DirectX::XMFLOAT3(3.0f, 3.0f, -3.0f),
          DirectX::XMFLOAT4(0, 1, 0, 1),
      },
      {
          DirectX::XMFLOAT3(3.0f, 3.0f, 3.0f),
          DirectX::XMFLOAT4(0, 1, 0, 1),
      },
  };

  const unsigned short indices[18] = {
      0, 1, 2, // x-
      1, 3, 2,

      4, 5, 6, // y-
      5, 7, 6,

      8, 9, 10, // x+
      9, 11, 10,
  };
};
