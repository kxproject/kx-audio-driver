// Rectx.h : Declaration of the CRectX

#ifndef __RectX_H_
#define __RectX_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CRectX
class ATL_NO_VTABLE CRectX : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CRectX, &CLSID_Rect>,
	public ISupportErrorInfo,
	public IDispatchImpl<IRect, &IID_IRect, &LIBID_CodeMax, CM_X_VERSION>
{
public:
	CRectX()
	{ 
		// set all values to zero
		ZeroMemory( &m_rc, sizeof( m_rc ) );
	}
	RECT m_rc;

public:

DECLARE_REGISTRY_RESOURCEID(IDR_RECTX)

BEGIN_COM_MAP(CRectX)
	COM_INTERFACE_ENTRY(IRect)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IRect
	virtual HRESULT STDMETHODCALLTYPE put_left(long lVal);
	virtual HRESULT STDMETHODCALLTYPE get_left(long *plVal);
	virtual HRESULT STDMETHODCALLTYPE put_right(long lVal);
	virtual HRESULT STDMETHODCALLTYPE get_right(long *plVal);
	virtual HRESULT STDMETHODCALLTYPE put_top(long lVal);
	virtual HRESULT STDMETHODCALLTYPE get_top(long *plVal);
	virtual HRESULT STDMETHODCALLTYPE put_bottom(long lVal);
	virtual HRESULT STDMETHODCALLTYPE get_bottom(long *plVal);
};

#endif //__RectX_H_
