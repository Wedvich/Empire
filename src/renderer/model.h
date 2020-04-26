#pragma once

#include "transform_proxy.h"
#include "vertex_types.h"

class Model {
public:
  void init(ID3D11Device*                        device,
            gsl::span<const VertexPositionColor> vertices,
            gsl::span<const unsigned short>      indices,
            ID3D11InputLayout*                   inputLayout,
            ID3D11VertexShader*                  vertexShader,
            ID3D11PixelShader*                   pixelShader);

  void render(ID3D11DeviceContext* context, ID3D11Buffer* const* constantBuffer) const;

private:
  UINT m_indexCount = 0;
  UINT m_stride     = sizeof(VertexPositionColor);
  UINT m_offset     = 0;

  ComPtr<ID3D11InputLayout>  m_inputLayout;
  ComPtr<ID3D11Buffer>       m_indexBuffer;
  ComPtr<ID3D11Buffer>       m_vertexBuffer;
  ComPtr<ID3D11VertexShader> m_vertexShader;
  ComPtr<ID3D11PixelShader>  m_pixelShader;
};
