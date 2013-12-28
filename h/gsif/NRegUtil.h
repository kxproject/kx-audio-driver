// CNRegUtil.h : header file
//

#if !defined(AFX_CNRegUtil_H__INCLUDED_)
#define AFX_CNRegUtil_H__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AppReg.h"  // This defines a number of Registry Nodes and Keys


/////////////////////////////////////////////////////////////////////////////
// CNRegUtil

class CNRegUtil
{
// Construction
public:
	CNRegUtil(HKEY hRootKey = HKEY_CURRENT_USER)
	{
		m_hRootKey = hRootKey;
	};	// standard constructor

	BOOL WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue )
	{
		LONG lResult = 1; 
		HKEY hKey = NULL;
		DWORD dwDep;
		if (RegCreateKeyEx(m_hRootKey, lpszSection,	0, _T("REG_DWORD"), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDep) == ERROR_SUCCESS)
		{
			lResult = RegSetValueEx(hKey, lpszEntry, NULL, REG_DWORD, (LPBYTE)&nValue, sizeof(nValue));
			RegCloseKey(hKey);
		}

		return lResult == ERROR_SUCCESS;
	}

	BOOL WriteProfileString(LPCTSTR lpszSection, 
							LPCTSTR lpszEntry, LPCTSTR lpszValue )
	{
		LONG lResult = 1;
		HKEY hKey = NULL;
		DWORD dwDep;
		if(RegCreateKeyEx(m_hRootKey, lpszSection,	0, _T("REG_SZ"), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDep) == ERROR_SUCCESS)
		{
			// If NULL is passed in delete the RegKey
			if (lpszValue == NULL)
			{
				lResult = ::RegDeleteValue(hKey, (LPTSTR)lpszEntry);
			}
			else // Write the value
			{
				lResult = RegSetValueEx(hKey, lpszEntry, NULL, REG_SZ, (LPBYTE)lpszValue, (lstrlen(lpszValue)+1)*sizeof(TCHAR));
			}
			RegCloseKey(hKey);
		}
		
		return lResult == ERROR_SUCCESS;
	}

	CString GetProfileString(LPCTSTR lpszSection, 
							 LPCTSTR lpszEntry, LPCTSTR lpszDefault)
	{
		CString strValue;
		DWORD dwType, dwCount;
		LONG lResult = 1;
		
		HKEY hKey = NULL;
		if(RegOpenKeyEx(m_hRootKey, lpszSection,	0, KEY_QUERY_VALUE,	&hKey) == ERROR_SUCCESS)
		{
			dwCount = 0;
			lResult = RegQueryValueEx(hKey, (LPTSTR)lpszEntry, NULL, &dwType, NULL, &dwCount);
			if (dwCount)
			{
				lResult = RegQueryValueEx(hKey, (LPTSTR)lpszEntry, NULL, &dwType, (LPBYTE)strValue.GetBuffer(dwCount/sizeof(TCHAR)), &dwCount);
				strValue.ReleaseBuffer();

				RegCloseKey(hKey);
				// If we have something return it
				if (lResult == ERROR_SUCCESS)
				{
					ASSERT(dwType == REG_SZ);
					return strValue;
				}
			}
			RegCloseKey(hKey);
		}
	
		// OK Return default value
		return lpszDefault;
	};

	LONG GetProfileString(LPCTSTR lpszSection, 
						  LPCTSTR lpszEntry, LPCTSTR lpszDefault, LPTSTR szValue, DWORD* pdwSize)
	{
		DWORD dwType, dwCount;
		LONG lResult = 1;
		
		HKEY hKey = NULL;
		if(RegOpenKeyEx(m_hRootKey, lpszSection,	0, KEY_QUERY_VALUE,	&hKey) == ERROR_SUCCESS)
		{
			dwCount = 0;
			lResult = RegQueryValueEx(hKey, (LPTSTR)lpszEntry, NULL, &dwType, NULL, &dwCount);
			if (dwCount <= *pdwSize)
			{
				lResult = RegQueryValueEx(hKey, 
										  (LPTSTR)lpszEntry, 
										  NULL, 
										  &dwType,
										  (LPBYTE) szValue, pdwSize);
			}

			*pdwSize = dwCount;
			RegCloseKey(hKey);
		}

		if (lResult)
		{
			if (lpszDefault)
				_tcscpy(szValue, lpszDefault);
			else
			{
				szValue[0] = '\0';
			}
		}

		// OK Return default value
		return lResult;
	};

	unsigned long GetProfileInt(LPCTSTR lpszSection, 
								LPCTSTR lpszEntry, int nDefault )
	{
		DWORD dwValue;
		DWORD dwType;
		DWORD dwCount = sizeof(DWORD);
		LONG lResult = 0;
		
		HKEY hKey = NULL;
		if (RegOpenKeyEx(m_hRootKey, lpszSection, 0, KEY_QUERY_VALUE,	&hKey) == ERROR_SUCCESS)
		{
			lResult = RegQueryValueEx(hKey, (LPTSTR)lpszEntry, NULL, &dwType, (LPBYTE)&dwValue, &dwCount);
			
			if (lResult == ERROR_SUCCESS)
			{
				nDefault = (int) dwValue;
				ASSERT((dwType == REG_DWORD) || (dwType == REG_BINARY));
				ASSERT(dwCount == sizeof(dwValue));
			}
			RegCloseKey(hKey);
		}
		return (unsigned long) nDefault;
	};

	void GetProfileBinary(LPCTSTR lpszSection,
						  LPCTSTR szEntry, LPBYTE lpBuffer, DWORD dwSize)
	{
	HKEY hKey;
	DWORD cbData=dwSize;

	if (::RegOpenKeyEx(m_hRootKey, lpszSection, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
		{
		if (::RegQueryValueEx(hKey, szEntry, 0, NULL, lpBuffer, &cbData ) == ERROR_SUCCESS)
			{
				if ( cbData > dwSize)
				{
					ASSERT(1);
				}
			}
		::RegCloseKey(hKey);
		}
	}

	void WriteProfileBinary(LPCTSTR lpszSection,
						    LPCTSTR szEntry, LPBYTE lpBuffer, DWORD dwSize)
	{
	    HKEY hKey;
	    DWORD dwDep;

	    if (::RegCreateKeyEx(m_hRootKey, lpszSection,	0, _T("REG_BINARY"), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDep) == ERROR_SUCCESS)
	    {
		    ::RegSetValueEx(hKey, szEntry, 0, REG_BINARY, (const unsigned char *) lpBuffer, dwSize );
		    ::RegCloseKey(hKey);
	    }
    }

    LONG  EnumKey(LPCTSTR lpszSection, DWORD dwIndex,  LPTSTR szNextKey, DWORD * pdwSize)
	{
	    HKEY hKey;
        LONG  lret = -1;

    	if (::RegOpenKeyEx(m_hRootKey, lpszSection, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
		{
            FILETIME ft;
		    lret = ::RegEnumKeyEx(hKey, dwIndex, szNextKey, pdwSize, NULL, NULL, NULL, &ft);
		    ::RegCloseKey(hKey);
		}

        return lret;
	}

    LONG  EnumKey(HKEY hKey, DWORD dwIndex,  LPTSTR szNextKey, DWORD * pdwSize)
	{
        LONG  lret = -1;
        FILETIME ft;
	    lret = ::RegEnumKeyEx(hKey, dwIndex, szNextKey, pdwSize, NULL, NULL, NULL, &ft);

        return lret;
	}

	void DeleteValue(LPCTSTR lpszSection,LPCTSTR szEntry)
	{
	HKEY hKey;

	if (::RegOpenKeyEx(m_hRootKey, lpszSection, 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
		{
		::RegDeleteValue(hKey, szEntry);
		::RegCloseKey(hKey);
		}
	}

	void DeleteKey(LPCTSTR lpszSection, HKEY hRootKey = NULL)
	{
	HKEY hKey;

    if (hRootKey == NULL) hRootKey = m_hRootKey;

	if (::RegOpenKeyEx(hRootKey, lpszSection, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
		{
        DWORD dwSize = MAX_PATH;
        TCHAR szBuffer[MAX_PATH];

		// NOTE: Win 9x deletes recursively, Win NT does not, so you have to 
		// recursively delete subkeys ourselves.
        // Note: we keep passing index 0, into EnumKey, because we expect the
        // last subkey to always be index 0 as they are recursively removed.
        //
        while (EnumKey(hKey, 0, szBuffer, &dwSize) == ERROR_SUCCESS)
        {
            DeleteKey(szBuffer, hKey);
            dwSize = MAX_PATH;
        }
		::RegCloseKey(hKey);
		::RegDeleteKey(hRootKey, lpszSection);
		}
	}

private:	

	HKEY m_hRootKey;

};

/*
 * ============================================================================
 * 
 * Class: CIniUtil
 *
 * Description: This class provides all the routines need to read/write .ini
 *  files.
 *
 * =============================================================================
 */

class CIniUtil
{
// Construction
public:
	CIniUtil(LPTSTR IniFile, BOOL bCurrentDir = TRUE, BOOL bExe = FALSE)
	{
		m_bExe      = bExe;
		if (bCurrentDir)
		{
			m_IniFile = GetCurrentPath() + "\\";
		}
		m_IniFile  += IniFile;

	};	// standard constructor

	CString GetCurrentPath()
	{
		CString path;

		TCHAR * pBuffer = path.GetBuffer(MAX_PATH);

		AFX_MANAGE_STATE(AfxGetStaticModuleState( ));		
		ZeroMemory(pBuffer, sizeof(pBuffer) * sizeof(TCHAR));
		DWORD dwSize = MAX_PATH;

		HINSTANCE hModule = (m_bExe == FALSE) ? AfxGetInstanceHandle(): 0;
		if (GetModuleFileName(hModule, pBuffer, dwSize) == 0)
		{
			ASSERT(FALSE); // Invalid file handle
		}

		pBuffer[path.ReverseFind('\\')] = '\0';

		GetShortPathName(pBuffer, pBuffer, MAX_PATH);
		path.ReleaseBuffer();

		return path;
	}

	BOOL WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue )
	{
		CString value;
		value.Format("%d", nValue);
		::WritePrivateProfileString(lpszSection, lpszEntry, value, m_IniFile);
		return ERROR_SUCCESS;
	}

	BOOL WriteProfileString(LPCTSTR lpszSection, 
							LPCTSTR lpszEntry, LPCTSTR lpszValue )
	{

		return ERROR_SUCCESS;
	}

	CString GetProfileString(LPCTSTR lpszSection, 
							 LPCTSTR lpszEntry, LPCTSTR lpszDefault)
	{ 
		CString strValue;
		
		::GetPrivateProfileString(lpszSection, lpszEntry, lpszDefault,
								  strValue.GetBuffer(MAX_PATH/sizeof(TCHAR)), MAX_PATH*sizeof(TCHAR),
		 						  m_IniFile);
		strValue.ReleaseBuffer();

		return strValue;
	};

	unsigned long GetProfileInt(LPCTSTR lpszSection, 
								LPCTSTR lpszEntry, int nDefault )
	{
		return ::GetPrivateProfileInt(lpszSection, lpszEntry, nDefault,
									  m_IniFile); 
	};

	void GetProfileBinary(LPCTSTR lpszSection,
						  LPCTSTR szEntry, LPBYTE lpBuffer, DWORD dwSize)
	{
		::GetPrivateProfileStruct(lpszSection, szEntry, (LPVOID)lpBuffer,
								dwSize, m_IniFile);
	}

	void WriteProfileBinary(LPCTSTR lpszSection,
						    LPCTSTR szEntry, LPBYTE lpBuffer, DWORD dwSize)
	{
		::WritePrivateProfileStruct(lpszSection, szEntry, (LPVOID)lpBuffer,
								dwSize, m_IniFile);

	}

private:	

	CString m_IniFile;
	BOOL    m_bExe;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CNRegUtil_H__INCLUDED_)
