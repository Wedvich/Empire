#pragma once

class Renderer {
public:
  Renderer()
    : m_hwnd{nullptr},
      m_featureLevel{D3D_FEATURE_LEVEL_11_1},
      m_outputWidth{1024},
      m_outputHeight{768},
      m_tearingSupport{false},
      m_backBufferDesc{},
      m_viewport{},
      m_constantBufferData{},
      m_indexCount{0},
      m_frameCount{0} {}

  void init(HWND hwnd);
  void render();

private:
  HWND m_hwnd;
  float_t m_outputWidth;
  float_t m_outputHeight;
  bool m_tearingSupport;

  D3D_FEATURE_LEVEL m_featureLevel;
  D3D11_TEXTURE2D_DESC m_backBufferDesc;
  D3D11_VIEWPORT m_viewport;

  ComPtr<IDXGIFactory5> m_dxgiFactory;
  ComPtr<IDXGIAdapter1> m_dxgiAdapter;
  ComPtr<ID3D11Device2> m_device;
  ComPtr<ID3D11DeviceContext2> m_context;
  ComPtr<IDXGISwapChain1> m_swapChain;
  ComPtr<ID3D11RenderTargetView> m_renderTargetView;
  ComPtr<ID3D11Texture2D> m_depthStencil;
  ComPtr<ID3D11DepthStencilView> m_depthStencilView;

  typedef struct ConstantBufferData_ {
    DirectX::XMFLOAT4X4 world;
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 projection;
  } ConstantBufferData;

  static_assert((sizeof(ConstantBufferData) % 16) == 0,
                "Constant Buffer size must be 16-byte aligned");

  typedef struct VertexPositionColor_ {
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 color;
  } VertexPositionColor;

  typedef struct VertexPositionColorTangent_ {
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT3 tangent;
  } VertexPositionColorTangent;

  ConstantBufferData m_constantBufferData;
  unsigned int m_indexCount;
  unsigned int m_frameCount;

  ComPtr<ID3D11Buffer> m_vertexBuffer;
  ComPtr<ID3D11Buffer> m_indexBuffer;
  ComPtr<ID3D11VertexShader> m_vertexShader;
  ComPtr<ID3D11InputLayout> m_inputLayout;
  ComPtr<ID3D11InputLayout> m_inputLayoutExtended;
  ComPtr<ID3D11PixelShader> m_pixelShader;
  ComPtr<ID3D11Buffer> m_constantBuffer;
};
