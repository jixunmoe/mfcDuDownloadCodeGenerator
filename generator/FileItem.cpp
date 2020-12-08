#include "stdafx.h"
#include "FileItem.h"


CFileItem::CFileItem()
{
}


CFileItem::~CFileItem()
{
	SAFE_DELETE(m_pDirectory);
	SAFE_DELETE(m_pFilename);
	SAFE_DELETE(m_pFirstHash);
	SAFE_DELETE(m_pFullHash);

	if (m_hIcon)
	{
		DestroyIcon(m_hIcon);
		m_hIcon = nullptr;
	}
}

bool CFileItem::Done() const
{
	return m_pFullHash != nullptr;
}

CString CFileItem::GetSizeString() const
{
	TCHAR sizes[][6] = {
		_T("B"),
		_T("KB"),
		_T("MB"),
		_T("GB"),
		_T("TB"),
		_T("PB")
	};

	auto power = log(double(m_nSize)) / log(1024);
	auto unit = int(floor(power));
	auto r = m_nSize / pow(1024, unit);

	CString str;
	str.Format(_T("%.2f %s"), r, sizes[unit]);
	return str;
}

CString CFileItem::DownloadCode() const
{
	CString str;
	if (Done())
	{
		str.Format(_T("%s#%s#%llu#%s"),
			m_pFullHash->GetString(),
			m_pFirstHash->GetString(),
			m_nSize,
			m_pFilename->GetString()
		);
	} else
	{
		str.Format(_T("%s#%s#%llu#%s\r\n"),
			_T("<md5(file)>"),
			_T("<md5(256kb)>"),
			m_nSize,
			m_pFilename->GetString()
		);
	}
	return str;
}
