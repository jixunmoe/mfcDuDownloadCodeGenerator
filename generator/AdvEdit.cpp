// AdvEdit.cpp : implementation file
//

#include "stdafx.h"
#include "AdvEdit.h"

#define SELECT_ALL_KEY_ID (10001)

// CAdvEdit

IMPLEMENT_DYNAMIC(CAdvEdit, CEdit)

CAdvEdit::CAdvEdit()
{
}

CAdvEdit::~CAdvEdit()
{
}

void CAdvEdit::Append(const CString& text)
{
	std::lock_guard<std::mutex> guard(mutex);

	auto nLength = GetWindowTextLength();
	SetSel(nLength, nLength);
	ReplaceSel(text);
}

void CAdvEdit::SelectAll()
{
	SetSel(0, GetWindowTextLength(), FALSE);
}

void CAdvEdit::InitEvents()
{
	RegisterHotKey(GetSafeHwnd(), SELECT_ALL_KEY_ID, MOD_CONTROL, 'A');
}

BEGIN_MESSAGE_MAP(CAdvEdit, CEdit)
	ON_WM_HOTKEY()
END_MESSAGE_MAP()



// CAdvEdit message handlers



void CAdvEdit::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	CEdit::OnHotKey(nHotKeyId, nKey1, nKey2);

	if (nHotKeyId == SELECT_ALL_KEY_ID) {
		this->SelectAll();
	}
}
