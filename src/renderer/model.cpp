#include "model.h"

void Model::init(ID3D11Device*                        device,
                 gsl::span<const VertexPositionColor> vertices,
                 gsl::span<const unsigned short>      indices,
                 ID3D11InputLayout*                   inputLayout,
                 ID3D11VertexShader*                  vertexShader,
                 ID3D11PixelShader*                   pixelShader) {
  CD3D11_BUFFER_DESC     vertexBufferDesc(gsl::narrow_cast<UINT>(vertices.size_bytes()), D3D11_BIND_VERTEX_BUFFER);
  D3D11_SUBRESOURCE_DATA vertexData{};
  vertexData.pSysMem = vertices.data();

  ThrowIfFailed(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer));

  m_indexCount = gsl::narrow_cast<UINT>(indices.size());

  CD3D11_BUFFER_DESC indexBufferDesc(gsl::narrow_cast<UINT>(indices.size_bytes()), D3D11_BIND_INDEX_BUFFER);

  D3D11_SUBRESOURCE_DATA indexData{};
  indexData.pSysMem = indices.data();

  ThrowIfFailed(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer));

  m_inputLayout  = inputLayout;
  m_vertexShader = vertexShader;
  m_pixelShader  = pixelShader;
}

void Model::render(ID3D11DeviceContext* context, ID3D11Buffer* const* constantBuffer) const {
  context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &m_stride, &m_offset);
  context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
  context->IASetInputLayout(m_inputLayout.Get());
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
  context->VSSetConstantBuffers(0, 1, constantBuffer);

  context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

  context->DrawIndexed(m_indexCount, 0, 0);
}
