#pragma once

class CEditView;

BOOL RegisterLineToolTip();
BOOL UnregisterLineToolTip();
void StartLineToolTip( CEditView *pEditView );
void EndLineToolTip();

#define HAS_FLAG( val, flag ) ( ( val & flag ) == flag )
#define ARRAY_SIZE( array ) ( sizeof( array ) / sizeof( array[ 0 ] ) )

BOOL is_space( register TCHAR ch );
BOOL is_alphanumeric( register TCHAR ch );
BOOL is_alpha( register TCHAR ch );
BOOL is_numeric( register TCHAR ch );
BOOL is_symbolorpunct( register TCHAR ch );
BOOL is_eoln( TCHAR ch );
#ifdef _UNICODE
#define is_leadbyte( ch ) FALSE
#define is_trailbyte( ch ) FALSE
#define _tclen_prev( pszStart, psz ) 1
#define log_strlen _tclen
#else
BOOL is_leadbyte( TCHAR ch );
BOOL is_trailbyte( TCHAR ch );
__forceinline int _tclen_prev( LPCTSTR pszStart, LPCTSTR psz ) { return (int) (( psz - CharPrev( pszStart, psz ) ) / sizeof( TCHAR )); }
int log_strlen( LPCTSTR psz );
#endif

#define VIRTKEY_IS_ALPHA( wParam ) ( ( wParam ) == VK_TAB || ( wParam ) == _T('\n') || ( wParam ) >= 32 )

#ifdef _UNICODE
#define ISLEADBYTE_EXPR( ch ) FALSE
#define ISTRAILBYTE_EXPR( ch ) FALSE
#else
#define ISLEADBYTE_EXPR( ch ) ( _ismbblead( ch ) )
#define ISTRAILBYTE_EXPR( ch ) ( _ismbbtrail( ch ) )
#endif
#define ISSPACE_EXPR( ch ) ( _istspace( ch ) )
#define ISALNUM_EXPR( ch ) ( ( ch == _T('_') ) || IsCharAlphaNumeric( ch ) || ISLEADBYTE_EXPR( ch ) )
#define ISALPHA_EXPR( ch ) ( ( ch == _T('_') ) || IsCharAlpha( ch ) || ISLEADBYTE_EXPR( ch ) )
#define ISNUMERIC_EXPR( ch ) ( _istdigit( ch ) )
#define ISSYMBOLORPUNCT_EXPR( ch ) ( !( ( ch == _T('_') ) || IsCharAlphaNumeric( ch ) || ISLEADBYTE_EXPR( ch ) ) && !_istspace( ch ) )
#define ISEOLN_EXPR( ch ) ( ( ch == _T('\n') ) || ( ch == _T('\r') ) || ( ch == _T('\0') ) )

#define DBCS_ACTIVE ( _getmbcp() != 0 )

int compare_char_i( register TCHAR ch1, register TCHAR ch2 );
BOOL equal_char_i( register TCHAR ch1, register TCHAR ch2 );
void CharFill( LPTSTR pszBuff, TCHAR ch, int nCount );
int cmp_hotkeys( const CM_HOTKEY *p1, const CM_HOTKEY *p2 );
int _log( int nLine, int nRadix );
int _atoi( LPCTSTR pszLine, int nRadix );
LPCTSTR LoadStringPtr( UINT unID );
HGLOBAL NormalizeDialogTemplate( LPCTSTR lpszTemplateName );
void RestoreDlgPos( HWND hWndDlg, UINT unDlg );

// NormalizeLOGFONT() helps construct a 'good' LOGFONT::lfWidth based
// on a LOGFONT::lfHeight.  We cannot use 0 as a value, as this does
// not work well in a double-byte environment, because the characters 
// in a fixed-pitch font are no longer are spaced uniformly.  By
// specifying an explicit width, the characters are correctly spaced.
void NormalizeLOGFONT( LOGFONT &lf );

HGLOBAL LoadDialogTemplate( LPCTSTR lpszTemplateName );
void UnlockAndFreeDialogTemplate( HGLOBAL hTemplate );


#define DEF_COLORSYNTAX			FALSE
#define DEF_ALLOWHSPLIT			TRUE
#define DEF_ALLOWVSPLIT			TRUE
#define DEF_HSCROLL				TRUE
#define DEF_VSCROLL				TRUE
#define DEF_SMOOTHSCROLL		FALSE
#define DEF_LINETOOLTIPS		TRUE
#define DEF_SHOWLEFTMARGIN		TRUE
#define DEF_ALLOWCOLUMNSEL		( !DBCS_ACTIVE )
#define DEF_ALLOWDRAGDROP		TRUE
#define DEF_EXPANDTABS			FALSE
#define DEF_POSHSPLITTER		0
#define DEF_POSVSPLITTER		0
#define DEF_INDENTSTYLE			CEdit::eIndentPrev
#define DEF_TABSIZE				4
#define DEF_CLRWINDOW			CLR_INVALID
#define DEF_CLRLEFTMARGIN		RGB( 255, 255, 255 )
#define DEF_CLRBOOKMARK			CLR_INVALID
#define DEF_CLRBOOKMARKBK		CLR_INVALID
#define DEF_CLRTEXT				RGB( 0, 0, 0 )
#define DEF_CLRTEXTBK			CLR_INVALID
#define DEF_CLRNUMBER			RGB( 0, 0, 0 )
#define DEF_CLRNUMBERBK			CLR_INVALID
#define DEF_CLRKEYWORD			RGB( 0, 0, 255 )
#define DEF_CLRKEYWORDBK		CLR_INVALID
#define DEF_CLROPERATOR			RGB( 255, 0, 0 )
#define DEF_CLROPERATORBK		CLR_INVALID
#define DEF_CLRSCOPEKEYWORD		RGB( 0, 0, 255 )
#define DEF_CLRSCOPEKEYWORDBK	CLR_INVALID
#define DEF_CLRCOMMENT			RGB( 0, 128, 0 )
#define DEF_CLRCOMMENTBK		CLR_INVALID
#define DEF_CLRSTRING			RGB( 128, 0, 128 )
#define DEF_CLRSTRINGBK			CLR_INVALID
#define DEF_CLRTAGTEXT			RGB( 0, 0, 0 )
#define DEF_CLRTAGTEXTBK		CLR_INVALID
#define DEF_CLRTAGENT			RGB( 255, 0, 0 )
#define DEF_CLRTAGENTBK			CLR_INVALID
#define DEF_CLRTAGELEMNAME		RGB( 128, 0, 0 )
#define DEF_CLRTAGELEMNAMEBK	CLR_INVALID
#define DEF_CLRTAGATTRNAME		RGB( 0, 0, 255 )
#define DEF_CLRTAGATTRNAMEBK	CLR_INVALID
#define DEF_CLRLINENUMBER		RGB( 0, 0, 0 )
#define DEF_CLRLINENUMBERBK		RGB( 255, 255, 255 )
#define DEF_CLRHDIVIDERLINES	CLR_INVALID
#define DEF_CLRVDIVIDERLINES	CLR_INVALID
#define DEF_CLRHIGHLIGHTEDLINE	RGB( 255, 255, 0 )
#define DEF_DISPLAYWHITESPACE	FALSE
#define DEF_WANTCARRIAGERETURN	TRUE
#define DEF_ENABLED				TRUE
#define DEF_GLOBALPROPS			TRUE
#define DEF_MODIFIED			FALSE
#define DEF_OVERTYPE			FALSE
#define DEF_READONLY			FALSE
#define DEF_PRESERVECASE		FALSE
#define DEF_CASESENSITIVESEARCH	FALSE
#define DEF_WHOLEWORDONLY		FALSE
#define DEF_REGEXP				FALSE
#define DEF_SELBOUNDS			FALSE
#define DEF_MAXUNDO				-1
#define DEF_FSKEYWORD			CM_FONT_BOLD
#define DEF_FSCOMMENT			CM_FONT_ITALIC
#define DEF_FSOPERATOR			CM_FONT_NORMAL
#define DEF_FSSCOPEKEYWORD		CM_FONT_NORMAL
#define DEF_FSSTRING			CM_FONT_NORMAL
#define DEF_FSTEXT				CM_FONT_NORMAL
#define DEF_FSNUMBER			CM_FONT_NORMAL
#define DEF_FSTAGTEXT			CM_FONT_BOLD
#define DEF_FSTAGENT			CM_FONT_NORMAL
#define DEF_FSTAGELEMNAME		CM_FONT_BOLD
#define DEF_FSTAGATTRNAME		CM_FONT_NORMAL
#define DEF_FSLINENUMBER		CM_FONT_NORMAL
#define DEF_LINENUMENABLE		FALSE
#define DEF_LINENUMBERSTART		0
#define DEF_LINENUMBERSTYLE		CM_DECIMAL
#define DEF_LINENUMBERMINWIDTH	2
#define DEF_HIDESEL				TRUE
#define	DEF_NORMALIZECASE		FALSE
#define DEF_OVERTYPECARET		TRUE
#define DEF_HIGHLIGHTEDLINE		-1
#define DEF_BORDERSTYLE			CM_BORDER_NONE

// ActiveX helper functions
#ifdef _ACTIVEX
#define CME2HRESULT(x) ( ( x ) == CME_SUCCESS ? S_OK : E_FAIL )
LPTSTR BSTR2TSTR( BSTR str );
#endif

__inline int CMIsEqualGUID(REFGUID rguid1, REFGUID rguid2)
{
   return (
      ((unsigned long *) &rguid1)[0] == ((unsigned long *) &rguid2)[0] &&
      ((unsigned long *) &rguid1)[1] == ((unsigned long *) &rguid2)[1] &&
      ((unsigned long *) &rguid1)[2] == ((unsigned long *) &rguid2)[2] &&
      ((unsigned long *) &rguid1)[3] == ((unsigned long *) &rguid2)[3]);
}
