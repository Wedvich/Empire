#include "game.h"

extern "C" {
__declspec(dllexport) DWORD NvOptimusEnablement                = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

_Use_decl_annotations_ int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int) {
  ThrowIfFailed(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

  { Game{}.run(); }

  CoUninitialize();
  return 0;
}
