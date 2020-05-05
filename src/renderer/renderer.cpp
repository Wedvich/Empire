#include "renderer.h"
#include "data\temp_model_data.h"

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

  CD3D11_RASTERIZER_DESC1 rasterizerDesc(D3D11_DEFAULT);
  rasterizerDesc.CullMode = D3D11_CULL_NONE;
  ThrowIfFailed(m_device->CreateRasterizerState1(&rasterizerDesc, m_rasterizerState.ReleaseAndGetAddressOf()));
  m_context->RSSetState(m_rasterizerState.Get()); 

  ZeroMemory(&m_viewport, sizeof(D3D11_VIEWPORT));
  m_viewport.Width    = static_cast<FLOAT>(m_backBufferDesc.Width);
  m_viewport.Height   = static_cast<FLOAT>(m_backBufferDesc.Height);
  m_viewport.MinDepth = 0;
  m_viewport.MaxDepth = 1;
  m_context->RSSetViewports(1, &m_viewport);

  CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ConstantBufferData), D3D11_BIND_CONSTANT_BUFFER);
  m_device->CreateBuffer(&constantBufferDesc, nullptr, m_constantBuffer.GetAddressOf());

  m_camera = std::make_unique<Camera>();

  const auto aspectRatio = static_cast<float>(m_outputWidth) / static_cast<float>(m_outputHeight);
  XMStoreFloat4x4(&m_constantBufferData.projection,
                  XMMatrixPerspectiveFovLH(XMConvertToRadians(70), aspectRatio, 0.01f, 100.0f));

  initResources();
  initUi();
}

void Renderer::render(TransformProxy* state) {
  clear();

  const auto scalingVector      = XMLoadFloat3(&state->scale);
  const auto rotationQuaternion = XMLoadFloat4(&state->rotate);
  const auto translationVector  = XMLoadFloat3(&state->translate);

  const auto worldMatrix = XMMatrixScalingFromVector(scalingVector) *
                           XMMatrixRotationQuaternion(rotationQuaternion) *
                           XMMatrixTranslationFromVector(translationVector);

  XMStoreFloat4x4(&m_constantBufferData.world, worldMatrix);
  updateViewMatrix();
  m_model.render(m_context.Get(), m_constantBuffer.GetAddressOf());

  XMStoreFloat4x4(&m_constantBufferData.world, XMMatrixIdentity());
  updateViewMatrix();
  m_worldModel.render(m_context.Get(), m_constantBuffer.GetAddressOf());

  renderUi();

  m_swapChain->Present(0, m_tearingSupport ? DXGI_PRESENT_ALLOW_TEARING : 0);
}

void Renderer::initResources() {
  Cube cube{};

  FILE* vertexShader;
  BYTE* bytes;

  size_t destSize  = 65536;
  size_t bytesRead = 0;
  bytes            = new BYTE[destSize];

  fopen_s(&vertexShader, "VertexShader.cso", "rb");
  bytesRead = fread_s(bytes, destSize, 1, 65536, vertexShader);
  ThrowIfFailed(
      m_device->CreateVertexShader(bytes, bytesRead, nullptr, m_vertexShader.GetAddressOf()));

  D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] = {
      {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
      {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
  };

  ThrowIfFailed(m_device->CreateInputLayout(inputLayoutDesc, _countof(inputLayoutDesc), bytes,
                                          bytesRead, &m_inputLayout));

  fclose(vertexShader);

  FILE* pixelShader;
  delete bytes;

  bytes     = new BYTE[destSize];
  bytesRead = 0;
  fopen_s(&pixelShader, "PixelShader.cso", "rb");
  bytesRead = fread_s(bytes, destSize, 1, 65536, pixelShader);
  ThrowIfFailed(m_device->CreatePixelShader(bytes, bytesRead, nullptr, m_pixelShader.GetAddressOf()));

  delete bytes;

  fclose(pixelShader);

  Room room{};

  m_model.init(m_device.Get(), cube.vertices, cube.indices, m_inputLayout.Get(),
               m_vertexShader.Get(), m_pixelShader.Get());
  m_worldModel.init(m_device.Get(), room.vertices, room.indices, m_inputLayout.Get(),
                    m_vertexShader.Get(), m_pixelShader.Get());
}

void Renderer::initUi() {
  // Direct2D
  ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, m_d2dFactory.GetAddressOf()));

  ComPtr<ID3D11Texture2D> backBuffer;
  ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
  ComPtr<IDXGISurface> backBufferSurface;
  ThrowIfFailed(backBuffer.As(&backBufferSurface));

  const auto dpi = GetDpiForWindow(m_hwnd);

  D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
      D2D1_RENDER_TARGET_TYPE_DEFAULT,
      D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), dpi, dpi);

  ThrowIfFailed(m_d2dFactory->CreateDxgiSurfaceRenderTarget(
      backBufferSurface.Get(), &props, m_d2dRenderTarget.ReleaseAndGetAddressOf()));

  ThrowIfFailed(m_d2dRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black),
                                                         m_textBrush.GetAddressOf()));

  // DirectWrite
  ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                    reinterpret_cast<IUnknown**>(m_dWriteFactory.GetAddressOf())));

  ThrowIfFailed(m_dWriteFactory->CreateTextFormat(
      L"Consolas", nullptr, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL,
      DWRITE_FONT_STRETCH_NORMAL, 16.0f, L"en-us", m_textFormat.GetAddressOf()));
}

void Renderer::clear() {
  const FLOAT clearColor[] = {0.25f, 0.5f, 0.75f, 1.0f};
  m_context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
  m_context->ClearDepthStencilView(m_depthStencilView.Get(),
                                   D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

  m_context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
}

void Renderer::updateViewMatrix() {
  const XMVECTOR eye = XMLoadFloat3(&m_camera->m_eye);
  const XMVECTOR at  = XMLoadFloat3(&m_camera->m_at);
  const XMVECTOR up  = XMLoadFloat3(&m_camera->m_up);

  XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixLookAtLH(eye, at, up));

  m_context->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &m_constantBufferData, 0, 0);
}

void Renderer::renderUi() {
  const D2D1_RECT_F layout{8.0f, 8.0f, m_outputWidth - 16.0f, m_outputHeight - 16.0f};

  std::wostringstream w;
  w << L"Camera position (press R to reset)" << std::endl << std::endl;
  w << L" x: " << std::to_wstring(m_camera->m_eye.x) << std::endl;
  w << L" y: " << std::to_wstring(m_camera->m_eye.y) << std::endl;
  w << L" z: " << std::to_wstring(m_camera->m_eye.z) << std::endl;
  const auto ws = w.str();

  m_d2dRenderTarget->BeginDraw();
  m_d2dRenderTarget->SetTransform(D2D1::IdentityMatrix());
  m_d2dRenderTarget->DrawText(ws.c_str(), ws.size(), m_textFormat.Get(), &layout,
                              m_textBrush.Get());
  m_d2dRenderTarget->EndDraw();
}
