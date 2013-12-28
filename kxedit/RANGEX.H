// rangex.h : Declaration of the CRangeX

#ifndef __RANGEX_H_
#define __RANGEX_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CRangeX
class ATL_NO_VTABLE CRangeX : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CRangeX, &CLSID_Range>,
	public ISupportErrorInfo,
	public IDispatchImpl<IRange, &IID_IRange, &LIBID_CodeMax, CM_X_VERSION>
{
public:
	CRangeX()
	{ 
		// set all values to zero
		ZeroMemory( &m_cmRange, sizeof( m_cmRange ) );
	}
	CM_RANGE m_cmRange;

public:

DECLARE_REGISTRY_RESOURCEID(IDR_RANGEX)

BEGIN_COM_MAP(CRangeX)
	COM_INTERFACE_ENTRY(IRange)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IRange
	virtual HRESULT STDMETHODCALLTYPE put_StartLineNo( long lPos );
	virtual HRESULT STDMETHODCALLTYPE get_StartLineNo( long *plPos );
	virtual HRESULT STDMETHODCALLTYPE put_StartColNo( long lPos );
	virtual HRESULT STDMETHODCALLTYPE get_StartColNo( long *plPos );
	virtual HRESULT STDMETHODCALLTYPE put_EndLineNo( long lPos );
	virtual HRESULT STDMETHODCALLTYPE get_EndLineNo( long *plPos );
	virtual HRESULT STDMETHODCALLTYPE put_EndColNo( long lPos );
	virtual HRESULT STDMETHODCALLTYPE get_EndColNo( long *plPos );
	virtual HRESULT STDMETHODCALLTYPE put_ColumnSel( VARIANT_BOOL bVal );
	virtual HRESULT STDMETHODCALLTYPE get_ColumnSel( VARIANT_BOOL *pbVal );
	virtual HRESULT STDMETHODCALLTYPE IsEmpty( VARIANT_BOOL __RPC_FAR *pbAnswer );
	virtual HRESULT STDMETHODCALLTYPE Empty();
	virtual HRESULT STDMETHODCALLTYPE Normalize();
public:
};

#endif //__RANGEX_H_
