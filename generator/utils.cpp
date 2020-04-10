#include "stdafx.h"
#include "utils.h"

/*
int CALLBACK cb_set_initial(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	
}
*/

std::vector<CString*> OldStyleFileDialog(const CString& title, HWND hwnd)
{
	std::vector<CString*> v;
	CString str;
	OPENFILENAME ofn = {0};
	ofn.lStructSize = sizeof ofn;
	ofn.lpstrTitle = title;
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = str.GetBuffer(1024);
	ofn.nMaxFile = 1024;
	if (GetOpenFileName(&ofn))
	{
		v.push_back(new CString(str));
	}
	str.ReleaseBuffer();

	return v;
}

inline std::vector<CString*> OldStyleDirectoryDialog(CString &title, HWND hWnd)
{
	std::vector<CString*> v;
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof bi);
	bi.hwndOwner = hWnd;
	// bi.lpfn = cb_set_initial;
	bi.lpszTitle = title;
	bi.ulFlags = BIF_EDITBOX | BIF_NEWDIALOGSTYLE;

	TCHAR szPath[1024];
	if (SHGetPathFromIDList(SHBrowseForFolder(&bi), szPath))
	{
		v.push_back(new CString(szPath));
	}
	return v;
}

DWORD OpenFileDialog(CString &title, HWND hWnd, DWORD flag, std::vector<CString*> &v)
{
	IFileOpenDialog* pFileOpen;
	do
	{
		if (FAILED(CoCreateInstance(CLSID_FileOpenDialog,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&pFileOpen))))
		{
			return 1;
		}

		FILEOPENDIALOGOPTIONS fos;
		if (FAILED(pFileOpen->GetOptions(&fos))) break;
		fos |= flag | FOS_ALLOWMULTISELECT | FOS_FORCEFILESYSTEM;
		if (FAILED(pFileOpen->SetOptions(fos))) break;
		if (FAILED(pFileOpen->SetTitle(title))) break;
		if (FAILED(pFileOpen->Show(hWnd))) break;

		IShellItem* pShellItem;
		pFileOpen->GetResult(&pShellItem);

		IShellItemArray* pResults;
		if (FAILED(pFileOpen->GetResults(&pResults))) break;
		DWORD dwCount;
		if (FAILED(pResults->GetCount(&dwCount))) break;


		v.clear();
		for (uint i = 0; i < dwCount; i++)
		{
			IShellItem* pItem;
			pResults->GetItemAt(i, &pItem);

			TCHAR* filePath;
			pItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &filePath);

			v.push_back(new CString(filePath));

			CoTaskMemFree(filePath);
			pItem->Release();
		}

		if (pFileOpen) pFileOpen->Release();
		return 0;
	} while (false);

	if (pFileOpen) pFileOpen->Release();
	return 0;
}

std::vector<CString*> OpenDirectoryDialog(CString &title, HWND hWnd)
{
	std::vector<CString*> r;
	auto err = OpenFileDialog(title, hWnd, FOS_PICKFOLDERS, r);
	if (err == 1) return OldStyleDirectoryDialog(title, hWnd);
	return r;
}

std::vector<CString*> OpenFileDialog(CString &title, HWND hWnd)
{
	std::vector<CString*> r;
	auto err = OpenFileDialog(title, hWnd, 0, r);
	if (err == 1) return OldStyleFileDialog(title, hWnd);
	return r;
}
