#ifndef __CEDIT_H__
#define __CEDIT_H__

#define HAS_FLAG( val, flag ) ( ( val & flag ) == flag )
#define ARRAY_SIZE( array ) ( sizeof( array ) / sizeof( array[ 0 ] ) )

#include "buffer.h"
#include "editsel.h"
#include "editview.h"
#include "dragdrop.h"

#ifdef _CODEMAX_BETA
void DrawBetaBackground( HDC hDC, const RECT &rc, COLORREF crWindow );
#endif

#ifdef _ACTIVEX
class CEditX;
#endif

class CHotKey
{
	public:
	WORD wCmd;
	CM_HOTKEY cmHotKey;
};

BOOL operator==(const CM_HOTKEY &cmHK1, const CM_HOTKEY &cmHK2);

class CRegCmd
{
	public:
	WORD wCmd;									// caller-supplied unique command id
	TCHAR szName[ CM_MAX_CMD_STRING + 1 ];		// caller-supplied command name
	TCHAR szDesc[ CM_MAX_CMD_DESCRIPTION + 1 ];	// caller-supplied command description
};

// CFindReplaceData stores the info the user sees in the find and find/replace dialogs
class CFindReplaceData
{
	public:
		CFindReplaceData();
		~CFindReplaceData();

		BOOL m_bWholeWordOnly : 2;
		BOOL m_bPreserveCase : 2;
		BOOL m_bCaseSensitiveSearch : 2;
		BOOL m_bRegExp : 2;
		LPTSTR m_pszFindText;
		LPTSTR m_pszReplaceText;
		LPTSTR m_pszFindMRUList;
		LPTSTR m_pszReplaceMRUList;
};

// CViewSelState is used for before-and-after comparisons of edit actions for parent notification
class CViewSelState
{
	public:

	int m_nChangeLevel;
	int m_nLeftIndex;
	int m_nTopIndex;
	int m_nStartCol, m_nStartRow, m_nEndCol, m_nEndRow;
	BOOL m_bModified : 2;
};

// CLanguage fully represents one registered language
class CLanguage
{
	public:

	CLanguage( LPCTSTR pszName, CM_LANGUAGE *pLang );
	~CLanguage();
	int GetSize() const;
	TCHAR m_szName[ CM_MAX_LANGUAGE_NAME + 1 ];
	CM_LANGUAGE *m_pLang;
};

typedef struct {
			WORD  wCmd;
			BYTE  fsModifiers1;
			UINT  nVirtKey1;
			BYTE  fsModifiers2;
			UINT  nVirtKey2;
		} _defhotkeyrec;

typedef struct {
            HFONT hFont;
			int cxExtraSpacing;
			int cyDescentShift;
			int cyFont;
			} FONT_DATA;

#define DECLARE_MESSAGE_HANDLER( fn ) LRESULT fn( WPARAM wParam, LPARAM lParam )
#define MACRO_SIZE( pMacro ) ( * ( DWORD * ) ( pMacro + 2 ) + 6 )    // + 6 = (1 byte : version) + (1 byte : unicode) + (4 bytes : count)
#define CMD_SELFTEST	CMD_RESERVED3
#ifdef _UNICODE
#define CLIP_TEXT CF_UNICODETEXT
#else
#define CLIP_TEXT CF_TEXT
#endif

// CEdit is the main class which represents the CodeMax window.  A pointer to a CEdit object is stored
// as SetWindowLong( hWndCodeMax, 0 )
class CEdit
{
	friend class CHourGlass;
	friend class CDelayRepaint;
	friend class CSelTransaction;
	friend class CNoHideSel;
    friend class CEditView;

	friend LRESULT PASCAL EditCtrlWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	friend BOOL ProcessWindowMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID );
	friend BOOL CALLBACK RecordMacroDlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
	friend BOOL CALLBACK SaveMacroDlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
	friend BOOL CALLBACK FindDlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
	friend BOOL CALLBACK ReplaceDlgProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
	friend BOOL CALLBACK PageColorFontDlgProc( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
	friend BOOL CALLBACK PageKeyboardDlgProc( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
	friend BOOL CALLBACK PageTabsDlgProc( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
	friend BOOL CALLBACK PageMiscDlgProc( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
	friend class CDropTarget;
	friend class CPropInfo;
	friend class CControlState;
	friend void CEditView::DrawLine( int nLine, HDC hDC, int x, int y, int xDividerStart ) const;
	friend void CEditView::Draw( HDC hDC, LPRECT prcClipBox, CSelection *pSel ) const;

	public:

		CEdit(HWND hWnd);
		~CEdit();

		CEditView *GetActiveView() const;
		void SetActiveView( CEditView *pView );
		HWND GetWindow() const	{ return m_hWnd; }
		HWND GetDlgParent() const	{ return m_hWndDlgParent ? m_hWndDlgParent : m_hWnd; }
		__forceinline CBuffer *GetBuffer()	{ return &m_Buffer; }
		__forceinline CSelection *GetSelection()	{ return &m_Selection; }
		HFONT GetFont( BOOL bAsSuppliedByCaller ) const	{ ASSERT( m_font[ 0 ].hFont ); return bAsSuppliedByCaller ? m_hFont : m_font[ 0 ].hFont; }
		int GetCharWidth() const	{ return m_cxChar; }

#ifdef _ACTIVEX
		friend class CEditX;
		void SetEditX( CEditX *pEditX ) { m_pEditX = pEditX; }
#endif

		BOOL ShowHScrollBar() const	{ return HAS_FLAG( m_dwStyle, WS_HSCROLL ); }
		BOOL ShowVScrollBar() const	{ return HAS_FLAG( m_dwStyle, WS_VSCROLL ); }
		BOOL SmoothScrolling() const	{ return m_bSmoothScroll; }
		BOOL ShowLineToolTips() const	{ return m_bLineToolTips; }
		BOOL IsEnabled() const	{ return !HAS_FLAG( m_dwStyle, WS_DISABLED ); }
		BOOL BoundSelection() const	 { return m_bSelBounds; }
		void SetBoundSelection( BOOL bSelBounds ) { m_bSelBounds = bSelBounds; }
		void ReplaceSelection( LPCTSTR pszText, BOOL bExtendIfEmpty, BOOL bEnsureVisible = TRUE, BOOL bSelectWhenDone = FALSE );
		void DeleteSelection( BOOL bExtendIfEmpty, BOOL bAllowConsumption );
		void CopySelection( BOOL bCopyLineIfEmpty, BOOL bAppend ) const;
		BOOL CanCopy() const { return !m_Selection.IsEmpty(); }
		BOOL CanCut( BOOL bCopyLineIfEmpty ) const;
		void ChangeCaseOfSelection( BOOL bUpper );
		HGLOBAL GetClipboardText() const;
		BOOL SetClipboardText( HGLOBAL hMem ) const;
		void Repaint( BOOL bUpdateNow );
		void SafeUpdateWindow() const;
		BOOL LineIsValid( int nLine, BOOL bAllowConcatenation = FALSE ) const;
		BOOL RangeIsValid( CM_RANGE *pRange ) const;
		HGLOBAL GetSelectionText() const;
		static BOOL Register();
		static void Unregister();
		LRESULT NotifyParent( UINT unNotification, LPNMHDR pnmhdr = NULL ) const;
		LRESULT NotifyParentOfCmdFailure( DWORD dwErr ) const;
		LRESULT NotifyParentOfKeyMsg( UINT unNotification, WPARAM wParam ) const;
		LRESULT NotifyParentOfMouseMsg( UINT unNotification, WPARAM wParam, LPARAM lParam ) const;
		LRESULT NotifyParentOfFindWrapped( BOOL bForward ) const;
		void ProcessViewSelNotifications();
		void SaveViewSelState();
		void FlashCaret() const;
		void OnChangeLineSelection();
		void GuessCRLFStateFromText( LPCTSTR pszText );
		BOOL InOvertypeMode() const { return m_bOverType; }
		BOOL OvertypeCaret() const { return m_bOvertypeCaret; }
		__forceinline HIMAGELIST GetImageList() const { return m_hImageList; }
		__forceinline const CM_LINENUMBERING *GetLineNumbering() const { return &m_LineNum; }
		__forceinline BOOL ColumnSelAllowed() const { return m_bAllowColumnSel; }
		void AboutBox();

		// hittest
		typedef enum { eNowhere = CM_NOWHERE, eHSplitter = CM_HSPLITTER, eVSplitter = CM_VSPLITTER, eHVSplitter = CM_HVSPLITTER, eEditSpace = CM_EDITSPACE, eHScrollBar = CM_HSCROLLBAR, eVScrollBar = CM_VSCROLLBAR, eSizeBox = CM_SIZEBOX, eLeftMargin = CM_LEFTMARGIN } HitTestCode;
		HitTestCode HitTest( int xPos, int yPos, int &nView ) const;
		BOOL PtInSelection( int xPos, int yPos, BOOL bViewsMustBeSame ) const;

		// auto-indent
		typedef enum { eIndentOff = CM_INDENT_OFF, eIndentScope = CM_INDENT_SCOPE, eIndentPrev = CM_INDENT_PREVLINE } IndentStyle;

		// mode
		typedef enum { eIdle, eConstructing, eDestructing, eHSplitting, eVSplitting, eHVSplitting, eMacroPlayback, eMacroRecord, eDragSelecting, eLineSelecting, eDragAndDrop } Mode;
		__forceinline Mode GetMode() const	{ return m_eMode; }
		__forceinline void SetMode( Mode eMode )	{ m_eMode = eMode; }
		void CancelMode();

		enum { CXY_SPLITTER = 7, CXY_SPLITTER_DRAG = 5, SCOPE_RADIUS = 100 };

	private:

		void OnDragEnter( LPDATAOBJECT pIDataSource, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect );
		void OnDragOver( LPDATAOBJECT pIDataSource, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect );
		void OnDragLeave();
		BOOL OnDrop( LPDATAOBJECT pIDataSource, DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect );
		DWORD GetDropEffect( LPDATAOBJECT pIDataSource, DWORD grfKeyState, POINTL pt, CEditView *&pView, int &nBuffCol, int &nRow );
		BOOL IsDragAndDropping() const;
		void DoDragDrop();
		void DrawDragRect( const RECT &rc ) const;

	public:

		void DrawDragCaret( BOOL bMakeEmpty ) const;
		
	private:

		__forceinline BOOL ShowHourGlass() const	{ return ( m_nHourGlass > 0 ); }

	public:

		__forceinline BOOL DelayPaint() const		{ return ( m_nDelayPaint > 0 ); }

	private:

		BOOL IsSplitting() const;
		void BeginSplitting( HitTestCode eHitTest );
		void UpdateSplitter( int xPos, int yPos );
		void EndSplitting( int xPos, int yPos, BOOL bCommit );
		void DrawMovingSplitters() const;
		void DrawStaticSplitters( HDC hDC ) const;
		void RecalcViews();
		void DamageAllViews( int nStartLine, int nLastLine ) const;
		void DamageSelection( BOOL bUpdateNow ) const;
		void EndSelecting();

	private:

		enum { TIMER_DRAG_SELECT = 1,
		       TIMER_PAINT_CLEANUP = 2};
		__forceinline BOOL IsSelecting() const	{ return ( m_eMode == eDragSelecting ); }

	private:

		static LRESULT OnCreate( HWND hWnd, WPARAM wParam, LPARAM lParam );
		static void BufferCallback( LPARAM lParam, DWORD dwNotification, int nLine, int nChar, int nCount );
		void OnBufferNotification( DWORD dwNotification, int nLine, int nChar, int nCount );
		BOOL TranslateHotKey( WPARAM wParam );
		BOOL TranslateChar( TCHAR ch );
		void ExecuteCommand( WORD wCmd, DWORD dwData, BOOL bApplyDefaultParam, BOOL bForcePersistInMacro = FALSE );
		void ExecuteRegisteredCommand( WORD wCmd ) const;
		static BOOL IsRegisteredCommand( WORD wCmd );
		void ShowRightClickMenu( int x, int y );
		BOOL UpdateAutoIndentStack( int nLine, DWORD *pStack, int nStackCeil, int &nStackPos, BOOL bSkipInitialStartTokens, BOOL &bHasLeadingText, BOOL &bHasTrailingStartTokens, BOOL &bException );
		int CopyLeadingIndentation( int nLineSrc, int nLineDest, BOOL bIndentExtra, BOOL bAllowRemoveLeadingIndentation );
		void RemoveLeadingIndentation( int nLine );
		int IndentLineToScope( int nLine, BOOL &bScopeStart );
		void InsertCharLowLevel( LPCTSTR pszChar );

		#define NUM_FONT_VARIATIONS 5		// normal, bold, italic, b+i, underline

		void DeleteDisplayFonts();
		void SetDisplayFont( HFONT hFont, BOOL bFirstTime );
		void OnFontChanged();
		static void GenerateFontVariations( HDC hDC, const LOGFONT &lfBase, FONT_DATA *m_pFonts, int &cxChar );
		void RemapAnsiFontToKeyboardLocale();
	
	private:

		void InitializeWindow( LPCREATESTRUCT lpcs );
		void CreateOffScreenBitmap( int cx, int cy );
		void DeleteOffScreenBitmap();

		// all data members are packed together to optimize storage
		HWND m_hWnd;
		HWND m_hWndDlgParent;
		HWND m_hDlgRecord;
		HFONT m_hFont;
		FONT_DATA m_font[ NUM_FONT_VARIATIONS ]; // 0: normal, 1: bold, 2: italic, 3: bold + italic
		int m_cxChar;
		HDC m_hdcPaint;			// temp storage for quick paints
		HBITMAP m_hbmPaint;			// temp storage for quick paints
		HBITMAP m_hbmEndRecord;
		SIZE m_sizeOffScreenBmp;
		HIMAGELIST m_hImageList;	// image list of line left margin images

		DWORD m_dwStyle;
		CM_HOTKEY m_cmHotKeyPending;
		CDropTarget m_DropTarget;
		Mode m_eMode : 5;
		IndentStyle m_eIndentStyle : 5;
		RECT m_rcDelayPaint;
		RECT m_rcDragCaret;
		int m_nHourGlass : 3;
		int m_nDelayPaint : 3;
		int m_nSelTrans : 5;
		int m_nViews : 4;
		int m_nRepeatCount : 16; // max is 9999 (2 ^ 14 + 1 = 15 bits needed)
		int m_yPosVSplitter : 16;
		int m_xPosHSplitter : 16;
		int m_yPosVSplitterBegin : 16;	// for rollback
		int m_xPosHSplitterBegin : 16;	// for rollback
		int m_xPosDown : 16;
		int m_yPosDown : 16;
		int m_cbMacroRecordBuffer : 16;
		int m_nLineNumWidth : 8;
		int m_nCmdNestLevel : 8;
		int m_nHighlightedLine;
		int m_nLastEdittedLine;
		BOOL m_bAllowHSplit : 2;
		BOOL m_bAllowVSplit : 2;
		BOOL m_bAppendNextCut : 2;
		BOOL m_bEatKeyMsg : 2;
		BOOL m_bSkipTranslate : 2;
		BOOL m_bOverType : 2;
		BOOL m_bDisplayWhitespace : 2;
		BOOL m_bSmoothScroll : 2;
		BOOL m_bLineToolTips : 2;
		BOOL m_bShowLeftMargin : 2;
		BOOL m_bColorSyntax : 2;
		BOOL m_bAbortMacro : 2;
		BOOL m_bExpandTabs : 2;
		BOOL m_bAllowDragDrop : 2;
		BOOL m_bAllowColumnSel : 2;
		BOOL m_bGlobalProps : 2;
		BOOL m_bOwnFont : 2;
		BOOL m_bDroppedHere : 2;
		BOOL m_bSelBounds : 2;
		BOOL m_bHideSel : 2;
		BOOL m_bPaintTimerSet : 2;
		BOOL m_bOvertypeCaret : 2;
		LPBYTE m_pActiveMacro;
		LPBYTE m_pMacroRecordBuffer;
		WORD m_wCurrCmd;
		TCHAR m_chLeadByteIn;

		CBuffer m_Buffer;

		CEditView *m_Views[ 4 ];		// max of four views
		CEditView *m_pActiveView;

		CSelection m_Selection;			// the current selection
		CViewSelState m_stateBegin;
		TCHAR m_szLang[ CM_MAX_LANGUAGE_NAME + 1 ];
        
        // the colors of the edit control
		CM_COLORS m_Colors;

        // the font styles of the various token classes
		CM_FONTSTYLES m_FontStyles;

		// the line numbering options
		CM_LINENUMBERING m_LineNum;

#ifdef _ACTIVEX
		CEditX *m_pEditX;
#endif

		
		// all cursors are shared between all CodeMax windows
		static HCURSOR g_hcurHSplit;
		static HCURSOR g_hcurVSplit;
		static HCURSOR g_hcurHVSplit;
		static HCURSOR g_hcurRecord;
		static HCURSOR g_hcurLeftMargin;
		static int g_nRegCount;
		static CHotKey *g_pHotKeys;
		static int g_nHotKeyCount;
		static int g_nHotKeyAllocCount;
		static CRegCmd *g_pRegCmds;
		static int g_nRegCmdCount;
		static int g_nRegCmdAllocCount;
		static CLanguage **g_pLanguages;
		static int g_nLanguageCount;
		static HWND *g_phWnds;
		static LPBYTE g_Macros[ CM_MAX_MACROS ];
		static int g_nhWndCount;
		static HFONT g_hFontDefault;
		static CM_LANGUAGE g_LangCPP;
		static CM_LANGUAGE g_LangPascal;
		static CM_LANGUAGE g_LangBasic;
		static CM_LANGUAGE g_LangSQL;
		static CM_LANGUAGE g_LangJava;
		static CM_LANGUAGE g_LangHTML;
		static CM_LANGUAGE g_LangXML;
		static _defhotkeyrec g_DefHotKeys[];
		static int g_nDefHotKeyCount;

	public:

		static int g_dwVersion;
		static CFindReplaceData g_FindReplaceData;
		// quick uppercase conversion map for general use
		static TCHAR g_UpperConv[ 256 ];
		// quick map of isalpha/isnumberic/etc flags for general use
		static BYTE g_CharFlags[ 256 ];
		enum { CHARFLAG_ALPHA			= 0x01,
		       CHARFLAG_ALPHANUMERIC	= 0x02,
		       CHARFLAG_NUMERIC			= 0x04,
		       CHARFLAG_SPACE			= 0x08,
		       CHARFLAG_SYMBOLORPUNCT	= 0x10,
		       CHARFLAG_EOLN			= 0x20,
		       CHARFLAG_LEADBYTE		= 0x40,
		       CHARFLAG_TRAILBYTE		= 0x80
		     };

	private:
		void FindReplaceSelection();

	private:
		
		BOOL IsRecordingMacro() const;
		BOOL IsPlayingMacro() const;
		void AddMacroData( LPBYTE pData, int cbData );
		void GetMacroData( LPBYTE pData, int cbData );
		void DisplayRecordMacroDialog( BOOL bShow );
		BOOL PromptUserToSaveMacro( int &nMacro, CM_HOTKEY &cmHotKey );

		enum { MACRO_GROWBY = 50 };

	public:

		__forceinline BOOL ShowLeftMargin() const	{ return m_bShowLeftMargin; }
		int GetAdjacentBookmark( int nLine, BOOL bPrev ) const;

	private:

		__forceinline BOOL IsLineSelecting() const	{ return ( m_eMode == eLineSelecting ); }

	public:

		__forceinline BOOL UseColorSyntax() const		{ return m_bColorSyntax && m_Buffer.HasLanguage(); }
		HFONT GetTokenFont( CBuffer::LangToken eToken, long &cxExtraSpacing, long &cyDescentShift, FONT_DATA *pFonts ) const;
		HFONT GetLineNumberFont() const	{ return m_font[ m_FontStyles.byLineNumber ].hFont; }
		COLORREF GetLineNumberForeColor() const	{ return m_Colors.crLineNumber; }
		COLORREF GetLineNumberBackColor() const	{ return m_Colors.crLineNumberBk; }
		COLORREF GetHDividerLineColor() const	{ return m_Colors.crHDividerLines; }
		COLORREF GetVDividerLineColor() const	{ return m_Colors.crVDividerLines; }
		COLORREF GetTokenColor( CBuffer::LangToken eToken, BOOL bTextColor ) const;
		COLORREF GetWindowColor( BOOL bSafe ) const;
		COLORREF GetLeftMarginColor() const	 { return m_Colors.crLeftMargin; }
		COLORREF GetBookmarkColor() const;
		COLORREF GetBookmarkBkColor() const;
		__forceinline BOOL DisplayWhitespace() const	{ return m_bDisplayWhitespace; }
		__forceinline int GetHighlightedLine() const { return m_nHighlightedLine; }
		void SetHighlightedLine(int nLine, BOOL bRaw = FALSE);
		__forceinline COLORREF GetHighlightedLineColor() const	{ return m_Colors.crHighlightedLine; }
		__forceinline BOOL LineNumberingEnabled( int &nRadix ) const
			{ nRadix = m_LineNum.dwStyle; return m_LineNum.bEnabled; }
		__forceinline BOOL LineNumberingEnabled() const
			{ return m_LineNum.bEnabled; }
		int GetLineNumWidth() const { return m_nLineNumWidth; }
		void SetLineNumWidth( int nWidth ) { m_nLineNumWidth = nWidth; }
		int GetLineNumberStart() const { return m_LineNum.nStartAt; }

	private:

		void FindText( BOOL bForward );
		void FindSelectedWord( BOOL bForward );

	public:

		static BOOL RegisterCommand( WORD wCmd, LPCTSTR pszName, LPCTSTR pszDesc );
		static BOOL UnregisterCommand( WORD wCmd );
		static void SetDefaultHotKeys();
		static BOOL UnregisterHotKey( CM_HOTKEY &cmHotKey );
		static BOOL RegisterHotKey( CM_HOTKEY &cmHotKey, WORD wCmd );
		static BOOL FindHotKeysForCommand( WORD wCmd, CM_HOTKEY *pHotKeys );
		static void GetCommandString( WORD wCmd, BOOL bDescription, LPTSTR psz, int cb );
		static int GetHotKeys( LPBYTE pBuff );
		static BOOL SetHotKeys( const LPBYTE pBuff );
		static void NormalizeHotKey( CM_HOTKEY &cmHotKey );
		static int GetMacro( int nMacro, LPBYTE pMacroBuff );
		static int SetMacro( int nMacro, const LPBYTE pMacroBuff );
		int MessageBox( UINT unText, UINT unCaption, UINT unType ) const;
		int MessageBox( HWND hWndParent, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType ) const;
		int DialogBoxParam( HINSTANCE hInstance, UINT unDlg, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam ) const;
		void RestoreDlgPos( HWND hWndDlg, UINT unDlg );
		void SaveDlgPos( HWND hWndDlg, UINT unDlg );
	private:

		enum { HOTKEY_BLOCKSIZE = 20,
		       REGCMD_BLOCKSIZE = 5 };
		static BOOL LookupHotKey( CM_HOTKEY &cmHotKey, WORD &wCmd, int &nPosFound );

	public:

		static CME_CODE RegisterLanguage( LPCTSTR pszName, CM_LANGUAGE *pLang );
		static BOOL UnregisterLanguage( LPCTSTR pszName );
		static CME_CODE UnregisterAllLanguages();
		static int GetLanguageDef( LPCTSTR pszName, CM_LANGUAGE *pLang );

	public:
	
		BOOL Print( HDC hDC, DWORD dwFlags ) const;

	private:

		void PrintPageBorder( HDC hDC, DWORD dwFlags, FONT_DATA *pFonts, RECT &rc, int nPage, int nLineCount, LPCTSTR pszDateTime, int cyLine ) const;
		void PrintOneLine( HDC hDC, DWORD dwFlags, int nLine, int x, int y, FONT_DATA *pFonts, int cxChar, int cxLine, int nStartCol, int nEndCol ) const;

#ifdef _DEBUG
	private:
		
		HWND m_hWndTest;
		BOOL m_bSelfTest;
		RECT m_rcLastPosTest;
		void ValidateSymmetricProp( UINT uMsgGet, UINT uMsgSet, DWORD dwPropVal ) const;
		void DoSelfTest();
		void DoCursorSelfTest( int x, int y );

#endif

	private:

		DECLARE_MESSAGE_HANDLER( OnNcDestroy );
		DECLARE_MESSAGE_HANDLER( OnDestroy );
		DECLARE_MESSAGE_HANDLER( OnEraseBkGnd );
		DECLARE_MESSAGE_HANDLER( OnPaint );
		DECLARE_MESSAGE_HANDLER( OnLButtonDown );
		DECLARE_MESSAGE_HANDLER( OnRButtonDown );
		DECLARE_MESSAGE_HANDLER( OnMButtonDown );
		DECLARE_MESSAGE_HANDLER( OnLButtonUp );
		DECLARE_MESSAGE_HANDLER( OnRButtonUp );
		DECLARE_MESSAGE_HANDLER( OnMButtonUp );
		DECLARE_MESSAGE_HANDLER( OnLButtonDblClk );
		DECLARE_MESSAGE_HANDLER( OnRButtonDblClk );
		DECLARE_MESSAGE_HANDLER( OnMButtonDblClk );
		DECLARE_MESSAGE_HANDLER( OnMouseMove );
		DECLARE_MESSAGE_HANDLER( OnSize );
		DECLARE_MESSAGE_HANDLER( OnSetFont );
		DECLARE_MESSAGE_HANDLER( OnGetFont );
		DECLARE_MESSAGE_HANDLER( OnSetFocus );
		DECLARE_MESSAGE_HANDLER( OnKillFocus );
		DECLARE_MESSAGE_HANDLER( OnHScroll );
		DECLARE_MESSAGE_HANDLER( OnVScroll );
		DECLARE_MESSAGE_HANDLER( OnKeyDown );
		DECLARE_MESSAGE_HANDLER( OnKeyUp );
		DECLARE_MESSAGE_HANDLER( OnSysKeyDown );
		DECLARE_MESSAGE_HANDLER( OnSysKeyUp );
		DECLARE_MESSAGE_HANDLER( OnSysChar );
		DECLARE_MESSAGE_HANDLER( OnChar );
		DECLARE_MESSAGE_HANDLER( OnSetCursor );
		DECLARE_MESSAGE_HANDLER( OnTimer );
		DECLARE_MESSAGE_HANDLER( OnContextMenu );
		DECLARE_MESSAGE_HANDLER( OnMouseWheel );
		DECLARE_MESSAGE_HANDLER( OnEnable );
		DECLARE_MESSAGE_HANDLER( OnGetDlgCode );
		DECLARE_MESSAGE_HANDLER( OnSysColorChange );

		// custom messages
		DECLARE_MESSAGE_HANDLER( OnSetLanguage );
		DECLARE_MESSAGE_HANDLER( OnGetLanguage );
		DECLARE_MESSAGE_HANDLER( OnEnableColorSyntax );
		DECLARE_MESSAGE_HANDLER( OnIsColorSyntaxEnabled );
		DECLARE_MESSAGE_HANDLER( OnSetColors );
		DECLARE_MESSAGE_HANDLER( OnGetColors );
		DECLARE_MESSAGE_HANDLER( OnOpenFile );
		DECLARE_MESSAGE_HANDLER( OnInsertFile );
		DECLARE_MESSAGE_HANDLER( OnInsertText );
		DECLARE_MESSAGE_HANDLER( OnSetText );
		DECLARE_MESSAGE_HANDLER( OnEnableWhitespaceDisplay );
		DECLARE_MESSAGE_HANDLER( OnIsWhitespaceDisplayEnabled );
		DECLARE_MESSAGE_HANDLER( OnEnableTabExpand );
		DECLARE_MESSAGE_HANDLER( OnIsTabExpandEnabled );
		DECLARE_MESSAGE_HANDLER( OnSetWindowColor );
		DECLARE_MESSAGE_HANDLER( OnGetWindowColor );
		DECLARE_MESSAGE_HANDLER( OnSetLeftMarginColor );
		DECLARE_MESSAGE_HANDLER( OnGetLeftMarginColor );
		DECLARE_MESSAGE_HANDLER( OnEnableSmoothScrolling );
		DECLARE_MESSAGE_HANDLER( OnIsSmoothScrollingEnabled );
		DECLARE_MESSAGE_HANDLER( OnSetTabSize );
		DECLARE_MESSAGE_HANDLER( OnGetTabSize );
		DECLARE_MESSAGE_HANDLER( OnSetReadOnly );
		DECLARE_MESSAGE_HANDLER( OnIsReadOnly );
		DECLARE_MESSAGE_HANDLER( OnEnableLineToolTips );
		DECLARE_MESSAGE_HANDLER( OnIsLineToolTipsEnabled );
		DECLARE_MESSAGE_HANDLER( OnEnableLeftMargin );
		DECLARE_MESSAGE_HANDLER( OnIsLeftMarginEnabled );
		DECLARE_MESSAGE_HANDLER( OnSaveFile );
		DECLARE_MESSAGE_HANDLER( OnGetTextLength );
		DECLARE_MESSAGE_HANDLER( OnGetText );
		DECLARE_MESSAGE_HANDLER( OnGetLineCount );
		DECLARE_MESSAGE_HANDLER( OnGetLine );
		DECLARE_MESSAGE_HANDLER( OnGetLineLength );
		DECLARE_MESSAGE_HANDLER( OnGetWord );
		DECLARE_MESSAGE_HANDLER( OnGetWordLength );
		DECLARE_MESSAGE_HANDLER( OnAddText );
		DECLARE_MESSAGE_HANDLER( OnDeleteLine );
		DECLARE_MESSAGE_HANDLER( OnInsertLine );
		DECLARE_MESSAGE_HANDLER( OnGetSel );
		DECLARE_MESSAGE_HANDLER( OnSetSel );
		DECLARE_MESSAGE_HANDLER( OnDeleteSel );
		DECLARE_MESSAGE_HANDLER( OnReplaceSel );
		DECLARE_MESSAGE_HANDLER( OnExecuteCmd );
		DECLARE_MESSAGE_HANDLER( OnSetSplitterPos );
		DECLARE_MESSAGE_HANDLER( OnGetSplitterPos );
		DECLARE_MESSAGE_HANDLER( OnSetAutoIndentMode );
		DECLARE_MESSAGE_HANDLER( OnGetAutoIndentMode );
		DECLARE_MESSAGE_HANDLER( OnCanCut );
		DECLARE_MESSAGE_HANDLER( OnCanCopy );
		DECLARE_MESSAGE_HANDLER( OnCanPaste );
		DECLARE_MESSAGE_HANDLER( OnCut );
		DECLARE_MESSAGE_HANDLER( OnCopy );
		DECLARE_MESSAGE_HANDLER( OnPaste );
		DECLARE_MESSAGE_HANDLER( OnCanUndo );
		DECLARE_MESSAGE_HANDLER( OnCanRedo );
		DECLARE_MESSAGE_HANDLER( OnRedo );
		DECLARE_MESSAGE_HANDLER( OnUndo );
		DECLARE_MESSAGE_HANDLER( OnClearUndoBuffer );
		DECLARE_MESSAGE_HANDLER( OnSetUndoLimit );
		DECLARE_MESSAGE_HANDLER( OnGetUndoLimit );
		DECLARE_MESSAGE_HANDLER( OnIsModified );
		DECLARE_MESSAGE_HANDLER( OnSetModified );
		DECLARE_MESSAGE_HANDLER( OnEnableOvertype );
		DECLARE_MESSAGE_HANDLER( OnIsOvertypeEnabled );
		DECLARE_MESSAGE_HANDLER( OnEnableCaseSensitive );
		DECLARE_MESSAGE_HANDLER( OnIsCaseSensitiveEnabled );
		DECLARE_MESSAGE_HANDLER( OnEnablePreserveCase );
		DECLARE_MESSAGE_HANDLER( OnIsPreserveCaseEnabled );
		DECLARE_MESSAGE_HANDLER( OnEnableWholeWord );
		DECLARE_MESSAGE_HANDLER( OnIsWholeWordEnabled );
		DECLARE_MESSAGE_HANDLER( OnSetTopIndex );
		DECLARE_MESSAGE_HANDLER( OnGetTopIndex );
		DECLARE_MESSAGE_HANDLER( OnGetVisibleLineCount );
		DECLARE_MESSAGE_HANDLER( OnEnableCRLF );
		DECLARE_MESSAGE_HANDLER( OnIsCRLFEnabled );
		DECLARE_MESSAGE_HANDLER( OnSetFontOwnership );
		DECLARE_MESSAGE_HANDLER( OnGetFontOwnership );
		DECLARE_MESSAGE_HANDLER( OnGetViewCount );
		DECLARE_MESSAGE_HANDLER( OnGetCurrentView );
		DECLARE_MESSAGE_HANDLER( OnSetCurrentView );
		DECLARE_MESSAGE_HANDLER( OnShowScrollBar );
		DECLARE_MESSAGE_HANDLER( OnHasScrollBar );
		DECLARE_MESSAGE_HANDLER( OnGetSelFromPoint );
		DECLARE_MESSAGE_HANDLER( OnSelectLine );
		DECLARE_MESSAGE_HANDLER( OnHitTest );
		DECLARE_MESSAGE_HANDLER( OnEnableDragDrop );
		DECLARE_MESSAGE_HANDLER( OnIsDragDropEnabled );
		DECLARE_MESSAGE_HANDLER( OnEnableSplitter );
		DECLARE_MESSAGE_HANDLER( OnIsSplitterEnabled );
		DECLARE_MESSAGE_HANDLER( OnEnableColumnSel );
		DECLARE_MESSAGE_HANDLER( OnIsColumnSelEnabled );
		DECLARE_MESSAGE_HANDLER( OnEnableGlobalProps );
		DECLARE_MESSAGE_HANDLER( OnIsGlobalPropsEnabled );
		DECLARE_MESSAGE_HANDLER( OnIsRecordingMacro );
		DECLARE_MESSAGE_HANDLER( OnIsPlayingMacro );
		DECLARE_MESSAGE_HANDLER( OnSetDlgParent );
		DECLARE_MESSAGE_HANDLER( OnEnableSelBounds );
		DECLARE_MESSAGE_HANDLER( OnIsSelBoundsEnabled );
		DECLARE_MESSAGE_HANDLER( OnSetFontStyles );
		DECLARE_MESSAGE_HANDLER( OnGetFontStyles );
		DECLARE_MESSAGE_HANDLER( OnEnableRegExp );
		DECLARE_MESSAGE_HANDLER( OnIsRegExpEnabled );
		DECLARE_MESSAGE_HANDLER( OnSetItemData );
		DECLARE_MESSAGE_HANDLER( OnGetItemData );
		DECLARE_MESSAGE_HANDLER( OnSetLineStyle );
		DECLARE_MESSAGE_HANDLER( OnGetLineStyle );
		DECLARE_MESSAGE_HANDLER( OnSetBookmark );
		DECLARE_MESSAGE_HANDLER( OnGetBookmark );
		DECLARE_MESSAGE_HANDLER( OnSetAllBookmarks );
		DECLARE_MESSAGE_HANDLER( OnGetAllBookmarks );
		DECLARE_MESSAGE_HANDLER( OnSetLineNumbering );
		DECLARE_MESSAGE_HANDLER( OnGetLineNumbering );
		DECLARE_MESSAGE_HANDLER( OnPosFromChar );
		DECLARE_MESSAGE_HANDLER( OnEnableHideSel );
		DECLARE_MESSAGE_HANDLER( OnIsHideSelEnabled );
		DECLARE_MESSAGE_HANDLER( OnSetHighlightedLine );
		DECLARE_MESSAGE_HANDLER( OnGetHighlightedLine );
		DECLARE_MESSAGE_HANDLER( OnEnableNormalizeCase );
		DECLARE_MESSAGE_HANDLER( OnIsNormalizeCaseEnabled );
		DECLARE_MESSAGE_HANDLER( OnSetDivider );
		DECLARE_MESSAGE_HANDLER( OnGetDivider );
		DECLARE_MESSAGE_HANDLER( OnEnableOvertypeCaret );
		DECLARE_MESSAGE_HANDLER( OnIsOvertypeCaretEnabled );
		DECLARE_MESSAGE_HANDLER( OnSetFindText );
		DECLARE_MESSAGE_HANDLER( OnGetFindText );
		DECLARE_MESSAGE_HANDLER( OnSetReplaceText );
		DECLARE_MESSAGE_HANDLER( OnGetReplaceText );
		DECLARE_MESSAGE_HANDLER( OnSetImageList );
		DECLARE_MESSAGE_HANDLER( OnGetImageList );
		DECLARE_MESSAGE_HANDLER( OnSetMarginImages );
		DECLARE_MESSAGE_HANDLER( OnGetMarginImages );
		DECLARE_MESSAGE_HANDLER( OnAboutBox );
		DECLARE_MESSAGE_HANDLER( OnPrint );
		DECLARE_MESSAGE_HANDLER( OnSetCaretPos );
		DECLARE_MESSAGE_HANDLER( OnViewColToBufferCol );
		DECLARE_MESSAGE_HANDLER( OnBufferColToViewCol );
		DECLARE_MESSAGE_HANDLER( OnSetBorderStyle );
		DECLARE_MESSAGE_HANDLER( OnGetBorderStyle );

		LRESULT OnReplaceText( WPARAM wParam, LPARAM lParam, BOOL bSelectWhenDone = FALSE );

		void WordUpperCase();
		void WordTranspose();
		void WordRightExtend();
		void WordRight();
		void WordEndRightExtend();
		void WordEndRight();
		void WordLowerCase();						 
		void WordLeftExtend();
		void WordLeft();
		void WordEndLeftExtend();
		void WordEndLeft();
		void WordDeleteToStart();
		void WordDeleteToEnd();
		void WordCapitalize();
		void WindowStart();
		void WindowScrollUp();
		void WindowScrollToTop();
		void WindowScrollToCenter();
		void WindowScrollToBottom();
		void WindowScrollRight();
		void WindowScrollLeft();
		void WindowScrollDown();
		void WindowRightEdge();
		void WindowLeftEdge();
		void WindowEnd();
		void UpperCaseSelection();
		void UntabifySelection();
		void UnindentSelection();
		void UndoChanges();
		void Undo();
		void TabifySelection();
		void SentenceRight();
		void SentenceLeft();
		void SentenceCut();
		void SelectSwapAnchor();
		void SelectAll();
		void SelectPara();
		void SelectLine();
		void RedoChanges();
		void Redo();
		void Paste();
		void ParaUp();
		void ParaDown();
		void PageUpExtend();
		void PageUp();
		void PageDownExtend();
		void PageDown();
		void LowerCaseSelection();
		void LineUpExtend();
		void LineUp();
		void LineTranspose();
		void LineStart();
		void LineOpenBelow();
		void LineOpenAbove();
		void LineEndExtend();
		void LineEnd();
		void LineDownExtend();
		void LineDown();
		void LineDeleteToStart();
		void LineDeleteToEnd();
		void LineDelete( BOOL bCopy = FALSE );
		void LineCut();
		void IndentToPrev();
		void IndentSelection();
		void HomeExtend();
		void Home();
		void GoToMatchBrace();
		void GoToIndentation();
		void GoToLine( int nLine );
		void GoToLine( int nLine, BOOL bVCenter );
		void FindReplace();
		void Replace();
		void ReplaceAllInBuffer();
		void ReplaceAllInSelection();
		void FindPrevWord();
		void FindPrev();
		void FindNextWord();
		void FindNext();
		void Find();
		void FindMarkAll();
		void SetFindText( LPCTSTR psz );
		void SetReplaceText( LPCTSTR psz );
		void End();
		void ToggleWhitespaceDisplay();
		void ToggleOvertype();
		void ToggleWholeWord();
		void ToggleRegExp();
		void TogglePreserveCase();
		void ToggleCaseSensitive();
		void SetRepeatCount( int nCount );
		void DocumentStartExtend();
		void DocumentStart();
		void DocumentEndExtend();
		void DocumentEnd();
		void DeleteHorizontalSpace();
		void DeleteBlankLines();
		void DeleteBack();
		void Delete();
		void CutSelection();
		void Cut( BOOL bCopyLineIfEmpty = FALSE );
		void Copy( BOOL bCopyLineIfEmpty = FALSE, BOOL bAppend = FALSE );
		void CharTranspose();
		void CharRightExtend();
		void CharRight();
		void CharLeftExtend();
		void CharLeft();
		void BookmarkToggle();
		void BookmarkPrev();
		void BookmarkNext();
		void BookmarkDrop( int nBookmark = -1 );
		void BookmarkClearAll();
		void BookmarkJumpToFirst();
		void BookmarkJumpToLast();
		void AppendNextCut();
		void InsertChar( TCHAR ch = _T('\0') );
		void NewLine();
		void RecordMacro( BOOL bAbortRecording = FALSE );
		void PlayMacro( int nMacro );
		void Properties();
		void ClearSelection();
		void RegExpOn();
		void RegExpOff();
		void WholeWordOn();
		void WholeWordOff();
		void PreserveCaseOn();
		void PreserveCaseOff();
		void CaseSensitiveOn();
		void CaseSensitiveOff();
		void WhitespaceDisplayOn();
		void WhitespaceDisplayOff();
		void OvertypeOn();
		void OvertypeOff();
		void BeginUndo();
		void EndUndo();
};

class CSaveSelection
{
	public:

	CSaveSelection( CEdit *pCtrl );
	CSaveSelection( CEdit *pCtrl, BOOL bAllowDamage );
	~CSaveSelection();
	
	private:

	void Initialize( CEdit *pCtrl, BOOL bAllowDamage );
	CEdit *m_pCtrl;
	int m_nStartRow, m_nStartCol, m_nEndRow, m_nEndCol;
	BOOL m_bColumnSel : 2;
	BOOL m_bAllowDamage : 2;
};

class CHourGlass
{
	public:
	
	CHourGlass( CEdit *pCtrl );
	~CHourGlass();

	private:

	CEdit *m_pCtrl;
};

class CDelayRepaint
{
	public:
	
	CDelayRepaint( CEdit *pCtrl );
	~CDelayRepaint();

	private:

	CEdit *m_pCtrl;
};

class CEditTransaction
{
	public:
	
	CEditTransaction( CEdit *pCtrl );
	~CEditTransaction();

	private:

	CEdit *m_pCtrl;
};

class CSelTransaction
{
	public:
	
	CSelTransaction( CEdit *pCtrl );
	~CSelTransaction();

	private:

	CEdit *m_pCtrl;
};

class CNoHideSel
{
	public:
	
	CNoHideSel( CEdit *pCtrl );
	~CNoHideSel();

	private:

	CEdit *m_pCtrl;
	BOOL m_bOldState;
};

// All of the language IDs
#define LCID_ARABIC					0x0401
#define LCID_IRAQ					0x0801
#define LCID_EGYPTIAN				0x0c01
#define LCID_LIBYA					0x1001
#define LCID_ALGERIAN				0x1401
#define LCID_MOROCCO				0x1801
#define LCID_TUNISIA				0x1c01
#define LCID_OMAN					0x2001
#define LCID_YEMEN					0x2401
#define LCID_SYRIA					0x2801
#define LCID_JORDAN					0x2c01
#define LCID_LEBANON				0x3001
#define LCID_KUWAIT					0x3401
#define LCID_UAE					0x3801
#define LCID_BAHRAIN				0x3c01
#define LCID_QATAR					0x4001
#define LCID_BULGARIAN				0x0402
#define LCID_CATALAN				0x0403
#define LCID_TAIWAN					0x0404
#define LCID_CHINA					0x0804
#define LCID_HONGKONG				0x0c04
#define LCID_SINGAPORE				0x1004
#define LCID_MACAU					0x1404
#define LCID_CZECH					0x0405
#define LCID_DANISH					0x0406
#define LCID_GERMAN					0x0407
#define LCID_SWISSGERMAN			0x0807
#define LCID_AUSTRIANGERMAN			0x0c07
#define LCID_GERMANLUXEMBOURG		0x1007
#define LCID_GERMANLIECHTENSTEIN	0x1407
#define LCID_GREEK					0x0408
#define LCID_AMERICAN 				0x0409
#define LCID_BRITISH				0x0809
#define LCID_AUSTRALIAN				0x0c09
#define LCID_CANADIANENGLISH		0x1009
#define LCID_ENGLISHNEWZEALAND		0x1409
#define LCID_ENGLISHIRELAND			0x1809
#define LCID_ENGLISHSOUTHAFRICA		0x1c09
#define LCID_ENGLISHJAMAICA			0x2009
#define LCID_ENGLISHCARIBBEAN		0x2409
#define LCID_ENGLISHBELIZE			0x2809
#define LCID_ENGLISHTRINIDAD		0x2c09
#define LCID_ENGLISHZIMBABWE		0x3009
#define LCID_ENGLISHPHILIPPINES		0x3409
#define LCID_ENGLISHINDONESIA		0x3809
#define LCID_ENGLISHHONGKONG		0x3c09
#define LCID_ENGLISHINDIA			0x4009
#define LCID_ENGLISHMALAYSIA		0x4409
#define LCID_ENGLISHSINGAPORE		0x4809
#define LCID_SPANISH				0x040a
#define LCID_SPANISHCA				0x080a
#define LCID_MEXICAN				0x080a
#define LCID_SPANISHMODERN			0x0c0a
#define LCID_GUATEMALA				0x100a
#define LCID_COSTARICA				0x140a
#define LCID_PANAMA					0x180a
#define LCID_DOMINICANREPUBLIC		0x1c0a
#define LCID_SPANISHSA				0x200a
#define LCID_VENEZUELA				0x200a
#define LCID_COLOMBIA				0x240a
#define LCID_PERU					0x280a
#define LCID_ARGENTINA				0x2c0a
#define LCID_ECUADOR				0x300a
#define LCID_CHILE					0x340a
#define LCID_URUGUAY				0x380a
#define LCID_PARGUAY				0x3c0a
#define LCID_BOLIVIA				0x400a
#define LCID_ELSALVADOR				0x440a
#define LCID_HONDURAS				0x480a
#define LCID_NICARAGUA				0x4c0a
#define LCID_PUERTORICO				0x500a
#define LCID_FINNISH				0x040b
#define LCID_FRENCH					0x040c
#define LCID_FRENCHBELGIAN			0x080c
#define LCID_FRENCHCANADIAN			0x0c0c
#define LCID_FRENCHSWISS			0x100c
#define LCID_FRENCHLUXEMBOURG		0x140c
#define LCID_FRENCHMONACO			0x180c
#define LCID_FRENCHWESTINDIES		0x1c0c
#define LCID_FRENCHREUNION			0x200c
#define LCID_FRENCHZAIRE			0x240c
#define LCID_FRENCHSENEGAL			0x280c
#define LCID_FRENCHCAMEROON			0x2c0c
#define LCID_FRENCHCOTEDIVOIRE		0x300c
#define LCID_FRENCHMALI				0x340c
#define LCID_HEBREW					0x040d
#define LCID_HUNGARIAN				0x040e
#define LCID_ICELANDIC				0x040f
#define LCID_ITALIAN				0x0410
#define LCID_ITALIANSWISS			0x0810
#define LCID_JAPANESE				0x0411
#define LCID_KOREAN					0x0412
#define LCID_DUTCHPREFERRED			0x0013
#define LCID_DUTCH					0x0413
#define LCID_DUTCHBELGIAN			0x0813
#define LCID_NORSKBOKMAL			0x0414
#define LCID_NORSKNYNORSK			0x0814
#define LCID_POLISH					0x0415
#define LCID_PORTBRAZIL				0x0416
#define LCID_PORTIBERIAN			0x0816
#define LCID_RHAETOROMANIC			0x0417
#define LCID_ROMANIAN				0x0418
#define LCID_ROMANIANMOLDAVIA		0x0818
#define LCID_RUSSIAN				0x0419
#define LCID_RUSSIANMOLDAVIA		0x0819
#define LCID_CROATIAN				0x041a
#define LCID_SERBIANLATIN			0x081a
#define LCID_SERBIANCYRILLIC		0x0c1a
#define LCID_BOSNIAHERZEGOVINA		0x101a
#define LCID_SLOVAK					0x041b
#define LCID_ALBANIAN				0x041c
#define LCID_SWEDISH				0x041d
#define LCID_SWEDISHFINLAND			0x081d
#define LCID_THAI					0x041e
#define LCID_TURKISH				0x041f
#define LCID_URDU					0x0420
#define LCID_URDUINDIA				0x0820
#define LCID_INDONESIAN				0x0421
#define LCID_UKRAINIAN				0x0422
#define LCID_BYELORUSSIAN			0x0423
#define LCID_SLOVENIAN				0x0424
#define LCID_ESTONIAN				0x0425
#define LCID_LATVIAN				0x0426
#define LCID_LITHUANIAN				0x0427
#define LCID_LITHUANIANTRAD			0x0827
#define LCID_TAJIK					0x0428
#define LCID_FARSI					0x0429
#define LCID_VIETNAMESE				0x042a
#define LCID_ARMENIAN				0x042b
#define LCID_AZERILATIN				0x042c
#define LCID_AZERICYRILLIC			0x082c
#define LCID_BASQUE					0x042d
#define LCID_SORBIAN				0x042e
#define LCID_MACEDONIAN				0x042f
#define LCID_SUTU					0x0430
#define LCID_TSONGA					0x0431
#define LCID_TSWANA					0x0432
#define LCID_VENDA					0x0433
#define LCID_XHOSA					0x0434
#define LCID_ZULU					0x0435
#define LCID_AFRIKAANS				0x0436
#define LCID_GEORGIAN				0x0437
#define LCID_FAEROESE				0x0438
#define LCID_HINDI					0x0439
#define LCID_MALTESE				0x043a
#define LCID_SAMILAPPISH			0x043b
#define LCID_GAELICSCOTS			0x043c
#define LCID_GAELICIRISH			0x083c
#define LCID_YIDDISH				0x043d
#define LCID_MALAYSIAN				0x043e
#define LCID_MALAYBRUNEI			0x083e
#define LCID_KAZAKH					0x043f
#define LCID_KYRGYZ					0x0440
#define LCID_SWAHILI				0x0441
#define LCID_TURKMEN				0x0442
#define LCID_UZBEKLATIN				0x0443
#define LCID_UZBEKCYRILLIC			0x0843
#define LCID_TATAR					0x0444
#define LCID_BENGALI				0x0445
#define LCID_PUNJABI				0x0446
#define LCID_GUJARATI				0x0447
#define LCID_ORIYA					0x0448
#define LCID_TAMIL					0x0449
#define LCID_TELUGU					0x044a
#define LCID_KANNADA				0x044b
#define LCID_MALAYALAM				0x044c
#define LCID_ASSAMESE				0x044d
#define LCID_MARATHI				0x044e
#define LCID_SANSKRIT				0x044f
#define LCID_MONGOLIAN				0x0450
#define LCID_TIBETAN				0x0451
#define LCID_WELSH					0x0452
#define LCID_KHMER					0x0453
#define LCID_LAO					0x0454
#define LCID_BURMESE				0x0455
#define LCID_GALICIAN				0x0456
#define LCID_KONKANI				0x0457
#define LCID_MANIPURI				0x0458
#define LCID_SINDHI					0x0459
#define LCID_SINDHIPAKISTAN			0x0859
#define LCID_SYRIAC					0x045a
#define LCID_SINHALESE				0x045b
#define LCID_CHEROKEE				0x045c
#define LCID_INUKTITUT				0x045d
#define LCID_AMHARIC				0x045e
#define LCID_TAMAZIGHT				0x045f
#define LCID_TAMAZIGHTLATIN			0x085f
#define LCID_KASHMIRI				0x0460
#define LCID_KASHMIRIINDIA			0x0860
#define LCID_NEPALI					0x0461
#define LCID_NEPALIINDIA			0x0861
#define LCID_FRISIAN				0x0462
#define LCID_PASHTO					0x0463
#define LCID_FILIPINO				0x0464
#define LCID_MALDIVIAN				0x0465

#endif

