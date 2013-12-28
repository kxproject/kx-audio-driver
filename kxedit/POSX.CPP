// Positionx.cpp : Implementation of CPositionX

#include "precomp.h"
#ifdef _ACTIVEX
#include "editx.h"
#include "posx.h"

/////////////////////////////////////////////////////////////////////////////
// CPositionX

STDMETHODIMP CPositionX::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IPosition,
	};
	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (CMIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE CPositionX::put_LineNo(long lPos)
{
	m_cmPos.nLine = max( 0, lPos );
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CPositionX::get_LineNo(long *plPos)
{
	*plPos = m_cmPos.nLine;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CPositionX::put_ColNo(long lPos)
{
	m_cmPos.nCol = max( 0, lPos );
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CPositionX::get_ColNo(long *plPos)
{
	*plPos = m_cmPos.nCol;
	return S_OK;
}

#endif