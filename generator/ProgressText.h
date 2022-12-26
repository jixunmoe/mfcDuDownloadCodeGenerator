#pragma once

// CProgressText

class CProgressText : public CProgressCtrl
{
	CFont* font;
	COLORREF progressForegroundColour = RGB(62, 231, 152);
	COLORREF progressBackgroundColour = RGB(0xE6, 0xE6, 0xE6);
	COLORREF componentBorderColour = RGB(0xBC, 0xBC, 0xBC);

	CBrush* progressForegroundBrush;
	CBrush* progressBackgroundBrush;
	CBrush* componentBorderBrush;
	void Init();

	DECLARE_DYNAMIC(CProgressText)
	uint current = 0;
	uint max = 0;
	CString text = CString("");
	double percent = 0;

public:
	CProgressText();
	virtual ~CProgressText();
	void SetCurrent(uint current);
	void SetMax(uint max);
	void Increase();
	void Reset();

protected:
	DECLARE_MESSAGE_MAP()
	void UpdateText();

public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


