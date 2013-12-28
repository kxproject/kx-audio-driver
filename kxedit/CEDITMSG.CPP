#include "precomp.h"
#include "cedit.h"
#include "resource.h"
#ifdef _ACTIVEX
#include "editx.h"
#include "ceditx.h"
#endif

LRESULT CEdit::OnCreate( HWND hWnd, WPARAM /* wParam */, LPARAM lParam )
{
	CEdit *pEdit = new CEdit( hWnd );
	if ( pEdit )
	{
		// Associate pEdit with hWnd
		SetWindowLong( hWnd, 0, ( long )pEdit );
		pEdit->InitializeWindow( ( LPCREATESTRUCT ) lParam );

		// Keep track of all CodeMax windows
		ASSERT( g_nhWndCount == 0 && !g_phWnds || g_nhWndCount && g_phWnds );
		g_phWnds = g_phWnds ? ( HWND * ) realloc( g_phWnds, ( g_nhWndCount + 1 ) * sizeof( HWND ) ) :
		                      ( HWND * ) malloc( sizeof( HWND ) );
		g_phWnds[ g_nhWndCount++ ] = hWnd;
	
		pEdit->NotifyParent( CMN_CREATE );

		return 0;
	}

	// allocation & initialization failed.  Don't allow ctrl to be created
	return -1;
}

LRESULT CEdit::OnDestroy( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	NotifyParent( CMN_DESTROY );

	if ( m_hDlgRecord )
	{
		DestroyWindow( m_hDlgRecord );
		m_hDlgRecord = NULL;
	}

	// unregister the drop target
	CoLockObjectExternal( ( LPUNKNOWN ) &m_DropTarget, FALSE, TRUE );
	m_DropTarget.Release();
	VERIFY( S_OK == RevokeDragDrop( m_hWnd ) );

	if ( m_hdcPaint )
	{
		DeleteDC( m_hdcPaint );
		m_hdcPaint = NULL;
	}

	DeleteOffScreenBitmap();

	// find the hWnd in the master list so it can be removed
    int nhWnd;

	for (nhWnd  = 0; nhWnd < g_nhWndCount; nhWnd++ )
	{
		if ( g_phWnds[ nhWnd ] == m_hWnd )
		{
			break;
		}
	}

	ASSERT( nhWnd < g_nhWndCount );
	// fixup array indexes
	if ( g_nhWndCount == 1 )
	{
		free( g_phWnds );
		g_phWnds = NULL;
	}
	else
	{
		// shift array down
		if ( nhWnd < g_nhWndCount - 1 )
		{
			memmove( g_phWnds + nhWnd, 
					 g_phWnds + nhWnd + 1, 
					 ( g_nhWndCount - nhWnd - 1 ) * sizeof( HWND ) );
		}
	}
	g_nhWndCount--;

#ifdef _ACTIVEX
	if ( m_pEditX )
	{
		m_pEditX->SetEdit( NULL );
		m_pEditX = NULL;
	}
#endif

	return 0;
}

LRESULT CEdit::OnNcDestroy( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	// notify the parent window of lines being deleted so it can cleanup any per-line memory it allocated
	int nLineCount = m_Buffer.GetLineCount();
	if ( nLineCount )
	{
		OnBufferNotification( CBuffer::eLineRemoved, 0, 0, nLineCount );
	}
	
	// detach this object from the window
	SetWindowLong( m_hWnd, 0, NULL );
	
	// free up the memory for this object
	delete this;

	return 0;
}

LRESULT CEdit::OnEraseBkGnd( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	// don't do anything -- WM_PAINT will paint everything
	return 1;
}

LRESULT CEdit::OnPaint( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	PAINTSTRUCT ps;
	BeginPaint( m_hWnd, &ps );

	if ( m_nDelayPaint == 0 )
	{
		HDC hPaintDC = ( HDC ) ps.hdc;
		int cxPaint = ps.rcPaint.right - ps.rcPaint.left;
		int cyPaint = ps.rcPaint.bottom - ps.rcPaint.top;

		// create a temp DC and bitmap for fast painting when called repeatedly (like scrolling)
		if ( !m_hdcPaint )
		{
			m_hdcPaint = CreateCompatibleDC( hPaintDC );
		}

		CreateOffScreenBitmap( cxPaint, cyPaint );

		if ( m_hbmPaint || m_hdcPaint )
		{
			// remind myself to clean up the temp stuff
			if ( !m_bPaintTimerSet )
			{
				SetTimer( m_hWnd, TIMER_PAINT_CLEANUP, 3000, NULL );
				m_bPaintTimerSet = TRUE;
			}
		}
		
		if ( !m_hbmPaint || !m_hdcPaint )
		{
			// out of memory?
			EndPaint( m_hWnd, &ps );
			return 0;
		}

		#ifdef _DEBUG
		// verify DC and bitmap integrity
		BITMAP bmTest;
		ASSERT( GetObject( m_hbmPaint, sizeof( bmTest ), &bmTest ) );
		ASSERT( GetTextColor( m_hdcPaint ) != CLR_INVALID );
		#endif
		HBITMAP hbmOld = ( HBITMAP ) SelectObject( m_hdcPaint, m_hbmPaint );

		SetWindowOrgEx( m_hdcPaint, ps.rcPaint.left, ps.rcPaint.top, NULL );
	
		HBRUSH hbr = CreateSolidBrush( IsEnabled() ? GetWindowColor( TRUE ) : GetSysColor( COLOR_BTNFACE ) );
		FillRect( m_hdcPaint, &ps.rcPaint, hbr );
		DeleteObject( hbr );
	
		#ifdef _CODEMAX_BETA
		{
			RECT rc;
			m_Views[ 0 ]->GetViewRect( &rc );
			rc.left += m_bShowLeftMargin * CM_CXLEFTMARGIN;
			DrawBetaBackground( m_hdcPaint, rc, GetWindowColor( TRUE ) );
		}
		#endif
	
		RECT rcView;
		RECT rcClipBox;
	
		BOOL bEmptySel = m_Selection.IsEmpty();
		HWND hWndFocus = GetFocus();
		CEditView *pSelView = m_Selection.GetView();

		// draw each view that is in the update region
		for ( register int nView = 0; nView < m_nViews; nView++ )
		{
			CEditView *pView = m_Views[ nView ];
			ASSERT( pView != NULL );
	
			pView->GetRect( &rcView );
			if ( IntersectRect( &rcClipBox, &rcView, &ps.rcPaint ) )
			{
				pView->Draw( m_hdcPaint, &rcClipBox,
				             ( !bEmptySel && ( pSelView == pView ) && ( !m_bHideSel || ( hWndFocus == m_hWnd ) ) ) ? &m_Selection : NULL );
			}
		}

		// cleanup the DC to a pristine state to prevent leaks
		SelectObject( m_hdcPaint, GetStockObject( SYSTEM_FONT ) );
			
		// draw the splitters
		if ( m_bAllowHSplit || m_bAllowVSplit )
		{
			DrawStaticSplitters( m_hdcPaint );
		}

		BitBlt( hPaintDC, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top, m_hdcPaint, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY );
		SelectObject( m_hdcPaint, hbmOld );
	}
	else
	{
		UnionRect( &m_rcDelayPaint, &m_rcDelayPaint, &ps.rcPaint );
	}
	EndPaint( m_hWnd, &ps );

	return 0;
}

LRESULT CEdit::OnSize( WPARAM /* wParam */, LPARAM lParam )
{
	int cx = LOWORD( lParam );
	int cy = HIWORD( lParam );
	if ( cx > 0 && cy > 0 )
	{
		CreateOffScreenBitmap( LOWORD( lParam ), HIWORD( lParam ) );

		RecalcViews();
		m_Selection.UpdateCaretPosition();	// make sure caret is not over a scrollbar
	}
	return 0;
}

LRESULT CEdit::OnSetFont( WPARAM wParam, LPARAM lParam )
{
	HFONT hFont = ( HFONT ) wParam;
	if ( !hFont )
		return 0;

	LRESULT lResult = 0;

	
	LOGFONT lf, lfSys;
	VERIFY( GetObject( GetStockObject( SYSTEM_FIXED_FONT ), sizeof( lfSys ), &lfSys ) );
	if ( !GetObject( hFont, sizeof( lf ), &lf ) )
		return 0;

	HDC hDC = GetDC( m_hWnd );
	HFONT hFontOld = ( HFONT ) SelectObject( hDC, hFont );
	TEXTMETRIC tm;
	GetTextMetrics( hDC, &tm );
	SelectObject( hDC, hFontOld );
	ReleaseDC( m_hWnd, hDC );

	// only allow fixed pitch fonts to be selected.  This edit control
	// does not support variable pitch fonts.
	if ( ( tm.tmPitchAndFamily & TMPF_FIXED_PITCH ) == 0 )
	{
		SetDisplayFont( hFont, FALSE );
		OnFontChanged();
		#ifdef _ACTIVEX
		if ( m_pEditX )
			m_pEditX->OnFontChanged();
		#endif

		// default behavior will dirty window if necessary
		lResult = DefWindowProc( m_hWnd, WM_SETFONT, wParam, lParam );
	}

	return lResult;
}

LRESULT CEdit::OnGetFont( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return ( LRESULT ) GetFont( TRUE );
}

LRESULT CEdit::OnSetFocus( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	NotifyParent( NM_SETFOCUS );

	// show the cursor
	m_Selection.OnFocusChange( TRUE );

	// honor CMM_ENABLEHIDESEL style
	if ( m_bHideSel )
		DamageSelection( TRUE );

	return 0;
}

LRESULT CEdit::OnKillFocus( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	NotifyParent( NM_KILLFOCUS );

	// hide the cursor
	m_Selection.OnFocusChange( FALSE );

	// honor CMM_ENABLEHIDESEL style
	if ( m_bHideSel )
		DamageSelection( FALSE );

	// any pending hotkey is no longer valid
	ZeroMemory( &m_cmHotKeyPending, sizeof( m_cmHotKeyPending ) );
	
	// if in certain modes, cancel out of them
	switch ( m_eMode )
	{
		case eHSplitting:
		case eVSplitting:
		case eHVSplitting:
		case eDragSelecting:
		case eLineSelecting:
		case eDragAndDrop:
		{
			CancelMode();
			break;
		}
	}

	return 0;
}

#define COLUMN_SEL_KEYS_PRESSED ( ( GetKeyState( VK_MENU ) < 0 ) || ( GetKeyState( VK_CONTROL ) < 0 ) )

LRESULT CEdit::OnLButtonDown( WPARAM wParam, LPARAM lParam )
{
	if ( ShowHourGlass() )
	{
		return 0;
	}
	#ifdef _DEBUG
	if ( m_bSelfTest )
	{
		return 0;
	}
	#endif

	if ( NotifyParentOfMouseMsg( CMN_MOUSEDOWN, wParam | MK_LBUTTON, lParam ) ||
	     NotifyParent( NM_CLICK ) )
	{
		// parent handled the notification
		return 0;
	}
	else
	{
		int xPos = ( short ) LOWORD( lParam );
		int yPos = ( short ) HIWORD( lParam );

		m_xPosDown = xPos;
		m_yPosDown = yPos;

		int nView;
		HitTestCode eLocation = HitTest( xPos, yPos, nView );
		// if over a splitter, start the splitting process
		if ( eLocation == eHSplitter || eLocation == eVSplitter || eLocation == eHVSplitter )
		{
			BeginSplitting( eLocation );
		}
		else if ( ( eLocation == eEditSpace ) || ( eLocation == eLeftMargin ) )
		{
			if ( !IsRecordingMacro() )
			{
				SetCapture( m_hWnd );
				m_Selection.HideCaret();
				if ( m_bAllowDragDrop && PtInSelection( xPos, yPos, TRUE ) )
				{
					// Get ready for text drag and drop
					SetMode( eDragAndDrop );
				}
				else
				{
					// over text, set the selection
					CSelTransaction trans( this );
					ASSERT( nView != -1 );
					CEditView *pView = m_Views[ nView ];
					SetActiveView( pView );
					m_Selection.SetSelectionFromPoint( pView, xPos, yPos, ( GetKeyState( VK_SHIFT ) >= 0 ), TRUE );
					m_Selection.EnableColumnSel( COLUMN_SEL_KEYS_PRESSED );
					if ( eLocation == eLeftMargin )
					{
						// select this line now
						m_Selection.SetSelectionFromPoint( pView, xPos, yPos, FALSE, TRUE );
					}

					if ( GetFocus() != m_hWnd )
					{
						SetFocus( m_hWnd );
					}
					
					SetTimer( m_hWnd, TIMER_DRAG_SELECT, 100, NULL );
					SetMode( ( eLocation == eEditSpace ) ? eDragSelecting : eLineSelecting );
				}
			}
		}
	}

	return 0;
}

LRESULT CEdit::OnLButtonUp( WPARAM wParam, LPARAM lParam )
{
	if ( ShowHourGlass() )
	{
		return 0;
	}
	else
	{
		#ifdef _DEBUG
		if ( m_bSelfTest )
		{
			return 0;
		}
		#endif

		if ( NotifyParentOfMouseMsg( CMN_MOUSEUP, wParam | MK_LBUTTON, lParam ) )
		{
			// parent handled the notification
			return 0;
		}

		int xPos = ( short ) LOWORD( lParam );
		int yPos = ( short ) HIWORD( lParam );

		if ( IsSplitting() )
		{
			CSelTransaction trans( this );
			// split the windows now
			EndSplitting( xPos, yPos, TRUE );
		}
		else if ( IsSelecting() || IsLineSelecting() )
		{
			EndSelecting();
		}
		else if ( IsDragAndDropping() )
		{
			// user never started drag and drop -- set the selection instead
			VERIFY( ReleaseCapture() );
			ASSERT( GetCapture() != m_hWnd );
			if ( GetFocus() != m_hWnd )
			{
				SetFocus( m_hWnd );
			}

			int nView;
			if ( HitTest( xPos, yPos, nView ) == eEditSpace )
			{
				// over text, set the selection
				CSelTransaction trans( this );
				ASSERT( nView != -1 );
				CEditView *pView = m_Views[ nView ];
				SetActiveView( pView );
				m_Selection.SetSelectionFromPoint( pView, xPos, yPos, TRUE, TRUE );
			}
			SetMode( eIdle );
		}
		m_Selection.ShowCaret();
	}

	return 0;
}

// IntelliMouse support
LRESULT CEdit::OnMButtonDown( WPARAM wParam, LPARAM lParam )
{
	if ( ShowHourGlass() )
	{
		return 0;
	}
	else
	{
		#ifdef _DEBUG
		if ( m_bSelfTest )
		{
			return 0;
		}
		#endif

		if ( NotifyParentOfMouseMsg( CMN_MOUSEDOWN, wParam | MK_MBUTTON, lParam ) )
		{
			// parent handled the notification
			return 0;
		}
		// the MButton messages do not set the focus
		if ( GetFocus() != m_hWnd )
		{
			SetFocus( m_hWnd );
		}

		int xPos = ( short ) LOWORD( lParam );
		int yPos = ( short ) HIWORD( lParam );
		int nView;
		HitTest( xPos, yPos, nView );
		if ( nView == -1 )
		{
			// not over any particular view -- just scroll the first view
			nView = 0;
		}

		CSelTransaction trans( this );
		ASSERT( nView != -1 );
		CEditView *pView = m_Views[ nView ];
		SetActiveView( pView );
		m_Selection.SetSelectionFromPoint( pView, xPos, yPos, TRUE, TRUE );
		m_Selection.Extend( CSelection::eOutward, CSelection::eWord, FALSE, TRUE, FALSE );

		return 0;
	}

	return DefWindowProc( m_hWnd, WM_MBUTTONDOWN, wParam, lParam );
}

LRESULT CEdit::OnMButtonUp( WPARAM wParam, LPARAM lParam )
{
	if ( ShowHourGlass() )
	{
		return 0;
	}
	else if ( NotifyParentOfMouseMsg( CMN_MOUSEUP, wParam | MK_MBUTTON, lParam ) )
	{
		// parent handled the notification
		return 0;
	}
	else
	{
		return DefWindowProc( m_hWnd, WM_MBUTTONUP, wParam, lParam );
	}
}

// IntelliMouse support
LRESULT CEdit::OnMButtonDblClk( WPARAM wParam, LPARAM lParam )
{
	if ( ShowHourGlass() )
	{
		return 0;
	}
	else
	{
		#ifdef _DEBUG
		if ( m_bSelfTest )
		{
			return 0;
		}
		#endif

		int xPos = ( short ) LOWORD( lParam );
		int yPos = ( short ) HIWORD( lParam );
		int nView;
		if ( HitTest( xPos, yPos, nView ) == eEditSpace )
		{
			// over text, set the selection
			CSelTransaction trans( this );
			ASSERT( nView != -1 );
			CEditView *pView = m_Views[ nView ];
			SetActiveView( pView );
			m_Selection.SetSelectionFromPoint( pView, xPos, yPos, TRUE, TRUE );
			m_Selection.Move( CSelection::eLeft, CSelection::eSmartAll, FALSE );
			m_Selection.Extend( CSelection::eRight, CSelection::eSmartAll, FALSE, TRUE, FALSE );
			return 0;
		}
	}

	return DefWindowProc( m_hWnd, WM_MBUTTONDBLCLK, wParam, lParam );
}

LRESULT CEdit::OnMouseMove( WPARAM wParam, LPARAM lParam )
{
	int xPos = ( short ) LOWORD( lParam );
	int yPos = ( short ) HIWORD( lParam );

	if ( ShowHourGlass() )
	{
		return 0;
	}
	else
	{
		#ifdef _DEBUG
		if ( m_bSelfTest )
		{
			DoCursorSelfTest( xPos, yPos );
			return 0;
		}
		#endif

		if ( NotifyParentOfMouseMsg( CMN_MOUSEMOVE, wParam, lParam ) )
		{
			// parent handled the notification
			return 0;
		}
		else if ( IsSplitting() )
		{
			UpdateSplitter( xPos, yPos );
		}
		else if ( IsSelecting() || IsLineSelecting() )
		{
			ASSERT( m_pActiveView );
			CSelTransaction trans( this );


			// don't allow line selecting unless user started in the left margin			
			RECT rcView;
			m_pActiveView->GetViewRect( &rcView );
			BOOL bStartedInLeftMargin = ( m_xPosDown >= rcView.left && m_xPosDown < ( rcView.left + m_pActiveView->GetLeftMargin( TRUE ) ) );

			m_Selection.SetSelectionFromPoint( m_pActiveView, xPos, yPos, m_Buffer.IsEmpty(), bStartedInLeftMargin );
			// if dragged half a char width or height, autoscroll, otherwise, the user may just be clicking (don't want to scroll!)
			if ( abs( xPos - m_xPosDown ) > m_pActiveView->GetCharWidth() / 2 )
			{
				m_pActiveView->ScrollIfNearBorder( xPos, yPos, FALSE );
			}
		}

		else if ( IsDragAndDropping() )
		{
			VERIFY( ReleaseCapture() );
			ASSERT( GetCapture() != m_hWnd );
			DoDragDrop();
		}
	}

	return 0;
}

LRESULT CEdit::OnLButtonDblClk( WPARAM /* wParam */, LPARAM lParam )
{
	if ( ShowHourGlass() )
	{
		return 0;
	}

	#ifdef _DEBUG
	if ( m_bSelfTest )
	{
		return 0;
	}
	#endif

	if ( NotifyParent( NM_DBLCLK ) )
	{
		// parent handled the notification
		return 0;
	}
	else
	{
		int xPos = ( short ) LOWORD( lParam );
		int yPos = ( short ) HIWORD( lParam );

		m_xPosDown = xPos;
		m_yPosDown = yPos;

		int nView;
		HitTestCode eLocation = HitTest( xPos, yPos, nView );
		// if over a splitter, remove the splitter(s)
		if ( eLocation == eHSplitter || eLocation == eVSplitter || eLocation == eHVSplitter )
		{
			CSelTransaction trans( this );
			BeginSplitting( eLocation );
			RECT rc;
			GetClientRect( m_hWnd, &rc );
			if ( ( eLocation == eHSplitter && !m_xPosHSplitter ) ||
				 ( eLocation == eVSplitter && !m_yPosVSplitter ) )
			{
				// split
				EndSplitting( ( rc.right + rc.left ) / 2, ( rc.top + rc.bottom ) / 2, TRUE );
			}
			else
			{
				// un-split
				m_xPosDown = m_xPosHSplitter;
				m_yPosDown = m_yPosVSplitter;
				EndSplitting( 0, 0, TRUE );
			}
		}
		else if ( eLocation == eEditSpace )
		{
			CSelTransaction trans( this );
			// select the word under the cursor, if any
			int nCol, nRow;
			RECT rcChar;
			m_pActiveView->GetCharPosFromPoint( xPos, yPos, nCol, nRow, &rcChar );
			nCol = m_Buffer.ConvertViewColToBufferCol( nRow, nCol );

			if ( !IsRectEmpty( &rcChar ) && ( xPos > ( rcChar.left + rcChar.right ) / 2 ) )
			{
				nCol += m_Buffer.GetCharSize( nRow, nCol );
			}
			m_Selection.SetEmptySelection( nCol, nRow );
			m_Selection.Extend( CSelection::eOutward, CSelection::eWord, TRUE, TRUE, FALSE );
		}
		else if ( eLocation == eLeftMargin )
		{
			CSelTransaction trans( this );
			// deselect the line that was selected
			m_Selection.SetSelectionFromPoint( m_pActiveView, xPos, yPos, TRUE, TRUE );
		}
	}

	return 0;
}

LRESULT CEdit::OnRButtonDown( WPARAM wParam, LPARAM lParam )
{
	if ( ShowHourGlass() )
	{
		return 0;
	}
	else
	{
		if ( NotifyParentOfMouseMsg( CMN_MOUSEDOWN, wParam | MK_RBUTTON, lParam ) )
		{
			// parent handled the notification
			return 0;
		}
		// the RButton messages do not set the focus
		if ( GetFocus() != m_hWnd )
		{
			SetFocus( m_hWnd );
		}
		int xPos = ( short ) LOWORD( lParam );
		int yPos = ( short ) HIWORD( lParam );

		int nView;
		HitTestCode eLocation = HitTest( xPos, yPos, nView );
		// Set the selection in preparation for a right click
		if ( ( eLocation == eEditSpace ) || ( eLocation == eLeftMargin ) )
		{
			if ( !IsRecordingMacro() && !PtInSelection( xPos, yPos, TRUE ) )
			{
				CSelTransaction trans( this );
				// over text, set the selection
				ASSERT( nView != -1 );
				CEditView *pView = m_Views[ nView ];
				SetActiveView( pView );
				m_Selection.SetSelectionFromPoint( pView, xPos, yPos, TRUE, TRUE );
			}
		}

		return DefWindowProc( m_hWnd, WM_RBUTTONDOWN, wParam, lParam );
	}
}

LRESULT CEdit::OnRButtonUp( WPARAM wParam, LPARAM lParam )
{
	if ( ShowHourGlass() )
	{
		return 0;
	}
	else if ( NotifyParentOfMouseMsg( CMN_MOUSEUP, wParam | MK_RBUTTON, lParam ) )
	{
		// parent handled the notification
		return 0;
	}
	else
	{
		return DefWindowProc( m_hWnd, WM_RBUTTONUP, wParam, lParam );
	}
}

LRESULT CEdit::OnRButtonDblClk( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	if ( ShowHourGlass() )
	{
		return 0;
	}

	#ifdef _DEBUG
	if ( m_bSelfTest )
	{
		return 0;
	}
	#endif

	NotifyParent( NM_RDBLCLK );

	return 0;
}

LRESULT CEdit::OnHScroll( WPARAM wParam, LPARAM lParam )
{
	CSelTransaction trans( this );
	HWND hWndScrollBar = ( HWND ) lParam;
	int nSBCode = ( int ) LOWORD( wParam );
	int nPos = ( int ) HIWORD( wParam );

    int i = 0;

	for ( i=0; i < m_nViews; i++ )
	{
		if ( m_Views[ i ]->OnHScroll( hWndScrollBar, nSBCode, nPos ) )
		{
			// this view claimed ownership of hWndScrollBar and handled the message
			m_Selection.UpdateCaretPosition();
			break;
		}
	}

	return i < m_nViews;
}

LRESULT CEdit::OnVScroll( WPARAM wParam, LPARAM lParam )
{
	CSelTransaction trans( this );
	HWND hWndScrollBar = ( HWND ) lParam;
	int nSBCode = ( int ) LOWORD( wParam );

	//////////////////////////////////////////
	// Note: Because the total number of lines can 
	// exceed 64K, we must use the 32-bit version
	// of the scrollbar APIs to get the real position.
	SCROLLINFO si;
	ZeroMemory(&si, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_TRACKPOS;
	VERIFY( GetScrollInfo( hWndScrollBar, SB_CTL, &si ) );
	int nPos = si.nTrackPos;

    int i;
	for ( i = 0; i < m_nViews; i++ )
	{
		if ( m_Views[ i ]->OnVScroll( hWndScrollBar, nSBCode, nPos ) )
		{
			// this view claimed ownership of hWndScrollBar and handled the message
			m_Selection.UpdateCaretPosition();
			break;
		}
	}

	return i < m_nViews;
}

LRESULT CEdit::OnMouseWheel( WPARAM wParam, LPARAM lParam )
{
	int xPos = ( short ) LOWORD( lParam );
	int yPos = ( short ) HIWORD( lParam );

	// Help for WM_MOUSEWHEEL claims xPos, yPos is in client coords -- not TRUE.  They
	// are in window coords.
	POINT pt = { xPos, yPos };
	ScreenToClient( m_hWnd, &pt );

	int nView;
	if ( HitTest( pt.x, pt.y, nView ) == eEditSpace )
	{
		int cyDelta = ( short ) HIWORD( wParam );
		m_Views[ nView ]->Scroll( 0, ( cyDelta > 0 ) ? -3 : 3 );
		m_Selection.UpdateCaretPosition();
	}
	
	return 0;
}

LRESULT CEdit::OnKeyDown( WPARAM wParam, LPARAM lParam )
{
	if ( ShowHourGlass() )
	{
		return 0;
	}

	#ifdef _DEBUG
	if ( m_bSelfTest )
	{
		return 0;
	}
	#endif

	if ( NotifyParentOfKeyMsg( CMN_KEYDOWN, wParam ) )
	{
		// parent handled the notification
		return 0;
	}

	BOOL bProcessed = FALSE;

	m_bEatKeyMsg = TRUE;
	m_bSkipTranslate = !TranslateHotKey( wParam );
	if ( !m_bSkipTranslate )
	{
		bProcessed = TRUE;
		m_bEatKeyMsg = TRUE;
	}
	else
	{
		m_bEatKeyMsg = FALSE;
		switch ( ( int ) wParam )
		{
			case VK_RETURN:
				{
					if ( !NotifyParent( NM_RETURN ) )
					{
						OnChar( _T('\n'), 0 );
						m_bEatKeyMsg = TRUE;
					}
					bProcessed = TRUE;
					break;
				}
			case VK_ESCAPE:
				{
					m_nRepeatCount = 1;
					ExecuteCommand( CMD_CLEARSELECTION, 0, FALSE, FALSE );
					CancelMode();
					break;
				}
		}
	}
	if ( !bProcessed )
		DefWindowProc( m_hWnd, WM_KEYDOWN, wParam, lParam );
	return bProcessed ? 0 : 1;
}

LRESULT CEdit::OnKeyUp( WPARAM wParam, LPARAM lParam )
{
	if ( ShowHourGlass() )
	{
		return 0;
	}

	#ifdef _DEBUG
	if ( m_bSelfTest )
	{
		return 0;
	}
	#endif

	if ( NotifyParentOfKeyMsg( CMN_KEYUP, wParam ) )
	{
		// parent handled the notification
		return 0;
	}

	// don't process the context menu key as a regular keyup --
	// it will automatically be translated into a WM_CONTEXTMENU event.
	BOOL bProcessed = ( wParam == VK_APPS );

	if ( !bProcessed )
		DefWindowProc( m_hWnd, WM_KEYUP, wParam, lParam );

	return bProcessed ? 0 : 1;
}

LRESULT CEdit::OnSysKeyDown( WPARAM wParam, LPARAM lParam )
{
	#ifdef _DEBUG
	if ( m_bSelfTest )
	{
		return 0;
	}
	#endif

	if ( NotifyParentOfKeyMsg( CMN_KEYDOWN, wParam ) )
	{
		// parent handled the notification
		return 0;
	}

	m_bEatKeyMsg = FALSE;
	m_bSkipTranslate = FALSE;
	BOOL bProcessed = FALSE;
	if ( ( GetMode() != eDragSelecting ) && !ShowHourGlass() )
	{
		// cancel any mode currently in progress (e.g. splitting, drag-selecting)
		CancelMode();

		m_bEatKeyMsg = TRUE;
		m_bSkipTranslate = !TranslateHotKey( wParam );
		if ( m_bSkipTranslate )
		{
			m_bEatKeyMsg = FALSE;
		}
		else
		{
			bProcessed = TRUE;
			return 0;
		}
	}
	if ( !bProcessed )
		DefWindowProc( m_hWnd, WM_SYSKEYDOWN, wParam, lParam );
	return bProcessed ? 0 : 1;
}

LRESULT CEdit::OnSysKeyUp( WPARAM wParam, LPARAM lParam )
{
	if ( NotifyParentOfKeyMsg( CMN_KEYUP, wParam ) )
	{
		// parent handled the notification
		return 0;
	}

	return DefWindowProc( m_hWnd, WM_SYSKEYUP, wParam, lParam );
}

LRESULT CEdit::OnSysChar( WPARAM wParam, LPARAM lParam )
{
	#ifdef _DEBUG
	if ( m_bSelfTest )
	{
		return 0;
	}
	#endif

	if ( ShowHourGlass() )
	{
		return 0;
	}
	
	if ( NotifyParentOfKeyMsg( CMN_KEYPRESS, wParam ) )
	{
		// parent handled the notification
		return 0;
	}

	if ( m_bEatKeyMsg || ( !m_bSkipTranslate && TranslateHotKey( wParam ) ) )
	{
		return 0;
	}
	else
	{
		return DefWindowProc( m_hWnd, WM_SYSCHAR, wParam, lParam );
	}
}

LRESULT CEdit::OnChar( WPARAM wParam, LPARAM lParam )
{
	if ( ShowHourGlass() )
	{
		return 0;
	}

	#ifdef _DEBUG
	if ( m_bSelfTest )
	{
		return 0;
	}
	#endif

	if ( NotifyParentOfKeyMsg( CMN_KEYPRESS, wParam ) )
	{
		// parent handled the notification
		return 0;
	}

	if ( !m_bEatKeyMsg && VIRTKEY_IS_ALPHA( wParam ) )
	{
		TranslateChar( ( TCHAR ) wParam );
		return 0;
	}

	return DefWindowProc( m_hWnd, WM_CHAR, wParam, lParam );
}

LRESULT CEdit::OnSetCursor( WPARAM wParam, LPARAM lParam )
{
	if ( LOWORD( lParam ) == HTCLIENT )
	{
		POINT pt;
		VERIFY( GetCursorPos( &pt ) );
		ScreenToClient( m_hWnd, &pt );
		int nView;
		HitTestCode eCode = HitTest( pt.x, pt.y, nView );
		HCURSOR hCursor = NULL;
		switch ( eCode )
		{
			case eEditSpace:
			{
				if ( IsRecordingMacro() )
				{
					hCursor = g_hcurRecord;
				}
				else if ( PtInSelection( pt.x, pt.y, TRUE ) )
				{
					hCursor = LoadCursor( NULL, IDC_ARROW );
				}
				else
				{
					hCursor = LoadCursor( NULL, IDC_IBEAM );
				}
				break;
			}
			case eHSplitter:
			{
				hCursor = g_hcurHSplit;
				break;
			}
			case eVSplitter:
			{
				hCursor = g_hcurVSplit;
				break;
			}
			case eHVSplitter:
			{
				hCursor = g_hcurHVSplit;
				break;
			}
			case eLeftMargin:
			{
				hCursor = g_hcurLeftMargin;
				break;
			}
			default:
			{
				return DefWindowProc( m_hWnd, WM_SETCURSOR, wParam, lParam );
			}
		}

		// let the child windows (scrollbars) first get a crack at the cursor before
		// overriding with hourglass
		if ( ShowHourGlass() )
		{
			hCursor = LoadCursor( NULL, IDC_WAIT );
		}

		ASSERT( hCursor );
		SetCursor( hCursor );
		return 0;
	}
	else
	{
		return DefWindowProc( m_hWnd, WM_SETCURSOR, wParam, lParam );
	}
}

LRESULT CEdit::OnTimer( WPARAM wParam, LPARAM /* lParam */ )
{
	BOOL bProcessed = FALSE;

	switch ( wParam )
	{
		case TIMER_DRAG_SELECT:
		{
			POINT pt;
			VERIFY( GetCursorPos( &pt ) );
			ScreenToClient( m_hWnd, &pt );
			OnMouseMove( MK_LBUTTON, MAKELPARAM( pt.x, pt.y ) );
			bProcessed = TRUE;
			break;
		}
		case TIMER_PAINT_CLEANUP:
		{
			KillTimer( m_hWnd, TIMER_PAINT_CLEANUP );  // one-shot timer
			m_bPaintTimerSet = FALSE;
			if	( m_hdcPaint )
			{
				DeleteDC( m_hdcPaint );
				m_hdcPaint = NULL;
			}
			DeleteOffScreenBitmap();

			break;
		}
	}

	return bProcessed ? 0 : 1;
}

LRESULT CEdit::OnContextMenu( WPARAM /* wParam */, LPARAM lParam )
{
	if ( ShowHourGlass() )
	{
		return 0;
	}

	if ( NotifyParent( NM_RCLICK ) )
	{
		// parent handled the notification
		return 0;
	}

	int x = LOWORD( lParam ); int y = HIWORD( lParam );
	if ( x == ( WORD ) -1 || y == ( WORD ) -1 )
		{
		// via VK_APPS key
		POINT pt = { 10, 10 };
		ClientToScreen( m_hWnd, &pt );
		x = pt.x;
		y = pt.y;
		}

	ShowRightClickMenu( x, y );
	
	return 0;	
}

LRESULT CEdit::OnEnable( WPARAM wParam, LPARAM lParam )
{
	if ( wParam )
	{
		m_dwStyle &= ~WS_DISABLED;
	}
	else
	{
		m_dwStyle |= WS_DISABLED;
		CancelMode();
		// if recording a macro, bail now
		if ( IsRecordingMacro() )
		{
			m_bAbortMacro = TRUE;
			RecordMacro();
		}
		m_Selection.MakeEmpty();
		
	}

	Repaint( FALSE );
	return DefWindowProc( m_hWnd, WM_ENABLE, wParam, lParam );
}

LRESULT CEdit::OnGetDlgCode( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return ( DLGC_WANTALLKEYS | DLGC_WANTARROWS | DLGC_WANTTAB | DLGC_HASSETSEL | DLGC_WANTCHARS );
}

LRESULT CEdit::OnSysColorChange( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	// resize the views to get the scrollbars to the proper size.
	RecalcViews();
	return 0;
}


LRESULT CEdit::OnEnableColorSyntax( WPARAM wParam, LPARAM /* lParam */ )
{
	BOOL bNew = wParam == 1;
	if ( bNew != m_bColorSyntax )
	{
		m_bColorSyntax = bNew;
		Repaint( FALSE );
	}

	return CME_SUCCESS;
}

LRESULT CEdit::OnIsColorSyntaxEnabled( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return m_bColorSyntax;
}

LRESULT CEdit::OnOpenFile( WPARAM /* wParam */, LPARAM lParam )
{
	// lParam == string ptr to file name
	m_Buffer.SetModified( FALSE );
	BOOL bUndo = m_Buffer.EnableUndo( FALSE );
	m_Buffer.BeginEdit();
	m_Buffer.AllowParse( FALSE );
	m_Buffer.RemoveLines( 0, m_Buffer.GetLineCount() );

	LRESULT lResult = OnInsertFile( 0, lParam );

	m_Buffer.AllowParse( TRUE );
	m_Buffer.EnableUndo( bUndo );
	m_Buffer.SetModified( FALSE );
	m_Buffer.EndEdit();

	return lResult;
}

LRESULT CEdit::OnInsertFile( WPARAM wParam, LPARAM lParam )
{
	// lParam == Ansi string ptr to file name
	// wParam == CM_POSITION insert pos, NULL == start of file
	ASSERT( lParam );
	LRESULT lResult = CME_BADARGUMENT;
	CSelTransaction save( this );

	if ( lParam )
	{
		lResult = CME_FAILURE;

		LPCTSTR pszFile = ( LPCTSTR ) lParam;
		// create a memory mapped file so it can be treated as regular old text

		HANDLE hFile = CreateFile( pszFile, 
								   GENERIC_READ,
								   FILE_SHARE_READ | FILE_SHARE_WRITE,
								   NULL,
								   OPEN_EXISTING,
								   FILE_ATTRIBUTE_NORMAL,
								   NULL );

		if ( hFile != INVALID_HANDLE_VALUE )
		{
			// zero-length files are ok -- but CreateFileMapping() cannot handle them.
			if ( GetFileSize( hFile, NULL ) == 0 )
			{
				lResult = CME_SUCCESS;
			}
			else
			{
				HANDLE hFileMapping = CreateFileMapping( hFile,
														 NULL,
														 PAGE_READONLY,
														 0,
														 0,
														 NULL );
				if ( hFileMapping )
				{
					LPCSTR pszText = ( LPCSTR ) MapViewOfFile( hFileMapping,
															   FILE_MAP_READ,
															   0,
															   0,
															   0 );
					if ( pszText )
					{
						int cbFile = GetFileSize( hFile, NULL );
						// presize the buffer to prevent too many reallocs
						m_Buffer.EnsureLineArraySize( cbFile / 30 );	// figure 30 characters per line (just a guess)
						#ifdef _UNICODE
						// must convert ASCII file contents to Unicode
						LPTSTR pszWText = new TCHAR[ cbFile + 1 ];
						if ( pszWText )
						{
							MultiByteToWideChar( CP_ACP, 0, pszText, cbFile, pszWText, cbFile );
							pszWText[ cbFile ] = _T('\0');
							GuessCRLFStateFromText( pszWText );
							lResult = OnInsertText( wParam, ( LPARAM ) pszWText );
							delete pszWText;
						}
						else
						{
							lResult = CME_FAILURE;
						}
						#else
						/////////////////////////////////////////////////////////////////////
						// we cannot trust the MapViewOfFile() function
						// to properly null-terminate the file image string if the file
						// was previously mapped into memory, and must make a copy of 
						// the file contents in memory and terminate the copy.
						/////////////////////////////////////////////////////////////////////
						LPTSTR pszImage = new TCHAR[ cbFile + 1 ];
						if ( pszImage )
						{
							strncpy( pszImage, pszText, cbFile );
							pszImage[ cbFile ] = '\0';
							GuessCRLFStateFromText( pszImage );
							lResult = OnInsertText( wParam, ( LPARAM ) pszImage );
							delete pszImage;
						}
						#endif
						// trim any buffer space that was overly-preallocated
						m_Buffer.FlushExtraLines();
						VERIFY( UnmapViewOfFile( pszText ) );
					}
					VERIFY( CloseHandle( hFileMapping ) );
				}
			}
			VERIFY( CloseHandle( hFile ) );
		}
	}

	return lResult;
}


LRESULT CEdit::OnSetText( WPARAM /* wParam */, LPARAM lParam )
{
	m_Buffer.SetModified( FALSE );
	BOOL bUndo = m_Buffer.EnableUndo( FALSE );
	m_Buffer.BeginEdit();
	m_Buffer.AllowParse( FALSE );
	int nLineCount = m_Buffer.GetLineCount();
	if ( nLineCount )
	{
		GoToLine( 0, FALSE );
		m_Buffer.RemoveLines( 0, nLineCount );
	}

	LRESULT lResult = OnInsertText( 0, lParam );

	m_Buffer.AllowParse( TRUE );
	m_Buffer.EnableUndo( bUndo );
	m_Buffer.SetModified( FALSE );
	m_Buffer.EndEdit();

	return lResult;
}

LRESULT CEdit::OnInsertText( WPARAM wParam, LPARAM lParam )
{
	// lParam == Ansi string ptr
	// wParam == CM_POSITION insert pos, NULL == start of file
	ASSERT( lParam );
	LRESULT lResult = CME_BADARGUMENT;

	if ( lParam )
	{
		CEditTransaction trans( this );
		CSelTransaction sel( this );
		CSaveSelection save( this );
		CM_POSITION pos = { 0, 0 };
		CM_POSITION *pPos = wParam ? ( CM_POSITION * ) wParam  : &pos;

		// do the insertion
		if ( LineIsValid( pPos->nLine, TRUE ) )
		{
			m_Selection.SetEmptySelection( pPos->nCol, pPos->nLine );
			ReplaceSelection( ( LPCTSTR ) lParam, FALSE, FALSE );
			lResult = CME_SUCCESS;
		}
	}

	return lResult;
}

LRESULT CEdit::OnReplaceText( WPARAM wParam, LPARAM lParam, BOOL bSelectWhenDone )
{
	// lParam == Ansi string ptr
	// wParam == CM_RANGE insert range, NULL == use current selection
	ASSERT( lParam );
	LRESULT lResult = CME_BADARGUMENT;

	if ( lParam && ( !wParam || RangeIsValid( ( CM_RANGE * ) wParam ) ) )
	{
		CEditTransaction trans( this );
		CSelTransaction sel( this );
		int nEndCol, nEndRow;

		if ( wParam )
		{
			CM_RANGE *pRange = ( CM_RANGE * ) wParam;
			nEndCol = pRange->posEnd.nCol;
			nEndRow = pRange->posEnd.nLine;
			m_Selection.SetExtendedSelection( pRange->posStart.nCol, pRange->posStart.nLine, nEndCol, nEndRow );
		}

		// do the replacement
		ReplaceSelection( ( LPCTSTR ) lParam, FALSE, FALSE, bSelectWhenDone );
		if ( !bSelectWhenDone )
			m_Selection.MakeEmpty( TRUE );

		lResult = CME_SUCCESS;
	}

	return lResult;
}

LRESULT CEdit::OnSetLanguage( WPARAM /*wParam*/, LPARAM lParam )
{
	LPCTSTR pszName = ( LPCTSTR ) lParam;
	CME_CODE cmResult = CME_BADARGUMENT;

	if ( pszName && *pszName )
	{
		// find the language among those registered
		for ( int i = 0; i < g_nLanguageCount; i++ )
		{
			CLanguage *pLang = g_pLanguages[ i ];
			if ( _tcsicmp( pLang->m_szName, pszName ) == 0 )
			{
				if ( m_Buffer.SetLanguage( pLang->m_pLang ) )
				{
					_tcsncpy( m_szLang, pLang->m_szName, CM_MAX_LANGUAGE_NAME );
					m_szLang[ CM_MAX_LANGUAGE_NAME ] = '\0';
					cmResult = CME_SUCCESS;
					break;
				}
				else
				{
					cmResult = CME_FAILURE;
					break;
				}
			}
		}
	}
	else
	{
		m_Buffer.SetLanguage( NULL );
		*m_szLang = '\0';
		cmResult = CME_SUCCESS;
	}

	Repaint( FALSE );

	return cmResult;
}

LRESULT CEdit::OnGetLanguage( WPARAM /*wParam*/, LPARAM lParam )
{
	if ( lParam )
	{
		_tcscpy( ( LPTSTR ) lParam, m_szLang );
		return CME_SUCCESS;
	}

	return CME_BADARGUMENT;
}

LRESULT CEdit::OnSetColors( WPARAM /*wParam*/, LPARAM lParam )
{
	CM_COLORS *pColors = ( CM_COLORS * ) lParam;
	if ( !pColors || 
		 ( pColors->crText & 0xff000000 ) || 
		 ( pColors->crNumber & 0xff000000 ) || 
		 ( pColors->crKeyword & 0xff000000 ) || 
		 ( pColors->crComment & 0xff000000 ) || 
		 ( pColors->crScopeKeyword & 0xff000000 ) || 
		 ( pColors->crString & 0xff000000 ) || 
		 ( pColors->crOperator & 0xff000000 ) ||
		 ( pColors->crTagText & 0xff000000 ) || 
	     ( pColors->crTagEntity & 0xff000000 ) ||
	     ( pColors->crTagElementName & 0xff000000 ) ||
	     ( pColors->crTagAttributeName & 0xff000000 ) ||
	     ( pColors->crLineNumber & 0xff000000 ) ||
	     ( pColors->crHighlightedLine & 0xff000000 ) )
	{
		// invalid colors
		ASSERT( FALSE );
		return CME_BADARGUMENT;
	}

	// assign the colors
	HDC hDC = GetDC( NULL );

	m_Colors.crWindow = pColors->crWindow; if ( m_Colors.crWindow != CLR_INVALID ) m_Colors.crWindow = GetNearestColor( hDC, pColors->crWindow );
	m_Colors.crLeftMargin = pColors->crLeftMargin; if ( m_Colors.crLeftMargin != CLR_INVALID ) m_Colors.crLeftMargin = GetNearestColor( hDC, pColors->crLeftMargin );
	m_Colors.crBookmark = pColors->crBookmark; if ( m_Colors.crBookmark != CLR_INVALID ) m_Colors.crBookmark = GetNearestColor( hDC, pColors->crBookmark );
	m_Colors.crBookmarkBk = pColors->crBookmarkBk; if ( m_Colors.crBookmarkBk != CLR_INVALID ) m_Colors.crBookmarkBk = GetNearestColor( hDC, pColors->crBookmarkBk );
	m_Colors.crText = GetNearestColor( hDC, pColors->crText );
	m_Colors.crTextBk = pColors->crTextBk; if ( m_Colors.crTextBk != CLR_INVALID ) m_Colors.crTextBk = GetNearestColor( hDC, pColors->crTextBk );
	m_Colors.crNumber = GetNearestColor( hDC, pColors->crNumber );
	m_Colors.crNumberBk = pColors->crNumberBk; if ( m_Colors.crNumberBk != CLR_INVALID ) m_Colors.crNumberBk = GetNearestColor( hDC, pColors->crNumberBk );
	m_Colors.crKeyword = GetNearestColor( hDC, pColors->crKeyword );
	m_Colors.crKeywordBk = pColors->crKeywordBk; if ( m_Colors.crKeywordBk != CLR_INVALID ) m_Colors.crKeywordBk = GetNearestColor( hDC, pColors->crKeywordBk );
	m_Colors.crOperator = GetNearestColor( hDC, pColors->crOperator );
	m_Colors.crOperatorBk = pColors->crOperatorBk; if ( m_Colors.crOperatorBk != CLR_INVALID ) m_Colors.crOperatorBk = GetNearestColor( hDC, pColors->crOperatorBk );
	m_Colors.crScopeKeyword = GetNearestColor( hDC, pColors->crScopeKeyword );
	m_Colors.crScopeKeywordBk = pColors->crScopeKeywordBk; if ( m_Colors.crScopeKeywordBk != CLR_INVALID ) m_Colors.crScopeKeywordBk = GetNearestColor( hDC, pColors->crScopeKeywordBk );
	m_Colors.crComment = GetNearestColor( hDC, pColors->crComment );
	m_Colors.crCommentBk = pColors->crCommentBk; if ( m_Colors.crCommentBk != CLR_INVALID ) m_Colors.crCommentBk = GetNearestColor( hDC, pColors->crCommentBk );
	m_Colors.crString = GetNearestColor( hDC, pColors->crString );
	m_Colors.crStringBk = pColors->crStringBk; if ( m_Colors.crStringBk != CLR_INVALID ) m_Colors.crStringBk = GetNearestColor( hDC, pColors->crStringBk );
	m_Colors.crTagText = GetNearestColor( hDC, pColors->crTagText );
	m_Colors.crTagTextBk = pColors->crTagTextBk; if ( m_Colors.crTagTextBk != CLR_INVALID ) m_Colors.crTagTextBk = GetNearestColor( hDC, pColors->crTagTextBk );
	m_Colors.crTagEntity = GetNearestColor( hDC, pColors->crTagEntity );
	m_Colors.crTagEntityBk = pColors->crTagEntityBk; if ( m_Colors.crTagEntityBk != CLR_INVALID ) m_Colors.crTagEntityBk = GetNearestColor( hDC, pColors->crTagEntityBk );
	m_Colors.crTagElementName = GetNearestColor( hDC, pColors->crTagElementName );
	m_Colors.crTagElementNameBk = pColors->crTagElementNameBk; if ( m_Colors.crTagElementNameBk != CLR_INVALID ) m_Colors.crTagElementNameBk = GetNearestColor( hDC, pColors->crTagElementNameBk );
	m_Colors.crTagAttributeName = GetNearestColor( hDC, pColors->crTagAttributeName );
	m_Colors.crTagAttributeNameBk = pColors->crTagAttributeNameBk; if ( m_Colors.crTagAttributeNameBk != CLR_INVALID ) m_Colors.crTagAttributeNameBk = GetNearestColor( hDC, pColors->crTagAttributeNameBk );
	m_Colors.crLineNumber = GetNearestColor( hDC, pColors->crLineNumber );
	m_Colors.crLineNumberBk = pColors->crLineNumberBk; if ( m_Colors.crLineNumberBk != CLR_INVALID ) m_Colors.crLineNumberBk = GetNearestColor( hDC, pColors->crLineNumberBk );
	m_Colors.crHDividerLines = pColors->crHDividerLines;
	m_Colors.crVDividerLines = pColors->crVDividerLines;
	m_Colors.crHighlightedLine = GetNearestColor( hDC, pColors->crHighlightedLine );
	ReleaseDC( NULL, hDC );

	// refresh the display to reflect the changes
	Repaint( FALSE );
	return CME_SUCCESS;
}

LRESULT CEdit::OnGetColors( WPARAM /*wParam*/, LPARAM lParam )
{
	CM_COLORS *pColors = ( CM_COLORS * ) lParam;
	if ( !pColors )
	{
		// invalid destination
		return CME_BADARGUMENT;
	}

	// assign the colors
	memcpy( pColors, &m_Colors, sizeof( m_Colors ) );

	return CME_SUCCESS;
}

LRESULT CEdit::OnEnableWhitespaceDisplay( WPARAM wParam, LPARAM /* lParam */ )
{
	m_bDisplayWhitespace = ( wParam != 0 );
	Repaint( FALSE );
	return CME_SUCCESS;
}

LRESULT CEdit::OnIsWhitespaceDisplayEnabled( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return ( LRESULT ) m_bDisplayWhitespace;
}

LRESULT CEdit::OnEnableTabExpand( WPARAM wParam, LPARAM /* lParam */ )
{
	m_bExpandTabs = ( wParam != 0 );
	return CME_SUCCESS;
}

LRESULT CEdit::OnIsTabExpandEnabled( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return ( LRESULT ) m_bExpandTabs;
}

LRESULT CEdit::OnEnableSmoothScrolling( WPARAM wParam, LPARAM /* lParam */ )
{
	m_bSmoothScroll = wParam == 1;
	return CME_SUCCESS;
}

LRESULT CEdit::OnIsSmoothScrollingEnabled( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return m_bSmoothScroll;
}

LRESULT CEdit::OnSetTabSize( WPARAM wParam, LPARAM /* lParam */ )
{
	if ( wParam >= CM_MIN_TABSIZE && wParam <= CM_MAX_TABSIZE )
	{
		m_Buffer.SetTabSize( ( int ) wParam );
		Repaint( FALSE );
		m_Selection.UpdateCaretPosition();
		return CME_SUCCESS;
	}
	else
	{
		return CME_BADARGUMENT;
	}
}

LRESULT CEdit::OnGetTabSize( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return m_Buffer.GetTabSize();
}

LRESULT CEdit::OnSetReadOnly( WPARAM wParam, LPARAM /* lParam */ )
{
	m_Buffer.SetReadOnly( wParam != 0 );
	return CME_SUCCESS;
}

LRESULT CEdit::OnIsReadOnly( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return m_Buffer.IsReadOnly();
}

LRESULT CEdit::OnEnableLineToolTips( WPARAM wParam, LPARAM /* lParam */ )
{
	m_bLineToolTips = wParam == 1;
	return CME_SUCCESS;
}

LRESULT CEdit::OnIsLineToolTipsEnabled( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return m_bLineToolTips;
}

LRESULT CEdit::OnEnableLeftMargin( WPARAM wParam, LPARAM /* lParam */ )
{
	BOOL bNew = ( wParam != 0 );
	if ( bNew != m_bShowLeftMargin )
	{
		m_bShowLeftMargin = bNew;
		RecalcViews();
		m_Selection.UpdateCaretPosition();	// make sure caret is not over a scrollbar
		Repaint( FALSE );
	}
	return CME_SUCCESS;
}

LRESULT CEdit::OnIsLeftMarginEnabled( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return m_bShowLeftMargin;
}

LRESULT CEdit::OnSaveFile( WPARAM wParam, LPARAM lParam )
{
	if ( lParam )
	{
		CHourGlass wait( this );
		HANDLE hFile = CreateFile( ( LPCTSTR ) lParam, 
		                           GENERIC_WRITE,
							 	   0,
							 	   NULL,
							 	   CREATE_ALWAYS,
							 	   FILE_ATTRIBUTE_NORMAL,
								   NULL );
		if ( hFile != INVALID_HANDLE_VALUE )
		{
			BOOL bSuccess = m_Buffer.GetText( hFile );
			CloseHandle( hFile );

			if ( bSuccess )
			{
				if ( wParam != 0 )
				{
					m_Buffer.ClearRedoChain( TRUE );
				}
				CEditTransaction trans( this );
				m_Buffer.SetModified( FALSE );
			}
			return bSuccess ? CME_SUCCESS : CME_FAILURE;
		}

		return CME_FAILURE;
	}
	return CME_BADARGUMENT;
}

LRESULT CEdit::OnGetTextLength( WPARAM wParam, LPARAM lParam )
{
	// wParam == range (optional)
	// lParam == TRUE if logical count, FALSE if TCHAR count
	if ( wParam )
	{
		CM_RANGE *pRange = ( CM_RANGE * ) wParam;
		if ( RangeIsValid( pRange ) )
		{
			HGLOBAL hMem;
			m_Buffer.GetText( pRange->posStart.nLine, pRange->posStart.nCol, pRange->posEnd.nLine, pRange->posEnd.nCol, hMem, FALSE );
			int cb = 0;
			if ( hMem )
			{
				LPCTSTR psz = ( LPCTSTR ) GlobalLock( hMem );
				cb = lParam ? log_strlen( psz ) :	// logical char count
				              _tcslen( psz );		// TCHAR char count
				GlobalUnlock( hMem );
				GlobalFree( hMem );
			}
			return cb;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		int cb = 0;

		if ( !m_Buffer.IsEmpty() )
		{
			int nLineCount = m_Buffer.GetLineCount();
			for ( int i = 0; i < nLineCount; i++ )
			{
				cb += lParam ? log_strlen( m_Buffer.GetLineText( i ) ) :	// logical char count
				               m_Buffer.GetLineLength( i );					// TCHAR char count
			}

			// count the line delimiters
			cb += ( m_Buffer.WantCarriageReturn() ? 2 : 1 ) * ( nLineCount - 1 );
		}
		return cb;
	}
}

LRESULT CEdit::OnGetText( WPARAM wParam, LPARAM lParam )
{
	if ( lParam )
	{
		if ( wParam )
		{
			CM_RANGE *pRange = ( CM_RANGE * ) wParam;
			if ( RangeIsValid( pRange ) )
			{
				HGLOBAL hMem;
				m_Buffer.GetText( pRange->posStart.nLine, pRange->posStart.nCol, pRange->posEnd.nLine, pRange->posEnd.nCol, hMem, FALSE );
				if ( hMem )
				{
					LPCTSTR psz = ( LPCTSTR ) GlobalLock( hMem );
					_tcscpy( ( LPTSTR ) lParam, psz );
					GlobalUnlock( hMem );
					GlobalFree( hMem );
					return CME_SUCCESS;
				}
				else
				{
					return CME_FAILURE;
				}
			}
			else
			{
				return CME_BADARGUMENT;
			}
		}
		else
		{
			m_Buffer.GetText( ( LPTSTR ) lParam );
			return CME_SUCCESS;
		}
	}
	else
	{
		return CME_BADARGUMENT;
	}
}

LRESULT CEdit::OnGetLineCount( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return m_Buffer.GetLineCount();
}

LRESULT CEdit::OnGetLine( WPARAM wParam, LPARAM lParam )
{
	if ( LineIsValid( ( int ) wParam ) && lParam )
	{
		_tcscpy( ( LPTSTR ) lParam, m_Buffer.GetLineText( ( int ) wParam ) );
		return CME_SUCCESS;
	}
	else
	{
		return CME_BADARGUMENT;
	}
}

LRESULT CEdit::OnGetLineLength( WPARAM wParam, LPARAM lParam )
{
	if ( LineIsValid( ( int ) wParam ) )
	{
		return lParam ? log_strlen( m_Buffer.GetLineText( ( int ) wParam ) ) :	// logical char count
		                m_Buffer.GetLineLength( ( int ) wParam );				// TCHAR char count
	}
	else
	{
		return CME_BADARGUMENT;
	}
}

LRESULT CEdit::OnGetWord( WPARAM wParam, LPARAM lParam )
{
	if ( lParam )
	{
		CDelayRepaint delay( this );
		{
			CSaveSelection save( this, FALSE );
			if ( wParam )
			{
				CM_POSITION *pPos = ( CM_POSITION * ) wParam;
				m_Selection.SetEmptySelection( pPos->nCol, pPos->nLine, FALSE, FALSE );
			}
			m_Selection.SelectWord();

			if ( m_Selection.IsEmpty() )
			{
				*( LPTSTR ) lParam = _T('\0');
			}
			else
			{
				int nStartCol, nStartRow, nEndCol, nEndRow;
				m_Selection.GetBufferSelection( nStartCol, nStartRow, nEndCol, nEndRow );
				HGLOBAL hMem;
				m_Buffer.GetText( nStartRow, nStartCol, nEndRow, nEndCol, hMem, TRUE );
				if ( hMem )
				{
					LPCTSTR psz = ( LPCTSTR ) GlobalLock( hMem );
					_tcscpy( ( LPTSTR ) lParam, psz );
					GlobalUnlock( hMem );
					GlobalFree( hMem );
				}
			}
		}
		return CME_SUCCESS;
	}
	else
	{
		return CME_BADARGUMENT;
	}
}

LRESULT CEdit::OnGetWordLength( WPARAM wParam, LPARAM lParam )
{
	// wParam == buffer position (optional)
	// lParam == TRUE if logical count, FALSE if TCHAR count
	int cb = 0;
	CDelayRepaint delay( this );
	{
		CSaveSelection save( this, FALSE );
		if ( wParam )
		{
			CM_POSITION *pPos = ( CM_POSITION * ) wParam;
			m_Selection.SetEmptySelection( pPos->nCol, pPos->nLine, FALSE, FALSE );
		}
		m_Selection.SelectWord();

		if ( !m_Selection.IsEmpty() )
		{
			int nStartCol, nStartRow, nEndCol, nEndRow;
			m_Selection.GetBufferSelection( nStartCol, nStartRow, nEndCol, nEndRow );
			HGLOBAL hMem;
			m_Buffer.GetText( nStartRow, nStartCol, nEndRow, nEndCol, hMem, TRUE );
			if ( hMem )
			{
				LPCTSTR psz = ( LPCTSTR ) GlobalLock( hMem );
				cb = lParam ? log_strlen( psz ) :
				              _tcslen( psz );
				GlobalUnlock( hMem );
				GlobalFree( hMem );
			}
		}
	}
	return cb;
}

LRESULT CEdit::OnAddText( WPARAM /* wParam */, LPARAM lParam )
{
	if ( lParam )
	{
		CEditTransaction trans( this );
		CSelTransaction sel( this );
		CSaveSelection save( this );
		int nLine = 0;
		int nCol = 0;
		if ( !m_Buffer.IsEmpty() )
		{
			nLine = m_Buffer.GetLineCount() - 1;
			nCol = m_Buffer.GetLineLength( nLine );
		}

		m_Selection.SetEmptySelection( nCol, nLine );
		ReplaceSelection( ( LPCTSTR ) lParam, FALSE, FALSE );
		return CME_SUCCESS;
	}
	else
	{
		return CME_BADARGUMENT;
	}
}

LRESULT CEdit::OnDeleteLine( WPARAM wParam, LPARAM /* lParam */ )
{
	CEditTransaction trans( this );
	CSelTransaction sel( this );
	if ( LineIsValid( ( int ) wParam ) )
	{
		m_Buffer.RemoveLines( ( int ) wParam, 1 );
		return CME_SUCCESS;
	}
	else
	{
		return CME_BADARGUMENT;
	}
}

LRESULT CEdit::OnInsertLine( WPARAM wParam, LPARAM lParam )
{
	CEditTransaction trans( this );
	CSelTransaction sel( this );
	if ( lParam && ( LineIsValid( ( int ) wParam, TRUE ) ) )
	{
		LPCTSTR psz = ( LPCTSTR ) lParam;
		BOOL bHasLineBreak = FALSE;
		while ( *psz && !bHasLineBreak )
		{
			bHasLineBreak = ( *psz == '\r' || *psz == '\n'  );
			psz++;
		}
		if ( bHasLineBreak )
		{
			return CME_BADARGUMENT;
		}
		else
		{
			m_Buffer.InsertLine( ( int ) wParam, ( LPCTSTR ) lParam, -1 );
			return CME_SUCCESS;
		}
	}
	else
	{
		return CME_BADARGUMENT;
	}
}

LRESULT CEdit::OnGetSel( WPARAM wParam, LPARAM lParam )
{
	if ( lParam )
	{
		CM_RANGE *pRange = ( CM_RANGE * ) lParam;
		if ( wParam )
		{
			m_Selection.GetNormalizedBufferSelection( pRange->posStart.nCol, pRange->posStart.nLine, pRange->posEnd.nCol, pRange->posEnd.nLine );
		}
		else
		{
			m_Selection.GetBufferSelection( pRange->posStart.nCol, pRange->posStart.nLine, pRange->posEnd.nCol, pRange->posEnd.nLine );
		}
		pRange->bColumnSel = m_Selection.IsColumnSel();
		return CME_SUCCESS;
	}
	else
	{
		return CME_BADARGUMENT;
	}
}

LRESULT CEdit::OnSetSel( WPARAM wParam, LPARAM lParam )
{
	CM_RANGE *pRange = ( CM_RANGE * ) lParam;
	if ( lParam && RangeIsValid( pRange ) )
	{
		m_Selection.SetExtendedSelection( pRange->posStart.nCol, pRange->posStart.nLine, pRange->posEnd.nCol, pRange->posEnd.nLine, wParam != 0 );
		m_Selection.EnableColumnSel( pRange->bColumnSel );
		// if in the middle of dragging or whatever, bail now -- the caller
		// modified the selection and probably doesn't want the drag to continue.
		CancelMode();
		return CME_SUCCESS;
	}
	else
	{
		return CME_BADARGUMENT;
	}
}

LRESULT CEdit::OnDeleteSel( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	CEditTransaction trans( this );
	CSelTransaction sel( this );
	DeleteSelection( FALSE, FALSE );
	return CME_SUCCESS;
}

LRESULT CEdit::OnReplaceSel( WPARAM /* wParam */, LPARAM lParam )
{
	if ( lParam )
	{
		CEditTransaction trans( this );
		CSelTransaction sel( this );
		CSaveSelection save( this );
		ReplaceSelection( ( LPCTSTR ) lParam, FALSE, FALSE );
		return CME_SUCCESS;
	}
	else
	{
		return CME_BADARGUMENT;
	}
}

LRESULT CEdit::OnExecuteCmd( WPARAM wParam, LPARAM lParam )
{
	if ( ( wParam >= CMD_FIRST && wParam <= CMD_LAST ) || wParam >= CMD_USER_BASE )
	{
		ExecuteCommand( ( WORD ) wParam, ( DWORD ) lParam, FALSE );
		return CME_SUCCESS;
	}
	else
	{
		return CME_BADARGUMENT;
	}
}

LRESULT CEdit::OnSetSplitterPos( WPARAM wParam, LPARAM lParam )
{
	if ( ( int ) lParam >= 0 )
	{
		if ( ( wParam && !m_bAllowHSplit ) ||
		     ( !wParam && !m_bAllowVSplit ) )
		{
			return CME_FAILURE;
		}
		else
		{
			m_xPosDown = m_xPosHSplitter;
			m_yPosDown = m_yPosVSplitter;
			if ( wParam )
			{
				BeginSplitting( eHSplitter );
				EndSplitting( ( int ) lParam, 0, TRUE );
			}
			else
			{
				BeginSplitting( eVSplitter );
				EndSplitting( 0, ( int ) lParam, TRUE );
			}
			return CME_SUCCESS;
		}
	}
	else
	{
		return CME_BADARGUMENT;
	}
}

LRESULT CEdit::OnGetSplitterPos( WPARAM wParam, LPARAM /* lParam */ )
{
	if ( wParam )
	{
		return ( m_bAllowHSplit ? m_xPosHSplitter : -1 ); 
	}
	else
	{
		return ( m_bAllowVSplit ? m_yPosVSplitter : -1 ); 
	}
}

LRESULT CEdit::OnSetAutoIndentMode( WPARAM wParam, LPARAM /* lParam */ )
{
	if ( wParam == CM_INDENT_OFF ||
	     wParam == CM_INDENT_SCOPE ||
	     wParam == CM_INDENT_PREVLINE )
	{
		m_eIndentStyle = ( IndentStyle ) wParam;
		return CME_SUCCESS;
	}
	else
	{
		return CME_BADARGUMENT;
	}
}

LRESULT CEdit::OnGetAutoIndentMode( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return m_eIndentStyle;
}

LRESULT CEdit::OnCanCut( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return ( LRESULT ) CanCut( FALSE );
}

LRESULT CEdit::OnCut( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	CEditTransaction trans( this );
	CSelTransaction sel( this );
	if ( CanCut( FALSE ) )
	{
		Cut();
		return CME_SUCCESS;
	}
	else
	{
		return CME_FAILURE;
	}
}

LRESULT CEdit::OnCanCopy( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return ( LRESULT ) CanCopy();
}

LRESULT CEdit::OnCopy( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	if ( CanCopy() )
	{
		Copy( TRUE );
		return CME_SUCCESS;
	}
	else
	{
		return CME_FAILURE;
	}
}
		
LRESULT CEdit::OnCanPaste( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return ( LRESULT ) ( IsClipboardFormatAvailable( CLIP_TEXT ) && !m_Buffer.IsReadOnly() );
}

LRESULT CEdit::OnPaste( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	CSelTransaction sel( this );
	if ( OnCanPaste( 0, 0 ) )
	{
		Paste();
		return CME_SUCCESS;
	}
	else
	{
		return CME_FAILURE;
	}
}

LRESULT CEdit::OnCanUndo( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return ( LRESULT ) m_Buffer.CanUndo();
}

LRESULT CEdit::OnCanRedo( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return ( LRESULT ) m_Buffer.CanRedo();
}

LRESULT CEdit::OnRedo( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	CSelTransaction sel( this );
	if ( m_Buffer.CanRedo() )
	{
		Redo();
		return CME_SUCCESS;
	}
	else
	{
		NotifyParentOfCmdFailure( CMDERR_EMPTYBUF );
		return CME_FAILURE;
	}
}

LRESULT CEdit::OnUndo( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	CSelTransaction sel( this );
	if ( m_Buffer.CanUndo() )
	{
		Undo();
		return CME_SUCCESS;
	}
	else
	{
		NotifyParentOfCmdFailure( CMDERR_EMPTYBUF );
		return CME_FAILURE;
	}
}

LRESULT CEdit::OnClearUndoBuffer( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	m_Buffer.ClearRedoChain( TRUE );
	return CME_SUCCESS;
}

LRESULT CEdit::OnSetUndoLimit( WPARAM wParam, LPARAM /* lParam */ )
{
	int nAmount = ( int ) wParam;
	if ( nAmount >= -1 )
	{
		m_Buffer.SetMaxUndo( nAmount );
		return CME_SUCCESS;
	}
	else
	{
		return CME_BADARGUMENT;
	}
}

LRESULT CEdit::OnGetUndoLimit( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return m_Buffer.GetMaxUndo();
}

LRESULT CEdit::OnIsModified( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return m_Buffer.IsModified();
}

LRESULT CEdit::OnSetModified( WPARAM wParam, LPARAM /* lParam */ )
{
	CEditTransaction trans( this ); // want transaction
	m_Buffer.SetModified( wParam != 0 );
	return CME_SUCCESS;
}

LRESULT CEdit::OnEnableOvertype( WPARAM wParam, LPARAM /* lParam */ )
{
	BOOL bOldVal = m_bOverType;
	m_bOverType = ( wParam != 0 );
	// make sure the correct caret (fat or thin) is set
	m_Selection.ResetCaret( m_font->hFont );

	if ( m_bOverType != bOldVal )
	{
		NotifyParent( CMN_OVERTYPECHANGE );
	}

	return CME_SUCCESS;
}

LRESULT CEdit::OnIsOvertypeEnabled( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return m_bOverType;
}

LRESULT CEdit::OnEnableCaseSensitive( WPARAM wParam, LPARAM /* lParam */ )
{
	g_FindReplaceData.m_bCaseSensitiveSearch = ( wParam != 0 );
	return CME_SUCCESS;
}

LRESULT CEdit::OnIsCaseSensitiveEnabled( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return g_FindReplaceData.m_bCaseSensitiveSearch;
}

LRESULT CEdit::OnEnablePreserveCase( WPARAM wParam, LPARAM /* lParam */ )
{
	g_FindReplaceData.m_bPreserveCase = ( wParam != 0 );
	return CME_SUCCESS;
}

LRESULT CEdit::OnIsPreserveCaseEnabled( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return g_FindReplaceData.m_bPreserveCase;
}

LRESULT CEdit::OnEnableWholeWord( WPARAM wParam, LPARAM /* lParam */ )
{
	g_FindReplaceData.m_bWholeWordOnly = ( wParam != 0 );
	return CME_SUCCESS;
}

LRESULT CEdit::OnEnableRegExp( WPARAM wParam, LPARAM /* lParam */ )
{
	g_FindReplaceData.m_bRegExp = ( wParam != 0 );
	return CME_SUCCESS;
}

LRESULT CEdit::OnIsWholeWordEnabled( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return g_FindReplaceData.m_bWholeWordOnly;
}

LRESULT CEdit::OnIsRegExpEnabled( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return g_FindReplaceData.m_bRegExp;
}

LRESULT CEdit::OnSetTopIndex( WPARAM wParam, LPARAM lParam )
{
	if ( ( wParam < ( WPARAM ) m_nViews ) && LineIsValid( lParam ) )
	{
		CEditView *pView = m_Views[ wParam ];
		pView->ScrollTo( pView->GetLeftIndex(), lParam );
		return CME_SUCCESS;
	}
	else
	{
		return CME_BADARGUMENT;
	}
}

LRESULT CEdit::OnGetTopIndex( WPARAM wParam, LPARAM /* lParam */ )
{
	if ( wParam < ( WPARAM ) m_nViews )
	{
		return m_Views[ wParam ]->GetTopIndex();
	}
	else
	{
		return CME_BADARGUMENT;
	}
}

LRESULT CEdit::OnGetVisibleLineCount( WPARAM wParam, LPARAM lParam )
{
	if ( wParam < ( WPARAM ) m_nViews )
	{
		CEditView *pView = m_Views[ wParam ];
		return ( pView->GetBottomIndex( lParam != 0 ) - pView->GetTopIndex() + 1 );
	}
	else
	{
		return CME_BADARGUMENT;
	}
}

LRESULT CEdit::OnEnableCRLF( WPARAM wParam, LPARAM /* lParam */ )
{
	m_Buffer.EnableCarriageReturn( ( wParam != 0 ) );
	return CME_SUCCESS;
}

LRESULT CEdit::OnIsCRLFEnabled( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return m_Buffer.WantCarriageReturn();
}

LRESULT CEdit::OnGetCurrentView( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	for ( int i = 0; i < m_nViews; i++ )
	{
		if ( m_pActiveView == m_Views[ i ] )
		{
			return i;
		}
	}
	return 0;
}

LRESULT CEdit::OnSetCurrentView(WPARAM wParam, LPARAM /* lParam */)
	{
	CME_CODE cmResult = CME_BADARGUMENT;
	if (/*wParam >= 0 && */wParam < m_nViews)
		{
		cmResult = CME_SUCCESS;
		SetActiveView(m_Views[wParam]);
		}
	return cmResult;
	}

LRESULT CEdit::OnGetViewCount( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return m_nViews;
}


LRESULT CEdit::OnShowScrollBar( WPARAM wParam, LPARAM lParam )
{
	BOOL bRecalc = FALSE;
	BOOL bShow = lParam != 0;
	if ( wParam )
	{
		if ( ShowHScrollBar() != bShow )
		{
			m_dwStyle = bShow ? m_dwStyle | WS_HSCROLL : m_dwStyle & ~WS_HSCROLL;
			if ( !bShow && m_bAllowHSplit )
			{
				// cannot have splitter all by itself
				m_bAllowHSplit = FALSE;
			}
			bRecalc = TRUE;
		}
	}
	else
	{
		if ( ShowVScrollBar() != bShow )
		{
			m_dwStyle = bShow ? m_dwStyle | WS_VSCROLL : m_dwStyle & ~WS_VSCROLL;
			if ( !bShow && m_bAllowVSplit )
			{
				// cannot have splitter all by itself
				m_bAllowVSplit = FALSE;
			}
			bRecalc = TRUE;
		}
	}

	if ( bRecalc )
	{
		RecalcViews();
	}

	return CME_SUCCESS;
}

LRESULT CEdit::OnHasScrollBar( WPARAM wParam, LPARAM /* lParam */ )
{
	return ( wParam ? ShowHScrollBar() : ShowVScrollBar() );
}

LRESULT CEdit::OnGetSelFromPoint( WPARAM wParam, LPARAM lParam )
{
	if ( lParam )
	{
		int nView;
		int x = LOWORD( wParam );
		int y = HIWORD( wParam );
		HitTestCode eLocation = HitTest( x, y, nView );
		if ( eLocation == eEditSpace )
		{
			RECT rcChar;
			int nCol, nRow;
			int x = LOWORD( wParam );
			m_Views[ nView ]->GetCharPosFromPoint( x, y, nCol, nRow, &rcChar );
			// round up to next col if necessary
			if ( !IsRectEmpty( &rcChar ) && ( x > ( rcChar.left + rcChar.right ) / 2 ) )
			{
				nCol++;
			}

			// caller will want buffer coords -- view coords don't mean much to them.
			if ( nRow < m_Buffer.GetLineCount() )
			{
				nCol = m_Buffer.ConvertViewColToBufferCol( nRow, nCol );
			}
	
			CM_POSITION *pPos = ( CM_POSITION * )lParam;
			pPos->nCol = nCol;
			pPos->nLine = nRow;
			return CME_SUCCESS;
		}
		else
		{
			return CME_FAILURE;
		}
	}
	else
	{
		return CME_BADARGUMENT;
	}	
}

LRESULT CEdit::OnSelectLine( WPARAM wParam, LPARAM lParam )
{
	if ( LineIsValid( wParam ) )
	{
		m_Selection.SetExtendedSelection( m_Buffer.GetLineLength( wParam ), wParam, 0, wParam, lParam != 0 );
		return CME_SUCCESS;
	}
	else
	{
		return CME_BADARGUMENT;
	}	
}

LRESULT CEdit::OnHitTest( WPARAM wParam, LPARAM /* lParam */ )
{
	int nView;
	HitTestCode eLocation = HitTest( LOWORD( wParam ), HIWORD( wParam ), nView );
	return MAKELPARAM( nView, eLocation );
}

LRESULT CEdit::OnEnableDragDrop( WPARAM wParam, LPARAM /* lParam */ )
{
	m_bAllowDragDrop = ( wParam != 0 );
	return CME_SUCCESS;
}

LRESULT CEdit::OnIsDragDropEnabled( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return m_bAllowDragDrop;
}

LRESULT CEdit::OnEnableSplitter( WPARAM wParam, LPARAM lParam )
{
	BOOL bRecalc = FALSE;	
	BOOL bAllow = lParam != 0;
	if ( wParam )
	{
		if ( m_bAllowHSplit != bAllow )
		{
			if ( bAllow && !ShowHScrollBar() )
			{
				// cannot turn on splitter unless scrollbar is visible
				return CME_FAILURE;	
			}
			else if ( !bAllow && m_xPosHSplitter )
			{
				BeginSplitting( eHSplitter );
				m_xPosDown = m_xPosHSplitter;
				m_yPosDown = m_yPosVSplitter;
				EndSplitting( 0, 0, TRUE );
			}
			m_bAllowHSplit = bAllow;
			bRecalc = TRUE;
		}
	}
	else
	{
		if ( m_bAllowVSplit != bAllow )
		{
			if ( bAllow && !ShowVScrollBar() )
			{
				// cannot turn on splitter unless scrollbar is visible
				return CME_FAILURE;	
			}
			else if ( !bAllow && m_yPosVSplitter )
			{
				BeginSplitting( eVSplitter );
				m_xPosDown = m_xPosHSplitter;
				m_yPosDown = m_yPosVSplitter;
				EndSplitting( 0, 0, TRUE );
			}
			m_bAllowVSplit = bAllow;
			bRecalc = TRUE;
		}
	}

	if ( bRecalc )
	{
		RecalcViews();
	}

	Repaint( FALSE );

	return CME_SUCCESS;
}

LRESULT CEdit::OnIsSplitterEnabled( WPARAM wParam, LPARAM /* lParam */ )
{
	return ( ( wParam != 0 ) ? m_bAllowHSplit : m_bAllowVSplit );
}

LRESULT CEdit::OnEnableColumnSel( WPARAM wParam, LPARAM /* lParam */ )
{
	// column selection doesn't really fit well into a DBCS environment
	m_bAllowColumnSel = ( wParam != 0 ) && !DBCS_ACTIVE;
	return CME_SUCCESS;
}

LRESULT CEdit::OnIsColumnSelEnabled( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return m_bAllowColumnSel;
}

LRESULT CEdit::OnEnableGlobalProps( WPARAM wParam, LPARAM /* lParam */ )
{
	m_bGlobalProps = ( wParam != 0 );
	return CME_SUCCESS;
}

LRESULT CEdit::OnIsGlobalPropsEnabled( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return m_bGlobalProps;
}

LRESULT CEdit::OnIsPlayingMacro( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return IsPlayingMacro();
}

LRESULT CEdit::OnIsRecordingMacro( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return IsRecordingMacro();
}

LRESULT CEdit::OnGetFontOwnership( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return m_bOwnFont;
}

LRESULT CEdit::OnSetFontOwnership( WPARAM wParam, LPARAM /* lParam */ )
{
	if ( g_hFontDefault != m_font->hFont )
	{
		m_bOwnFont = ( wParam != 0 );
		return CME_SUCCESS;
	}
	else
	{
		// caller has no say whether I own the default font
		return CME_FAILURE;
	}
}

LRESULT CEdit::OnSetDlgParent( WPARAM wParam, LPARAM /* lParam */ )
{
	HWND hWndParent = ( HWND ) wParam;
	if ( !hWndParent || IsWindow( hWndParent ) )
	{
		m_hWndDlgParent = hWndParent;
		return CME_SUCCESS;
	}
	else
	{
		// invalid HWND
		return CME_BADARGUMENT;
	}
}

LRESULT CEdit::OnEnableSelBounds( WPARAM wParam, LPARAM /* lParam */ )
{
	m_bSelBounds = ( wParam != 0 );
	return CME_SUCCESS;
}

LRESULT CEdit::OnIsSelBoundsEnabled( WPARAM /*wParam*/, LPARAM /* lParam */ )
{
	return m_bSelBounds;
}

LRESULT CEdit::OnSetFontStyles( WPARAM /*wParam*/, LPARAM lParam )
{
	CM_FONTSTYLES *pFontStyles = ( CM_FONTSTYLES * ) lParam;
	if ( !pFontStyles )
	{
		// invalid FontStyles
		ASSERT( FALSE );
		return CME_BADARGUMENT;
	}

	// assign the FontStyles
	memcpy( &m_FontStyles, pFontStyles, sizeof( m_FontStyles ) );

	OnFontChanged();

	// refresh the display to reflect the changes
	Repaint( FALSE );
	return CME_SUCCESS;
}

LRESULT CEdit::OnGetFontStyles( WPARAM /*wParam*/, LPARAM lParam )
{
	CM_FONTSTYLES *pFontStyles = ( CM_FONTSTYLES * ) lParam;
	if ( !pFontStyles )
	{
		// invalid destination
		return CME_BADARGUMENT;
	}

	// assign the FontStyles
	memcpy( pFontStyles, &m_FontStyles, sizeof( m_FontStyles ) );

	return CME_SUCCESS;
}

LRESULT CEdit::OnSetItemData( WPARAM wParam, LPARAM lParam )
{
	// wParam == line no
	// lParam == itemdata
	LRESULT lResult = CME_BADARGUMENT;

	if ( LineIsValid( wParam, TRUE ) )
	{
		m_Buffer.SetItemData( wParam, lParam );
		lResult = CME_SUCCESS;
	}

	return lResult;
}

LRESULT CEdit::OnGetItemData( WPARAM wParam, LPARAM /*lParam*/ )
{
	// wParam == line no
	return ( LineIsValid( wParam, TRUE ) ? m_Buffer.GetItemData( wParam ) : 0 );
}

LRESULT CEdit::OnSetLineStyle( WPARAM wParam, LPARAM lParam )
{
	// wParam == line no
	// lParam == line style
	LRESULT lResult = CME_BADARGUMENT;

	if ( LineIsValid( wParam, TRUE ) )
	{
		m_Buffer.SetLineStyle( wParam, lParam );
		lResult = CME_SUCCESS;
	}

	return lResult;
}

LRESULT CEdit::OnGetLineStyle( WPARAM wParam, LPARAM /*lParam*/ )
{
	// wParam == line no
	return ( LineIsValid( wParam, TRUE ) ? m_Buffer.GetLineStyle( wParam ) : 0 );
}

LRESULT CEdit::OnSetBookmark( WPARAM wParam, LPARAM lParam )
{
	// wParam == line no
	// lParam == TRUE/FALSE
	LRESULT lResult = CME_BADARGUMENT;

	if ( LineIsValid( wParam, TRUE ) )
	{
		if ( lParam )
			m_Buffer.SetBookmark( wParam );
		else
			m_Buffer.RemoveBookmark( wParam );

		// force a repaint of the affected line
		m_pActiveView->DamageView( wParam, wParam );

		lResult = CME_SUCCESS;
	}

	return lResult;
}
		
LRESULT CEdit::OnGetBookmark( WPARAM wParam, LPARAM /*lParam*/ )
{
	// wParam == line no
	return ( LineIsValid( wParam, TRUE ) ? m_Buffer.HasBookmark( wParam ) : 0 );
}
		
LRESULT CEdit::OnSetAllBookmarks( WPARAM wParam, LPARAM lParam )
{
	// wParam == # of bookmarks
	// lParam == DWORD array of line numbers
	LRESULT lResult = CME_BADARGUMENT;

	if ( wParam && lParam )
	{
		lResult = CME_SUCCESS;
		LPDWORD pdwLines = ( LPDWORD ) lParam;
		// wipe the slate clean
		BookmarkClearAll();
		// turn on the bookmarks the caller requested
		for ( int i = 0; i < ( int ) wParam; i++ )
		{
			int nLine = pdwLines[ i ];
			if ( LineIsValid( nLine ) )
				m_Buffer.SetBookmark( nLine ); 
		}
		Repaint( FALSE );
	}

	return lResult;
}
		
LRESULT CEdit::OnGetAllBookmarks( WPARAM /*wParam*/, LPARAM lParam )
{
	// lParam = NULL to return the count, or DWORD buffer for bookmarks

	int nLines = m_Buffer.GetLineCount();
	LPDWORD pdwLines = ( LPDWORD ) lParam;
	int nBookmarks = 0;
	for ( int i = 0; i < nLines; i++ )
	{
		if ( m_Buffer.HasBookmark( i ) )
		{
			if ( lParam )
			{
				// caller supplied a buffer
				pdwLines[ nBookmarks ] = i;
			}
			nBookmarks++;
		}
	}
	
	return nBookmarks;
}

LRESULT CEdit::OnSetLineNumbering( WPARAM /*wParam*/, LPARAM lParam )
{
	// lParam == CM_LINENUMBERING
	LRESULT lResult = CME_BADARGUMENT;

	if ( lParam )
	{
		lResult = CME_SUCCESS;
		m_LineNum = *( ( CM_LINENUMBERING * ) lParam );
		m_LineNum.bEnabled = ( m_LineNum.bEnabled != 0 );
		if ( m_LineNum.dwStyle < CM_BINARY || m_LineNum.dwStyle > CM_HEXADECIMAL )
			m_LineNum.dwStyle = CM_DECIMAL;

		// rethink the width of the line number column
		int nWidthNew = _log( m_Buffer.GetLineCount() + m_LineNum.nStartAt - 1, m_LineNum.dwStyle );
		nWidthNew = max( nWidthNew, DEF_LINENUMBERMINWIDTH );
		SetLineNumWidth( nWidthNew );

		// view widths changed -- recalc their layouts
		RecalcViews();
		m_Selection.UpdateCaretPosition();
	}

	return lResult;
}

LRESULT CEdit::OnGetLineNumbering( WPARAM /*wParam*/, LPARAM lParam )
{
	// lParam == CM_LINENUMBERING *
	LRESULT lResult = CME_BADARGUMENT;

	if ( lParam )
	{
		lResult = CME_SUCCESS;
		*( ( CM_LINENUMBERING * ) lParam ) = m_LineNum;
	}

	return lResult;
}

LRESULT CEdit::OnPosFromChar( WPARAM wParam, LPARAM lParam )
{
	// wParam == CM_POSITION *
	// lParam == RECT *
	LRESULT lResult = CME_BADARGUMENT;
	CM_POSITION *pPos = ( CM_POSITION * ) wParam;

	if ( pPos && lParam &&
	     pPos->nCol >= 0 &&
		 pPos->nLine >= 0 )
	{
		m_pActiveView->GetCharBoundingRect( pPos->nCol, pPos->nLine, ( RECT * ) lParam );
		lResult = CME_SUCCESS;
	}

	return lResult;
}

LRESULT CEdit::OnEnableHideSel( WPARAM wParam, LPARAM /* lParam */ )
{
	m_bHideSel = ( wParam != 0 );
	return CME_SUCCESS;
}

LRESULT CEdit::OnIsHideSelEnabled( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return m_bHideSel;
}

LRESULT CEdit::OnGetHighlightedLine( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return m_nHighlightedLine;
}

LRESULT CEdit::OnSetHighlightedLine( WPARAM wParam, LPARAM /* lParam */ )
{
	// wParam == line no
	LRESULT lResult = CME_BADARGUMENT;
	int nLine = ( int ) wParam;

	if ( ( nLine == -1 ) || LineIsValid( nLine ) )
	{
		SetHighlightedLine(nLine);

		lResult = CME_SUCCESS;
	}

	return lResult;
}


LRESULT CEdit::OnEnableNormalizeCase( WPARAM wParam, LPARAM /* lParam */ )
{
	m_Buffer.EnableNormalizeCase( wParam != 0 );
	return CME_SUCCESS;
}

LRESULT CEdit::OnIsNormalizeCaseEnabled( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return m_Buffer.NormalizeCase();
}

LRESULT CEdit::OnSetDivider( WPARAM wParam, LPARAM lParam )
{
	// wParam == line no
	// lParam == TRUE/FALSE
	LRESULT lResult = CME_BADARGUMENT;

	if ( LineIsValid( wParam, TRUE ) )
	{
		m_Buffer.SetDivider( wParam, lParam != 0 );
		Repaint( FALSE );
		lResult = CME_SUCCESS;
	}

	return lResult;
}
		
LRESULT CEdit::OnGetDivider( WPARAM wParam, LPARAM /*lParam*/ )
{
	// wParam == line no
	return ( LineIsValid( wParam, TRUE ) ? m_Buffer.HasDivider( wParam ) : 0 );
}

LRESULT CEdit::OnEnableOvertypeCaret( WPARAM wParam, LPARAM /* lParam */ )
{
	m_bOvertypeCaret = ( wParam != 0 );
	m_Selection.ResetCaret( m_font->hFont );
	return CME_SUCCESS;
}

LRESULT CEdit::OnIsOvertypeCaretEnabled( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return m_bOvertypeCaret;
}

LRESULT CEdit::OnSetFindText( WPARAM /* wParam */, LPARAM lParam )
{
	SetFindText( lParam ? ( LPCTSTR ) lParam : NULL );
	return CME_SUCCESS;
}

LRESULT CEdit::OnGetFindText( WPARAM /* wParam */, LPARAM lParam )
{
	LPCTSTR psz = CEdit::g_FindReplaceData.m_pszFindText;
	if ( !psz )
		psz = _T("");
	_tcscpy( ( LPTSTR ) lParam, psz );

	return CME_SUCCESS;
}

LRESULT CEdit::OnSetReplaceText( WPARAM /* wParam */, LPARAM lParam )
{
	SetReplaceText( lParam ? ( LPCTSTR ) lParam : NULL );
	return CME_SUCCESS;
}

LRESULT CEdit::OnGetReplaceText( WPARAM /* wParam */, LPARAM lParam )
{
	LPCTSTR psz = CEdit::g_FindReplaceData.m_pszReplaceText;
	if ( !psz )
		psz = _T("");
	_tcscpy( ( LPTSTR ) lParam, psz );

	return CME_SUCCESS;
}

LRESULT CEdit::OnSetImageList( WPARAM /* wParam */, LPARAM lParam )
{
	m_hImageList = ( HIMAGELIST ) lParam;
	Repaint( FALSE );
	return CME_SUCCESS;
}

LRESULT CEdit::OnGetImageList( WPARAM /* wParam */, LPARAM /* lParam */ )
{
	return ( LRESULT ) m_hImageList;
}

LRESULT CEdit::OnSetMarginImages( WPARAM wParam, LPARAM lParam )
{
	// wParam == line no
	// lParam == byImages
	LRESULT lResult = CME_BADARGUMENT;

	if ( LineIsValid( wParam, TRUE ) )
	{
		m_Buffer.SetMarginImages( wParam, ( BYTE ) lParam );
		DamageAllViews( wParam, wParam );
		lResult = CME_SUCCESS;
	}

	return lResult;
}
		
LRESULT CEdit::OnGetMarginImages( WPARAM wParam, LPARAM /*lParam*/ )
{
	// wParam == line no
	return ( LineIsValid( wParam, FALSE ) ? m_Buffer.GetMarginImages( wParam ) : 0 );
}

LRESULT CEdit::OnAboutBox( WPARAM /*wParam*/, LPARAM /*lParam*/ )
{
	AboutBox();
	return CME_SUCCESS;
}

LRESULT CEdit::OnPrint( WPARAM wParam, LPARAM lParam )
{
	// wParam == print DC
	// lParam == flags
	return ( Print( ( HDC ) wParam, ( DWORD ) lParam ) ? CME_SUCCESS : CME_FAILURE );
}

LRESULT CEdit::OnSetCaretPos( WPARAM wParam, LPARAM lParam )
{
	// wParam == line no
	// lParam == col no
	LRESULT lResult = CME_BADARGUMENT;

	if ( LineIsValid( wParam, TRUE ) )
	{
		m_Selection.SetEmptySelection( ( int ) lParam, ( int ) wParam, TRUE );
		lResult = CME_SUCCESS;
	}

	return lResult;
}

LRESULT CEdit::OnViewColToBufferCol( WPARAM wParam, LPARAM lParam )
{
	// wParam == line no
	// lParam == col no
	if ( LineIsValid( wParam, TRUE ) )
	{
		return m_Buffer.ConvertViewColToBufferCol( ( int ) wParam, ( int ) lParam );
	}

	return ( LRESULT ) -1;
}

LRESULT CEdit::OnBufferColToViewCol( WPARAM wParam, LPARAM lParam )
{
	// wParam == line no
	// lParam == col no
	if ( LineIsValid( wParam, TRUE ) )
	{
		return m_Buffer.ConvertBufferColToViewCol( ( int ) wParam, ( int ) lParam );
	}

	return ( LRESULT ) -1;
}

LRESULT CEdit::OnGetBorderStyle( WPARAM /*wParam*/, LPARAM /* lParam */ )
{
	LRESULT lBorder = CM_BORDER_NONE;
	long lStyle = GetWindowLong( m_hWnd, GWL_STYLE );
	long lExStyle = GetWindowLong( m_hWnd, GWL_EXSTYLE );
	
	if ( HAS_FLAG( lStyle, WS_BORDER ) )
		lBorder |= CM_BORDER_THIN;
	if ( HAS_FLAG( lExStyle, WS_EX_CLIENTEDGE ) )
		lBorder |= CM_BORDER_CLIENT;
	if ( HAS_FLAG( lExStyle, WS_EX_STATICEDGE ) )
		lBorder |= CM_BORDER_STATIC;
	if ( HAS_FLAG( lExStyle, WS_EX_DLGMODALFRAME ) )
		lBorder |= CM_BORDER_MODAL;

	return lBorder;
}

LRESULT CEdit::OnSetBorderStyle( WPARAM wParam, LPARAM /* lParam */ )
{
	LRESULT lResult = CME_BADARGUMENT;
	if ( wParam == ( wParam & ( CM_BORDER_THIN | CM_BORDER_CLIENT | CM_BORDER_STATIC | CM_BORDER_MODAL ) ) )
	{
		long lStyle = ( GetWindowLong( m_hWnd, GWL_STYLE ) & ~WS_BORDER );
		long lExStyle = ( GetWindowLong( m_hWnd, GWL_EXSTYLE ) & ~( WS_EX_CLIENTEDGE | WS_EX_STATICEDGE | WS_EX_DLGMODALFRAME ) );
		
		if ( HAS_FLAG( wParam, CM_BORDER_THIN ) )
			lStyle |= WS_BORDER;
		if ( HAS_FLAG( wParam, CM_BORDER_CLIENT ) )
			lExStyle |= WS_EX_CLIENTEDGE;
		if ( HAS_FLAG( wParam, CM_BORDER_STATIC ) )
			lExStyle |= WS_EX_STATICEDGE;
		if ( HAS_FLAG( wParam, CM_BORDER_MODAL ) )
			lExStyle |= WS_EX_DLGMODALFRAME;

		SetWindowLong( m_hWnd, GWL_EXSTYLE, lExStyle );
		SetWindowLong( m_hWnd, GWL_STYLE, lStyle );
		SetWindowPos( m_hWnd, NULL, -1, -1, -1, -1, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_DRAWFRAME );
		lResult = CME_SUCCESS;
	}

	return lResult;
}

