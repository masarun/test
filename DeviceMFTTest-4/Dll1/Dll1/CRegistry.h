#pragma once

#include <windows.h>

class CRegistry
{
public:
	CRegistry(HKEY hKey);
	~CRegistry();

	bool CreateOrOpenSubKey(LPCTSTR szSubKey);
	bool SetStringValue(LPCTSTR szValueName, LPCTSTR szValue);
	bool DeleteSubKey(LPCTSTR szSubKey);

private:
	HKEY m_hRootKey;
	HKEY m_hKey;
	bool m_fOpen;
};