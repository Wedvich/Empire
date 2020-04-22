#pragma once

#define NOMINMAX
#include <windows.h>
#include <algorithm>
#include <stdexcept>
#include <wrl/client.h>
#include <gsl/gsl>

#ifdef _DEBUG
#include <crtdbg.h>
#endif

#include <dxgi1_5.h>
#include <d3d11_2.h>
#include <directxmath.h>
#include <d3dcompiler.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

using Microsoft::WRL::ComPtr;

class com_exception : std::exception {
  public:
    com_exception(HRESULT hr) : result_(hr) {}

    virtual const char *what() const override {
        static char s_str[64] = {};
        sprintf_s(s_str, "Failure with HRESULT of %08X",
                  static_cast<unsigned int>(result_));
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
