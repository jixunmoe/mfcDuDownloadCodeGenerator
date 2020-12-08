// ListBoxEx.cpp : implementation file
//

#include "stdafx.h"
#include "ListBoxEx.h"
#include "resource.h"
#include <fstream>      // std::ifstream
#include "Hasher.h"
#include <debugapi.h>
#include <winuser.h>
#include <minwinbase.h>
#include "utils.h"

#define MIN_FILE_SIZE 256 * 1024

// CListBoxEx

IMPLEMENT_DYNAMIC(CListBoxEx, CListBox)
HICON CListBoxEx::m_hIconTick = nullptr;
CString CListBoxEx::m_sPlaceholder;
HFONT CListBoxEx::m_systemFont;
CListBoxEx::CListBoxEx()
{
	// init guard
	if (m_hIconTick == nullptr)
	{
		m_hIconTick = static_cast<HICON>(LoadImage(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON_TICK), IMAGE_ICON, 16, 16, 0));
		int _ = m_sPlaceholder.LoadStringW(IDS_LIST_PLACEHOLDER);

		NONCLIENTMETRICS metrics = {};
		metrics.cbSize = sizeof(metrics);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &metrics, 0);
		m_systemFont = CreateFontIndirect(&metrics.lfCaptionFont);
		CString placeholder;
	}
}

CListBoxEx::~CListBoxEx()
{
}


BEGIN_MESSAGE_MAP(CListBoxEx, CListBox)
	ON_WM_VKEYTOITEM_REFLECT()
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CListBoxEx message handlers
void CListBoxEx::GetVisibleRange(int& s, int& e)
{
	auto maxIndex = this->GetCount() - 1;
	e = s = GetTopIndex();
	RECT rect;
	this->GetClientRect(&rect);
	auto height = rect.bottom - rect.top;
	auto currentHeight = 0;

	while(e < maxIndex)
	{
		currentHeight += GetItemHeight(e);
		if (currentHeight >= height) break;
		e++;
	}
}

bool CListBoxEx::ItemIsVisible(int nIndex)
{
	int s, e;
	GetVisibleRange(s, e);
	return s <= nIndex && nIndex <= e;
}

bool CListBoxEx::RedrawIfVisible(int i)
{
	RECT rectItem;
	if (this->GetItemRect(i, &rectItem) != LB_ERR)
	{
		if (rectItem.bottom < 0) return false;

		RECT rectCtrl;
		this->GetClientRect(&rectCtrl);

		if (rectItem.top <= rectCtrl.bottom - rectCtrl.top)
		{
			this->InvalidateRect(&rectItem, FALSE);
			return true;
		}
	}

	return false;
}


// 4MB Buffer 
#define BUF_SIZE 1024*1024*4
void CListBoxEx::ProcessFiles(f_proc_file_callback cb, void* extra)
{
	std::lock_guard<std::mutex> guard(mutex);
	m_bStop = false;

	auto c = GetCount();
	char* buffer = new char[BUF_SIZE];
	Hasher hash(CALG_MD5);
	for(auto i = 0; i < c; i++)
	{
		auto data = reinterpret_cast<LPFileItemStruct>(GetItemData(i));
		if (data->Done())
		{
			cb(ProcType::INC_FILE, 0, data, extra);
			continue;
		}

		auto path(*data->m_pDirectory + _T("\\") + *data->m_pFilename);
		std::ifstream is(path, std::ifstream::binary);

		if (!is)
		{
			cb(ProcType::ERR_FILE, 0, nullptr, extra);
			is.close();
			break;
		}

		is.read(buffer, MIN_FILE_SIZE);
		hash.Init();
		hash.Feed(buffer, MIN_FILE_SIZE);
		data->m_pFirstHash = hash.GetHashStr();

		is.seekg(0, SEEK_SET);
		hash.Init();
		double step = double(BUF_SIZE) / data->m_nSize;
		double prog = 0;
		
		do
		{
			if (m_bStop)
			{
				is.close();
				delete[] buffer;
				return;
			}

			is.read(buffer, BUF_SIZE);

			auto eof = is.eof();
			if (!eof && is.fail())
			{
#ifdef _DEBUG
				CString str;
				str.Format(_T("state: %x (goodbit: 0, eof: 1, fail: 2, badbit: 4)\n"), is.rdstate());
				OutputDebugString(str);
#endif
				cb(ProcType::ERR_FILE, 0, nullptr, extra);
				break;
			}

			hash.Feed(buffer, eof ? int(is.gcount()) : BUF_SIZE);

			if (eof)
			{
				data->m_pFullHash = hash.GetHashStr();

				if (this->RedrawIfVisible(i))
				{
#ifdef _DEBUG
					OutputDebugString(_T("Redraw item.\n"));
#endif

				}
				cb(ProcType::INC_FILE, 0, data, extra);
				break;
			}
			prog += step;
			cb(ProcType::FILE_PROG, prog, data, extra);
		} while (true);
		is.close();
	}

	delete[] buffer;
}

void CListBoxEx::StopProcessing()
{
	m_bStop = true;
	{
		std::lock_guard<std::mutex> guard(mutex); 
	}
}

void CListBoxEx::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = 48 + 4*2 + 8;
}

void CListBoxEx::DrawItemData(LPDRAWITEMSTRUCT lpDrawItemStruct, FileItemStruct* pItem)
{
	auto pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	pDC->SetBkMode(TRANSPARENT);

	CRect rectItem(lpDrawItemStruct->rcItem);
	rectItem.DeflateRect(4, 4, 4, 4);
	CRect rectIcon(rectItem.left, rectItem.top, rectItem.left + 48, rectItem.top + 48);
	CRect rectIconTick(rectIcon.left, rectIcon.top + 48 - 16, rectIcon.left + 16, rectItem.top + 48);
	CRect rectText(rectIcon.right, rectItem.top, rectItem.right - 8, rectItem.bottom);

	auto action = lpDrawItemStruct->itemAction;
	auto state = lpDrawItemStruct->itemState;

	bool selected = (state & ODS_SELECTED) && (action & (ODA_SELECT | ODA_DRAWENTIRE));
	bool redraw = (action & ODA_DRAWENTIRE) || (!(state & ODS_SELECTED) && (action & ODA_SELECT));

	auto textColour = selected ? m_textSelected : m_text;
	auto descTextColour = selected ? m_descTextSelected : m_descText;

	CBrush brushBackground(selected ? m_bgSelected : m_bgClear);
	CBrush brushText(textColour);
	CBrush brushDescText(descTextColour);


	if (selected || redraw)
	{
		CString str;
		pDC->FillRect(&lpDrawItemStruct->rcItem, &brushBackground);
		pDC->DrawIcon(rectIcon.left, rectIcon.top, pItem->m_hIcon);

		if (pItem->m_pFullHash)
		{
			DrawIconEx(*pDC, rectIconTick.left, rectIconTick.top, m_hIconTick, 16, 16, NULL, nullptr, DI_NORMAL);
		}


		auto rect = rectText;
		pDC->SetTextColor(textColour);
		rect.OffsetRect(2, 0);

		str.SetString(*pItem->m_pFilename);
		pDC->DrawText(str, str.GetLength(), rect, DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);
		rect.OffsetRect(0, 2 + pDC->GetTextExtent(str).cy);


		str.Format(IDS_DIR, *pItem->m_pDirectory);
		pDC->SetTextColor(descTextColour);
		pDC->DrawText(str, str.GetLength(), rect, DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);
		rect.OffsetRect(0, 2 + pDC->GetTextExtent(str).cy);

		str.Format(IDS_FILE_SIZE, pItem->GetSizeString());
		pDC->DrawText(str, str.GetLength(), rect, DT_LEFT | DT_SINGLELINE);
		rect.OffsetRect(0, 2 + pDC->GetTextExtent(str).cy);
	}
}

void CListBoxEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	auto pData = GetItemDataPtr(lpDrawItemStruct->itemID);
	if (pData != (void*)-1) {
		OutputDebugStringA("Draw item");
		this->DrawItemData(lpDrawItemStruct, reinterpret_cast<FileItemStruct*>(pData));
	}

}

uint64_t FileSize(const wchar_t* name)
{
	struct __stat64 buf;
	if (_wstat64(name, &buf) != 0)
		return 0; // error, could use errno to find out more

	return buf.st_size;
}

int CListBoxEx::AddItem(const CString& srcDir, const CString& filename)
{
 	std::lock_guard<std::mutex> guard(this->mutex);

	auto fullPath(srcDir + _T("\\") + filename);
	auto fSize = FileSize(fullPath);
	if (fSize < MIN_FILE_SIZE)
	{
		return -1;
	}

	auto index = this->AddString(_T(""));
	auto data = new FileItemStruct();
	data->m_pDirectory = new CString(srcDir);
	data->m_pFilename = new CString(filename);

	SHFILEINFO shfi = {};
	SHGetFileInfo(fullPath, FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(SHFILEINFO),
		SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_SHELLICONSIZE);

	data->m_hIcon = shfi.hIcon;
	data->m_nSize = fSize;

	SetItemData(index, DWORD_PTR(data));

	return 0;
}

void CListBoxEx::CopySelectedHashes()
{
	CString str;

	for (int i = 0; i < GetCount(); i++)
	{
		if (GetSel(i))
		{
			str.AppendFormat(L"%s\r\n", reinterpret_cast<CFileItem*>(GetItemData(i))->DownloadCode().GetString());
		}
	}

	CopyStringToClipboard(str);
}

int CListBoxEx::VKeyToItem(UINT nKey, UINT nIndex)
{
	// Returns –2 for no further action,
	// –1 for default action, or
	// a nonnegative number to specify an index of a
	// list box item on which to perform the default action for the keystroke.

	if(mutex.try_lock())
	{
		// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
		switch (nKey)
		{
		case 'E':
			for (int i = 0; i < GetCount(); i++)
				SetSel(i, false);

			mutex.unlock();
			return -2;

		case 'C':
			this->CopySelectedHashes();
			mutex.unlock();
			return -2;

		case 'D':
		case VK_DELETE:
			for (int i = GetCount() - 1; i >= 0; i--)
			{
				if (GetSel(i))
				{
					DeleteString(i);
					SetSel(i, true);
				}
			}

			mutex.unlock();
			return -2;

		}
		mutex.unlock();
		return -1;
	}

	return -1;
}


void CListBoxEx::PreSubclassWindow()
{
	CListBox::PreSubclassWindow();
	EnableToolTips(TRUE);
}

INT_PTR CListBoxEx::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	RECT itemRect;
	BOOL isOutside = FALSE;
	int row = ItemFromPoint(point, isOutside);
	if (row == -1 || isOutside)
		return -1;

	GetItemRect(row, &itemRect);

	pTI->rect = itemRect;
	pTI->hwnd = m_hWnd;
	pTI->uId = row;
	pTI->lpszText = LPSTR_TEXTCALLBACK;
	return pTI->uId;
}

BOOL CListBoxEx::OnToolTipText(UINT id, NMHDR * pNMHDR, LRESULT * pResult)
{
	auto pToolTip = AfxGetModuleThreadState()->m_pToolTip;
	if (pToolTip) pToolTip->SetMaxTipWidth(SHRT_MAX);

	auto ptText = reinterpret_cast<LPTOOLTIPTEXTW>(pNMHDR);

	auto pItem = reinterpret_cast<CFileItem*>(this->GetItemData(static_cast<int>(pNMHDR->idFrom)));

	CString str;
	str.Format(IDS_FILE_INFO,
		*pItem->m_pFilename,
		*pItem->m_pDirectory,
		 pItem->GetSizeString());


	auto strW = static_cast<const TCHAR*>(str);
	auto memSize = str.GetLength() * sizeof TCHAR + (2 * sizeof TCHAR);
	auto lpszText = (TCHAR*)calloc(memSize, 1);
	if (!lpszText) {
		return FALSE;
	}

	memcpy(lpszText, strW, memSize);

	ptText->lpszText = lpszText;

	*pResult = 0;

	return TRUE;

}

void CListBoxEx::OnPaint()
{
	if (GetCount() == 0) {
		CPaintDC dc(this);

		CRect rect;
		this->GetClientRect(rect);
		dc.SetTextColor(m_descText);
		dc.SelectObject(m_systemFont);
		dc.DrawText(m_sPlaceholder, m_sPlaceholder.GetLength(), rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
	}
	else {
		Default();
	}
}
