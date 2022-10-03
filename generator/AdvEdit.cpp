// AdvEdit.cpp : implementation file
//

#include "stdafx.h"
#include "AdvEdit.h"

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

BEGIN_MESSAGE_MAP(CAdvEdit, CEdit)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

void CAdvEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);

	if (nChar == 'A') {
		BYTE keyState[256] = { 0 };
		GetKeyboardState(keyState);
		bool useControl = (keyState[VK_CONTROL] & 0x80) != 0;
		bool useShift = (keyState[VK_SHIFT] & 0x80) != 0;
		bool useAlt = (keyState[VK_MENU] & 0x80) != 0;

		if (useControl && !useShift && !useAlt) {
			this->SelectAll();
		}
	}
}
