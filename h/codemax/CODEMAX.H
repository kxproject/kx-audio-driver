/////////////////////////////////////////////////////////////////////////////////
//
//  CodeMax Code Editor Control
//
//  Copyright © 1997-2000  WinMain Software
//
//  This header file declares all exported functionality of the CodeMax custom
//  control.  For a complete description of all declarations below, please refer
//  to the CodeMax documentation.
//
/////////////////////////////////////////////////////////////////////////////////

#ifndef __CODEMAX_H__
#define __CODEMAX_H__

#define CODEMAXWNDCLASS _T("CodeMax")

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

/////////////////////////////////////////////////////////////
//
// Constants
//
//
// maximum size (TCHARs) of text to find or replace
#define CM_MAX_FINDREPL_TEXT           100
// maximum MRU size in find and find/replace dialogs
#define CM_FIND_REPLACE_MRU_MAX        10
// required buffer size for pszMRUList argument CMSetFindReplaceMRUList() and CMGetFindReplaceMRUList() 
#define CM_FIND_REPLACE_MRU_BUFF_SIZE  ( ( CM_MAX_FINDREPL_TEXT + 1 ) * CM_FIND_REPLACE_MRU_MAX )
// maximum number of keystroke macros supported by CodeMax
#define CM_MAX_MACROS                  10
// maximum size of command string returned in pszBuff param of CMGetCommandString() if bDescription is FALSE
#define CM_MAX_CMD_STRING              50
// maximum size of command string returned in pszBuff param of CMGetCommandString() if bDescription is TRUE
#define CM_MAX_CMD_DESCRIPTION         100
// maximum size of a language name set with CMRegisterLanguage
#define CM_MAX_LANGUAGE_NAME           30
// maximum tab size (characters)
#define CM_MAX_TABSIZE                 100
// minimum tab size (characters)
#define CM_MIN_TABSIZE                 2
// left margin width (pixels)
#define CM_CXLEFTMARGIN                24
/////////////////////////////////////////////////////////////
//
// CMM_SETLINESTYLE style bits
//
//
#define CML_OWNERDRAW      0x1      // Parent window should receive CMN_DRAWLINE notifications
#define CML_NOTIFY_DEL     0x2      // Parent window should receive CMN_LINEDELETED notifications

/////////////////////////////////////////////////////////////
//
// Messages
//
//
#define CMM_SETLANGUAGE                ( WM_USER + 1600 )
#define CMM_GETLANGUAGE                ( WM_USER + 1601 )
#define CMM_ENABLECOLORSYNTAX          ( WM_USER + 1610 )
#define CMM_ISCOLORSYNTAXENABLED       ( WM_USER + 1620 )
#define CMM_SETCOLORS                  ( WM_USER + 1630 )
#define CMM_GETCOLORS                  ( WM_USER + 1640 )
#define CMM_ENABLEWHITESPACEDISPLAY    ( WM_USER + 1800 )
#define CMM_ISWHITESPACEDISPLAYENABLED ( WM_USER + 1810 )
#define CMM_ENABLETABEXPAND            ( WM_USER + 1811 )
#define CMM_ISTABEXPANDENABLED         ( WM_USER + 1812 )
#define CMM_ENABLESMOOTHSCROLLING      ( WM_USER + 1820 )
#define CMM_SETTABSIZE                 ( WM_USER + 1821 )
#define CMM_GETTABSIZE                 ( WM_USER + 1822 )
#define CMM_ISSMOOTHSCROLLINGENABLED   ( WM_USER + 1830 )
#define CMM_SETREADONLY                ( WM_USER + 1840 )
#define CMM_ISREADONLY                 ( WM_USER + 1850 )
#define CMM_ENABLELINETOOLTIPS         ( WM_USER + 1860 )
#define CMM_ISLINETOOLTIPSENABLED      ( WM_USER + 1870 )
#define CMM_ENABLELEFTMARGIN           ( WM_USER + 1880 )
#define CMM_ISLEFTMARGINENABLED        ( WM_USER + 1890 )
#define CMM_ENABLECOLUMNSEL            ( WM_USER + 1891 )
#define CMM_ISCOLUMNSELENABLED         ( WM_USER + 1892 )
#define CMM_ENABLEDRAGDROP             ( WM_USER + 1893 )
#define CMM_ISDRAGDROPENABLED          ( WM_USER + 1894 )
#define CMM_ENABLEOVERTYPE             ( WM_USER + 1900 )
#define CMM_ISOVERTYPEENABLED          ( WM_USER + 1910 )
#define CMM_ENABLECASESENSITIVE        ( WM_USER + 1920 )
#define CMM_ISCASESENSITIVEENABLED     ( WM_USER + 1930 )
#define CMM_ENABLEPRESERVECASE         ( WM_USER + 1931 )
#define CMM_ISPRESERVECASEENABLED      ( WM_USER + 1932 )
#define CMM_ENABLEWHOLEWORD            ( WM_USER + 1940 )
#define CMM_ISWHOLEWORDENABLED         ( WM_USER + 1950 )
#define CMM_SETTOPINDEX                ( WM_USER + 1960 )
#define CMM_GETTOPINDEX                ( WM_USER + 1970 )
#define CMM_GETVISIBLELINECOUNT        ( WM_USER + 1980 )
#define CMM_HITTEST                    ( WM_USER + 1990 )
#define CMM_OPENFILE                   ( WM_USER + 2300 )
#define CMM_SAVEFILE                   ( WM_USER + 2310 )
#define CMM_INSERTFILE                 ( WM_USER + 2320 )
#define CMM_INSERTTEXT                 ( WM_USER + 2330 )
#define CMM_SETTEXT                    ( WM_USER + 2335 )
#define CMM_REPLACETEXT                ( WM_USER + 2340 )
#define CMM_GETTEXTLENGTH              ( WM_USER + 2350 )
#define CMM_GETTEXT                    ( WM_USER + 2360 )
#define CMM_GETLINECOUNT               ( WM_USER + 2365 )
#define CMM_GETLINE                    ( WM_USER + 2370 )
#define CMM_GETLINELENGTH              ( WM_USER + 2380 )
#define CMM_GETWORD                    ( WM_USER + 2381 )
#define CMM_GETWORDLENGTH              ( WM_USER + 2382 )
#define CMM_ADDTEXT                    ( WM_USER + 2390 )
#define CMM_DELETELINE                 ( WM_USER + 2400 )
#define CMM_INSERTLINE                 ( WM_USER + 2410 )
#define CMM_GETSEL                     ( WM_USER + 2420 )
#define CMM_GETSELFROMPOINT            ( WM_USER + 2425 )
#define CMM_SETSEL                     ( WM_USER + 2430 )
#define CMM_SELECTLINE                 ( WM_USER + 2435 )
#define CMM_DELETESEL                  ( WM_USER + 2440 )
#define CMM_REPLACESEL                 ( WM_USER + 2450 )
#define CMM_ISMODIFIED                 ( WM_USER + 2460 )
#define CMM_SETMODIFIED                ( WM_USER + 2461 )
#define CMM_ENABLECRLF                 ( WM_USER + 2470 )
#define CMM_ISCRLFENABLED              ( WM_USER + 2480 )
#define CMM_SETFONTOWNERSHIP           ( WM_USER + 2485 )
#define CMM_GETFONTOWNERSHIP           ( WM_USER + 2486 )
#define CMM_EXECUTECMD                 ( WM_USER + 2700 )
#define CMM_SETSPLITTERPOS             ( WM_USER + 2900 )
#define CMM_GETSPLITTERPOS             ( WM_USER + 2901 )
#define CMM_SETAUTOINDENTMODE          ( WM_USER + 3100 )
#define CMM_GETAUTOINDENTMODE          ( WM_USER + 3110 )
#define CMM_CANUNDO                    ( WM_USER + 3300 )
#define CMM_CANREDO                    ( WM_USER + 3310 )
#define CMM_CANCUT                     ( WM_USER + 3320 )
#define CMM_CANCOPY                    ( WM_USER + 3330 )
#define CMM_CANPASTE                   ( WM_USER + 3340 )
#define CMM_REDO                       ( WM_USER + 3350 )
#define CMM_CLEARUNDOBUFFER            ( WM_USER + 3351 )
#define CMM_UNDO                       ( WM_USER + 3360 )
#define CMM_CUT                        ( WM_USER + 3370 )
#define CMM_COPY                       ( WM_USER + 3380 )
#define CMM_PASTE                      ( WM_USER + 3390 )
#define CMM_SETUNDOLIMIT               ( WM_USER + 3400 )
#define CMM_GETUNDOLIMIT               ( WM_USER + 3410 )
#define CMM_GETVIEWCOUNT               ( WM_USER + 3600 )
#define CMM_GETCURRENTVIEW             ( WM_USER + 3610 )
#define CMM_SHOWSCROLLBAR              ( WM_USER + 3700 )
#define CMM_HASSCROLLBAR               ( WM_USER + 3710 )
#define CMM_ENABLESPLITTER             ( WM_USER + 3720 )
#define CMM_ISSPLITTERENABLED          ( WM_USER + 3730 )
#define CMM_ISRECORDINGMACRO           ( WM_USER + 3731 )
#define CMM_ISPLAYINGMACRO             ( WM_USER + 3732 )
#define CMM_ENABLEGLOBALPROPS          ( WM_USER + 3740 )
#define CMM_ISGLOBALPROPSENABLED       ( WM_USER + 3741 )
#define CMM_SETDLGPARENT               ( WM_USER + 3750 )

// 2.0
#define CMM_ENABLESELBOUNDS            ( WM_USER + 3760 )
#define CMM_ISSELBOUNDSENABLED         ( WM_USER + 3770 )
#define CMM_SETFONTSTYLES              ( WM_USER + 3780 )
#define CMM_GETFONTSTYLES              ( WM_USER + 3790 )
#define CMM_ENABLEREGEXP               ( WM_USER + 3800 )
#define CMM_ISREGEXPENABLED            ( WM_USER + 3810 )
#define CMM_SETITEMDATA                ( WM_USER + 3820 )
#define CMM_GETITEMDATA                ( WM_USER + 3830 )
#define CMM_SETLINESTYLE               ( WM_USER + 3840 )
#define CMM_GETLINESTYLE               ( WM_USER + 3850 )
#define CMM_SETBOOKMARK                ( WM_USER + 3860 )
#define CMM_GETBOOKMARK                ( WM_USER + 3870 )
#define CMM_SETALLBOOKMARKS            ( WM_USER + 3880 )
#define CMM_GETALLBOOKMARKS            ( WM_USER + 3890 )
#define CMM_SETLINENUMBERING           ( WM_USER + 3900 )
#define CMM_GETLINENUMBERING           ( WM_USER + 3910 )
#define CMM_POSFROMCHAR                ( WM_USER + 3920 )
#define CMM_ENABLEHIDESEL              ( WM_USER + 3930 )
#define CMM_ISHIDESELENABLED           ( WM_USER + 3940 )
#define CMM_SETHIGHLIGHTEDLINE         ( WM_USER + 3950 )
#define CMM_GETHIGHLIGHTEDLINE         ( WM_USER + 3960 )
#define CMM_ENABLENORMALIZECASE        ( WM_USER + 3970 )
#define CMM_ISNORMALIZECASEENABLED     ( WM_USER + 3980 )
#define CMM_SETDIVIDER                 ( WM_USER + 3990 )
#define CMM_GETDIVIDER                 ( WM_USER + 4000 )
#define CMM_ENABLEOVERTYPECARET        ( WM_USER + 4010 )
#define CMM_ISOVERTYPECARETENABLED     ( WM_USER + 4020 )
#define CMM_SETFINDTEXT                ( WM_USER + 4030 )
#define CMM_GETFINDTEXT                ( WM_USER + 4040 )
#define CMM_SETREPLACETEXT             ( WM_USER + 4050 )
#define CMM_GETREPLACETEXT             ( WM_USER + 4060 )
#define CMM_SETIMAGELIST               ( WM_USER + 4070 )
#define CMM_GETIMAGELIST               ( WM_USER + 4080 )
#define CMM_SETMARGINIMAGES            ( WM_USER + 4090 )
#define CMM_GETMARGINIMAGES            ( WM_USER + 4100 )
#define CMM_ABOUTBOX                   ( WM_USER + 4110 )
#define CMM_PRINT                      ( WM_USER + 4120 )
#define CMM_SETCARETPOS                ( WM_USER + 4130 )
#define CMM_VIEWCOLTOBUFFERCOL         ( WM_USER + 4140 )
#define CMM_BUFFERCOLTOVIEWCOL         ( WM_USER + 4150 )

// 2.1
#define CMM_SETBORDERSTYLE             ( WM_USER + 4160 )
#define CMM_GETBORDERSTYLE             ( WM_USER + 4170 )
#define CMM_SETCURRENTVIEW             ( WM_USER + 4180 )

/////////////////////////////////////////////////////////////
//
// Notifications
//
//
#define CMN_CHANGE                     100      // buffer contents changed
#define CMN_HSCROLL                    110      // horizontal scrollbar pos changed
#define CMN_VSCROLL                    120      // vertical scrollbar pos changed
#define CMN_SELCHANGE                  130      // active selection has changed
#define CMN_VIEWCHANGE                 140      // the user switched to a different splitter view
#define CMN_MODIFIEDCHANGE             150      // the modified state of the buffer has changed
#define CMN_SHOWPROPS                  160      // the user is about to show the props window
#define CMN_PROPSCHANGE                170      // the user changed properties via the props window
#define CMN_CREATE                     180      // the control has been created
#define CMN_DESTROY                    190      // the control is about to be destroyed

// 2.0
#define CMN_DRAWLINE                   200      // owner-draw functionality
#define CMN_DELETELINE                 210      // line is about to be deleted
#define CMN_CMDFAILURE                 220      // a keystroke command failed to successfully complete
#define CMN_REGISTEREDCMD              230      // a registered command has been invoked by the user
#define CMN_KEYDOWN                    240      // a key was pressed (WM_KEYDOWN)
#define CMN_KEYUP                      250      // a key was released (WM_KEYUP)
#define CMN_KEYPRESS                   260      // a key was pressed and released (WM_CHAR)
#define CMN_MOUSEDOWN                  270      // a mouse button was pressed (WM_xBUTTONDOWN)
#define CMN_MOUSEUP                    280      // a mouse button was released (WM_xBUTTONUP)
#define CMN_MOUSEMOVE                  290      // the mouse pointer was moved (WM_MOUSEMOVE)
#define CMN_OVERTYPECHANGE             300      // the overtype mode has changed

// 2.1
#define CMN_FINDWRAPPED                310      // a find operation wrapped to start/end of buffer

/////////////////////////////////////////////////////////////
//
// Edit commands
//
//

#define CMD_FIRST                       100
#define CMD_WORDUPPERCASE               ( CMD_FIRST + 0 )
#define CMD_WORDTRANSPOSE               ( CMD_FIRST + 1 )
#define CMD_WORDRIGHTEXTEND             ( CMD_FIRST + 2 )
#define CMD_WORDRIGHT                   ( CMD_FIRST + 3 )
#define CMD_WORDENDRIGHT                ( CMD_FIRST + 4 )
#define CMD_WORDENDRIGHTEXTEND          ( CMD_FIRST + 5 )
#define CMD_WORDLOWERCASE               ( CMD_FIRST + 6 )
#define CMD_WORDLEFTEXTEND              ( CMD_FIRST + 7 )
#define CMD_WORDLEFT                    ( CMD_FIRST + 8 )
#define CMD_WORDENDLEFT                 ( CMD_FIRST + 9 )
#define CMD_WORDENDLEFTEXTEND           ( CMD_FIRST + 10 )
#define CMD_WORDDELETETOSTART           ( CMD_FIRST + 11 )
#define CMD_WORDDELETETOEND             ( CMD_FIRST + 12 )
#define CMD_WORDCAPITALIZE              ( CMD_FIRST + 13 )
#define CMD_WINDOWSTART                 ( CMD_FIRST + 14 )
#define CMD_WINDOWSCROLLUP              ( CMD_FIRST + 15 )
#define CMD_WINDOWSCROLLTOTOP           ( CMD_FIRST + 16 )
#define CMD_WINDOWSCROLLTOCENTER        ( CMD_FIRST + 17 )
#define CMD_WINDOWSCROLLTOBOTTOM        ( CMD_FIRST + 18 )
#define CMD_WINDOWSCROLLRIGHT           ( CMD_FIRST + 19 )
#define CMD_WINDOWSCROLLLEFT            ( CMD_FIRST + 20 )
#define CMD_WINDOWSCROLLDOWN            ( CMD_FIRST + 21 )
#define CMD_WINDOWRIGHTEDGE             ( CMD_FIRST + 22 )
#define CMD_WINDOWLEFTEDGE              ( CMD_FIRST + 23 )
#define CMD_WINDOWEND                   ( CMD_FIRST + 24 )
#define CMD_UPPERCASESELECTION          ( CMD_FIRST + 25 )
#define CMD_UNTABIFYSELECTION           ( CMD_FIRST + 26 )
#define CMD_UNINDENTSELECTION           ( CMD_FIRST + 27 )
#define CMD_UNDOCHANGES                 ( CMD_FIRST + 28 )
#define CMD_UNDO                        ( CMD_FIRST + 29 )
#define CMD_TABIFYSELECTION             ( CMD_FIRST + 30 )
#define CMD_SENTENCERIGHT               ( CMD_FIRST + 31 )
#define CMD_SENTENCELEFT                ( CMD_FIRST + 32 )
#define CMD_SENTENCECUT                 ( CMD_FIRST + 33 )
#define CMD_SELECTSWAPANCHOR            ( CMD_FIRST + 34 )
#define CMD_SELECTPARA                  ( CMD_FIRST + 35 )
#define CMD_SELECTLINE                  ( CMD_FIRST + 36 )
#define CMD_SELECTALL                   ( CMD_FIRST + 37 )
#define CMD_REDOCHANGES                 ( CMD_FIRST + 38 )
#define CMD_REDO                        ( CMD_FIRST + 39 )
#define CMD_PASTE                       ( CMD_FIRST + 40 )
#define CMD_PARAUP                      ( CMD_FIRST + 41 )
#define CMD_PARADOWN                    ( CMD_FIRST + 42 )
#define CMD_PAGEUPEXTEND                ( CMD_FIRST + 43 )
#define CMD_PAGEUP                      ( CMD_FIRST + 44 )
#define CMD_PAGEDOWNEXTEND              ( CMD_FIRST + 45 )
#define CMD_PAGEDOWN                    ( CMD_FIRST + 46 )
#define CMD_LOWERCASESELECTION          ( CMD_FIRST + 47 )
#define CMD_LINEUPEXTEND                ( CMD_FIRST + 48 )
#define CMD_LINEUP                      ( CMD_FIRST + 49 )
#define CMD_LINETRANSPOSE               ( CMD_FIRST + 50 )
#define CMD_LINESTART                   ( CMD_FIRST + 51 )
#define CMD_LINEOPENBELOW               ( CMD_FIRST + 52 )
#define CMD_LINEOPENABOVE               ( CMD_FIRST + 53 )
#define CMD_LINEENDEXTEND               ( CMD_FIRST + 54 )
#define CMD_LINEEND                     ( CMD_FIRST + 55 )
#define CMD_LINEDOWNEXTEND              ( CMD_FIRST + 56 )
#define CMD_LINEDOWN                    ( CMD_FIRST + 57 )
#define CMD_LINEDELETETOSTART           ( CMD_FIRST + 58 )
#define CMD_LINEDELETETOEND             ( CMD_FIRST + 59 )
#define CMD_LINEDELETE                  ( CMD_FIRST + 60 )
#define CMD_LINECUT                     ( CMD_FIRST + 61 )
#define CMD_INDENTTOPREV                ( CMD_FIRST + 62 )
#define CMD_INDENTSELECTION             ( CMD_FIRST + 63 )
#define CMD_HOMEEXTEND                  ( CMD_FIRST + 64 )
#define CMD_HOME                        ( CMD_FIRST + 65 )
#define CMD_GOTOMATCHBRACE              ( CMD_FIRST + 66 )
#define CMD_GOTOINDENTATION             ( CMD_FIRST + 67 )
#define CMD_GOTOLINE                    ( CMD_FIRST + 68 )
#define CMD_FINDREPLACE                 ( CMD_FIRST + 69 )
#define CMD_REPLACE                     ( CMD_FIRST + 70 )
#define CMD_REPLACEALLINBUFFER          ( CMD_FIRST + 71 )
#define CMD_REPLACEALLINSELECTION       ( CMD_FIRST + 72 )
#define CMD_FINDPREVWORD                ( CMD_FIRST + 73 )
#define CMD_FINDPREV                    ( CMD_FIRST + 74 )
#define CMD_FINDNEXTWORD                ( CMD_FIRST + 75 )
#define CMD_FINDNEXT                    ( CMD_FIRST + 76 )
#define CMD_FINDMARKALL                 ( CMD_FIRST + 77 )
#define CMD_FIND                        ( CMD_FIRST + 78 )
#define CMD_SETFINDTEXT                 ( CMD_FIRST + 79 )
#define CMD_SETREPLACETEXT              ( CMD_FIRST + 80 )
#define CMD_TOGGLEPRESERVECASE          ( CMD_FIRST + 81 )
#define CMD_TOGGLEWHOLEWORD             ( CMD_FIRST + 82 )
#define CMD_TOGGLECASESENSITIVE         ( CMD_FIRST + 83 )
#define CMD_END                         ( CMD_FIRST + 84 )
#define CMD_TOGGLEWHITESPACEDISPLAY     ( CMD_FIRST + 85 )
#define CMD_TOGGLEOVERTYPE              ( CMD_FIRST + 86 )
#define CMD_SETREPEATCOUNT              ( CMD_FIRST + 87 )
#define CMD_DOCUMENTSTARTEXTEND         ( CMD_FIRST + 88 )
#define CMD_DOCUMENTSTART               ( CMD_FIRST + 89 )
#define CMD_DOCUMENTENDEXTEND           ( CMD_FIRST + 90 )
#define CMD_DOCUMENTEND                 ( CMD_FIRST + 91 )
#define CMD_DELETEHORIZONTALSPACE       ( CMD_FIRST + 92 )
#define CMD_DELETEBLANKLINES            ( CMD_FIRST + 93 )
#define CMD_DELETEBACK                  ( CMD_FIRST + 94 )
#define CMD_DELETE                      ( CMD_FIRST + 95 )
#define CMD_CUTSELECTION                ( CMD_FIRST + 96 )
#define CMD_CUT                         ( CMD_FIRST + 97 )
#define CMD_COPY                        ( CMD_FIRST + 98 )
#define CMD_CHARTRANSPOSE               ( CMD_FIRST + 99 )
#define CMD_CHARRIGHTEXTEND             ( CMD_FIRST + 100 )
#define CMD_CHARRIGHT                   ( CMD_FIRST + 101 )
#define CMD_CHARLEFTEXTEND              ( CMD_FIRST + 102 )
#define CMD_CHARLEFT                    ( CMD_FIRST + 103 )
#define CMD_BOOKMARKTOGGLE              ( CMD_FIRST + 104 )
#define CMD_BOOKMARKPREV                ( CMD_FIRST + 105 )
#define CMD_BOOKMARKNEXT                ( CMD_FIRST + 106 )
#define CMD_BOOKMARKCLEARALL            ( CMD_FIRST + 107 )
#define CMD_BOOKMARKJUMPTOFIRST         ( CMD_FIRST + 108 )
#define CMD_BOOKMARKJUMPTOLAST          ( CMD_FIRST + 109 )
#define CMD_APPENDNEXTCUT               ( CMD_FIRST + 110 )
#define CMD_INSERTCHAR                  ( CMD_FIRST + 111 )
#define CMD_NEWLINE                     ( CMD_FIRST + 112 )
#define CMD_RECORDMACRO                 ( CMD_FIRST + 113 )
#define CMD_PLAYMACRO1                  ( CMD_FIRST + 114 )
#define CMD_PLAYMACRO2                  ( CMD_FIRST + 115 )
#define CMD_PLAYMACRO3                  ( CMD_FIRST + 116 )
#define CMD_PLAYMACRO4                  ( CMD_FIRST + 117 )
#define CMD_PLAYMACRO5                  ( CMD_FIRST + 118 )
#define CMD_PLAYMACRO6                  ( CMD_FIRST + 119 )
#define CMD_PLAYMACRO7                  ( CMD_FIRST + 120 )
#define CMD_PLAYMACRO8                  ( CMD_FIRST + 121 )
#define CMD_PLAYMACRO9                  ( CMD_FIRST + 122 )
#define CMD_PLAYMACRO10                 ( CMD_FIRST + 123 )
#define CMD_PROPERTIES                  ( CMD_FIRST + 124 )
#define CMD_BEGINUNDO                   ( CMD_FIRST + 125 )
#define CMD_ENDUNDO                     ( CMD_FIRST + 126 )
#define CMD_RESERVED3                   ( CMD_FIRST + 127 )  // internal use only
// 2.0
#define CMD_TOGGLEREGEXP                ( CMD_FIRST + 128 )
#define CMD_CLEARSELECTION              ( CMD_FIRST + 129 )
#define CMD_REGEXPON                    ( CMD_FIRST + 130 )
#define CMD_REGEXPOFF                   ( CMD_FIRST + 131 )
#define CMD_WHOLEWORDON                 ( CMD_FIRST + 132 )
#define CMD_WHOLEWORDOFF                ( CMD_FIRST + 133 )
#define CMD_PRESERVECASEON              ( CMD_FIRST + 134 )
#define CMD_PRESERVECASEOFF             ( CMD_FIRST + 135 )
#define CMD_CASESENSITIVEON             ( CMD_FIRST + 136 )
#define CMD_CASESENSITIVEOFF            ( CMD_FIRST + 137 )
#define CMD_WHITESPACEDISPLAYON         ( CMD_FIRST + 138 )
#define CMD_WHITESPACEDISPLAYOFF        ( CMD_FIRST + 139 )
#define CMD_OVERTYPEON                  ( CMD_FIRST + 140 )
#define CMD_OVERTYPEOFF                 ( CMD_FIRST + 141 )
#define CMD_LAST                        ( CMD_FIRST + 141 )

// all register commands must be at or higher than CMD_USER_BASE
#define CMD_USER_BASE                   ( CMD_FIRST + 900 )

#define CMDERR_FAILURE      1     // general failure
#define CMDERR_INPUT        2     // bad input
#define CMDERR_SELECTION    3     // bad selection
#define CMDERR_NOTFOUND     4     // data not found
#define CMDERR_EMPTYBUF     5     // buffer is empty
#define CMDERR_READONLY     6     // buffer is read-only

////////////////////////////////////////////////////////////////////////////////////////
//
// Language Support for CMM_SETLANGUAGE, RegisterLanguage(), and UnregisterLanguage()
//
//

// stock languages
#define CMLANG_CPP         _T("C/C++")
#define CMLANG_PASCAL      _T("Pascal")
#define CMLANG_BASIC       _T("Basic")
#define CMLANG_SQL         _T("SQL")
#define CMLANG_JAVA        _T("Java")
#define CMLANG_HTML        _T("HTML")
#define CMLANG_XML         _T("XML")

// language styles
#define CMLS_PROCEDURAL    0
#define CMLS_SGML          1

typedef struct _CM_LANGUAGE {
    DWORD   dwStyle;                     // One of the CMLS_ values
    BOOL    bIsCaseSensitive;            // TRUE if keywords are case sensitive
    LPCTSTR pszKeywords;                 // The keywords delimited by '\n'
    LPCTSTR pszOperators;                // The operators delimited by '\n'
    LPCTSTR pszSingleLineComments;       // The single line comment tokens (e.g. "//")
    LPCTSTR pszMultiLineComments1;       // The multiline comment start tokens (e.g. "/*\n{\n(*")
    LPCTSTR pszMultiLineComments2;       // The multiline comment end tokens (e.g. "*/\n}\n*)")
    LPCTSTR pszScopeKeywords1;           // The scoping start tokens (e.g. "{\nbegin")
    LPCTSTR pszScopeKeywords2;           // The multiline comment end tokens (e.g. "}\nend")
    LPCTSTR pszStringDelims;             // The string literal delimiters (e.g. "\"\n'") -- also includes character literals
    TCHAR   chEscape;                    // The escape character
    TCHAR   chTerminator;                // The statement terminator char (usually ';')
    LPCTSTR pszTagElementNames;          // Tag-based element names delimited by '\n'
    LPCTSTR pszTagAttributeNames;        // Tag-based attribute names delimited by '\n'
    LPCTSTR pszTagEntities;              // Tag-based entities delimited by '\n'
} CM_LANGUAGE;

/////////////////////////////////////////////////////////////
//
// Color settings for CMM_GETCOLORS and CMM_SETCOLORS
//
// Note:  Use CLR_INVALID on background colors to specify transparent 
//        (text) or appropriate control panel setting
//
typedef struct _CM_COLORS {
    COLORREF crWindow;               // window background color
    COLORREF crLeftMargin;           // left margin background color
    COLORREF crBookmark;             // bookmark foreground color
    COLORREF crBookmarkBk;           // bookmark background color
    COLORREF crText;                 // plain text foreground color
    COLORREF crTextBk;               // plain text background color
    COLORREF crNumber;               // numeric literal foreground color
    COLORREF crNumberBk;             // numeric literal background color
    COLORREF crKeyword;              // keyword foreground color
    COLORREF crKeywordBk;            // keyword background color
    COLORREF crOperator;             // operator foreground color
    COLORREF crOperatorBk;           // operator background color
    COLORREF crScopeKeyword;         // scope keyword foreground color
    COLORREF crScopeKeywordBk;       // scope keyword background color
    COLORREF crComment;              // comment foreground color
    COLORREF crCommentBk;            // comment background color
    COLORREF crString;               // string foreground color
    COLORREF crStringBk;             // string background color
    COLORREF crTagText;              // plain tag text foreground color
    COLORREF crTagTextBk;            // plain tag text background color
    COLORREF crTagEntity;            // tag entity foreground color
    COLORREF crTagEntityBk;          // tag entity background color
    COLORREF crTagElementName;       // tag element name foreground color
    COLORREF crTagElementNameBk;     // tag element name background color
    COLORREF crTagAttributeName;     // tag attribute name foreground color
    COLORREF crTagAttributeNameBk;   // tag attribute name background color
    COLORREF crLineNumber;           // line number foreground color
    COLORREF crLineNumberBk;         // line number background color
    COLORREF crHDividerLines;        // line number separate line color
    COLORREF crVDividerLines;        // left margin separate line color
    COLORREF crHighlightedLine;      // highlighted line color
} CM_COLORS;

/////////////////////////////////////////////////////////////
//
// Font style settings for CMM_GETFONTSTYLES and CMM_SETFONTSTYLES
// each byte value is one of the CM_FONT_XXX values listed below
// this declaration
//
typedef struct _CM_FONTSTYLES {
    BYTE byText;                 // plain text font style
    BYTE byNumber;               // numeric literal font style
    BYTE byKeyword;              // keyword font style
    BYTE byOperator;             // operator font style
    BYTE byScopeKeyword;         // scope keyword font style
    BYTE byComment;              // comment font style
    BYTE byString;               // string font style
    BYTE byTagText;              // plain tag text font style
    BYTE byTagEntity;            // tag entity font style
    BYTE byTagElementName;       // tag element name font style
    BYTE byTagAttributeName;     // tag attribute name font style
    BYTE byLineNumber;           // line number font style
} CM_FONTSTYLES;

/////////////////////////////////////////////////////////////
//
// Font style options used in CM_FONTSTYLES
//
//
#define CM_FONT_NORMAL      0   // normal weight
#define CM_FONT_BOLD        1   // bold weight
#define CM_FONT_ITALIC      2   // normal weight, italic
#define CM_FONT_BOLDITALIC  3   // bold weight, italic
#define CM_FONT_UNDERLINE   4   // normal weight, underline

/////////////////////////////////////////////////////////////
//
// AutoIndent options
//
//
#define CM_INDENT_OFF          0    // auto-indent off -- new line begins at column 0
#define CM_INDENT_SCOPE        1    // new line begins at correct language scope indentation level
#define CM_INDENT_PREVLINE     2    // new line has identical indentation of previous line

/////////////////////////////////////////////////////////////
//
// Print option flags used with CMM_PRINT
//
//
#define CM_PRINT_PROMPTDLG    0x000   // display the print common dialog
#define CM_PRINT_DEFAULTPRN   0x001   // use default printer (no print dialog displayed)
#define CM_PRINT_HDC          0x002   // use HDC provided
#define CM_PRINT_RICHFONTS    0x004   // use bold, italics, underline, etc. when appropriate
#define CM_PRINT_COLOR        0x008   // print in color
#define CM_PRINT_PAGENUMS     0x010   // print 'page # of #' at the bottom of the page
#define CM_PRINT_DATETIME     0x020   // print date and time at top of the page
#define CM_PRINT_BORDERTHIN   0x040   // surround text with a thin border
#define CM_PRINT_BORDERTHICK  0x080   // surround text with a thick border
#define CM_PRINT_BORDERDOUBLE 0x100   // surround text with two thin borders
#define CM_PRINT_SELECTION    0x200   // print the selection rather than entire edit contents

/////////////////////////////////////////////////////////////
//
// Border option flags used with CMM_GETBORDERSTYLE and CMM_SETBORDERSTYLE.
// Note: this values may be or'd together to achieve different effects.
//
#define CM_BORDER_NONE        0x0  // no border
#define CM_BORDER_THIN        0x1  // 1-pixel border
#define CM_BORDER_CLIENT      0x2  // client edge (WS_EX_CLIENTEDGE)
#define CM_BORDER_STATIC      0x4  // static edge (WS_EX_STATICEDGE)
#define CM_BORDER_MODAL       0x8  // modal edge (WS_EX_DLGMODALFRAME)
#define CM_BORDER_CORRAL      (CM_BORDER_MODAL|CM_BORDER_CLIENT)


/////////////////////////////////////////////////////////////
//
// Text position indicators
//
//
typedef struct _CM_POSITION {
    int nLine;   // zero-based line number
    int nCol;   // zero-based *buffer* column number
} CM_POSITION;

typedef struct _CM_RANGE {
    CM_POSITION posStart;  // the anchor
    CM_POSITION posEnd;    // the extension (if same as anchor, selection is empty)
    BOOL bColumnSel;       // TRUE if is a column selection, FALSE if paragragh selection
} CM_RANGE;

/////////////////////////////////////////////////////////////
//
// Hot key descriptor
//
//
typedef struct _CM_HOTKEY {
    BYTE byModifiers1;  // 1st keystroke's modifiers (combination of HOTKEYF_ALT, HOTKEYF_SHIFT, HOTKEYF_CONTROL)
    UINT nVirtKey1;     // 1st keystroke's virtkey (e.g. Ctrl + 'A')
    BYTE byModifiers2;  // 2nd keystroke's modifiers (combination of HOTKEYF_ALT, HOTKEYF_SHIFT, HOTKEYF_CONTROL)
    UINT nVirtKey2;     // 2nd keystroke's virtkey (e.g. Ctrl + 'A')
} CM_HOTKEY;

/////////////////////////////////////////////////////////////
//
// CMN_DRAWLINE notification data passed to parent window
//
//
typedef struct _CM_DRAWLINEDATA {
    NMHDR hdr;          // standard notification data
    HDC hDC;            // device context to draw with, clipped to current paint area
    RECT rcLine;        // bounds of line, excluding left margin region.
    int nLine;          // line number to draw
    int nLeftCol;       // leftmost visible col
    int nRightCol;      // rightmost visible col
    LPARAM lParam;      // itemdata associated with the line (CMM_SETITEMDATA)
    DWORD dwStyle;      // line style bits (CML_XXX)
} CM_DRAWLINEDATA;

/////////////////////////////////////////////////////////////
//
// CMN_DELETELINE notification data passed to parent window
//
//
typedef struct _CM_DELETELINEDATA {
    NMHDR hdr;          // standard notification data
    int nLine;          // line that is about to be deleted
    LPARAM lParam;      // itemdata associated with the line (CMM_SETITEMDATA)
} CM_DELETELINEDATA;

/////////////////////////////////////////////////////////////
//
// CMN_CMDFAILURE notification data passed to parent window
//
//
typedef struct _CM_CMDFAILUREDATA {
    NMHDR hdr;          // standard notification data
    WORD wCmd;          // CMD_XXX command that failed
    DWORD dwErr;        // CMDERR_XXX failure code
} CM_CMDFAILUREDATA;


/////////////////////////////////////////////////////////////
//
// CMN_REGISTEREDCMD notification data passed to parent window
//
//
typedef struct _CM_REGISTEREDCMDDATA {
    NMHDR hdr;          // standard notification data
    WORD wCmd;          // registered command to be executed
} CM_REGISTEREDCMDDATA;

/////////////////////////////////////////////////////////////
//
// CMN_KEYDOWN, CMN_KEYUP, CMN_KEYPRESS notification 
// data passed to parent window
//
//
#define CM_KEY_SHIFT      0x1
#define CM_KEY_CTRL       0x2
#define CM_KEY_ALT        0x4
typedef struct _CM_KEYDATA {
    NMHDR hdr;          // standard notification data
    int nKeyCode;       // virtkey if CMN_KEYUP or CMN_KEYDOWN.  Ascii code if CMN_KEYPRESS
    int nKeyModifier;   // bitfield of: CM_KEY_SHIFT, CM_KEY_CTRL, and/or CM_KEY_ALT
} CM_KEYDATA;

/////////////////////////////////////////////////////////////
//
// CMN_MOUSEDOWN, CMN_MOUSEUP, CMN_MOUSEPRESS notification 
// data passed to parent window
//
//
#define CM_BTN_LEFT      0x1
#define CM_BTN_RIGHT     0x2
#define CM_BTN_MIDDLE    0x4
typedef struct _CM_MOUSEDATA {
    NMHDR hdr;          // standard notification data
    POINT pt;           // position of mouse (client coordinates)
    int nButton;        // bitfield of: CM_BTN_LEFT, CM_BTN_RIGHT, and/or CM_BTN_MIDDLE
    int nKeyModifier;   // bitfield of: CM_KEY_SHIFT, CM_KEY_CTRL, and/or CM_KEY_ALT
} CM_MOUSEDATA;

/////////////////////////////////////////////////////////////
//
// CMN_FINDWRAPPED notification data passed to parent window
//
//
typedef struct _CM_FINDWRAPPEDDATA {
    NMHDR hdr;          // standard notification data
	WORD wCmd;			// the command being executed
    BOOL bForward;      // TRUE if wrapped while searching forward, FALSE if searching backward
} CM_FINDWRAPPEDDATA;

/////////////////////////////////////////////////////////////
//
// data passed to CMM_SETLINENUMBERING
//
//
#define CM_BINARY            2
#define CM_OCTAL             8
#define CM_DECIMAL           10
#define CM_HEXADECIMAL       16

typedef struct _CM_LINENUMBERING {
    BOOL bEnabled;      // TRUE to enable, FALSE to disable
    UINT nStartAt;      // # of line at top of buffer (line 0)
    DWORD dwStyle;      // CM_DECIMAL, CM_HEXADECIMAL, CM_OCTAL, or CM_BINARY
} CM_LINENUMBERING;
                  
/////////////////////////////////////////////////////////////
//
// Error codes
//
//
typedef LRESULT CME_CODE;

#define CME_SUCCESS        1        // function or method completed successfully
#define CME_FAILURE        0        // function or method did not complete because of an error
#define CME_BADARGUMENT    -1       // function or method did not complete because an invalid argument was passed in

/////////////////////////////////////////////////////////////
//
// CMM_HITTEST return codes
//
//

#define CM_NOWHERE         0        // Not over the CodeMax control 
#define CM_HSPLITTER       1        // Over the horizontal splitter bar
#define CM_VSPLITTER       2        // Over the vertical splitter bar
#define CM_HVSPLITTER      3        // Over the intersection of the horizontal and vertical splitter bar
#define CM_EDITSPACE       4        // Over the buffer contents (code)
#define CM_HSCROLLBAR      5        // Over the horizontal scrollbar
#define CM_VSCROLLBAR      6        // Over the vertical scrollbar
#define CM_SIZEBOX         7        // Over the sizebox visible when both scrollbars are visible
#define CM_LEFTMARGIN      8        // Over the left margin area

/////////////////////////////////////////////////////////////
//
// Exported functions
//
//

#define CM_VERSION 0x02100
#ifdef __cplusplus
EXTERN_C CME_CODE CMRegisterControl( DWORD dwVersion = CM_VERSION );
#else
EXTERN_C CME_CODE CMRegisterControl( DWORD dwVersion );
#endif

EXTERN_C CME_CODE CMUnregisterControl();
EXTERN_C void CMSetFindReplaceMRUList( LPCTSTR pszMRUList, BOOL bFind );
EXTERN_C void CMGetFindReplaceMRUList( LPTSTR pszMRUList, BOOL bFind );
EXTERN_C CME_CODE CMRegisterLanguage( LPCTSTR pszName, CM_LANGUAGE *pLang );
EXTERN_C CME_CODE CMUnregisterLanguage( LPCTSTR pszName );
EXTERN_C int CMGetHotKeys( LPBYTE pBuff );
EXTERN_C CME_CODE CMSetHotKeys( const LPBYTE pBuff );
EXTERN_C void CMResetDefaultHotKeys();
EXTERN_C int CMGetHotKeysForCmd( WORD wCmd, CM_HOTKEY *pHotKeys );
EXTERN_C CME_CODE CMRegisterHotKey( CM_HOTKEY *pHotKey, WORD wCmd );
EXTERN_C CME_CODE CMUnregisterHotKey( CM_HOTKEY *pHotKey );
EXTERN_C void CMGetCommandString( WORD wCmd, BOOL bDescription, LPTSTR pszBuff, int nBuffLen );
EXTERN_C int CMGetMacro( int nMacro, LPBYTE pMacroBuff );
EXTERN_C int CMSetMacro( int nMacro, const LPBYTE pMacroBuff );

// 2.0
EXTERN_C CME_CODE CMUnregisterAllLanguages();
EXTERN_C int CMGetLanguageDef( LPCTSTR pszName, CM_LANGUAGE *pLang );
EXTERN_C CME_CODE CMRegisterCommand( WORD wCmd, LPCTSTR pszName, LPCTSTR pszDesc );
EXTERN_C CME_CODE CMUnregisterCommand( WORD wCmd );

///////////////////////////////////////////////////////////////////
//
// Message crackers for all CM_xxxx messages
//
//

#define CM_SetLanguage( hWnd, pszName ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETLANGUAGE, 0, ( LPARAM ) ( pszName ) ) )
#define CM_GetLanguage( hWnd, pszName ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_GETLANGUAGE, 0, ( LPARAM ) ( pszName ) ) )
#define CM_EnableColorSyntax( hWnd, bEnable ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ENABLECOLORSYNTAX, ( WPARAM ) ( bEnable ), 0 ) )
#define CM_IsColorSyntaxEnabled( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_ISCOLORSYNTAXENABLED, 0, 0 ) )
#define CM_OpenFile( hWnd, pszFileName ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_OPENFILE, 0, ( LPARAM ) ( pszFileName ) ) )
#define CM_InsertFile( hWnd, pszFileName, pPos ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_INSERTFILE, ( WPARAM ) ( pPos ), ( LPARAM ) ( pszFileName ) ) )
#define CM_InsertText( hWnd, pszText, pPos ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_INSERTTEXT, ( WPARAM ) ( pPos ), ( LPARAM ) ( pszText ) ) )
#define CM_SetText( hWnd, pszText ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETTEXT, 0, ( LPARAM ) ( pszText ) ) )
#define CM_SetColors( hWnd, pColors ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETCOLORS, 0, ( LPARAM ) ( pColors ) ) )
#define CM_GetColors( hWnd, pColors ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_GETCOLORS, 0, ( LPARAM ) ( pColors ) ) )
#define CM_EnableWhitespaceDisplay( hWnd, bEnable ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ENABLEWHITESPACEDISPLAY, ( WPARAM ) ( bEnable ), 0 ) )
#define CM_IsWhitespaceDisplayEnabled( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_ISWHITESPACEDISPLAYENABLED, 0, 0 ) )
#define CM_EnableTabExpand( hWnd, bEnable ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ENABLETABEXPAND, ( WPARAM ) ( bEnable ), 0 ) )
#define CM_IsTabExpandEnabled( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_ISTABEXPANDENABLED, 0, 0 ) )
#define CM_EnableSmoothScrolling( hWnd, bEnable ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ENABLESMOOTHSCROLLING, ( WPARAM ) ( bEnable ), 0 ) )
#define CM_IsSmoothScrollingEnabled( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_ISSMOOTHSCROLLINGENABLED, 0, 0 ) )
#define CM_SetTabSize( hWnd, nTabSize ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETTABSIZE, ( WPARAM ) ( nTabSize ), 0 ) )
#define CM_GetTabSize( hWnd ) \
                ( ( int ) ::SendMessage( hWnd, CMM_GETTABSIZE, 0, 0 ) )
#define CM_SetReadOnly( hWnd, bReadOnly ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETREADONLY, ( WPARAM ) ( bReadOnly ), 0 ) )
#define CM_IsReadOnly( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_ISREADONLY, 0, 0 ) )
#define CM_EnableLineToolTips( hWnd, bEnable ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ENABLELINETOOLTIPS, ( WPARAM ) ( bEnable ), 0 ) )
#define CM_IsLineToolTipsEnabled( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_ISLINETOOLTIPSENABLED, 0, 0 ) )
#define CM_EnableLeftMargin( hWnd, bEnable ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ENABLELEFTMARGIN, ( WPARAM ) ( bEnable ), 0 ) )
#define CM_IsLeftMarginEnabled( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_ISLEFTMARGINENABLED, 0, 0 ) )
#define CM_SaveFile( hWnd, pszFileName, bClearUndo ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SAVEFILE, ( WPARAM ) ( bClearUndo ), ( LPARAM ) ( pszFileName ) ) )
#define CM_ReplaceText( hWnd, pszText, pRange ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_REPLACETEXT, ( WPARAM ) ( pRange ), ( LPARAM ) ( pszText ) ) )
#define CM_GetTextLength( hWnd, pRange, bLogical ) \
                ( ( int ) ::SendMessage( hWnd, CMM_GETTEXTLENGTH, ( WPARAM ) ( pRange ), ( LPARAM ) ( bLogical ) ) )
#define CM_GetText( hWnd, pszBuff, pRange ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_GETTEXT, ( WPARAM ) ( pRange ), ( LPARAM ) ( pszBuff ) ) )
#define CM_GetLineCount( hWnd ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_GETLINECOUNT, 0, 0 ) )
#define CM_GetLine( hWnd, nLine, pszBuff ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_GETLINE, ( WPARAM ) ( nLine ), ( LPARAM ) ( pszBuff ) ) )
#define CM_GetLineLength( hWnd, nLine, bLogical ) \
                ( ( int ) ::SendMessage( hWnd, CMM_GETLINELENGTH, ( WPARAM ) ( nLine ), ( LPARAM ) ( bLogical ) ) )
#define CM_GetWord( hWnd, pszBuff, pPos ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_GETWORD, ( WPARAM ) ( pPos ), ( LPARAM ) ( pszBuff ) ) )
#define CM_GetWordLength( hWnd, pPos, bLogical ) \
                ( ( int ) ::SendMessage( hWnd, CMM_GETWORDLENGTH, ( WPARAM ) ( pPos ), ( LPARAM ) ( bLogical ) ) )
#define CM_AddText( hWnd, pszText ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ADDTEXT, 0, ( LPARAM ) ( pszText ) ) )
#define CM_DeleteLine( hWnd, nLine ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_DELETELINE, ( WPARAM ) ( nLine ), 0 ) )
#define CM_InsertLine( hWnd, nLine, pszText ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_INSERTLINE, ( WPARAM ) ( nLine ), ( LPARAM ) ( pszText ) ) )
#define CM_GetSel( hWnd, pRange, bNormalized ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_GETSEL, ( WPARAM ) ( bNormalized ), ( LPARAM ) ( pRange ) ) )
#define CM_SetSel( hWnd, pRange, bMakeVisible ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETSEL, ( WPARAM ) ( bMakeVisible ), ( LPARAM ) ( pRange ) ) )
#define CM_DeleteSel( hWnd ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_DELETESEL, 0, 0 ) )
#define CM_ReplaceSel( hWnd, pszText ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_REPLACESEL, 0, ( LPARAM ) ( pszText ) ) )
#define CM_ExecuteCmd( hWnd, wCmd, dwCmdData ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_EXECUTECMD, ( WPARAM ) ( wCmd ), ( LPARAM ) ( dwCmdData ) ) )
#define CM_SetSplitterPos( hWnd, bHorz, nPos ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETSPLITTERPOS, ( WPARAM ) ( bHorz ), ( LPARAM ) ( nPos ) ) )
#define CM_GetSplitterPos( hWnd, bHorz ) \
                ( ( int ) ::SendMessage( hWnd, CMM_GETSPLITTERPOS, ( WPARAM ) ( bHorz ), 0 ) )
#define CM_SetAutoIndentMode( hWnd, nMode ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETAUTOINDENTMODE, ( WPARAM ) ( nMode ), 0 ) )
#define CM_GetAutoIndentMode( hWnd ) \
                ( ( int ) ::SendMessage( hWnd, CMM_GETAUTOINDENTMODE, 0, 0 ) )
#define CM_CanUndo( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_CANUNDO, 0, 0 ) )
#define CM_CanRedo( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_CANREDO, 0, 0 ) )
#define CM_Redo( hWnd ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_REDO, 0, 0 ) )
#define CM_Undo( hWnd ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_UNDO, 0, 0 ) )
#define CM_ClearUndoBuffer( hWnd ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_CLEARUNDOBUFFER, 0, 0 ) )
#define CM_CanCut( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_CANCUT, 0, 0 ) )
#define CM_CanCopy( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_CANCOPY, 0, 0 ) )
#define CM_CanPaste( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_CANPASTE, 0, 0 ) )
#define CM_Cut( hWnd ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_CUT, 0, 0 ) )
#define CM_Copy( hWnd ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_COPY, 0, 0 ) )
#define CM_Paste( hWnd ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_PASTE, 0, 0 ) )
#define CM_IsRecordingMacro( hWnd ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ISRECORDINGMACRO, 0, 0 ) )
#define CM_IsPlayingMacro( hWnd ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ISPLAYINGMACRO, 0, 0 ) )
#define CM_SetUndoLimit( hWnd, nLimit ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETUNDOLIMIT, ( WPARAM ) ( nLimit ), 0 ) )
#define CM_GetUndoLimit( hWnd ) \
                ( ( int ) ::SendMessage( hWnd, CMM_GETUNDOLIMIT, 0, 0 ) )
#define CM_IsModified( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_ISMODIFIED, 0, 0 ) )
#define CM_SetModified( hWnd, bModified ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETMODIFIED, ( WPARAM ) ( bModified ), 0 ) )
#define CM_EnableOvertype( hWnd, bEnable ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ENABLEOVERTYPE, ( WPARAM ) ( bEnable ), 0 ) )
#define CM_IsOvertypeEnabled( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_ISOVERTYPEENABLED, 0, 0 ) )
#define CM_EnableCaseSensitive( hWnd, bEnable ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ENABLECASESENSITIVE, ( WPARAM ) ( bEnable ), 0 ) )
#define CM_IsCaseSensitiveEnabled( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_ISCASESENSITIVEENABLED, 0, 0 ) )
#define CM_EnablePreserveCase( hWnd, bEnable ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ENABLEPRESERVECASE, ( WPARAM ) ( bEnable ), 0 ) )
#define CM_IsPreserveCaseEnabled( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_ISPRESERVECASEENABLED, 0, 0 ) )
#define CM_EnableWholeWord( hWnd, bEnable ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ENABLEWHOLEWORD, ( WPARAM ) ( bEnable ), 0 ) )
#define CM_IsWholeWordEnabled( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_ISWHOLEWORDENABLED, 0, 0 ) )
#define CM_EnableRegExp( hWnd, bEnable ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ENABLEREGEXP, ( WPARAM ) ( bEnable ), 0 ) )
#define CM_IsRegExpEnabled( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_ISREGEXPENABLED, 0, 0 ) )
#define CM_SetTopIndex( hWnd, nView, nLine ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETTOPINDEX, ( WPARAM ) ( nView ), ( LPARAM ) ( nLine ) ) )
#define CM_GetTopIndex( hWnd, nView ) \
                ( ( int ) ::SendMessage( hWnd, CMM_GETTOPINDEX, ( WPARAM ) ( nView ), 0 ) )
#define CM_GetVisibleLineCount( hWnd, nView, bFullyVisible ) \
                ( ( int ) ::SendMessage( hWnd, CMM_GETVISIBLELINECOUNT, ( WPARAM ) ( nView ), ( LPARAM ) ( bFullyVisible ) ) )
#define CM_EnableCRLF( hWnd, bEnable ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ENABLECRLF, ( WPARAM ) ( bEnable ), 0 ) )
#define CM_IsCRLFEnabled( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_ISCRLFENABLED, 0, 0 ) )
#define CM_SetFontOwnership( hWnd, bEnable ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETFONTOWNERSHIP, ( WPARAM ) ( bEnable ), 0 ) )
#define CM_GetFontOwnership( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_GETFONTOWNERSHIP, 0, 0 ) )
#define CM_GetCurrentView( hWnd ) \
                ( ( int ) ::SendMessage( hWnd, CMM_GETCURRENTVIEW, 0, 0 ) )
#define CM_SetCurrentView( hWnd, nLine ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETCURRENTVIEW, nLine, 0 ) )
#define CM_GetViewCount( hWnd ) \
                ( ( int ) ::SendMessage( hWnd, CMM_GETVIEWCOUNT, 0, 0 ) )
#define CM_ShowScrollBar( hWnd, bHorz, bShow ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SHOWSCROLLBAR, ( WPARAM ) ( bHorz ), ( LPARAM ) ( bShow ) ) )
#define CM_HasScrollBar( hWnd, bHorz ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_HASSCROLLBAR, ( WPARAM ) ( bHorz ), 0 ) )
#define CM_GetSelFromPoint( hWnd, xClient, yClient, pPos ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_GETSELFROMPOINT, MAKEWPARAM( ( xClient ), ( yClient ) ), ( LPARAM ) ( pPos ) ) )
#define CM_SelectLine( hWnd, nLine, bMakeVisible ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SELECTLINE, ( WPARAM ) ( nLine ), ( LPARAM ) ( bMakeVisible ) ) )
#define CM_HitTest( hWnd, xClient, yClient ) \
                ( ( int ) ::SendMessage( hWnd, CMM_HITTEST, MAKEWPARAM( ( xClient ), ( yClient ) ), 0 ) )
#define CM_EnableDragDrop( hWnd, bEnable ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ENABLEDRAGDROP, ( WPARAM ) ( bEnable ), 0 ) )
#define CM_IsDragDropEnabled( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_ISDRAGDROPENABLED, 0, 0 ) )
#define CM_EnableSplitter( hWnd, bHorz, bEnable ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ENABLESPLITTER, ( WPARAM ) ( bHorz ), ( LPARAM ) ( bEnable ) ) )
#define CM_IsSplitterEnabled( hWnd, bHorz ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_ISSPLITTERENABLED, ( WPARAM ) ( bHorz ), 0 ) )
#define CM_EnableColumnSel( hWnd, bEnable ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ENABLECOLUMNSEL, ( WPARAM ) ( bEnable ), 0 ) )
#define CM_IsColumnSelEnabled( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_ISCOLUMNSELENABLED, 0, 0 ) )
#define CM_EnableGlobalProps( hWnd, bEnable ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ENABLEGLOBALPROPS, ( WPARAM ) ( bEnable ), 0 ) )
#define CM_IsGlobalPropsEnabled( hWnd ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ISGLOBALPROPSENABLED, 0, 0 ) )
#define CM_SetDlgParent( hWnd, hWndParent ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETDLGPARENT, ( WPARAM ) ( hWndParent ), 0 ) )
#define CM_EnableSelBounds( hWnd, bEnable ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ENABLESELBOUNDS, ( WPARAM ) ( bEnable ), 0 ) )
#define CM_IsSelBoundsEnabled( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_ISSELBOUNDSENABLED, 0, 0 ) )
#define CM_SetFontStyles( hWnd, pFontStyles ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETFONTSTYLES, 0, ( LPARAM ) ( pFontStyles ) ) )
#define CM_GetFontStyles( hWnd, pFontStyles ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_GETFONTSTYLES, 0, ( LPARAM ) ( pFontStyles ) ) )
#define CM_SetItemData( hWnd, nLine, lParam ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETITEMDATA, ( WPARAM ) ( nLine ), ( LPARAM ) ( lParam ) ) )
#define CM_GetItemData( hWnd, nLine ) \
                ( ( LPARAM ) ::SendMessage( hWnd, CMM_GETITEMDATA, ( WPARAM ) ( nLine ), 0 ) )
#define CM_SetLineStyle( hWnd, nLine, dwStyle ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETLINESTYLE, ( WPARAM ) ( nLine ), ( LPARAM ) ( dwStyle ) ) )
#define CM_GetLineStyle( hWnd, nLine ) \
                ( ( DWORD ) ::SendMessage( hWnd, CMM_GETLINESTYLE, ( WPARAM ) ( nLine ), 0 ) )
#define CM_SetBookmark( hWnd, nLine, bEnable ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETBOOKMARK, ( WPARAM ) ( nLine ), ( LPARAM ) ( bEnable ) ) )
#define CM_GetBookmark( hWnd, nLine ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_GETBOOKMARK, ( WPARAM ) ( nLine ), 0 ) )
#define CM_SetAllBookmarks( hWnd, nCount, pdwLines ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETALLBOOKMARKS, ( WPARAM ) ( nCount ), ( LPARAM ) ( pdwLines ) ) )
#define CM_GetAllBookmarks( hWnd, pdwLines ) \
                ( ( int ) ::SendMessage( hWnd, CMM_GETALLBOOKMARKS, 0, ( LPARAM ) ( pdwLines ) ) )
#define CM_SetLineNumbering( hWnd, pNumbering ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETLINENUMBERING, 0, ( LPARAM ) ( pNumbering ) ) )
#define CM_GetLineNumbering( hWnd, pNumbering ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_GETLINENUMBERING, 0, ( LPARAM ) ( pNumbering ) ) )
#define CM_PosFromChar( hWnd, pPos, pRect ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_POSFROMCHAR, ( WPARAM ) ( pPos ), ( LPARAM ) ( pRect ) ) )
#define CM_EnableHideSel( hWnd, bEnable ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ENABLEHIDESEL, ( WPARAM ) ( bEnable ), 0 ) )
#define CM_IsHideSelEnabled( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_ISHIDESELENABLED, 0, 0 ) )
#define CM_SetHighlightedLine( hWnd, nLine ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETHIGHLIGHTEDLINE, ( WPARAM ) ( nLine ), 0 ) )
#define CM_GetHighlightedLine( hWnd ) \
                ( ( int ) ::SendMessage( hWnd, CMM_GETHIGHLIGHTEDLINE, 0, 0 ) )
#define CM_EnableNormalizeCase( hWnd, bEnable ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ENABLENORMALIZECASE, ( WPARAM ) ( bEnable ), 0 ) )
#define CM_IsNormalizeCaseEnabled( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_ISNORMALIZECASEENABLED, 0, 0 ) )
#define CM_SetDivider( hWnd, nLine, bEnable ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETDIVIDER, ( WPARAM ) ( nLine ), ( LPARAM ) ( bEnable ) ) )
#define CM_GetDivider( hWnd, nLine ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_GETDIVIDER, ( WPARAM ) ( nLine ), 0 ) )
#define CM_EnableOvertypeCaret( hWnd, bEnable ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ENABLEOVERTYPECARET, ( WPARAM ) ( bEnable ), 0 ) )
#define CM_IsOvertypeCaretEnabled( hWnd ) \
                ( ( BOOL ) ::SendMessage( hWnd, CMM_ISOVERTYPECARETENABLED, 0, 0 ) )
#define CM_SetFindText( hWnd, pszText ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETFINDTEXT, 0, ( LPARAM ) ( pszText ) ) )
#define CM_GetFindText( hWnd, pszText ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_GETFINDTEXT, 0, ( LPARAM ) ( pszText ) ) )
#define CM_SetReplaceText( hWnd, pszText ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETREPLACETEXT, 0, ( LPARAM ) ( pszText ) ) )
#define CM_GetReplaceText( hWnd, pszText ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_GETREPLACETEXT, 0, ( LPARAM ) ( pszText ) ) )
#define CM_SetImageList( hWnd, hImageList ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETIMAGELIST, 0, ( LPARAM ) ( hImageList ) ) )
#define CM_GetImageList( hWnd ) \
                ( ( HIMAGELIST ) ::SendMessage( hWnd, CMM_GETIMAGELIST, 0, 0 ) )
#define CM_SetMarginImages( hWnd, nLine, byImages ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETMARGINIMAGES, ( WPARAM ) ( nLine ), ( LPARAM ) ( byImages ) ) )
#define CM_GetMarginImages( hWnd, nLine ) \
                ( ( BYTE ) ::SendMessage( hWnd, CMM_GETMARGINIMAGES, ( WPARAM ) ( nLine ), 0 ) )
#define CM_AboutBox( hWnd ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_ABOUTBOX, 0, 0 ) )
#define CM_Print( hWnd, hDC, dwFlags ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_PRINT, ( WPARAM ) ( hDC ), ( LPARAM ) ( dwFlags ) ) )
#define CM_SetCaretPos( hWnd, nLine, nCol ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETCARETPOS, ( WPARAM ) ( nLine ), ( LPARAM ) ( nCol ) ) )
#define CM_ViewColToBufferCol( hWnd, nLine, nViewCol ) \
                ( ( int ) ::SendMessage( hWnd, CMM_VIEWCOLTOBUFFERCOL, ( WPARAM ) ( nLine ), ( LPARAM ) ( nViewCol ) ) )
#define CM_BufferColToViewCol( hWnd, nLine, nBufferCol ) \
                ( ( int ) ::SendMessage( hWnd, CMM_BUFFERCOLTOVIEWCOL, ( WPARAM ) ( nLine ), ( LPARAM ) ( nBufferCol ) ) )
#define CM_SetBorderStyle( hWnd, dwStyle ) \
                ( ( CME_CODE ) ::SendMessage( hWnd, CMM_SETBORDERSTYLE, ( WPARAM ) ( dwStyle ), 0 ) )
#define CM_GetBorderStyle( hWnd ) \
                ( ( DWORD ) ::SendMessage( hWnd, CMM_GETBORDERSTYLE, 0, 0 ) )
#endif
