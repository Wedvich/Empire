#include "renderer.h"

using namespace DirectX;

void Renderer::init(HWND hwnd) {
  m_hwnd = hwnd;

  UINT factoryFlags = 0;
#if defined(_DEBUG)
  factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

  ComPtr<IDXGIFactory4> dxgiFactory;
  ThrowIfFailed(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&dxgiFactory)));
  ThrowIfFailed(dxgiFactory.As(&m_dxgiFactory));

  {
    BOOL       allowTearing = FALSE;
    const auto hr           = m_dxgiFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING,
                                                       &allowTearing, sizeof(allowTearing));
    m_tearingSupport        = SUCCEEDED(hr) && allowTearing;
  }

  m_dxgiFactory->EnumAdapters1(0, m_dxgiAdapter.GetAddressOf());

  D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_11_1};

  UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
  deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  ComPtr<ID3D11Device>        device;
  ComPtr<ID3D11DeviceContext> context;

  ThrowIfFailed(D3D11CreateDevice(m_dxgiAdapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr,
                                  deviceFlags, levels, _countof(levels), D3D11_SDK_VERSION, &device,
                                  &m_featureLevel, &context));

  ComPtr<IDXGIDevice1> dxgiDevice;
  device.As(&dxgiDevice);
  dxgiDevice->SetMaximumFrameLatency(1);

  device.As(&m_device);
  context.As(&m_context);

  DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
  swapChainDesc.BufferCount        = 2;
  swapChainDesc.Format             = DXGI_FORMAT_B8G8R8A8_UNORM;
  swapChainDesc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapChainDesc.SampleDesc.Count   = 1;
  swapChainDesc.SampleDesc.Quality = 0;
  swapChainDesc.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
  swapChainDesc.Flags              = m_tearingSupport ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
  swapChainDesc.Width              = gsl::narrow_cast<UINT>(m_outputWidth);
  swapChainDesc.Height             = gsl::narrow_cast<UINT>(m_outputHeight);

  DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenSwapChainDesc{};
  fullscreenSwapChainDesc.Windowed = TRUE;

  ThrowIfFailed(m_dxgiFactory->CreateSwapChainForHwnd(
      m_device.Get(), m_hwnd, &swapChainDesc, &fullscreenSwapChainDesc, nullptr, &m_swapChain));

  m_dxgiFactory->MakeWindowAssociation(m_hwnd, DXGI_MWA_NO_ALT_ENTER);

  {
    ComPtr<ID3D11Texture2D> backBuffer;
    ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));

    backBuffer->GetDesc(&m_backBufferDesc);

    ThrowIfFailed(m_device->CreateRenderTargetView(backBuffer.Get(), nullptr,
                                                   m_renderTargetView.ReleaseAndGetAddressOf()));
  }

  CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, m_backBufferDesc.Width,
                                         m_backBufferDesc.Height, 1, 1, D3D11_BIND_DEPTH_STENCIL);

  ThrowIfFailed(m_device->CreateTexture2D(&depthStencilDesc, nullptr,
                                          m_depthStencil.ReleaseAndGetAddressOf()));

  CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);

  ThrowIfFailed(m_device->CreateDepthStencilView(m_depthStencil.Get(), &depthStencilViewDesc,
                                                 m_depthStencilView.ReleaseAndGetAddressOf()));

  ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
  m_viewport.Width    = static_cast<FLOAT>(m_backBufferDesc.Width);
  m_viewport.Height   = static_cast<FLOAT>(m_backBufferDesc.Height);
  m_viewport.MinDepth = 0;
  m_viewport.MaxDepth = 1;
  m_context->RSSetViewports(1, &m_viewport);

  CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ConstantBufferData), D3D11_BIND_CONSTANT_BUFFER);
  m_device->CreateBuffer(&constantBufferDesc, nullptr, m_constantBuffer.GetAddressOf());

  XMVECTOR eye = XMVectorSet(0.0f, 0.7f, 1.5f, 0.f);
  XMVECTOR at  = XMVectorSet(0.0f, -0.1f, 0.0f, 0.f);
  XMVECTOR up  = XMVectorSet(0.0f, 1.0f, 0.0f, 0.f);

  XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));

  const auto aspectRatio = static_cast<float>(m_outputWidth) / static_cast<float>(m_outputHeight);
  XMStoreFloat4x4(&m_constantBufferData.projection,
                  XMMatrixTranspose(XMMatrixPerspectiveFovRH(XMConvertToRadians(70), aspectRatio,
                                                             0.01f, 100.0f)));

  Cube cube{};

  CD3D11_BUFFER_DESC     vertexBufferDesc(sizeof(cube.vertices), D3D11_BIND_VERTEX_BUFFER);
  D3D11_SUBRESOURCE_DATA vertexData{};
  vertexData.pSysMem = cube.vertices;

  ThrowIfFailed(m_device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer));

  m_indexCount = _countof(cube.indices);

  CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cube.indices), D3D11_BIND_INDEX_BUFFER);

  D3D11_SUBRESOURCE_DATA indexData{};
  indexData.pSysMem = cube.indices;

  ThrowIfFailed(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer));

  FILE* vertexShader;
  BYTE* bytes;

  size_t destSize  = 65536;
  size_t bytesRead = 0;
  bytes            = new BYTE[destSize];

  fopen_s(&vertexShader, "VertexShader.cso", "rb");
  bytesRead = fread_s(bytes, destSize, 1, 65536, vertexShader);
  ThrowIfFailed(
      device->CreateVertexShader(bytes, bytesRead, nullptr, m_vertexShader.GetAddressOf()));

  D3D11_INPUT_ELEMENT_DESC inpultLayoutDesc[] = {
      {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
      {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
  };

  ThrowIfFailed(device->CreateInputLayout(inpultLayoutDesc, _countof(inpultLayoutDesc), bytes,
                                          bytesRead, &m_inputLayout));

  fclose(vertexShader);

  FILE* pixelShader;
  delete bytes;

  bytes     = new BYTE[destSize];
  bytesRead = 0;
  fopen_s(&pixelShader, "PixelShader.cso", "rb");
  bytesRead = fread_s(bytes, destSize, 1, 65536, pixelShader);
  ThrowIfFailed(device->CreatePixelShader(bytes, bytesRead, nullptr, m_pixelShader.GetAddressOf()));

  delete bytes;

  fclose(pixelShader);
}

void Renderer::render(TransformComponent* state) {
  XMStoreFloat4x4(&m_constantBufferData.world,
                  XMMatrixTranspose(XMMatrixRotationRollPitchYaw(
                      XMConvertToRadians(state->rotation.x), XMConvertToRadians(state->rotation.y),
                      XMConvertToRadians(state->rotation.z))));

  m_context->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &m_constantBufferData, 0, 0);

  const FLOAT clearColor[] = {0.25f, 0.5f, 0.75f, 1.0f};
  m_context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
  m_context->ClearDepthStencilView(m_depthStencilView.Get(),
                                   D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

  m_context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

  UINT stride = sizeof(VertexPositionColor);
  UINT offset = 0;

  m_context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
  m_context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
  m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  m_context->IASetInputLayout(m_inputLayout.Get());

  m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
  m_context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

  m_context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

  m_context->DrawIndexed(m_indexCount, 0, 0);

  m_swapChain->Present(0, m_tearingSupport ? DXGI_PRESENT_ALLOW_TEARING : 0);
}
