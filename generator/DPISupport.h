#pragma once

#include <Windows.h>
#include <cstdint>

namespace DPISupport {
	uint32_t GetWindowDPI(HWND hWnd);
}
