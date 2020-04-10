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

protected:
	DECLARE_MESSAGE_MAP()
};


