// ceditx.cpp : Implementation of CEditX
#include "precomp.h"
#ifdef _ACTIVEX
#include "editx.h"
#include "ceditx.h"
#include "posx.h"
#include "rangex.h"
#include "rectx.h"
#include "cmidlids.h"

EXTERN_C const IID IID_ICodeMax;
const IID IID_ICodeMaxEvents = {0xECEDB959,0xAC41,0x11d2,{0xAB,0x20,0x00,0x00,0x00,0x00,0x00,0x00}};
int CEditX::g_nRef = 0;
HACCEL CEditX::g_hAccel = NULL;
int CEditX::g_nAccelCount = 0;
#define MAX_NOTIFY_ARGS 6	// max number of args that will be passed thru IDispatch::Invoke()

CFontDisp::CFontDisp(CEditX *pEditX)
{
	m_pIFont = NULL;
	m_pEditX = pEditX;
}

CFontDisp::~CFontDisp()
{
}

ULONG STDMETHODCALLTYPE CFontDisp::Release()
{
	ULONG u = m_pIFont->Release();
	if (u == 0)
		{
		if (m_pEditX)
			m_pEditX->m_pFont = NULL;

		delete this;
		}
	return u;
}

HRESULT CFontDisp::GetFontDispatch(IFontDisp **ppIFont)
{
	HRESULT hr = S_OK;
	if (m_pIFont)
	{
		*ppIFont = (IFontDisp *) this;	// no addref required since this object doesn't die
		AddRef();
	}
	else
	{
		HFONT hFont = (HFONT) ::SendMessage(m_pEditX->m_hWnd, WM_GETFONT, 0, 0);
		LOGFONT lf;
		VERIFY(GetObject(hFont, GetObject(hFont, 0, NULL), &lf));
		USES_CONVERSION;
		HDC hDC = ::GetDC(NULL);
		double cyFont = (((double)lf.lfHeight * 72.0) / (double)GetDeviceCaps(hDC, LOGPIXELSY)) * 10000.0;
		FONTDESC fd =
			{
			sizeof(FONTDESC),
			T2OLE(lf.lfFaceName),
			{ abs(cyFont), 0 },
			(short) lf.lfWeight,
			(short) lf.lfCharSet,
			lf.lfItalic,
			lf.lfUnderline,
			lf.lfStrikeOut
			};
		hr = OleCreateFontIndirect(&fd, IID_IFontDisp, (void **) &m_pIFont);	// internal addref happened here
		::ReleaseDC(NULL, hDC);
		if (SUCCEEDED(hr))
		{
			*ppIFont = (IFontDisp *) this;
			AddRef();
		}
		else
		{
			*ppIFont = NULL;
		}
	}

	return hr;
}

HRESULT STDMETHODCALLTYPE CFontDisp::QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)
{
	if (riid == IID_IUnknown ||
	     riid == IID_IDispatch ||
	     riid == IID_IFontDisp)
	{
		*ppvObject = this;
		AddRef();
		return S_OK;
	}
	else
		return m_pIFont->QueryInterface(riid, ppvObject);
}
HRESULT STDMETHODCALLTYPE CFontDisp::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, 
                                             WORD wFlags, DISPPARAMS __RPC_FAR *pDispParams,
                                             VARIANT __RPC_FAR *pVarResult, EXCEPINFO __RPC_FAR *pExcepInfo,
                                             UINT __RPC_FAR *puArgErr)
{
	if (!m_pIFont)
		return E_FAIL;

	HRESULT hr = m_pIFont->Invoke(dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);

	if (SUCCEEDED(hr) && (wFlags & (DISPATCH_PROPERTYPUT | DISPATCH_PROPERTYPUTREF)))
	{
		// the font object was updated -- make sure the control reflects this
		hr = m_pEditX->put_Font((IFontDisp *) this);
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CEditX

CEditX::CEditX()
{
	m_bWindowOnly = TRUE;
	m_pEdit = NULL;
	m_pState = NULL;
	m_nFreezeEvents = FALSE;
	m_pFont = NULL;
	m_hImageList = NULL;
	m_bRecomposeOnResize = TRUE;
	if (++g_nRef == 1)
		ResetAccelTable();
	static int nDebug = 0;
	ASSERT(++nDebug != 1);  // uncomment for debugging before container catches exceptions
}

CEditX::~CEditX()
{
	FreeState();
	if (--g_nRef == 0)
	{
		if (g_hAccel)
		{
			VERIFY(DestroyAcceleratorTable(g_hAccel));
			g_hAccel = NULL;
		}
		g_nAccelCount = 0;
	}

	if (m_pFont)
	{
		m_pFont->m_pEditX = NULL;
		m_pFont->Release();
	}

	if (m_hImageList)
		ImageList_Destroy(m_hImageList);
}

void CEditX::FreeState()
{
	if (m_pState)
	{
		delete m_pState;
		m_pState = NULL;
	}
}

STDMETHODIMP CEditX::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ICodeMax,
		&IID_ICodeMaxEvents
	};
	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (CMIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

void CEditX::RaiseLastErrorException() const
{
	ICreateErrorInfo *pICreateErr;
	HRESULT hr = CreateErrorInfo(&pICreateErr);
	if (SUCCEEDED(hr))
	{
		pICreateErr->SetSource(L"CodeMax");
		wchar_t wszMsg[ 1024 ];
		LPTSTR pszMsg;

		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
	  				  NULL,
					  GetLastError(),
	  				  0,
	  				  (LPTSTR) &pszMsg, 0, NULL);
#ifdef _UNICODE
		_tcsncpy(wszMsg, pszMsg, ARRAY_SIZE(wszMsg) - 1);
#else
		int nLen = _tcslen(pszMsg);
		MultiByteToWideChar(CP_ACP, 0, pszMsg, nLen, wszMsg, nLen);
		wszMsg[ nLen ] = L'\0';
#endif
		LocalFree(pszMsg);	// FormatMessage() requires this

		pICreateErr->SetDescription(wszMsg);
		IErrorInfo *pIErrorInfo = NULL;
		hr = pICreateErr->QueryInterface(IID_IErrorInfo, (void **) &pIErrorInfo);
		if (SUCCEEDED(hr))
		{
			SetErrorInfo(0, pIErrorInfo);
			pIErrorInfo->Release();
		}
		pICreateErr->Release();
	}
}

void CEditX::SetEdit(CEdit *pEdit)
{
	if (!pEdit && m_pEdit && m_pState)
	{
		// remember the state before the control is destroyed
		m_pState->Load(this);
	}
	m_pEdit = pEdit;
}

HRESULT CEditX::Quick_n_DirtyActivate()
{
	if (!::IsWindow(m_hWnd))
	{
		RECT rc = { 0, 0, 100, 100 };
		return DoVerbInPlaceActivate(&rc, NULL);
	}

	return E_FAIL;
}

HRESULT CEditX::OnDraw(ATL_DRAWINFO& di)
{
	// this is a window-only control, so this code should not be called, but
	// some containers do anyway.  The control is painted in its WM_PAINT
	// handler.
	if (!m_pEdit)
	{
		RECT& rc = *(RECT*)di.prcBounds;
		HDC hDC = di.hdcDraw;
		SIZE size;
	
		Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);
		LPCTSTR psz = LoadStringPtr(IDS_ACTIVEX_WIN_TEXT);
		int cb = _tcslen(psz);
		HDC hDCScreen = ::GetDC(NULL);
		HFONT hFontSys = (HFONT) GetStockObject(SYSTEM_FONT);
		SelectObject(hDCScreen, hFontSys);
		SetMapMode(hDCScreen, GetMapMode(hDC));
		GetWindowExtEx(hDC, &size);
		SetWindowExtEx(hDCScreen, size.cx, size.cy, NULL);
		HFONT hfontOld = (HFONT) SelectObject(hDC, hFontSys);
		VERIFY(GetTextExtentPoint32(hDCScreen, psz, cb, &size));
		::ReleaseDC(NULL, hDCScreen);
		COLORREF cr = SetTextColor(hDC, RGB(0, 0, 0));
		TextOut(hDC, 
		         rc.left + (rc.right - rc.left - size.cx) / 2,
		         rc.top + (rc.bottom - rc.top - size.cy) / 2,
		         psz,
		         cb);
		int cyHScroll = GetSystemMetrics(SM_CYHSCROLL);
		int cxVScroll = GetSystemMetrics(SM_CXVSCROLL);

		// Draw the fake sizebox
		RECT rcScroll = rc;
		rcScroll.top = rcScroll.bottom - cyHScroll;
		rcScroll.left = rcScroll.right - cxVScroll;
		DrawFrameControl(hDC, &rcScroll, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);

		// Draw the fake horz scrollbar
		rcScroll = rc;
		rcScroll.top = rcScroll.bottom - cyHScroll;
		rcScroll.right -= cxVScroll;
		HPEN hpen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOWFRAME));
		HPEN hpenOld = (HPEN) SelectObject(hDC, hpen);
		HBRUSH hbrOld = (HBRUSH) SelectObject(hDC, GetStockObject(WHITE_BRUSH));
		Rectangle(hDC, rcScroll.left, rcScroll.top, rcScroll.right, rcScroll.bottom);
		rcScroll.right = rc.left + cxVScroll;
		DrawFrameControl(hDC, &rcScroll, DFC_SCROLL, DFCS_SCROLLLEFT | DFCS_FLAT | DFCS_MONO);
		rcScroll.left = rc.right - 2 * cxVScroll + 1;
		rcScroll.right = rcScroll.left + cxVScroll;
		DrawFrameControl(hDC, &rcScroll, DFC_SCROLL, DFCS_SCROLLRIGHT | DFCS_FLAT | DFCS_MONO);

		// Draw the fake vert scrollbar
		rcScroll = rc;
		rcScroll.left = rcScroll.right - cxVScroll;
		rcScroll.bottom -= cyHScroll;
		Rectangle(hDC, rcScroll.left, rcScroll.top, rcScroll.right, rcScroll.bottom);
		rcScroll.bottom = rc.top + cyHScroll;
		DrawFrameControl(hDC, &rcScroll, DFC_SCROLL, DFCS_SCROLLUP | DFCS_FLAT | DFCS_MONO);
		rcScroll.top = rc.bottom - 2 * cyHScroll + 1;
		rcScroll.bottom = rcScroll.top + cyHScroll;
		DrawFrameControl(hDC, &rcScroll, DFC_SCROLL, DFCS_SCROLLDOWN | DFCS_FLAT | DFCS_MONO);

		SelectObject(hDC, hpenOld);
		DeleteObject(hpen);
		SelectObject(hDC, hbrOld);
		SelectObject(hDC, hfontOld);
		SetTextColor(hDC, cr);
	}
	return S_OK;
}

BOOL CEditX::IsInDesignMode() const
{
	return (m_nFreezeEvents > 0);
}

void CEditX::OnFontChanged()
{
	if (m_pFont)
		{
		m_pFont->Release();
		m_pFont = NULL;
		}
}

BOOL CEditX::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
		LRESULT& lResult, DWORD /*dwMsgMapID */)
{
	if (hWnd != m_hWnd)
	{
		lResult = FALSE;
		return FALSE;
	}

	// make sure this control is around for the duration 
	// of this call, since an accelerator may shut down
	// the window.
	AddRef();

	lResult = DefWindowProc(uMsg, wParam, lParam);

	Release();

	switch (uMsg)
	{
		case WM_CREATE:
		{
			m_pEdit = (CEdit *) ::GetWindowLong(hWnd, 0);
			if (m_pEdit)
			{
				m_pEdit->SetEditX(this);
				if (m_pState)
				{
					// transfer local copy of persistent data to control
					m_pState->Save(this);
					FreeState();
				}
				NotifySiteofAccelChange();
			}
			break;
		}
		case WM_DESTROY:
		{
			// transfer control state to local persistent copy
			FreeState();
			m_pState = new CControlState();
			m_pState->Load(this);
			break;
		}
		case WM_NCDESTROY:
		{
			if (m_pEdit)
			{
				m_pEdit->SetEditX(NULL);
				m_pEdit = NULL;
			}
			break;
		}
		case WM_SETFOCUS:
		{
			// inform control site so default keys work correctly
			BOOL bDontCare;
			lResult = CComControlBase::OnSetFocus(uMsg, wParam, lParam, bDontCare);
			break;
		}
		case WM_KILLFOCUS:
		{
			// inform control site so default keys work correctly
			BOOL bDontCare;
			lResult = CComControlBase::OnKillFocus(uMsg, wParam, lParam, bDontCare);
			break;
		}
	}

	return TRUE;
}

void CEditX::OnChanged(int nDispID)
{
	m_bRequiresSave = TRUE;
	FireOnChanged(nDispID);
	FireViewChange();
}

HRESULT CEditX::GetControlInfo(LPCONTROLINFO pCI)
{
	pCI->cb = sizeof(CONTROLINFO);
	pCI->hAccel = g_hAccel;
	pCI->cAccel = g_nAccelCount;
	pCI->dwFlags = CTRLINFO_EATS_RETURN | CTRLINFO_EATS_ESCAPE;
	return S_OK;
}

HRESULT CEditX::OnMnemonic(LPMSG pMsg)
{
	MSG msg = *pMsg;
	msg.hwnd = m_hWnd;
	DispatchMessage(&msg);
	return S_OK;
}

BOOL CEditX::ControlSiteWantsAccelerator(LPMSG pmsg)
{
	BOOL bHandled = FALSE;
	DWORD dwKeyMod = 0;
	if (::GetKeyState(VK_SHIFT) < 0)
		dwKeyMod |= HOTKEYF_SHIFT;
	if (::GetKeyState(VK_CONTROL) < 0)
		dwKeyMod |= HOTKEYF_CONTROL;
	if (::GetKeyState(VK_MENU) < 0)
		dwKeyMod |= HOTKEYF_ALT;

	switch (pmsg->wParam)
	{
		// VB will process these keys on behalf of the user and
		// cycle thru windows.  We don't want this behavior because
		// it completely hoses CodeMax.  This happens when you put
		// two CodeMax windows on the same form.
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
		case VK_TAB:
		case VK_RETURN:
			if (dwKeyMod == 0)
				break;
		default:
		{
			CComPtr<IOleControlSite> spCtlSite;
			if (SUCCEEDED(InternalGetSite(IID_IOleControlSite, (void**) &spCtlSite)))
			{
				if (spCtlSite)
				{
					WPARAM wParamPrev = pmsg->wParam;
					bHandled = (spCtlSite->TranslateAccelerator(pmsg, dwKeyMod) == S_OK) ||
					             (!pmsg->wParam && wParamPrev);	// ate the key
				}
			}
			break;
		}
	}

	return bHandled;
}

HRESULT CEditX::TranslateAccelerator(LPMSG pmsg)
{
	// if don't have the focus, I don't handle keyboard shortcuts!
	if (GetFocus() != m_hWnd)
		return S_FALSE;

	// if this message is not a keyboard message, ignore it
	if (pmsg->message < WM_KEYFIRST || pmsg->message > WM_KEYLAST)
		return S_FALSE;

	// make safe local temp copy before modifying
	MSG msg = *pmsg;
	pmsg = &msg;
	msg.hwnd = m_hWnd;

	HRESULT hr = S_OK;

	// make sure this control is around for the duration 
	// of this call, since an accelerator may shut down
	// the window.
	AddRef();

	BOOL bHandled = ControlSiteWantsAccelerator(pmsg);

	Release();

	return bHandled ? S_OK : S_FALSE;
}

HRESULT CEditX::OnFrameWindowActivate(BOOL bActivate)
{
	if (bActivate && ::IsWindow(m_hWnd))
	{
		::SetFocus(m_hWnd);
	}
	return S_OK;
}

/* static */ void CEditX::ResetAccelTable()
{
	if (g_hAccel)
	{
		VERIFY(DestroyAcceleratorTable(g_hAccel));
		g_hAccel = NULL;
	}

	if (CEdit::g_nHotKeyCount)
	{
		int nAccel = 0;
		// count up the number of accelerators.  Some hotkeys
		// require two keystrokes, so we must run thru all of
		// them and account for them.
		g_nAccelCount = CEdit::g_nHotKeyCount;
		int i = 0;
		while (i < CEdit::g_nHotKeyCount)
		{
			CHotKey *pHotKey = &CEdit::g_pHotKeys[ i ];
			// account for two-keystroke hotkeys
			if (pHotKey->cmHotKey.nVirtKey2)
				g_nAccelCount++;
			i++;
		}

		LPACCEL pAccels = new ACCEL[ g_nAccelCount ];
		for (i = 0; i < CEdit::g_nHotKeyCount; i++)
			GetACCEL(pAccels, nAccel, &CEdit::g_pHotKeys[ i ]);
		ASSERT(nAccel == g_nAccelCount);
		g_hAccel = CreateAcceleratorTable(pAccels, g_nAccelCount);
		ASSERT(g_hAccel);
		delete [] pAccels;

		// tell the client site about all of the controls that have
		// new accel tables
		for (int nWnd = 0; nWnd < CEdit::g_nhWndCount; nWnd++)
		{
			HWND hWnd = CEdit::g_phWnds[ nWnd ];
			ASSERT(::IsWindow(hWnd));
			CEdit *pCtrl = (CEdit *) ::GetWindowLong(hWnd, 0);
			ASSERT(pCtrl);
			if (pCtrl->m_pEditX)
				pCtrl->m_pEditX->NotifySiteofAccelChange();
		}
	}
}

void CEditX::NotifySiteofAccelChange()
{
	// tell the container that the accels changed
	if (m_spClientSite)
	{
		CComQIPtr <IOleControlSite, &IID_IOleControlSite> spSite(m_spClientSite);
		if (spSite)
			spSite->OnControlInfoChanged();
	}
}

/* static */ void CEditX::GetACCEL(LPACCEL pAccels, int &nAccel, const CHotKey *pHotKey)
{
	ASSERT(pAccels);
	ASSERT(pHotKey);

	int nAccels = (pHotKey->cmHotKey.nVirtKey2 ? 2 : 1);
	BYTE byModifiers = pHotKey->cmHotKey.byModifiers1;
	UINT nVirtKey = pHotKey->cmHotKey.nVirtKey1;
	for (int i = 0; i < nAccels; i++)
	{
		LPACCEL pAccel = pAccels + nAccel + i;
		pAccel->fVirt = FVIRTKEY;
		if (HAS_FLAG(byModifiers, HOTKEYF_CONTROL))
			pAccel->fVirt |= FCONTROL;
		if (HAS_FLAG(byModifiers, HOTKEYF_ALT))
			pAccel->fVirt |= FALT;
		if (HAS_FLAG(byModifiers, HOTKEYF_SHIFT))
			pAccel->fVirt |= FSHIFT;
		pAccel->key = (WORD) nVirtKey;
		pAccel->cmd = pHotKey->wCmd;
		byModifiers = (pHotKey->cmHotKey.byModifiers2 ? pHotKey->cmHotKey.byModifiers2 : pHotKey->cmHotKey.byModifiers1);
		nVirtKey = pHotKey->cmHotKey.nVirtKey2;
	}
	nAccel += nAccels;
}

HRESULT CEditX::IOleObject_SetExtent(DWORD dwDrawAspect, SIZEL *psizel)
{
	HRESULT hr = CComControlBase::IOleObject_SetExtent(dwDrawAspect, psizel);
	if (SUCCEEDED(hr) && m_hWnd)
	{
		// resize the control
		HDC hDC = ::GetDC(NULL);
		int cx = MulDiv(psizel->cx, GetDeviceCaps(hDC, LOGPIXELSX), 2540);
		int cy = MulDiv(psizel->cy, GetDeviceCaps(hDC, LOGPIXELSY), 2540);
		::ReleaseDC(NULL, hDC);
		RECT rcW;
		GetWindowRect(&rcW);
		if ((rcW.right - rcW.left != cx) ||
		     (rcW.bottom - rcW.top != cy))
		{
			::SetWindowPos(m_hWnd, NULL, -1, -1, cx, cy, SWP_NOZORDER | SWP_NOMOVE);
		}
	}
	return hr;
}

HRESULT CEditX::DoVerbProperties(LPCRECT /*prcPosRect*/, HWND hwndParent)
{
	HWND hWndSave = m_pEdit ? m_pEdit->GetDlgParent() : NULL;
	CM_SetDlgParent(m_hWnd, hwndParent);
	CM_ExecuteCmd(m_hWnd, CMD_PROPERTIES, 0);
	CM_SetDlgParent(m_hWnd, hWndSave);
	return S_OK;
}

BOOL CEditX::IsActiveXEvent(int nEvent) const
{
	return (nEvent != CMN_DRAWLINE);
}

void CEditX::GetEventNotificationParams(int nEvent, LPNMHDR pnmhdr, VARIANTARG *pParams, int &nDispID, int &nArgs, VARIANT_BOOL *pbResult, BOOL &bHasRetVal) const
{
	nArgs = 0;
	nDispID = 0xffff;
	bHasRetVal = FALSE;
	
	// push on the BOOL return value reference parameter for those events that require a return value
	switch(nEvent)
	{
		case NM_CLICK:
		case NM_DBLCLK:
		case NM_RETURN:
		case NM_RCLICK:
		case CMN_SHOWPROPS:
		case CMN_CMDFAILURE:
		case CMN_KEYDOWN:
		case CMN_KEYUP:
		case CMN_KEYPRESS:
		case CMN_MOUSEDOWN:
		case CMN_MOUSEUP:
		case CMN_MOUSEMOVE:
		case CMN_FINDWRAPPED:
		{
			bHasRetVal = TRUE;
			VariantInit(pParams + nArgs);
			pParams[ nArgs ].vt = VT_BOOL | VT_BYREF;
			pParams[ nArgs++ ].pboolVal = pbResult;
			break;
		}
	}

	switch(nEvent)
	{
		default: { ASSERT(FALSE); break; } // not handled! Add a handler!
		case CMN_CHANGE:			{ nDispID = DISPID_NTFY_CHANGE; break; }
		case CMN_HSCROLL: 			{ nDispID = DISPID_NTFY_HSCROLL; break; }
		case CMN_VSCROLL: 			{ nDispID = DISPID_NTFY_VSCROLL; break; }
		case CMN_SELCHANGE: 		{ nDispID = DISPID_NTFY_SELCHANGE; break; }
		case CMN_VIEWCHANGE: 		{ nDispID = DISPID_NTFY_VIEWCHANGE; break; }
		case CMN_MODIFIEDCHANGE:	{ nDispID = DISPID_NTFY_MODIFIEDCHANGE; break; }
		case CMN_PROPSCHANGE: 		{ nDispID = DISPID_NTFY_PROPSCHANGE; break; }
		case CMN_CREATE: 			{ nDispID = DISPID_NTFY_CREATE; break; }
		case CMN_DESTROY: 			{ nDispID = DISPID_NTFY_DESTROY; break; }
		case NM_SETFOCUS: 			{ nDispID = DISPID_NTFY_SETFOCUS; break; }
		case NM_KILLFOCUS: 			{ nDispID = DISPID_NTFY_KILLFOCUS; break; }
		case NM_RDBLCLK: 			{ nDispID = DISPID_NTFY_RDBLCLK; break; }
		case CMN_SHOWPROPS: 		{ nDispID = DISPID_NTFY_SHOWPROPS; break; }
		case NM_CLICK: 				{ nDispID = DISPID_NTFY_CLICK; break; }
		case NM_DBLCLK: 			{ nDispID = DISPID_NTFY_DBLCLK; break; }
		case NM_RETURN: 			{ nDispID = DISPID_NTFY_RETURN; break; }
		case NM_RCLICK: 			{ nDispID = DISPID_NTFY_RCLICK; break; }
		case CMN_OVERTYPECHANGE:	{ nDispID = DISPID_NTFY_OVERTYPECHANGE; break; }
	
		case CMN_DELETELINE:
		{
			CM_DELETELINEDATA *pdld = (CM_DELETELINEDATA *) pnmhdr;

			nDispID = DISPID_NTFY_DELETELINE;

			// push on lParam param
			VariantInit(pParams + nArgs);
			pParams[ nArgs ].vt = VT_I4;
			pParams[ nArgs++ ].lVal = pdld->lParam;

			// push on line param
			VariantInit(pParams + nArgs);
			pParams[ nArgs ].vt = VT_I4;
			pParams[ nArgs++ ].lVal = pdld->nLine;
			break;
		}
		case CMN_CMDFAILURE:
		{
			CM_CMDFAILUREDATA *pcfd = (CM_CMDFAILUREDATA *) pnmhdr;

			nDispID = DISPID_NTFY_CMDFAILURE;

			// push on dwErr param
			VariantInit(pParams + nArgs);
			pParams[ nArgs ].vt = VT_I4;
			pParams[ nArgs++ ].lVal = pcfd->dwErr;

			// push on wCmd param
			VariantInit(pParams + nArgs);
			pParams[ nArgs ].vt = VT_I4;
			pParams[ nArgs++ ].lVal = pcfd->wCmd;
			break;
		}
		case CMN_REGISTEREDCMD:
		{
			CM_REGISTEREDCMDDATA *pcfd = (CM_REGISTEREDCMDDATA *) pnmhdr;

			nDispID = DISPID_NTFY_REGISTEREDCMD;

			// push on wCmd param
			VariantInit(pParams + nArgs);
			pParams[ nArgs ].vt = VT_I4;
			pParams[ nArgs++ ].lVal = pcfd->wCmd;
			break;
		}

		case CMN_KEYDOWN:
			nDispID = DISPID_NTFY_KEYDOWN;
			goto key_params;
		case CMN_KEYUP:
			nDispID = DISPID_NTFY_KEYUP;
			goto key_params;
		case CMN_KEYPRESS:
		{
			nDispID = DISPID_NTFY_KEYPRESS;
			key_params:
			CM_KEYDATA *pkd = (CM_KEYDATA *) pnmhdr;

			// push on nKeyModifier param
			VariantInit(pParams + nArgs);
			pParams[ nArgs ].vt = VT_I4;
			pParams[ nArgs++ ].lVal = pkd->nKeyModifier;

			// push on nKeyCode param
			VariantInit(pParams + nArgs);
			pParams[ nArgs ].vt = VT_I4;
			pParams[ nArgs++ ].lVal = pkd->nKeyCode;
			break;
		}

		case CMN_MOUSEDOWN:
			nDispID = DISPID_NTFY_MOUSEDOWN;
			goto mouse_params;
		case CMN_MOUSEUP:
			nDispID = DISPID_NTFY_MOUSEUP;
			goto mouse_params;
		case CMN_MOUSEMOVE:
		{
			nDispID = DISPID_NTFY_MOUSEMOVE;
			mouse_params:
			CM_MOUSEDATA *pmd = (CM_MOUSEDATA *) pnmhdr;

			// push on y param
			VariantInit(pParams + nArgs);
			pParams[ nArgs ].vt = VT_I4;
			pParams[ nArgs++ ].lVal = pmd->pt.y;

			// push on x param
			VariantInit(pParams + nArgs);
			pParams[ nArgs ].vt = VT_I4;
			pParams[ nArgs++ ].lVal = pmd->pt.x;

			// push on nKeyModifier param
			VariantInit(pParams + nArgs);
			pParams[ nArgs ].vt = VT_I4;
			pParams[ nArgs++ ].lVal = pmd->nKeyModifier;

			// push on nButton param
			VariantInit(pParams + nArgs);
			pParams[ nArgs ].vt = VT_I4;
			pParams[ nArgs++ ].lVal = pmd->nButton;
			break;
		}
		case CMN_FINDWRAPPED:
		{
			nDispID = DISPID_NTFY_FINDWRAPPED;
			CM_FINDWRAPPEDDATA *pfwd = (CM_FINDWRAPPEDDATA *) pnmhdr;

			// push on bForward param
			VariantInit(pParams + nArgs);
			pParams[ nArgs ].vt = VT_BOOL;
			pParams[ nArgs++ ].boolVal = pfwd->bForward;

			// push on wCmd param
			VariantInit(pParams + nArgs);
			pParams[ nArgs ].vt = VT_I4;
			pParams[ nArgs++ ].lVal = pfwd->wCmd;
			break;
		}
	}

	// push on the Dispatch interface pointer
	VariantInit(pParams + nArgs);
	pParams[ nArgs ].vt = VT_DISPATCH;
	pParams[ nArgs++ ].pdispVal = (IDispatch *) this;

	ASSERT(nArgs <= MAX_NOTIFY_ARGS);	// need to bump up MAX_NOTIFY_ARGS
}


BOOL CEditX::FireControlEvent(int nEvent, LPNMHDR pnmhdr)
{
	BOOL bContinue = TRUE;
	if (IsActiveXEvent(nEvent))
		{
		if (m_nFreezeEvents > 0)
		{
			return TRUE;
		}

		CComQIPtr<IConnectionPointContainer, &IID_IConnectionPointContainer> pCPC(GetUnknown());
		if (!pCPC)
			return TRUE;
		CComPtr<IConnectionPoint> pCP;
		pCPC->FindConnectionPoint(IID_ICodeMaxEvents, &pCP);
		if (!pCP)
			return TRUE;
		CComPtr<IEnumConnections> pEnum;

		if (FAILED(pCP->EnumConnections(&pEnum)))
			return TRUE;

		CONNECTDATA cd;
		while (pEnum->Next(1, &cd, NULL) == S_OK)
		{
			if (cd.pUnk)
			{
				CComQIPtr<IDispatch, &IID_ICodeMaxEvents> pSink(cd.pUnk);
				if (pSink)
				{
					int nDispID;
					BOOL bHasRetVal;
					VARIANTARG params[ MAX_NOTIFY_ARGS ];
					DISPID dispids[ MAX_NOTIFY_ARGS ];
					VARIANT_BOOL bResult = FALSE;
					int nArgs;
					GetEventNotificationParams(nEvent, pnmhdr, params, nDispID, nArgs, &bResult, bHasRetVal);
					for (int i = 0; i < nArgs; i++)
					{
						dispids[ i ] = nArgs - i - 1;
					}

					VARIANT varRetVal;
					VariantInit(&varRetVal);
					varRetVal.vt = VT_HRESULT;

					DISPPARAMS disp = {params, dispids, nArgs, 0};
					EXCEPINFO excinfo; 
					pSink->Invoke(nDispID,
								   IID_NULL,
								   LOCALE_USER_DEFAULT,
								   DISPATCH_METHOD,
								   &disp,
								   &varRetVal,
								   &excinfo,
								   NULL);
					if (bHasRetVal)
					{
						bContinue = (bResult == 0);
					}
				}
				cd.pUnk->Release();
			}
		}
	}
	return bContinue;
}

HRESULT CEditX::IPersistStreamInit_Load(LPSTREAM pStm, ATL_PROPMAP_ENTRY* /* pMap */)
{
	HRESULT hr = S_OK;
	if (!m_pState)
	{
		m_pState = new CControlState;
	}
	hr = m_pState->Load(pStm);

	if (SUCCEEDED(hr))
		hr = pStm->Read(&m_sizeExtent, sizeof(m_sizeExtent), NULL);

	if (SUCCEEDED(hr))
	{
		if (::IsWindow(m_hWnd))
		{
			// transfer control state to the real control
	    	m_pState->Save(this);
	    }
	}

	return hr;
}

HRESULT CEditX::IPersistStreamInit_Save(LPSTREAM pStm, BOOL /* fClearDirty */, ATL_PROPMAP_ENTRY* /* pMap */)
{
	HRESULT hr = S_OK;

	if (!m_pState)
	{
		m_pState = new CControlState;
	}

	if (::IsWindow(m_hWnd))
	{
		// transfer control state to the real control
	    m_pState->Load(this);
	}

	hr = m_pState->Save(this, pStm);

	if (SUCCEEDED(hr))
		hr = pStm->Write(&m_sizeExtent, sizeof(m_sizeExtent), NULL);

	return hr;
}

///////////////////////////////////////
//
// Implement BOOL prop get/set methods
//
//
IMPLEMENT_AUT_BOOL_PROP_GET_PUT(DISPID_CMPROP_COLORSYNTAX, ColorSyntax, CM_IsColorSyntaxEnabled, CM_EnableColorSyntax);
IMPLEMENT_AUT_BOOL_PROP_GET_PUT(DISPID_CMPROP_DISPLAYWHITESPACE, DisplayWhitespace, CM_IsWhitespaceDisplayEnabled, CM_EnableWhitespaceDisplay);
IMPLEMENT_AUT_BOOL_PROP_GET_PUT(DISPID_CMPROP_EXPANDTABS, ExpandTabs, CM_IsTabExpandEnabled, CM_EnableTabExpand);
IMPLEMENT_AUT_BOOL_PROP_GET_PUT(DISPID_CMPROP_SMOOTHSCROLLING, SmoothScrolling, CM_IsSmoothScrollingEnabled, CM_EnableSmoothScrolling);
IMPLEMENT_AUT_BOOL_PROP_GET_PUT(DISPID_CMPROP_READONLY, ReadOnly, CM_IsReadOnly, CM_SetReadOnly);
IMPLEMENT_AUT_BOOL_PROP_GET_PUT(DISPID_CMPROP_LINETOOLTIPS, LineToolTips, CM_IsLineToolTipsEnabled, CM_EnableLineToolTips);
IMPLEMENT_AUT_BOOL_PROP_GET_PUT(DISPID_CMPROP_DISPLAYLEFTMARGIN, DisplayLeftMargin, CM_IsLeftMarginEnabled, CM_EnableLeftMargin);
IMPLEMENT_AUT_BOOL_PROP_GET_PUT(DISPID_CMPROP_ENABLECOLUMNSEL, EnableColumnSel, CM_IsColumnSelEnabled, CM_EnableColumnSel);
IMPLEMENT_AUT_BOOL_PROP_GET_PUT(DISPID_CMPROP_ENABLEDRAGDROP, EnableDragDrop, CM_IsDragDropEnabled, CM_EnableDragDrop);
IMPLEMENT_AUT_BOOL_PROP_GET_PUT(DISPID_CMPROP_OVERTYPE, Overtype, CM_IsOvertypeEnabled, CM_EnableOvertype);
IMPLEMENT_AUT_BOOL_PROP_GET_PUT(DISPID_CMPROP_SEARCHCASESENSITIVE, SearchCaseSensitive, CM_IsCaseSensitiveEnabled, CM_EnableCaseSensitive);
IMPLEMENT_AUT_BOOL_PROP_GET_PUT(DISPID_CMPROP_REPLACEPRESERVECASE, ReplacePreserveCase, CM_IsPreserveCaseEnabled, CM_EnablePreserveCase);
IMPLEMENT_AUT_BOOL_PROP_GET_PUT(DISPID_CMPROP_SEARCHWHOLEWORD, SearchWholeWord, CM_IsWholeWordEnabled, CM_EnableWholeWord);
IMPLEMENT_AUT_BOOL_PROP_GET_PUT(DISPID_CMPROP_SEARCHREGEXP, SearchRegExp, CM_IsRegExpEnabled, CM_EnableRegExp);
IMPLEMENT_AUT_BOOL_PROP_GET_PUT(DISPID_CMPROP_MODIFIED, Modified, CM_IsModified, CM_SetModified);
IMPLEMENT_AUT_BOOL_PROP_GET_PUT(DISPID_CMPROP_ENABLECRLF, EnableCRLF, CM_IsCRLFEnabled, CM_EnableCRLF);
IMPLEMENT_AUT_BOOL_PROP_GET_PUT(DISPID_CMPROP_GLOBALPROPS, GlobalProps, CM_IsGlobalPropsEnabled, CM_EnableGlobalProps);
IMPLEMENT_AUT_BOOL_PROP_GET_PUT(DISPID_CMPROP_SELBOUNDS, SelBounds, CM_IsSelBoundsEnabled, CM_EnableSelBounds);
IMPLEMENT_AUT_BOOL_PROP_GET_PUT(DISPID_CMPROP_GLOBALPROPS, HideSel, CM_IsHideSelEnabled, CM_EnableHideSel);
IMPLEMENT_AUT_BOOL_PROP_GET_PUT(DISPID_CMPROP_NORMALIZECASE, NormalizeCase, CM_IsNormalizeCaseEnabled, CM_EnableNormalizeCase);
IMPLEMENT_AUT_BOOL_PROP_GET_PUT(DISPID_CMPROP_GLOBALPROPS, OvertypeCaret, CM_IsOvertypeCaretEnabled, CM_EnableOvertypeCaret);

HRESULT STDMETHODCALLTYPE CEditX::get_VScrollVisible(VARIANT_BOOL __RPC_FAR *pBool)
{
	CATCH_ERROR_NO_HWND();
	*pBool = (VARIANT_BOOL) (CM_HasScrollBar(m_hWnd, FALSE) ? -1 : 0);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::put_VScrollVisible(VARIANT_BOOL vBool)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_CMPROP_VSCROLLVISIBLE) == S_FALSE)
	{
		return S_FALSE;
	}
	HRESULT hRes = CME2HRESULT(CM_ShowScrollBar(m_hWnd, FALSE, vBool != 0));
	OnChanged(DISPID_CMPROP_VSCROLLVISIBLE);
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::get_HScrollVisible(VARIANT_BOOL __RPC_FAR *pBool)
{
	CATCH_ERROR_NO_HWND();
	*pBool = (VARIANT_BOOL) (CM_HasScrollBar(m_hWnd, TRUE) ? -1 : 0);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::put_HScrollVisible(VARIANT_BOOL vBool)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_CMPROP_HSCROLLVISIBLE) == S_FALSE)
	{
		return S_FALSE;
	}
	HRESULT hRes = CME2HRESULT(CM_ShowScrollBar(m_hWnd, TRUE, vBool != 0));
	OnChanged(DISPID_CMPROP_HSCROLLVISIBLE);
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::get_EnableVSplitter(VARIANT_BOOL __RPC_FAR *pBool)
{
	CATCH_ERROR_NO_HWND();
	*pBool = (VARIANT_BOOL) (CM_IsSplitterEnabled(m_hWnd, FALSE) ? -1 : 0);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::put_EnableVSplitter(VARIANT_BOOL vBool)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_CMPROP_ENABLEVSPLITTER) == S_FALSE)
	{
		return S_FALSE;
	}
	HRESULT hRes = CME2HRESULT(CM_EnableSplitter(m_hWnd, FALSE, vBool != 0));
	OnChanged(DISPID_CMPROP_ENABLEVSPLITTER);
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::get_EnableHSplitter(VARIANT_BOOL __RPC_FAR *pBool)
{
	CATCH_ERROR_NO_HWND();
	*pBool = (VARIANT_BOOL) (CM_IsSplitterEnabled(m_hWnd, TRUE) ? -1 : 0);
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CEditX::put_EnableHSplitter(VARIANT_BOOL vBool)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_CMPROP_ENABLEHSPLITTER) == S_FALSE)
	{
		return S_FALSE;
	}
	HRESULT hRes = CME2HRESULT(CM_EnableSplitter(m_hWnd, TRUE, vBool != 0));
	OnChanged(DISPID_CMPROP_ENABLEHSPLITTER);
	return hRes;
}

////////////////////////////////////////
//
// Misc properties that are not BOOL
//
HRESULT STDMETHODCALLTYPE CEditX::put_Text(BSTR strText)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_TEXT) == S_FALSE)
	{
		return S_FALSE;
	}

	LPTSTR psz = BSTR2TSTR(strText);
	HRESULT hRes = CME2HRESULT(CM_SetText(m_hWnd, psz));
	delete psz;

	OnChanged(DISPID_TEXT);
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::get_Text(BSTR __RPC_FAR *pstrText)
{
	CATCH_ERROR_NO_HWND();
	TCHAR *psz = new TCHAR[ CM_GetTextLength(m_hWnd, NULL, FALSE) + 1 ];
	*psz = _T('\0');
	CM_GetText(m_hWnd, psz, NULL);
	USES_CONVERSION;
	*pstrText = T2BSTR(psz);
	delete psz;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::get_SelText(BSTR __RPC_FAR *pstrText)
{
	CATCH_ERROR_NO_HWND();
	CM_RANGE cmRange;
	CM_GetSel(m_hWnd, &cmRange, TRUE);
	TCHAR *psz = new TCHAR[ CM_GetTextLength(m_hWnd, &cmRange, FALSE) + 1 ];
	*psz = _T('\0');
	if (cmRange.posStart.nLine != cmRange.posEnd.nLine ||
	     cmRange.posStart.nCol != cmRange.posEnd.nCol)
	{
		CM_GetText(m_hWnd, psz, &cmRange);
	}
	USES_CONVERSION;
	*pstrText = T2BSTR(psz);
	delete psz;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::put_SelText(BSTR strText)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_CMPROP_SELTEXT) == S_FALSE)
	{
		return S_FALSE;
	}

	LPTSTR psz = BSTR2TSTR(strText);
	HRESULT hRes = CME2HRESULT(m_pEdit->OnReplaceText(NULL, (WPARAM) psz, TRUE));
	delete psz;

	OnChanged(DISPID_CMPROP_SELTEXT);
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::put_Language(BSTR strLang)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_CMPROP_LANGUAGE) == S_FALSE)
	{
		return S_FALSE;
	}
	LPTSTR psz = BSTR2TSTR(strLang);
	HRESULT hRes = CME2HRESULT(CM_SetLanguage(m_hWnd, psz));
	delete psz;

	OnChanged(DISPID_CMPROP_LANGUAGE);
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::get_Language(BSTR __RPC_FAR *pstrLang)
{
	CATCH_ERROR_NO_HWND();
	TCHAR *psz = new TCHAR[ CM_MAX_LANGUAGE_NAME + 1 ];
	*psz = _T('\0');
	HRESULT hRes = CME2HRESULT(CM_GetLanguage(m_hWnd, psz));
	USES_CONVERSION;
	*pstrLang = T2BSTR(psz);
	delete psz;
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::put_TabSize(long lTabSize)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_CMPROP_TABSIZE) == S_FALSE)
	{
		return S_FALSE;
	}
	HRESULT hRes = CME2HRESULT(CM_SetTabSize(m_hWnd, lTabSize));
	OnChanged(DISPID_CMPROP_TABSIZE);
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::get_TabSize(long __RPC_FAR *plTabSize)
{
	CATCH_ERROR_NO_HWND();
	*plTabSize = CM_GetTabSize(m_hWnd);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::put_TopIndex(long lTopIndex)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_CMPROP_TOPINDEX) == S_FALSE)
	{
		return S_FALSE;
	}
	HRESULT hRes = CME2HRESULT(CM_SetTopIndex(m_hWnd, 0, lTopIndex));
	OnChanged(DISPID_CMPROP_TOPINDEX);
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::get_TopIndex(long __RPC_FAR *plTopIndex)
{
	CATCH_ERROR_NO_HWND();
	*plTopIndex = CM_GetTopIndex(m_hWnd, 0);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::put_UndoLimit(long lLimit)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_CMPROP_UNDOLIMIT) == S_FALSE)
	{
		return S_FALSE;
	}
	HRESULT hRes = CME2HRESULT(CM_SetUndoLimit(m_hWnd, lLimit));
	OnChanged(DISPID_CMPROP_UNDOLIMIT);
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::get_UndoLimit(long __RPC_FAR *plLimit)
{
	CATCH_ERROR_NO_HWND();
	*plLimit = CM_GetUndoLimit(m_hWnd);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::put_CurrentView(long lView)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_CMPROP_CURRENTVIEW) == S_FALSE)
	{
		return S_FALSE;
	}
	HRESULT hr = CME2HRESULT(CM_SetCurrentView(m_hWnd, lView));
	OnChanged(DISPID_CMPROP_CURRENTVIEW);
	return hr;
}

HRESULT STDMETHODCALLTYPE CEditX::get_CurrentView(long __RPC_FAR *plView)
{
	CATCH_ERROR_NO_HWND();
	*plView = CM_GetCurrentView(m_hWnd);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::get_TextLength(long __RPC_FAR *plLen)
{
	CATCH_ERROR_NO_HWND();
	*plLen = CM_GetTextLength(m_hWnd, NULL, FALSE);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::get_TextLengthLogical(long __RPC_FAR *plLen)
{
	CATCH_ERROR_NO_HWND();
	*plLen = CM_GetTextLength(m_hWnd, NULL, TRUE);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::get_LineCount(long __RPC_FAR *plCount)
{
	CATCH_ERROR_NO_HWND();
	*plCount = CM_GetLineCount(m_hWnd);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::get_CurrentWord(BSTR __RPC_FAR *pstrWord)
{
	CATCH_ERROR_NO_HWND();
	TCHAR *psz = new TCHAR[ CM_GetWordLength(m_hWnd, NULL, FALSE) + 1 ];
	*psz = _T('\0');
	CM_GetWord(m_hWnd, psz, NULL);
	USES_CONVERSION;
	*pstrWord = T2BSTR(psz);
	delete psz;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::get_CurrentWordLength(long __RPC_FAR *plLen)
{
	CATCH_ERROR_NO_HWND();
	*plLen = CM_GetWordLength(m_hWnd, NULL, FALSE);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::get_CurrentWordLengthLogical(long __RPC_FAR *plLen)
{
	CATCH_ERROR_NO_HWND();
	*plLen = CM_GetWordLength(m_hWnd, NULL, TRUE);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::put_AutoIndentMode(cmAutoIndentMode lMode)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_CMPROP_AUTOINDENTMODE) == S_FALSE)
	{
		return S_FALSE;
	}
	HRESULT hRes = CME2HRESULT(CM_SetAutoIndentMode(m_hWnd, lMode));
	OnChanged(DISPID_CMPROP_AUTOINDENTMODE);
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::get_AutoIndentMode(cmAutoIndentMode __RPC_FAR *plMode)
{
	CATCH_ERROR_NO_HWND();
	*plMode = (cmAutoIndentMode) CM_GetAutoIndentMode(m_hWnd);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::get_ViewCount(long __RPC_FAR *plCount)
{
	CATCH_ERROR_NO_HWND();
	*plCount = CM_GetViewCount(m_hWnd);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::put_VSplitterPos(long lVal)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_CMPROP_VSPLITTERPOS) == S_FALSE)
	{
		return S_FALSE;
	}
	HRESULT hRes = CME2HRESULT(CM_SetSplitterPos(m_hWnd, FALSE, lVal));
	OnChanged(DISPID_CMPROP_VSPLITTERPOS);
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::get_VSplitterPos(long __RPC_FAR *plVal)
{
	CATCH_ERROR_NO_HWND();
	*plVal = CM_GetSplitterPos(m_hWnd, FALSE);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::put_HSplitterPos(long lVal)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_CMPROP_HSPLITTERPOS) == S_FALSE)
	{
		return S_FALSE;
	}
	HRESULT hRes = CME2HRESULT(CM_SetSplitterPos(m_hWnd, TRUE, lVal));
	OnChanged(DISPID_CMPROP_HSPLITTERPOS);
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::get_HSplitterPos(long __RPC_FAR *plVal)
{
	CATCH_ERROR_NO_HWND();
	*plVal = CM_GetSplitterPos(m_hWnd, TRUE);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::put_FindText(BSTR strVal)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_CMPROP_FINDTEXT) == S_FALSE)
	{
		return S_FALSE;
	}
	LPTSTR psz = BSTR2TSTR(strVal);
	HRESULT hRes = CME2HRESULT(CM_SetFindText(m_hWnd, psz));
	delete psz;

	OnChanged(DISPID_CMPROP_FINDTEXT);
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::get_FindText(BSTR __RPC_FAR *pstrVal)
{
	CATCH_ERROR_NO_HWND();
	TCHAR sz[ CM_MAX_FINDREPL_TEXT + 1 ];
	HRESULT hRes = CME2HRESULT(CM_GetFindText(m_hWnd, sz));
	USES_CONVERSION;
	*pstrVal = T2BSTR(sz);
	return hRes;
}


HRESULT STDMETHODCALLTYPE CEditX::put_ReplText(BSTR strVal)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_CMPROP_REPLACETEXT) == S_FALSE)
	{
		return S_FALSE;
	}
	LPTSTR psz = BSTR2TSTR(strVal);
	HRESULT hRes = CME2HRESULT(CM_SetReplaceText(m_hWnd, psz));
	delete psz;

	OnChanged(DISPID_CMPROP_REPLACETEXT);
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::get_ReplText(BSTR __RPC_FAR *pstrVal)
{
	CATCH_ERROR_NO_HWND();
	TCHAR sz[ CM_MAX_FINDREPL_TEXT + 1 ];
	HRESULT hRes = CME2HRESULT(CM_GetReplaceText(m_hWnd, sz));
	USES_CONVERSION;
	*pstrVal = T2BSTR(sz);
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::get_Window(long* phWnd)
{
	CATCH_ERROR_NO_HWND();
	*phWnd = (long) m_hWnd;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::get_hWnd(long* phWnd)
{
	CATCH_ERROR_NO_HWND();
	*phWnd = (long) m_hWnd;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::put_Enabled(VARIANT_BOOL vBool)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_ENABLED) == S_FALSE)
	{
		return S_FALSE;
	}
	::EnableWindow(m_hWnd, vBool != 0);
	OnChanged(DISPID_ENABLED);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::get_Enabled(VARIANT_BOOL __RPC_FAR *pBool)
{
	CATCH_ERROR_NO_HWND();
	*pBool = (VARIANT_BOOL) (::IsWindowEnabled(m_hWnd) ? -1 : 0);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::putref_Font(IFontDisp* pFont)
{
	CATCH_ERROR_NO_HWND();
	return put_Font(pFont);
}

HRESULT STDMETHODCALLTYPE CEditX::put_Font(IFontDisp* pFont)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_FONT) == S_FALSE)
	{
		return S_FALSE;
	}

	HRESULT hRes = S_FALSE;
	if (pFont)
	{
		IFont *pIFont;
		if (SUCCEEDED(pFont->QueryInterface(IID_IFont, (void **) &pIFont)))
		{
			CY cy;
			pIFont->get_Size(&cy);
			BSTR bstr;
			pIFont->get_Name(&bstr);
			LPTSTR pszFaceName = BSTR2TSTR(bstr);
			HDC hDC = ::GetDC(NULL);
			BOOL bItalic, bUnderline, bStrikeOut;
			pIFont->get_Italic(&bItalic);
			pIFont->get_Underline(&bUnderline);
			pIFont->get_Strikethrough(&bStrikeOut);
			short sWeight, sCharSet;
			pIFont->get_Weight(&sWeight);
			pIFont->get_Charset(&sCharSet);
			double cyFont = (((double)cy.Lo / 10000.0) * (double)GetDeviceCaps(hDC, LOGPIXELSY)) / 72.0;
			LOGFONT lf = { -abs((LONG)(cyFont + 0.5)),
			               0, 0, 0, 
			               sWeight, 
			               bItalic, 
			               bUnderline, 
			               bStrikeOut, 
			               (BYTE) sCharSet, 
			               0, 0, 0, 0, 
			               _T("") };
			_tcsncpy(lf.lfFaceName, pszFaceName, ARRAY_SIZE(lf.lfFaceName) - 1);
			NormalizeLOGFONT(lf);

			HFONT hFont = CreateFontIndirect(&lf);
			::ReleaseDC(NULL, hDC);
			::SendMessage(m_hWnd, WM_SETFONT, (WPARAM) hFont, 0);
			if (hFont == (HFONT) ::SendMessage(m_hWnd, WM_GETFONT, 0, 0))
			{
				// success -- make sure CodeMax takes ownership of hFont
				CM_SetFontOwnership(m_hWnd, TRUE);
				OnChanged(DISPID_FONT);
				hRes = S_OK;
			}
			else
			{
				DeleteObject(hFont);
				hRes = S_FALSE;
			}
			delete pszFaceName;
			pIFont->Release();
		}
	}
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::get_Font(IFontDisp** ppFont)
{
	CATCH_ERROR_NO_HWND();
	if (!m_pFont)
		m_pFont = new CFontDisp(this);
	return m_pFont->GetFontDispatch(ppFont);
}

HRESULT STDMETHODCALLTYPE CEditX::GetWord(IPosition __RPC_FAR *pPos, BSTR __RPC_FAR *pstrWord)
{
	CATCH_ERROR_NO_HWND();
	CM_POSITION *pcmPos = pPos ? &(((CPositionX *) pPos)->m_cmPos) : NULL;
	TCHAR *psz = new TCHAR[ CM_GetWordLength(m_hWnd, pcmPos, FALSE) + 1 ];
	*psz = _T('\0');
	CM_GetWord(m_hWnd, psz, pcmPos);
	USES_CONVERSION;
	*pstrWord = T2BSTR(psz);
	delete psz;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::GetWordLength(IPosition __RPC_FAR *pPos, VARIANT_BOOL bLogical, long __RPC_FAR *plLen)
{
	CATCH_ERROR_NO_HWND();
	CM_POSITION *pcmPos = pPos ? &(((CPositionX *) pPos)->m_cmPos) : NULL;
	*plLen = CM_GetWordLength(m_hWnd, pcmPos, bLogical == TRUE);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::GetTopIndex(long lView, long __RPC_FAR *plLine)
{
	CATCH_ERROR_NO_HWND();
	*plLine = CM_GetTopIndex(m_hWnd, lView);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::SetTopIndex(long lView, long lLine)
{
	CATCH_ERROR_NO_HWND();
	return 	CME2HRESULT(CM_SetTopIndex(m_hWnd, lView, lLine));
}

HRESULT STDMETHODCALLTYPE CEditX::GetSel(VARIANT_BOOL bNormalized, IRange __RPC_FAR *__RPC_FAR *ppRange)
{
	CATCH_ERROR_NO_HWND();
	if (ppRange)
	{
		CM_RANGE cmRange;
		if (CME_SUCCESS == CM_GetSel(m_hWnd, &cmRange, bNormalized != 0))
		{
			// release any existing object
			if (*ppRange)
			{
		    	(*ppRange)->Release();
		    }
		    // return a new object
			if (SUCCEEDED(CoCreateInstance(CLSID_Range, NULL, CLSCTX_INPROC_SERVER, IID_IRange, (void **) ppRange)))
			{
				CM_RANGE *pcmRange = &(((CRangeX *) *ppRange)->m_cmRange);
				*pcmRange = cmRange;
				return S_OK;
			}
		}
	}
	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE CEditX::SetSel(IRange __RPC_FAR *pRange, VARIANT_BOOL bMakeVisible)
{
	CATCH_ERROR_NO_HWND();
	if (pRange)
	{
		CM_RANGE *pcmRange = &(((CRangeX *) pRange)->m_cmRange);
		return CME2HRESULT(CM_SetSel(m_hWnd, pcmRange, bMakeVisible != 0));
	}
	
	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE CEditX::GetSelFromPoint(long lxClientPos, long lyClientPos, IPosition __RPC_FAR *__RPC_FAR *ppPos)
{
	CATCH_ERROR_NO_HWND();
	if (ppPos)
	{
		CM_POSITION cmPos;
		if (CME_SUCCESS == CM_GetSelFromPoint(m_hWnd, lxClientPos, lyClientPos, &cmPos))
		{
			// release any existing object
			if (*ppPos)
			{
		    	(*ppPos)->Release();
		    }
		    // return a new object
			if (SUCCEEDED(CoCreateInstance(CLSID_Position, NULL, CLSCTX_INPROC_SERVER, IID_IPosition, (void **) ppPos)))
			{
				CM_POSITION *pcmPos = &(((CPositionX *) *ppPos)->m_cmPos);
				*pcmPos = cmPos;
				return S_OK;
			}
		}
	}
	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE CEditX::GetVisibleLineCount(long lView, VARIANT_BOOL bFullyVisible, long __RPC_FAR *plCount)
{
	CATCH_ERROR_NO_HWND();
	*plCount = CM_GetVisibleLineCount(m_hWnd, lView, bFullyVisible != 0);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::GetLine(long lLine, BSTR __RPC_FAR *pstrLine)
{
	CATCH_ERROR_NO_HWND();
	TCHAR *psz = new TCHAR[ CM_GetLineLength(m_hWnd, lLine, FALSE) + 1 ];
	*psz = _T('\0');
	CM_GetLine(m_hWnd, lLine, psz);
	USES_CONVERSION;
	*pstrLine = T2BSTR(psz);
	delete psz;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::GetLineLength(long lLine, long __RPC_FAR *plLen)
{
	CATCH_ERROR_NO_HWND();
	*plLen = CM_GetLineLength(m_hWnd, lLine, FALSE);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::SetColor(cmColorItem lColorItem, OLE_COLOR crColor)
{
	CATCH_ERROR_NO_HWND();
	CM_COLORS cmColors;
	CM_GetColors(m_hWnd, &cmColors);
	
	switch(lColorItem)
	{
		case cmClrWindow:				cmColors.crWindow = (COLORREF) crColor;				break;
		case cmClrLeftMargin:			cmColors.crLeftMargin = (COLORREF) crColor;			break;
		case cmClrBookmark:				cmColors.crBookmark = (COLORREF) crColor;				break;
		case cmClrBookmarkBk:			cmColors.crBookmarkBk = (COLORREF) crColor;			break;
		case cmClrText:					cmColors.crText = (COLORREF) crColor;					break;
		case cmClrTextBk:				cmColors.crTextBk = (COLORREF) crColor;				break;
		case cmClrNumber:				cmColors.crNumber = (COLORREF) crColor;				break;
		case cmClrNumberBk:				cmColors.crNumberBk = (COLORREF) crColor;				break;
		case cmClrKeyword:				cmColors.crKeyword = (COLORREF) crColor;				break;
		case cmClrKeywordBk:			cmColors.crKeywordBk = (COLORREF) crColor;			break;
		case cmClrOperator:				cmColors.crOperator = (COLORREF) crColor;				break;
		case cmClrOperatorBk:			cmColors.crOperatorBk = (COLORREF) crColor;			break;
		case cmClrScopeKeyword:			cmColors.crScopeKeyword = (COLORREF) crColor;			break;
		case cmClrScopeKeywordBk:		cmColors.crScopeKeywordBk = (COLORREF) crColor;		break;
		case cmClrComment:				cmColors.crComment = (COLORREF) crColor;				break;
		case cmClrCommentBk:			cmColors.crCommentBk = (COLORREF) crColor;			break;
		case cmClrString:				cmColors.crString = (COLORREF) crColor;				break;
		case cmClrStringBk:				cmColors.crStringBk = (COLORREF) crColor;				break;
		case cmClrTagText:				cmColors.crTagText = (COLORREF) crColor;				break;
		case cmClrTagTextBk:			cmColors.crTagTextBk = (COLORREF) crColor;			break;
		case cmClrTagEntity:			cmColors.crTagEntity = (COLORREF) crColor;			break;
		case cmClrTagEntityBk:			cmColors.crTagEntityBk = (COLORREF) crColor;			break;
		case cmClrTagElementName:		cmColors.crTagElementName = (COLORREF) crColor;		break;
		case cmClrTagElementNameBk:		cmColors.crTagElementNameBk = (COLORREF) crColor;		break;
		case cmClrTagAttributeName:		cmColors.crTagAttributeName = (COLORREF) crColor;		break;
		case cmClrTagAttributeNameBk:	cmColors.crTagAttributeNameBk = (COLORREF) crColor;	break;
		case cmClrLineNumber:			cmColors.crLineNumber = (COLORREF) crColor;			break;
		case cmClrLineNumberBk:			cmColors.crLineNumberBk = (COLORREF) crColor;			break;
		case cmClrHDividerLines:		cmColors.crHDividerLines = (COLORREF) crColor;		break;
		case cmClrVDividerLines:		cmColors.crVDividerLines = (COLORREF) crColor;		break;
		case cmClrHighlightedLine:		cmColors.crHighlightedLine = (COLORREF) crColor;		break;
		default:
		{
			return S_FALSE;
		}
	}
	return CME2HRESULT(CM_SetColors(m_hWnd, &cmColors));
}

HRESULT STDMETHODCALLTYPE CEditX::GetColor(cmColorItem lColorItem, OLE_COLOR __RPC_FAR *pcrColor)
{
	CATCH_ERROR_NO_HWND();
	CM_COLORS cmColors;
	CM_GetColors(m_hWnd, &cmColors);
	
	switch(lColorItem)
	{
		case cmClrWindow:				*pcrColor = (OLE_COLOR) cmColors.crWindow;				break;
		case cmClrLeftMargin:			*pcrColor = (OLE_COLOR) cmColors.crLeftMargin;			break;
		case cmClrBookmark:				*pcrColor = (OLE_COLOR) cmColors.crBookmark;				break;
		case cmClrBookmarkBk:			*pcrColor = (OLE_COLOR) cmColors.crBookmarkBk;			break;
		case cmClrText:					*pcrColor = (OLE_COLOR) cmColors.crText;					break;
		case cmClrTextBk:				*pcrColor = (OLE_COLOR) cmColors.crTextBk;				break;
		case cmClrNumber:				*pcrColor = (OLE_COLOR) cmColors.crNumber;				break;
		case cmClrNumberBk:				*pcrColor = (OLE_COLOR) cmColors.crNumberBk;				break;
		case cmClrKeyword:				*pcrColor = (OLE_COLOR) cmColors.crKeyword;				break;
		case cmClrKeywordBk:			*pcrColor = (OLE_COLOR) cmColors.crKeywordBk;				break;
		case cmClrOperator:				*pcrColor = (OLE_COLOR) cmColors.crOperator;				break;
		case cmClrOperatorBk:			*pcrColor = (OLE_COLOR) cmColors.crOperatorBk;			break;
		case cmClrScopeKeyword:			*pcrColor = (OLE_COLOR) cmColors.crScopeKeyword;			break;
		case cmClrScopeKeywordBk:		*pcrColor = (OLE_COLOR) cmColors.crScopeKeywordBk;		break;
		case cmClrComment:				*pcrColor = (OLE_COLOR) cmColors.crComment;				break;
		case cmClrCommentBk:			*pcrColor = (OLE_COLOR) cmColors.crCommentBk;				break;
		case cmClrString:				*pcrColor = (OLE_COLOR) cmColors.crString;				break;
		case cmClrStringBk:				*pcrColor = (OLE_COLOR) cmColors.crStringBk;				break;
		case cmClrTagText:				*pcrColor = (OLE_COLOR) cmColors.crTagText;				break;
		case cmClrTagTextBk:			*pcrColor = (OLE_COLOR) cmColors.crTagTextBk;				break;
		case cmClrTagEntity:			*pcrColor = (OLE_COLOR) cmColors.crTagEntity;				break;
		case cmClrTagEntityBk:			*pcrColor = (OLE_COLOR) cmColors.crTagEntityBk;			break;
		case cmClrTagElementName:		*pcrColor = (OLE_COLOR) cmColors.crTagElementName;		break;
		case cmClrTagElementNameBk:		*pcrColor = (OLE_COLOR) cmColors.crTagElementNameBk;		break;
		case cmClrTagAttributeName:		*pcrColor = (OLE_COLOR) cmColors.crTagAttributeName;		break;
		case cmClrTagAttributeNameBk:	*pcrColor = (OLE_COLOR) cmColors.crTagAttributeNameBk;	break;
		case cmClrLineNumber:			*pcrColor = (OLE_COLOR) cmColors.crLineNumber;			break;
		case cmClrLineNumberBk:			*pcrColor = (OLE_COLOR) cmColors.crLineNumberBk;			break;
		case cmClrHDividerLines:		*pcrColor = (OLE_COLOR) cmColors.crHDividerLines;			break;
		case cmClrVDividerLines:		*pcrColor = (OLE_COLOR) cmColors.crVDividerLines;			break;
		case cmClrHighlightedLine:		*pcrColor = (OLE_COLOR) cmColors.crHighlightedLine;		break;

		default:
		{
			return S_FALSE;
		}
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::OpenFile(BSTR strFileName)
{
	CATCH_ERROR_NO_HWND();
	LPTSTR psz = BSTR2TSTR(strFileName);
	HRESULT hRes = CME2HRESULT(CM_OpenFile(m_hWnd, psz));
	delete psz;
	if (FAILED(hRes))
	{
		RaiseLastErrorException();
	}
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::SaveFile(BSTR strFileName, VARIANT_BOOL bClearUndo)
{
	CATCH_ERROR_NO_HWND();
	LPTSTR psz = BSTR2TSTR(strFileName);
	HRESULT hRes = CME2HRESULT(CM_SaveFile(m_hWnd, psz, bClearUndo != 0));
	delete psz;
	if (FAILED(hRes))
	{
		RaiseLastErrorException();
	}
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::InsertFile(BSTR strFileName, IPosition __RPC_FAR *pInsertPos)
{
	CATCH_ERROR_NO_HWND();
	LPTSTR psz = BSTR2TSTR(strFileName);
	CM_POSITION *pcmPos = pInsertPos ? &(((CPositionX *) pInsertPos)->m_cmPos) : NULL;
	HRESULT hRes = CME2HRESULT(CM_InsertFile(m_hWnd, psz, pcmPos));
	delete psz;
	if (FAILED(hRes))
	{
		RaiseLastErrorException();
	}
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::InsertText(BSTR strText, IPosition __RPC_FAR *pInsertPos)
{
	CATCH_ERROR_NO_HWND();
	LPTSTR psz = BSTR2TSTR(strText);
	CM_POSITION *pcmPos = pInsertPos ? &(((CPositionX *) pInsertPos)->m_cmPos) : NULL;
	HRESULT hRes = CME2HRESULT(CM_InsertText(m_hWnd, psz, pcmPos));
	delete psz;
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::ReplaceText(BSTR strText, IRange __RPC_FAR *pRange)
{
	CATCH_ERROR_NO_HWND();
	LPTSTR psz = BSTR2TSTR(strText);
	CM_RANGE *pcmRange = pRange ? &(((CRangeX *) pRange)->m_cmRange) : NULL;
	HRESULT hRes = CME2HRESULT(CM_ReplaceText(m_hWnd, psz, pcmRange));
	delete psz;
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::HitTest(long lxClientPos, long lyClientPos, long __RPC_FAR *plView, cmHitTestCode __RPC_FAR *plHitTestCode)
{
	CATCH_ERROR_NO_HWND();
	DWORD dwRetVal = CM_HitTest(m_hWnd, lxClientPos, lyClientPos);
	*plView = (long) LOWORD(dwRetVal);
	*plHitTestCode = (cmHitTestCode) HIWORD(dwRetVal);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::AddText(BSTR strText)
{
	CATCH_ERROR_NO_HWND();
	LPTSTR psz = BSTR2TSTR(strText);
	HRESULT hRes = CME2HRESULT(CM_AddText(m_hWnd, psz));
	delete psz;
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::DeleteLine(long lLine)
{
	CATCH_ERROR_NO_HWND();
	return CME2HRESULT(CM_DeleteLine(m_hWnd, lLine));
}

HRESULT STDMETHODCALLTYPE CEditX::InsertLine(long lLine, BSTR strLine)
{
	CATCH_ERROR_NO_HWND();
	LPTSTR psz = BSTR2TSTR(strLine);
	HRESULT hRes = CME2HRESULT(CM_InsertLine(m_hWnd, lLine, psz));
	delete psz;
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::SelectLine(long lLine, VARIANT_BOOL bMakeVisible)
{
	CATCH_ERROR_NO_HWND();
	return CME2HRESULT(CM_SelectLine(m_hWnd, lLine, bMakeVisible != 0));
}

HRESULT STDMETHODCALLTYPE CEditX::DeleteSel()
{
	CATCH_ERROR_NO_HWND();
	return CME2HRESULT(CM_DeleteSel(m_hWnd));
}

HRESULT STDMETHODCALLTYPE CEditX::ReplaceSel(BSTR strText)
{
	CATCH_ERROR_NO_HWND();
	LPTSTR psz = BSTR2TSTR(strText);
	HRESULT hRes = CME2HRESULT(CM_ReplaceSel(m_hWnd, psz));
	delete psz;
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::ExecuteCmd(cmCommand lCommand, VARIANT vData)
{
	CATCH_ERROR_NO_HWND();
	DWORD dwData = 0;
	LPTSTR pszData = NULL;
	CComVariant vNew(vData);
	switch (lCommand)
	{
		case CMD_SETFINDTEXT:
		case CMD_SETREPLACETEXT:
		{
			if (SUCCEEDED(vNew.ChangeType(VT_BSTR)))
			{
				pszData = BSTR2TSTR(vData.bstrVal);
				dwData = (DWORD) pszData;
			}
			break;
		}
		default:
		{
			if (SUCCEEDED(vNew.ChangeType(VT_I4)))
			{
				dwData = vNew.lVal;
			}
			break;
		}
	}
	HRESULT hRes = CME2HRESULT(CM_ExecuteCmd(m_hWnd, lCommand, dwData));
	delete pszData;
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::CanUndo(VARIANT_BOOL __RPC_FAR *pbAnswer)
{
	CATCH_ERROR_NO_HWND();
	*pbAnswer = (CM_CanUndo(m_hWnd) != 0) ? (VARIANT_BOOL) -1 : 0;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::CanRedo(VARIANT_BOOL __RPC_FAR *pbAnswer)
{
	CATCH_ERROR_NO_HWND();
	*pbAnswer = (CM_CanRedo(m_hWnd) != 0) ? (VARIANT_BOOL) -1 : 0;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::CanCut(VARIANT_BOOL __RPC_FAR *pbAnswer)
{
	CATCH_ERROR_NO_HWND();
	*pbAnswer = (CM_CanCut(m_hWnd) != 0) ? (VARIANT_BOOL) -1 : 0;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::CanCopy(VARIANT_BOOL __RPC_FAR *pbAnswer)
{
	CATCH_ERROR_NO_HWND();
	*pbAnswer = (CM_CanCopy(m_hWnd) != 0) ? (VARIANT_BOOL) -1 : 0;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::CanPaste(VARIANT_BOOL __RPC_FAR *pbAnswer)
{
	CATCH_ERROR_NO_HWND();
	*pbAnswer = (CM_CanPaste(m_hWnd) != 0) ? (VARIANT_BOOL) -1 : 0;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::Undo()
{
	CATCH_ERROR_NO_HWND();
	return CME2HRESULT(CM_Undo(m_hWnd));
}

HRESULT STDMETHODCALLTYPE CEditX::Redo()
{
	CATCH_ERROR_NO_HWND();
	return CME2HRESULT(CM_Redo(m_hWnd));
}

HRESULT STDMETHODCALLTYPE CEditX::Cut()
{
	CATCH_ERROR_NO_HWND();
	return CME2HRESULT(CM_Cut(m_hWnd));
}

HRESULT STDMETHODCALLTYPE CEditX::Copy()
{
	CATCH_ERROR_NO_HWND();
	return CME2HRESULT(CM_Copy(m_hWnd));
}

HRESULT STDMETHODCALLTYPE CEditX::Paste()
{
	CATCH_ERROR_NO_HWND();
	return CME2HRESULT(CM_Paste(m_hWnd));
}

HRESULT STDMETHODCALLTYPE CEditX::ClearUndoBuffer()
{
	CATCH_ERROR_NO_HWND();
	return CME2HRESULT(CM_ClearUndoBuffer(m_hWnd));
}

HRESULT STDMETHODCALLTYPE CEditX::IsRecordingMacro(VARIANT_BOOL __RPC_FAR *pbAnswer)
{
	CATCH_ERROR_NO_HWND();
	*pbAnswer = (CM_IsRecordingMacro(m_hWnd) != 0) ? (VARIANT_BOOL) -1 : 0;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::IsPlayingMacro(VARIANT_BOOL __RPC_FAR *pbAnswer)
{
	CATCH_ERROR_NO_HWND();
	*pbAnswer = (CM_IsPlayingMacro(m_hWnd) != 0) ? (VARIANT_BOOL) -1 : 0;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::SetDlgParent(long lhWnd)
{
	CATCH_ERROR_NO_HWND();
	return CME2HRESULT(CM_SetDlgParent(m_hWnd, lhWnd));
}

HRESULT STDMETHODCALLTYPE CEditX::SetFontStyle(cmFontStyleItem lFontStyleItem, cmFontStyle byFontStyle)
{
	CATCH_ERROR_NO_HWND();
	CM_FONTSTYLES cmFontStyles;
	CM_GetFontStyles(m_hWnd, &cmFontStyles);
	
	switch(lFontStyleItem)
	{
		case cmStyText:					cmFontStyles.byText = byFontStyle;					break;
		case cmStyNumber:				cmFontStyles.byNumber = byFontStyle;				break;
		case cmStyKeyword:				cmFontStyles.byKeyword = byFontStyle;				break;
		case cmStyOperator:				cmFontStyles.byOperator = byFontStyle;				break;
		case cmStyScopeKeyword:			cmFontStyles.byScopeKeyword = byFontStyle;			break;
		case cmStyComment:				cmFontStyles.byComment = byFontStyle;				break;
		case cmStyString:				cmFontStyles.byString = byFontStyle;				break;
		case cmStyTagText:				cmFontStyles.byTagText = byFontStyle;				break;
		case cmStyTagEntity:			cmFontStyles.byTagEntity = byFontStyle;				break;
		case cmStyTagElementName:		cmFontStyles.byTagElementName = byFontStyle;		break;
		case cmStyTagAttributeName:		cmFontStyles.byTagAttributeName = byFontStyle;		break;
		case cmStyLineNumber:			cmFontStyles.byLineNumber = byFontStyle;			break;
		default:
		{
			return S_FALSE;
		}
	}
	return CME2HRESULT(CM_SetFontStyles(m_hWnd, &cmFontStyles));
}

HRESULT STDMETHODCALLTYPE CEditX::GetFontStyle(cmFontStyleItem lFontStyleItem, cmFontStyle __RPC_FAR *pbyFontStyle)
{
	CATCH_ERROR_NO_HWND();
	CM_FONTSTYLES cmFontStyles;
	CM_GetFontStyles(m_hWnd, &cmFontStyles);
	
	switch(lFontStyleItem)
	{
		case cmStyText:					*pbyFontStyle = (cmFontStyle) cmFontStyles.byText;				break;
		case cmStyNumber:				*pbyFontStyle = (cmFontStyle) cmFontStyles.byNumber;				break;
		case cmStyKeyword:				*pbyFontStyle = (cmFontStyle) cmFontStyles.byKeyword;				break;
		case cmStyOperator:				*pbyFontStyle = (cmFontStyle) cmFontStyles.byOperator;			break;
		case cmStyScopeKeyword:			*pbyFontStyle = (cmFontStyle) cmFontStyles.byScopeKeyword;		break;
		case cmStyComment:				*pbyFontStyle = (cmFontStyle) cmFontStyles.byComment;				break;
		case cmStyString:				*pbyFontStyle = (cmFontStyle) cmFontStyles.byString;				break;
		case cmStyLineNumber:			*pbyFontStyle = (cmFontStyle) cmFontStyles.byLineNumber;			break;
		case cmStyTagText:				*pbyFontStyle = (cmFontStyle) cmFontStyles.byTagText;				break;
		case cmStyTagEntity:			*pbyFontStyle = (cmFontStyle) cmFontStyles.byTagEntity;			break;
		case cmStyTagElementName:		*pbyFontStyle = (cmFontStyle) cmFontStyles.byTagElementName;		break;
		case cmStyTagAttributeName:		*pbyFontStyle = (cmFontStyle) cmFontStyles.byTagAttributeName;	break;
		default:
		{
			return S_FALSE;
		}
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::SetItemData(long lLine, long lData)
{
	CATCH_ERROR_NO_HWND();
	return CME2HRESULT(CM_SetItemData(m_hWnd, lLine, lData));
}

HRESULT STDMETHODCALLTYPE CEditX::GetItemData(long lLine, long __RPC_FAR *plData)
{
	CATCH_ERROR_NO_HWND();
	*plData = CM_GetItemData(m_hWnd, lLine);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::SetLineStyle(long lLine, long lStyle)
{
	CATCH_ERROR_NO_HWND();
	return CME2HRESULT(CM_SetLineStyle(m_hWnd, lLine, lStyle));
}

HRESULT STDMETHODCALLTYPE CEditX::GetLineStyle(long lLine, long __RPC_FAR *plStyle)
{
	CATCH_ERROR_NO_HWND();
	*plStyle = CM_GetLineStyle(m_hWnd, lLine);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::SetBookmark(long lLine, VARIANT_BOOL bVal)
{
	CATCH_ERROR_NO_HWND();
	return CME2HRESULT(CM_SetBookmark(m_hWnd, lLine, bVal != 0));
}

HRESULT STDMETHODCALLTYPE CEditX::GetBookmark(long lLine, VARIANT_BOOL __RPC_FAR *pbVal)
{
	CATCH_ERROR_NO_HWND();
	*pbVal = (VARIANT_BOOL) (CM_GetBookmark(m_hWnd, lLine) ? -1 : 0);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::SetAllBookmarks(SAFEARRAY __RPC_FAR * __RPC_FAR *ppLines)
{
	CATCH_ERROR_NO_HWND();
	LPDWORD pDWords;
	HRESULT hr = SafeArrayAccessData(*ppLines, (void **) &pDWords);
	if (SUCCEEDED(hr))
	{
		LONG lLowerBound, lUpperBound;
		if (SUCCEEDED(hr = SafeArrayGetLBound(*ppLines, 1, &lLowerBound)) &&
		     SUCCEEDED(hr = SafeArrayGetUBound(*ppLines, 1, &lUpperBound)))
			{
			hr = CME2HRESULT(CM_SetAllBookmarks(m_hWnd, lUpperBound - lLowerBound + 1, pDWords));
			}
		SafeArrayUnaccessData(*ppLines);
	}
	return hr;
}

HRESULT STDMETHODCALLTYPE CEditX::GetAllBookmarks(SAFEARRAY __RPC_FAR * __RPC_FAR *ppLines)
{
	CATCH_ERROR_NO_HWND();
	SAFEARRAYBOUND rgsabound[1];
	rgsabound[0].lLbound = 0;
	rgsabound[0].cElements = CM_GetAllBookmarks(m_hWnd, NULL);
	HRESULT hr = E_FAIL;
	*ppLines = SafeArrayCreate(VT_I4, 1, rgsabound);
	if(*ppLines)
	{
		hr = SafeArrayAllocData(*ppLines);
		if (SUCCEEDED(hr))
		{
			LPDWORD pDWords;
			hr = SafeArrayAccessData( *ppLines, (void **) &pDWords);
			if (SUCCEEDED(hr))
			{
				CM_GetAllBookmarks(m_hWnd, pDWords);
				SafeArrayUnaccessData(*ppLines);
			}
			else
			{
				SafeArrayDestroy(*ppLines);
			}
		}
	}

	if(FAILED(hr))
	{
		*ppLines = NULL;
	}
	return hr;
}

HRESULT STDMETHODCALLTYPE CEditX::put_LineNumbering(VARIANT_BOOL bVal)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_CMPROP_LINENUMBERS) == S_FALSE)
	{
		return S_FALSE;
	}
	CM_LINENUMBERING cmLineNum;
	CM_GetLineNumbering(m_hWnd, &cmLineNum);
	cmLineNum.bEnabled = (bVal != 0);
	HRESULT hRes = CME2HRESULT(CM_SetLineNumbering(m_hWnd, &cmLineNum));
	OnChanged(DISPID_CMPROP_LINENUMBERS);
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::get_LineNumbering(VARIANT_BOOL __RPC_FAR *pbVal)
{
	CATCH_ERROR_NO_HWND();
	CM_LINENUMBERING cmLineNum;
	CM_GetLineNumbering(m_hWnd, &cmLineNum);
	*pbVal = (VARIANT_BOOL) (cmLineNum.bEnabled ? -1 : 0);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::put_LineNumberStart(long lVal)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_CMPROP_LINENUMBERSTART) == S_FALSE)
	{
		return S_FALSE;
	}
	CM_LINENUMBERING cmLineNum;
	CM_GetLineNumbering(m_hWnd, &cmLineNum);
	cmLineNum.nStartAt = lVal;
	HRESULT hRes = CME2HRESULT(CM_SetLineNumbering(m_hWnd, &cmLineNum));
	OnChanged(DISPID_CMPROP_LINENUMBERSTART);
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::get_LineNumberStart(long __RPC_FAR *plVal)
{
	CATCH_ERROR_NO_HWND();
	CM_LINENUMBERING cmLineNum;
	CM_GetLineNumbering(m_hWnd, &cmLineNum);
	*plVal = cmLineNum.nStartAt;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::put_LineNumberStyle(cmLineNumStyle lStyle)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_CMPROP_LINENUMBERSTYLE) == S_FALSE)
	{
		return S_FALSE;
	}
	CM_LINENUMBERING cmLineNum;
	CM_GetLineNumbering(m_hWnd, &cmLineNum);
	cmLineNum.dwStyle = (DWORD) lStyle;
	HRESULT hRes = CME2HRESULT(CM_SetLineNumbering(m_hWnd, &cmLineNum));
	OnChanged(DISPID_CMPROP_LINENUMBERSTYLE);
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::get_LineNumberStyle(cmLineNumStyle __RPC_FAR *plStyle)
{
	CATCH_ERROR_NO_HWND();
	CM_LINENUMBERING cmLineNum;
	CM_GetLineNumbering(m_hWnd, &cmLineNum);
	*plStyle = (cmLineNumStyle) cmLineNum.dwStyle;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::PosFromChar(IPosition __RPC_FAR *pPos, IRect __RPC_FAR *__RPC_FAR *pprc)
{
	CATCH_ERROR_NO_HWND();
	CM_POSITION *pcmPos = &(((CPositionX *) pPos)->m_cmPos);
	if (pprc)
	{
		RECT rc;
		if (CME_SUCCESS == CM_PosFromChar(m_hWnd, pcmPos, &rc))
		{
			// release any existing object
			if (*pprc)
			{
		    	(*pprc)->Release();
		    }
		    // return a new object
			if (SUCCEEDED(CoCreateInstance(CLSID_Rect, NULL, CLSCTX_INPROC_SERVER, IID_IRect, (void **) pprc)))
			{
				RECT *prc = &(((CRectX *) *pprc)->m_rc);
				*prc = rc;
				return S_OK;
			}
		}
	}

	return S_FALSE;
}

HRESULT STDMETHODCALLTYPE CEditX::get_HighlightedLine(long __RPC_FAR *plLine)
{
	CATCH_ERROR_NO_HWND();
	*plLine = CM_GetHighlightedLine(m_hWnd);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::put_HighlightedLine(long lLine)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_CMPROP_HIGHLIGHTEDLINE) == S_FALSE)
	{
		return S_FALSE;
	}
	return CME2HRESULT(CM_SetHighlightedLine(m_hWnd, lLine));
}

HRESULT STDMETHODCALLTYPE CEditX::SetDivider(long lLine, VARIANT_BOOL bVal)
{
	CATCH_ERROR_NO_HWND();
	return CME2HRESULT(CM_SetDivider(m_hWnd, lLine, bVal != 0));
}

HRESULT STDMETHODCALLTYPE CEditX::GetDivider(long lLine, VARIANT_BOOL __RPC_FAR *pbVal)
{
	CATCH_ERROR_NO_HWND();
	*pbVal = (VARIANT_BOOL) (CM_GetDivider(m_hWnd, lLine) ? -1 : 0);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::SetMarginImages(long lLine, BYTE byImages)
{
	CATCH_ERROR_NO_HWND();
	return CME2HRESULT(CM_SetMarginImages(m_hWnd, lLine, byImages));
}

HRESULT STDMETHODCALLTYPE CEditX::GetMarginImages(long lLine, BYTE __RPC_FAR *pbyImages)
{
	CATCH_ERROR_NO_HWND();
	*pbyImages = CM_GetMarginImages(m_hWnd, lLine);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::put_hImageList(OLE_HANDLE hImageList)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_CMPROP_HIMAGELIST) == S_FALSE)
	{
		return S_FALSE;
	}
	HRESULT hRes = CME2HRESULT(CM_SetImageList(m_hWnd, hImageList));
	OnChanged(DISPID_CMPROP_HIMAGELIST);
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::get_hImageList(OLE_HANDLE __RPC_FAR *phImageList)
{
	CATCH_ERROR_NO_HWND();
	*phImageList = (OLE_HANDLE) CM_GetImageList(m_hWnd);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::get_ImageList(IDispatch __RPC_FAR *__RPC_FAR *ppImageList)
{
	CATCH_ERROR_NO_HWND();
	*ppImageList = m_spImageList;
	if (m_spImageList)
		m_spImageList.p->AddRef();
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::put_ImageList(IDispatch __RPC_FAR *pImageList)
{
	if (FireOnRequestEdit(DISPID_CMPROP_IMAGELIST) == S_FALSE)
	{
		return S_FALSE;
	}
	if (!pImageList)
	{
		m_spImageList = NULL;
		// switch to the stock images
		CM_SetImageList(m_hWnd, NULL);
		return S_OK;
	}
	else
	{
		CComDispatchDriver pDisp(pImageList);

		VARIANT vRet;
		HRESULT hr = pDisp.GetPropertyByName(L"hImageList", &vRet);

		if (SUCCEEDED(hr))
		{
			if (m_hImageList)
			{
				ImageList_Destroy(m_hImageList);
				m_hImageList = NULL;
			}

			if (vRet.vt == VT_I4)
			{
#if 0
				// switch to the new images
				// save a new copy of the imagelist.  We cannot make any
				// assumptions about the lifetime of the one we just got.
				IMAGEINFO ii;
				if (ImageList_GetImageInfo((HIMAGELIST) vRet.lVal, 0, &ii))
					{
					int nImages = ImageList_GetImageCount((HIMAGELIST)vRet.lVal);
					HDC hDCScreen = ::GetDC(::GetDesktopWindow());
					SetTextColor(hDCScreen, 0);
					TextOut(hDCScreen, 300, 16, "Hello World", 11);
					HDC hDCSrc = CreateCompatibleDC(hDCScreen);
					BITMAP bm;
					int nSize = GetObject(ii.hbmImage, sizeof(bm), &bm);
					ASSERT(nSize == sizeof(bm));
					HBITMAP hbmOld = (HBITMAP)SelectObject(hDCSrc,ii.hbmImage);
					BitBlt(hDCScreen, 300, 16,
					       (ii.rcImage.right - ii.rcImage.left) * nImages,
					       ii.rcImage.bottom - ii.rcImage.top,
						   hDCSrc,
						   0, 0,
						   SRCCOPY);
					SelectObject(hDCSrc, hbmOld);
					DeleteDC(hDCSrc);
					::ReleaseDC(::GetDesktopWindow(), hDCScreen);

//					HDC hDCDest = CreateCompatibleDC(NULL);
					//HDC hbmDest = CreateCompatibleBitmap(hDCDest);
					//if (hDCSrc && hDCDest && hbmDest)
						//{
						//SelectBitmap(hDCSrc, ii.hbmImage
						//SelectReleaseDC(NULL, hDCScreen);
						//}

					}
				else
					hr = E_FAIL;
#else
				// save a new copy of the imagelist.  We cannot make any
				// assumptions about the lifetime of the one we just got.
				m_hImageList = ImageList_Duplicate((HIMAGELIST) vRet.lVal);
				IMAGEINFO ii1, ii2;
				ImageList_GetImageInfo((HIMAGELIST) vRet.lVal, 0, &ii1);
				ImageList_GetImageInfo(m_hImageList, 0, &ii2);
				// switch to the new images
				CM_SetImageList(m_hWnd, m_hImageList);
				m_spImageList = pImageList;
#endif
			}
			else
				hr = E_INVALIDARG;
		}

		return hr;
	}
}

HRESULT STDMETHODCALLTYPE CEditX::putref_ImageList(IDispatch __RPC_FAR *ppImageList)
{
	return put_ImageList(ppImageList);
}

HRESULT STDMETHODCALLTYPE CEditX::AboutBox()
{
	CM_AboutBox(m_hWnd);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::PrintContents(long hDC, long lFlags)
{
	return CME2HRESULT(CM_Print(m_hWnd, hDC, lFlags));
}

HRESULT STDMETHODCALLTYPE CEditX::SetCaretPos(long lLine, long lCol)
{
	return CME2HRESULT(CM_SetCaretPos(m_hWnd, lLine, lCol));
}

HRESULT STDMETHODCALLTYPE CEditX::BufferColToViewCol(long lLine, long lBuffCol, long *plViewCol)
{
	*plViewCol = CM_BufferColToViewCol(m_hWnd, lLine, lBuffCol);
	return (*plViewCol != -1);
}

HRESULT STDMETHODCALLTYPE CEditX::ViewColToBufferCol(long lLine, long lViewCol, long *plBuffCol)
{
	*plBuffCol = CM_ViewColToBufferCol(m_hWnd, lLine, lViewCol);
	return (*plBuffCol != -1);
}

HRESULT STDMETHODCALLTYPE CEditX::get_SelLength(long __RPC_FAR *plLen)
{
	CATCH_ERROR_NO_HWND();
	CM_RANGE cmRange;
	CM_GetSel(m_hWnd, &cmRange, TRUE);
	*plLen = CM_GetTextLength(m_hWnd, &cmRange, FALSE);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::get_SelLengthLogical(long __RPC_FAR *plLen)
{
	CATCH_ERROR_NO_HWND();
	CM_RANGE cmRange;
	CM_GetSel(m_hWnd, &cmRange, TRUE);
	*plLen = CM_GetTextLength(m_hWnd, &cmRange, TRUE);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CEditX::put_BorderStyle(cmBorderStyle lBorderStyle)
{
	CATCH_ERROR_NO_HWND();
	if (FireOnRequestEdit(DISPID_CMPROP_BORDERSTYLE) == S_FALSE)
	{
		return S_FALSE;
	}
	HRESULT hRes = CME2HRESULT(CM_SetBorderStyle(m_hWnd, lBorderStyle));
	OnChanged(DISPID_CMPROP_BORDERSTYLE);
	return hRes;
}

HRESULT STDMETHODCALLTYPE CEditX::get_BorderStyle(cmBorderStyle __RPC_FAR *plBorderStyle)
{
	CATCH_ERROR_NO_HWND();
	*plBorderStyle = (cmBorderStyle) CM_GetBorderStyle(m_hWnd);
	return S_OK;
}

#endif
