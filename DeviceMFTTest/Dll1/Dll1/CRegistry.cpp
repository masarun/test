#include "pch.h"
#include "CRegistry.h"

CRegistry::CRegistry(HKEY hKey) : m_hRootKey(hKey), m_fOpen(false), m_hKey(NULL)
{
}

CRegistry::~CRegistry()
{
	if (m_hKey != NULL)
	{
		RegCloseKey(m_hKey);
	}
}

bool CRegistry::CreateOrOpenSubKey(LPCTSTR szSubKey)
{
	bool result = false;
	LSTATUS status;

	if ((m_hRootKey == NULL) || (szSubKey == NULL) || (lstrlen(szSubKey) == 0))
	{
		return result;
	}

	// Check if the subkey already exists.
	status = RegOpenKeyEx(m_hRootKey, szSubKey, 0, KEY_READ | KEY_WRITE, &m_hKey);
	if (status == ERROR_SUCCESS)
	{
		// Subkey exists, and is opened.

		m_fOpen = true;
		result = true;
	}
	else
	{
		if (status == ERROR_FILE_NOT_FOUND)
		{
			// Subkey doesn't exist.
			// Create the subkey.

			status = RegCreateKeyEx(m_hRootKey, szSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &m_hKey, NULL);
			if (status == ERROR_SUCCESS)
			{
				m_fOpen = true;
				result = true;
			}
		}
	}

	return result;
}

bool CRegistry::SetStringValue(LPCTSTR szValueName, LPCTSTR szValue)
{
	bool result = false;
	LSTATUS status;

	if ((szValue == NULL) || (lstrlen(szValue) == 0))
	{
		return result;
	}

	if ((m_hRootKey == NULL) || (!m_fOpen))
	{
		return result;
	}

	DWORD cchValue = lstrlen(szValue) + 1;
	status = RegSetValueEx(m_hKey, szValueName, 0, REG_SZ, (const BYTE*)szValue, sizeof(TCHAR) * cchValue);
	if (status == ERROR_SUCCESS)
	{
		result = true;
	}

	return result;
}

bool CRegistry::DeleteSubKey(LPCTSTR szSubKey)
{
	bool result = false;

	if ((m_hRootKey == NULL) || (szSubKey == NULL) || (lstrlen(szSubKey) == 0))
	{
		return result;
	}

	if (RegDeleteTree(m_hRootKey, szSubKey) == ERROR_SUCCESS)
	{
		result = true;
	}

	return result;
}
