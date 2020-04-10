#pragma once

// CProgressText

class CProgressText : public CProgressCtrl
{
	CFont* font;
	COLORREF Colour;
	COLORREF BkColour;
	COLORREF BdColour;
	CBrush* brushForeground;
	CBrush* brushBackground;
	CBrush* brushBorder;
	void Init();

	DECLARE_DYNAMIC(CProgressText)
	uint current = 0;
	uint max = 0;
	CString* text;
	double percent = 0;

public:
	CProgressText();
	virtual ~CProgressText();
	void SetCurrent(uint current);
	void SetMax(uint max);
	void Increase();


protected:
	DECLARE_MESSAGE_MAP()
	void UpdateText();

public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


