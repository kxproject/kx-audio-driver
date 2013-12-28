#include "precomp.h"
#include "cedit.h"

CLanguage **CEdit::g_pLanguages = NULL;
int CEdit::g_nLanguageCount = 0;

CM_LANGUAGE CEdit::g_LangCPP = { 
//////////////////////////////////////////////////////////////////
// language style
//////////////////////////////////////////////////////////////////
CMLS_PROCEDURAL,
//////////////////////////////////////////////////////////////////
// Is case-sensitive?
//////////////////////////////////////////////////////////////////
TRUE,
//////////////////////////////////////////////////////////////////
// keywords
//////////////////////////////////////////////////////////////////
_T("#define\n#elif\n#else\n#endif\n#error\n#if\n#ifdef\n#ifndef\n#include\n#line\n#pragma\n#printf\n#undef\nauto\nbreak\ncase\ncatch\n")
_T("char\nclass\nconst\nconst_cast\ncontinue\ndefault\ndelete\ndo\ndouble\ndynamic_cast\nelse\nenum\nexcept\nextern\nfalse\nfinally\nfloat\n")
_T("for\nfriend\ngoto\nif\ninline\nint\nlong\nmutable\nnamespace\nnew\noperator\nprivate\nprotected\npublic\nregister\nreinterpret_cast\n")
_T("return\nshort\nsigned\nsizeof\nstatic\nstatic_cast\nstruct\nswitch\ntemplate\nthis\nthrow\ntrue\ntry\ntypedef\ntypeid\nunion\nunsigned\n")
_T("using\nvirtual\nvoid\nvolatile\nwhile\nxalloc\nnew\ndelete"),
//////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////
_T("::\n.\n->\n++\n--\n*\n&\n+\n-\n!\n~\nsizeof\ntypeid\nconst_cast\ndynamic_cast\nreinterpret_cast\nstatic_cast\n.*\n->*\n*\n")
_T("/\n%\n<<\n>>\n<\n>\n<=\n>=\n==\n!=\n&\n^\n|\n&&\n||\n?\n=\n*=\n/=\n%=\n+=\n-=\n<<=\n>>=\n&=\n|=\n^="),
//////////////////////////////////////////////////////////////////
// Single line comments
//////////////////////////////////////////////////////////////////
_T("//"),
//////////////////////////////////////////////////////////////////
// Multi-line comment set
//////////////////////////////////////////////////////////////////
_T("/*"),
_T("*/"),
//////////////////////////////////////////////////////////////////
// Scope words
//////////////////////////////////////////////////////////////////
_T("{"),
_T("}"),
//////////////////////////////////////////////////////////////////
// String literal char
//////////////////////////////////////////////////////////////////
_T("\"\n'"),
//////////////////////////////////////////////////////////////////
// Escape Char
//////////////////////////////////////////////////////////////////
_T('\\'),
//////////////////////////////////////////////////////////////////
// Statement Terminator Char
//////////////////////////////////////////////////////////////////
_T(';'),
//////////////////////////////////////////////////////////////////
// Tag element names
//////////////////////////////////////////////////////////////////
NULL,
//////////////////////////////////////////////////////////////////
// Tag attribute names
//////////////////////////////////////////////////////////////////
NULL,
//////////////////////////////////////////////////////////////////
// Tag entities
//////////////////////////////////////////////////////////////////
NULL
};

CM_LANGUAGE CEdit::g_LangPascal = { 
//////////////////////////////////////////////////////////////////
// language style
//////////////////////////////////////////////////////////////////
CMLS_PROCEDURAL,
//////////////////////////////////////////////////////////////////
// Is case-sensitive?
//////////////////////////////////////////////////////////////////
FALSE,
//////////////////////////////////////////////////////////////////
// keywords
//////////////////////////////////////////////////////////////////
_T("ABSOLUTE\nAND\nARRAY\nASM\nASSEMBLER\nBOOLEAN\nBYTE\nCASE\nCHAR\nCOMP\nCONST\nCONSTRUCTOR\nDESTRUCTOR\nDIV\n")
_T("DO\nDOUBLE\nDOWNTO\nELSE\nEXTENDED\nEXTERNAL\nFALSE\nFAR\nFILE\nFOR\nFORWARD\nFUNCTION\nGOTO\n")
_T("IF\nIMPLEMENTATION\nIN\nINLINE\nINTEGER\nINTERFACE\nINTERRUPT\nLABEL\nLONGINT\nMOD\nNEAR\nNIL\nNOT\nOBJECT\nOF\n")
_T("OPERATOR\nOR\nPACKED\nPRIVATE\nPROCEDURE\nPROGRAM\nREAL\nRECORD\nREPEAT\nSET\nSHL\nSHORTINT\nSHR\nSINGLE\nSTRING\n")
_T("TEXT\nTHEN\nTO\nTRUE\nTYPE\nUNIT\nUNTIL\nUSES\nVAR\nVIRTUAL\nWHILE\nWITH\nWORD\nXOR"),
//////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////
_T("=\n<>\n<=\n>=\n>\n<\n+\n-\n*\n&\n^\n/\n:="),
//////////////////////////////////////////////////////////////////
// Single line comments
//////////////////////////////////////////////////////////////////
NULL,
//////////////////////////////////////////////////////////////////
// Multi-line comment set
//////////////////////////////////////////////////////////////////
_T("(*\n{"),
_T("*)\n}"),
//////////////////////////////////////////////////////////////////
// Scope words
//////////////////////////////////////////////////////////////////
_T("BEGIN"),
_T("END"),
//////////////////////////////////////////////////////////////////
// String literal char
//////////////////////////////////////////////////////////////////
_T("\"\n'"),
//////////////////////////////////////////////////////////////////
// Escape Char
//////////////////////////////////////////////////////////////////
_T('\0'),
//////////////////////////////////////////////////////////////////
// Statement Terminator Char
//////////////////////////////////////////////////////////////////
_T(';'),
//////////////////////////////////////////////////////////////////
// Tag element names
//////////////////////////////////////////////////////////////////
NULL,
//////////////////////////////////////////////////////////////////
// Tag attribute names
//////////////////////////////////////////////////////////////////
NULL,
//////////////////////////////////////////////////////////////////
// Tag entities
//////////////////////////////////////////////////////////////////
NULL
};

CM_LANGUAGE CEdit::g_LangBasic = { 
//////////////////////////////////////////////////////////////////
// language style
//////////////////////////////////////////////////////////////////
CMLS_PROCEDURAL,
//////////////////////////////////////////////////////////////////
// Is case-sensitive?
//////////////////////////////////////////////////////////////////
FALSE,
//////////////////////////////////////////////////////////////////
// keywords
//////////////////////////////////////////////////////////////////
_T("#Const\n#Else\n#If\nArray\nAs\nAsc\nBase\nCall\nCase\nChoose\nChr\nClose\nCls\nConst\nData\nDeclare\nDef\nDim\nDoEvents\n")
_T("Each\nElse\nElseIf\nErase\nError\nExit\nFalse\nFormat\nGet\nGoSub\nGoTo\nHex\nIf\nIn\nInput\nIsArray\n")
_T("LBound\nLCase\nLet\nMe\nModule\nNew\nOct\nOn\nOpen\nOption\nPeek\nPoke\nPreserve\nPrint\nPrivate\nProperty\nPublic\nPut\n")
_T("Read\nReDim\nRestore\nResume\nReturn\nSelect\nSet\nShared\nStatic\nStep\nStop\nStr\nSub\nSwitch\nThen\nTo\nTrue\nType\nUBound\n")
_T("UCase\nWrite"),
//////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////
_T("^\n-\n*\n/\n\\\n+\n&\n=\n<>\n<\n>\n<=\n>=\nMod\nIs\nLike\nAnd\nEqv\nImp\nNot\nOr\nXor"),
//////////////////////////////////////////////////////////////////
// Single line comments
//////////////////////////////////////////////////////////////////
_T("'\nRem"),
//////////////////////////////////////////////////////////////////
// Multi-line comment set
//////////////////////////////////////////////////////////////////
NULL,
NULL,
//////////////////////////////////////////////////////////////////
// Scope words
//////////////////////////////////////////////////////////////////
_T("Begin\nWhile\nFor\nDo\nWith\nFunction"),
_T("End\nWend\nNext\nLoop\nEnd With\nEnd Function"),
//////////////////////////////////////////////////////////////////
// String literal char
//////////////////////////////////////////////////////////////////
_T("\""),
//////////////////////////////////////////////////////////////////
// Escape Char
//////////////////////////////////////////////////////////////////
_T('\0'),
//////////////////////////////////////////////////////////////////
// Statement Terminator Char
//////////////////////////////////////////////////////////////////
_T('\0'),
//////////////////////////////////////////////////////////////////
// Tag element names
//////////////////////////////////////////////////////////////////
NULL,
//////////////////////////////////////////////////////////////////
// Tag attribute names
//////////////////////////////////////////////////////////////////
NULL,
//////////////////////////////////////////////////////////////////
// Tag entities
//////////////////////////////////////////////////////////////////
NULL
};

CM_LANGUAGE CEdit::g_LangSQL = { 
//////////////////////////////////////////////////////////////////
// language style
//////////////////////////////////////////////////////////////////
CMLS_PROCEDURAL,
//////////////////////////////////////////////////////////////////
// Is case-sensitive?
//////////////////////////////////////////////////////////////////
FALSE,
//////////////////////////////////////////////////////////////////
// keywords
//////////////////////////////////////////////////////////////////
_T("ABSOLUTE\nACTION\nADA\nADD\nALL\nALLOCATE\nALTER\nAND\nANY\nARE\nAS\nASC\nASSERTION\nAT\nAUTHORIZATION\nAVG\nBETWEEN\nBIT\n")
_T("BIT_LENGTH\nBOTH\nCARCHAR\nCARYING\nCASCADE\nCASCADED\nCASE\nCAST\nCATALOG\nCATALOG_NAME\nCHAR\nCHAR_LENGTH\nCHARACTER\nCHARACTER_LENGTH\n")
_T("CHARACTER_SET_CATALOG\nCHARACTER_SET_NAME\nCHARACTER_SET_SCHEMA\nCHECK\nCLASS_ORIGIN\nCLOSE\nCOALESCE\nCOBOL\nCOLLATE\n")
_T("COLLATION\nCOLLATION_CATALOG\nCOLLATION_NAME\nCOLLATION_SCHEMA\nCOLUMN\nCOLUMN_NAME\nCOMMAND_FUNCTION\nCOMMIT\nCOMMITTED\n")
_T("CONDITION_NUMBER\nCONNECT\nCONNECTION\nCONNECTION_NAME\nCONSTRAINT\nCONSTRAINT_CATALOG\nCONSTRAINT_NAME\nCONSTRAINT_SCHEMA\nCONSTRAINTS\nCONTINUE\n")
_T("CONVERT\nCORRESPONDING\nCOUNT\nCREATE\nCROSS\nCURRENT\nCURRENT_DATE\nCURRENT_TIME\nCURRENT_TIMSTAMP\nCURRENT_USER\nCURSOR\nCURSOR_NAME\n")
_T("DATA\nDATE\nDATETIME_INTERVAL_CODE\nDATETIME_INTERVAL_PRECISION\nDAY\nDEALLOCATE\nDEC\nDECIMAL\nDECLARE\nDEFAULT\nDEFERRABLE\n")
_T("DEFERRED\nDELETE\nDESC\nDESCRIBE\nDESCRIPTOR\nDIAGNOSTICS\nDISCONNECT\nDISTINCT\nDOMAIN\nDOUBLE\nDROP\nDYNAMIC_FUNCTION\nELSE\n")
_T("END-EXEC\nESCAPE\nEXCEPT\nEXCEPTION\nEXEC\nEXECUTE\nEXISTS\nEXTERNAL\nEXTRACT\nFETCH\nFIRST\nFLASE\nFLOAT\nFOR\nFOREIGN\n")
_T("FORTRAN\nFOUND\nFULL\nGET\nGLOBAL\nGO\nGOTO\nGRANT\nGROUP\nHAVING\nHOUR\nIDENTITY\nIMMEDIATE\nIN\nINDICATOR\nINITIALLY\nINNER\n")
_T("INPUT\nINSENSITIVE\nINSERT\nINT\nINTEGER\nINTERSECT\nINTERVAL\nINTO\nIS\nISOLATION\nJOIN\nKEY\nLANGUAGE\nLAST\nLEADING\nLEFT\n")
_T("LENGTH\nLEVEL\nLIKE\nLOCAL\nLOWER\nMATCH\nMAX\nMESSAGE_LENGTH\nMESSAGE_OCTET_LENGTH\nMESSAGE_TEXT\nMIN\nMINUTE\nMODULE\nMONTH\nMORE\n")
_T("MUMPS\nNAME\nNAMES\nNATIONAL\nNATURAL\nNCHAR\nNEXT\nNO\nNOT\nNULL\nNULLABLE\nNULLIF\nNUMBER\nNUMERIC\nOCTET_LENGTH\nOF\nON\n")
_T("ONLY\nOPEN\nOPTION\nOR\nOUTER\nOUTPUT\nOVERLAPS\nPAD\nPARTIAL\nPASCAL\nPLI\nPOSITION\nPRECISION\nPREPARE\nPRESERVE\nPRIMARY\nPRIOR\n")
_T("PRIVILEGES\nPROCEDURE\nPUBLIC\nREAD\nREAL\nREFERENCES\nRELATIVE\nREPEATABLE\nRESTRICT\nRETURNED_LENGTH\nRETURNED_OCTET_LENGTH\nRETURNED_SQLSTATE\n")
_T("REVOKE\nRIGHT\nROLLBACK\nROW\nROW_COUNT\nROWS\nSCALE\nSCHEMA\nSCHEMA_NAME\nSCROLL\nSECOND\nSECTION\nSERIALIZABLE\nSERVER_NAME\n")
_T("SESSION\nSESSION_USER\nSET\nSIZE\nSMALLINT\nSOME\nSPACE\nSQL\nSQLCODE\nSQLERROR\nSQLSTATE\nSUBCLASS_ORIGIN\nSUBSTRING\nSUM\nSYSTEM_USER\n")
_T("TABLE\nTABLE_NAME\nTEMPORARY\nTHEN\nTIME\nTIMESTAMP\nTIMEZONE_HOUR\nTIMEZONE_MINUTE\nTO\nTRAILING\nTRANSACTION\nTRANSLATE\nTRANSLATION\n")
_T("TRIM\nTRUE\nTYPE\nUNCOMMITTED\nUNION\nUNIQUE\nUNKNOWN\nUNNAMED\nUPDATE\nUPPER\nUSAGE\nUSER\nUSING\nVALUE\nVALUES\nVIEW\nWHEN\nWHENEVER\n")
_T("WITH\nWORK\nWRITE\nYEAR\nZONE"),
//////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////
_T("%\n&\n*\n+\n-\n/\n<\n=\n>\n?\n|\n#"),
//////////////////////////////////////////////////////////////////
// Single line comments
//////////////////////////////////////////////////////////////////
_T("--"),
//////////////////////////////////////////////////////////////////
// Multi-line comment set
//////////////////////////////////////////////////////////////////
_T("/*"),
_T("*/"),
//////////////////////////////////////////////////////////////////
// Scope words
//////////////////////////////////////////////////////////////////
_T("BEGIN\nSELECT\nSELECT\nSELECT\nFROM\nFROM\nWHERE\nINSERT INTO\nORDER BY"),
_T("END\nFROM\nWHERE\nORDER BY\nWHERE\nORDER BY\nORDER BY\nVALUES\nORDER BY"),
//////////////////////////////////////////////////////////////////
// String literal char
//////////////////////////////////////////////////////////////////
_T("\"\n'\n`"),
//////////////////////////////////////////////////////////////////
// Escape Char
//////////////////////////////////////////////////////////////////
_T('\0'),
//////////////////////////////////////////////////////////////////
// Statement Terminator Char
//////////////////////////////////////////////////////////////////
_T(';'),
//////////////////////////////////////////////////////////////////
// Tag element names
//////////////////////////////////////////////////////////////////
NULL,
//////////////////////////////////////////////////////////////////
// Tag attribute names
//////////////////////////////////////////////////////////////////
NULL,
//////////////////////////////////////////////////////////////////
// Tag entities
//////////////////////////////////////////////////////////////////
NULL
};

CM_LANGUAGE CEdit::g_LangJava = { 
//////////////////////////////////////////////////////////////////
// language style
//////////////////////////////////////////////////////////////////
CMLS_PROCEDURAL,
//////////////////////////////////////////////////////////////////
// Is case-sensitive?
//////////////////////////////////////////////////////////////////
TRUE,
//////////////////////////////////////////////////////////////////
// keywords
//////////////////////////////////////////////////////////////////
_T("abstract\nboolean\nbreak\nbyte\ncase\ncatch\nchar\nclass\nconst\ncontinue\ndefault\ndo\ndouble\nelse\nextends\nfinal\nfinally\nfloat\nfor\ngoto\n")
_T("if\nimplements\nimport\ninstanceof\nint\ninterface\nlong\nnative\nnew\npackage\nprivate\npublic\nshort\nstatic\nsuper\nswitch\nsynchronized\nthis\nthrow\nthrows\n")
_T("protected\nreturn\ntransient\ntry\nvoid\nvolatile\nwhile"),
//////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////
_T("=\n>\n<\n!\n~\n?\n:\n==\n<=\n>=\n!=\n&&\n||\n++\n--\n+\n-\n*\n/\n&\n|\n^\n%\n<<\n>>\n>>>\n+=\n-=\n*=\n/=\n&=\n|=\n^=\n%=\n<<=\n>>=\n>>>="),
//////////////////////////////////////////////////////////////////
// Single line comments
//////////////////////////////////////////////////////////////////
_T("//"),
//////////////////////////////////////////////////////////////////
// Multi-line comment set
//////////////////////////////////////////////////////////////////
_T("/*"),
_T("*/"),
//////////////////////////////////////////////////////////////////
// Scope words
//////////////////////////////////////////////////////////////////
_T("{"),
_T("}"),
//////////////////////////////////////////////////////////////////
// String literal char
//////////////////////////////////////////////////////////////////
_T("\"\n'"),
//////////////////////////////////////////////////////////////////
// Escape Char
//////////////////////////////////////////////////////////////////
_T('\\'),
//////////////////////////////////////////////////////////////////
// Statement Terminator Char
//////////////////////////////////////////////////////////////////
_T(';'),
//////////////////////////////////////////////////////////////////
// Tag element names
//////////////////////////////////////////////////////////////////
NULL,
//////////////////////////////////////////////////////////////////
// Tag attribute names
//////////////////////////////////////////////////////////////////
NULL,
//////////////////////////////////////////////////////////////////
 // Tag entities
//////////////////////////////////////////////////////////////////
NULL
};

CM_LANGUAGE CEdit::g_LangHTML = { 
//////////////////////////////////////////////////////////////////
// language style
//////////////////////////////////////////////////////////////////
CMLS_SGML,
//////////////////////////////////////////////////////////////////
// Is case-sensitive?
//////////////////////////////////////////////////////////////////
FALSE,
//////////////////////////////////////////////////////////////////
// keywords
//////////////////////////////////////////////////////////////////
NULL,
//////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////
_T("/\n=\n&\n;"),
//////////////////////////////////////////////////////////////////
// Single line comments
//////////////////////////////////////////////////////////////////
NULL,
//////////////////////////////////////////////////////////////////
// Multi-line comment set
//////////////////////////////////////////////////////////////////
_T("<!--\n<comment>"),
_T("-->\n</comment>"),
//////////////////////////////////////////////////////////////////
// Scope words
//////////////////////////////////////////////////////////////////
_T("<"),
_T(">"),
//////////////////////////////////////////////////////////////////
// String literal char
//////////////////////////////////////////////////////////////////
_T("\"\n'"),
//////////////////////////////////////////////////////////////////
// Escape Char
//////////////////////////////////////////////////////////////////
_T('\0'),
//////////////////////////////////////////////////////////////////
// Statement Terminator Char
//////////////////////////////////////////////////////////////////
_T('\0'),
//////////////////////////////////////////////////////////////////
// Tag element names
//////////////////////////////////////////////////////////////////
_T("a\nabbr\nacronym\naddress\napplet\narea\nb\nbase\nbasefont\nbdo\nbig\nblockquote\nbody\n")
_T("br\nbutton\ncaption\ncenter\ncite\ncode\ncol\ncolgroup\ndd\ndel\ndfn\ndir\ndiv\ndl\ndt\n")
_T("em\nfieldset\nfont\nform\nframe\nframeset\nh1\nh2\nh3\nh4\nh5\nh6\nhead\nhr\nhtml\ni\niframe\nimg\n")
_T("input\nins\nisindex\nkdb\nlabel\nlegend\nli\nlink\nmap\nmenu\nmeta\nnoframes\nnoscript\n")
_T("object\nol\noptgroup\noption\np\nparam\npre\nq\ns\nsamp\nscript\nselect\nsmall\nspan\n")
_T("strike\nstrong\nstyle\nsub\nsup\ntable\ntbody\ntd\ntextarea\ntfoot\nth\nthead\ntitle\n")
_T("tr\ntt\nu\nul\nvar"),
//////////////////////////////////////////////////////////////////
// Tag attribute names
//////////////////////////////////////////////////////////////////
_T("abbr\naccept\naccept-charset\naccesskey\naction\nalign\nalink\nalt\narchive\naxis\nbackground\n")
_T("bgcolor\nborder\ncellpadding\ncellspacing\nchar\ncharoff\ncharset\nchecked\ncite\nclass\nclassid\nclear\ncode\ncodebase\n")
_T("codetype\ncolor\ncols\ncolspan\ncompact\ncontent\ncoords\ndata\ndatetime\ndeclare\ndefer\ndir\n")
_T("disabled\ndisables\nemphasis\nenctype\nf\nface\nfieldset\nfor\nframe\nframeborder\nheaders\n")
_T("height\nhref\nhreflang\nhspace\nhttp-equiv\nid\nismap\nlabel\nlang\nlanguage\nlink\nlongdesc\nmarginheight\n")
_T("marginwidth\nmaxlength\nmedia\nmethod\nmultiple\nname\nnohref\nnoresize\nnoshade\nnowrap\nonblur\n")
_T("onchange\nonfocus\nonkeydown\nonkeyup\nonload\nonmousedown\nonmousemove\nonmouseout\nonmouseover\nonmouseup\n")
_T("onreset\nonselect\nonsubmit\nonunload\nprofile\nprompt\nreadonly\nrel\n")
_T("rev\nrows\nrowspan\nrules\nscheme\nscope\nscrolling\nselected\nshape\nsize\nspan\nsrc\nstandby\nstart\n")
_T("style\nsummary\ntabindex\ntarget\ntext\ntitle\ntype\nusemap\nvalign\nvalue\nvaluetype\nversion\n")
_T("vlink\nvspace\nwidth"),
//////////////////////////////////////////////////////////////////
// Tag entities
//////////////////////////////////////////////////////////////////
_T("&aacute;\n&acirc;\n&acute;\n&aelig;\n&agrave;\n&alefsym;\n")
_T("&alpha;\n&amp;\n&and;\n&ang;\n&aring;\n&asymp;\n&atilde;\n")
_T("&auml;\n&bdquo;\n&beta;\n&brvbar;\n&bull;\n&cap;\n&ccedil;\n")
_T("&cent;\n&chi;\n&circ;\n&clubs;\n&cong;\n&copy;\n&crarr;\n&cup;\n")
_T("&curren;\n&dagger;\n&darr;\n&deg;\n&delta;\n&diams;\n&divide;\n")
_T("&eacute;\n&ecirc;\n&egrave;\n&empty;\n&emsp;\n&ensp;\n&epsilon;\n")
_T("&equiv;\n&eta;\n&eth;\n&euml;\n&euro;\n&exist;\n")
_T("&fnof;\n&forall;\n&frac12;\n&frac14;\n&frac34;\n&frasl;\n&gamma;\n&ge;\n&gt;\n")
_T("&harr;\n&hearts;\n&hellip;\n&iacute;\n&icirc;\n&iexcl;\n&igrave;\n")
_T("&image;\n&infin;\n&int;\n&iota;\n&iquest;\n&isin;\n&iuml;\n")
_T("&kappa;\n&lambda;\n&lang;\n&laquo;\n&larr;\n&lceil;\n&ldquo;\n")
_T("&le;\n&lfloor;\n&lowast;\n&loz;\n&lrm;\n&lsaquo;\n&lsquo;\n&lt;\n&macr;\n&mdash;\n")
_T("&micro;\n&middot;\n&minus;\n&mu;\n&nabla;\n&nbsp;\n&ndash;\n&ne;\n&ni;\n")
_T("&not;\n&notin;\n&nsub;\n&ntilde;\n&nu;\n&oacute;\n&ocirc;\n")
_T("&oelig;\n&ograve;\n&oline;\n&omega;\n&omicron;\n")
_T("&oplus;\n&or;\n&ordf;\n&ordm;\n&oslash;\n&otilde;\n&otimes;\n&ouml;\n")
_T("&para;\n&part;\n&permil;\n&perp;\n&phi;\n&pi;\n&piv;\n")
_T("&plusmn;\n&pound;\n&prime;\n&prod;\n&prop;\n&psi;\n&quot;\n&radic;\n")
_T("&rang;\n&raquo;\n&rarr;\n&rceil;\n&rdquo;\n&real;\n&reg;\n&rfloor;\n&rho;\n")
_T("&rlm;\n&rsaquo;\n&rsquo;\n&sbquo;\n&scaron;\n&sdot;\n&sect;\n&shy;\n")
_T("&sigma;\n&sigmaf;\n&sim;\n&spades;\n&sub;\n&sube;\n&sum;\n&sup;\n&sup1;\n")
_T("&sup2;\n&sup3;\n&supe;\n&szlig;\n&tau;\n&there4;\n&theta;\n&thetasym;\n")
_T("&thinsp;\n&thorn;\n&tilde;\n&times;\n&trade;\n&uacute;\n&uarr;\n")
_T("&ucirc;\n&ugrave;\n&uml;\n&upsih;\n&upsilon;\n&uuml;\n")
_T("&weierp;\n&xi;\n&yacute;\n&yen;\n&yuml;\n&zeta;\n")
_T("&zwj;\n&zwnj;")
};

CM_LANGUAGE CEdit::g_LangXML = { 
//////////////////////////////////////////////////////////////////
// language style
//////////////////////////////////////////////////////////////////
CMLS_SGML,
//////////////////////////////////////////////////////////////////
// Is case-sensitive?
//////////////////////////////////////////////////////////////////
FALSE,
//////////////////////////////////////////////////////////////////
// keywords
//////////////////////////////////////////////////////////////////
_T("#IMPLIED\n#PCDATA\nPCDATA\nEMPTY\nCDATA\n#REQUIRED\n!NOTATION\n#FIXED\nANY"),
//////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////
_T("\n=\n*\n?\n|\n,\n[\n]\n(\n)\n+\n%"),
//////////////////////////////////////////////////////////////////
// Single line comments
//////////////////////////////////////////////////////////////////
NULL,
//////////////////////////////////////////////////////////////////
// Multi-line comment set
//////////////////////////////////////////////////////////////////
_T("<!--"),
_T("-->"),
//////////////////////////////////////////////////////////////////
// Scope words
//////////////////////////////////////////////////////////////////
_T("<"),
_T(">"),
//////////////////////////////////////////////////////////////////
// String literal char
//////////////////////////////////////////////////////////////////
_T("\"\n'"),
//////////////////////////////////////////////////////////////////
// Escape Char
//////////////////////////////////////////////////////////////////
_T('\0'),
//////////////////////////////////////////////////////////////////
// Statement Terminator Char
//////////////////////////////////////////////////////////////////
_T('\0'),
//////////////////////////////////////////////////////////////////
// Tag element names
//////////////////////////////////////////////////////////////////
_T("!ELEMENT\n!ATTLIST\n!DOCTYPE\n!ENTITY"),
//////////////////////////////////////////////////////////////////
// Tag attribute names
//////////////////////////////////////////////////////////////////
NULL,
//////////////////////////////////////////////////////////////////
// Tag entities
//////////////////////////////////////////////////////////////////
_T("&ge;\n&gt;\n&le;\n&lt;\n&amp;")
};


CME_CODE CEdit::RegisterLanguage( LPCTSTR pszName, CM_LANGUAGE *pLang )
{
	ASSERT( pszName );

	if ( !pLang )
	{
		// caller did not supply a language definition.  See if this
		// is a stock language for which we have a definition.
		static struct _StockLang {
			CM_LANGUAGE *pLang;
			LPCTSTR pszName;
		} Languages[] = { { &g_LangCPP, CMLANG_CPP },
						  { &g_LangPascal, CMLANG_PASCAL },
						  { &g_LangBasic, CMLANG_BASIC },
						  { &g_LangSQL, CMLANG_SQL },
						  { &g_LangJava, CMLANG_JAVA },
						  { &g_LangHTML, CMLANG_HTML },
						  { &g_LangXML, CMLANG_XML } };

		for ( int i = 0; i < ARRAY_SIZE( Languages ); i++ )
		{
			_StockLang *pLangCurr = Languages + i;
			if ( _tcsicmp( pLangCurr->pszName, pszName ) == 0 )
			{
				pLang = pLangCurr->pLang;
				break;
			}
		}
	}
	
	ASSERT( pLang );

	if ( !pLang )
	{
		return CME_BADARGUMENT;
	}

	// make sure the same language is not registered twice
	BOOL bDupe = FALSE;
    int i = 0;

	for ( i=0; i < g_nLanguageCount; i++ )
	{
		if ( _tcsicmp( g_pLanguages[ i ]->m_szName, pszName ) == 0 )
		{
			bDupe = TRUE;
			break;
		} 
	} 
	
	if ( bDupe )
	{
		delete g_pLanguages[ i ];
		g_pLanguages[ i ] = new CLanguage( pszName, pLang );
		
		// 'flash' the controls that have this language
		for ( int nWnd = 0; nWnd < g_nhWndCount; nWnd++ )
		{
			HWND hWnd = CEdit::g_phWnds[ nWnd ];
			ASSERT( IsWindow( hWnd ) );
			TCHAR szLang[ CM_MAX_LANGUAGE_NAME ];
			CM_GetLanguage( hWnd, szLang );
			if ( _tcsicmp( szLang, pszName ) == 0 )
			{
				// language in use -- re-establish the new language
				CM_SetLanguage( hWnd, NULL );
				CM_SetLanguage( hWnd, pszName );
			}
		}
	}
	else
	{
		g_pLanguages = g_pLanguages ? ( CLanguage ** ) realloc( g_pLanguages, ( g_nLanguageCount + 1 ) * sizeof( CLanguage * ) ) :
		                              ( CLanguage ** ) malloc( sizeof( CLanguage * ) );
		g_pLanguages[ g_nLanguageCount++ ] = new CLanguage( pszName, pLang );
	}
	
	return CME_SUCCESS;
}

BOOL CEdit::UnregisterLanguage( LPCTSTR pszName )
{
	ASSERT( pszName );
	ASSERT( g_pLanguages );
	BOOL bFound = FALSE;
	if ( g_pLanguages )
	{
		// find the language
        int nLanguage;

		for ( nLanguage = 0; nLanguage < g_nLanguageCount; nLanguage++ )
		{
			if ( _tcsicmp( g_pLanguages[ nLanguage ]->m_szName, pszName ) == 0 )
			{
				bFound = TRUE;
				break;
			}
		}

		if ( bFound )
		{
			delete g_pLanguages[ nLanguage ];
			// fixup array indexes
			if ( g_nLanguageCount == 1 )
			{
				free( g_pLanguages );
				g_pLanguages = NULL;
			}
			else
			{
				// shift array down
				if ( nLanguage < g_nLanguageCount - 1 )
				{
					memmove( g_pLanguages + nLanguage, 
					         g_pLanguages + nLanguage + 1, 
							 ( g_nLanguageCount - nLanguage - 1 ) * sizeof( CLanguage * ) );
				}
			}
			g_nLanguageCount--;
		}
	}

	return bFound;
}

CME_CODE CEdit::UnregisterAllLanguages()
{
	int nCount = CEdit::g_nLanguageCount;
	for ( int i = 0; i < nCount; i++ )
	{
		UnregisterLanguage( CEdit::g_pLanguages[ 0 ]->m_szName );
	}
	return ( CEdit::g_nLanguageCount == 0 ) ? CME_SUCCESS : CME_FAILURE;
}

int CEdit::GetLanguageDef( LPCTSTR pszName, CM_LANGUAGE *pLang )
{
	int cbLang = 0;
	for ( int i = 0; i < g_nLanguageCount; i++ )
	{
		CLanguage *pLangFound = g_pLanguages[ i ];
		if ( _tcsicmp( pLangFound->m_szName, pszName ) == 0 )
		{
			cbLang = pLangFound->GetSize();
			if ( pLang )
			{
				memcpy( pLang, pLangFound->m_pLang, cbLang );
			}
			break;
		} 
	}

	return cbLang;
}

CLanguage::CLanguage( LPCTSTR pszName, CM_LANGUAGE *pLang )
{
	//////////////////////////////////
	// Must make complete copy of language details
	//////////////////////////////////
	ASSERT( pszName );
	ASSERT( pLang );
	_tcscpy( m_szName, pszName );

	int nLen = ( pLang->pszKeywords ? _tcslen( pLang->pszKeywords ) + 1 : 0 ) +
	           ( pLang->pszOperators ? _tcslen( pLang->pszOperators ) + 1 : 0 ) +
	           ( pLang->pszSingleLineComments ? _tcslen( pLang->pszSingleLineComments ) + 1 : 0 ) +
	           ( pLang->pszMultiLineComments1 ? _tcslen( pLang->pszMultiLineComments1 ) + 1 : 0 ) +
	           ( pLang->pszMultiLineComments2 ? _tcslen( pLang->pszMultiLineComments2 ) + 1 : 0 ) +
	           ( pLang->pszScopeKeywords1 ? _tcslen( pLang->pszScopeKeywords1 ) + 1 : 0 ) +
	           ( pLang->pszScopeKeywords2 ? _tcslen( pLang->pszScopeKeywords2 ) + 1 : 0 ) +
	           ( pLang->pszStringDelims ? _tcslen( pLang->pszStringDelims ) + 1 : 0 ) +
	           ( pLang->pszTagElementNames ? _tcslen( pLang->pszTagElementNames ) + 1 : 0 ) +
	           ( pLang->pszTagAttributeNames ? _tcslen( pLang->pszTagAttributeNames ) + 1 : 0 ) +
	           ( pLang->pszTagEntities ? _tcslen( pLang->pszTagEntities ) + 1 : 0 );

	int cbLang = sizeof( CM_LANGUAGE ) +		 // the language structure
	             sizeof( DWORD ) +				 // size of language struct
	             sizeof( TCHAR ) * ( nLen + 1 ); // the lang buffer. +1 in case nLen = 0
	m_pLang = ( CM_LANGUAGE * ) new BYTE[ cbLang ];
	ZeroMemory( m_pLang, cbLang );

	*( ( LPDWORD ) ( m_pLang + 1 ) ) = cbLang;
	m_pLang->bIsCaseSensitive = pLang->bIsCaseSensitive;
	m_pLang->chEscape = pLang->chEscape;
	m_pLang->chTerminator = pLang->chTerminator;
	m_pLang->dwStyle = pLang->dwStyle;

	LPTSTR pszOut = ( LPTSTR ) ( ( LPBYTE ) ( m_pLang + 1 ) + sizeof( DWORD ) );

	if ( pLang->pszKeywords )
	{
		_tcscpy( pszOut, pLang->pszKeywords );
		m_pLang->pszKeywords = pszOut;
		pszOut += _tcslen( pLang->pszKeywords ) + 1;
	}

	if ( pLang->pszOperators )
	{
		_tcscpy( pszOut, pLang->pszOperators );
		m_pLang->pszOperators = pszOut;
		pszOut += _tcslen( pLang->pszOperators ) + 1;
	}

	if ( pLang->pszSingleLineComments )
	{
		_tcscpy( pszOut, pLang->pszSingleLineComments );
		m_pLang->pszSingleLineComments = pszOut;
		pszOut += _tcslen( pLang->pszSingleLineComments ) + 1;
	}

	if ( pLang->pszMultiLineComments1 )
	{
		_tcscpy( pszOut, pLang->pszMultiLineComments1 );
		m_pLang->pszMultiLineComments1 = pszOut;
		pszOut += _tcslen( pLang->pszMultiLineComments1 ) + 1;
		_tcscpy( pszOut, pLang->pszMultiLineComments2 );
		m_pLang->pszMultiLineComments2 = pszOut;
		pszOut += _tcslen( pLang->pszMultiLineComments2 ) + 1;
	}

	if ( pLang->pszScopeKeywords1 )
	{
		_tcscpy( pszOut, pLang->pszScopeKeywords1 );
		m_pLang->pszScopeKeywords1 = pszOut;
		pszOut += _tcslen( pLang->pszScopeKeywords1 ) + 1;
		_tcscpy( pszOut, pLang->pszScopeKeywords2 );
		m_pLang->pszScopeKeywords2 = pszOut;
		pszOut += _tcslen( pLang->pszScopeKeywords2 ) + 1;
	}

	if ( pLang->pszStringDelims )
	{
		_tcscpy( pszOut, pLang->pszStringDelims );
		m_pLang->pszStringDelims = pszOut;
		pszOut += _tcslen( pLang->pszStringDelims ) + 1;
	}

	if ( pLang->pszTagElementNames )
	{
		_tcscpy( pszOut, pLang->pszTagElementNames );
		m_pLang->pszTagElementNames = pszOut;
		pszOut += _tcslen( pLang->pszTagElementNames ) + 1;
	}

	if ( pLang->pszTagAttributeNames )
	{
		_tcscpy( pszOut, pLang->pszTagAttributeNames );
		m_pLang->pszTagAttributeNames = pszOut;
		pszOut += _tcslen( pLang->pszTagAttributeNames ) + 1;
	}

	if ( pLang->pszTagEntities )
	{
		_tcscpy( pszOut, pLang->pszTagEntities );
		m_pLang->pszTagEntities = pszOut;
		pszOut += _tcslen( pLang->pszTagEntities ) + 1;
	}

	ASSERT( pszOut <= ( LPTSTR ) ( ( ( LPBYTE ) m_pLang ) + cbLang ) );
}

int CLanguage::GetSize() const
{
	ASSERT( m_pLang );
	return ( int ) *( ( LPDWORD ) ( m_pLang + 1 ) );
}

CLanguage::~CLanguage()
{
	ASSERT( m_pLang );
	delete m_pLang;
}