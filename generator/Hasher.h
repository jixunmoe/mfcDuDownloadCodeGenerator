#pragma once
#include <Wincrypt.h>

class Hasher
{
	HCRYPTPROV m_hCryptProv = NULL;
	HCRYPTHASH m_phHash = NULL;
	HCRYPTKEY m_hKey = NULL;
	ALG_ID m_alg = NULL;

public:
	bool Init();
	Hasher(ALG_ID alg);
	~Hasher();

	void Cleanup();

	CString GetHashStr();
	void Feed(char* str, int i) const;
};

