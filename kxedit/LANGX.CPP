// Langx.cpp : Implementation of CLanguageX

#include "precomp.h"
#ifdef _ACTIVEX
#include "misc.h"
#include "editx.h"
#include "langx.h"

/////////////////////////////////////////////////////////////////////////////
// CLanguageX

CLanguageX::CLanguageX()
{
	ZeroMemory( &m_cmLang, sizeof( m_cmLang ) );
}

CLanguageX::~CLanguageX()
{
	DeleteMembers();
}

STDMETHODIMP CLanguageX::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ILanguage,
	};
	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (CMIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

static void StringDupe( LPCTSTR &pszDest, LPCTSTR &pszSrc )
{
	pszDest = NULL;
	if ( pszSrc )
	{
		pszDest = new TCHAR[ _tcslen( pszSrc ) + 1 ];
		_tcscpy( ( LPTSTR ) pszDest, pszSrc );
	}
}

void CLanguageX::Copy( CM_LANGUAGE *pLang )
{
	DeleteMembers();
	m_cmLang = *pLang;
	StringDupe( m_cmLang.pszKeywords, pLang->pszKeywords );
	StringDupe( m_cmLang.pszOperators, pLang->pszOperators );
	StringDupe( m_cmLang.pszSingleLineComments, pLang->pszSingleLineComments );
	StringDupe( m_cmLang.pszMultiLineComments1, pLang->pszMultiLineComments1 );
	StringDupe( m_cmLang.pszMultiLineComments2, pLang->pszMultiLineComments2 );
	StringDupe( m_cmLang.pszScopeKeywords1, pLang->pszScopeKeywords1 );
	StringDupe( m_cmLang.pszScopeKeywords2, pLang->pszScopeKeywords2 );
	StringDupe( m_cmLang.pszStringDelims, pLang->pszStringDelims );
	StringDupe( m_cmLang.pszTagElementNames, pLang->pszTagElementNames );
	StringDupe( m_cmLang.pszTagAttributeNames, pLang->pszTagAttributeNames );
	StringDupe( m_cmLang.pszTagEntities, pLang->pszTagEntities );
}

void CLanguageX::DeleteMembers()
{
    delete ( void * ) m_cmLang.pszKeywords;
    delete ( void * ) m_cmLang.pszOperators;
    delete ( void * ) m_cmLang.pszSingleLineComments;
    delete ( void * ) m_cmLang.pszMultiLineComments1;
    delete ( void * ) m_cmLang.pszMultiLineComments2;
    delete ( void * ) m_cmLang.pszScopeKeywords1;
    delete ( void * ) m_cmLang.pszScopeKeywords2;
    delete ( void * ) m_cmLang.pszStringDelims;
    delete ( void * ) m_cmLang.pszTagElementNames;
    delete ( void * ) m_cmLang.pszTagAttributeNames;
    delete ( void * ) m_cmLang.pszTagEntities;
	ZeroMemory( &m_cmLang, sizeof( m_cmLang ) );
}

HRESULT STDMETHODCALLTYPE CLanguageX::put_CaseSensitive(VARIANT_BOOL bVal)
{
	m_cmLang.bIsCaseSensitive = ( bVal != 0 );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::get_CaseSensitive(VARIANT_BOOL __RPC_FAR *pbVal)
{
	*pbVal = ( VARIANT_BOOL ) ( m_cmLang.bIsCaseSensitive ? -1 : 0 );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::put_Keywords(BSTR strVal)
{
	delete ( void * ) m_cmLang.pszKeywords;
	m_cmLang.pszKeywords = BSTR2TSTR( strVal );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::get_Keywords(BSTR __RPC_FAR *pstrVal)
{
	*pstrVal = T2BSTR( m_cmLang.pszKeywords ? m_cmLang.pszKeywords : "" );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::put_Operators(BSTR strVal)
{
	delete ( void * ) m_cmLang.pszOperators;
	m_cmLang.pszOperators = BSTR2TSTR( strVal );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::get_Operators(BSTR __RPC_FAR *pstrVal)
{
	*pstrVal = T2BSTR( m_cmLang.pszOperators ? m_cmLang.pszOperators : "" );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::put_SingleLineComments(BSTR strVal)
{
	delete ( void * ) m_cmLang.pszSingleLineComments;
	m_cmLang.pszSingleLineComments = BSTR2TSTR( strVal );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::get_SingleLineComments(BSTR __RPC_FAR *pstrVal)
{
	*pstrVal = T2BSTR( m_cmLang.pszSingleLineComments ? m_cmLang.pszSingleLineComments : "" );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::put_MultiLineComments1(BSTR strVal)
{
	delete ( void * ) m_cmLang.pszMultiLineComments1;
	m_cmLang.pszMultiLineComments1 = BSTR2TSTR( strVal );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::get_MultiLineComments1(BSTR __RPC_FAR *pstrVal)
{
	*pstrVal = T2BSTR( m_cmLang.pszMultiLineComments1 ? m_cmLang.pszMultiLineComments1 : "" );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::put_MultiLineComments2(BSTR strVal)
{
	delete ( void * ) m_cmLang.pszMultiLineComments2;
	m_cmLang.pszMultiLineComments2 = BSTR2TSTR( strVal );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::get_MultiLineComments2(BSTR __RPC_FAR *pstrVal)
{
	*pstrVal = T2BSTR( m_cmLang.pszMultiLineComments2 ? m_cmLang.pszMultiLineComments2 : "" );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::put_ScopeKeywords1(BSTR strVal)
{
	delete ( void * ) m_cmLang.pszScopeKeywords1;
	m_cmLang.pszScopeKeywords1 = BSTR2TSTR( strVal );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::get_ScopeKeywords1(BSTR __RPC_FAR *pstrVal)
{
	*pstrVal = T2BSTR( m_cmLang.pszScopeKeywords1 ? m_cmLang.pszScopeKeywords1 : "" );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::put_ScopeKeywords2(BSTR strVal)
{
	delete ( void * ) m_cmLang.pszScopeKeywords2;
	m_cmLang.pszScopeKeywords2 = BSTR2TSTR( strVal );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::get_ScopeKeywords2(BSTR __RPC_FAR *pstrVal)
{
	*pstrVal = T2BSTR( m_cmLang.pszScopeKeywords2 ? m_cmLang.pszScopeKeywords2 : "" );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::put_StringDelims(BSTR strVal)
{
	delete ( void * ) m_cmLang.pszStringDelims;
	m_cmLang.pszStringDelims = BSTR2TSTR( strVal );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::get_StringDelims(BSTR __RPC_FAR *pstrVal)
{
	*pstrVal = T2BSTR( m_cmLang.pszStringDelims ? m_cmLang.pszStringDelims : "" );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::put_EscapeChar(BSTR strVal)
{
	LPTSTR psz = BSTR2TSTR( strVal );
	m_cmLang.chEscape = *psz;
	delete psz;
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::get_EscapeChar(BSTR __RPC_FAR *pstrVal)
{
	TCHAR sz[ 2 ] = { m_cmLang.chEscape, _T('\0') }; 
	*pstrVal = T2BSTR( sz );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::put_TerminatorChar(BSTR strVal)
{
	LPTSTR psz = BSTR2TSTR( strVal );
	m_cmLang.chTerminator = *psz;
	delete psz;
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::get_TerminatorChar(BSTR __RPC_FAR *pstrVal)
{
	TCHAR sz[ 2 ] = { m_cmLang.chTerminator, _T('\0') }; 
	*pstrVal = T2BSTR( sz );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::put_TagElementNames(BSTR strVal)
{
	delete ( void * ) m_cmLang.pszTagElementNames;
	m_cmLang.pszTagElementNames = BSTR2TSTR( strVal );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::get_TagElementNames(BSTR __RPC_FAR *pstrVal)
{
	*pstrVal = T2BSTR( m_cmLang.pszTagElementNames ? m_cmLang.pszTagElementNames : "" );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::put_TagAttributeNames(BSTR strVal)
{
	delete ( void * ) m_cmLang.pszTagAttributeNames;
	m_cmLang.pszTagAttributeNames = BSTR2TSTR( strVal );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::get_TagAttributeNames(BSTR __RPC_FAR *pstrVal)
{
	*pstrVal = T2BSTR( m_cmLang.pszTagAttributeNames ? m_cmLang.pszTagAttributeNames : "" );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::put_TagEntities(BSTR strVal)
{
	delete ( void * ) m_cmLang.pszTagEntities;
	m_cmLang.pszTagEntities = BSTR2TSTR( strVal );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::get_TagEntities(BSTR __RPC_FAR *pstrVal)
{
	*pstrVal = T2BSTR( m_cmLang.pszTagEntities ? m_cmLang.pszTagEntities : "" );
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::put_Style( cmLangStyle Style )
{
	m_cmLang.dwStyle = ( DWORD ) Style;
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CLanguageX::get_Style( cmLangStyle __RPC_FAR *pStyle )
{
	*pStyle = ( cmLangStyle ) m_cmLang.dwStyle;
	return S_OK;
}
#endif
