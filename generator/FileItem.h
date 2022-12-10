#pragma once
class CFileItem
{
public:
	CString m_pDirectory = CString("");
	CString m_pBaseDirectory = CString("");
	CString m_pFilename = CString("");
	HICON m_hIcon;
	uint64_t m_nSize = 0;

	CString m_pFirstHash = CString("");
	CString m_pFullHash = CString("");

public:
	CFileItem();
	~CFileItem();

	bool Done() const;
	CString GetSizeString() const;

	CString BDLink() const;
	CString DownloadCode() const;
};

