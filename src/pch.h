#pragma once

#include <sdkddkver.h>

#include <winsdkver.h>

#define NOMINMAX

#include <algorithm>
#include <gsl/gsl>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <wrl/client.h>

#ifdef _DEBUG
#include <crtdbg.h>
#endif

#include <d2d1_2.h>
#include <d3d11_2.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <dwrite.h>
#include <dxgi1_5.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

using Microsoft::WRL::ComPtr;

class com_exception : std::exception {
public:
  com_exception(HRESULT hr) : result_(hr) {}

  virtual const char* what() const override {
    static char s_str[64] = {};
    sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result_));
    return s_str;
  }

private:
  HRESULT result_;
};

inline void ThrowIfFailed(HRESULT hr) {
  if (FAILED(hr)) {
    throw com_exception(hr);
  }
}
