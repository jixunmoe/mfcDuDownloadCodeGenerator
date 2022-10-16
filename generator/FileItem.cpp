#include "stdafx.h"
#include "FileItem.h"
#include "encoding.h"
#include "base64.h"
#include "resource.h"

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
	CString str;
	if (m_nSize == 0) {
		str = TEXT("0");
		return str;
	}

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

	str.Format(_T("%.2f %s"), r, sizes[unit]);
	return str;
}

CString CFileItem::BDLink() const
{
	CString result;
	CString dlCode = this->DownloadCode();
	utf8_str str = {};
	ToUTF8(str, dlCode.GetBuffer());
	if (str.str == nullptr) {
		BOOL _ = result.LoadStringW(IDS_ERROR_ENCODE_FAIL);
		return result;
	}

	// 使用 strlen 计算字符串大小，防止 base64 变化后的内容含有 NUL 字节
	wchar_t* encoded = base64_encode(reinterpret_cast<unsigned char*>(str.str), strlen(str.str));
	free(str.str);

	if (encoded == nullptr) {
		BOOL _ = result.LoadStringW(IDS_ERROR_ENCODE_FAIL);
		return result;
	}

	// 拼接地址
	result.Append(_T("https://pan.baidu.com/#bdlink="));
	result.Append(encoded);

	free(encoded);
	return result;
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
