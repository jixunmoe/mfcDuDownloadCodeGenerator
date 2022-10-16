#include "DPISupport.h"
#include "stdafx.h"

#if !__BUILD_FOR_XP
#include <ShellScalingApi.h>
#include <VersionHelpers.h>

#pragma comment(lib, "Shcore")
#endif

uint32_t DPISupport::GetWindowDPI(HWND hWnd) {
  UINT xdpi = 0;
  UINT ydpi = 0;

#if !__BUILD_FOR_XP
  // Windows 8.1 or higher
  if (IsWindows8Point1OrGreater()) {
    HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    LRESULT success =
        GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &xdpi, &ydpi);
    return success == S_OK ? ydpi : 96;
  }
#endif

  HDC hDC = GetDC(hWnd);
  ydpi = static_cast<UINT>(GetDeviceCaps(hDC, LOGPIXELSY));
  ReleaseDC(hWnd, hDC);
  return ydpi;
}
