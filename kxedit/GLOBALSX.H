// Globalsx.h : Declaration of the CGlobalsX

#ifndef __GLOBALSX_H_
#define __GLOBALSX_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CGlobalsX
class ATL_NO_VTABLE CGlobalsX : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CGlobalsX, &CLSID_Globals>,
	public ISupportErrorInfo,
	public IDispatchImpl<IGlobals, &IID_IGlobals, &LIBID_CodeMax, CM_X_VERSION>
{
public:

DECLARE_REGISTRY_RESOURCEID(IDR_GLOBALSX)

BEGIN_COM_MAP(CGlobalsX)
	COM_INTERFACE_ENTRY(IGlobals)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IGlobals
public:
	HRESULT STDMETHODCALLTYPE put_FindMRUList( BSTR strMRUList );
	HRESULT STDMETHODCALLTYPE get_FindMRUList( BSTR __RPC_FAR *pstrMRUList );
	HRESULT STDMETHODCALLTYPE put_ReplaceMRUList( BSTR strMRUList );
	HRESULT STDMETHODCALLTYPE get_ReplaceMRUList( BSTR __RPC_FAR *pstrMRUList );
	HRESULT STDMETHODCALLTYPE RegisterLanguage( BSTR strName, ILanguage __RPC_FAR *LanguageDef );
	HRESULT STDMETHODCALLTYPE UnregisterLanguage( BSTR strName );
	HRESULT STDMETHODCALLTYPE UnregisterAllLanguages();
	HRESULT STDMETHODCALLTYPE ResetDefaultHotKeys();
	HRESULT STDMETHODCALLTYPE GetHotKeyForCmd( cmCommand lCmd, long lNum, IHotKey __RPC_FAR *__RPC_FAR *ppHotKey );
	HRESULT STDMETHODCALLTYPE GetNumHotKeysForCmd( cmCommand lCmd, long __RPC_FAR *plCount );
	HRESULT STDMETHODCALLTYPE RegisterHotKey( IHotKey __RPC_FAR *pHotKey, cmCommand lCmd );
	HRESULT STDMETHODCALLTYPE UnregisterHotKey( IHotKey __RPC_FAR *pHotKey );
	HRESULT STDMETHODCALLTYPE GetCommandString( cmCommand lCmd, VARIANT_BOOL bDescription, BSTR __RPC_FAR *pstrVal );
	HRESULT STDMETHODCALLTYPE GetHotKeys( SAFEARRAY __RPC_FAR * __RPC_FAR *ppBuff );
	HRESULT STDMETHODCALLTYPE SetHotKeys( SAFEARRAY __RPC_FAR * __RPC_FAR *ppBuff );
	HRESULT STDMETHODCALLTYPE GetMacro( long lMacro, SAFEARRAY __RPC_FAR * __RPC_FAR *ppBuff );
	HRESULT STDMETHODCALLTYPE SetMacro( long lMacro, SAFEARRAY __RPC_FAR * __RPC_FAR *ppBuff );
	HRESULT STDMETHODCALLTYPE RegisterCommand( long lCmdId, BSTR strName, BSTR strDescription );
	HRESULT STDMETHODCALLTYPE UnregisterCommand( long lCmdId );
	HRESULT STDMETHODCALLTYPE GetLanguageDef( BSTR strName, ILanguage __RPC_FAR *__RPC_FAR *ppLang );
};

#endif
