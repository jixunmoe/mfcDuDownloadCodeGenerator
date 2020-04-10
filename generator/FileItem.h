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
	CString GetSizeString() const
	{
		TCHAR sizes[][6] = {
			_T("B"),
			_T("KB"),
			_T("MB"),
			_T("GB"),
			_T("TB"),
			_T("PB")
		};

		auto power = log(m_nSize) / log(1024);
		auto unit = int(floor(power));
		auto r = m_nSize / pow(1024, unit);

		CString str;
		str.Format(_T("%.2f %s"), r, sizes[unit]);
		return str;
	}

	CString DownloadCode() const;
};

