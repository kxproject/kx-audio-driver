#ifndef __EDITVIEW_H__
#define __EDITVIEW_H__

#include "buffer.h"

class CEdit;
class CSelection;

class CEditView
{
	public:
		CEditView( CEdit *pCtrl, CBuffer *pBuffer, int nLeftIndex = 0, int nTopIndex = 0 );
		~CEditView();

		HWND GetWindow()	{ return m_hWndView; }
		HWND GetVScrollBar()	{ return m_hWndVScroll; }

		void Draw( HDC hDC, LPRECT prcClipBox, CSelection *pSel ) const;
		void SetPosition( LPRECT prcView, BOOL bReserveHSplitter, BOOL bReserveVSplitter );
		BOOL OnHScroll( HWND hWndScrollBar, UINT nSBCode, int nPos );
		BOOL OnVScroll( HWND hWndScrollBar, UINT nSBCode, int nPos );
		void OnFontChanged( HFONT hFont, int cxChar );

		void RecalcLayout( BOOL bReserveHSplitter, BOOL bReserveVSplitter );

		void GetRect( LPRECT prcView ) const;
		void GetViewRect( LPRECT prcView ) const;

		__forceinline int GetLeftIndex() const	{ return m_nLeftIndex; }
		__forceinline int GetTopIndex() const	{ return m_nTopIndex; }
		__forceinline int GetRightIndex( BOOL bFullyVisible ) const	{ return bFullyVisible ? m_nRightIndexFullyVisible : m_nRightIndex; }
		__forceinline int GetBottomIndex( BOOL bFullyVisible ) const	{ return bFullyVisible ? m_nBottomIndexFullyVisible : m_nBottomIndex; }
		
		BOOL LineIsVisible( int nLine ) const;

	private:
		void SetRightIndex( int nPos );
		void SetBottomIndex( int nPos );
		void RecalcIndexes();

	public:
		int GetLeftMargin( BOOL bIncludeLineNums, BOOL bIncludeTextMargin = FALSE ) const;

		__forceinline int GetLineHeight() const	{ return m_cyLine; }
		__forceinline int GetCharWidth() const	{ return m_cxChar; }

		void GetCharPosFromPoint( int x, int y, int &nCol, int &nRow, LPRECT prcChar ) const;
		void GetCharBoundingRect( int nCol, int nRow, LPRECT prcChar ) const;

		void ScrollTo( int nCol, int nRow, BOOL bSmooth = FALSE );
		void Scroll( int nNumCols, int nNumRows );
		BOOL ScrollIfNearBorder( int xPos, int yPos, BOOL bHideDragCaretOnScroll );
		enum { CXY_DRAG_SCROLL = 10, 
		       CX_DRAG_SCROLL_INCREMENT_SMALL = 3, 
			   CX_DRAG_SCROLL_INCREMENT_LARGE = 6, 
			   CY_DRAG_SCROLL_INCREMENT_SMALL = 2, 
			   CY_DRAG_SCROLL_INCREMENT_LARGE = 4 };

		void OnBeginEdit()	{ m_nEditLevel++; }
		void OnEndEdit();
		void DamageView( int nLineStart, int nLineEnd );
		void RepaintView( int nLineStart, int nLineEnd ) const;

		void ResetDamage();

		enum { MAXCOL = 3000, MAXROW = INT_MAX, CXY_MIN_VIEW = 20 };	
		void RecalcScrollInfo() const;

	private:

		enum { CX_DIVIDERLINE = 1,		// vert divider line thickness (pixels)
		       CY_DIVIDERLINE = 1 };	// horz divider line thickness (pixels)
		void DrawLine( int nLine, HDC hDC, int x, int y, int xDividerStart ) const;
		void DrawVDividerLine( HDC hDC, const RECT &rcView, int xPos ) const;
		void DrawLeftMarginImages( HDC hDC, BOOL bBookMark, BYTE byImages, int x, int y ) const;
		enum { LINEWIDTH_EXTRA = 1 };  // extra chars to pad in line number margin
		void DrawLineNumber( int nLine, int nRadix, HDC hDC, int x, int y ) const;

	private:

		HWND m_hWndVScroll;
		HWND m_hWndHScroll;
		HWND m_hWndView;
		CEdit *m_pCtrl;

		RECT m_rcAll;
		RECT m_rcView;
		CBuffer *m_pBuffer;

		int m_nLeftIndex;
		int m_nTopIndex;

		int m_nRightIndex;
		int m_nBottomIndex;

		int m_nRightIndexFullyVisible;
		int m_nBottomIndexFullyVisible;

		int m_cyLine;
		int m_cyLinePlusOverlap;
		int m_cxChar;

		int m_nLineDamageStart;
		int m_nLineDamageEnd;
		int m_nEditLevel;

		static BOOL m_bInit;
		static TCHAR m_szSpace[ CBuffer::MAX_TABSIZE + 1 ];
};

#endif