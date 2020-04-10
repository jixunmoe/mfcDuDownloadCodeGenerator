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

	// int start;
	// int end;
	// GetSel(start, end);

	auto nLength = GetWindowTextLength();
	SetSel(nLength, nLength);
	ReplaceSel(text);

	// SetSel(start, end);
}


BEGIN_MESSAGE_MAP(CAdvEdit, CEdit)
END_MESSAGE_MAP()



// CAdvEdit message handlers


