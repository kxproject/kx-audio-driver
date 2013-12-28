// LCD7x5DM.cpp : implementation file
//

#include "stdafx.h"
#include "LCD7x5DM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(disable:4242)

/////////////////////////////////////////////////////////////////////////////
// CLCD7x5DM

IMPLEMENT_DYNAMIC(CLCD7x5DM, CWnd)

BEGIN_MESSAGE_MAP(CLCD7x5DM, CWnd)
	//{{AFX_MSG_MAP(CLCD7x5DM)
	ON_WM_CHAR()
	ON_WM_ERASEBKGND()
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()	
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEACTIVATE()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_SETFOCUS()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CLCD7x5DM::CharFromPos(CPoint point)
{
	int nChar = (int)((point.x - m_ptCharOneStart.x ) / (m_nCharWidth + m_nCharPadding));
	if (nChar >= m_nChars) 
		nChar = m_nChars-1;

	int iTextLen = m_strText.GetLength();
	
	if (nChar > iTextLen)
		nChar = iTextLen;

	return nChar;
}

CLCD7x5DM::CLCD7x5DM()
{
	m_nCtrlID			 = 0;
	m_nCharWidth		 = 16;
	m_nCharHeight		 = 22;
	m_nCharPadding		 = 1;
	m_nCharPos			 = 0; // 0 to m_nChars - 1
	m_nChars			 = 1;
	m_ptCharOneStart.x	 = 5;
	m_ptCharOneStart.y	 = 5;
	m_nMinCaretPos.x	 = m_ptCharOneStart.x + 1;
	m_nMinCaretPos.y	 = m_ptCharOneStart.y + 1;
	m_nMaxCaretPos.x	 = 0;
	m_nMaxCaretPos.y	 = 0;
	m_nCaretPos.x		 = 0;
	m_nCaretPos.y		 = 0;
	m_bBackLight		 = FALSE;
	m_bInsert			 = TRUE;
	m_strText			 = "";
	m_strUndo			 = "";	
	m_pParentWnd		 = NULL;

	BYTE caretBits[] = 
	{ 
		0xDB, 0x6C, 0xDB, 0x6C, 0x00, 0x00,
		0xDB, 0x6C, 0xDB, 0x6C, 0x00, 0x00,
		0xDB, 0x6C, 0xDB, 0x6C, 0x00, 0x00,
		0xDB, 0x6C, 0xDB, 0x6C, 0x00, 0x00,
		0xDB, 0x6C, 0xDB, 0x6C, 0x00, 0x00,
		0xDB, 0x6C, 0xDB, 0x6C, 0x00, 0x00,
		0xDB, 0x6C, 0xDB, 0x6C, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	m_bmpCaret.CreateBitmap(16,24,1,1, caretBits);
	RegisterWindowClass();
}

CLCD7x5DM::~CLCD7x5DM()
{
	m_bmpCaret.DeleteObject();
}

BOOL CLCD7x5DM::Create(LPCTSTR lpszText, CPoint topLeft, CWnd *pParentWnd, UINT nID, int nChars, BOOL bBackLight)
{
	// calculates size from nChars
	// Height is 32 pixels
	// Width is ((nChars * 17) + 11) pixels

	ASSERT(nChars > 0);

	m_nCtrlID = nID;
	m_pParentWnd = pParentWnd;
	CString strTemp(lpszText);
	strTemp.MakeUpper();
	m_strText			= strTemp.Left(nChars);	
	m_nChars			= nChars;	
	m_bBackLight		= bBackLight;
	m_nMaxCaretPos.x	= PosFromChar(m_nChars - 1);
	m_nMaxCaretPos.y	= m_nMinCaretPos.y;
	m_nCaretPos.x		= m_nMaxCaretPos.x;
	m_nCaretPos.y		= m_nMaxCaretPos.y;
	CRect rect(topLeft.x,topLeft.y,0,0);
	rect.right = rect.left + (m_nChars * (m_nCharWidth + m_nCharPadding)) + 5 + 7 - 1;  
	rect.bottom = rect.top + m_nCharHeight + 5 + 5;  
	DWORD dwStyle = WS_CHILD | WS_VISIBLE;
	return CWnd::Create(LCD7X5DM_CLASSNAME, m_strText, dwStyle, rect, pParentWnd, nID);	
}

void CLCD7x5DM::DrawText(TCHAR charText, CDC * pDC, CRect &rect)
{
	//	ABCDEFGHIJKLMNOPQRSTUVWXYZ 1234567890`~!@#$%^&*()-_=+[]{}\\|;:\'\",.<>/\?
	
	BYTE charBits[7];
	switch (charText)
	{
		case 'A':
			charBits[0] = 0x04;
			charBits[1] = 0x0A;
			charBits[2] = 0x11;
			charBits[3] = 0x11;
			charBits[4] = 0x1F;
			charBits[5] = 0x11;
			charBits[6] = 0x11;
			break;
		case 'B':
			charBits[0] = 0x1E;
			charBits[1] = 0x11;
			charBits[2] = 0x11;
			charBits[3] = 0x1E;
			charBits[4] = 0x11;
			charBits[5] = 0x11;
			charBits[6] = 0x1E;
			break;
		case 'C':
			charBits[0] = 0x0E;
			charBits[1] = 0x11;
			charBits[2] = 0x10;
			charBits[3] = 0x10;
			charBits[4] = 0x10;
			charBits[5] = 0x11;
			charBits[6] = 0x0E;
			break;
		case'D':
			charBits[0] = 0x1C;
			charBits[1] = 0x12;
			charBits[2] = 0x11;
			charBits[3] = 0x11;
			charBits[4] = 0x11;
			charBits[5] = 0x12;
			charBits[6] = 0x1C;
			
			break;
		case'E':
			charBits[0] = 0x1F;
			charBits[1] = 0x10;
			charBits[2] = 0x10;
			charBits[3] = 0x1E;
			charBits[4] = 0x10;
			charBits[5] = 0x10;
			charBits[6] = 0x1F;
			
			break;
		case'F':
			charBits[0] = 0x1F;
			charBits[1] = 0x10;
			charBits[2] = 0x10;
			charBits[3] = 0x1E;
			charBits[4] = 0x10;
			charBits[5] = 0x10;
			charBits[6] = 0x10;
			
			break;
		case'G':
			charBits[0] = 0x0E;
			charBits[1] = 0x11;
			charBits[2] = 0x10;
			charBits[3] = 0x17;
			charBits[4] = 0x11;
			charBits[5] = 0x11;
			charBits[6] = 0x0E;
			
			break;
		case'H':
			charBits[0] = 0x11;
			charBits[1] = 0x11;
			charBits[2] = 0x11;
			charBits[3] = 0x1F;
			charBits[4] = 0x11;
			charBits[5] = 0x11;
			charBits[6] = 0x11;
			
			break;
		case'I':
			charBits[0] = 0x0E;
			charBits[1] = 0x04;
			charBits[2] = 0x04;
			charBits[3] = 0x04;
			charBits[4] = 0x04;
			charBits[5] = 0x04;
			charBits[6] = 0x0E;
			
			break;
		case'J':
			charBits[0] = 0x07;
			charBits[1] = 0x02;
			charBits[2] = 0x02;
			charBits[3] = 0x02;
			charBits[4] = 0x02;
			charBits[5] = 0x12;
			charBits[6] = 0x0C;
			
			break;
		case'K':
			charBits[0] = 0x11;
			charBits[1] = 0x12;
			charBits[2] = 0x14;
			charBits[3] = 0x18;
			charBits[4] = 0x14;
			charBits[5] = 0x12;
			charBits[6] = 0x11;
			
			break;
		case'L':
			charBits[0] = 0x10;
			charBits[1] = 0x10;
			charBits[2] = 0x10;
			charBits[3] = 0x10;
			charBits[4] = 0x10;
			charBits[5] = 0x10;
			charBits[6] = 0x1F;
			
			break;
		case'M':
			charBits[0] = 0x11;
			charBits[1] = 0x1B;
			charBits[2] = 0x15;
			charBits[3] = 0x15;
			charBits[4] = 0x11;
			charBits[5] = 0x11;
			charBits[6] = 0x11;
			
			break;
		case'N':
			charBits[0] = 0x11;
			charBits[1] = 0x11;
			charBits[2] = 0x19;
			charBits[3] = 0x15;
			charBits[4] = 0x13;
			charBits[5] = 0x11;
			charBits[6] = 0x11;
			
			break;
		case'O':
			charBits[0] = 0x0E;
			charBits[1] = 0x11;
			charBits[2] = 0x11;
			charBits[3] = 0x11;
			charBits[4] = 0x11;
			charBits[5] = 0x11;
			charBits[6] = 0x0E;
			
			break;
		case'P':
			charBits[0] = 0x1E;
			charBits[1] = 0x11;
			charBits[2] = 0x11;
			charBits[3] = 0x1E;
			charBits[4] = 0x10;
			charBits[5] = 0x10;
			charBits[6] = 0x10;
			
			break;
		case'Q':
			charBits[0] = 0x0E;
			charBits[1] = 0x11;
			charBits[2] = 0x11;
			charBits[3] = 0x11;
			charBits[4] = 0x15;
			charBits[5] = 0x12;
			charBits[6] = 0x0D;
			
			break;
		case'R':
			charBits[0] = 0x1E;
			charBits[1] = 0x11;
			charBits[2] = 0x11;
			charBits[3] = 0x1E;
			charBits[4] = 0x14;
			charBits[5] = 0x12;
			charBits[6] = 0x11;
			
			break;
		case'S':
			charBits[0] = 0x0E;
			charBits[1] = 0x11;
			charBits[2] = 0x10;
			charBits[3] = 0x0E;
			charBits[4] = 0x01;
			charBits[5] = 0x11;
			charBits[6] = 0x0E;
			
			break;
		case'T':
			charBits[0] = 0x1F;
			charBits[1] = 0x04;
			charBits[2] = 0x04;
			charBits[3] = 0x04;
			charBits[4] = 0x04;
			charBits[5] = 0x04;
			charBits[6] = 0x04;
			
			break;
		case'U':
			charBits[0] = 0x11;
			charBits[1] = 0x11;
			charBits[2] = 0x11;
			charBits[3] = 0x11;
			charBits[4] = 0x11;
			charBits[5] = 0x11;
			charBits[6] = 0x0E;
			
			break;
		case'V':
			charBits[0] = 0x11;
			charBits[1] = 0x11;
			charBits[2] = 0x11;
			charBits[3] = 0x11;
			charBits[4] = 0x11;
			charBits[5] = 0x0A;
			charBits[6] = 0x04;
			
			break;
		case'W':
			charBits[0] = 0x11;
			charBits[1] = 0x11;
			charBits[2] = 0x11;
			charBits[3] = 0x11;
			charBits[4] = 0x15;
			charBits[5] = 0x15;
			charBits[6] = 0x0A;
			
			break;
		case'X':
			charBits[0] = 0x11;
			charBits[1] = 0x11;
			charBits[2] = 0x0A;
			charBits[3] = 0x04;
			charBits[4] = 0x0A;
			charBits[5] = 0x11;
			charBits[6] = 0x11;
			
			break;
		case'Y':
			charBits[0] = 0x11;
			charBits[1] = 0x11;
			charBits[2] = 0x11;
			charBits[3] = 0x0A;
			charBits[4] = 0x04;
			charBits[5] = 0x04;
			charBits[6] = 0x04;
			
			break;
		case'Z':
			charBits[0] = 0x1F;
			charBits[1] = 0x01;
			charBits[2] = 0x02;
			charBits[3] = 0x04;
			charBits[4] = 0x08;
			charBits[5] = 0x10;
			charBits[6] = 0x1F;
			
			break;
		case' ':
			charBits[0] = 0x00;
			charBits[1] = 0x00;
			charBits[2] = 0x00;
			charBits[3] = 0x00;
			charBits[4] = 0x00;
			charBits[5] = 0x00;
			charBits[6] = 0x00;
			
			break;
		case'1':
			charBits[0] = 0x04;
			charBits[1] = 0x0C;
			charBits[2] = 0x04;
			charBits[3] = 0x04;
			charBits[4] = 0x04;
			charBits[5] = 0x04;
			charBits[6] = 0x0E;
			
			break;
		case'2':
			charBits[0] = 0x0E;
			charBits[1] = 0x11;
			charBits[2] = 0x01;
			charBits[3] = 0x02;
			charBits[4] = 0x04;
			charBits[5] = 0x08;
			charBits[6] = 0x1F;
			
			break;
		case'3':
			charBits[0] = 0x1F;
			charBits[1] = 0x02;
			charBits[2] = 0x04;
			charBits[3] = 0x02;
			charBits[4] = 0x01;
			charBits[5] = 0x11;
			charBits[6] = 0x0E;
			
			break;
		case'4':
			charBits[0] = 0x02;
			charBits[1] = 0x06;
			charBits[2] = 0x0A;
			charBits[3] = 0x12;
			charBits[4] = 0x1F;
			charBits[5] = 0x02;
			charBits[6] = 0x02;
			
			break;
		case'5':
			charBits[0] = 0x1F;
			charBits[1] = 0x10;
			charBits[2] = 0x1E;
			charBits[3] = 0x01;
			charBits[4] = 0x01;
			charBits[5] = 0x11;
			charBits[6] = 0x0E;
			
			break;
		case'6':
			charBits[0] = 0x06;
			charBits[1] = 0x08;
			charBits[2] = 0x10;
			charBits[3] = 0x1E;
			charBits[4] = 0x11;
			charBits[5] = 0x11;
			charBits[6] = 0x0E;
			
			break;
		case'7':
			charBits[0] = 0x1F;
			charBits[1] = 0x01;
			charBits[2] = 0x02;
			charBits[3] = 0x04;
			charBits[4] = 0x08;
			charBits[5] = 0x08;
			charBits[6] = 0x08;
			
			break;
		case'8':
			charBits[0] = 0x0E;
			charBits[1] = 0x11;
			charBits[2] = 0x11;
			charBits[3] = 0x0E;
			charBits[4] = 0x11;
			charBits[5] = 0x11;
			charBits[6] = 0x0E;
			
			break;
		case'9':
			charBits[0] = 0x0E;
			charBits[1] = 0x11;
			charBits[2] = 0x11;
			charBits[3] = 0x0F;
			charBits[4] = 0x01;
			charBits[5] = 0x02;
			charBits[6] = 0x0C;
			
			break;
		case'0':
			charBits[0] = 0x0E;
			charBits[1] = 0x11;
			charBits[2] = 0x13;
			charBits[3] = 0x15;
			charBits[4] = 0x19;
			charBits[5] = 0x11;
			charBits[6] = 0x0E;
			
			break;
		case'`':
			charBits[0] = 0x0C;
			charBits[1] = 0x08;
			charBits[2] = 0x04;
			charBits[3] = 0x00;
			charBits[4] = 0x00;
			charBits[5] = 0x00;
			charBits[6] = 0x00;
			
			break;
		case'~':
			charBits[0] = 0x00;
			charBits[1] = 0x00;
			charBits[2] = 0x08;
			charBits[3] = 0x15;
			charBits[4] = 0x02;
			charBits[5] = 0x00;
			charBits[6] = 0x00;
			
			break;
		case'!':
			charBits[0] = 0x04;
			charBits[1] = 0x04;
			charBits[2] = 0x04;
			charBits[3] = 0x04;
			charBits[4] = 0x04;
			charBits[5] = 0x00;
			charBits[6] = 0x04;
			
			break;
		case'@':
			charBits[0] = 0x06;
			charBits[1] = 0x09;
			charBits[2] = 0x17;
			charBits[3] = 0x1B;
			charBits[4] = 0x16;
			charBits[5] = 0x08;
			charBits[6] = 0x06;
			
			break;
		case'#':
			charBits[0] = 0x0A;
			charBits[1] = 0x0A;
			charBits[2] = 0x1F;
			charBits[3] = 0x0A;
			charBits[4] = 0x1F;
			charBits[5] = 0x0A;
			charBits[6] = 0x0A;
			
			break;
		case'$':
			charBits[0] = 0x04;
			charBits[1] = 0x0F;
			charBits[2] = 0x14;
			charBits[3] = 0x0E;
			charBits[4] = 0x05;
			charBits[5] = 0x1E;
			charBits[6] = 0x04;
			
			break;
		case'%':
			charBits[0] = 0x00;
			charBits[1] = 0x19;
			charBits[2] = 0x1A;
			charBits[3] = 0x04;
			charBits[4] = 0x0B;
			charBits[5] = 0x13;
			charBits[6] = 0x00;
			
			break;
		case'^':
			charBits[0] = 0x04;
			charBits[1] = 0x0A;
			charBits[2] = 0x11;
			charBits[3] = 0x00;
			charBits[4] = 0x00;
			charBits[5] = 0x00;
			charBits[6] = 0x00;
			
			break;
		case'&':
			charBits[0] = 0x0C;
			charBits[1] = 0x12;
			charBits[2] = 0x14;
			charBits[3] = 0x08;
			charBits[4] = 0x15;
			charBits[5] = 0x12;
			charBits[6] = 0x0D;
			
			break;
		case'*':
			charBits[0] = 0x00;
			charBits[1] = 0x04;
			charBits[2] = 0x15;
			charBits[3] = 0x0E;
			charBits[4] = 0x15;
			charBits[5] = 0x04;
			charBits[6] = 0x00;
			
			break;
		case'(':
			charBits[0] = 0x04;
			charBits[1] = 0x08;
			charBits[2] = 0x08;
			charBits[3] = 0x08;
			charBits[4] = 0x08;
			charBits[5] = 0x08;
			charBits[6] = 0x04;
			
			break;
		case')':
			charBits[0] = 0x04;
			charBits[1] = 0x02;
			charBits[2] = 0x02;
			charBits[3] = 0x02;
			charBits[4] = 0x02;
			charBits[5] = 0x02;
			charBits[6] = 0x04;
			
			break;
		case'-':
			charBits[0] = 0x00;
			charBits[1] = 0x00;
			charBits[2] = 0x00;
			charBits[3] = 0x0E;
			charBits[4] = 0x00;
			charBits[5] = 0x00;
			charBits[6] = 0x00;
			
			break;
		case'_':
			charBits[0] = 0x00;
			charBits[1] = 0x00;
			charBits[2] = 0x00;
			charBits[3] = 0x00;
			charBits[4] = 0x00;
			charBits[5] = 0x00;
			charBits[6] = 0x1F;
			
			break;
		case'=':
			charBits[0] = 0x00;
			charBits[1] = 0x00;
			charBits[2] = 0x0E;
			charBits[3] = 0x00;
			charBits[4] = 0x0E;
			charBits[5] = 0x00;
			charBits[6] = 0x00;
			
			break;
		case'+':
			charBits[0] = 0x00;
			charBits[1] = 0x04;
			charBits[2] = 0x04;
			charBits[3] = 0x1F;
			charBits[4] = 0x04;
			charBits[5] = 0x04;
			charBits[6] = 0x00;
			
			break;
		case'[':
			charBits[0] = 0x0C;
			charBits[1] = 0x08;
			charBits[2] = 0x08;
			charBits[3] = 0x08;
			charBits[4] = 0x08;
			charBits[5] = 0x08;
			charBits[6] = 0x0C;
			
			break;
		case']':
			charBits[0] = 0x06;
			charBits[1] = 0x02;
			charBits[2] = 0x02;
			charBits[3] = 0x02;
			charBits[4] = 0x02;
			charBits[5] = 0x02;
			charBits[6] = 0x06;
			
			break;
		case'{':
			charBits[0] = 0x04;
			charBits[1] = 0x08;
			charBits[2] = 0x08;
			charBits[3] = 0x10;
			charBits[4] = 0x08;
			charBits[5] = 0x08;
			charBits[6] = 0x04;
			
			break;
		case'}':
			charBits[0] = 0x04;
			charBits[1] = 0x02;
			charBits[2] = 0x02;
			charBits[3] = 0x01;
			charBits[4] = 0x02;
			charBits[5] = 0x02;
			charBits[6] = 0x04;
			
			break;
		case'\\':
			charBits[0] = 0x00;
			charBits[1] = 0x10;
			charBits[2] = 0x08;
			charBits[3] = 0x04;
			charBits[4] = 0x02;
			charBits[5] = 0x01;
			charBits[6] = 0x00;
			
			break;
		case'|':
			charBits[0] = 0x04;
			charBits[1] = 0x04;
			charBits[2] = 0x04;
			charBits[3] = 0x04;
			charBits[4] = 0x04;
			charBits[5] = 0x04;
			charBits[6] = 0x04;
			
			break;
		case';':
			charBits[0] = 0x00;
			charBits[1] = 0x0C;
			charBits[2] = 0x0C;
			charBits[3] = 0x00;
			charBits[4] = 0x0C;
			charBits[5] = 0x04;
			charBits[6] = 0x08;
			
			break;
		case':':
			charBits[0] = 0x00;
			charBits[1] = 0x0C;
			charBits[2] = 0x0C;
			charBits[3] = 0x00;
			charBits[4] = 0x0C;
			charBits[5] = 0x0C;
			charBits[6] = 0x00;
			
			break;
		case'\'':
			charBits[0] = 0x0C;
			charBits[1] = 0x04;
			charBits[2] = 0x08;
			charBits[3] = 0x00;
			charBits[4] = 0x00;
			charBits[5] = 0x00;
			charBits[6] = 0x00;
			
			break;
		case'\"':
			charBits[0] = 0x0A;
			charBits[1] = 0x0A;
			charBits[2] = 0x14;
			charBits[3] = 0x00;
			charBits[4] = 0x00;
			charBits[5] = 0x00;
			charBits[6] = 0x00;
			
			break;
		case',':
			charBits[0] = 0x00;
			charBits[1] = 0x00;
			charBits[2] = 0x00;
			charBits[3] = 0x00;
			charBits[4] = 0x0C;
			charBits[5] = 0x04;
			charBits[6] = 0x08;
			
			break;
		case'.':
			charBits[0] = 0x00;
			charBits[1] = 0x00;
			charBits[2] = 0x00;
			charBits[3] = 0x00;
			charBits[4] = 0x00;
			charBits[5] = 0x0C;
			charBits[6] = 0x0C;
			
			break;
		case'<':
			charBits[0] = 0x00;
			charBits[1] = 0x04;
			charBits[2] = 0x08;
			charBits[3] = 0x10;
			charBits[4] = 0x08;
			charBits[5] = 0x04;
			charBits[6] = 0x00;
			
			break;
		case'>':
			charBits[0] = 0x00;
			charBits[1] = 0x04;
			charBits[2] = 0x02;
			charBits[3] = 0x01;
			charBits[4] = 0x02;
			charBits[5] = 0x04;
			charBits[6] = 0x00;
			
			break;
		case'/':
			charBits[0] = 0x00;
			charBits[1] = 0x01;
			charBits[2] = 0x02;
			charBits[3] = 0x04;
			charBits[4] = 0x08;
			charBits[5] = 0x10;
			charBits[6] = 0x00;
			
			break;
		case'\?':
			charBits[0] = 0x0E;
			charBits[1] = 0x11;
			charBits[2] = 0x01;
			charBits[3] = 0x06;
			charBits[4] = 0x04;
			charBits[5] = 0x00;
			charBits[6] = 0x04;
			
			break;
		default:
			// not in charset
			charBits[0] = 0x1F;
			charBits[1] = 0x1F;
			charBits[2] = 0x1F;
			charBits[3] = 0x1F;
			charBits[4] = 0x1F;
			charBits[5] = 0x1F;
			charBits[6] = 0x1F;
	}
	
	CPoint pt(rect.left + 1, rect.top + 1);	
	COLORREF clrText, clrTextBackground;
	clrText = RGB(0,0,0);
	if (m_bBackLight)
		clrTextBackground = RGB(161,180,180);
	else
		clrTextBackground = RGB(148,148,148);

	for (int i=0; i<7; i++)
	{
		for (int j=16; j>0; j>>=1)
		{
			if (charBits[i] & j)
				pDC->FillSolidRect(pt.x, pt.y, 2, 2, clrText);
			else
				pDC->FillSolidRect(pt.x, pt.y, 2, 2, clrTextBackground);
			pt.Offset(3,0);			
		}
		pt.Offset(-15,3);
	}	
}

BOOL CLCD7x5DM::GetBackLight()
{
	return m_bBackLight;
}

void CLCD7x5DM::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// process characters keys
	TCHAR * lpszCharset = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ 1234567890`~!@#$%^&*()-_=+[]{}\\|;:\'\",.<>/\?");
	
	if ( (nChar >= 'a') && (nChar <= 'z') )
		nChar -= 32;

	int nCharsetLen = _tcslen(lpszCharset);
	for (int i=0; i<nCharsetLen; i++)
	{
		if(nChar == (unsigned int)lpszCharset[i])
		{
			int iTextLen = m_strText.GetLength();

			if (m_nCharPos < m_nChars)
				if (m_nCharPos < iTextLen)
					if (m_bInsert)						
						if (iTextLen < m_nChars)
							m_strText.Insert(m_nCharPos, nChar);
						else
							return;
					else
						m_strText.SetAt(m_nCharPos, nChar);
				else
					m_strText.Insert(m_nCharPos, nChar);
			else
				m_strText.SetAt(m_nChars - 1, nChar);
			
			if (m_nCaretPos.x < m_nMaxCaretPos.x)
			{
				m_nCaretPos.Offset(m_nCharWidth + m_nCharPadding, 0);
				SetCaretPos(m_nCaretPos);
				m_nCharPos += 1;
			}

			SetWindowText(m_strText);
			Invalidate(false);			
		}
		else
			CWnd::OnChar(nChar, nRepCnt, nFlags);	
	}	
}

BOOL CLCD7x5DM::OnEraseBkgnd(CDC* pDC) 
{
	return true;
}

UINT CLCD7x5DM::OnGetDlgCode() 
{
	return CWnd::OnGetDlgCode() | DLGC_WANTALLKEYS;
}

void CLCD7x5DM::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// process non-character keys
	switch (nChar)
	{
		case VK_LEFT:
			if (m_nCaretPos.x > m_nMinCaretPos.x)
			{
				m_nCaretPos.Offset(-(m_nCharWidth + m_nCharPadding), 0);
				SetCaretPos(m_nCaretPos);
				m_nCharPos -= 1;
			}
			break;			
		case VK_RIGHT:
			if ( (m_nCaretPos.x < m_nMaxCaretPos.x) && (m_nCharPos < m_strText.GetLength()) )
			{
				m_nCaretPos.Offset(m_nCharWidth + m_nCharPadding, 0);
				SetCaretPos(m_nCaretPos);
				m_nCharPos += 1;
			}
			break;
		case VK_HOME:
		{
			m_nCharPos = 0;
			m_nCaretPos  = m_nMinCaretPos;
			SetCaretPos(m_nCaretPos);
		}
		break;
		case VK_END:
		{
			int iTextLen = m_strText.GetLength();		
			if (iTextLen == m_nChars)
			{
				m_nCharPos = m_nChars - 1;
				m_nCaretPos.x = m_nMaxCaretPos.x;	
			}
			else		
			{
				m_nCharPos = iTextLen;
				m_nCaretPos.x = PosFromChar(iTextLen);		
			}
			SetCaretPos(m_nCaretPos);
			break;
		}
		case VK_INSERT:
			m_bInsert = !m_bInsert;
			break;
		case VK_DELETE:
		{		
			int iTextLen = m_strText.GetLength();
			if (m_nCharPos < iTextLen)
			{
				m_strText.Delete(m_nCharPos, 1);
				SetWindowText(m_strText);
				Invalidate(false);
			}
			break;			
		}
		case VK_BACK:
			if (m_nCaretPos.x > m_nMinCaretPos.x)
			{
				m_nCaretPos.Offset(-(m_nCharWidth + m_nCharPadding), 0);
				SetCaretPos(m_nCaretPos);
				m_strText.Delete(m_nCharPos - 1, 1);
				m_nCharPos -= 1;		
				SetWindowText(m_strText);
				Invalidate(false);
			}
			break;
		case VK_ESCAPE:
			m_strText = m_strUndo;
			m_strUndo = "";
			GetParent()->SetFocus();
			SetWindowText(m_strText);
			Invalidate(false);
			break;		
		case VK_RETURN:
			GetParent()->SetFocus();
			break;
		default:
			CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
	}	
}

void CLCD7x5DM::OnKillFocus(CWnd* pNewWnd) 
{
	m_strUndo = "";
	DestroyCaret();	
	CWnd::OnKillFocus(pNewWnd);
}

void CLCD7x5DM::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_nCharPos = CharFromPos(point);	
	m_nCaretPos.x = PosFromChar(m_nCharPos);
	SetCaretPos(m_nCaretPos);
	
	CWnd::OnLButtonDown(nFlags, point);
}

int CLCD7x5DM::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{	
	if (message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN)
		SetFocus();

	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CLCD7x5DM::OnPaint() 
{
	CPaintDC dc(this);

	CRect rectClient;
	GetClientRect(&rectClient);

	CDC dcMem;
    dcMem.CreateCompatibleDC(&dc);   

	CBitmap bitmap;	
	bitmap.CreateCompatibleBitmap(&dc, rectClient.Width(), rectClient.Height());

	CBitmap * pOldBitmap = (CBitmap*) dcMem.SelectObject(&bitmap);

	dcMem.SetPixel(0,0,(RGB(74,74,74)));	
	dcMem.MoveTo(1,0);
	
	CPen pen;
	CPen * pOldPen;

	pen.CreatePen(PS_SOLID, 1, RGB(115,115,115));
	pOldPen = dcMem.SelectObject(&pen);
	dcMem.LineTo(rectClient.right-1, rectClient.top);
	pen.DeleteObject();

	dcMem.SetPixel(rectClient.right-1,rectClient.top, RGB(132,132,132));	
	dcMem.MoveTo(rectClient.right-1, rectClient.top+1);

	pen.CreatePen(PS_SOLID, 1, RGB(206,206,206));
	dcMem.LineTo(rectClient.right-1, rectClient.bottom);
	dcMem.MoveTo(rectClient.left, rectClient.bottom-1);
	dcMem.LineTo(rectClient.right-1, rectClient.bottom-1);
	pen.DeleteObject();	
	
	dcMem.SetPixel(rectClient.left, rectClient.bottom-1, RGB(165,165,165));		
	dcMem.MoveTo(rectClient.left, rectClient.top+1);	

	pen.CreatePen(PS_SOLID, 1, RGB(115,115,115));	
	dcMem.LineTo(rectClient.left, rectClient.bottom-1);
	dcMem.SelectObject(pOldPen);
	pen.DeleteObject();

	rectClient.DeflateRect(1,1);
	if (!m_bBackLight)
	{
		dcMem.FillSolidRect(1,1,rectClient.right-1, 5,RGB(140,140,140));
		dcMem.FillSolidRect(1,6,4,rectClient.bottom-6,RGB(140,140,140));
		dcMem.FillSolidRect(5,6,rectClient.right-5,rectClient.bottom-6,RGB(173,173,173));
		dcMem.SetPixel(rectClient.left,rectClient.bottom-1,RGB(165,165,165));
		dcMem.SetPixel(rectClient.left+1,rectClient.bottom-1,RGB(165,165,165));
		dcMem.SetPixel(rectClient.left+1,rectClient.bottom-2,RGB(165,165,165));
		dcMem.SetPixel(rectClient.left+2,rectClient.bottom-2,RGB(165,165,165));
		dcMem.SetPixel(rectClient.left+2,rectClient.bottom-3,RGB(165,165,165));
		dcMem.SetPixel(rectClient.left+3,rectClient.bottom-3,RGB(165,165,165));
		dcMem.SetPixel(rectClient.left+3,rectClient.bottom-4,RGB(165,165,165));
		dcMem.SetPixel(rectClient.left+2,rectClient.bottom-1,RGB(173,173,173));
		dcMem.SetPixel(rectClient.left+3,rectClient.bottom-1,RGB(173,173,173));
		dcMem.SetPixel(rectClient.left+3,rectClient.bottom-2,RGB(173,173,173));		
	}
	else
		dcMem.FillSolidRect(&rectClient, RGB(179,198,198));	

	CRect textRect(m_ptCharOneStart.x, m_ptCharOneStart.y, m_ptCharOneStart.x + m_nCharWidth, m_ptCharOneStart.y + m_nCharHeight);

	int iTextLength = m_strText.GetLength();
	for (int i=0; i<m_nChars; i++)
	{
		if (i < iTextLength)
			DrawText(m_strText.GetAt(i), &dcMem, textRect);
		else
			DrawText(' ', &dcMem, textRect);

		textRect.OffsetRect(m_nCharWidth + m_nCharPadding, 0);
	}

	CRect rect(dc.m_ps.rcPaint);	
	dc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMem, rect.left, rect.top, SRCCOPY);
    dcMem.SelectObject(pOldBitmap); 	
	bitmap.DeleteObject();
	dcMem.DeleteDC();	
}

void CLCD7x5DM::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_nCharPos = CharFromPos(point);	
	m_nCaretPos.x = PosFromChar(m_nCharPos);
	SetCaretPos(m_nCaretPos);	
	
	CWnd::OnRButtonDown(nFlags, point);
}

void CLCD7x5DM::OnSetFocus(CWnd* pOldWnd) 
{	

	::CreateCaret(m_hWnd, (HBITMAP)m_bmpCaret.GetSafeHandle(), 14, 20);
	SetCaretPos(m_nCaretPos);
    ShowCaret();    
	m_strUndo = m_strText;
	CWnd::OnSetFocus(pOldWnd);	
}

int CLCD7x5DM::PosFromChar(int nChar)
{
	return m_nMinCaretPos.x + (nChar * (m_nCharWidth + m_nCharPadding));		
}

BOOL CLCD7x5DM::RegisterWindowClass()
{
	WNDCLASS wndclass;
    HINSTANCE hInst = AfxGetInstanceHandle();

    if (!(::GetClassInfo(hInst, LCD7X5DM_CLASSNAME, &wndclass)))
    {
        wndclass.style            = CS_DBLCLKS | CS_PARENTDC;
        wndclass.lpfnWndProc      = ::DefWindowProc;
        wndclass.cbClsExtra       = wndclass.cbWndExtra = 0;
        wndclass.hInstance        = hInst;
        wndclass.hIcon            = NULL;
        wndclass.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        wndclass.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
        wndclass.lpszMenuName     = NULL;
        wndclass.lpszClassName    = LCD7X5DM_CLASSNAME;

        if (!AfxRegisterClass(&wndclass))
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}

void CLCD7x5DM::SetBackLight(BOOL bBackLight)
{
	if (m_bBackLight != bBackLight)
	{
		m_bBackLight = bBackLight;
		Invalidate(false);
	}
}

void CLCD7x5DM::SetWindowText(LPCTSTR lpszString)
{
	// trims string to m_nChars, but does not filter the characters in the string
	// characters that are not in the character set, will be displayed as a block
	CString strTemp(lpszString);
	strTemp.MakeUpper();
	m_strText = strTemp.Left(m_nChars);
	CWnd::SetWindowText(m_strText);
	if (m_nCharPos > m_strText.GetLength())
	{
		m_nCharPos = m_strText.GetLength();
		m_nCaretPos.x = PosFromChar(m_nCharPos);
		SetCaretPos(m_nCaretPos);	
	}

	OnTextChanged();
	Invalidate(false);
}

void CLCD7x5DM::OnTextChanged()
{
	NMHDR nm;
	nm.code = LD_CHANGE;
	nm.hwndFrom = this->m_hWnd;
	nm.idFrom = this->m_nCtrlID;
	m_pParentWnd->SendMessage(WM_NOTIFY, (WPARAM) (int) m_nCtrlID, (LPARAM) (LPNMHDR) &nm);	
}
