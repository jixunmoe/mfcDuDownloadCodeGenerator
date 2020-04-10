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
