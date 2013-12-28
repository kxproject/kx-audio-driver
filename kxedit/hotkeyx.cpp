// HotKeyX.cpp : Implementation of CHotKeyX

#include "precomp.h"
#ifdef _ACTIVEX
#include "editx.h"
#include "HotKeyX.h"

/////////////////////////////////////////////////////////////////////////////
// CHotKeyX

STDMETHODIMP CHotKeyX::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IHotKey,
	};
	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (CMIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE CHotKeyX::put_Modifiers1( long lVal )
{
	m_cmHotKey.byModifiers1 = ( BYTE ) lVal;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CHotKeyX::get_Modifiers1( long __RPC_FAR *plVal )
{
	*plVal = m_cmHotKey.byModifiers1;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CHotKeyX::put_VirtKey1( BSTR strKey )
{
	LPTSTR psz = BSTR2TSTR( strKey );
#ifdef _UNICODE
	m_cmHotKey.nVirtKey1 = (unsigned wchar_t)*psz;
#else
	m_cmHotKey.nVirtKey1 = (unsigned char)*psz;
#endif
	delete psz;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CHotKeyX::get_VirtKey1( BSTR __RPC_FAR *pstrKey )
{
	TCHAR sz[ 2 ] = { m_cmHotKey.nVirtKey1, _T('\0') };
	USES_CONVERSION;
	*pstrKey = SysAllocString( T2COLE( sz ) );
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CHotKeyX::put_Modifiers2( long lVal )
{
	m_cmHotKey.byModifiers2 = ( BYTE ) lVal;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CHotKeyX::get_Modifiers2( long __RPC_FAR *plVal )
{
	*plVal = m_cmHotKey.byModifiers2;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CHotKeyX::put_VirtKey2( BSTR strKey )
{
	LPTSTR psz = BSTR2TSTR( strKey );
#ifdef _UNICODE
	m_cmHotKey.nVirtKey2 = (unsigned wchar_t)*psz;
#else
	m_cmHotKey.nVirtKey2 = (unsigned char)*psz;
#endif
	delete psz;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CHotKeyX::get_VirtKey2( BSTR __RPC_FAR *pstrKey )
{
	TCHAR sz[ 2 ] = { m_cmHotKey.nVirtKey2, _T('\0') };
	USES_CONVERSION;
	*pstrKey = SysAllocString( T2COLE( sz ) );
	return S_OK;
}
#endif