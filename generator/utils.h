#pragma once

std::vector<CString*> OpenFileDialog(CString &title, HWND hWnd);
std::vector<CString*> OpenDirectoryDialog(CString &title, HWND hWnd);

typedef void(*f_file_callback) (const CString &strDir, const CString &strName, void* extra);
void EnumFiles(const CString& srcDir, bool recursive, f_file_callback cb, void* extra = nullptr);
