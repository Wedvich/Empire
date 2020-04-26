#pragma once

#include "renderer/vertex_types.h"

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

  //  const unsigned short indices[36] = {
  //    0, 2, 1, // -x
  //    1, 2, 3,

  //    4, 5, 6, // +x
  //    5, 7, 6,

  //    0, 1, 5, // -y
  //    0, 5, 4,

  //    2, 6, 7, // +y
  //    2, 7, 3,

  //    0, 4, 6, // -z
  //    0, 6, 2,

  //    1, 3, 7, // +z
  //    1, 7, 5,
  //};
};
