// globalsx.cpp : Implementation of CGlobalsX

#include "precomp.h"
#ifdef _ACTIVEX
#include "editx.h"
#include "langx.h"
#include "hotkeyx.h"
#include "globalsx.h"

/////////////////////////////////////////////////////////////////////////////
// CGlobalsX

STDMETHODIMP CGlobalsX::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IGlobals,
	};
	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (CMIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE CGlobalsX::put_FindMRUList( BSTR strMRUList )
{
	LPTSTR psz = BSTR2TSTR( strMRUList );
	CMSetFindReplaceMRUList( psz, TRUE );
	delete psz;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CGlobalsX::get_FindMRUList( BSTR __RPC_FAR *pstrMRUList )
{
	LPTSTR psz = new TCHAR[ CM_FIND_REPLACE_MRU_BUFF_SIZE + 1 ];
	CMGetFindReplaceMRUList( psz, TRUE );
	*pstrMRUList = T2BSTR( psz );
	delete psz;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CGlobalsX::put_ReplaceMRUList( BSTR strMRUList )
{
	LPTSTR psz = BSTR2TSTR( strMRUList );
	CMSetFindReplaceMRUList( psz, FALSE );
	delete psz;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CGlobalsX::get_ReplaceMRUList( BSTR __RPC_FAR *pstrMRUList )
{
	LPTSTR psz = new TCHAR[ CM_FIND_REPLACE_MRU_BUFF_SIZE + 1 ];
	CMGetFindReplaceMRUList( psz, FALSE );
	*pstrMRUList = T2BSTR( psz );
	delete psz;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CGlobalsX::RegisterLanguage( BSTR strName, ILanguage __RPC_FAR *pLang )
{
	HRESULT hr = S_FALSE;
	if ( pLang )
	{
		CM_LANGUAGE *pcmLang = pLang ? &( ( ( CLanguageX * ) pLang )->m_cmLang ) : NULL;
		LPTSTR psz = BSTR2TSTR( strName );
		hr = CME2HRESULT( CMRegisterLanguage( psz, pcmLang ) );
		delete psz;
	}
	
	return hr;
}

HRESULT STDMETHODCALLTYPE CGlobalsX::UnregisterLanguage( BSTR strName )
{
	LPTSTR psz = BSTR2TSTR( strName );
	HRESULT hr = CME2HRESULT( CMUnregisterLanguage( psz ) );
	delete psz;
	return hr;
}

HRESULT STDMETHODCALLTYPE CGlobalsX::UnregisterAllLanguages()
{
	return CME2HRESULT( CMUnregisterAllLanguages() );
}

HRESULT STDMETHODCALLTYPE CGlobalsX::GetHotKeys( SAFEARRAY __RPC_FAR * __RPC_FAR *ppBuff )
{
	SAFEARRAYBOUND rgsabound[1];
	rgsabound[0].lLbound = 0;
	rgsabound[0].cElements = CMGetHotKeys( NULL );
	HRESULT hr = E_FAIL;
	*ppBuff = SafeArrayCreate( VT_I1, 1, rgsabound );
	if( *ppBuff )
	{
		hr = SafeArrayAllocData( *ppBuff );
		if ( SUCCEEDED( hr ) )
		{
			LPBYTE pBytes;
			hr = SafeArrayAccessData(  *ppBuff, ( void ** ) &pBytes );
			if ( SUCCEEDED( hr ) )
			{
				CMGetHotKeys( pBytes );
				SafeArrayUnaccessData( *ppBuff );
			}
			else
			{
				SafeArrayDestroy( *ppBuff );
			}
		}
	}

	if( FAILED( hr ) )
	{
		*ppBuff = NULL;
	}
	return hr;
}

HRESULT STDMETHODCALLTYPE CGlobalsX::SetHotKeys( SAFEARRAY __RPC_FAR * __RPC_FAR *ppBuff )
{
	LPBYTE pBytes;
	HRESULT hr = SafeArrayAccessData( *ppBuff, ( void ** ) &pBytes );
	if ( SUCCEEDED( hr ) )
	{
		hr = CME2HRESULT( CMSetHotKeys( pBytes ) );
		SafeArrayUnaccessData( *ppBuff );
	}
	return hr;
}

HRESULT STDMETHODCALLTYPE CGlobalsX::ResetDefaultHotKeys( void )
{
	CMResetDefaultHotKeys();
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CGlobalsX::GetHotKeyForCmd( cmCommand lCmd, long lNum, IHotKey __RPC_FAR *__RPC_FAR *ppHotKey )
{
	HRESULT hr = S_FALSE;
	int nKeys = CMGetHotKeysForCmd( ( WORD ) lCmd, NULL );
	if ( nKeys && lNum >=0 && lNum < nKeys )
	{
		CM_HOTKEY *pHotKeys = new CM_HOTKEY[ nKeys ];
		CMGetHotKeysForCmd( ( WORD ) lCmd, pHotKeys );
		hr = CoCreateInstance( CLSID_HotKey, NULL, CLSCTX_INPROC_SERVER, IID_IHotKey, ( void ** ) ppHotKey );
		if ( SUCCEEDED( hr ) )
		{
			CM_HOTKEY *pcmHotKey = &( ( ( CHotKeyX * ) *ppHotKey )->m_cmHotKey );
			*pcmHotKey = pHotKeys[ lNum ];
			return S_OK;
		}
		delete [] pHotKeys;
		hr = S_OK;
	}
	return hr;
}

HRESULT STDMETHODCALLTYPE CGlobalsX::GetNumHotKeysForCmd( cmCommand lCmd, long __RPC_FAR *plLen )
{
	*plLen = CMGetHotKeysForCmd( ( WORD ) lCmd, NULL );
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CGlobalsX::RegisterHotKey( IHotKey __RPC_FAR *pHotKey, cmCommand lCmd )
{
	CM_HOTKEY *pcmHotKey = &( ( ( CHotKeyX * ) pHotKey )->m_cmHotKey );
	return CME2HRESULT( CMRegisterHotKey( pcmHotKey, ( WORD ) lCmd ) );
}

HRESULT STDMETHODCALLTYPE CGlobalsX::UnregisterHotKey( IHotKey __RPC_FAR *pHotKey )
{
	CME_CODE cmResult = CME_BADARGUMENT;
	if ( pHotKey )
	{
		CM_HOTKEY *pcmHotKey = &( ( ( CHotKeyX * ) pHotKey )->m_cmHotKey );
		cmResult = CME2HRESULT( CMUnregisterHotKey( pcmHotKey ) );
	}

	return cmResult;
}

HRESULT STDMETHODCALLTYPE CGlobalsX::GetCommandString( cmCommand lCmd, VARIANT_BOOL bDescription, BSTR __RPC_FAR *pstrVal )
{
	TCHAR sz[ max( CM_MAX_CMD_STRING, CM_MAX_CMD_DESCRIPTION ) + 1 ];
	CMGetCommandString( ( WORD ) lCmd, bDescription != 0, sz, ARRAY_SIZE( sz ) );
	*pstrVal = T2BSTR( sz );	
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CGlobalsX::GetMacro( long lMacro, SAFEARRAY __RPC_FAR * __RPC_FAR *ppBuff )
{
	SAFEARRAYBOUND rgsabound[1];
	rgsabound[0].lLbound = 0;
	rgsabound[0].cElements = CMGetMacro( lMacro, NULL );
	HRESULT hr = E_FAIL;
	*ppBuff = SafeArrayCreate( VT_I1, 1, rgsabound );
	if( *ppBuff )
	{
		hr = SafeArrayAllocData( *ppBuff );
		if ( SUCCEEDED( hr ) )
		{
			LPBYTE pBytes;
			hr = SafeArrayAccessData(  *ppBuff, ( void ** ) &pBytes );
			if ( SUCCEEDED( hr ) )
			{
				CMGetMacro( lMacro, pBytes );
				SafeArrayUnaccessData( *ppBuff );
			}
			else
			{
				SafeArrayDestroy( *ppBuff );
			}
		}
	}

	if( FAILED( hr ) )
	{
		*ppBuff = NULL;
	}
	return hr;
}

HRESULT STDMETHODCALLTYPE CGlobalsX::SetMacro( long lMacro, SAFEARRAY __RPC_FAR * __RPC_FAR *ppBuff )
{
	LPBYTE pBytes;
	LONG lLowerBound, lUpperBound;
	HRESULT hr;
	if ( SUCCEEDED( hr = SafeArrayGetLBound( *ppBuff, 1, &lLowerBound ) ) &&
		 SUCCEEDED( hr = SafeArrayGetUBound( *ppBuff, 1, &lUpperBound ) ) &&
		 lLowerBound >= lUpperBound )
	{
		CMSetMacro( lMacro, NULL );
	}
	else if ( SUCCEEDED( hr = SafeArrayAccessData( *ppBuff, ( void ** ) &pBytes ) ) )
	{
		CMSetMacro( lMacro, pBytes );
		SafeArrayUnaccessData( *ppBuff );
	}
	return hr;
}

HRESULT STDMETHODCALLTYPE CGlobalsX::RegisterCommand( long lCmdId, BSTR strName, BSTR strDescription )
{
	LPTSTR pszName = BSTR2TSTR( strName );
	LPTSTR pszDesc = BSTR2TSTR( strDescription );

	HRESULT hr = CME2HRESULT( CMRegisterCommand( ( WORD ) lCmdId, pszName, pszDesc ) );
	delete pszName;
	delete pszDesc;
	return hr;
}

HRESULT STDMETHODCALLTYPE CGlobalsX::UnregisterCommand( long lCmdId )
{
	return CME2HRESULT( CMUnregisterCommand( ( WORD ) lCmdId ) );
}

HRESULT STDMETHODCALLTYPE CGlobalsX::GetLanguageDef( BSTR strName, ILanguage __RPC_FAR *__RPC_FAR *ppLang )
{
	HRESULT hr = S_FALSE;
	if ( ppLang )
	{
		LPTSTR pszName = BSTR2TSTR( strName );
		int cbLang = CMGetLanguageDef( pszName, NULL );
		CM_LANGUAGE *pLang = NULL;
		if ( cbLang )
		{
			pLang = ( CM_LANGUAGE * ) new BYTE[ cbLang ];
			VERIFY( cbLang == CMGetLanguageDef( pszName, pLang ) );
		}
		delete pszName;

		if ( cbLang )
		{
			// release any existing object
			if ( *ppLang )
			{
				( *ppLang )->Release();
			}
			// return a new object
			if ( SUCCEEDED( CoCreateInstance( CLSID_Language, NULL, CLSCTX_INPROC_SERVER, IID_ILanguage, ( void ** ) ppLang ) ) )
			{
				( ( CLanguageX * ) *ppLang )->Copy( pLang );
				hr = S_OK;
			}
		}
		else
			hr = E_INVALIDARG;

		delete pLang;
	}
	return hr;
}

#endif