#pragma once
#include <mutex>


// CAdvEdit

class CAdvEdit : public CEdit
{
	DECLARE_DYNAMIC(CAdvEdit)
	std::mutex mutex;

public:
	CAdvEdit();
	virtual ~CAdvEdit();
	void Append(const CString &text);
	void SelectAll();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


