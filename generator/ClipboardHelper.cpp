#include "stdafx.h"
#include "ClipboardHelper.h"

void CopyStringToClipboard(CString& str, HWND hWnd) {
	auto pString = static_cast<const TCHAR*>(str);
	auto dwStrByteLen = (str.GetLength() + 1) * sizeof TCHAR;

	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, dwStrByteLen);
	if (!hMem) return;

	auto pLockedMem = GlobalLock(hMem);
	if (!pLockedMem) {
		GlobalFree(hMem);
		return;
	}
	memcpy(pLockedMem, pString, dwStrByteLen);
	GlobalUnlock(hMem);

	if (OpenClipboard(hWnd)) {
		EmptyClipboard();
#ifndef _UNICODE
		SetClipboardData(CF_TEXT, hMem);
#else
		SetClipboardData(CF_UNICODETEXT, hMem);
#endif
		CloseClipboard();
	}
}
