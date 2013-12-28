#include "precomp.h"
#include "cedit.h"
#include "dragdrop.h"

CDataObject::CDataObject( HGLOBAL hGlobal )
{
	m_hGlobal = hGlobal;
	m_XEnumFORMATETC.SetOuterUnknown( this );
	VERIFY( SUCCEEDED( CoLockObjectExternal( ( LPUNKNOWN ) ( LPVOID ) &m_XEnumFORMATETC, TRUE, FALSE ) ) );
}

CDataObject::~CDataObject()
{
	VERIFY( SUCCEEDED( CoLockObjectExternal( ( LPUNKNOWN ) &m_XEnumFORMATETC, FALSE, TRUE ) ) );
}

STDMETHODIMP CDataObject::QueryInterface( REFIID riid, LPVOID * ppv )
{
	*ppv=NULL;

	if ( IsEqualIID( riid, IID_IUnknown ) )
	{
		*ppv = this;
	}
	else if ( IsEqualIID( riid, IID_IDataObject ) )
	{
		*ppv = static_cast< IDataObject * >( this );
	}
	else if ( IsEqualIID( riid, IID_IEnumFORMATETC ) )
	{
		*ppv = reinterpret_cast< IEnumFORMATETC * >( &m_XEnumFORMATETC );
	}

	if ( *ppv )
	{
		( ( LPUNKNOWN ) *ppv )->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_( ULONG ) CDataObject::AddRef()
{
	return 2;
}

STDMETHODIMP_( ULONG ) CDataObject::Release()
{
	return 1;
}

STDMETHODIMP CDataObject::GetData( LPFORMATETC pFE, LPSTGMEDIUM pSTM )
{
	switch ( pFE->cfFormat )
	{
		case CLIP_TEXT:
		{
			if ( !HAS_FLAG( pFE->tymed, TYMED_HGLOBAL ) )
			{
				return DV_E_TYMED;
			}

			pSTM->tymed = TYMED_HGLOBAL;
			pSTM->hGlobal = m_hGlobal;
			pSTM->pUnkForRelease = NULL;
			return S_OK;
		}
	}

	return DATA_E_FORMATETC;
}

STDMETHODIMP CDataObject::GetDataHere( LPFORMATETC, LPSTGMEDIUM )
{
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::QueryGetData( LPFORMATETC pFE )
{
	return ( pFE->cfFormat == CLIP_TEXT ) ? S_OK : DATA_E_FORMATETC;
}

STDMETHODIMP CDataObject::GetCanonicalFormatEtc( LPFORMATETC, LPFORMATETC )
{
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::SetData( LPFORMATETC, STGMEDIUM FAR *, BOOL )
{
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::EnumFormatEtc( DWORD, LPENUMFORMATETC *ppv )
{
	*ppv = reinterpret_cast< IEnumFORMATETC * >( &m_XEnumFORMATETC );
	( static_cast< IUnknown * >( *ppv ) )->AddRef(); // does nothing, but might someday
	return S_OK;
}

STDMETHODIMP CDataObject::DAdvise( LPFORMATETC /*pFormatetc*/, DWORD /*advf*/, LPADVISESINK /*pAdvSink*/, LPDWORD /*pdwConnection*/ )
{
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::DUnadvise( DWORD /*dwConnection*/ )
{
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::EnumDAdvise( LPENUMSTATDATA * /*ppenumAdvise*/ )
{
	return E_NOTIMPL;
}

STDMETHODIMP CDataObject::XEnumFORMATETC::QueryInterface( REFIID riid, LPVOID * ppv )
{
	return m_pUnkOuter->QueryInterface( riid, ppv );
}

STDMETHODIMP_( ULONG ) CDataObject::XEnumFORMATETC::AddRef()
{
	return m_pUnkOuter->AddRef();
}

STDMETHODIMP_( ULONG ) CDataObject::XEnumFORMATETC::Release()
{
	return m_pUnkOuter->Release();
}

STDMETHODIMP CDataObject::XEnumFORMATETC::Next( ULONG celt, FORMATETC *rgelt, ULONG *pceltFetched )
{
	if ( !m_nPos && celt > 0 && rgelt )
	{
		rgelt->cfFormat = CLIP_TEXT;
		rgelt->ptd = NULL;
		rgelt->dwAspect = DVASPECT_CONTENT;
		rgelt->lindex = -1;
		rgelt->tymed = TYMED_HGLOBAL;
		
		if ( pceltFetched )
		{
			*pceltFetched = 1;
		}

		m_nPos++;
		return S_OK;
	}
	else
	{
		m_nPos = 0; // implicit Reset()
		if ( pceltFetched )
		{
			*pceltFetched = 0;
		}
		return S_FALSE; // no more left
	}
}

STDMETHODIMP CDataObject::XEnumFORMATETC::Skip( ULONG )
{
	// only have one FORMATETC!
	return S_FALSE;
}

STDMETHODIMP CDataObject::XEnumFORMATETC::Reset()
{
	m_nPos = 0;
	return S_OK;
}

STDMETHODIMP CDataObject::XEnumFORMATETC::Clone( IEnumFORMATETC ** )
{
	return E_NOTIMPL;
}

CDropSource::CDropSource()
{
}

STDMETHODIMP CDropSource::QueryInterface( REFIID riid, LPVOID * ppv )
{
	*ppv=NULL;

	if ( IsEqualIID( riid, IID_IUnknown ) )
	{
		*ppv = static_cast< IUnknown * >( this );
	}
	else if ( IsEqualIID( riid, IID_IDropSource ) )
	{
		*ppv = static_cast< IDropSource * >( this );
	}

	if ( *ppv )
	{
		( ( LPUNKNOWN ) *ppv )->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_( ULONG ) CDropSource::AddRef()
{
	return 2;
}

STDMETHODIMP_( ULONG ) CDropSource::Release()
{
	return 1;
}

STDMETHODIMP CDropSource::QueryContinueDrag( BOOL bEsc, DWORD grfKeyState )
{
	if ( bEsc )
	{
		return DRAGDROP_S_CANCEL;
	}

	if ( ! ( grfKeyState & MK_LBUTTON ) )
	{
		return DRAGDROP_S_DROP;
	}

	return S_OK;
}

STDMETHODIMP CDropSource::GiveFeedback( DWORD /* dwEffect */ )
{
	return DRAGDROP_S_USEDEFAULTCURSORS;
}

CDropTarget::CDropTarget()
{
	m_pCtrl = NULL;
	m_pIDataObject = NULL;
	m_dwSourceEffect = DROPEFFECT_COPY | DROPEFFECT_MOVE;
}

void CDropTarget::SetCtrl( CEdit *pCtrl )
{
	ASSERT( pCtrl );
	m_pCtrl = pCtrl;
}

STDMETHODIMP CDropTarget::QueryInterface( REFIID riid, LPVOID * ppv )
{
	*ppv=NULL;

	if ( IsEqualIID( riid, IID_IUnknown ) )
	{
		*ppv = static_cast< IUnknown * >( this );
	}
	if ( IsEqualIID( riid, IID_IDropTarget ) )
	{
		*ppv = static_cast< IDropTarget * >( this );
	}

	if ( NULL != *ppv )
	{
		( ( LPUNKNOWN ) *ppv )->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_( ULONG ) CDropTarget::AddRef()
{
	return 2;
}

STDMETHODIMP_( ULONG ) CDropTarget::Release()
{
	return 1;
}

STDMETHODIMP CDropTarget::DragEnter( LPDATAOBJECT pIDataSource, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect )
{
	m_pIDataObject = NULL;
	m_dwSourceEffect = *pdwEffect;

	ASSERT( m_pCtrl );
	m_pCtrl->OnDragEnter( pIDataSource, grfKeyState, pt, pdwEffect );

	m_pIDataObject = pIDataSource;
	m_pIDataObject->AddRef();

	return S_OK;
}

STDMETHODIMP CDropTarget::DragOver( DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect )
{
	if ( !m_pIDataObject )
	{
		*pdwEffect = DROPEFFECT_NONE;
		return S_OK;
	}

	ASSERT( m_pCtrl );
	m_pCtrl->OnDragOver( m_pIDataObject, grfKeyState, pt, pdwEffect );

	return S_OK;
}

STDMETHODIMP CDropTarget::DragLeave()
{
	if ( !m_pIDataObject )
		return S_OK;

	ASSERT( m_pCtrl );
	m_pCtrl->OnDragLeave();

	m_pIDataObject->Release();
	return S_OK;
}

STDMETHODIMP CDropTarget::Drop( LPDATAOBJECT pIDataSource, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect )
{
	*pdwEffect = DROPEFFECT_NONE;

	if ( !m_pIDataObject )
		return E_FAIL;
	
	HRESULT hr = E_FAIL;
	ASSERT( m_pCtrl );
	if ( m_pCtrl->OnDrop( pIDataSource, grfKeyState, pt, pdwEffect ) )
	{
		hr = S_OK;
	}

	m_pIDataObject->Release();

	return hr;
}

void CDropTarget::NormalizeDropEffect( DWORD &dwEffect ) const
{
	// switch from move to copy if source object doesn't allow move
	if ( HAS_FLAG( dwEffect, DROPEFFECT_MOVE ) &&
	     !HAS_FLAG( m_dwSourceEffect, DROPEFFECT_MOVE ) )
		{
		dwEffect &= ~DROPEFFECT_MOVE;
		dwEffect |= DROPEFFECT_COPY;
		}
}

void CEdit::DrawDragRect( const RECT &rc ) const
{
	HDC hDC = GetDC( m_hWnd );
	short bits[8] = { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 };
	HBITMAP hbmPattern = CreateBitmap( 8, 8, 1, 1, &bits );
	HBRUSH hbrPattern = CreatePatternBrush( hbmPattern );
	HBRUSH hOldBrush = ( HBRUSH ) SelectObject( hDC, hbrPattern );
	VERIFY( PatBlt( hDC, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, PATINVERT ) );
	SelectObject( hDC, hOldBrush );
	DeleteObject( hbrPattern );
	DeleteObject( hbmPattern );
	ReleaseDC( m_hWnd, hDC );
}

void CEdit::DrawDragCaret( BOOL bMakeEmpty ) const
{
	if ( m_rcDragCaret.right )
		DrawDragRect( m_rcDragCaret );

	if ( bMakeEmpty )
		SetRectEmpty( ( LPRECT ) &m_rcDragCaret );
}

void CEdit::OnDragEnter( LPDATAOBJECT pIDataSource, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect )
{
	CEditView *pDontCare;
	SetRectEmpty( &m_rcDragCaret );
	int nDontCare1, nDontCare2;
	*pdwEffect = GetDropEffect( pIDataSource, grfKeyState, pt, pDontCare, nDontCare1, nDontCare2 );
}

void CEdit::OnDragOver( LPDATAOBJECT pIDataSource, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect )
{
	CEditView *pView;
	int nBuffCol, nRow;
	*pdwEffect = GetDropEffect( pIDataSource, grfKeyState, pt, pView, nBuffCol, nRow );

	if ( *pdwEffect != DROPEFFECT_NONE )
	{
		RECT rcDragCaret;
		m_Selection.GetCaretRect( pView, nBuffCol, nRow, rcDragCaret );
		if ( !EqualRect( &rcDragCaret, &m_rcDragCaret ) )
		{
			// erase old caret
			DrawDragCaret( FALSE );

			m_rcDragCaret = rcDragCaret;

			// draw new caret
			DrawDragCaret( FALSE );
		}
	}
	else
	{
		// erase old caret
		DrawDragCaret( TRUE );
	}
}

void CEdit::OnDragLeave()
{
	// erase old caret
	DrawDragCaret( TRUE );
}

BOOL CEdit::OnDrop( LPDATAOBJECT pIDataSource, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect )
{
	CEditView *pView;
	int nBuffCol, nRow;
	*pdwEffect = GetDropEffect( pIDataSource, grfKeyState, pt, pView, nBuffCol, nRow );
	m_bDroppedHere = TRUE;
	
	BOOL bDropped = FALSE;
	if ( *pdwEffect != DROPEFFECT_NONE )
	{
		FORMATETC fe = { CLIP_TEXT, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		STGMEDIUM stgm;
		if ( !SUCCEEDED( pIDataSource->GetData( &fe, &stgm ) ) )
			{
				// data could not be retrieved
				m_Selection.SetEmptySelection( nBuffCol, nRow );
				m_Selection.ShowCaret();
				return FALSE;
			}
		HGLOBAL hMem = stgm.hGlobal;
		ASSERT( hMem );
		LPCTSTR pszText = ( LPCTSTR ) GlobalLock( hMem );

		m_Buffer.BeginEdit( m_Selection.GetEndRow(), m_Selection.GetEndCol() );
		if ( pszText && IsDragAndDropping() )
		{
			// we are dragging and dropping from within this control -- be careful how
			// the *current* selection is handled
			POINT ptClient = { pt.x, pt.y };
			ScreenToClient( m_hWnd, &ptClient );
			if ( PtInSelection( ptClient.x, ptClient.y, FALSE ) )
			{
				m_Selection.SetEmptySelection( nBuffCol, nRow );
			}
			else
			{
				BOOL bMove = HAS_FLAG( *pdwEffect, DROPEFFECT_MOVE );
				int nStartRow, nStartCol, nEndRow, nEndCol;
				m_Selection.GetNormalizedBufferSelection( nStartCol, nStartRow, nEndCol, nEndRow );
				if (nRow == nStartRow && nBuffCol == nStartCol)
				{
					// nothing really to do -- the source and destination are identical.
					// We do however need to erase the drag insertion caret.
					DamageSelection(FALSE);
				}
				else if ( ( nRow < nStartRow ) || ( nRow == nStartRow && nBuffCol < nStartCol ) )
				{
					// dragging before the selection -- delete the selection first (if move)
					if ( bMove )
						DeleteSelection( FALSE, FALSE );
					m_Selection.SetEmptySelection( nBuffCol, nRow );
					ReplaceSelection( pszText, FALSE, TRUE, TRUE );
				}
				else
				{
					// dragging after the selection -- insert the new text first
					m_Selection.SetEmptySelection( nBuffCol, nRow );
					ReplaceSelection( pszText, FALSE, TRUE, TRUE );
					int nStartRowNew, nStartColNew, nEndRowNew, nEndColNew;
					m_Selection.GetNormalizedBufferSelection( nStartColNew, nStartRowNew, nEndColNew, nEndRowNew );
					if ( bMove )
					{
						m_Selection.SetExtendedSelection( nStartCol, nStartRow, nEndCol, nEndRow );
						DeleteSelection( FALSE, FALSE );
					}
					// we want to select the dropped text, but we must first factor in the 
					// shift caused by the text that was just deleted.
					if ( nStartRowNew == nEndRow )
					{
						ASSERT( nStartColNew >= nEndCol );
						if ( nStartRow == nEndRow )
							nStartColNew -= nEndCol - nStartCol;
						else if ( nStartRowNew == nEndRow )
							nStartColNew = nStartCol + nStartColNew - nEndCol;
						else
							nStartColNew -= nEndCol;
					}
					nStartRowNew -= nEndRow - nStartRow;

					if ( nEndRowNew == nEndRow )
					{
						ASSERT( nEndColNew > nEndCol );
						if ( nStartRow == nEndRow )
							nEndColNew -= nEndCol - nStartCol;
						else
							nEndColNew -= nEndCol;
					}
					nEndRowNew -= nEndRow - nStartRow;
					// select the dropped text
					m_Selection.SetExtendedSelection( nStartColNew, nStartRowNew, nEndColNew, nEndRowNew );
				}
			}
		}
		else
		{
			// data came from another window -- just insert it as usual -- the other window
			// will remove the text if a move
			m_Selection.SetEmptySelection( nBuffCol, nRow );
			ReplaceSelection( pszText, FALSE, TRUE, TRUE );
		}

		if ( m_pActiveView != pView )
		{
			SetActiveView( pView );
		}
		m_Buffer.EndEdit( m_Selection.GetEndRow(), m_Selection.GetEndCol() );
		GlobalUnlock( hMem );
		if ( !stgm.pUnkForRelease )
		{
			// provider of data decided that we should release the data
			ReleaseStgMedium( &stgm );
		}

		bDropped = TRUE;
	}

	m_Selection.ShowCaret();

	return bDropped;
}

DWORD CEdit::GetDropEffect( LPDATAOBJECT pIDataSource, DWORD grfKeyState, POINTL pt, CEditView *&pView, int &nBuffCol, int &nRow )
{
	DWORD dwEffect = DROPEFFECT_NONE;

	if ( !m_Buffer.IsReadOnly() )
	{
		POINT ptClient = { pt.x, pt.y };
		ScreenToClient( m_hWnd, &ptClient );
		int nView;
		if ( HitTest( ptClient.x, ptClient.y, nView ) == eEditSpace && !PtInSelection( ptClient.x, ptClient.y, FALSE ) )
		{
			FORMATETC fe = { CLIP_TEXT, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
			if ( pIDataSource->QueryGetData( &fe ) == S_OK )
			{
				RECT rcChar;
				pView = m_Views[ nView ];
				pView->GetCharPosFromPoint( ptClient.x, ptClient.y, nBuffCol, nRow, &rcChar );
				nBuffCol = m_Buffer.ConvertViewColToBufferCol( nRow, nBuffCol );
				if ( !IsRectEmpty( &rcChar ) && ( ptClient.x > ( ( rcChar.left + rcChar.right ) / 2 ) ) )
				{
					// cursor is closer to the next char
					nBuffCol += m_Buffer.GetCharSize( nRow, nBuffCol );
				}
				dwEffect = ( HAS_FLAG( grfKeyState, MK_CONTROL ) ? DROPEFFECT_COPY : DROPEFFECT_MOVE );
				if ( pView->ScrollIfNearBorder( ptClient.x, ptClient.y, TRUE ) )
				{
					dwEffect |= DROPEFFECT_SCROLL;
				}
				// make sure the effect is allowed by the source object
				m_DropTarget.NormalizeDropEffect( dwEffect );
			}
		}
	}

	return dwEffect;
}

void CEdit::DoDragDrop()
{
	int nStartCol, nStartRow, nEndCol, nEndRow;
	ASSERT( !m_Selection.IsEmpty() );
	// Drag and drop does not currently support column selection.  Convert
	// the column selection to a paragraph selection as a fallback.
	if ( m_Selection.IsColumnSel() )
		{
		m_Selection.EnableColumnSel( FALSE );
		DamageSelection( TRUE );
		}
	m_Selection.GetNormalizedBufferSelection( nStartCol, nStartRow, nEndCol, nEndRow );
	HGLOBAL hGlobal;
	if ( m_Buffer.GetText( nStartRow, nStartCol, nEndRow, nEndCol, hGlobal, FALSE ) )
	{
		CDataObject DataObj( hGlobal );

		CDropSource DropSource;
		VERIFY( SUCCEEDED( CoLockObjectExternal( ( LPUNKNOWN ) ( LPVOID ) &DataObj, TRUE, FALSE ) ) );
		VERIFY( SUCCEEDED( CoLockObjectExternal( ( LPUNKNOWN ) &DropSource, TRUE, FALSE ) ) );
		DWORD dwEffect;
		m_bDroppedHere = FALSE;
		if ( ::DoDragDrop( &DataObj, &DropSource, DROPEFFECT_COPY | ( m_Buffer.IsReadOnly() ? 0 : DROPEFFECT_MOVE ), &dwEffect ) == DRAGDROP_S_DROP )
		{
			// text was dropped successfully -- The OnDrop() method of the drop target freed the memory
			// because CDataObject::GetData() set pUnkForRelease = NULL.
			
			// if moved text to another window, delete the selection here
			if ( !m_bDroppedHere && HAS_FLAG( dwEffect, DROPEFFECT_MOVE ) )
			{
				DeleteSelection( FALSE, FALSE );
			}
		}
		else
		{
			// cleanup clip data since target window didn't drop the text
			GlobalFree( hGlobal );
		}
		VERIFY( SUCCEEDED( CoLockObjectExternal( ( LPUNKNOWN ) &DropSource, FALSE, TRUE ) ) );
		VERIFY( SUCCEEDED( CoLockObjectExternal( ( LPUNKNOWN ) ( LPVOID ) &DataObj, FALSE, TRUE ) ) );
		DataObj.Release();
	}
	SetMode( eIdle );
}

BOOL CEdit::IsDragAndDropping() const
{
	return ( m_eMode == eDragAndDrop );
}
