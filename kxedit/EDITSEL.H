#ifndef __EDITSEL_H__
#define __EDITSEL_H__

class CEditView;
class CEdit;

class CSelection
{
	public:
		CSelection();
		~CSelection();

		typedef enum { eUp, eDown, eLeft, eRight, eOutward } Direction;
		typedef enum { eChar, eWord, eWordEnd, ePage, eAll, eSmartAll, eSentence, eParagraph, eWindow } Amount;

		void Initialize( CEdit *pCtrl, CEditView *pActiveView );
		void OnFocusChange( BOOL bSetFocus );
		BOOL ShowCaret();
		BOOL HideCaret();
		void ResetCaret( HFONT hFont );
		int GetCaretWidth() const	{ return m_cxCaret; }
		void SetView( CEditView *pView, BOOL bSilent = FALSE );
		void SetExtendedSelection( int nStartCol, int nStartRow, int nEndCol, int nEndRow, BOOL bEnsureVisible = FALSE, BOOL bAllowDamage = TRUE );	// expressed in buffer positions
		void SetEmptySelection( int nCol, int nRow, BOOL bEnsureVisible = FALSE, BOOL bAllowDamage = TRUE );										// expressed in buffer positions
		void SetSelectionFromPoint( CEditView *View, int xPos, int yPos, BOOL bEmpty, BOOL bAllowLineSel );
		void Extend( Direction eDirection, Amount eAmount, BOOL bScrollIfNeccessary, BOOL bDamage, BOOL bAllowPastEndOfLine );
		void ExtendTo( int nCol, int nRow );
		void SelectWord();
		void Move( Direction eDirection, Amount eAmount, BOOL bScrollIfNeccessary );
		void EnsureVisible( BOOL bScrollIfNeccessary, BOOL bCanCenter = FALSE, BOOL bPreserveSelIfPossible = FALSE );
		void OnFontChanged( HFONT hFont );
		void UpdateCaretPosition() const;
		void GetCaretRect( CEditView *pView, int nBuffCol, int nRow, RECT &rcCaret );
		__forceinline BOOL IsEmpty() const	{ return ( m_nStartCol == m_nEndCol && m_nStartRow == m_nEndRow ); }
		void MakeEmpty( BOOL bDamage = TRUE, BOOL bAtEnd = TRUE );
		CEditView *GetView() const	{ return m_pView; }
		void Damage() const;

		__forceinline int GetStartRow() const { return m_nStartRow; }
		__forceinline int GetStartCol() const { return m_nStartCol; }
		__forceinline int GetStartViewCol() const  { return m_nStartViewCol; }
		__forceinline int GetEndRow() const { return m_nEndRow; }
		__forceinline int GetEndCol() const { return m_nEndCol; }
		__forceinline int GetEndViewCol() const { return m_nEndViewCol; }
		void GetBufferSelection( int &nStartCol, int &nStartRow, int &nEndCol, int &nEndRow ) const;
		void GetViewSelection( int &nStartCol, int &nStartRow, int &nEndCol, int &nEndRow ) const;
		void GetNormalizedBufferSelection( int &nStartCol, int &nStartRow, int &nEndCol, int &nEndRow ) const;
		void GetNormalizedViewSelection( int &nStartCol, int &nStartRow, int &nEndCol, int &nEndRow ) const;
		void GetNormalizedColumnSelection( int &nStartCol, int &nStartRow, int &nEndCol, int &nEndRow ) const;
		__forceinline BOOL IsColumnSel() const	{ return ( m_bColumnSel && !IsEmpty() ); }
		void EnableColumnSel( BOOL bEnable, BOOL bRaw = FALSE );
		BOOL EnforceSelBounds();
		BOOL BoundSelection() const;

	private:

		HWND m_hWnd;
		CEditView *m_pView;
		CEdit *m_pCtrl;

		int m_nStartRow;
		int m_nEndRow;

		int m_nStartCol;
		int m_nEndCol;
		int m_nEndViewColPreferred;

		int m_nStartViewCol;
		int m_nEndViewCol;

		int m_cxCaret : 16;
		int m_cyCaret : 16;
		int m_cxCaretIns : 16;

		BOOL m_bCaretVisible : 2;
		BOOL m_bColumnSel : 2;
};

#endif