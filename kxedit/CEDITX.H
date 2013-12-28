// ceditx.h : Declaration of the CEditX

#ifndef __CEDITX_H_
#define __CEDITX_H_

#include "resource.h"       // main symbols

class CEdit;
class CControlState;

#define DECLARE_AUT_BOOL_PROP_PUT( PROPNAME )\
HRESULT STDMETHODCALLTYPE put_##PROPNAME( VARIANT_BOOL vBool );

#define DECLARE_AUT_BOOL_PROP_GET( PROPNAME )\
HRESULT STDMETHODCALLTYPE get_##PROPNAME( VARIANT_BOOL __RPC_FAR *pBool );

#define DECLARE_AUT_BOOL_PROP_GET_PUT( PROPNAME )\
	DECLARE_AUT_BOOL_PROP_GET( PROPNAME )\
	DECLARE_AUT_BOOL_PROP_PUT( PROPNAME )

#define CATCH_ERROR_NO_HWND() if ( !::IsWindow( m_hWnd ) ) { if ( FAILED( Quick_n_DirtyActivate() ) ) return OLE_E_INVALIDHWND; }

#define IMPLEMENT_AUT_BOOL_PROP_PUT( DISPID, PROPNAME, MACRONAME ) \
HRESULT STDMETHODCALLTYPE CEditX::put_##PROPNAME( VARIANT_BOOL vBool )\
{\
	CATCH_ERROR_NO_HWND();\
	HRESULT hResult = CME2HRESULT( MACRONAME( m_hWnd, vBool != 0 ) );\
	OnChanged( DISPID );\
	return hResult;\
}

#define IMPLEMENT_AUT_BOOL_PROP_GET( PROPNAME, MACRONAME ) \
HRESULT STDMETHODCALLTYPE CEditX::get_##PROPNAME( VARIANT_BOOL __RPC_FAR *pBool )\
{\
	CATCH_ERROR_NO_HWND();\
	*pBool = ( VARIANT_BOOL ) ( MACRONAME( m_hWnd ) ? -1 : 0 );\
	return S_OK;\
}

#define IMPLEMENT_AUT_BOOL_PROP_GET_PUT( DISPID, PROPNAME, MACRONAMEGET, MACRONAMEPUT )\
IMPLEMENT_AUT_BOOL_PROP_PUT( DISPID, PROPNAME, MACRONAMEPUT )\
IMPLEMENT_AUT_BOOL_PROP_GET( PROPNAME, MACRONAMEGET )

EXTERN_C const IID IID_ICodeMaxEvents;

class CControlState
{
	public:
	CControlState();
	~CControlState();
	void Load( CEditX *pEditX );
	void Save( CEditX *pEditX );
	HRESULT Load( LPSTREAM pStm );
	HRESULT Save( CEditX *pCtrl, LPSTREAM pStm );
	void FreeStrings();

	BYTE m_bColorSyntax;
	BYTE m_bAllowHSplit;
	BYTE m_bAllowVSplit;
	BYTE m_bHScroll;
	BYTE m_bVScroll;
	BYTE m_bSmoothScroll;
	BYTE m_bLineToolTips;
	BYTE m_bShowLeftMargin;
	BYTE m_bAllowColumnSel;
	BYTE m_bAllowDragDrop;
	BYTE m_bExpandTabs;
	DWORD m_xPosHSplitter;
	DWORD m_yPosVSplitter;
	BYTE m_eIndentStyle;
	WORD m_nTabSize;
	BOOL m_bDisplayWhitespace;
	BOOL m_bWantCarriageReturn;
	BOOL m_bEnabled;
	BOOL m_bGlobalProps;
    BOOL m_bModified;
    BOOL m_bOverType;
    BOOL m_bReadOnly;
    BOOL m_bPreserveCase;
    BOOL m_bCaseSensitiveSearch;
    BOOL m_bWholeWordOnly;
    BOOL m_bRegExp;
    DWORD m_nMaxUndo;
	CM_COLORS m_Colors;
	CM_FONTSTYLES m_FontStyles;
	LOGFONT m_lf;
	BSTR m_bstrText;
	BSTR m_bstrLang;
	BOOL m_bSelBounds;
	CM_LINENUMBERING m_LineNum;
	BOOL m_bHideSel;
	BOOL m_bNormalizeCase;
	BOOL m_bOvertypeCaret;
	long m_nHighlightedLine;
	DWORD m_dwBorderStyle;
};

class CFontDisp : public IDispatch
{
	public:
	CFontDisp( CEditX *pEditX );
	~CFontDisp();
	
	HRESULT CFontDisp::GetFontDispatch( IFontDisp **ppIFont );

    public:
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject );
    virtual ULONG STDMETHODCALLTYPE AddRef( void )
		{ return m_pIFont->AddRef(); }
    virtual ULONG STDMETHODCALLTYPE Release( void );
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount( UINT __RPC_FAR *pctinfo )
		{ return m_pIFont->GetTypeInfoCount( pctinfo ); }
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo )
		{ return m_pIFont->GetTypeInfo( iTInfo, lcid, ppTInfo ); }
	virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( REFIID riid, LPOLESTR __RPC_FAR *rgszNames, UINT cNames,
	                                                 LCID lcid, DISPID __RPC_FAR *rgDispId )
		{ return m_pIFont->GetIDsOfNames( riid, rgszNames, cNames, lcid, rgDispId ); }
	virtual HRESULT STDMETHODCALLTYPE Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, 
	                                          WORD wFlags, DISPPARAMS __RPC_FAR *pDispParams,
	                                          VARIANT __RPC_FAR *pVarResult, EXCEPINFO __RPC_FAR *pExcepInfo,
	                                          UINT __RPC_FAR *puArgErr );

	IFontDisp *m_pIFont;
	CEditX *m_pEditX;
};

/////////////////////////////////////////////////////////////////////////////
// CEditX
class ATL_NO_VTABLE CEditX : 
	public CComObjectRoot,
	public CComCoClass<CEditX, &CLSID_CodeMax>,
	public CComControl<CEditX>,
	public IDispatchImpl<ICodeMax, &IID_ICodeMax, &LIBID_CodeMax, CM_X_VERSION>,
	public IProvideClassInfo2Impl<&CLSID_CodeMax, &IID_ICodeMaxEvents, &LIBID_CodeMax, CM_X_VERSION>,
	public IPersistStreamInitImpl<CEditX>,
	public IPersistStorageImpl<CEditX>,
	public IQuickActivateImpl<CEditX>,
	public IOleControlImpl<CEditX>,
	public IOleObjectImpl<CEditX>,
	public IOleInPlaceActiveObjectImpl<CEditX>,
	public IViewObjectExImpl<CEditX>,
	public IOleInPlaceObjectWindowlessImpl<CEditX>,
	public IDataObjectImpl<CEditX>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CEditX>,
	public IConnectionPointImpl<CEditX, &IID_ICodeMaxEvents>,
	public IPropertyNotifySinkCP<CEditX>,
	public ISpecifyPropertyPagesImpl<CEditX>
{
	friend class CControlState;
	friend class CFontDisp;
public:
	CEditX();
	~CEditX();

DECLARE_REGISTRY_RESOURCEID( IDR_CEDITX )

BEGIN_COM_MAP(CEditX)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(ICodeMax)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY_IMPL(IViewObjectEx)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IViewObject2, IViewObjectEx)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IViewObject, IViewObjectEx)
	COM_INTERFACE_ENTRY_IMPL(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IOleInPlaceObject, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY_IMPL(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY_IMPL(IOleControl)
	COM_INTERFACE_ENTRY_IMPL(IOleObject)
	COM_INTERFACE_ENTRY_IMPL(IQuickActivate)
	COM_INTERFACE_ENTRY_IMPL(IPersistStorage)
	COM_INTERFACE_ENTRY_IMPL(IPersistStreamInit)
	COM_INTERFACE_ENTRY_IMPL(IDataObject)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY_IMPL(ISpecifyPropertyPages)
END_COM_MAP()

BEGIN_PROPERTY_MAP(CEditX)
END_PROPERTY_MAP()


BEGIN_CONNECTION_POINT_MAP(CEditX)
	CONNECTION_POINT_ENTRY(IID_IPropertyNotifySink)
	CONNECTION_POINT_ENTRY(IID_ICodeMaxEvents)
END_CONNECTION_POINT_MAP()

// All messages are handled by CEdit
//BEGIN_MSG_MAP(CEditX)
//END_MSG_MAP()
	virtual BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
		LRESULT& lResult, DWORD dwMsgMapID = 0);

	HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, DWORD dwExStyle = 0,
			UINT nID = 0)
	{
		ATOM atom = GetWndClassInfo().Register(&m_pfnSuperWindowProc);
		return CWindowImplBase::Create(hWndParent, rcPos, szWindowName, dwStyle, dwExStyle,
			nID, atom);
	}

DECLARE_WND_SUPERCLASS( "XCodeMax", "CodeMax" );

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IViewObjectEx
	STDMETHOD(GetViewStatus)(DWORD* pdwStatus)
	{
		ATLTRACE(_T("IViewObjectExImpl::GetViewStatus\n"));
		*pdwStatus = VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE;
		return S_OK;
	}

// IPersistStream
	HRESULT IPersistStreamInit_Load( LPSTREAM pStm, ATL_PROPMAP_ENTRY* pMap );
	HRESULT IPersistStreamInit_Save( LPSTREAM pStm, BOOL fClearDirty, ATL_PROPMAP_ENTRY* pMap );

// IOleObject
	HRESULT IOleObject_SetExtent( DWORD dwDrawAspect, SIZEL *psizel );

// ICodeMax
public:
	HRESULT OnDraw(ATL_DRAWINFO& di);

	// ICodeMax Dispatch Properties
	DECLARE_AUT_BOOL_PROP_GET_PUT( ColorSyntax )
	DECLARE_AUT_BOOL_PROP_GET_PUT( DisplayWhitespace )
	DECLARE_AUT_BOOL_PROP_GET_PUT( ExpandTabs )
	DECLARE_AUT_BOOL_PROP_GET_PUT( SmoothScrolling )
	DECLARE_AUT_BOOL_PROP_GET_PUT( ReadOnly )
	DECLARE_AUT_BOOL_PROP_GET_PUT( LineToolTips )
	DECLARE_AUT_BOOL_PROP_GET_PUT( DisplayLeftMargin )
	DECLARE_AUT_BOOL_PROP_GET_PUT( EnableColumnSel )
	DECLARE_AUT_BOOL_PROP_GET_PUT( EnableDragDrop )
	DECLARE_AUT_BOOL_PROP_GET_PUT( Overtype )
	DECLARE_AUT_BOOL_PROP_GET_PUT( SearchCaseSensitive )
	DECLARE_AUT_BOOL_PROP_GET_PUT( ReplacePreserveCase )
	DECLARE_AUT_BOOL_PROP_GET_PUT( SearchWholeWord )
	DECLARE_AUT_BOOL_PROP_GET_PUT( SearchRegExp )
	DECLARE_AUT_BOOL_PROP_GET_PUT( Modified )
	DECLARE_AUT_BOOL_PROP_GET_PUT( EnableCRLF )
	DECLARE_AUT_BOOL_PROP_GET_PUT( GlobalProps )
	DECLARE_AUT_BOOL_PROP_GET_PUT( VScrollVisible )
	DECLARE_AUT_BOOL_PROP_GET_PUT( HScrollVisible )
	DECLARE_AUT_BOOL_PROP_GET_PUT( EnableHSplitter )
	DECLARE_AUT_BOOL_PROP_GET_PUT( EnableVSplitter )
	DECLARE_AUT_BOOL_PROP_GET_PUT( SelBounds )
	DECLARE_AUT_BOOL_PROP_GET_PUT( HideSel )
	DECLARE_AUT_BOOL_PROP_GET_PUT( NormalizeCase )
	DECLARE_AUT_BOOL_PROP_GET_PUT( OvertypeCaret )
	HRESULT STDMETHODCALLTYPE get_Window( long* phWnd );
	HRESULT STDMETHODCALLTYPE get_hWnd( long* phWnd );
	HRESULT STDMETHODCALLTYPE put_Enabled( VARIANT_BOOL vBool );
	HRESULT STDMETHODCALLTYPE get_Enabled( VARIANT_BOOL __RPC_FAR *pBool );
	HRESULT STDMETHODCALLTYPE put_Font( IFontDisp* pFont );
	HRESULT STDMETHODCALLTYPE putref_Font( IFontDisp* pFont );
	HRESULT STDMETHODCALLTYPE get_Font( IFontDisp** ppFont );
	HRESULT STDMETHODCALLTYPE put_Text( BSTR strText );
	HRESULT STDMETHODCALLTYPE get_Text( BSTR __RPC_FAR *pstrText );
	HRESULT STDMETHODCALLTYPE put_SelText( BSTR strText );
	HRESULT STDMETHODCALLTYPE get_SelText( BSTR __RPC_FAR *pstrText );
	HRESULT STDMETHODCALLTYPE put_Language( BSTR strLang );
	HRESULT STDMETHODCALLTYPE get_Language( BSTR __RPC_FAR *pstrLang );
	HRESULT STDMETHODCALLTYPE put_TabSize( long lTabSize );
	HRESULT STDMETHODCALLTYPE get_TabSize( long __RPC_FAR *plTabSize );
	HRESULT STDMETHODCALLTYPE put_TopIndex( long lTopIndex );
	HRESULT STDMETHODCALLTYPE get_TopIndex( long __RPC_FAR *plTopIndex );
	HRESULT STDMETHODCALLTYPE put_UndoLimit( long lLimit );
	HRESULT STDMETHODCALLTYPE get_UndoLimit( long __RPC_FAR *plLimit );
	HRESULT STDMETHODCALLTYPE get_CurrentView( long __RPC_FAR *plView );
	HRESULT STDMETHODCALLTYPE put_CurrentView( long lView );
	HRESULT STDMETHODCALLTYPE get_TextLength( long __RPC_FAR *plLen );
	HRESULT STDMETHODCALLTYPE get_TextLengthLogical( long __RPC_FAR *plLen );
	HRESULT STDMETHODCALLTYPE get_LineCount( long __RPC_FAR *plCount );
	HRESULT STDMETHODCALLTYPE get_CurrentWord( BSTR __RPC_FAR *pstrWord );
	HRESULT STDMETHODCALLTYPE get_CurrentWordLength( long __RPC_FAR *plLen );
	HRESULT STDMETHODCALLTYPE get_CurrentWordLengthLogical( long __RPC_FAR *plLen );
	HRESULT STDMETHODCALLTYPE put_AutoIndentMode( cmAutoIndentMode lMode );
	HRESULT STDMETHODCALLTYPE get_AutoIndentMode( cmAutoIndentMode __RPC_FAR *plMode );
	HRESULT STDMETHODCALLTYPE get_ViewCount( long __RPC_FAR *plCount );
	HRESULT STDMETHODCALLTYPE put_VSplitterPos( long lVal );
	HRESULT STDMETHODCALLTYPE get_VSplitterPos( long __RPC_FAR *plVal );
	HRESULT STDMETHODCALLTYPE put_HSplitterPos( long lVal );
	HRESULT STDMETHODCALLTYPE get_HSplitterPos( long __RPC_FAR *plVal );
	HRESULT STDMETHODCALLTYPE put_FindText( BSTR strVal );
	HRESULT STDMETHODCALLTYPE get_FindText( BSTR __RPC_FAR *pstrVal );
	HRESULT STDMETHODCALLTYPE put_ReplText( BSTR strVal );
	HRESULT STDMETHODCALLTYPE get_ReplText( BSTR __RPC_FAR *pstrVal );
	HRESULT STDMETHODCALLTYPE put_LineNumbering( VARIANT_BOOL bVal );
	HRESULT STDMETHODCALLTYPE get_LineNumbering( VARIANT_BOOL __RPC_FAR *pbVal );
	HRESULT STDMETHODCALLTYPE put_LineNumberStart( long lVal );
	HRESULT STDMETHODCALLTYPE get_LineNumberStart( long __RPC_FAR *plVal );
	HRESULT STDMETHODCALLTYPE put_LineNumberStyle( cmLineNumStyle lStyle );
	HRESULT STDMETHODCALLTYPE get_LineNumberStyle( cmLineNumStyle __RPC_FAR *plStyle );
	HRESULT STDMETHODCALLTYPE put_hImageList( OLE_HANDLE hImageList );
	HRESULT STDMETHODCALLTYPE get_hImageList( OLE_HANDLE __RPC_FAR *phImageList );
	HRESULT STDMETHODCALLTYPE get_ImageList( IDispatch __RPC_FAR *__RPC_FAR *ppImageList );
	HRESULT STDMETHODCALLTYPE put_ImageList( IDispatch __RPC_FAR *pImageList );
	HRESULT STDMETHODCALLTYPE putref_ImageList( IDispatch __RPC_FAR *pImageList );
	HRESULT STDMETHODCALLTYPE put_HighlightedLine( long lLine );
	HRESULT STDMETHODCALLTYPE get_HighlightedLine( long __RPC_FAR *plLine );
	HRESULT STDMETHODCALLTYPE get_SelLength( long __RPC_FAR *plLen );
	HRESULT STDMETHODCALLTYPE get_SelLengthLogical( long __RPC_FAR *plLen );
	HRESULT STDMETHODCALLTYPE put_BorderStyle( cmBorderStyle lBorderStyle );
	HRESULT STDMETHODCALLTYPE get_BorderStyle( cmBorderStyle __RPC_FAR *plBorderStyle );
	
	// ICodeMax Dispatch Methods
	HRESULT STDMETHODCALLTYPE GetWord( IPosition __RPC_FAR *pPos, BSTR __RPC_FAR *pstrWord );
	HRESULT STDMETHODCALLTYPE GetWordLength( IPosition __RPC_FAR *pPos, VARIANT_BOOL bLogical, long __RPC_FAR *plLen );
	HRESULT STDMETHODCALLTYPE GetTopIndex( long lView, long __RPC_FAR *plLine );
	HRESULT STDMETHODCALLTYPE SetTopIndex( long lView, long lLine );
	HRESULT STDMETHODCALLTYPE GetSel( VARIANT_BOOL bNormalized, IRange __RPC_FAR *__RPC_FAR *ppRange );
	HRESULT STDMETHODCALLTYPE SetSel( IRange __RPC_FAR *pRange, VARIANT_BOOL bMakeVisible );
	HRESULT STDMETHODCALLTYPE GetSelFromPoint( long lxClientPos, long lyClientPos, IPosition __RPC_FAR *__RPC_FAR *ppPos );
	HRESULT STDMETHODCALLTYPE GetVisibleLineCount( long lView, VARIANT_BOOL bFullyVisible, long __RPC_FAR *plCount );
	HRESULT STDMETHODCALLTYPE GetLine( long lLine, BSTR __RPC_FAR *pstrLine );
	HRESULT STDMETHODCALLTYPE GetLineLength( long lLine, long __RPC_FAR *plLen );
	HRESULT STDMETHODCALLTYPE SetColor( cmColorItem lColorItem, OLE_COLOR crColor );
	HRESULT STDMETHODCALLTYPE GetColor( cmColorItem lColorItem, OLE_COLOR __RPC_FAR *pcrColor );
	HRESULT STDMETHODCALLTYPE OpenFile( BSTR strFileName );
	HRESULT STDMETHODCALLTYPE SaveFile( BSTR strFileName, VARIANT_BOOL bClearUndo );
	HRESULT STDMETHODCALLTYPE InsertFile( BSTR strFileName, IPosition __RPC_FAR *pInsertPos );
	HRESULT STDMETHODCALLTYPE InsertText( BSTR strText, IPosition __RPC_FAR *pInsertPos );
	HRESULT STDMETHODCALLTYPE ReplaceText( BSTR strText, IRange __RPC_FAR *pRange );
	HRESULT STDMETHODCALLTYPE HitTest( long lxClientPos, long lyClientPos, long __RPC_FAR *plView, cmHitTestCode __RPC_FAR *plHitTestCode );
	HRESULT STDMETHODCALLTYPE AddText( BSTR strText );
	HRESULT STDMETHODCALLTYPE DeleteLine( long lLine );
	HRESULT STDMETHODCALLTYPE InsertLine( long lLine, BSTR strLine );
	HRESULT STDMETHODCALLTYPE SelectLine( long lLine, VARIANT_BOOL bMakeVisible );
	HRESULT STDMETHODCALLTYPE DeleteSel();
	HRESULT STDMETHODCALLTYPE ReplaceSel( BSTR strText );
	HRESULT STDMETHODCALLTYPE ExecuteCmd( cmCommand lCommand, VARIANT vData );
	HRESULT STDMETHODCALLTYPE CanUndo( VARIANT_BOOL __RPC_FAR *pbAnswer );
	HRESULT STDMETHODCALLTYPE CanRedo( VARIANT_BOOL __RPC_FAR *pbAnswer );
	HRESULT STDMETHODCALLTYPE CanCut( VARIANT_BOOL __RPC_FAR *pbAnswer );
	HRESULT STDMETHODCALLTYPE CanCopy( VARIANT_BOOL __RPC_FAR *pbAnswer );
	HRESULT STDMETHODCALLTYPE CanPaste( VARIANT_BOOL __RPC_FAR *pbAnswer );
	HRESULT STDMETHODCALLTYPE Undo();
	HRESULT STDMETHODCALLTYPE Redo();
	HRESULT STDMETHODCALLTYPE Cut();
	HRESULT STDMETHODCALLTYPE Copy();
	HRESULT STDMETHODCALLTYPE Paste();
	HRESULT STDMETHODCALLTYPE ClearUndoBuffer();
	HRESULT STDMETHODCALLTYPE IsRecordingMacro( VARIANT_BOOL __RPC_FAR *pbAnswer );
	HRESULT STDMETHODCALLTYPE IsPlayingMacro( VARIANT_BOOL __RPC_FAR *pbAnswer );
	HRESULT STDMETHODCALLTYPE SetDlgParent( long lhWnd );
	HRESULT STDMETHODCALLTYPE SetFontStyle( cmFontStyleItem lFontStyleItem, cmFontStyle byFontStyle );
	HRESULT STDMETHODCALLTYPE GetFontStyle( cmFontStyleItem lFontStyleItem, cmFontStyle __RPC_FAR *pbyFontStyle );
	HRESULT STDMETHODCALLTYPE SetItemData( long lLine, long lData );
	HRESULT STDMETHODCALLTYPE GetItemData( long lLine, long __RPC_FAR *plData );
	HRESULT STDMETHODCALLTYPE SetLineStyle( long lLine, long lStyle );
	HRESULT STDMETHODCALLTYPE GetLineStyle( long lLine, long __RPC_FAR *lStyle );
	HRESULT STDMETHODCALLTYPE SetBookmark( long lLine, VARIANT_BOOL bVal );
	HRESULT STDMETHODCALLTYPE GetBookmark( long lLine, VARIANT_BOOL __RPC_FAR *pbVal );
	HRESULT STDMETHODCALLTYPE SetAllBookmarks( SAFEARRAY __RPC_FAR * __RPC_FAR *ppLines );
	HRESULT STDMETHODCALLTYPE GetAllBookmarks( SAFEARRAY __RPC_FAR * __RPC_FAR *ppLines );
	HRESULT STDMETHODCALLTYPE PosFromChar( IPosition __RPC_FAR *pPos, IRect __RPC_FAR *__RPC_FAR *pprc );
	HRESULT STDMETHODCALLTYPE SetDivider( long lLine, VARIANT_BOOL bVal );
	HRESULT STDMETHODCALLTYPE GetDivider( long lLine, VARIANT_BOOL __RPC_FAR *pbVal );
	HRESULT STDMETHODCALLTYPE SetMarginImages( long lLine, BYTE byImages );
	HRESULT STDMETHODCALLTYPE GetMarginImages( long lLine, BYTE __RPC_FAR *pbyImages );
	HRESULT STDMETHODCALLTYPE AboutBox();
	HRESULT STDMETHODCALLTYPE PrintContents( long hDC, long lFlags );
	HRESULT STDMETHODCALLTYPE SetCaretPos( long lLine, long lCol );
	HRESULT STDMETHODCALLTYPE BufferColToViewCol( long lLine, long lBuffCol, long *plViewCol );
	HRESULT STDMETHODCALLTYPE ViewColToBufferCol( long lLine, long lViewCol, long *plBuffCol );

	HRESULT DoVerbProperties( LPCRECT prcPosRect, HWND hwndParent );
	BOOL FireControlEvent( int nEvent, LPNMHDR pnmhdr );
	BOOL IsInDesignMode() const;
	static void ResetAccelTable();
	void NotifySiteofAccelChange();
	void RaiseLastErrorException() const;
	void SetEdit( CEdit *pEdit );
	void OnFontChanged();

private:

	BOOL IsActiveXEvent( int nEvent ) const;
	void GetEventNotificationParams( int nEvent, LPNMHDR pnmhdr, VARIANTARG *pParams, int &nDispID, int &nArgs, VARIANT_BOOL *pbResult, BOOL &bHasRetVal ) const;

	// IOleControl
	STDMETHOD( GetControlInfo )( LPCONTROLINFO pCI );
	STDMETHOD( OnMnemonic )( LPMSG pMsg );

	// IOleInPlaceActiveObject
	STDMETHOD( TranslateAccelerator )(LPMSG pmsg );
	STDMETHOD( OnFrameWindowActivate )(BOOL bActivate );

	BOOL ControlSiteWantsAccelerator( LPMSG pmsg );
	void OnChanged( int nDispID );
	static void GetACCEL( LPACCEL pAccels, int &nAccel, const CHotKey *pHotKey );
	void SetImageListCtrl( BSTR bstr );
	HRESULT Quick_n_DirtyActivate();

	CEdit *m_pEdit;
	CControlState *m_pState;
	static int g_nRef;
	static HACCEL g_hAccel;
	static int g_nAccelCount;
	void FreeState();
	CFontDisp *m_pFont;
	HIMAGELIST m_hImageList;
	CComPtr<IDispatch> m_spImageList;
};

#endif //__CEDITX_H_
