#include "precomp.h"
#include "hkeyctrl.h"
#include "resource2.h"
#include "resource.h"

void GetHotKeyString( CM_HOTKEY &cmHotKey, LPTSTR psz )
{
	*psz = _T('\0');
	UINT nVirtKey = cmHotKey.nVirtKey1;
	BYTE byModifiers = cmHotKey.byModifiers1;
	BYTE byOrigModifiers = byModifiers;

	for ( int i = 0; nVirtKey && ( i < 2 ); i++ )
	{
		if ( !i || ( i && byModifiers != byOrigModifiers ) )
		{
			BOOL bAddPlus = FALSE;
			if ( HAS_FLAG( byModifiers, HOTKEYF_CONTROL ) )
			{
				_tcscat( psz, LoadStringPtr( IDS_CTRL ) );
				bAddPlus = TRUE;
			}
			if ( bAddPlus ) { _tcscat( psz, _T(" + ") ); bAddPlus = FALSE; }
			if ( HAS_FLAG( byModifiers, HOTKEYF_SHIFT ) )
			{
				_tcscat( psz, LoadStringPtr( IDS_SHIFT ) );
				bAddPlus = TRUE;
			}
			if ( bAddPlus ) { _tcscat( psz, _T(" + ") ); bAddPlus = FALSE; }
			if ( HAS_FLAG( byModifiers, HOTKEYF_ALT ) )
			{
				_tcscat( psz, LoadStringPtr( IDS_ALT ) );
				bAddPlus = TRUE;
			}
			if ( bAddPlus ) { _tcscat( psz, _T(" + ") ); bAddPlus = FALSE; }
		}

		if  ( nVirtKey )
		{
			LPTSTR pszChar = NULL;
			TCHAR szTemp[ 2 ];
			switch ( nVirtKey )
			{
				case VK_NUMLOCK:
				case VK_BACK:
				case VK_INSERT:
				case VK_DELETE:
				case VK_HOME:
				case VK_END:
				case VK_PRIOR:
				case VK_NEXT:
				case VK_LEFT:
				case VK_RIGHT:
				case VK_UP:
				case VK_DOWN:
				case VK_SCROLL:
				case VK_TAB:
				case VK_ESCAPE:
				case VK_RETURN:
				case VK_PAUSE:
				case VK_CANCEL:
				case VK_SPACE:
				case VK_F1:
				case VK_F2:
				case VK_F3:
				case VK_F4:
				case VK_F5:
				case VK_F6:
				case VK_F7:
				case VK_F8:
				case VK_F9:
				case VK_F10:
				case VK_F11:
				case VK_F12:
					{
					pszChar = ( LPTSTR ) LoadStringPtr( nVirtKey + VK_TO_STRING_OFFSET );
					break;
					}

				default:
				{
					if ( nVirtKey >= 0x60 && nVirtKey <= 0x6f )
					{
						_tcscat( psz, LoadStringPtr( IDS_NUM_KEY ) );
					}
					switch ( nVirtKey )
					{
						case 0xc0:			{ nVirtKey = _T('`'); break; }
						case 0x30:			{ nVirtKey = _T('0'); break; }
						case 0x31:			{ nVirtKey = _T('1'); break; }
						case 0x32:			{ nVirtKey = _T('2'); break; }
						case 0x33:			{ nVirtKey = _T('3'); break; }
						case 0x34:			{ nVirtKey = _T('4'); break; }
						case 0x35:			{ nVirtKey = _T('5'); break; }
						case 0x36:			{ nVirtKey = _T('6'); break; }
						case 0x37:			{ nVirtKey = _T('7'); break; }
						case 0x38:			{ nVirtKey = _T('8'); break; }
						case 0x39:			{ nVirtKey = _T('9'); break; }
						case 0xbd:			{ nVirtKey = _T('-'); break; }
						case 0xbb:			{ nVirtKey = _T('='); break; }
						case 0xdb:			{ nVirtKey = _T('['); break; }
						case 0xdd:			{ nVirtKey = _T(']'); break; }
						case 0xdc:			{ nVirtKey = _T('\\'); break; }
						case 0xba:			{ nVirtKey = _T(';'); break; }
						case 0xde:			{ nVirtKey = _T('\''); break; }
						case 0xbc:			{ nVirtKey = _T(','); break; }
						case 0xbe:			{ nVirtKey = _T('.'); break; }
						case 0xbf:			{ nVirtKey = _T('/'); break; }
						case VK_NUMPAD0:	{ nVirtKey = _T('0'); break; }
						case VK_NUMPAD1:	{ nVirtKey = _T('1'); break; }
						case VK_NUMPAD2:	{ nVirtKey = _T('2'); break; }
						case VK_NUMPAD3:	{ nVirtKey = _T('3'); break; }
						case VK_NUMPAD4:	{ nVirtKey = _T('4'); break; }
						case VK_NUMPAD5:	{ nVirtKey = _T('5'); break; }
						case VK_NUMPAD6:	{ nVirtKey = _T('6'); break; }
						case VK_NUMPAD7:	{ nVirtKey = _T('7'); break; }
						case VK_NUMPAD8:	{ nVirtKey = _T('8'); break; }
						case VK_NUMPAD9:	{ nVirtKey = _T('9'); break; }
						case VK_MULTIPLY:	{ nVirtKey = _T('*'); break; }
						case VK_ADD:		{ nVirtKey = _T('+'); break; }
						case VK_SUBTRACT:	{ nVirtKey = _T('-'); break; }
						case VK_DECIMAL:	{ nVirtKey = _T('.'); break; }
						case VK_DIVIDE :	{ nVirtKey = _T('/'); break; }
						case 0x3b:			{ nVirtKey = _T('+'); break; }
						case 0x3d:			{ nVirtKey = _T('-'); break; }
					}
					szTemp[ 0 ] = ( TCHAR ) nVirtKey;
					szTemp[ 1 ] = _T('\0');
					pszChar = szTemp;
				}
			}

			_tcscat( psz, pszChar );

			nVirtKey = cmHotKey.nVirtKey2;
			byModifiers = cmHotKey.byModifiers2;

			if ( nVirtKey && ( i == 0 ) )
			{
				_tcscat( psz, _T(", ") );
			}
		}
	}
}

#define CMHOTKEY_CLASS _T("CMHotKey")
LRESULT PASCAL HotKeyWndProc(HWND hWnd, register UINT uMsg, register WPARAM wParam, register LPARAM lParam);

BOOL RegisterCMHotKey( HINSTANCE hInstance )
{
	WNDCLASS wcEdit;
	GetClassInfo( hInstance, _T("EDIT"), &wcEdit );

	WNDCLASS wc;
    wc.style = 0;
    wc.lpfnWndProc = ( WNDPROC )HotKeyWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = wcEdit.cbWndExtra;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = CMHOTKEY_CLASS;

	
	return ( RegisterClass( &wc ) != 0 );
}

BOOL UnregisterCMHotKey( HINSTANCE hInstance )
{
	return UnregisterClass( CMHOTKEY_CLASS, hInstance );
}

#define UM_UPDATEHOTKEYTEXT			WM_USER + 12667
#define UM_INTERNALSETHOTKEY		WM_USER + 12668
#define UM_TAKEBACKFOCUS			WM_USER + 12669

BOOL g_bCanUpdateText;
BOOL g_bEatCommand;
LRESULT PASCAL HotKeyWndProc(HWND hWnd, register UINT uMsg, register WPARAM wParam, register LPARAM lParam)
{
	CM_HOTKEY *pHotKey = ( CM_HOTKEY * ) GetProp( hWnd, _T("HOTKEY") ); 

	switch ( uMsg )
	{
		case WM_CREATE:
			{
				CM_HOTKEY *pHotKey = new CM_HOTKEY;
				ZeroMemory( pHotKey, sizeof( CM_HOTKEY ) );
				SetProp( hWnd, _T("HOTKEY"), pHotKey ); 
				SetWindowLong( hWnd, GWL_EXSTYLE, GetWindowLong( hWnd, GWL_EXSTYLE ) | WS_EX_CLIENTEDGE );
				SetWindowPos( hWnd, NULL, -1, -1, -1, -1, SWP_NOZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_DRAWFRAME );
				SendMessage( hWnd, UM_UPDATEHOTKEYTEXT, 0, 0 );
				g_bEatCommand = FALSE;
				break;
			}

		case WM_DESTROY:
			{
				RemoveProp( hWnd, _T("HOTKEY") );
				delete pHotKey;
				break;
			}

		case UM_TAKEBACKFOCUS:
		{
			if ( GetFocus() != hWnd )
			{
				SetFocus( hWnd );
			}
			g_bEatCommand = FALSE;
			return 0;
		}

		case HOTM_QUERYEATCOMMAND:
		{
			return g_bEatCommand;
		}

		case UM_UPDATEHOTKEYTEXT:
		{
			TCHAR szHotKey[ 100 ];
			*szHotKey = '\0';
			GetHotKeyString( *pHotKey, szHotKey );
			g_bCanUpdateText = TRUE;
			SetWindowText( hWnd, szHotKey );
			g_bCanUpdateText = FALSE;
			SendMessage( hWnd, EM_SETSEL, 1000, 1000 );

			return 0;
		}

		case UM_INTERNALSETHOTKEY:
		{
			UINT nVirtKey = wParam;
			switch ( nVirtKey )
			{
				case VK_CONTROL:
				case VK_SHIFT:
				case VK_MENU:
				{
					nVirtKey = 0;
				}
			}

			if ( nVirtKey )
			{
				CM_HOTKEY cmHotKey = *pHotKey;
				BYTE byModifiers = 0;
				byModifiers |= ( ( GetKeyState( VK_CONTROL ) < 0 ) ? HOTKEYF_CONTROL : 0 );
				byModifiers |= ( ( GetKeyState( VK_SHIFT ) < 0 ) ? HOTKEYF_SHIFT : 0 );
				byModifiers |= ( ( GetKeyState( VK_MENU ) < 0 ) ? HOTKEYF_ALT : 0 );

				if ( cmHotKey.byModifiers1 || cmHotKey.nVirtKey1 )
				{
					if ( !cmHotKey.byModifiers2 && !cmHotKey.nVirtKey2 )
					{
						cmHotKey.byModifiers2 = byModifiers;
						cmHotKey.nVirtKey2 = nVirtKey;
					}
					else
					{
						cmHotKey.byModifiers2 = 0;
						cmHotKey.nVirtKey2 = 0;
						goto set_loword;
					}
				}
				else
				{
					set_loword:
					ASSERT( !cmHotKey.byModifiers2 && !cmHotKey.nVirtKey2 );
					cmHotKey.byModifiers1 = byModifiers;
					cmHotKey.nVirtKey1 = nVirtKey;
				}

				SendMessage( hWnd, HOTM_SETHOTKEY, 0, ( LPARAM ) &cmHotKey );
			}
			return 0;
		}

		case HOTM_SETHOTKEY:
		{
			if ( lParam )
			{
				*pHotKey = *( CM_HOTKEY * ) lParam;
			}
			else
			{
				CM_HOTKEY cmHotKey = { 0, 0, 0, 0 };
				*pHotKey = cmHotKey;
			}
			SendMessage( hWnd, UM_UPDATEHOTKEYTEXT, 0, 0 );
			return 0;
		}

		case HOTM_GETHOTKEY:
		{
			*( ( CM_HOTKEY * ) lParam ) = *pHotKey;
			break;
		}

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			g_bEatCommand = TRUE;
			SendMessage( hWnd, UM_INTERNALSETHOTKEY, wParam, lParam );
			PostMessage( hWnd, UM_TAKEBACKFOCUS, 0, 0 );
			return DefWindowProc( hWnd, uMsg, wParam, lParam );
		}
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			return DefWindowProc( hWnd, uMsg, wParam, lParam );
		}
		case WM_CHAR:
		case WM_SYSCHAR:
		{
			return DefWindowProc( hWnd, uMsg, wParam, lParam );
		}

	case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDBLCLK:
		case WM_CONTEXTMENU:
		{
			SetFocus( hWnd );
			return 0;
		}

		case WM_SYSCOMMAND:
		{
			return 0;
		}
		case WM_SETTEXT:
		{
			if ( g_bCanUpdateText )
		{
				break;
			}
			else
			{
				return 0;
			}
		}
		case WM_GETDLGCODE:
		{
			DWORD dwDlgCode = DLGC_WANTARROWS | DLGC_WANTCHARS | DLGC_WANTTAB;
			if ( HAS_FLAG( GetWindowLong( hWnd, GWL_STYLE ), ES_WANTRETURN ) )
			{
				dwDlgCode |= DLGC_WANTALLKEYS;
			}
			return dwDlgCode;
		}
	}

	// Other than the above messages, the hotkey control behaves identical to an edit control
	WNDCLASS wcEdit;
	VERIFY( GetClassInfo( ( HINSTANCE ) GetWindowLong( hWnd, GWL_HINSTANCE ), _T("EDIT"), &wcEdit ) );
#ifdef _ACTIVEX
	return CallWindowProc( wcEdit.lpfnWndProc, hWnd, uMsg, wParam, lParam );
#else
	#if _MSC_VER  > 1100 
		return CallWindowProc( ( WNDPROC ) wcEdit.lpfnWndProc, hWnd, uMsg, wParam, lParam );
	#else
		return CallWindowProc( ( FARPROC ) wcEdit.lpfnWndProc, hWnd, uMsg, wParam, lParam );
	#endif

#endif
}

