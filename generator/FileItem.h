#pragma once
class CFileItem
{
public:
	CString* m_pDirectory = nullptr;
	CString* m_pFilename = nullptr;
	HICON m_hIcon = nullptr;
	uint64_t m_nSize = 0;

	CString* m_pFirstHash = nullptr;
	CString* m_pFullHash = nullptr;

public:
	CFileItem();
	~CFileItem();

	bool Done() const;
	CString GetSizeString() const;

	CString DownloadCode() const;
};

