// rangex.cpp : Implementation of CRectX

#include "precomp.h"
#ifdef _ACTIVEX
#include "editx.h"
#include "rectx.h"

/////////////////////////////////////////////////////////////////////////////
// CRectX

STDMETHODIMP CRectX::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IRect,
	};
	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (CMIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE CRectX::put_left(long lVal)
{
	m_rc.left = lVal;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CRectX::get_left(long *plVal)
{
	*plVal = m_rc.left;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CRectX::put_right(long lVal)
{
	m_rc.right = lVal;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CRectX::get_right(long *plVal)
{
	*plVal = m_rc.right;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CRectX::put_top(long lVal)
{
	m_rc.top = lVal;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CRectX::get_top(long *plVal)
{
	*plVal = m_rc.top;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CRectX::put_bottom(long lVal)
{
	m_rc.bottom = lVal;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CRectX::get_bottom(long *plVal)
{
	*plVal = m_rc.bottom;
	return S_OK;
}
#endif