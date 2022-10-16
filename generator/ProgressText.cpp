// ProgressText.cpp : implementation file
//

#include "stdafx.h"

#include "generator.h"
#include "ProgressText.h"
#include "DPISupport.h"

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

	progressForegroundBrush = new CBrush(progressForegroundColour);
	progressBackgroundBrush = new CBrush(progressBackgroundColour);
	componentBorderBrush = new CBrush(componentBorderColour);

	font = new CFont();
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

void CProgressText::Reset()
{
	this->current = 0;
	this->max = 0;
	this->SetPos(0);
	this->UpdateText();
	this->RedrawWindow();
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

	double dpi_scale = double(DPISupport::GetWindowDPI(GetSafeHwnd())) / 96.0;

	CRect rect;
	GetClientRect(&rect);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	memDC.SetMapMode(dc.GetMapMode());
	memDC.SetViewportOrg(dc.GetViewportOrg());
	memDC.IntersectClipRect(rect);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	auto pOldBmp = memDC.SelectObject(&bmp);

	memDC.SetBkMode(TRANSPARENT);

	// 绘制一圈边框
	memDC.SelectObject(GetStockObject(DC_PEN));
	memDC.SetDCPenColor(this->componentBorderColour);
	memDC.Rectangle(rect);

	// 绘制四方形
	{
		auto rectProgress = rect;
		shrink(&rectProgress);
		memDC.FillRect(rectProgress, this->progressBackgroundBrush);

		rectProgress.right = rectProgress.left + LONG(rectProgress.Width() * this->percent);
		memDC.FillRect(rectProgress, this->progressForegroundBrush);
	}

	auto font_height = static_cast<uint32_t>((rect.Height() - 4) * dpi_scale * 0.75);
	font->DeleteObject();
	VERIFY(font->CreateFont(
		font_height,               // nHeight
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

	auto def_font = memDC.SelectObject(font);
	auto size = memDC.GetTextExtent(*text);
	auto x = rect.right - size.cx - 8;
	auto y = (rect.Height() - size.cy) / 2;
	memDC.TextOut(x, y, *text, this->text->GetLength());
	memDC.SelectObject(def_font);

	// 拷贝图片
	dc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memDC, rect.left, rect.top, SRCCOPY);

	// 还原选择的对象
	memDC.SelectObject(pOldBmp);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
// ReSharper disable once CppMemberFunctionMayBeConst
BOOL CProgressText::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}
