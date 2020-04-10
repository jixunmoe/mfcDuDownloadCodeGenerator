#pragma once
#include <debugapi.h>

std::vector<CString*> OpenFileDialog(CString &title, HWND hWnd);
std::vector<CString*> OpenDirectoryDialog(CString &title, HWND hWnd);

inline CStringA W2UTF8(const CString &input, DWORD &size)
{
	CStringA utf8;
	auto cc = WideCharToMultiByte(CP_UTF8, MB_ERR_INVALID_CHARS, input, -1, nullptr, 0, nullptr, nullptr) - 1;
	if (cc <= 0) return utf8;

	auto ptr = utf8.GetBuffer(cc);
	WideCharToMultiByte(CP_UTF8, MB_ERR_INVALID_CHARS, input, -1, ptr, cc, nullptr, nullptr);
	utf8.ReleaseBuffer();

	return utf8;
}

typedef void(*f_file_callback) (const CString &strDir, const CString &strName, void* extra);
inline void EnumFiles(const CString &srcDir, bool recursive, f_file_callback cb, void* extra = nullptr)
{
	auto dir(srcDir);
	if (dir.Right(1).Compare(_T("\\")) == 0)
	{
		dir.AppendChar(_T('*'));
	} else
	{
		dir.Append(_T("\\*"));
	}

	WIN32_FIND_DATA ffd;
	auto hFind = FindFirstFile(dir, &ffd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return;
	}

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (recursive)
			{
				if (_tcscmp(ffd.cFileName, _T(".")) != 0
					&& _tcscmp(ffd.cFileName, _T("..")) != 0)
				{
#if _DEBUG
					CString str;
					str.Format(_T("enter dir: %s\n"), ffd.cFileName);
					OutputDebugString(str);
#endif
					EnumFiles(srcDir + _T("\\") + ffd.cFileName, recursive, cb, extra);
				}
			}
		}
		else
		{
#if _DEBUG
			CString str;
			str.Format(_T("add file: %s\n"), ffd.cFileName);
			OutputDebugString(str);
#endif
			cb(srcDir, ffd.cFileName, extra);
		}
	} while (FindNextFile(hFind, &ffd) != 0);
	FindClose(hFind);
}

inline void CopyStringToClipboard(CString &str)
{
	auto strW = static_cast<const TCHAR*>(str);
	auto len = str.GetLength() * sizeof TCHAR + (sizeof TCHAR);
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
	memcpy(GlobalLock(hMem), strW, len);
	GlobalUnlock(hMem);
	OpenClipboard(nullptr);
	EmptyClipboard();
#ifndef _UNICODE
	SetClipboardData(CF_TEXT, hMem);
#else
	SetClipboardData(CF_UNICODETEXT, hMem);
#endif
	CloseClipboard();
}