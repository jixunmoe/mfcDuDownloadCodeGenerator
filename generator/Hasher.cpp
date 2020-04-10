#include "stdafx.h"
#include "Hasher.h"


bool Hasher::Init()
{
	auto bSuccess = false;

	if (CryptAcquireContext(&m_hCryptProv, nullptr, nullptr, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
	{
		bSuccess = CryptCreateHash(m_hCryptProv, this->m_alg, NULL, NULL, &m_phHash);

		if (bSuccess) return bSuccess;
	}
	Cleanup();

	if (CryptAcquireContext(&m_hCryptProv, nullptr, nullptr, PROV_RSA_FULL, NULL))
	{
		bSuccess = CryptCreateHash(m_hCryptProv, this->m_alg, NULL, NULL, &m_phHash);

		if (bSuccess) return bSuccess;
	}
	Cleanup();

	if (CryptAcquireContext(&m_hCryptProv, nullptr, nullptr, PROV_RSA_FULL, CRYPT_NEWKEYSET))
	{
		bSuccess = CryptCreateHash(m_hCryptProv, this->m_alg, NULL, NULL, &m_phHash);

		if (bSuccess) return bSuccess;
	}
	Cleanup();

	return bSuccess;
}

Hasher::Hasher(ALG_ID alg)
{
	this->m_alg = alg;
}


Hasher::~Hasher()
{
}

void Hasher::Cleanup()
{
	if (m_hKey)
	{
		CryptDestroyKey(m_hKey);
		m_hKey = NULL;
	}

	if (m_phHash)
	{
		CryptDestroyHash(m_phHash);
		m_phHash = NULL;
	}

	if (m_hCryptProv)
	{
		CryptReleaseContext(m_hCryptProv, NULL);
		m_hCryptProv = NULL;
	}
}


CString* Hasher::GetHashStr()
{
	DWORD dwDataLen;
	auto r = new CString("");
	if (CryptGetHashParam(this->m_phHash, HP_HASHVAL, nullptr, &dwDataLen, 0))
	{
		BYTE* d = new BYTE[dwDataLen];
		CryptGetHashParam(this->m_phHash, HP_HASHVAL, d, &dwDataLen, 0);
		for(DWORD i = 0; i < dwDataLen; i++)
		{
			r->AppendFormat(_T("%X%X"), d[i] >> 4, d[i] & 0x0F);
		}
		delete[] d;
	}
	Cleanup();

	return r;
}

void Hasher::Feed(char* str, int n) const
{
	CryptHashData(m_phHash, LPBYTE(str), n, NULL);
}
