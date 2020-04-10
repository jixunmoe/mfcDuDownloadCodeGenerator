// ProgressText.cpp : implementation file
//

#include "stdafx.h"
#include "generator.h"
#include "ProgressText.h"

void inline shrink(CRect* rect) {
	rect->left++;
	rect->top++;
	rect->right--;
	rect->bottom--;
}

// CProgressText

IMPLEMENT_DYNAMIC(CProgressText, CProgressCtrl)

CProgressText::CProgressText()
{
	Init();
}

CProgressText::~CProgressText()
{
	SAFE_DELETE(text);
}

void CProgressText::Init()
{
	text = new CString("");

	Colour = RGB(66, 244, 161);
	BkColour = RGB(0xE6, 0xE6, 0xE6);
	BdColour = RGB(0xBC, 0xBC, 0xBC);

	brushForeground = new CBrush(Colour);
	brushBackground = new CBrush(BkColour);
	brushBorder = new CBrush(BdColour);

	font = new CFont();
	VERIFY(CProgressText::font->CreateFont(
		16,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Consolas")));          // lpszFacename
}

void CProgressText::SetCurrent(uint current)
{
	this->current = current;
	this->SetPos(this->current);
	this->UpdateText();
}

void CProgressText::SetMax(uint max)
{
	this->max = max;
	this->SetRange32(0, max);
	this->UpdateText();
}

void CProgressText::Increase()
{
	this->current++;
	this->SetPos(this->current);
	this->UpdateText();
}

void CProgressText::UpdateText() {
	if (this->max == 0) {
		this->percent = 0;
		this->text->SetString(_T(""));
	}
	else
	{
		this->percent = double(this->current) / double(this->max);
		this->text->Format(_T("%d / %d (%.2f%%)"), this->current, this->max, this->percent * 100);
	}
}


BEGIN_MESSAGE_MAP(CProgressText, CProgressCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CProgressText message handlers




void CProgressText::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // Do not call CProgressCtrl::OnPaint() for painting messages

	CRect rcClipBox;
	dc.GetClipBox(rcClipBox);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	memDC.SetMapMode(dc.GetMapMode());
	memDC.SetViewportOrg(dc.GetViewportOrg());
	memDC.IntersectClipRect(rcClipBox);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, rcClipBox.Width(), rcClipBox.Height());
	auto pOldBmp = memDC.SelectObject(&bmp);



	CRect rect;
	GetClientRect(&rect);
	auto rectDraw = rect;

	memDC.SetBkMode(TRANSPARENT);

	// 绘制一圈边框
	memDC.SelectObject(GetStockObject(DC_PEN));
	memDC.SetDCPenColor(this->BdColour);
	memDC.Rectangle(rectDraw);

	// 绘制四方形
	shrink(&rect);
	memDC.FillRect(rect, this->brushBackground);

	rect.right = rect.left + LONG((rect.right - rect.left) * this->percent);
	memDC.FillRect(rect, this->brushForeground);


	auto def_font = memDC.SelectObject(font);
	auto size = memDC.GetTextExtent(*text);
	auto x = rectDraw.right - size.cx - 5;
	auto y = (rectDraw.bottom - rectDraw.top - size.cy) / 2;
	memDC.TextOut(x, y, *text, this->text->GetLength());
	memDC.SelectObject(def_font);

	// 拷贝图片
	dc.BitBlt(rcClipBox.left, rcClipBox.top, rcClipBox.Width(), rcClipBox.Height(), &memDC, rcClipBox.left, rcClipBox.top, SRCCOPY);

	// 还原选择的对象
	memDC.SelectObject(pOldBmp);

	// dc.TextOut(0, 0, L"test", 4);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
// ReSharper disable once CppMemberFunctionMayBeConst
BOOL CProgressText::OnEraseBkgnd(CDC* pDC)
{
	// return CProgressCtrl::OnEraseBkgnd(pDC);
	return TRUE;
}
