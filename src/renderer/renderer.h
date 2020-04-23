#pragma once

#include "constants.h"
#include "cube.h"
#include "components/transform.h"
#include "camera.h"

class Renderer {
public:
  void init(HWND hwnd);
  void render(TransformComponent* state);
  Camera* getCamera() const { return m_camera.get(); }

private:
  HWND    m_hwnd           = nullptr;
  float_t m_outputWidth    = static_cast<float_t>(c_defaultWidth);
  float_t m_outputHeight   = static_cast<float_t>(c_defaultHeight);
  bool    m_tearingSupport = false;

  D3D_FEATURE_LEVEL    m_featureLevel   = D3D_FEATURE_LEVEL_11_1;
  D3D11_TEXTURE2D_DESC m_backBufferDesc = {};
  D3D11_VIEWPORT       m_viewport       = {};

  ComPtr<IDXGIFactory5>          m_dxgiFactory;
  ComPtr<IDXGIAdapter1>          m_dxgiAdapter;
  ComPtr<ID3D11Device2>          m_device;
  ComPtr<ID3D11DeviceContext2>   m_context;
  ComPtr<IDXGISwapChain1>        m_swapChain;
  ComPtr<ID3D11RenderTargetView> m_renderTargetView;
  ComPtr<ID3D11Texture2D>        m_depthStencil;
  ComPtr<ID3D11DepthStencilView> m_depthStencilView;

  typedef struct ConstantBufferData_ {
    DirectX::XMFLOAT4X4 world;
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 projection;
  } ConstantBufferData;

  static_assert((sizeof(ConstantBufferData) % 16) == 0,
                "Constant Buffer size must be 16-byte aligned");

  ConstantBufferData m_constantBufferData = {};
  unsigned int       m_indexCount         = 0;

  ComPtr<ID3D11Buffer>       m_vertexBuffer;
  ComPtr<ID3D11Buffer>       m_indexBuffer;
  ComPtr<ID3D11VertexShader> m_vertexShader;
  ComPtr<ID3D11InputLayout>  m_inputLayout;
  ComPtr<ID3D11InputLayout>  m_inputLayoutExtended;
  ComPtr<ID3D11PixelShader>  m_pixelShader;
  ComPtr<ID3D11Buffer>       m_constantBuffer;

  std::unique_ptr<Camera> m_camera;

  void clear();
  void updateViewMatrix(TransformComponent* transform);
};
