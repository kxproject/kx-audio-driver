//  posx.h : Declaration of the CPositionX

#ifndef __POSX_H_
#define __POSX_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CPositionX
class ATL_NO_VTABLE CPositionX : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPositionX, &CLSID_Position>,
	public ISupportErrorInfo,
	public IDispatchImpl<IPosition, &IID_IPosition, &LIBID_CodeMax, CM_X_VERSION>
{
public:
	CPositionX()
	{ 
		// set all values to zero
		ZeroMemory( &m_cmPos, sizeof( m_cmPos ) );
	}
	CM_POSITION m_cmPos;

public:

DECLARE_REGISTRY_RESOURCEID(IDR_POSITIONX)

BEGIN_COM_MAP(CPositionX)
	COM_INTERFACE_ENTRY(IPosition)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IPosition
	virtual HRESULT STDMETHODCALLTYPE put_LineNo(long lPos);
	virtual HRESULT STDMETHODCALLTYPE get_LineNo(long *plPos);
	virtual HRESULT STDMETHODCALLTYPE put_ColNo(long lPos);
	virtual HRESULT STDMETHODCALLTYPE get_ColNo(long *plPos);
public:
};

#endif
