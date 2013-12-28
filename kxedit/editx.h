/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Wed Jan 02 02:02:14 2002
 */
/* Compiler settings for editx.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __editx_h__
#define __editx_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __CodeMax_FWD_DEFINED__
#define __CodeMax_FWD_DEFINED__

#ifdef __cplusplus
typedef class CodeMax CodeMax;
#else
typedef struct CodeMax CodeMax;
#endif /* __cplusplus */

#endif 	/* __CodeMax_FWD_DEFINED__ */


#ifndef __Range_FWD_DEFINED__
#define __Range_FWD_DEFINED__

#ifdef __cplusplus
typedef class Range Range;
#else
typedef struct Range Range;
#endif /* __cplusplus */

#endif 	/* __Range_FWD_DEFINED__ */


#ifndef __Position_FWD_DEFINED__
#define __Position_FWD_DEFINED__

#ifdef __cplusplus
typedef class Position Position;
#else
typedef struct Position Position;
#endif /* __cplusplus */

#endif 	/* __Position_FWD_DEFINED__ */


#ifndef __Language_FWD_DEFINED__
#define __Language_FWD_DEFINED__

#ifdef __cplusplus
typedef class Language Language;
#else
typedef struct Language Language;
#endif /* __cplusplus */

#endif 	/* __Language_FWD_DEFINED__ */


#ifndef __HotKey_FWD_DEFINED__
#define __HotKey_FWD_DEFINED__

#ifdef __cplusplus
typedef class HotKey HotKey;
#else
typedef struct HotKey HotKey;
#endif /* __cplusplus */

#endif 	/* __HotKey_FWD_DEFINED__ */


#ifndef __Globals_FWD_DEFINED__
#define __Globals_FWD_DEFINED__

#ifdef __cplusplus
typedef class Globals Globals;
#else
typedef struct Globals Globals;
#endif /* __cplusplus */

#endif 	/* __Globals_FWD_DEFINED__ */


#ifndef __Rect_FWD_DEFINED__
#define __Rect_FWD_DEFINED__

#ifdef __cplusplus
typedef class Rect Rect;
#else
typedef struct Rect Rect;
#endif /* __cplusplus */

#endif 	/* __Rect_FWD_DEFINED__ */


#ifndef __IRange_FWD_DEFINED__
#define __IRange_FWD_DEFINED__
typedef interface IRange IRange;
#endif 	/* __IRange_FWD_DEFINED__ */


#ifndef __IPosition_FWD_DEFINED__
#define __IPosition_FWD_DEFINED__
typedef interface IPosition IPosition;
#endif 	/* __IPosition_FWD_DEFINED__ */


#ifndef __ILanguage_FWD_DEFINED__
#define __ILanguage_FWD_DEFINED__
typedef interface ILanguage ILanguage;
#endif 	/* __ILanguage_FWD_DEFINED__ */


#ifndef __IHotKey_FWD_DEFINED__
#define __IHotKey_FWD_DEFINED__
typedef interface IHotKey IHotKey;
#endif 	/* __IHotKey_FWD_DEFINED__ */


#ifndef __IGlobals_FWD_DEFINED__
#define __IGlobals_FWD_DEFINED__
typedef interface IGlobals IGlobals;
#endif 	/* __IGlobals_FWD_DEFINED__ */


#ifndef __IRect_FWD_DEFINED__
#define __IRect_FWD_DEFINED__
typedef interface IRect IRect;
#endif 	/* __IRect_FWD_DEFINED__ */


#ifndef __ICodeMaxEvents_FWD_DEFINED__
#define __ICodeMaxEvents_FWD_DEFINED__
typedef interface ICodeMaxEvents ICodeMaxEvents;
#endif 	/* __ICodeMaxEvents_FWD_DEFINED__ */


#ifndef __ICodeMax_FWD_DEFINED__
#define __ICodeMax_FWD_DEFINED__
typedef interface ICodeMax ICodeMax;
#endif 	/* __ICodeMax_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_editx_0000 */
/* [local] */ 










extern RPC_IF_HANDLE __MIDL_itf_editx_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_editx_0000_v0_0_s_ifspec;


#ifndef __CodeMax_LIBRARY_DEFINED__
#define __CodeMax_LIBRARY_DEFINED__

/* library CodeMax */
/* [helpfile][helpstring][version][uuid] */ 

typedef /* [uuid] */ 
enum cmLimit
    {	cmMaxFindReplText	= 100,
	cmFindReplaceMRUMax	= 10,
	cmFindReplaceMRUBuffSize	= 1010,
	cmMaxMacros	= 10,
	cmMaxCmdString	= 50,
	cmMaxCmdDescription	= 100,
	cmMaxLanguageName	= 30,
	cmMaxTabSize	= 100,
	cmMinTabSize	= 2
    }	cmLimit;

typedef /* [uuid] */ 
enum cmCommand
    {	cmCmdWordUppercase	= 100,
	cmCmdWordTranspose	= 101,
	cmCmdWordRightExtend	= 102,
	cmCmdWordRight	= 103,
	cmCmdWordEndRight	= 104,
	cmCmdWordEndRightExtend	= 105,
	cmCmdWordLowerCase	= 106,
	cmCmdWordLeftExtend	= 107,
	cmCmdWordLeft	= 108,
	cmCmdWordEndLeft	= 109,
	cmCmdWordEndLeftExtend	= 110,
	cmCmdWordDeleteToStart	= 111,
	cmCmdWordDeleteToEnd	= 112,
	cmCmdWordCapitalize	= 113,
	cmCmdWindowStart	= 114,
	cmCmdWindowScrollUp	= 115,
	cmCmdWindowScrollToTop	= 116,
	cmCmdWindowScrollToCenter	= 117,
	cmCmdWindowScrollToBottom	= 118,
	cmCmdWindowScrollRight	= 119,
	cmCmdWindowScrollLeft	= 120,
	cmCmdWindowScrollDown	= 121,
	cmCmdWindowRightEdge	= 122,
	cmCmdWindowLeftEdge	= 123,
	cmCmdWindowEnd	= 124,
	cmCmdUppercaseSelection	= 125,
	cmCmdUntabifySelection	= 126,
	cmCmdUnindentSelection	= 127,
	cmCmdUndoChanges	= 128,
	cmCmdUndo	= 129,
	cmCmdTabifySelection	= 130,
	cmCmdSentenceRight	= 131,
	cmCmdSentenceLeft	= 132,
	cmCmdSentenceCut	= 133,
	cmCmdSelectSwapAnchor	= 134,
	cmCmdSelectPara	= 135,
	cmCmdSelectLine	= 136,
	cmCmdSelectAll	= 137,
	cmCmdRedoChanges	= 138,
	cmCmdRedo	= 139,
	cmCmdPaste	= 140,
	cmCmdParaUp	= 141,
	cmCmdParaDown	= 142,
	cmCmdPageUpExtend	= 143,
	cmCmdPageUp	= 144,
	cmCmdPageDownExtend	= 145,
	cmCmdPageDown	= 146,
	cmCmdLowercaseSelection	= 147,
	cmCmdLineUpExtend	= 148,
	cmCmdLineUp	= 149,
	cmCmdLineTranspose	= 150,
	cmCmdLineStart	= 151,
	cmCmdLineOpenBelow	= 152,
	cmCmdLineOpenAbove	= 153,
	cmCmdLineEndExtend	= 154,
	cmCmdLineEnd	= 155,
	cmCmdLineDownExtend	= 156,
	cmCmdLineDown	= 157,
	cmCmdLineDeleteToStart	= 158,
	cmCmdLineDeleteToEnd	= 159,
	cmCmdLineDelete	= 160,
	cmCmdLineCut	= 161,
	cmCmdIndentToPrev	= 162,
	cmCmdIndentSelection	= 163,
	cmCmdHomeExtend	= 164,
	cmCmdHome	= 165,
	cmCmdGotoMatchBrace	= 166,
	cmCmdGotoIndentation	= 167,
	cmCmdGotoLine	= 168,
	cmCmdFindReplace	= 169,
	cmCmdReplace	= 170,
	cmCmdReplaceAllInBuffer	= 171,
	cmCmdReplaceAllInSelection	= 172,
	cmCmdFindPrevWord	= 173,
	cmCmdFindPrev	= 174,
	cmCmdFindNextWord	= 175,
	cmCmdFindNext	= 176,
	cmCmdFindMarkAll	= 177,
	cmCmdFind	= 178,
	cmCmdSetFindText	= 179,
	cmCmdSetReplaceText	= 180,
	cmCmdTogglePreserveCase	= 181,
	cmCmdToggleWholeWord	= 182,
	cmCmdToggleCaseSensitive	= 183,
	cmCmdEnd	= 184,
	cmCmdToggleWhitespaceDisplay	= 185,
	cmCmdToggleOvertype	= 186,
	cmCmdSetRepeatCount	= 187,
	cmCmdDocumentStartExtend	= 188,
	cmCmdDocumentStart	= 189,
	cmCmdDocumentEndExtend	= 190,
	cmCmdDocumentEnd	= 191,
	cmCmdDeleteHorizontalSpace	= 192,
	cmCmdDeleteBlankLines	= 193,
	cmCmdDeleteBack	= 194,
	cmCmdDelete	= 195,
	cmCmdCutSelection	= 196,
	cmCmdCut	= 197,
	cmCmdCopy	= 198,
	cmCmdCharTranspose	= 199,
	cmCmdCharRightExtend	= 200,
	cmCmdCharRight	= 201,
	cmCmdCharLeftExtend	= 202,
	cmCmdCharLeft	= 203,
	cmCmdBookmarkToggle	= 204,
	cmCmdBookmarkPrev	= 205,
	cmCmdBookmarkNext	= 206,
	cmCmdBookmarkClearAll	= 207,
	cmCmdBookmarkJumpToFirst	= 208,
	cmCmdBookmarkJumpToLast	= 209,
	cmCmdAppendNextCut	= 210,
	cmCmdInsertChar	= 211,
	cmCmdNewLine	= 212,
	cmCmdRecordMacro	= 213,
	cmCmdPlayMacro1	= 214,
	cmCmdPlayMacro2	= 215,
	cmCmdPlayMacro3	= 216,
	cmCmdPlayMacro4	= 217,
	cmCmdPlayMacro5	= 218,
	cmCmdPlayMacro6	= 219,
	cmCmdPlayMacro7	= 220,
	cmCmdPlayMacro8	= 221,
	cmCmdPlayMacro9	= 222,
	cmCmdPlayMacro10	= 223,
	cmCmdProperties	= 224,
	cmCmdBeginUndo	= 225,
	cmCmdEndUndo	= 226,
	cmCmdToggleRegExp	= 228,
	cmCmdClearSelection	= 229,
	cmCmdRegExpOn	= 230,
	cmCmdRegExpOff	= 231,
	cmCmdWholeWordOn	= 232,
	cmCmdWholeWordOff	= 233,
	cmCmdPreserveCaseOn	= 234,
	cmCmdPreserveCaseOff	= 235,
	cmCmdCaseSensitiveOn	= 236,
	cmCmdCaseSensitiveOff	= 237,
	cmCmdWhitespaceDisplayOn	= 238,
	cmCmdWhitespaceDisplayOff	= 239,
	cmCmdOvertypeOn	= 240,
	cmCmdOvertypeOff	= 241
    }	cmCommand;

typedef /* [uuid] */ 
enum cmCommandErr
    {	cmErrFailure	= 1,
	cmErrInput	= 2,
	cmErrSelection	= 3,
	cmErrNotFound	= 4,
	cmErrEmptyBuf	= 5,
	cmErrReadOnly	= 6
    }	cmCommandErr;

typedef /* [uuid] */ 
enum cmColorItem
    {	cmClrWindow	= 0,
	cmClrLeftMargin	= 1,
	cmClrBookmark	= 2,
	cmClrBookmarkBk	= 3,
	cmClrText	= 4,
	cmClrTextBk	= 5,
	cmClrNumber	= 6,
	cmClrNumberBk	= 7,
	cmClrKeyword	= 8,
	cmClrKeywordBk	= 9,
	cmClrOperator	= 10,
	cmClrOperatorBk	= 11,
	cmClrScopeKeyword	= 12,
	cmClrScopeKeywordBk	= 13,
	cmClrComment	= 14,
	cmClrCommentBk	= 15,
	cmClrString	= 16,
	cmClrStringBk	= 17,
	cmClrTagText	= 18,
	cmClrTagTextBk	= 19,
	cmClrTagEntity	= 20,
	cmClrTagEntityBk	= 21,
	cmClrTagElementName	= 22,
	cmClrTagElementNameBk	= 23,
	cmClrTagAttributeName	= 24,
	cmClrTagAttributeNameBk	= 25,
	cmClrLineNumber	= 26,
	cmClrLineNumberBk	= 27,
	cmClrHDividerLines	= 28,
	cmClrVDividerLines	= 29,
	cmClrHighlightedLine	= 30
    }	cmColorItem;

typedef /* [uuid] */ 
enum cmFontStyleItem
    {	cmStyText	= 0,
	cmStyNumber	= 1,
	cmStyKeyword	= 2,
	cmStyOperator	= 3,
	cmStyScopeKeyword	= 4,
	cmStyComment	= 5,
	cmStyString	= 6,
	cmStyTagText	= 7,
	cmStyTagEntity	= 8,
	cmStyTagElementName	= 9,
	cmStyTagAttributeName	= 10,
	cmStyLineNumber	= 11
    }	cmFontStyleItem;

typedef /* [uuid] */ 
enum cmFontStyle
    {	cmFontNormal	= 0,
	cmFontBold	= 1,
	cmFontItalic	= 2,
	cmFontBoldItalic	= 3,
	cmFontUnderline	= 4
    }	cmFontStyle;

typedef /* [uuid] */ 
enum cmAutoIndentMode
    {	cmIndentOff	= 0,
	cmIndentScope	= 1,
	cmIndentPrevLine	= 2
    }	cmAutoIndentMode;

typedef /* [uuid] */ 
enum cmHitTestCode
    {	cmNowhere	= 0,
	cmHSplitter	= 1,
	cmVSplitter	= 2,
	cmHVSplitter	= 3,
	cmEditSpace	= 4,
	cmHScrollBar	= 5,
	cmVScrollBar	= 6,
	cmSizeBox	= 7,
	cmLeftMargin	= 8
    }	cmHitTestCode;

typedef /* [uuid] */ 
enum cmLineNumStyle
    {	cmDecimal	= 10,
	cmHexadecimal	= 16,
	cmBinary	= 2,
	cmOctal	= 8
    }	cmLineNumStyle;

typedef /* [uuid] */ 
enum cmPrintFlags
    {	cmPrnPromptDlg	= 0,
	cmPrnDefaultPrn	= 0x1,
	cmPrnHDC	= 0x2,
	cmPrnRichFonts	= 0x4,
	cmPrnColor	= 0x8,
	cmPrnPageNums	= 0x10,
	cmPrnDateTime	= 0x20,
	cmPrnBorderThin	= 0x40,
	cmPrnBorderThick	= 0x80,
	cmPrnBorderDouble	= 0x100,
	cmPrnSelection	= 0x200
    }	cmPrintFlags;

typedef /* [uuid] */ 
enum cmLangStyle
    {	cmLangStyleProcedural	= 0,
	cmLangStyleSGML	= 1
    }	cmLangStyle;

typedef /* [uuid] */ 
enum cmBorderStyle
    {	cmBorderNone	= 0,
	cmBorderThin	= 0x1,
	cmBorderClient	= 0x2,
	cmBorderStatic	= 0x4,
	cmBorderModal	= 0x8,
	cmBorderCorral	= 0xa
    }	cmBorderStyle;


EXTERN_C const IID LIBID_CodeMax;

EXTERN_C const CLSID CLSID_CodeMax;

#ifdef __cplusplus

class DECLSPEC_UUID("ECEDB941-AC41-11d2-AB20-000000000000")
CodeMax;
#endif

EXTERN_C const CLSID CLSID_Range;

#ifdef __cplusplus

class DECLSPEC_UUID("ECEDB94A-AC41-11d2-AB20-000000000000")
Range;
#endif

EXTERN_C const CLSID CLSID_Position;

#ifdef __cplusplus

class DECLSPEC_UUID("ECEDB944-AC41-11d2-AB20-000000000000")
Position;
#endif

EXTERN_C const CLSID CLSID_Language;

#ifdef __cplusplus

class DECLSPEC_UUID("ECEDB945-AC41-11d2-AB20-000000000000")
Language;
#endif

EXTERN_C const CLSID CLSID_HotKey;

#ifdef __cplusplus

class DECLSPEC_UUID("ECEDB946-AC41-11d2-AB20-000000000000")
HotKey;
#endif

EXTERN_C const CLSID CLSID_Globals;

#ifdef __cplusplus

class DECLSPEC_UUID("ECEDB948-AC41-11d2-AB20-000000000000")
Globals;
#endif

EXTERN_C const CLSID CLSID_Rect;

#ifdef __cplusplus

class DECLSPEC_UUID("8FF88AC1-2AEE-11d4-AE1E-000000000000")
Rect;
#endif
#endif /* __CodeMax_LIBRARY_DEFINED__ */

#ifndef __IRange_INTERFACE_DEFINED__
#define __IRange_INTERFACE_DEFINED__

/* interface IRange */
/* [unique][helpstring][helpcontext][dual][uuid][object] */ 


EXTERN_C const IID IID_IRange;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ECEDB952-AC41-11d2-AB20-000000000000")
    IRange : public IDispatch
    {
    public:
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_StartLineNo( 
            /* [in] */ long lVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_StartLineNo( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_StartColNo( 
            /* [in] */ long lVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_StartColNo( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_EndLineNo( 
            /* [in] */ long lVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_EndLineNo( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_EndColNo( 
            /* [in] */ long lVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_EndColNo( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_ColumnSel( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ColumnSel( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IsEmpty( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE Empty( void) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE Normalize( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRangeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IRange __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IRange __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IRange __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IRange __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IRange __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IRange __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IRange __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_StartLineNo )( 
            IRange __RPC_FAR * This,
            /* [in] */ long lVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_StartLineNo )( 
            IRange __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_StartColNo )( 
            IRange __RPC_FAR * This,
            /* [in] */ long lVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_StartColNo )( 
            IRange __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EndLineNo )( 
            IRange __RPC_FAR * This,
            /* [in] */ long lVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EndLineNo )( 
            IRange __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EndColNo )( 
            IRange __RPC_FAR * This,
            /* [in] */ long lVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EndColNo )( 
            IRange __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ColumnSel )( 
            IRange __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ColumnSel )( 
            IRange __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsEmpty )( 
            IRange __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Empty )( 
            IRange __RPC_FAR * This);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Normalize )( 
            IRange __RPC_FAR * This);
        
        END_INTERFACE
    } IRangeVtbl;

    interface IRange
    {
        CONST_VTBL struct IRangeVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRange_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRange_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IRange_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IRange_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IRange_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IRange_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IRange_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IRange_put_StartLineNo(This,lVal)	\
    (This)->lpVtbl -> put_StartLineNo(This,lVal)

#define IRange_get_StartLineNo(This,plVal)	\
    (This)->lpVtbl -> get_StartLineNo(This,plVal)

#define IRange_put_StartColNo(This,lVal)	\
    (This)->lpVtbl -> put_StartColNo(This,lVal)

#define IRange_get_StartColNo(This,plVal)	\
    (This)->lpVtbl -> get_StartColNo(This,plVal)

#define IRange_put_EndLineNo(This,lVal)	\
    (This)->lpVtbl -> put_EndLineNo(This,lVal)

#define IRange_get_EndLineNo(This,plVal)	\
    (This)->lpVtbl -> get_EndLineNo(This,plVal)

#define IRange_put_EndColNo(This,lVal)	\
    (This)->lpVtbl -> put_EndColNo(This,lVal)

#define IRange_get_EndColNo(This,plVal)	\
    (This)->lpVtbl -> get_EndColNo(This,plVal)

#define IRange_put_ColumnSel(This,bVal)	\
    (This)->lpVtbl -> put_ColumnSel(This,bVal)

#define IRange_get_ColumnSel(This,pbVal)	\
    (This)->lpVtbl -> get_ColumnSel(This,pbVal)

#define IRange_IsEmpty(This,pbAnswer)	\
    (This)->lpVtbl -> IsEmpty(This,pbAnswer)

#define IRange_Empty(This)	\
    (This)->lpVtbl -> Empty(This)

#define IRange_Normalize(This)	\
    (This)->lpVtbl -> Normalize(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE IRange_put_StartLineNo_Proxy( 
    IRange __RPC_FAR * This,
    /* [in] */ long lVal);


void __RPC_STUB IRange_put_StartLineNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE IRange_get_StartLineNo_Proxy( 
    IRange __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB IRange_get_StartLineNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE IRange_put_StartColNo_Proxy( 
    IRange __RPC_FAR * This,
    /* [in] */ long lVal);


void __RPC_STUB IRange_put_StartColNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE IRange_get_StartColNo_Proxy( 
    IRange __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB IRange_get_StartColNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE IRange_put_EndLineNo_Proxy( 
    IRange __RPC_FAR * This,
    /* [in] */ long lVal);


void __RPC_STUB IRange_put_EndLineNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE IRange_get_EndLineNo_Proxy( 
    IRange __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB IRange_get_EndLineNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE IRange_put_EndColNo_Proxy( 
    IRange __RPC_FAR * This,
    /* [in] */ long lVal);


void __RPC_STUB IRange_put_EndColNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE IRange_get_EndColNo_Proxy( 
    IRange __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB IRange_get_EndColNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE IRange_put_ColumnSel_Proxy( 
    IRange __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB IRange_put_ColumnSel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE IRange_get_ColumnSel_Proxy( 
    IRange __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB IRange_get_ColumnSel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IRange_IsEmpty_Proxy( 
    IRange __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer);


void __RPC_STUB IRange_IsEmpty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IRange_Empty_Proxy( 
    IRange __RPC_FAR * This);


void __RPC_STUB IRange_Empty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IRange_Normalize_Proxy( 
    IRange __RPC_FAR * This);


void __RPC_STUB IRange_Normalize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IRange_INTERFACE_DEFINED__ */


#ifndef __IPosition_INTERFACE_DEFINED__
#define __IPosition_INTERFACE_DEFINED__

/* interface IPosition */
/* [unique][helpstring][helpcontext][dual][uuid][object] */ 


EXTERN_C const IID IID_IPosition;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ECEDB953-AC41-11d2-AB20-000000000000")
    IPosition : public IDispatch
    {
    public:
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_LineNo( 
            /* [in] */ long lVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_LineNo( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_ColNo( 
            /* [in] */ long lVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ColNo( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPositionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IPosition __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IPosition __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IPosition __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IPosition __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IPosition __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IPosition __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IPosition __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LineNo )( 
            IPosition __RPC_FAR * This,
            /* [in] */ long lVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LineNo )( 
            IPosition __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ColNo )( 
            IPosition __RPC_FAR * This,
            /* [in] */ long lVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ColNo )( 
            IPosition __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        END_INTERFACE
    } IPositionVtbl;

    interface IPosition
    {
        CONST_VTBL struct IPositionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPosition_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPosition_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPosition_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPosition_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IPosition_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IPosition_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IPosition_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IPosition_put_LineNo(This,lVal)	\
    (This)->lpVtbl -> put_LineNo(This,lVal)

#define IPosition_get_LineNo(This,plVal)	\
    (This)->lpVtbl -> get_LineNo(This,plVal)

#define IPosition_put_ColNo(This,lVal)	\
    (This)->lpVtbl -> put_ColNo(This,lVal)

#define IPosition_get_ColNo(This,plVal)	\
    (This)->lpVtbl -> get_ColNo(This,plVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE IPosition_put_LineNo_Proxy( 
    IPosition __RPC_FAR * This,
    /* [in] */ long lVal);


void __RPC_STUB IPosition_put_LineNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE IPosition_get_LineNo_Proxy( 
    IPosition __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB IPosition_get_LineNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE IPosition_put_ColNo_Proxy( 
    IPosition __RPC_FAR * This,
    /* [in] */ long lVal);


void __RPC_STUB IPosition_put_ColNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE IPosition_get_ColNo_Proxy( 
    IPosition __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB IPosition_get_ColNo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPosition_INTERFACE_DEFINED__ */


#ifndef __ILanguage_INTERFACE_DEFINED__
#define __ILanguage_INTERFACE_DEFINED__

/* interface ILanguage */
/* [unique][helpstring][helpcontext][dual][uuid][object] */ 


EXTERN_C const IID IID_ILanguage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ECEDB954-AC41-11d2-AB20-000000000000")
    ILanguage : public IDispatch
    {
    public:
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_CaseSensitive( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_CaseSensitive( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Keywords( 
            /* [in] */ BSTR strVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Keywords( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Operators( 
            /* [in] */ BSTR strVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Operators( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_SingleLineComments( 
            /* [in] */ BSTR strVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_SingleLineComments( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_MultiLineComments1( 
            /* [in] */ BSTR strVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_MultiLineComments1( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_MultiLineComments2( 
            /* [in] */ BSTR strVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_MultiLineComments2( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_ScopeKeywords1( 
            /* [in] */ BSTR strVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ScopeKeywords1( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_ScopeKeywords2( 
            /* [in] */ BSTR strVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ScopeKeywords2( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_StringDelims( 
            /* [in] */ BSTR strVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_StringDelims( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_EscapeChar( 
            /* [in] */ BSTR strVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_EscapeChar( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_TerminatorChar( 
            /* [in] */ BSTR strVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_TerminatorChar( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_TagElementNames( 
            /* [in] */ BSTR strVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_TagElementNames( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_TagAttributeNames( 
            /* [in] */ BSTR strVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_TagAttributeNames( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_TagEntities( 
            /* [in] */ BSTR strVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_TagEntities( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Style( 
            /* [in] */ cmLangStyle Style) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Style( 
            /* [retval][out] */ cmLangStyle __RPC_FAR *pStyle) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILanguageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILanguage __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILanguage __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILanguage __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ILanguage __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ILanguage __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ILanguage __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ILanguage __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CaseSensitive )( 
            ILanguage __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CaseSensitive )( 
            ILanguage __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Keywords )( 
            ILanguage __RPC_FAR * This,
            /* [in] */ BSTR strVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Keywords )( 
            ILanguage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Operators )( 
            ILanguage __RPC_FAR * This,
            /* [in] */ BSTR strVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Operators )( 
            ILanguage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SingleLineComments )( 
            ILanguage __RPC_FAR * This,
            /* [in] */ BSTR strVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SingleLineComments )( 
            ILanguage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MultiLineComments1 )( 
            ILanguage __RPC_FAR * This,
            /* [in] */ BSTR strVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MultiLineComments1 )( 
            ILanguage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MultiLineComments2 )( 
            ILanguage __RPC_FAR * This,
            /* [in] */ BSTR strVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MultiLineComments2 )( 
            ILanguage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ScopeKeywords1 )( 
            ILanguage __RPC_FAR * This,
            /* [in] */ BSTR strVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ScopeKeywords1 )( 
            ILanguage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ScopeKeywords2 )( 
            ILanguage __RPC_FAR * This,
            /* [in] */ BSTR strVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ScopeKeywords2 )( 
            ILanguage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_StringDelims )( 
            ILanguage __RPC_FAR * This,
            /* [in] */ BSTR strVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_StringDelims )( 
            ILanguage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EscapeChar )( 
            ILanguage __RPC_FAR * This,
            /* [in] */ BSTR strVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EscapeChar )( 
            ILanguage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TerminatorChar )( 
            ILanguage __RPC_FAR * This,
            /* [in] */ BSTR strVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TerminatorChar )( 
            ILanguage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TagElementNames )( 
            ILanguage __RPC_FAR * This,
            /* [in] */ BSTR strVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TagElementNames )( 
            ILanguage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TagAttributeNames )( 
            ILanguage __RPC_FAR * This,
            /* [in] */ BSTR strVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TagAttributeNames )( 
            ILanguage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TagEntities )( 
            ILanguage __RPC_FAR * This,
            /* [in] */ BSTR strVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TagEntities )( 
            ILanguage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Style )( 
            ILanguage __RPC_FAR * This,
            /* [in] */ cmLangStyle Style);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Style )( 
            ILanguage __RPC_FAR * This,
            /* [retval][out] */ cmLangStyle __RPC_FAR *pStyle);
        
        END_INTERFACE
    } ILanguageVtbl;

    interface ILanguage
    {
        CONST_VTBL struct ILanguageVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILanguage_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILanguage_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILanguage_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILanguage_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ILanguage_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ILanguage_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ILanguage_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ILanguage_put_CaseSensitive(This,bVal)	\
    (This)->lpVtbl -> put_CaseSensitive(This,bVal)

#define ILanguage_get_CaseSensitive(This,pbVal)	\
    (This)->lpVtbl -> get_CaseSensitive(This,pbVal)

#define ILanguage_put_Keywords(This,strVal)	\
    (This)->lpVtbl -> put_Keywords(This,strVal)

#define ILanguage_get_Keywords(This,pstrVal)	\
    (This)->lpVtbl -> get_Keywords(This,pstrVal)

#define ILanguage_put_Operators(This,strVal)	\
    (This)->lpVtbl -> put_Operators(This,strVal)

#define ILanguage_get_Operators(This,pstrVal)	\
    (This)->lpVtbl -> get_Operators(This,pstrVal)

#define ILanguage_put_SingleLineComments(This,strVal)	\
    (This)->lpVtbl -> put_SingleLineComments(This,strVal)

#define ILanguage_get_SingleLineComments(This,pstrVal)	\
    (This)->lpVtbl -> get_SingleLineComments(This,pstrVal)

#define ILanguage_put_MultiLineComments1(This,strVal)	\
    (This)->lpVtbl -> put_MultiLineComments1(This,strVal)

#define ILanguage_get_MultiLineComments1(This,pstrVal)	\
    (This)->lpVtbl -> get_MultiLineComments1(This,pstrVal)

#define ILanguage_put_MultiLineComments2(This,strVal)	\
    (This)->lpVtbl -> put_MultiLineComments2(This,strVal)

#define ILanguage_get_MultiLineComments2(This,pstrVal)	\
    (This)->lpVtbl -> get_MultiLineComments2(This,pstrVal)

#define ILanguage_put_ScopeKeywords1(This,strVal)	\
    (This)->lpVtbl -> put_ScopeKeywords1(This,strVal)

#define ILanguage_get_ScopeKeywords1(This,pstrVal)	\
    (This)->lpVtbl -> get_ScopeKeywords1(This,pstrVal)

#define ILanguage_put_ScopeKeywords2(This,strVal)	\
    (This)->lpVtbl -> put_ScopeKeywords2(This,strVal)

#define ILanguage_get_ScopeKeywords2(This,pstrVal)	\
    (This)->lpVtbl -> get_ScopeKeywords2(This,pstrVal)

#define ILanguage_put_StringDelims(This,strVal)	\
    (This)->lpVtbl -> put_StringDelims(This,strVal)

#define ILanguage_get_StringDelims(This,pstrVal)	\
    (This)->lpVtbl -> get_StringDelims(This,pstrVal)

#define ILanguage_put_EscapeChar(This,strVal)	\
    (This)->lpVtbl -> put_EscapeChar(This,strVal)

#define ILanguage_get_EscapeChar(This,pstrVal)	\
    (This)->lpVtbl -> get_EscapeChar(This,pstrVal)

#define ILanguage_put_TerminatorChar(This,strVal)	\
    (This)->lpVtbl -> put_TerminatorChar(This,strVal)

#define ILanguage_get_TerminatorChar(This,pstrVal)	\
    (This)->lpVtbl -> get_TerminatorChar(This,pstrVal)

#define ILanguage_put_TagElementNames(This,strVal)	\
    (This)->lpVtbl -> put_TagElementNames(This,strVal)

#define ILanguage_get_TagElementNames(This,pstrVal)	\
    (This)->lpVtbl -> get_TagElementNames(This,pstrVal)

#define ILanguage_put_TagAttributeNames(This,strVal)	\
    (This)->lpVtbl -> put_TagAttributeNames(This,strVal)

#define ILanguage_get_TagAttributeNames(This,pstrVal)	\
    (This)->lpVtbl -> get_TagAttributeNames(This,pstrVal)

#define ILanguage_put_TagEntities(This,strVal)	\
    (This)->lpVtbl -> put_TagEntities(This,strVal)

#define ILanguage_get_TagEntities(This,pstrVal)	\
    (This)->lpVtbl -> get_TagEntities(This,pstrVal)

#define ILanguage_put_Style(This,Style)	\
    (This)->lpVtbl -> put_Style(This,Style)

#define ILanguage_get_Style(This,pStyle)	\
    (This)->lpVtbl -> get_Style(This,pStyle)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ILanguage_put_CaseSensitive_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ILanguage_put_CaseSensitive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ILanguage_get_CaseSensitive_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ILanguage_get_CaseSensitive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ILanguage_put_Keywords_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [in] */ BSTR strVal);


void __RPC_STUB ILanguage_put_Keywords_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ILanguage_get_Keywords_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrVal);


void __RPC_STUB ILanguage_get_Keywords_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ILanguage_put_Operators_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [in] */ BSTR strVal);


void __RPC_STUB ILanguage_put_Operators_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ILanguage_get_Operators_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrVal);


void __RPC_STUB ILanguage_get_Operators_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ILanguage_put_SingleLineComments_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [in] */ BSTR strVal);


void __RPC_STUB ILanguage_put_SingleLineComments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ILanguage_get_SingleLineComments_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrVal);


void __RPC_STUB ILanguage_get_SingleLineComments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ILanguage_put_MultiLineComments1_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [in] */ BSTR strVal);


void __RPC_STUB ILanguage_put_MultiLineComments1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ILanguage_get_MultiLineComments1_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrVal);


void __RPC_STUB ILanguage_get_MultiLineComments1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ILanguage_put_MultiLineComments2_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [in] */ BSTR strVal);


void __RPC_STUB ILanguage_put_MultiLineComments2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ILanguage_get_MultiLineComments2_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrVal);


void __RPC_STUB ILanguage_get_MultiLineComments2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ILanguage_put_ScopeKeywords1_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [in] */ BSTR strVal);


void __RPC_STUB ILanguage_put_ScopeKeywords1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ILanguage_get_ScopeKeywords1_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrVal);


void __RPC_STUB ILanguage_get_ScopeKeywords1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ILanguage_put_ScopeKeywords2_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [in] */ BSTR strVal);


void __RPC_STUB ILanguage_put_ScopeKeywords2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ILanguage_get_ScopeKeywords2_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrVal);


void __RPC_STUB ILanguage_get_ScopeKeywords2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ILanguage_put_StringDelims_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [in] */ BSTR strVal);


void __RPC_STUB ILanguage_put_StringDelims_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ILanguage_get_StringDelims_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrVal);


void __RPC_STUB ILanguage_get_StringDelims_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ILanguage_put_EscapeChar_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [in] */ BSTR strVal);


void __RPC_STUB ILanguage_put_EscapeChar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ILanguage_get_EscapeChar_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrVal);


void __RPC_STUB ILanguage_get_EscapeChar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ILanguage_put_TerminatorChar_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [in] */ BSTR strVal);


void __RPC_STUB ILanguage_put_TerminatorChar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ILanguage_get_TerminatorChar_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrVal);


void __RPC_STUB ILanguage_get_TerminatorChar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ILanguage_put_TagElementNames_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [in] */ BSTR strVal);


void __RPC_STUB ILanguage_put_TagElementNames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ILanguage_get_TagElementNames_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrVal);


void __RPC_STUB ILanguage_get_TagElementNames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ILanguage_put_TagAttributeNames_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [in] */ BSTR strVal);


void __RPC_STUB ILanguage_put_TagAttributeNames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ILanguage_get_TagAttributeNames_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrVal);


void __RPC_STUB ILanguage_get_TagAttributeNames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ILanguage_put_TagEntities_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [in] */ BSTR strVal);


void __RPC_STUB ILanguage_put_TagEntities_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ILanguage_get_TagEntities_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrVal);


void __RPC_STUB ILanguage_get_TagEntities_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ILanguage_put_Style_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [in] */ cmLangStyle Style);


void __RPC_STUB ILanguage_put_Style_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ILanguage_get_Style_Proxy( 
    ILanguage __RPC_FAR * This,
    /* [retval][out] */ cmLangStyle __RPC_FAR *pStyle);


void __RPC_STUB ILanguage_get_Style_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILanguage_INTERFACE_DEFINED__ */


#ifndef __IHotKey_INTERFACE_DEFINED__
#define __IHotKey_INTERFACE_DEFINED__

/* interface IHotKey */
/* [unique][helpstring][helpcontext][dual][uuid][object] */ 


EXTERN_C const IID IID_IHotKey;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ECEDB956-AC41-11d2-AB20-000000000000")
    IHotKey : public IDispatch
    {
    public:
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Modifiers1( 
            /* [in] */ long lVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Modifiers1( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_VirtKey1( 
            /* [in] */ BSTR strKey) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_VirtKey1( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrKey) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Modifiers2( 
            /* [in] */ long lVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Modifiers2( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_VirtKey2( 
            /* [in] */ BSTR strKey) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_VirtKey2( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrKey) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHotKeyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IHotKey __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IHotKey __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IHotKey __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IHotKey __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IHotKey __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IHotKey __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IHotKey __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Modifiers1 )( 
            IHotKey __RPC_FAR * This,
            /* [in] */ long lVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Modifiers1 )( 
            IHotKey __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_VirtKey1 )( 
            IHotKey __RPC_FAR * This,
            /* [in] */ BSTR strKey);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VirtKey1 )( 
            IHotKey __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrKey);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Modifiers2 )( 
            IHotKey __RPC_FAR * This,
            /* [in] */ long lVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Modifiers2 )( 
            IHotKey __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_VirtKey2 )( 
            IHotKey __RPC_FAR * This,
            /* [in] */ BSTR strKey);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VirtKey2 )( 
            IHotKey __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrKey);
        
        END_INTERFACE
    } IHotKeyVtbl;

    interface IHotKey
    {
        CONST_VTBL struct IHotKeyVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHotKey_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IHotKey_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IHotKey_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IHotKey_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IHotKey_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IHotKey_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IHotKey_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IHotKey_put_Modifiers1(This,lVal)	\
    (This)->lpVtbl -> put_Modifiers1(This,lVal)

#define IHotKey_get_Modifiers1(This,plVal)	\
    (This)->lpVtbl -> get_Modifiers1(This,plVal)

#define IHotKey_put_VirtKey1(This,strKey)	\
    (This)->lpVtbl -> put_VirtKey1(This,strKey)

#define IHotKey_get_VirtKey1(This,pstrKey)	\
    (This)->lpVtbl -> get_VirtKey1(This,pstrKey)

#define IHotKey_put_Modifiers2(This,lVal)	\
    (This)->lpVtbl -> put_Modifiers2(This,lVal)

#define IHotKey_get_Modifiers2(This,plVal)	\
    (This)->lpVtbl -> get_Modifiers2(This,plVal)

#define IHotKey_put_VirtKey2(This,strKey)	\
    (This)->lpVtbl -> put_VirtKey2(This,strKey)

#define IHotKey_get_VirtKey2(This,pstrKey)	\
    (This)->lpVtbl -> get_VirtKey2(This,pstrKey)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE IHotKey_put_Modifiers1_Proxy( 
    IHotKey __RPC_FAR * This,
    /* [in] */ long lVal);


void __RPC_STUB IHotKey_put_Modifiers1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE IHotKey_get_Modifiers1_Proxy( 
    IHotKey __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB IHotKey_get_Modifiers1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE IHotKey_put_VirtKey1_Proxy( 
    IHotKey __RPC_FAR * This,
    /* [in] */ BSTR strKey);


void __RPC_STUB IHotKey_put_VirtKey1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE IHotKey_get_VirtKey1_Proxy( 
    IHotKey __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrKey);


void __RPC_STUB IHotKey_get_VirtKey1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE IHotKey_put_Modifiers2_Proxy( 
    IHotKey __RPC_FAR * This,
    /* [in] */ long lVal);


void __RPC_STUB IHotKey_put_Modifiers2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE IHotKey_get_Modifiers2_Proxy( 
    IHotKey __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB IHotKey_get_Modifiers2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE IHotKey_put_VirtKey2_Proxy( 
    IHotKey __RPC_FAR * This,
    /* [in] */ BSTR strKey);


void __RPC_STUB IHotKey_put_VirtKey2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE IHotKey_get_VirtKey2_Proxy( 
    IHotKey __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrKey);


void __RPC_STUB IHotKey_get_VirtKey2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IHotKey_INTERFACE_DEFINED__ */


#ifndef __IGlobals_INTERFACE_DEFINED__
#define __IGlobals_INTERFACE_DEFINED__

/* interface IGlobals */
/* [unique][helpstring][helpcontext][dual][uuid][object] */ 


EXTERN_C const IID IID_IGlobals;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ECEDB958-AC41-11d2-AB20-000000000000")
    IGlobals : public IDispatch
    {
    public:
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_FindMRUList( 
            /* [in] */ BSTR strMRUList) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_FindMRUList( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrMRUList) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_ReplaceMRUList( 
            /* [in] */ BSTR strMRUList) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ReplaceMRUList( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrMRUList) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE RegisterLanguage( 
            /* [in] */ BSTR strName,
            /* [in] */ ILanguage __RPC_FAR *LanguageDef) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE UnregisterLanguage( 
            /* [in] */ BSTR strName) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetHotKeys( 
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *HotKeysBuff) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SetHotKeys( 
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *HotKeysBuff) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ResetDefaultHotKeys( void) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetHotKeyForCmd( 
            /* [in] */ cmCommand lCmd,
            /* [in] */ long lNum,
            /* [retval][out] */ IHotKey __RPC_FAR *__RPC_FAR *ppHotKey) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetNumHotKeysForCmd( 
            /* [in] */ cmCommand lCmd,
            /* [retval][out] */ long __RPC_FAR *plCount) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE RegisterHotKey( 
            /* [in] */ IHotKey __RPC_FAR *pHotKey,
            /* [in] */ cmCommand lCmd) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE UnregisterHotKey( 
            /* [in] */ IHotKey __RPC_FAR *pHotKey) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetCommandString( 
            /* [in] */ cmCommand lCmd,
            /* [in] */ VARIANT_BOOL bDescription,
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetMacro( 
            /* [in] */ long lMacro,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *MacroBuff) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SetMacro( 
            /* [in] */ long lMacro,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *MacroBuff) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE RegisterCommand( 
            /* [in] */ long lCmdId,
            /* [in] */ BSTR strName,
            /* [in] */ BSTR strDescription) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE UnregisterCommand( 
            /* [in] */ long lCmdId) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetLanguageDef( 
            /* [in] */ BSTR strName,
            /* [retval][out] */ ILanguage __RPC_FAR *__RPC_FAR *Language) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE UnregisterAllLanguages( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGlobalsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IGlobals __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IGlobals __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IGlobals __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IGlobals __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IGlobals __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IGlobals __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IGlobals __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FindMRUList )( 
            IGlobals __RPC_FAR * This,
            /* [in] */ BSTR strMRUList);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FindMRUList )( 
            IGlobals __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrMRUList);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ReplaceMRUList )( 
            IGlobals __RPC_FAR * This,
            /* [in] */ BSTR strMRUList);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReplaceMRUList )( 
            IGlobals __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrMRUList);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RegisterLanguage )( 
            IGlobals __RPC_FAR * This,
            /* [in] */ BSTR strName,
            /* [in] */ ILanguage __RPC_FAR *LanguageDef);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UnregisterLanguage )( 
            IGlobals __RPC_FAR * This,
            /* [in] */ BSTR strName);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetHotKeys )( 
            IGlobals __RPC_FAR * This,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *HotKeysBuff);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetHotKeys )( 
            IGlobals __RPC_FAR * This,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *HotKeysBuff);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ResetDefaultHotKeys )( 
            IGlobals __RPC_FAR * This);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetHotKeyForCmd )( 
            IGlobals __RPC_FAR * This,
            /* [in] */ cmCommand lCmd,
            /* [in] */ long lNum,
            /* [retval][out] */ IHotKey __RPC_FAR *__RPC_FAR *ppHotKey);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetNumHotKeysForCmd )( 
            IGlobals __RPC_FAR * This,
            /* [in] */ cmCommand lCmd,
            /* [retval][out] */ long __RPC_FAR *plCount);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RegisterHotKey )( 
            IGlobals __RPC_FAR * This,
            /* [in] */ IHotKey __RPC_FAR *pHotKey,
            /* [in] */ cmCommand lCmd);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UnregisterHotKey )( 
            IGlobals __RPC_FAR * This,
            /* [in] */ IHotKey __RPC_FAR *pHotKey);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCommandString )( 
            IGlobals __RPC_FAR * This,
            /* [in] */ cmCommand lCmd,
            /* [in] */ VARIANT_BOOL bDescription,
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMacro )( 
            IGlobals __RPC_FAR * This,
            /* [in] */ long lMacro,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *MacroBuff);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetMacro )( 
            IGlobals __RPC_FAR * This,
            /* [in] */ long lMacro,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *MacroBuff);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RegisterCommand )( 
            IGlobals __RPC_FAR * This,
            /* [in] */ long lCmdId,
            /* [in] */ BSTR strName,
            /* [in] */ BSTR strDescription);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UnregisterCommand )( 
            IGlobals __RPC_FAR * This,
            /* [in] */ long lCmdId);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetLanguageDef )( 
            IGlobals __RPC_FAR * This,
            /* [in] */ BSTR strName,
            /* [retval][out] */ ILanguage __RPC_FAR *__RPC_FAR *Language);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UnregisterAllLanguages )( 
            IGlobals __RPC_FAR * This);
        
        END_INTERFACE
    } IGlobalsVtbl;

    interface IGlobals
    {
        CONST_VTBL struct IGlobalsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGlobals_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IGlobals_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IGlobals_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IGlobals_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IGlobals_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IGlobals_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IGlobals_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IGlobals_put_FindMRUList(This,strMRUList)	\
    (This)->lpVtbl -> put_FindMRUList(This,strMRUList)

#define IGlobals_get_FindMRUList(This,pstrMRUList)	\
    (This)->lpVtbl -> get_FindMRUList(This,pstrMRUList)

#define IGlobals_put_ReplaceMRUList(This,strMRUList)	\
    (This)->lpVtbl -> put_ReplaceMRUList(This,strMRUList)

#define IGlobals_get_ReplaceMRUList(This,pstrMRUList)	\
    (This)->lpVtbl -> get_ReplaceMRUList(This,pstrMRUList)

#define IGlobals_RegisterLanguage(This,strName,LanguageDef)	\
    (This)->lpVtbl -> RegisterLanguage(This,strName,LanguageDef)

#define IGlobals_UnregisterLanguage(This,strName)	\
    (This)->lpVtbl -> UnregisterLanguage(This,strName)

#define IGlobals_GetHotKeys(This,HotKeysBuff)	\
    (This)->lpVtbl -> GetHotKeys(This,HotKeysBuff)

#define IGlobals_SetHotKeys(This,HotKeysBuff)	\
    (This)->lpVtbl -> SetHotKeys(This,HotKeysBuff)

#define IGlobals_ResetDefaultHotKeys(This)	\
    (This)->lpVtbl -> ResetDefaultHotKeys(This)

#define IGlobals_GetHotKeyForCmd(This,lCmd,lNum,ppHotKey)	\
    (This)->lpVtbl -> GetHotKeyForCmd(This,lCmd,lNum,ppHotKey)

#define IGlobals_GetNumHotKeysForCmd(This,lCmd,plCount)	\
    (This)->lpVtbl -> GetNumHotKeysForCmd(This,lCmd,plCount)

#define IGlobals_RegisterHotKey(This,pHotKey,lCmd)	\
    (This)->lpVtbl -> RegisterHotKey(This,pHotKey,lCmd)

#define IGlobals_UnregisterHotKey(This,pHotKey)	\
    (This)->lpVtbl -> UnregisterHotKey(This,pHotKey)

#define IGlobals_GetCommandString(This,lCmd,bDescription,pstrVal)	\
    (This)->lpVtbl -> GetCommandString(This,lCmd,bDescription,pstrVal)

#define IGlobals_GetMacro(This,lMacro,MacroBuff)	\
    (This)->lpVtbl -> GetMacro(This,lMacro,MacroBuff)

#define IGlobals_SetMacro(This,lMacro,MacroBuff)	\
    (This)->lpVtbl -> SetMacro(This,lMacro,MacroBuff)

#define IGlobals_RegisterCommand(This,lCmdId,strName,strDescription)	\
    (This)->lpVtbl -> RegisterCommand(This,lCmdId,strName,strDescription)

#define IGlobals_UnregisterCommand(This,lCmdId)	\
    (This)->lpVtbl -> UnregisterCommand(This,lCmdId)

#define IGlobals_GetLanguageDef(This,strName,Language)	\
    (This)->lpVtbl -> GetLanguageDef(This,strName,Language)

#define IGlobals_UnregisterAllLanguages(This)	\
    (This)->lpVtbl -> UnregisterAllLanguages(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE IGlobals_put_FindMRUList_Proxy( 
    IGlobals __RPC_FAR * This,
    /* [in] */ BSTR strMRUList);


void __RPC_STUB IGlobals_put_FindMRUList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE IGlobals_get_FindMRUList_Proxy( 
    IGlobals __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrMRUList);


void __RPC_STUB IGlobals_get_FindMRUList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE IGlobals_put_ReplaceMRUList_Proxy( 
    IGlobals __RPC_FAR * This,
    /* [in] */ BSTR strMRUList);


void __RPC_STUB IGlobals_put_ReplaceMRUList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE IGlobals_get_ReplaceMRUList_Proxy( 
    IGlobals __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrMRUList);


void __RPC_STUB IGlobals_get_ReplaceMRUList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IGlobals_RegisterLanguage_Proxy( 
    IGlobals __RPC_FAR * This,
    /* [in] */ BSTR strName,
    /* [in] */ ILanguage __RPC_FAR *LanguageDef);


void __RPC_STUB IGlobals_RegisterLanguage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IGlobals_UnregisterLanguage_Proxy( 
    IGlobals __RPC_FAR * This,
    /* [in] */ BSTR strName);


void __RPC_STUB IGlobals_UnregisterLanguage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IGlobals_GetHotKeys_Proxy( 
    IGlobals __RPC_FAR * This,
    /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *HotKeysBuff);


void __RPC_STUB IGlobals_GetHotKeys_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IGlobals_SetHotKeys_Proxy( 
    IGlobals __RPC_FAR * This,
    /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *HotKeysBuff);


void __RPC_STUB IGlobals_SetHotKeys_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IGlobals_ResetDefaultHotKeys_Proxy( 
    IGlobals __RPC_FAR * This);


void __RPC_STUB IGlobals_ResetDefaultHotKeys_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IGlobals_GetHotKeyForCmd_Proxy( 
    IGlobals __RPC_FAR * This,
    /* [in] */ cmCommand lCmd,
    /* [in] */ long lNum,
    /* [retval][out] */ IHotKey __RPC_FAR *__RPC_FAR *ppHotKey);


void __RPC_STUB IGlobals_GetHotKeyForCmd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IGlobals_GetNumHotKeysForCmd_Proxy( 
    IGlobals __RPC_FAR * This,
    /* [in] */ cmCommand lCmd,
    /* [retval][out] */ long __RPC_FAR *plCount);


void __RPC_STUB IGlobals_GetNumHotKeysForCmd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IGlobals_RegisterHotKey_Proxy( 
    IGlobals __RPC_FAR * This,
    /* [in] */ IHotKey __RPC_FAR *pHotKey,
    /* [in] */ cmCommand lCmd);


void __RPC_STUB IGlobals_RegisterHotKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IGlobals_UnregisterHotKey_Proxy( 
    IGlobals __RPC_FAR * This,
    /* [in] */ IHotKey __RPC_FAR *pHotKey);


void __RPC_STUB IGlobals_UnregisterHotKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IGlobals_GetCommandString_Proxy( 
    IGlobals __RPC_FAR * This,
    /* [in] */ cmCommand lCmd,
    /* [in] */ VARIANT_BOOL bDescription,
    /* [retval][out] */ BSTR __RPC_FAR *pstrVal);


void __RPC_STUB IGlobals_GetCommandString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IGlobals_GetMacro_Proxy( 
    IGlobals __RPC_FAR * This,
    /* [in] */ long lMacro,
    /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *MacroBuff);


void __RPC_STUB IGlobals_GetMacro_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IGlobals_SetMacro_Proxy( 
    IGlobals __RPC_FAR * This,
    /* [in] */ long lMacro,
    /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *MacroBuff);


void __RPC_STUB IGlobals_SetMacro_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IGlobals_RegisterCommand_Proxy( 
    IGlobals __RPC_FAR * This,
    /* [in] */ long lCmdId,
    /* [in] */ BSTR strName,
    /* [in] */ BSTR strDescription);


void __RPC_STUB IGlobals_RegisterCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IGlobals_UnregisterCommand_Proxy( 
    IGlobals __RPC_FAR * This,
    /* [in] */ long lCmdId);


void __RPC_STUB IGlobals_UnregisterCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IGlobals_GetLanguageDef_Proxy( 
    IGlobals __RPC_FAR * This,
    /* [in] */ BSTR strName,
    /* [retval][out] */ ILanguage __RPC_FAR *__RPC_FAR *Language);


void __RPC_STUB IGlobals_GetLanguageDef_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IGlobals_UnregisterAllLanguages_Proxy( 
    IGlobals __RPC_FAR * This);


void __RPC_STUB IGlobals_UnregisterAllLanguages_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IGlobals_INTERFACE_DEFINED__ */


#ifndef __IRect_INTERFACE_DEFINED__
#define __IRect_INTERFACE_DEFINED__

/* interface IRect */
/* [unique][helpstring][helpcontext][dual][uuid][object] */ 


EXTERN_C const IID IID_IRect;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EF28E751-2AEF-11d4-AE1E-000000000000")
    IRect : public IDispatch
    {
    public:
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_left( 
            /* [in] */ long lVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_left( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_top( 
            /* [in] */ long lVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_top( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_right( 
            /* [in] */ long lVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_right( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_bottom( 
            /* [in] */ long lVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_bottom( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IRect __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IRect __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IRect __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IRect __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IRect __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IRect __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IRect __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_left )( 
            IRect __RPC_FAR * This,
            /* [in] */ long lVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_left )( 
            IRect __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_top )( 
            IRect __RPC_FAR * This,
            /* [in] */ long lVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_top )( 
            IRect __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_right )( 
            IRect __RPC_FAR * This,
            /* [in] */ long lVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_right )( 
            IRect __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_bottom )( 
            IRect __RPC_FAR * This,
            /* [in] */ long lVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_bottom )( 
            IRect __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        END_INTERFACE
    } IRectVtbl;

    interface IRect
    {
        CONST_VTBL struct IRectVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRect_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRect_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IRect_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IRect_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IRect_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IRect_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IRect_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IRect_put_left(This,lVal)	\
    (This)->lpVtbl -> put_left(This,lVal)

#define IRect_get_left(This,plVal)	\
    (This)->lpVtbl -> get_left(This,plVal)

#define IRect_put_top(This,lVal)	\
    (This)->lpVtbl -> put_top(This,lVal)

#define IRect_get_top(This,plVal)	\
    (This)->lpVtbl -> get_top(This,plVal)

#define IRect_put_right(This,lVal)	\
    (This)->lpVtbl -> put_right(This,lVal)

#define IRect_get_right(This,plVal)	\
    (This)->lpVtbl -> get_right(This,plVal)

#define IRect_put_bottom(This,lVal)	\
    (This)->lpVtbl -> put_bottom(This,lVal)

#define IRect_get_bottom(This,plVal)	\
    (This)->lpVtbl -> get_bottom(This,plVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE IRect_put_left_Proxy( 
    IRect __RPC_FAR * This,
    /* [in] */ long lVal);


void __RPC_STUB IRect_put_left_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE IRect_get_left_Proxy( 
    IRect __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB IRect_get_left_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE IRect_put_top_Proxy( 
    IRect __RPC_FAR * This,
    /* [in] */ long lVal);


void __RPC_STUB IRect_put_top_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE IRect_get_top_Proxy( 
    IRect __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB IRect_get_top_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE IRect_put_right_Proxy( 
    IRect __RPC_FAR * This,
    /* [in] */ long lVal);


void __RPC_STUB IRect_put_right_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE IRect_get_right_Proxy( 
    IRect __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB IRect_get_right_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE IRect_put_bottom_Proxy( 
    IRect __RPC_FAR * This,
    /* [in] */ long lVal);


void __RPC_STUB IRect_put_bottom_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE IRect_get_bottom_Proxy( 
    IRect __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB IRect_get_bottom_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IRect_INTERFACE_DEFINED__ */


#ifndef __ICodeMax_INTERFACE_DEFINED__
#define __ICodeMax_INTERFACE_DEFINED__

/* interface ICodeMax */
/* [unique][helpstring][helpcontext][dual][uuid][object] */ 


EXTERN_C const IID IID_ICodeMax;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ECEDB95A-AC41-11d2-AB20-000000000000")
    ICodeMax : public IDispatch
    {
    public:
        virtual /* [helpstring][helpcontext][id][propputref] */ HRESULT STDMETHODCALLTYPE putref_Font( 
            /* [in] */ IFontDisp __RPC_FAR *pFont) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Font( 
            /* [in] */ IFontDisp __RPC_FAR *pFont) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Font( 
            /* [retval][out] */ IFontDisp __RPC_FAR *__RPC_FAR *ppFont) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Enabled( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Enabled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Window( 
            /* [retval][out] */ long __RPC_FAR *phwnd) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Text( 
            /* [in] */ BSTR strVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Text( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_ColorSyntax( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ColorSyntax( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_DisplayWhitespace( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_DisplayWhitespace( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_ExpandTabs( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ExpandTabs( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_SmoothScrolling( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_SmoothScrolling( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_ReadOnly( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ReadOnly( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_LineToolTips( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_LineToolTips( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_DisplayLeftMargin( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_DisplayLeftMargin( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_EnableColumnSel( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_EnableColumnSel( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_EnableDragDrop( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_EnableDragDrop( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Overtype( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Overtype( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_SearchCaseSensitive( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_SearchCaseSensitive( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_ReplacePreserveCase( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ReplacePreserveCase( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_SearchWholeWord( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_SearchWholeWord( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Modified( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Modified( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_EnableCRLF( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_EnableCRLF( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_GlobalProps( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_GlobalProps( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_VScrollVisible( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_VScrollVisible( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_HScrollVisible( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_HScrollVisible( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_EnableVSplitter( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_EnableVSplitter( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_EnableHSplitter( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_EnableHSplitter( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Language( 
            /* [in] */ BSTR strVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Language( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_TabSize( 
            /* [in] */ long lVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_TabSize( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_TopIndex( 
            /* [in] */ long lVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_TopIndex( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_UndoLimit( 
            /* [in] */ long lVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_UndoLimit( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentView( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_TextLength( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_TextLengthLogical( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_LineCount( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentWord( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentWordLength( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentWordLengthLogical( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_AutoIndentMode( 
            /* [in] */ cmAutoIndentMode lVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_AutoIndentMode( 
            /* [retval][out] */ cmAutoIndentMode __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ViewCount( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_VSplitterPos( 
            /* [in] */ long lVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_VSplitterPos( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_HSplitterPos( 
            /* [in] */ long lVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_HSplitterPos( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_FindText( 
            /* [in] */ BSTR strVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_FindText( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_ReplText( 
            /* [in] */ BSTR strVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ReplText( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_LineNumbering( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_LineNumbering( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_LineNumberStart( 
            /* [in] */ long lVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_LineNumberStart( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_LineNumberStyle( 
            /* [in] */ cmLineNumStyle lStyle) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_LineNumberStyle( 
            /* [retval][out] */ cmLineNumStyle __RPC_FAR *plStyle) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetWord( 
            /* [in] */ IPosition __RPC_FAR *Pos,
            /* [retval][out] */ BSTR __RPC_FAR *pstrWord) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetWordLength( 
            /* [in] */ IPosition __RPC_FAR *Pos,
            /* [in] */ VARIANT_BOOL bLogical,
            /* [retval][out] */ long __RPC_FAR *plLen) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetTopIndex( 
            /* [in] */ long lView,
            /* [retval][out] */ long __RPC_FAR *plLine) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SetTopIndex( 
            /* [in] */ long lView,
            /* [in] */ long lLine) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetSel( 
            /* [in] */ VARIANT_BOOL bNormalized,
            /* [retval][out] */ IRange __RPC_FAR *__RPC_FAR *Sel) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SetSel( 
            /* [in] */ IRange __RPC_FAR *Sel,
            /* [in] */ VARIANT_BOOL bMakeVisible) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetSelFromPoint( 
            /* [in] */ long lxClientPos,
            /* [in] */ long lyClientPos,
            /* [retval][out] */ IPosition __RPC_FAR *__RPC_FAR *Pos) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetVisibleLineCount( 
            /* [in] */ long lView,
            /* [in] */ VARIANT_BOOL bFullyVisible,
            /* [retval][out] */ long __RPC_FAR *plCount) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetLine( 
            /* [in] */ long lLine,
            /* [retval][out] */ BSTR __RPC_FAR *pstrLine) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetLineLength( 
            /* [in] */ long lLine,
            /* [retval][out] */ long __RPC_FAR *plLen) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SetColor( 
            /* [in] */ cmColorItem lColorItem,
            /* [in] */ OLE_COLOR crColor) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetColor( 
            /* [in] */ cmColorItem lColorItem,
            /* [retval][out] */ OLE_COLOR __RPC_FAR *pcrColor) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE OpenFile( 
            /* [in] */ BSTR strFileName) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SaveFile( 
            /* [in] */ BSTR strFileName,
            /* [in] */ VARIANT_BOOL bClearUndo) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE InsertFile( 
            /* [in] */ BSTR strFileName,
            /* [in] */ IPosition __RPC_FAR *InsertPos) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE InsertText( 
            /* [in] */ BSTR strText,
            /* [in] */ IPosition __RPC_FAR *InsertPos) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ReplaceText( 
            /* [in] */ BSTR strText,
            /* [in] */ IRange __RPC_FAR *Range) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE HitTest( 
            /* [in] */ long lxClientPos,
            /* [in] */ long lyClientPos,
            /* [out] */ long __RPC_FAR *plView,
            /* [retval][out] */ cmHitTestCode __RPC_FAR *plHitTestCode) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE AddText( 
            /* [in] */ BSTR strText) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE DeleteLine( 
            /* [in] */ long lLine) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE InsertLine( 
            /* [in] */ long lLine,
            /* [in] */ BSTR strLine) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SelectLine( 
            /* [in] */ long lLine,
            /* [in] */ VARIANT_BOOL bMakeVisible) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE DeleteSel( void) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ReplaceSel( 
            /* [in] */ BSTR strText) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ExecuteCmd( 
            /* [in] */ cmCommand lCommand,
            /* [optional][in] */ VARIANT CmdData) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE CanUndo( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE CanRedo( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE CanCut( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE CanCopy( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE CanPaste( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE Undo( void) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE Redo( void) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE Cut( void) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE Copy( void) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE Paste( void) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ClearUndoBuffer( void) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IsRecordingMacro( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IsPlayingMacro( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SetDlgParent( 
            /* [in] */ long lhWnd) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_SelText( 
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_SelText( 
            /* [in] */ BSTR strVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_SelBounds( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_SelBounds( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SetFontStyle( 
            /* [in] */ cmFontStyleItem lFontStyleItem,
            /* [in] */ cmFontStyle byFontStyle) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetFontStyle( 
            /* [in] */ cmFontStyleItem lFontStyleItem,
            /* [retval][out] */ cmFontStyle __RPC_FAR *pbyFontStyle) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_SearchRegExp( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_SearchRegExp( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SetItemData( 
            /* [in] */ long lLine,
            /* [in] */ long lData) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetItemData( 
            /* [in] */ long lLine,
            /* [retval][out] */ long __RPC_FAR *plData) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SetLineStyle( 
            /* [in] */ long lLine,
            /* [in] */ long lStyle) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetLineStyle( 
            /* [in] */ long lLine,
            /* [retval][out] */ long __RPC_FAR *plStyle) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SetBookmark( 
            /* [in] */ long lLine,
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetBookmark( 
            /* [in] */ long lLine,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SetAllBookmarks( 
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *Lines) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetAllBookmarks( 
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *Lines) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE PosFromChar( 
            /* [in] */ IPosition __RPC_FAR *pPos,
            /* [retval][out] */ IRect __RPC_FAR *__RPC_FAR *Rect) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_HideSel( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_HideSel( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_HighlightedLine( 
            /* [in] */ long lLine) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_HighlightedLine( 
            /* [retval][out] */ long __RPC_FAR *plLine) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_NormalizeCase( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_NormalizeCase( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SetDivider( 
            /* [in] */ long lLine,
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetDivider( 
            /* [in] */ long lLine,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_OvertypeCaret( 
            /* [in] */ VARIANT_BOOL bVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_OvertypeCaret( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SetMarginImages( 
            /* [in] */ long lLine,
            /* [in] */ BYTE byImages) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetMarginImages( 
            /* [in] */ long lLine,
            /* [retval][out] */ BYTE __RPC_FAR *pbyImages) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_hImageList( 
            /* [in] */ OLE_HANDLE hImageList) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_hImageList( 
            /* [retval][out] */ OLE_HANDLE __RPC_FAR *phImageList) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ImageList( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppImageList) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_ImageList( 
            /* [in] */ IDispatch __RPC_FAR *pImageList) = 0;
        
        virtual /* [helpstring][helpcontext][id][propputref] */ HRESULT STDMETHODCALLTYPE putref_ImageList( 
            /* [in] */ IDispatch __RPC_FAR *pImageList) = 0;
        
        virtual /* [helpcontext][helpstring][id][hidden] */ HRESULT STDMETHODCALLTYPE AboutBox( void) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE PrintContents( 
            long hDC,
            long lFlags) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SetCaretPos( 
            /* [in] */ long lLine,
            /* [in] */ long lCol) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ViewColToBufferCol( 
            /* [in] */ long lLine,
            /* [in] */ long lViewCol,
            /* [retval][out] */ long __RPC_FAR *plBuffCol) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE BufferColToViewCol( 
            /* [in] */ long lLine,
            /* [in] */ long lBuffCol,
            /* [retval][out] */ long __RPC_FAR *plViewCol) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_SelLength( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_SelLengthLogical( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_BorderStyle( 
            /* [in] */ cmBorderStyle lBorderStyle) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_BorderStyle( 
            /* [retval][out] */ cmBorderStyle __RPC_FAR *plBorderStyle) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_hWnd( 
            /* [retval][out] */ long __RPC_FAR *phwnd) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_CurrentView( 
            /* [in] */ long lVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICodeMaxVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICodeMax __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICodeMax __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ICodeMax __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][helpcontext][id][propputref] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_Font )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ IFontDisp __RPC_FAR *pFont);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Font )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ IFontDisp __RPC_FAR *pFont);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Font )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ IFontDisp __RPC_FAR *__RPC_FAR *ppFont);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Enabled )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Enabled )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Window )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *phwnd);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Text )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ BSTR strVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Text )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ColorSyntax )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ColorSyntax )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DisplayWhitespace )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DisplayWhitespace )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ExpandTabs )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ExpandTabs )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SmoothScrolling )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SmoothScrolling )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ReadOnly )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReadOnly )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LineToolTips )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LineToolTips )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DisplayLeftMargin )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DisplayLeftMargin )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableColumnSel )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableColumnSel )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableDragDrop )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableDragDrop )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Overtype )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Overtype )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SearchCaseSensitive )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SearchCaseSensitive )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ReplacePreserveCase )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReplacePreserveCase )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SearchWholeWord )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SearchWholeWord )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Modified )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Modified )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableCRLF )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableCRLF )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_GlobalProps )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_GlobalProps )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_VScrollVisible )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VScrollVisible )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HScrollVisible )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HScrollVisible )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableVSplitter )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableVSplitter )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableHSplitter )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableHSplitter )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Language )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ BSTR strVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Language )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TabSize )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TabSize )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TopIndex )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TopIndex )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_UndoLimit )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UndoLimit )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentView )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TextLength )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TextLengthLogical )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LineCount )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentWord )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentWordLength )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentWordLengthLogical )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AutoIndentMode )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ cmAutoIndentMode lVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AutoIndentMode )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ cmAutoIndentMode __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ViewCount )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_VSplitterPos )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VSplitterPos )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HSplitterPos )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HSplitterPos )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FindText )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ BSTR strVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FindText )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ReplText )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ BSTR strVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReplText )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LineNumbering )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LineNumbering )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LineNumberStart )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LineNumberStart )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LineNumberStyle )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ cmLineNumStyle lStyle);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LineNumberStyle )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ cmLineNumStyle __RPC_FAR *plStyle);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetWord )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ IPosition __RPC_FAR *Pos,
            /* [retval][out] */ BSTR __RPC_FAR *pstrWord);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetWordLength )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ IPosition __RPC_FAR *Pos,
            /* [in] */ VARIANT_BOOL bLogical,
            /* [retval][out] */ long __RPC_FAR *plLen);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTopIndex )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lView,
            /* [retval][out] */ long __RPC_FAR *plLine);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetTopIndex )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lView,
            /* [in] */ long lLine);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSel )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bNormalized,
            /* [retval][out] */ IRange __RPC_FAR *__RPC_FAR *Sel);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetSel )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ IRange __RPC_FAR *Sel,
            /* [in] */ VARIANT_BOOL bMakeVisible);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSelFromPoint )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lxClientPos,
            /* [in] */ long lyClientPos,
            /* [retval][out] */ IPosition __RPC_FAR *__RPC_FAR *Pos);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetVisibleLineCount )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lView,
            /* [in] */ VARIANT_BOOL bFullyVisible,
            /* [retval][out] */ long __RPC_FAR *plCount);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetLine )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lLine,
            /* [retval][out] */ BSTR __RPC_FAR *pstrLine);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetLineLength )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lLine,
            /* [retval][out] */ long __RPC_FAR *plLen);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetColor )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ cmColorItem lColorItem,
            /* [in] */ OLE_COLOR crColor);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetColor )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ cmColorItem lColorItem,
            /* [retval][out] */ OLE_COLOR __RPC_FAR *pcrColor);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OpenFile )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ BSTR strFileName);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveFile )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ BSTR strFileName,
            /* [in] */ VARIANT_BOOL bClearUndo);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InsertFile )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ BSTR strFileName,
            /* [in] */ IPosition __RPC_FAR *InsertPos);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InsertText )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ BSTR strText,
            /* [in] */ IPosition __RPC_FAR *InsertPos);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReplaceText )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ BSTR strText,
            /* [in] */ IRange __RPC_FAR *Range);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *HitTest )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lxClientPos,
            /* [in] */ long lyClientPos,
            /* [out] */ long __RPC_FAR *plView,
            /* [retval][out] */ cmHitTestCode __RPC_FAR *plHitTestCode);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddText )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ BSTR strText);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DeleteLine )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lLine);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InsertLine )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lLine,
            /* [in] */ BSTR strLine);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SelectLine )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lLine,
            /* [in] */ VARIANT_BOOL bMakeVisible);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DeleteSel )( 
            ICodeMax __RPC_FAR * This);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReplaceSel )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ BSTR strText);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ExecuteCmd )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ cmCommand lCommand,
            /* [optional][in] */ VARIANT CmdData);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CanUndo )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CanRedo )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CanCut )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CanCopy )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CanPaste )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Undo )( 
            ICodeMax __RPC_FAR * This);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Redo )( 
            ICodeMax __RPC_FAR * This);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Cut )( 
            ICodeMax __RPC_FAR * This);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Copy )( 
            ICodeMax __RPC_FAR * This);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Paste )( 
            ICodeMax __RPC_FAR * This);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ClearUndoBuffer )( 
            ICodeMax __RPC_FAR * This);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsRecordingMacro )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsPlayingMacro )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDlgParent )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lhWnd);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SelText )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pstrVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SelText )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ BSTR strVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SelBounds )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SelBounds )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetFontStyle )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ cmFontStyleItem lFontStyleItem,
            /* [in] */ cmFontStyle byFontStyle);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetFontStyle )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ cmFontStyleItem lFontStyleItem,
            /* [retval][out] */ cmFontStyle __RPC_FAR *pbyFontStyle);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SearchRegExp )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SearchRegExp )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetItemData )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lLine,
            /* [in] */ long lData);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetItemData )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lLine,
            /* [retval][out] */ long __RPC_FAR *plData);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetLineStyle )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lLine,
            /* [in] */ long lStyle);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetLineStyle )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lLine,
            /* [retval][out] */ long __RPC_FAR *plStyle);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetBookmark )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lLine,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetBookmark )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lLine,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetAllBookmarks )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *Lines);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAllBookmarks )( 
            ICodeMax __RPC_FAR * This,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *Lines);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PosFromChar )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ IPosition __RPC_FAR *pPos,
            /* [retval][out] */ IRect __RPC_FAR *__RPC_FAR *Rect);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HideSel )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HideSel )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HighlightedLine )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lLine);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HighlightedLine )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plLine);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_NormalizeCase )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_NormalizeCase )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDivider )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lLine,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDivider )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lLine,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OvertypeCaret )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OvertypeCaret )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetMarginImages )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lLine,
            /* [in] */ BYTE byImages);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMarginImages )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lLine,
            /* [retval][out] */ BYTE __RPC_FAR *pbyImages);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_hImageList )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ OLE_HANDLE hImageList);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_hImageList )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ OLE_HANDLE __RPC_FAR *phImageList);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ImageList )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppImageList);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ImageList )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *pImageList);
        
        /* [helpstring][helpcontext][id][propputref] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_ImageList )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *pImageList);
        
        /* [helpcontext][helpstring][id][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AboutBox )( 
            ICodeMax __RPC_FAR * This);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PrintContents )( 
            ICodeMax __RPC_FAR * This,
            long hDC,
            long lFlags);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetCaretPos )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lLine,
            /* [in] */ long lCol);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ViewColToBufferCol )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lLine,
            /* [in] */ long lViewCol,
            /* [retval][out] */ long __RPC_FAR *plBuffCol);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BufferColToViewCol )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lLine,
            /* [in] */ long lBuffCol,
            /* [retval][out] */ long __RPC_FAR *plViewCol);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SelLength )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SelLengthLogical )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BorderStyle )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ cmBorderStyle lBorderStyle);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BorderStyle )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ cmBorderStyle __RPC_FAR *plBorderStyle);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_hWnd )( 
            ICodeMax __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *phwnd);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CurrentView )( 
            ICodeMax __RPC_FAR * This,
            /* [in] */ long lVal);
        
        END_INTERFACE
    } ICodeMaxVtbl;

    interface ICodeMax
    {
        CONST_VTBL struct ICodeMaxVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICodeMax_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICodeMax_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICodeMax_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICodeMax_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ICodeMax_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ICodeMax_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ICodeMax_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ICodeMax_putref_Font(This,pFont)	\
    (This)->lpVtbl -> putref_Font(This,pFont)

#define ICodeMax_put_Font(This,pFont)	\
    (This)->lpVtbl -> put_Font(This,pFont)

#define ICodeMax_get_Font(This,ppFont)	\
    (This)->lpVtbl -> get_Font(This,ppFont)

#define ICodeMax_put_Enabled(This,bVal)	\
    (This)->lpVtbl -> put_Enabled(This,bVal)

#define ICodeMax_get_Enabled(This,pbVal)	\
    (This)->lpVtbl -> get_Enabled(This,pbVal)

#define ICodeMax_get_Window(This,phwnd)	\
    (This)->lpVtbl -> get_Window(This,phwnd)

#define ICodeMax_put_Text(This,strVal)	\
    (This)->lpVtbl -> put_Text(This,strVal)

#define ICodeMax_get_Text(This,pstrVal)	\
    (This)->lpVtbl -> get_Text(This,pstrVal)

#define ICodeMax_put_ColorSyntax(This,bVal)	\
    (This)->lpVtbl -> put_ColorSyntax(This,bVal)

#define ICodeMax_get_ColorSyntax(This,pbVal)	\
    (This)->lpVtbl -> get_ColorSyntax(This,pbVal)

#define ICodeMax_put_DisplayWhitespace(This,bVal)	\
    (This)->lpVtbl -> put_DisplayWhitespace(This,bVal)

#define ICodeMax_get_DisplayWhitespace(This,pbVal)	\
    (This)->lpVtbl -> get_DisplayWhitespace(This,pbVal)

#define ICodeMax_put_ExpandTabs(This,bVal)	\
    (This)->lpVtbl -> put_ExpandTabs(This,bVal)

#define ICodeMax_get_ExpandTabs(This,pbVal)	\
    (This)->lpVtbl -> get_ExpandTabs(This,pbVal)

#define ICodeMax_put_SmoothScrolling(This,bVal)	\
    (This)->lpVtbl -> put_SmoothScrolling(This,bVal)

#define ICodeMax_get_SmoothScrolling(This,pbVal)	\
    (This)->lpVtbl -> get_SmoothScrolling(This,pbVal)

#define ICodeMax_put_ReadOnly(This,bVal)	\
    (This)->lpVtbl -> put_ReadOnly(This,bVal)

#define ICodeMax_get_ReadOnly(This,pbVal)	\
    (This)->lpVtbl -> get_ReadOnly(This,pbVal)

#define ICodeMax_put_LineToolTips(This,bVal)	\
    (This)->lpVtbl -> put_LineToolTips(This,bVal)

#define ICodeMax_get_LineToolTips(This,pbVal)	\
    (This)->lpVtbl -> get_LineToolTips(This,pbVal)

#define ICodeMax_put_DisplayLeftMargin(This,bVal)	\
    (This)->lpVtbl -> put_DisplayLeftMargin(This,bVal)

#define ICodeMax_get_DisplayLeftMargin(This,pbVal)	\
    (This)->lpVtbl -> get_DisplayLeftMargin(This,pbVal)

#define ICodeMax_put_EnableColumnSel(This,bVal)	\
    (This)->lpVtbl -> put_EnableColumnSel(This,bVal)

#define ICodeMax_get_EnableColumnSel(This,pbVal)	\
    (This)->lpVtbl -> get_EnableColumnSel(This,pbVal)

#define ICodeMax_put_EnableDragDrop(This,bVal)	\
    (This)->lpVtbl -> put_EnableDragDrop(This,bVal)

#define ICodeMax_get_EnableDragDrop(This,pbVal)	\
    (This)->lpVtbl -> get_EnableDragDrop(This,pbVal)

#define ICodeMax_put_Overtype(This,bVal)	\
    (This)->lpVtbl -> put_Overtype(This,bVal)

#define ICodeMax_get_Overtype(This,pbVal)	\
    (This)->lpVtbl -> get_Overtype(This,pbVal)

#define ICodeMax_put_SearchCaseSensitive(This,bVal)	\
    (This)->lpVtbl -> put_SearchCaseSensitive(This,bVal)

#define ICodeMax_get_SearchCaseSensitive(This,pbVal)	\
    (This)->lpVtbl -> get_SearchCaseSensitive(This,pbVal)

#define ICodeMax_put_ReplacePreserveCase(This,bVal)	\
    (This)->lpVtbl -> put_ReplacePreserveCase(This,bVal)

#define ICodeMax_get_ReplacePreserveCase(This,pbVal)	\
    (This)->lpVtbl -> get_ReplacePreserveCase(This,pbVal)

#define ICodeMax_put_SearchWholeWord(This,bVal)	\
    (This)->lpVtbl -> put_SearchWholeWord(This,bVal)

#define ICodeMax_get_SearchWholeWord(This,pbVal)	\
    (This)->lpVtbl -> get_SearchWholeWord(This,pbVal)

#define ICodeMax_put_Modified(This,bVal)	\
    (This)->lpVtbl -> put_Modified(This,bVal)

#define ICodeMax_get_Modified(This,pbVal)	\
    (This)->lpVtbl -> get_Modified(This,pbVal)

#define ICodeMax_put_EnableCRLF(This,bVal)	\
    (This)->lpVtbl -> put_EnableCRLF(This,bVal)

#define ICodeMax_get_EnableCRLF(This,pbVal)	\
    (This)->lpVtbl -> get_EnableCRLF(This,pbVal)

#define ICodeMax_put_GlobalProps(This,bVal)	\
    (This)->lpVtbl -> put_GlobalProps(This,bVal)

#define ICodeMax_get_GlobalProps(This,pbVal)	\
    (This)->lpVtbl -> get_GlobalProps(This,pbVal)

#define ICodeMax_put_VScrollVisible(This,bVal)	\
    (This)->lpVtbl -> put_VScrollVisible(This,bVal)

#define ICodeMax_get_VScrollVisible(This,pbVal)	\
    (This)->lpVtbl -> get_VScrollVisible(This,pbVal)

#define ICodeMax_put_HScrollVisible(This,bVal)	\
    (This)->lpVtbl -> put_HScrollVisible(This,bVal)

#define ICodeMax_get_HScrollVisible(This,pbVal)	\
    (This)->lpVtbl -> get_HScrollVisible(This,pbVal)

#define ICodeMax_put_EnableVSplitter(This,bVal)	\
    (This)->lpVtbl -> put_EnableVSplitter(This,bVal)

#define ICodeMax_get_EnableVSplitter(This,pbVal)	\
    (This)->lpVtbl -> get_EnableVSplitter(This,pbVal)

#define ICodeMax_put_EnableHSplitter(This,bVal)	\
    (This)->lpVtbl -> put_EnableHSplitter(This,bVal)

#define ICodeMax_get_EnableHSplitter(This,pbVal)	\
    (This)->lpVtbl -> get_EnableHSplitter(This,pbVal)

#define ICodeMax_put_Language(This,strVal)	\
    (This)->lpVtbl -> put_Language(This,strVal)

#define ICodeMax_get_Language(This,pstrVal)	\
    (This)->lpVtbl -> get_Language(This,pstrVal)

#define ICodeMax_put_TabSize(This,lVal)	\
    (This)->lpVtbl -> put_TabSize(This,lVal)

#define ICodeMax_get_TabSize(This,plVal)	\
    (This)->lpVtbl -> get_TabSize(This,plVal)

#define ICodeMax_put_TopIndex(This,lVal)	\
    (This)->lpVtbl -> put_TopIndex(This,lVal)

#define ICodeMax_get_TopIndex(This,plVal)	\
    (This)->lpVtbl -> get_TopIndex(This,plVal)

#define ICodeMax_put_UndoLimit(This,lVal)	\
    (This)->lpVtbl -> put_UndoLimit(This,lVal)

#define ICodeMax_get_UndoLimit(This,plVal)	\
    (This)->lpVtbl -> get_UndoLimit(This,plVal)

#define ICodeMax_get_CurrentView(This,plVal)	\
    (This)->lpVtbl -> get_CurrentView(This,plVal)

#define ICodeMax_get_TextLength(This,plVal)	\
    (This)->lpVtbl -> get_TextLength(This,plVal)

#define ICodeMax_get_TextLengthLogical(This,plVal)	\
    (This)->lpVtbl -> get_TextLengthLogical(This,plVal)

#define ICodeMax_get_LineCount(This,plVal)	\
    (This)->lpVtbl -> get_LineCount(This,plVal)

#define ICodeMax_get_CurrentWord(This,pstrVal)	\
    (This)->lpVtbl -> get_CurrentWord(This,pstrVal)

#define ICodeMax_get_CurrentWordLength(This,plVal)	\
    (This)->lpVtbl -> get_CurrentWordLength(This,plVal)

#define ICodeMax_get_CurrentWordLengthLogical(This,plVal)	\
    (This)->lpVtbl -> get_CurrentWordLengthLogical(This,plVal)

#define ICodeMax_put_AutoIndentMode(This,lVal)	\
    (This)->lpVtbl -> put_AutoIndentMode(This,lVal)

#define ICodeMax_get_AutoIndentMode(This,plVal)	\
    (This)->lpVtbl -> get_AutoIndentMode(This,plVal)

#define ICodeMax_get_ViewCount(This,plVal)	\
    (This)->lpVtbl -> get_ViewCount(This,plVal)

#define ICodeMax_put_VSplitterPos(This,lVal)	\
    (This)->lpVtbl -> put_VSplitterPos(This,lVal)

#define ICodeMax_get_VSplitterPos(This,plVal)	\
    (This)->lpVtbl -> get_VSplitterPos(This,plVal)

#define ICodeMax_put_HSplitterPos(This,lVal)	\
    (This)->lpVtbl -> put_HSplitterPos(This,lVal)

#define ICodeMax_get_HSplitterPos(This,plVal)	\
    (This)->lpVtbl -> get_HSplitterPos(This,plVal)

#define ICodeMax_put_FindText(This,strVal)	\
    (This)->lpVtbl -> put_FindText(This,strVal)

#define ICodeMax_get_FindText(This,pstrVal)	\
    (This)->lpVtbl -> get_FindText(This,pstrVal)

#define ICodeMax_put_ReplText(This,strVal)	\
    (This)->lpVtbl -> put_ReplText(This,strVal)

#define ICodeMax_get_ReplText(This,pstrVal)	\
    (This)->lpVtbl -> get_ReplText(This,pstrVal)

#define ICodeMax_put_LineNumbering(This,bVal)	\
    (This)->lpVtbl -> put_LineNumbering(This,bVal)

#define ICodeMax_get_LineNumbering(This,pbVal)	\
    (This)->lpVtbl -> get_LineNumbering(This,pbVal)

#define ICodeMax_put_LineNumberStart(This,lVal)	\
    (This)->lpVtbl -> put_LineNumberStart(This,lVal)

#define ICodeMax_get_LineNumberStart(This,plVal)	\
    (This)->lpVtbl -> get_LineNumberStart(This,plVal)

#define ICodeMax_put_LineNumberStyle(This,lStyle)	\
    (This)->lpVtbl -> put_LineNumberStyle(This,lStyle)

#define ICodeMax_get_LineNumberStyle(This,plStyle)	\
    (This)->lpVtbl -> get_LineNumberStyle(This,plStyle)

#define ICodeMax_GetWord(This,Pos,pstrWord)	\
    (This)->lpVtbl -> GetWord(This,Pos,pstrWord)

#define ICodeMax_GetWordLength(This,Pos,bLogical,plLen)	\
    (This)->lpVtbl -> GetWordLength(This,Pos,bLogical,plLen)

#define ICodeMax_GetTopIndex(This,lView,plLine)	\
    (This)->lpVtbl -> GetTopIndex(This,lView,plLine)

#define ICodeMax_SetTopIndex(This,lView,lLine)	\
    (This)->lpVtbl -> SetTopIndex(This,lView,lLine)

#define ICodeMax_GetSel(This,bNormalized,Sel)	\
    (This)->lpVtbl -> GetSel(This,bNormalized,Sel)

#define ICodeMax_SetSel(This,Sel,bMakeVisible)	\
    (This)->lpVtbl -> SetSel(This,Sel,bMakeVisible)

#define ICodeMax_GetSelFromPoint(This,lxClientPos,lyClientPos,Pos)	\
    (This)->lpVtbl -> GetSelFromPoint(This,lxClientPos,lyClientPos,Pos)

#define ICodeMax_GetVisibleLineCount(This,lView,bFullyVisible,plCount)	\
    (This)->lpVtbl -> GetVisibleLineCount(This,lView,bFullyVisible,plCount)

#define ICodeMax_GetLine(This,lLine,pstrLine)	\
    (This)->lpVtbl -> GetLine(This,lLine,pstrLine)

#define ICodeMax_GetLineLength(This,lLine,plLen)	\
    (This)->lpVtbl -> GetLineLength(This,lLine,plLen)

#define ICodeMax_SetColor(This,lColorItem,crColor)	\
    (This)->lpVtbl -> SetColor(This,lColorItem,crColor)

#define ICodeMax_GetColor(This,lColorItem,pcrColor)	\
    (This)->lpVtbl -> GetColor(This,lColorItem,pcrColor)

#define ICodeMax_OpenFile(This,strFileName)	\
    (This)->lpVtbl -> OpenFile(This,strFileName)

#define ICodeMax_SaveFile(This,strFileName,bClearUndo)	\
    (This)->lpVtbl -> SaveFile(This,strFileName,bClearUndo)

#define ICodeMax_InsertFile(This,strFileName,InsertPos)	\
    (This)->lpVtbl -> InsertFile(This,strFileName,InsertPos)

#define ICodeMax_InsertText(This,strText,InsertPos)	\
    (This)->lpVtbl -> InsertText(This,strText,InsertPos)

#define ICodeMax_ReplaceText(This,strText,Range)	\
    (This)->lpVtbl -> ReplaceText(This,strText,Range)

#define ICodeMax_HitTest(This,lxClientPos,lyClientPos,plView,plHitTestCode)	\
    (This)->lpVtbl -> HitTest(This,lxClientPos,lyClientPos,plView,plHitTestCode)

#define ICodeMax_AddText(This,strText)	\
    (This)->lpVtbl -> AddText(This,strText)

#define ICodeMax_DeleteLine(This,lLine)	\
    (This)->lpVtbl -> DeleteLine(This,lLine)

#define ICodeMax_InsertLine(This,lLine,strLine)	\
    (This)->lpVtbl -> InsertLine(This,lLine,strLine)

#define ICodeMax_SelectLine(This,lLine,bMakeVisible)	\
    (This)->lpVtbl -> SelectLine(This,lLine,bMakeVisible)

#define ICodeMax_DeleteSel(This)	\
    (This)->lpVtbl -> DeleteSel(This)

#define ICodeMax_ReplaceSel(This,strText)	\
    (This)->lpVtbl -> ReplaceSel(This,strText)

#define ICodeMax_ExecuteCmd(This,lCommand,CmdData)	\
    (This)->lpVtbl -> ExecuteCmd(This,lCommand,CmdData)

#define ICodeMax_CanUndo(This,pbAnswer)	\
    (This)->lpVtbl -> CanUndo(This,pbAnswer)

#define ICodeMax_CanRedo(This,pbAnswer)	\
    (This)->lpVtbl -> CanRedo(This,pbAnswer)

#define ICodeMax_CanCut(This,pbAnswer)	\
    (This)->lpVtbl -> CanCut(This,pbAnswer)

#define ICodeMax_CanCopy(This,pbAnswer)	\
    (This)->lpVtbl -> CanCopy(This,pbAnswer)

#define ICodeMax_CanPaste(This,pbAnswer)	\
    (This)->lpVtbl -> CanPaste(This,pbAnswer)

#define ICodeMax_Undo(This)	\
    (This)->lpVtbl -> Undo(This)

#define ICodeMax_Redo(This)	\
    (This)->lpVtbl -> Redo(This)

#define ICodeMax_Cut(This)	\
    (This)->lpVtbl -> Cut(This)

#define ICodeMax_Copy(This)	\
    (This)->lpVtbl -> Copy(This)

#define ICodeMax_Paste(This)	\
    (This)->lpVtbl -> Paste(This)

#define ICodeMax_ClearUndoBuffer(This)	\
    (This)->lpVtbl -> ClearUndoBuffer(This)

#define ICodeMax_IsRecordingMacro(This,pbAnswer)	\
    (This)->lpVtbl -> IsRecordingMacro(This,pbAnswer)

#define ICodeMax_IsPlayingMacro(This,pbAnswer)	\
    (This)->lpVtbl -> IsPlayingMacro(This,pbAnswer)

#define ICodeMax_SetDlgParent(This,lhWnd)	\
    (This)->lpVtbl -> SetDlgParent(This,lhWnd)

#define ICodeMax_get_SelText(This,pstrVal)	\
    (This)->lpVtbl -> get_SelText(This,pstrVal)

#define ICodeMax_put_SelText(This,strVal)	\
    (This)->lpVtbl -> put_SelText(This,strVal)

#define ICodeMax_put_SelBounds(This,bVal)	\
    (This)->lpVtbl -> put_SelBounds(This,bVal)

#define ICodeMax_get_SelBounds(This,pbVal)	\
    (This)->lpVtbl -> get_SelBounds(This,pbVal)

#define ICodeMax_SetFontStyle(This,lFontStyleItem,byFontStyle)	\
    (This)->lpVtbl -> SetFontStyle(This,lFontStyleItem,byFontStyle)

#define ICodeMax_GetFontStyle(This,lFontStyleItem,pbyFontStyle)	\
    (This)->lpVtbl -> GetFontStyle(This,lFontStyleItem,pbyFontStyle)

#define ICodeMax_put_SearchRegExp(This,bVal)	\
    (This)->lpVtbl -> put_SearchRegExp(This,bVal)

#define ICodeMax_get_SearchRegExp(This,pbVal)	\
    (This)->lpVtbl -> get_SearchRegExp(This,pbVal)

#define ICodeMax_SetItemData(This,lLine,lData)	\
    (This)->lpVtbl -> SetItemData(This,lLine,lData)

#define ICodeMax_GetItemData(This,lLine,plData)	\
    (This)->lpVtbl -> GetItemData(This,lLine,plData)

#define ICodeMax_SetLineStyle(This,lLine,lStyle)	\
    (This)->lpVtbl -> SetLineStyle(This,lLine,lStyle)

#define ICodeMax_GetLineStyle(This,lLine,plStyle)	\
    (This)->lpVtbl -> GetLineStyle(This,lLine,plStyle)

#define ICodeMax_SetBookmark(This,lLine,bVal)	\
    (This)->lpVtbl -> SetBookmark(This,lLine,bVal)

#define ICodeMax_GetBookmark(This,lLine,pbVal)	\
    (This)->lpVtbl -> GetBookmark(This,lLine,pbVal)

#define ICodeMax_SetAllBookmarks(This,Lines)	\
    (This)->lpVtbl -> SetAllBookmarks(This,Lines)

#define ICodeMax_GetAllBookmarks(This,Lines)	\
    (This)->lpVtbl -> GetAllBookmarks(This,Lines)

#define ICodeMax_PosFromChar(This,pPos,Rect)	\
    (This)->lpVtbl -> PosFromChar(This,pPos,Rect)

#define ICodeMax_put_HideSel(This,bVal)	\
    (This)->lpVtbl -> put_HideSel(This,bVal)

#define ICodeMax_get_HideSel(This,pbVal)	\
    (This)->lpVtbl -> get_HideSel(This,pbVal)

#define ICodeMax_put_HighlightedLine(This,lLine)	\
    (This)->lpVtbl -> put_HighlightedLine(This,lLine)

#define ICodeMax_get_HighlightedLine(This,plLine)	\
    (This)->lpVtbl -> get_HighlightedLine(This,plLine)

#define ICodeMax_put_NormalizeCase(This,bVal)	\
    (This)->lpVtbl -> put_NormalizeCase(This,bVal)

#define ICodeMax_get_NormalizeCase(This,pbVal)	\
    (This)->lpVtbl -> get_NormalizeCase(This,pbVal)

#define ICodeMax_SetDivider(This,lLine,bVal)	\
    (This)->lpVtbl -> SetDivider(This,lLine,bVal)

#define ICodeMax_GetDivider(This,lLine,pbVal)	\
    (This)->lpVtbl -> GetDivider(This,lLine,pbVal)

#define ICodeMax_put_OvertypeCaret(This,bVal)	\
    (This)->lpVtbl -> put_OvertypeCaret(This,bVal)

#define ICodeMax_get_OvertypeCaret(This,pbVal)	\
    (This)->lpVtbl -> get_OvertypeCaret(This,pbVal)

#define ICodeMax_SetMarginImages(This,lLine,byImages)	\
    (This)->lpVtbl -> SetMarginImages(This,lLine,byImages)

#define ICodeMax_GetMarginImages(This,lLine,pbyImages)	\
    (This)->lpVtbl -> GetMarginImages(This,lLine,pbyImages)

#define ICodeMax_put_hImageList(This,hImageList)	\
    (This)->lpVtbl -> put_hImageList(This,hImageList)

#define ICodeMax_get_hImageList(This,phImageList)	\
    (This)->lpVtbl -> get_hImageList(This,phImageList)

#define ICodeMax_get_ImageList(This,ppImageList)	\
    (This)->lpVtbl -> get_ImageList(This,ppImageList)

#define ICodeMax_put_ImageList(This,pImageList)	\
    (This)->lpVtbl -> put_ImageList(This,pImageList)

#define ICodeMax_putref_ImageList(This,pImageList)	\
    (This)->lpVtbl -> putref_ImageList(This,pImageList)

#define ICodeMax_AboutBox(This)	\
    (This)->lpVtbl -> AboutBox(This)

#define ICodeMax_PrintContents(This,hDC,lFlags)	\
    (This)->lpVtbl -> PrintContents(This,hDC,lFlags)

#define ICodeMax_SetCaretPos(This,lLine,lCol)	\
    (This)->lpVtbl -> SetCaretPos(This,lLine,lCol)

#define ICodeMax_ViewColToBufferCol(This,lLine,lViewCol,plBuffCol)	\
    (This)->lpVtbl -> ViewColToBufferCol(This,lLine,lViewCol,plBuffCol)

#define ICodeMax_BufferColToViewCol(This,lLine,lBuffCol,plViewCol)	\
    (This)->lpVtbl -> BufferColToViewCol(This,lLine,lBuffCol,plViewCol)

#define ICodeMax_get_SelLength(This,plVal)	\
    (This)->lpVtbl -> get_SelLength(This,plVal)

#define ICodeMax_get_SelLengthLogical(This,plVal)	\
    (This)->lpVtbl -> get_SelLengthLogical(This,plVal)

#define ICodeMax_put_BorderStyle(This,lBorderStyle)	\
    (This)->lpVtbl -> put_BorderStyle(This,lBorderStyle)

#define ICodeMax_get_BorderStyle(This,plBorderStyle)	\
    (This)->lpVtbl -> get_BorderStyle(This,plBorderStyle)

#define ICodeMax_get_hWnd(This,phwnd)	\
    (This)->lpVtbl -> get_hWnd(This,phwnd)

#define ICodeMax_put_CurrentView(This,lVal)	\
    (This)->lpVtbl -> put_CurrentView(This,lVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][helpcontext][id][propputref] */ HRESULT STDMETHODCALLTYPE ICodeMax_putref_Font_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ IFontDisp __RPC_FAR *pFont);


void __RPC_STUB ICodeMax_putref_Font_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_Font_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ IFontDisp __RPC_FAR *pFont);


void __RPC_STUB ICodeMax_put_Font_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_Font_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ IFontDisp __RPC_FAR *__RPC_FAR *ppFont);


void __RPC_STUB ICodeMax_get_Font_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_Enabled_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_Enabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_Enabled_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_Enabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_Window_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *phwnd);


void __RPC_STUB ICodeMax_get_Window_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_Text_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ BSTR strVal);


void __RPC_STUB ICodeMax_put_Text_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_Text_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrVal);


void __RPC_STUB ICodeMax_get_Text_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_ColorSyntax_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_ColorSyntax_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_ColorSyntax_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_ColorSyntax_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_DisplayWhitespace_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_DisplayWhitespace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_DisplayWhitespace_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_DisplayWhitespace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_ExpandTabs_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_ExpandTabs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_ExpandTabs_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_ExpandTabs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_SmoothScrolling_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_SmoothScrolling_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_SmoothScrolling_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_SmoothScrolling_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_ReadOnly_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_ReadOnly_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_ReadOnly_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_ReadOnly_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_LineToolTips_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_LineToolTips_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_LineToolTips_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_LineToolTips_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_DisplayLeftMargin_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_DisplayLeftMargin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_DisplayLeftMargin_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_DisplayLeftMargin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_EnableColumnSel_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_EnableColumnSel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_EnableColumnSel_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_EnableColumnSel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_EnableDragDrop_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_EnableDragDrop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_EnableDragDrop_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_EnableDragDrop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_Overtype_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_Overtype_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_Overtype_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_Overtype_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_SearchCaseSensitive_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_SearchCaseSensitive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_SearchCaseSensitive_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_SearchCaseSensitive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_ReplacePreserveCase_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_ReplacePreserveCase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_ReplacePreserveCase_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_ReplacePreserveCase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_SearchWholeWord_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_SearchWholeWord_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_SearchWholeWord_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_SearchWholeWord_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_Modified_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_Modified_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_Modified_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_Modified_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_EnableCRLF_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_EnableCRLF_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_EnableCRLF_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_EnableCRLF_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_GlobalProps_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_GlobalProps_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_GlobalProps_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_GlobalProps_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_VScrollVisible_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_VScrollVisible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_VScrollVisible_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_VScrollVisible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_HScrollVisible_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_HScrollVisible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_HScrollVisible_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_HScrollVisible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_EnableVSplitter_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_EnableVSplitter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_EnableVSplitter_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_EnableVSplitter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_EnableHSplitter_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_EnableHSplitter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_EnableHSplitter_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_EnableHSplitter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_Language_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ BSTR strVal);


void __RPC_STUB ICodeMax_put_Language_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_Language_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrVal);


void __RPC_STUB ICodeMax_get_Language_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_TabSize_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lVal);


void __RPC_STUB ICodeMax_put_TabSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_TabSize_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB ICodeMax_get_TabSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_TopIndex_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lVal);


void __RPC_STUB ICodeMax_put_TopIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_TopIndex_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB ICodeMax_get_TopIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_UndoLimit_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lVal);


void __RPC_STUB ICodeMax_put_UndoLimit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_UndoLimit_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB ICodeMax_get_UndoLimit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_CurrentView_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB ICodeMax_get_CurrentView_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_TextLength_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB ICodeMax_get_TextLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_TextLengthLogical_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB ICodeMax_get_TextLengthLogical_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_LineCount_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB ICodeMax_get_LineCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_CurrentWord_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrVal);


void __RPC_STUB ICodeMax_get_CurrentWord_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_CurrentWordLength_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB ICodeMax_get_CurrentWordLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_CurrentWordLengthLogical_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB ICodeMax_get_CurrentWordLengthLogical_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_AutoIndentMode_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ cmAutoIndentMode lVal);


void __RPC_STUB ICodeMax_put_AutoIndentMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_AutoIndentMode_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ cmAutoIndentMode __RPC_FAR *plVal);


void __RPC_STUB ICodeMax_get_AutoIndentMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_ViewCount_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB ICodeMax_get_ViewCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_VSplitterPos_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lVal);


void __RPC_STUB ICodeMax_put_VSplitterPos_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_VSplitterPos_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB ICodeMax_get_VSplitterPos_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_HSplitterPos_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lVal);


void __RPC_STUB ICodeMax_put_HSplitterPos_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_HSplitterPos_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB ICodeMax_get_HSplitterPos_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_FindText_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ BSTR strVal);


void __RPC_STUB ICodeMax_put_FindText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_FindText_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrVal);


void __RPC_STUB ICodeMax_get_FindText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_ReplText_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ BSTR strVal);


void __RPC_STUB ICodeMax_put_ReplText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_ReplText_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrVal);


void __RPC_STUB ICodeMax_get_ReplText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_LineNumbering_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_LineNumbering_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_LineNumbering_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_LineNumbering_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_LineNumberStart_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lVal);


void __RPC_STUB ICodeMax_put_LineNumberStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_LineNumberStart_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB ICodeMax_get_LineNumberStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_LineNumberStyle_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ cmLineNumStyle lStyle);


void __RPC_STUB ICodeMax_put_LineNumberStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_LineNumberStyle_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ cmLineNumStyle __RPC_FAR *plStyle);


void __RPC_STUB ICodeMax_get_LineNumberStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_GetWord_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ IPosition __RPC_FAR *Pos,
    /* [retval][out] */ BSTR __RPC_FAR *pstrWord);


void __RPC_STUB ICodeMax_GetWord_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_GetWordLength_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ IPosition __RPC_FAR *Pos,
    /* [in] */ VARIANT_BOOL bLogical,
    /* [retval][out] */ long __RPC_FAR *plLen);


void __RPC_STUB ICodeMax_GetWordLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_GetTopIndex_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lView,
    /* [retval][out] */ long __RPC_FAR *plLine);


void __RPC_STUB ICodeMax_GetTopIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_SetTopIndex_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lView,
    /* [in] */ long lLine);


void __RPC_STUB ICodeMax_SetTopIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_GetSel_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bNormalized,
    /* [retval][out] */ IRange __RPC_FAR *__RPC_FAR *Sel);


void __RPC_STUB ICodeMax_GetSel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_SetSel_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ IRange __RPC_FAR *Sel,
    /* [in] */ VARIANT_BOOL bMakeVisible);


void __RPC_STUB ICodeMax_SetSel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_GetSelFromPoint_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lxClientPos,
    /* [in] */ long lyClientPos,
    /* [retval][out] */ IPosition __RPC_FAR *__RPC_FAR *Pos);


void __RPC_STUB ICodeMax_GetSelFromPoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_GetVisibleLineCount_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lView,
    /* [in] */ VARIANT_BOOL bFullyVisible,
    /* [retval][out] */ long __RPC_FAR *plCount);


void __RPC_STUB ICodeMax_GetVisibleLineCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_GetLine_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lLine,
    /* [retval][out] */ BSTR __RPC_FAR *pstrLine);


void __RPC_STUB ICodeMax_GetLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_GetLineLength_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lLine,
    /* [retval][out] */ long __RPC_FAR *plLen);


void __RPC_STUB ICodeMax_GetLineLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_SetColor_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ cmColorItem lColorItem,
    /* [in] */ OLE_COLOR crColor);


void __RPC_STUB ICodeMax_SetColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_GetColor_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ cmColorItem lColorItem,
    /* [retval][out] */ OLE_COLOR __RPC_FAR *pcrColor);


void __RPC_STUB ICodeMax_GetColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_OpenFile_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ BSTR strFileName);


void __RPC_STUB ICodeMax_OpenFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_SaveFile_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ BSTR strFileName,
    /* [in] */ VARIANT_BOOL bClearUndo);


void __RPC_STUB ICodeMax_SaveFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_InsertFile_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ BSTR strFileName,
    /* [in] */ IPosition __RPC_FAR *InsertPos);


void __RPC_STUB ICodeMax_InsertFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_InsertText_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ BSTR strText,
    /* [in] */ IPosition __RPC_FAR *InsertPos);


void __RPC_STUB ICodeMax_InsertText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_ReplaceText_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ BSTR strText,
    /* [in] */ IRange __RPC_FAR *Range);


void __RPC_STUB ICodeMax_ReplaceText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_HitTest_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lxClientPos,
    /* [in] */ long lyClientPos,
    /* [out] */ long __RPC_FAR *plView,
    /* [retval][out] */ cmHitTestCode __RPC_FAR *plHitTestCode);


void __RPC_STUB ICodeMax_HitTest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_AddText_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ BSTR strText);


void __RPC_STUB ICodeMax_AddText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_DeleteLine_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lLine);


void __RPC_STUB ICodeMax_DeleteLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_InsertLine_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lLine,
    /* [in] */ BSTR strLine);


void __RPC_STUB ICodeMax_InsertLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_SelectLine_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lLine,
    /* [in] */ VARIANT_BOOL bMakeVisible);


void __RPC_STUB ICodeMax_SelectLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_DeleteSel_Proxy( 
    ICodeMax __RPC_FAR * This);


void __RPC_STUB ICodeMax_DeleteSel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_ReplaceSel_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ BSTR strText);


void __RPC_STUB ICodeMax_ReplaceSel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_ExecuteCmd_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ cmCommand lCommand,
    /* [optional][in] */ VARIANT CmdData);


void __RPC_STUB ICodeMax_ExecuteCmd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_CanUndo_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer);


void __RPC_STUB ICodeMax_CanUndo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_CanRedo_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer);


void __RPC_STUB ICodeMax_CanRedo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_CanCut_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer);


void __RPC_STUB ICodeMax_CanCut_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_CanCopy_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer);


void __RPC_STUB ICodeMax_CanCopy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_CanPaste_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer);


void __RPC_STUB ICodeMax_CanPaste_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_Undo_Proxy( 
    ICodeMax __RPC_FAR * This);


void __RPC_STUB ICodeMax_Undo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_Redo_Proxy( 
    ICodeMax __RPC_FAR * This);


void __RPC_STUB ICodeMax_Redo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_Cut_Proxy( 
    ICodeMax __RPC_FAR * This);


void __RPC_STUB ICodeMax_Cut_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_Copy_Proxy( 
    ICodeMax __RPC_FAR * This);


void __RPC_STUB ICodeMax_Copy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_Paste_Proxy( 
    ICodeMax __RPC_FAR * This);


void __RPC_STUB ICodeMax_Paste_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_ClearUndoBuffer_Proxy( 
    ICodeMax __RPC_FAR * This);


void __RPC_STUB ICodeMax_ClearUndoBuffer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_IsRecordingMacro_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer);


void __RPC_STUB ICodeMax_IsRecordingMacro_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_IsPlayingMacro_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbAnswer);


void __RPC_STUB ICodeMax_IsPlayingMacro_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_SetDlgParent_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lhWnd);


void __RPC_STUB ICodeMax_SetDlgParent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_SelText_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pstrVal);


void __RPC_STUB ICodeMax_get_SelText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_SelText_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ BSTR strVal);


void __RPC_STUB ICodeMax_put_SelText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_SelBounds_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_SelBounds_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_SelBounds_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_SelBounds_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_SetFontStyle_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ cmFontStyleItem lFontStyleItem,
    /* [in] */ cmFontStyle byFontStyle);


void __RPC_STUB ICodeMax_SetFontStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_GetFontStyle_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ cmFontStyleItem lFontStyleItem,
    /* [retval][out] */ cmFontStyle __RPC_FAR *pbyFontStyle);


void __RPC_STUB ICodeMax_GetFontStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_SearchRegExp_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_SearchRegExp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_SearchRegExp_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_SearchRegExp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_SetItemData_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lLine,
    /* [in] */ long lData);


void __RPC_STUB ICodeMax_SetItemData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_GetItemData_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lLine,
    /* [retval][out] */ long __RPC_FAR *plData);


void __RPC_STUB ICodeMax_GetItemData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_SetLineStyle_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lLine,
    /* [in] */ long lStyle);


void __RPC_STUB ICodeMax_SetLineStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_GetLineStyle_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lLine,
    /* [retval][out] */ long __RPC_FAR *plStyle);


void __RPC_STUB ICodeMax_GetLineStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_SetBookmark_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lLine,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_SetBookmark_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_GetBookmark_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lLine,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_GetBookmark_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_SetAllBookmarks_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *Lines);


void __RPC_STUB ICodeMax_SetAllBookmarks_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_GetAllBookmarks_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *Lines);


void __RPC_STUB ICodeMax_GetAllBookmarks_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_PosFromChar_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ IPosition __RPC_FAR *pPos,
    /* [retval][out] */ IRect __RPC_FAR *__RPC_FAR *Rect);


void __RPC_STUB ICodeMax_PosFromChar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_HideSel_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_HideSel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_HideSel_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_HideSel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_HighlightedLine_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lLine);


void __RPC_STUB ICodeMax_put_HighlightedLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_HighlightedLine_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plLine);


void __RPC_STUB ICodeMax_get_HighlightedLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_NormalizeCase_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_NormalizeCase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_NormalizeCase_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_NormalizeCase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_SetDivider_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lLine,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_SetDivider_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_GetDivider_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lLine,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_GetDivider_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_OvertypeCaret_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bVal);


void __RPC_STUB ICodeMax_put_OvertypeCaret_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_OvertypeCaret_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbVal);


void __RPC_STUB ICodeMax_get_OvertypeCaret_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_SetMarginImages_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lLine,
    /* [in] */ BYTE byImages);


void __RPC_STUB ICodeMax_SetMarginImages_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_GetMarginImages_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lLine,
    /* [retval][out] */ BYTE __RPC_FAR *pbyImages);


void __RPC_STUB ICodeMax_GetMarginImages_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_hImageList_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ OLE_HANDLE hImageList);


void __RPC_STUB ICodeMax_put_hImageList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_hImageList_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ OLE_HANDLE __RPC_FAR *phImageList);


void __RPC_STUB ICodeMax_get_hImageList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_ImageList_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppImageList);


void __RPC_STUB ICodeMax_get_ImageList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_ImageList_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ IDispatch __RPC_FAR *pImageList);


void __RPC_STUB ICodeMax_put_ImageList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propputref] */ HRESULT STDMETHODCALLTYPE ICodeMax_putref_ImageList_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ IDispatch __RPC_FAR *pImageList);


void __RPC_STUB ICodeMax_putref_ImageList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id][hidden] */ HRESULT STDMETHODCALLTYPE ICodeMax_AboutBox_Proxy( 
    ICodeMax __RPC_FAR * This);


void __RPC_STUB ICodeMax_AboutBox_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_PrintContents_Proxy( 
    ICodeMax __RPC_FAR * This,
    long hDC,
    long lFlags);


void __RPC_STUB ICodeMax_PrintContents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_SetCaretPos_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lLine,
    /* [in] */ long lCol);


void __RPC_STUB ICodeMax_SetCaretPos_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_ViewColToBufferCol_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lLine,
    /* [in] */ long lViewCol,
    /* [retval][out] */ long __RPC_FAR *plBuffCol);


void __RPC_STUB ICodeMax_ViewColToBufferCol_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ICodeMax_BufferColToViewCol_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lLine,
    /* [in] */ long lBuffCol,
    /* [retval][out] */ long __RPC_FAR *plViewCol);


void __RPC_STUB ICodeMax_BufferColToViewCol_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_SelLength_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB ICodeMax_get_SelLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_SelLengthLogical_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB ICodeMax_get_SelLengthLogical_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_BorderStyle_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ cmBorderStyle lBorderStyle);


void __RPC_STUB ICodeMax_put_BorderStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_BorderStyle_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ cmBorderStyle __RPC_FAR *plBorderStyle);


void __RPC_STUB ICodeMax_get_BorderStyle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ICodeMax_get_hWnd_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *phwnd);


void __RPC_STUB ICodeMax_get_hWnd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ICodeMax_put_CurrentView_Proxy( 
    ICodeMax __RPC_FAR * This,
    /* [in] */ long lVal);


void __RPC_STUB ICodeMax_put_CurrentView_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICodeMax_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

unsigned long             __RPC_USER  LPSAFEARRAY_UserSize(     unsigned long __RPC_FAR *, unsigned long            , LPSAFEARRAY __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  LPSAFEARRAY_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, LPSAFEARRAY __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  LPSAFEARRAY_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, LPSAFEARRAY __RPC_FAR * ); 
void                      __RPC_USER  LPSAFEARRAY_UserFree(     unsigned long __RPC_FAR *, LPSAFEARRAY __RPC_FAR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long __RPC_FAR *, unsigned long            , VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long __RPC_FAR *, VARIANT __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
