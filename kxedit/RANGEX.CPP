// rangex.cpp : Implementation of CRangeX

#include "precomp.h"
#ifdef _ACTIVEX
#include "editx.h"
#include "rangex.h"

/////////////////////////////////////////////////////////////////////////////
// CRangeX

STDMETHODIMP CRangeX::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IRange,
	};
	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (CMIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE CRangeX::put_StartLineNo(long lPos)
{
	m_cmRange.posStart.nLine = max( 0, lPos );
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CRangeX::get_StartLineNo(long *plPos)
{
	*plPos = m_cmRange.posStart.nLine;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CRangeX::put_StartColNo(long lPos)
{
	m_cmRange.posStart.nCol = max( 0, lPos );
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CRangeX::get_StartColNo(long *plPos)
{
	*plPos = m_cmRange.posStart.nCol;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CRangeX::put_EndLineNo(long lPos)
{
	m_cmRange.posEnd.nLine = max( 0, lPos );
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CRangeX::get_EndLineNo(long *plPos)
{
	*plPos = m_cmRange.posEnd.nLine;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CRangeX::put_EndColNo(long lPos)
{
	m_cmRange.posEnd.nCol = max( 0, lPos );
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CRangeX::get_EndColNo(long *plPos)
{
	*plPos = m_cmRange.posEnd.nCol;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CRangeX::put_ColumnSel(VARIANT_BOOL bVal)
{
	m_cmRange.bColumnSel = ( bVal != FALSE );
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CRangeX::get_ColumnSel(VARIANT_BOOL *pbVal)
{
	*pbVal = m_cmRange.bColumnSel;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CRangeX::IsEmpty( VARIANT_BOOL __RPC_FAR *pbAnswer )
{
	*pbAnswer = ( VARIANT_BOOL ) ( ( m_cmRange.posStart.nCol == m_cmRange.posEnd.nCol && 
	                                 m_cmRange.posStart.nLine == m_cmRange.posEnd.nLine ) ? -1 : 0 );
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CRangeX::Empty()
{
	m_cmRange.posStart.nCol = m_cmRange.posEnd.nCol;
	m_cmRange.posStart.nLine = m_cmRange.posEnd.nLine;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CRangeX::Normalize()
{
	if ( m_cmRange.posStart.nLine == m_cmRange.posEnd.nLine )
	{
		if ( m_cmRange.posStart.nCol > m_cmRange.posEnd.nCol )
		{
			// normalize cols
			int nSwap = m_cmRange.posStart.nCol;
			m_cmRange.posStart.nCol = m_cmRange.posEnd.nCol;
			m_cmRange.posEnd.nCol = nSwap;
		}
	}
	else if ( m_cmRange.posStart.nLine > m_cmRange.posEnd.nLine )
	{
		// normalize rows
		int nSwap = m_cmRange.posStart.nLine;
		m_cmRange.posStart.nLine = m_cmRange.posEnd.nLine;
		m_cmRange.posEnd.nLine = nSwap;
	}
	return S_OK;
}

#endif