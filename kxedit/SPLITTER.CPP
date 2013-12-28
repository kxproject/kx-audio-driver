#include "precomp.h"
#include "cedit.h"

BOOL CEdit::IsSplitting() const
{
	Mode eMode = m_eMode;
	return ( eMode == eHSplitting || eMode == eVSplitting || eMode == eHVSplitting );
}

void CEdit::BeginSplitting( HitTestCode eHitTest )
{
	SetCapture( m_hWnd );

	m_xPosHSplitterBegin = m_xPosHSplitter;
	m_yPosVSplitterBegin = m_yPosVSplitter;

	switch ( eHitTest )
	{
		case eHSplitter:
		{
			SetMode( eHSplitting );
			break;
		}
		case eVSplitter:
		{
			SetMode( eVSplitting );
			break;
		}
		case eHVSplitter:
		{
			SetMode( eHVSplitting );
			break;
		}
		#if 0
		default:
		{
			ASSERT( FALSE );
			break;
		}
		#endif
	}
	
	DrawMovingSplitters();
}

void CEdit::UpdateSplitter( int xPos, int yPos )
{
	ASSERT( IsSplitting() );

	BOOL bUpdateSplitter = FALSE;
	int xNewPos = m_xPosHSplitterBegin + xPos - m_xPosDown;
	int yNewPos = m_yPosVSplitterBegin + yPos - m_yPosDown;

	RECT rc;
	GetClientRect( m_hWnd, &rc );

	BOOL bHSplit = ( m_eMode == eHSplitting || m_eMode == eHVSplitting );
	BOOL bVSplit = ( m_eMode == eVSplitting || m_eMode == eHVSplitting );

	switch ( m_eMode )
	{
		case eHSplitting:
		{
			bUpdateSplitter = ( xNewPos != m_xPosHSplitter );
			break;
		}
		case eVSplitting:
		{
			bUpdateSplitter = ( yNewPos != m_yPosVSplitter );
			break;
		}
		case eHVSplitting:
		{
			bUpdateSplitter = ( ( xNewPos != m_xPosHSplitter ) || ( yNewPos != m_yPosVSplitter ) );
			break;
		}
	}

	if ( bUpdateSplitter )
	{
		if ( bHSplit )
		{
			if ( xNewPos < 0 )
			{
				xNewPos = 0;
			}
			else
			{
				int nExtreme = rc.right - ( ShowVScrollBar() ? GetSystemMetrics( SM_CXVSCROLL ) : 0 ) - CXY_SPLITTER_DRAG;
				if ( xNewPos > nExtreme )
				{
					xNewPos = nExtreme;
				}
			}
		}

		if ( bVSplit )
		{
			if ( yNewPos < 0 )
			{
				yNewPos = 0;
			}
			else
			{
				int nExtreme = rc.bottom - ( ShowHScrollBar() ? GetSystemMetrics( SM_CYHSCROLL ) : 0 ) - CXY_SPLITTER_DRAG;
				if ( yNewPos > nExtreme )
				{
					yNewPos = nExtreme;
				}
			}
		}

		if ( ( bHSplit && m_xPosHSplitter != xNewPos ) ||
		     ( bVSplit && m_yPosVSplitter != yNewPos ) )
		{
			// erase
			DrawMovingSplitters();

			if ( bHSplit )
			{
				m_xPosHSplitter = xNewPos;
			}
			if ( bVSplit )
			{
				m_yPosVSplitter = yNewPos;
			}

			// redraw
			DrawMovingSplitters();
		}
	}
}

void CEdit::EndSplitting( int xPos, int yPos, BOOL bCommit )
{
	VERIFY( ReleaseCapture() );
	ASSERT( GetCapture() != m_hWnd );

	UpdateSplitter( xPos, yPos );

	// erase splitter for good
	DrawMovingSplitters();

	if ( bCommit )
	{
		BOOL bHSplit = ( m_eMode == eHVSplitting || m_eMode == eHSplitting );
		BOOL bVSplit = ( m_eMode == eHVSplitting || m_eMode == eVSplitting );

		RECT rc;
		GetClientRect( m_hWnd, &rc );
		if ( ShowHScrollBar() )
		{
			int cxVScroll = GetSystemMetrics( SM_CXVSCROLL );
			rc.right -= cxVScroll;
			if ( m_xPosHSplitterBegin > 0 )
			{
				rc.left += cxVScroll;
			}
		}
		if ( ShowVScrollBar() )
		{
			int cyHScroll = GetSystemMetrics( SM_CYHSCROLL );
			rc.bottom -= cyHScroll;
			if ( m_yPosVSplitterBegin > 0 )
			{
				rc.top += cyHScroll;
			}
		}

		BOOL bBottom = FALSE;
		BOOL bRight = FALSE;

		// if close to an edge, treat the splitting as a splitter removal
		if ( bHSplit &&
		     ( m_xPosHSplitter - rc.left < CEditView::CXY_MIN_VIEW ) || 
		     ( rc.right - m_xPosHSplitter < CEditView::CXY_MIN_VIEW ) )
		{
			bRight = ( m_xPosHSplitter - rc.left >= CEditView::CXY_MIN_VIEW );
			m_xPosHSplitter = 0;
		}
		if ( bVSplit &&
		     ( m_yPosVSplitter - rc.top < CEditView::CXY_MIN_VIEW ) || 
		     ( rc.bottom - m_yPosVSplitter < CEditView::CXY_MIN_VIEW ) )
		{
			bBottom = ( m_yPosVSplitter - rc.top >= CEditView::CXY_MIN_VIEW );
			m_yPosVSplitter = 0;
		}

		BOOL bAddingHSplit = ( m_xPosHSplitterBegin == 0 && m_xPosHSplitter > 0 );
		BOOL bRemovingHSplit = ( m_xPosHSplitterBegin > 0 && m_xPosHSplitter == 0 );
		BOOL bAddingVSplit = ( m_yPosVSplitterBegin == 0 && m_yPosVSplitter > 0 );
		BOOL bRemovingVSplit = ( m_yPosVSplitterBegin > 0 && m_yPosVSplitter == 0 );

		BOOL bRecalcViews = bAddingHSplit || bRemovingHSplit || 
		                    bAddingVSplit || bRemovingVSplit ||
							( m_xPosHSplitterBegin != m_xPosHSplitter ) ||
							( m_yPosVSplitterBegin != m_yPosVSplitter );

		BOOL bSwitchViews = FALSE;

		CEditView *pView;
		if ( bHSplit )
		{
			if ( bAddingHSplit )
			{
				if ( m_yPosVSplitterBegin )
				{
					ASSERT( !m_Views[ 2 ] );
					ASSERT( !m_Views[ 3 ] );
					ASSERT( m_Views[ 0 ] );
					ASSERT( m_Views[ 1 ] );
					m_Views[ 3 ] = m_Views[ 1 ];
					m_Views[ 1 ] = m_Views[ 0 ];
					pView = m_Views[ 1 ];
					m_Views[ 0 ] = new CEditView( this, &m_Buffer, pView->GetLeftIndex(), pView->GetTopIndex() );
					pView = m_Views[ 3 ];
					m_Views[ 2 ] = new CEditView( this, &m_Buffer, pView->GetLeftIndex(), pView->GetTopIndex() );
					m_nViews = 4;
				}
				else
				{
					ASSERT( m_Views[ 0 ] );
					m_Views[ 1 ] = m_Views[ 0 ];
					pView = m_Views[ 1 ];
					m_Views[ 0 ] = new CEditView( this, &m_Buffer, pView->GetLeftIndex(), pView->GetTopIndex() );
					m_nViews = 2;
				}
				bSwitchViews = TRUE;
			}
			else if ( bRemovingHSplit )
			{
				if ( m_yPosVSplitterBegin )
				{
					if ( bRight )
					{
						delete m_Views[ 1 ];
						delete m_Views[ 3 ];
						m_Views[ 1 ] = m_Views[ 2 ];
					}
					else
					{
						delete m_Views[ 0 ];
						delete m_Views[ 2 ];
						m_Views[ 0 ] = m_Views[ 1 ];
						m_Views[ 1 ] = m_Views[ 3 ];
					}
					m_Views[ 2 ] = m_Views[ 3 ] = NULL;
					m_nViews = 2;
				}
				else
				{
					if ( bRight )
					{
						ASSERT( m_Views[ 1 ] );
						delete m_Views[ 1 ];
					}
					else
					{
						delete m_Views[ 0 ];
						m_Views[ 0 ] = m_Views[ 1 ];
					}
					m_Views[ 1 ] = NULL;
					m_nViews = 1;
				}
				m_xPosHSplitter = 0;
				bSwitchViews = TRUE;
			}
		}

		if ( bVSplit )
		{
			if ( bAddingVSplit )
			{
				if ( m_xPosHSplitterBegin )
				{
					ASSERT( !m_Views[ 2 ] );
					ASSERT( !m_Views[ 3 ] );
					ASSERT( m_Views[ 0 ] );
					ASSERT( m_Views[ 1 ] );
					m_Views[ 2 ] = m_Views[ 0 ];
					m_Views[ 3 ] = m_Views[ 1 ];
					pView = m_Views[ 2 ];
					m_Views[ 0 ] = new CEditView( this, &m_Buffer, pView->GetLeftIndex(), pView->GetTopIndex() );
					pView = m_Views[ 3 ];
					m_Views[ 1 ] = new CEditView( this, &m_Buffer, pView->GetLeftIndex(), pView->GetTopIndex() );
					m_nViews = 4;
				}
				else
				{
					ASSERT( m_Views[ 0 ] );
					m_Views[ 1 ] = m_Views[ 0 ];
					pView = m_Views[ 1 ];
					m_Views[ 0 ] = new CEditView( this, &m_Buffer, pView->GetLeftIndex(), pView->GetTopIndex() );
					m_nViews = 2;
				}
				bSwitchViews = TRUE;
			}
			else if ( bRemovingVSplit )
			{
				if ( m_xPosHSplitterBegin && !bRemovingHSplit )
				{
					if ( bBottom )
					{
						delete m_Views[ 2 ];
						delete m_Views[ 3 ];
					}
					else
					{
						delete m_Views[ 0 ];
						delete m_Views[ 1 ];
						m_Views[ 0 ] = m_Views[ 2 ];
						m_Views[ 1 ] = m_Views[ 3 ];
					}
					m_Views[ 2 ] = m_Views[ 3 ] = NULL;
					m_nViews = 2;
				}
				else
				{
					if ( bBottom )
					{
						ASSERT( m_Views[ 1 ] );
						delete m_Views[ 1 ];
					}
					else
					{
						delete m_Views[ 0 ];
						m_Views[ 0 ] = m_Views[ 1 ];
					}
					m_Views[ 1 ] = NULL;
					m_nViews = 1;
				}
				m_yPosVSplitter = 0;
				bSwitchViews = TRUE;
			}
		}

		if ( bRecalcViews )
		{
			RecalcViews();
		}

		if ( bSwitchViews )
		{
			CEditView *pView = m_Views[ 0 ];
			m_Selection.SetView( pView, TRUE );
			SetActiveView( pView );
		}
		else
			m_Selection.UpdateCaretPosition();
	}
	else
	{
		m_xPosHSplitter = m_xPosHSplitterBegin;
		m_yPosVSplitter = m_yPosVSplitterBegin;
	}

	SetMode( eIdle );

	Repaint( TRUE );
}

void CEdit::RecalcViews()
{
	// if splitting not allowed, make sure the splitters are removed
	if ( !m_bAllowHSplit && m_xPosHSplitter )
	{
		m_xPosDown = m_xPosHSplitter;
		BeginSplitting( eHSplitter );
		EndSplitting( 0, 0, TRUE );
	}
	if ( !m_bAllowVSplit && m_yPosVSplitter )
	{
		m_yPosDown = m_yPosVSplitter;
		BeginSplitting( eVSplitter );
		EndSplitting( 0, 0, TRUE );
	}

	ASSERT( IsWindow( m_hWnd ) );
	RECT rc;
	GetClientRect( m_hWnd, &rc );
	switch ( m_nViews )
	{
		case 1:
		{
			m_Views[ 0 ]->SetPosition( &rc, m_bAllowHSplit, m_bAllowVSplit );
			break;
		}
		case 2:
		{
			RECT rcView = rc;
			if ( m_xPosHSplitter )
			{
				rcView.right = m_xPosHSplitter;
				m_Views[ 0 ]->SetPosition( &rcView, FALSE, FALSE );
				rcView.left = rcView.right + CXY_SPLITTER;
				rcView.right = rc.right;
				m_Views[ 1 ]->SetPosition( &rcView, FALSE, m_bAllowVSplit );
			}
			else
			{
				rcView.bottom = m_yPosVSplitter;
				m_Views[ 0 ]->SetPosition( &rcView, FALSE, FALSE );
				rcView.top = rcView.bottom + CXY_SPLITTER;
				rcView.bottom = rc.bottom;
				m_Views[ 1 ]->SetPosition( &rcView, m_bAllowHSplit, FALSE );
			}
			break;
		}
		case 4:
		{
			RECT rcView = rc;
			rcView.bottom = m_yPosVSplitter;
			rcView.right = m_xPosHSplitter;
			m_Views[ 0 ]->SetPosition( &rcView, FALSE, FALSE );
			rcView.left = rcView.right + CXY_SPLITTER;
			rcView.right = rc.right;
			m_Views[ 1 ]->SetPosition( &rcView, FALSE, FALSE );

			rcView.top = rcView.bottom + CXY_SPLITTER;
			rcView.bottom = rc.bottom;
			rcView.left = rc.left;
			rcView.right = m_xPosHSplitter;
			m_Views[ 2 ]->SetPosition( &rcView, FALSE, FALSE );
			rcView.left = rcView.right + CXY_SPLITTER;
			rcView.right = rc.right;
			m_Views[ 3 ]->SetPosition( &rcView, FALSE, FALSE );
			break;
		}
		#if 0
		default:
		{
			ASSERT( FALSE );
			break;
		}
		#endif
	}

	// force repaint of everything.  Repaint() will not invalidate the splitters
	InvalidateRect( m_hWnd, NULL, FALSE );
}

void CEdit::DrawMovingSplitters() const
{
	ASSERT( IsWindow( m_hWnd ) );
	RECT rc;
	GetClientRect( m_hWnd, &rc );
	RECT rcSplitter;
	rcSplitter = rc;
	switch ( m_eMode )
	{
		case eHSplitting:
		{
			rcSplitter.left = m_xPosHSplitter;
			rcSplitter.right = rcSplitter.left + CXY_SPLITTER_DRAG;
			break;
		}
		case eVSplitting:
		{
			rcSplitter.top = m_yPosVSplitter;
			rcSplitter.bottom = rcSplitter.top + CXY_SPLITTER_DRAG;
			break;
		}
		case eHVSplitting:
		{
			rcSplitter.left = m_xPosHSplitter;
			rcSplitter.right = rcSplitter.left + CXY_SPLITTER_DRAG;
			DrawDragRect( rcSplitter );
			rcSplitter = rc;
			rcSplitter.top = m_yPosVSplitter;
			rcSplitter.bottom = rcSplitter.top + CXY_SPLITTER_DRAG;
			break;
		}
	}

	DrawDragRect( rcSplitter );
}

void DrawLine( HDC hDC, HPEN hPen, int x1, int y1, int x2, int y2 )
{
	if ( hPen )
	{
		SelectObject( hDC, hPen );
		// note: caller must restore DC to orig pen!
	}
	MoveToEx( hDC, x1, y1, NULL );
	LineTo( hDC, x2, y2 );
}

void CEdit::DrawStaticSplitters( HDC hDC ) const
{
	ASSERT( hDC );

	RECT rc;
	GetClientRect( m_hWnd, &rc );

	HBRUSH hbrBtnFace = ( HBRUSH ) CreateSolidBrush( GetSysColor( COLOR_BTNFACE ) );
	HPEN hpenBtnFace = CreatePen( PS_SOLID, 0, GetSysColor( COLOR_BTNFACE ) );
	HPEN hpenBtnHighlight = CreatePen( PS_SOLID, 0, GetSysColor( COLOR_BTNHIGHLIGHT ) );
	HPEN hpenBtnShadow = CreatePen( PS_SOLID, 0, GetSysColor( COLOR_BTNSHADOW ) );
	HPEN hpenWindowFrame = CreatePen( PS_SOLID, 0, GetSysColor( COLOR_WINDOWFRAME ) );

	BOOL bRenderHSplit = m_bAllowHSplit;
	BOOL bRenderVSplit = m_bAllowVSplit;
	// Special case rendering:  if rendering a two-pane split view, don't allow
	// the un-split splitter bar to be rendered if it is out of view.
	if ( m_nViews == 2 )
	{
		RECT rcTest;
		m_Views[ 1 ]->GetRect( &rcTest );
		RECT rcIntersect;
		BOOL bViewIsVisible = IntersectRect( &rcIntersect, &rc, &rcTest );
		if ( m_xPosHSplitter )
		{
			// don't show the vsplitter if not visible
			bRenderVSplit = bViewIsVisible;
		}
		else
		{
			// don't show the hsplitter if not visible
			bRenderHSplit = bViewIsVisible;
		}
	}

	BOOL bIsHSplit = ( m_xPosHSplitter > 0 );
	BOOL bIsVSplit = ( m_yPosVSplitter > 0 );

	int cyHScroll = GetSystemMetrics( SM_CYHSCROLL );
	int cxVScroll = GetSystemMetrics( SM_CXVSCROLL );
	///////////////////////////////////////////////////
	// Draw both splitters at the same time
	///////////////////////////////////////////////////
	HPEN hpenOld = ( HPEN ) SelectObject( hDC, hpenBtnFace );
	HPEN hbrOld = ( HPEN ) SelectObject( hDC, hbrBtnFace );

	int yTop = ( bIsHSplit ? -1 : 
	                      rc.bottom - cyHScroll - 1 );
	int xLeft = ( bIsVSplit ? -1 : 
	                      rc.right - cxVScroll - 1 );

	int x = m_xPosHSplitter + CXY_SPLITTER - 1;
	if ( bRenderHSplit ) DrawLine( hDC, hpenWindowFrame, x, rc.bottom, x, yTop );
	int y = m_yPosVSplitter + CXY_SPLITTER - 1;
	if ( bRenderVSplit ) DrawLine( hDC, hpenWindowFrame, rc.right, y, xLeft, y );

	x = m_xPosHSplitter;
	if ( bRenderHSplit ) DrawLine( hDC, hpenBtnFace, x, rc.bottom, x, yTop );
	y = m_yPosVSplitter;
	if ( bRenderVSplit ) DrawLine( hDC, hpenBtnFace, rc.right, y, xLeft, y );

	x = m_xPosHSplitter + CXY_SPLITTER - 2;
	if ( bRenderHSplit ) DrawLine( hDC, hpenBtnShadow, x, rc.bottom, x, yTop );
	y = m_yPosVSplitter + CXY_SPLITTER - 2;
	if ( bRenderVSplit ) DrawLine( hDC, hpenBtnShadow, rc.right, y, xLeft, y );

	x = m_xPosHSplitter + 1;
	if ( bRenderHSplit ) DrawLine( hDC, hpenBtnHighlight, x, rc.bottom, x, yTop );
	y = m_yPosVSplitter + 1;
	if ( bRenderVSplit ) DrawLine( hDC, hpenBtnHighlight, rc.right, y, xLeft, y );

	if ( bRenderHSplit ) 
	{
		y = bIsHSplit ? -1 : ( rc.bottom - cyHScroll );
		PatBlt( hDC, 
				m_xPosHSplitter + 2, 
				y,
				CXY_SPLITTER - 4,
				rc.bottom - y,
				PATCOPY );
	}

	if ( bRenderVSplit )
	{
		x = bIsVSplit ? -1 : ( rc.right - cxVScroll );
		PatBlt( hDC, 
				x, 
				m_yPosVSplitter + 2,
				rc.right - x,
				CXY_SPLITTER - 4,
				PATCOPY );
	}

	if ( bRenderHSplit && !bIsHSplit )
	{
		DrawLine( hDC, hpenWindowFrame, m_xPosHSplitter, rc.bottom - 1, m_xPosHSplitter + CXY_SPLITTER, rc.bottom - 1 );
		DrawLine( hDC, hpenBtnShadow, m_xPosHSplitter + 1, rc.bottom - 2, m_xPosHSplitter + CXY_SPLITTER - 1, rc.bottom - 2 ); 
		DrawLine( hDC, hpenBtnFace, m_xPosHSplitter + 1, rc.bottom - cyHScroll, m_xPosHSplitter + CXY_SPLITTER - 1, rc.bottom - cyHScroll ); 
		DrawLine( hDC, hpenBtnHighlight, m_xPosHSplitter + 1, rc.bottom - cyHScroll + 1, m_xPosHSplitter + CXY_SPLITTER - 2, rc.bottom - cyHScroll + 1 ); 
	}

	if ( bRenderVSplit && !bIsVSplit )
	{
		DrawLine( hDC, hpenWindowFrame, rc.right - 1, m_yPosVSplitter, rc.right - 1, m_yPosVSplitter + CXY_SPLITTER );
		DrawLine( hDC, hpenBtnShadow, rc.right - 2, m_yPosVSplitter + 1, rc.right - 2, m_yPosVSplitter + CXY_SPLITTER - 1 ); 
		DrawLine( hDC, hpenBtnFace, rc.right - cxVScroll, m_yPosVSplitter + 1, rc.right - cxVScroll, m_yPosVSplitter + CXY_SPLITTER - 1 ); 
		DrawLine( hDC, hpenBtnHighlight, rc.right - cxVScroll + 1, m_yPosVSplitter + 1, rc.right - cxVScroll + 1, m_yPosVSplitter + CXY_SPLITTER - 2 ); 
	}
	///////////////////////////////////////////////////
	// Restore the DC back to normal
	///////////////////////////////////////////////////
	SelectObject( hDC, hpenOld );
	SelectObject( hDC, hbrOld );

	DeleteObject( hbrBtnFace );
	DeleteObject( hpenBtnFace );
	DeleteObject( hpenBtnHighlight );
	DeleteObject( hpenBtnShadow );
	DeleteObject( hpenWindowFrame );
}