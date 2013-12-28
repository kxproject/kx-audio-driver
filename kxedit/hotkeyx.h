// HotKeyx.h : Declaration of the CHotKeyX

#ifndef __HotKeyX_H_
#define __HotKeyX_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CHotKeyX
class ATL_NO_VTABLE CHotKeyX : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CHotKeyX, &CLSID_HotKey>,
	public ISupportErrorInfo,
	public IDispatchImpl<IHotKey, &IID_IHotKey, &LIBID_CodeMax, CM_X_VERSION>
{
public:
	CHotKeyX()
	{ 
		// set all values to zero
		ZeroMemory( &m_cmHotKey, sizeof( m_cmHotKey ) );
	}
	CM_HOTKEY m_cmHotKey;

public:

DECLARE_REGISTRY_RESOURCEID(IDR_HOTKEYX)

BEGIN_COM_MAP(CHotKeyX)
	COM_INTERFACE_ENTRY(IHotKey)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IHotKey
    virtual HRESULT STDMETHODCALLTYPE put_Modifiers1( long lVal );
    virtual HRESULT STDMETHODCALLTYPE get_Modifiers1( long __RPC_FAR *plVal );
    virtual HRESULT STDMETHODCALLTYPE put_VirtKey1( BSTR strKey );
    virtual HRESULT STDMETHODCALLTYPE get_VirtKey1( BSTR __RPC_FAR *pstrKey );
    virtual HRESULT STDMETHODCALLTYPE put_Modifiers2( long lVal );
    virtual HRESULT STDMETHODCALLTYPE get_Modifiers2( long __RPC_FAR *plVal );
    virtual HRESULT STDMETHODCALLTYPE put_VirtKey2( BSTR strKey );
    virtual HRESULT STDMETHODCALLTYPE get_VirtKey2( BSTR __RPC_FAR *pstrKey );

public:
};

#endif //__HotKeyX_H_
