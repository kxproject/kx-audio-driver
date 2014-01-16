
#include "ukx.h"
#include "uplgcont.h"

// #define UINEXE

//.............................................................................

// window class names
#define KXU_PLUGINCONT_CLASSNAME _T("UFX-PluginContainer") 
#define KXU_PLUGINVIEW_CLASSNAME _T("UFX-PluginView")
const char* const titledelim = " - ";
const char* const nullstr = "";

#define ID_PLUGINVIEW	200
#define ID_PRESETS		300
#define ID_BYPASS		301
#define ID_MUTE			302
#define ID_ONTOP		303

//.............................................................................
// uPluginContainer dialog

uPluginContainer::uPluginContainer(kDialog* pParent, uPluginView* plgview)
{
	CWnd::CWnd();

	kxuRegisterWindowClass(KXU_PLUGINCONT_CLASSNAME);
	
	pParentWindow = pParent;
	uCore = uGetCore();

	// init child pluginview
	pluginview = plgview;
	kxuRegisterWindowClass(KXU_PLUGINVIEW_CLASSNAME);
	pluginview->pParentContainer = this;

	// set plugin
	plugin = pluginview->plugin;

	// init presetman
	presetman.SelectPlugin(
		plugin->presets_guid, 
		plugin->get_plugin_description(IKX_PLUGIN_NAME));
	
	// local
	flags = 0;
	texture_id = 0;
	icon_id = 0;
	icon[0] = 0;
	icon[1] = 0;
	texture_depth = 160;
	bypass_id = 0;
	mute_id = 0;
	title = nullstr;
	state = UPC_NOWIN;
	x=0;

	SetRect(&rectTitle, 0, 0, 0, 0);
	SetRect(&rectPreset, 0, 0, 0, 0);
	SetRect(&rectBypass, 0, 0, 0, 0);
	SetRect(&rectMute, 0, 0, 0, 0);
	SetRect(&rectClose, 0, 0, 0, 0);
	SetRect(&rectMinimize, 0, 0, 0, 0);
	SetRect(&rectOnTop, 0, 0, 0, 0);

	MoveRegion = 0;
}

uPluginContainer::~uPluginContainer()
{
	if (pluginview) delete pluginview;

	if (MoveRegion) DeleteObject(MoveRegion);

	DestroyIcon(icon[0]);
	DestroyIcon(icon[1]);
}

BEGIN_MESSAGE_MAP(uPluginContainer, CWnd)
	//{{AFX_MSG_MAP(uPluginContainer)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	//ON_WM_LBUTTONDBLCLK()
	ON_WM_NCHITTEST()
	ON_WM_NCRBUTTONDOWN()
	//ON_WM_LBUTTONDOWN()
	//ON_WM_RBUTTONDOWN()
	ON_NOTIFY_EX(TTN_GETDISPINFO, 0, OnToolTip)
	ON_MESSAGE(WM_UCONTROL_MESSAGE, uControlChanged)
	ON_WM_SYSCOMMAND()
	ON_WM_ACTIVATE()
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//.............................................................................
// uPluginContainer message handlers

extern HMODULE hmodule;
#define PluginHandle hmodule

int uPluginContainer::create()
{
	//trace("<kxu> uPluginContainer::Create()\n");
	// Initialization
	title = plugin->name; //plugin->get_plugin_description(IKX_PLUGIN_NAME);
	char name[256];
	sprintf(name, "%s [%i]", title, ((iKXPlugin*) plugin)->pgm_id);
	title = nullstr;

	// Check if skin dll is ok
	if (!uCore->IsOK())
	{
		::MessageBox(NULL, "Can't load " RESOURCELIB "!\nPlease, reinstall UFX.", "UFX", MB_ICONERROR);
		return false;
	}

	HWND hWndParent = NULL;
	// if (pParentWindow) hWndParent = pParentWindow->m_hWnd;
	// fixme (hWndParent != NULL) kills large icon 

	// try to load x/y positions from settings
	RECT rect1 = {0, 0, 250, 100}, rect2;
	int x = y = 0;
	#ifndef UINEXE
	plugin->settings->get("tweakz_x", (DWORD*) &x);
	plugin->settings->get("tweakz_y", (DWORD*) &y);
    trace("plugview load pos %i, %i\n", x, y);
	#endif // UINEXE
	::GetWindowRect(::GetDesktopWindow(), &rect2);
	// check if window is out of screen
	if ((x <= 0) | (x > rect2.right) | (y <= 0) | (y > rect2.bottom))
	{ // center to screen
		trace("outofscreen\n");
		kxuCenterRectToAnotherOne(&rect1, &rect2);
	}
	else
	{
		MOVERECT(rect1, x, y);
	}
	VERIFY(CreateEx(WS_EX_APPWINDOW | WS_EX_CONTROLPARENT,
		KXU_PLUGINCONT_CLASSNAME, NULL,
		WS_POPUP |/*WS_CAPTION |*/ WS_SYSMENU | WS_MINIMIZEBOX,
		EXPANDRECTWH(rect1),
		hWndParent, NULL));

	hmenu = ::GetSystemMenu(m_hWnd, FALSE);
	::AppendMenu(hmenu, MF_SEPARATOR, 0, 0);
	::AppendMenu(hmenu, MF_STRING, 1, "&Keep Visible");
	EnableMenuItem(hmenu, SC_SIZE, MF_GRAYED | MF_BYCOMMAND);
	EnableMenuItem(hmenu, SC_MAXIMIZE, MF_GRAYED | MF_BYCOMMAND);
	EnableMenuItem(hmenu, SC_RESTORE, MF_GRAYED | MF_BYCOMMAND);
	SetMenuDefaultItem(hmenu, SC_CLOSE, FALSE);

	// pliginview init
	pluginview->Create(KXU_PLUGINVIEW_CLASSNAME, NULL, WS_VISIBLE, pluginview->size, this, ID_PLUGINVIEW);
	plugin->view = pluginview;

	pluginview->tooltip.Create(0, pluginview->m_hWnd);

	#ifndef UINEXE
	HINSTANCE PrevResHandle = AfxGetResourceHandle();
	AfxSetResourceHandle(PluginHandle);
	#endif // UINEXE

	pluginview->open();

	#ifndef UINEXE
	AfxSetResourceHandle(PrevResHandle);
	#endif // UINEXE


	// Set the icon for this dialog.
	if (icon_id)
	{
		#ifdef UINEXE
			HINSTANCE hResInstance =  AfxGetInstanceHandle();
		#else
			HINSTANCE hResInstance = PluginHandle;
		#endif // UINEXE

		icon[0] = (HICON) LoadImage(hResInstance, MAKEINTRESOURCE(icon_id), IMAGE_ICON, 16, 16, 0);
		icon[1] = (HICON) LoadImage(hResInstance, MAKEINTRESOURCE(icon_id), IMAGE_ICON, 32, 32, 0);		
	}
	if (icon[0] == NULL)
	{
		uCore->GetIcon(icon);
		SetIcon(icon[1], TRUE);	// Set large icon
		SetIcon(icon[0], FALSE); // Set small icon
	}

	// "biginit"
	if (bkgbitmap.GetSafeHandle() == NULL) BigInit();

	// Controls

	tooltip.Create(0, this->m_hWnd);

	int sysbtnorder = STATEFRAMEORDER(0, 3, 2, 3, 1, 2, 0, 0);
	btnClose.Create(&bmpClose, sysbtnorder, WS_VISIBLE, rectClose, this, BTN_CLOSE_ID);
	tooltip.SetTipString(&btnClose, "Close");
	if (RECTW(rectMinimize) > 0)
	{
		btnMinimize.Create(&bmpMinimize, sysbtnorder, WS_VISIBLE, rectMinimize, this, BTN_MINIMIZE_ID);
		tooltip.SetTipString(&btnMinimize, "Minimize");
	}
	if (RECTW(rectOnTop) > 0)
	{
	btnOnTop.Create(&bmpOnTop, sysbtnorder, UIBS_2STATE | WS_VISIBLE, rectOnTop, this, BTN_ONTOP_ID);
	tooltip.SetTipString(&btnOnTop, "Keep Visible");
	}

	lblTitle.Create("", ULS_HEADERFONT | ULS_SHADOW | WS_VISIBLE, rectTitle, this, LBL_TITLE_ID);
	const char* delim = (title[0]) ? titledelim : nullstr;
	lblTitle.printf("%s%s%s", title, delim, name);
	lblTitle.SetColor(0xD0D0D0); lblTitle.SetShadowColor(0x181818);

	if (RECTW(rectBypass) > 0)
	{
		chkBypass.Create("", WS_VISIBLE, rectBypass, this, BTN_BYPASS_ID);
		chkBypass.SetLampColor(UC_GOOD);
		tooltip.SetTipString(&chkBypass, "Bypass");
	}

	if (RECTW(rectMute) > 0)
	{
		chkMute.Create("", WS_VISIBLE, rectMute, this, BTN_MUTE_ID);
		chkMute.SetLampColor(UC_BAD);
		tooltip.SetTipString(&chkMute, "Mute");
	}

	if (RECTW(rectPreset) > 0)
	{
		cmbPreset.Create("Custom", UECS_DROPDOWN | UECS_HIDDENNOTIFIES | WS_VISIBLE, 
			rectPreset, this, CMB_PRESET_ID);
		tooltip.SetTipString(&cmbPreset, NULL, UTT_AUTO | UTT_CENTERTIP);
	}

	if (plugin->ikx) {if (kx_get_ndevices() > 1) {strcat(name, " - "); strcat(name, plugin->ikx->get_device_name());}}
	SetWindowText(name);

	// synchronize pluginview to plugin state, 
	int n = plugin->get_param_count();
	int v = 0;
	for (int i = 0; i < n; i++) 
	{
		plugin->get_param(i, &v);
		plugin->post_param_changed(i, v);
	}

	return true;  // OK
}

#define SYSBUTTON_OFFSET	4
#define SYSBUTTON_SPACE		1
#define	HEADER_HEIGHT		50
#define HDRCTRL_SPACE		8
#define COMBO_HEIGHT		19
#define MINIMUM_COMBO_WIDTH 100
#define MAXIMUM_COMBO_WIDTH 220

int uPluginContainer::BigInit2()
{
	LPRECT plgviewrect = &pluginview->size;

	int q;

	RECT hdr_rect;

	// captionbar
	uCore->GetElementData(UCB_CAPTIONBAR_RIGHT, UED_SIZERECT, sizeof hdr_rect, &hdr_rect);
	CaptionBarHeight = hdr_rect.bottom;

	// calculate size
	SETRECT(BkgSize, 0, 0, plgviewrect->right, CaptionBarHeight) 
	SETRECT(Size, 0, 0, plgviewrect->right, CaptionBarHeight + plgviewrect->bottom) 

	hdr_rect.left = 0; hdr_rect.right = BkgSize.right;
	MoveRegion = CreateRectRgnIndirect(&hdr_rect);
	ASSERT(MoveRegion);

	// RENDER BACKGROUND
	LPRECT pRenderData = &hdr_rect;
	int Element = UCB_CAPTIONBAR_RIGHT;
	uCore->GetControlBitmap(&bkgbitmap, 0, 1, &Element, &pRenderData);

	// sys buttons
	Element = UCB_CLOSE;
	pRenderData = &rectClose;
	uCore->GetControlBitmap(&bmpClose, 0, 1, &Element, &pRenderData);
	rectClose.right /= uCore->GetElementData(Element, UED_NFRAMES);
	q = rectClose.right;
	MOVERECT(rectClose, Size.right - (SYSBUTTON_OFFSET + q), SYSBUTTON_OFFSET);

	// calc title
	CopyRect(&rectTitle, &hdr_rect);
	rectTitle.right = rectClose.left;
	SCALERECT(rectTitle, -(HDRCTRL_SPACE >> 1), 0);
	rectTitle.left += (HDRCTRL_SPACE >> 2);

	// apply texture and bevel
	CBitmap texture;
	CBitmap bevel;

	if (texture_id) 
	{
		#ifndef UINEXE
			HINSTANCE PrevResHandle = AfxGetResourceHandle();
			AfxSetResourceHandle(PluginHandle);
		#endif // UINEXE
		texture.LoadBitmap(texture_id);
		#ifndef UINEXE
			AfxSetResourceHandle(PrevResHandle);
		#endif // UINEXE
	}
	if (texture.m_hObject == NULL)
	{
		Element = UCB_TEXTURE;
		SETRECT(hdr_rect, 0, 0, 0, 0);
		pRenderData = &hdr_rect;
		uCore->GetControlBitmap(&texture, 0, 1, &Element, &pRenderData);
	}

	kxuApplyTexture((HBITMAP) bkgbitmap, &BkgSize, (HBITMAP) texture, 0, 0, NULL, texture_depth);

	// bevel
	Element = UCB_FRAMEBEVEL;
	CopyRect(&hdr_rect, &BkgSize); // hdr_rect.top -= 8;
	pRenderData = &hdr_rect;
	uCore->GetControlBitmap(&bevel, 0, 1, &Element, &pRenderData);
	kxuBlend((HBITMAP) bkgbitmap, &BkgSize, (HBITMAP) bevel, 0, 0, BLEND_OVERLAY, 480);

	// render view
	if (!(flags & UPC_FLAGS_CUSTOMBKGND))
	{
		Element = UCB_BACK;
		pRenderData = plgviewrect;
		uCore->GetControlBitmap(&pluginview->bkgbitmap, 0, 1, &Element, &pRenderData);
	}

	kxuApplyTexture((HBITMAP) pluginview->bkgbitmap, plgviewrect, (HBITMAP) texture, 0, CaptionBarHeight, NULL, texture_depth);

	// bevel
	Element = UCB_FRAMEBEVEL;
	CopyRect(&hdr_rect, plgviewrect); hdr_rect.bottom += 2; hdr_rect.left -= 3;
	pRenderData = &hdr_rect;
	uCore->GetControlBitmap(&bevel, 0, 1, &Element, &pRenderData);
	kxuBlend((HBITMAP) pluginview->bkgbitmap, plgviewrect, (HBITMAP) bevel, 0, 0, BLEND_OVERLAY, 480);

	// resize all windows
	CopyRect(&hdr_rect, &Size); CalcWindowRect(&hdr_rect);
	if (GetSafeHwnd()) SetWindowPos(0, EXPANDRECTWH(hdr_rect), SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER);

	MOVERECT(*plgviewrect, 0, CaptionBarHeight);
	if (pluginview->GetSafeHwnd()) pluginview->SetWindowPos(0, EXPANDRECTWH(*plgviewrect), 0);

	return 0;

}

int uPluginContainer::BigInit()
{
	enum
	{
		CAPTIONBAR_LEFT,
		CAPTIONBAR,
		SCREW1,
		SCREW2,
		PRESET,
		BYPASS,
		MUTE,

		_numofelements
	};

	RECT emap[_numofelements]; ZM(emap);

	int Element[_numofelements + 1] = 
	{
		UCB_BACK, 
		UCB_CAPTIONBAR_LEFT, 
		UCB_CAPTIONBAR_RIGHT, 
		UCB_SCREW, 
		UCB_SCREW,
		UCB_PRESET,
		UCB_BYPASS,
		UCB_MUTE
	};

	LPRECT pRenderData[_numofelements + 1] = 
	{
		&BkgSize, 
		&emap[CAPTIONBAR_LEFT], 
		&emap[CAPTIONBAR], 
		&emap[SCREW1], 
		&emap[SCREW2],
		&emap[PRESET],
		&emap[BYPASS],
		&emap[MUTE]
	};

	int q, s;

	// check if plugin has its own background
	BITMAP bm;
	if (pluginview->bkgbitmap.GetSafeHandle())
	{
		flags |= UPC_FLAGS_CUSTOMBKGND;
		pluginview->bkgbitmap.GetBitmap(&bm);
		pluginview->size.right = bm.bmWidth;
		pluginview->size.bottom = bm.bmHeight;
	}

	if (flags & UPC_FLAGS_TOOLSIZE) return BigInit2();

	LPRECT plgviewrect = &pluginview->size;

	// calculate size
	SETRECT(BkgSize, 0, 0, plgviewrect->right, HEADER_HEIGHT) 
	SETRECT(Size, 0, 0, plgviewrect->right, HEADER_HEIGHT + plgviewrect->bottom) 

	// bypass
	if (flags & UPC_FLAGS_BYPASS)
	{
		SETRECT(rectBypass, 0, 0, 12, 8);
		MOVERECT(rectBypass, HDRCTRL_SPACE, 9);
		uCore->GetElementData(UCB_BYPASS, UED_SIZERECT, sizeof emap[BYPASS], &emap[BYPASS]);
		MOVERECT(emap[BYPASS], rectBypass.right + 1, rectBypass.top - 1);
		rectBypass.right = emap[BYPASS].right;
	}

	// mute
	if (flags & UPC_FLAGS_MUTE)
	{
		SETRECT(rectMute, 0, 0, 12, 8);
		MOVERECT(rectMute, HDRCTRL_SPACE, 9 + 13);
		uCore->GetElementData(UCB_MUTE, UED_SIZERECT, sizeof emap[MUTE], &emap[MUTE]);
		MOVERECT(emap[MUTE], rectMute.right + 1, rectMute.top - 1);
		rectMute.right = emap[MUTE].right;
	}
	
	// captionbar
	uCore->GetElementData(UCB_CAPTIONBAR_LEFT, UED_SIZERECT, sizeof emap[CAPTIONBAR_LEFT], &emap[CAPTIONBAR_LEFT]);
	CaptionBarHeight = emap[CAPTIONBAR_LEFT].bottom;
	MOVERECT(emap[CAPTIONBAR_LEFT], rectBypass.right, 0);
	if (!(flags & (UPC_FLAGS_BYPASS | UPC_FLAGS_MUTE))) emap[CAPTIONBAR_LEFT].right = emap[CAPTIONBAR_LEFT].left;
	CopyRect(&emap[CAPTIONBAR], &emap[CAPTIONBAR_LEFT]);
	emap[CAPTIONBAR].left = emap[CAPTIONBAR].right; emap[CAPTIONBAR].right = BkgSize.right;
	POINT ps[4] = 
	{
		{emap[CAPTIONBAR_LEFT].left, emap[CAPTIONBAR_LEFT].top},
		{emap[CAPTIONBAR].right, emap[CAPTIONBAR].top},
		{emap[CAPTIONBAR].right, emap[CAPTIONBAR].bottom},
		{emap[CAPTIONBAR].left, emap[CAPTIONBAR].bottom}
	};
	MoveRegion = CreatePolygonRgn(ps, 4, ALTERNATE);
	ASSERT(MoveRegion);

	// screws
	uCore->GetElementData(UCB_SCREW, UED_SIZERECT, sizeof emap[SCREW2], &emap[SCREW2]);
	SETRECT(emap[SCREW1], BkgSize.right - emap[SCREW2].right - 8, CaptionBarHeight + 6, 
		BkgSize.right - 8, CaptionBarHeight + emap[SCREW2].bottom + 6);
	MOVERECT(emap[SCREW2], emap[CAPTIONBAR].left, emap[SCREW1].top);
	if (emap[SCREW2].left == 0) {MOVERECT(emap[SCREW2], 8, 0);}
	
	// preset
	RECT rect;
	uCore->GetElementData(UCB_PRESET, UED_SIZERECT, sizeof rect, &rect);
	SETRECT(rectPreset, emap[SCREW2].right + (HDRCTRL_SPACE - (HDRCTRL_SPACE >> 2)),
		CaptionBarHeight + 3,
		emap[SCREW1].left - (HDRCTRL_SPACE - (HDRCTRL_SPACE >> 2)),
		CaptionBarHeight + 3 + COMBO_HEIGHT);

	q = RECTW(rectPreset);
	s = RECTW(rect) + (HDRCTRL_SPACE >> 1) - 1;

	if (q > MINIMUM_COMBO_WIDTH) 
	{
		if ((q - s) > MINIMUM_COMBO_WIDTH)
		{
			if ((q - s) < MAXIMUM_COMBO_WIDTH)
			{
				rectPreset.left += s;
			}
			else
			{
				rectPreset.left = rectPreset.right - MAXIMUM_COMBO_WIDTH;
			}
			CopyRect(&emap[PRESET], &rect);
			MOVERECT(emap[PRESET], rectPreset.left - s, rectPreset.top + 4);
		}
	}

	// RENDER BACKGROUND
	uCore->GetControlBitmap(&bkgbitmap, 0, _numofelements + 1, Element, pRenderData);

	// sys buttons
	Element[0] = UCB_CLOSE;
	pRenderData[0] = &rectClose;
	uCore->GetControlBitmap(&bmpClose, 0, 1, Element, pRenderData);
	rectClose.right /= uCore->GetElementData(Element[0], UED_NFRAMES);
	q = rectClose.right;
	MOVERECT(rectClose, Size.right - (SYSBUTTON_OFFSET + q), SYSBUTTON_OFFSET);

	Element[0] = UCB_MINIMIZE;
	pRenderData[0] = &rectMinimize;
	uCore->GetControlBitmap(&bmpMinimize, 0, 1, Element, pRenderData);
	rectMinimize.right /= uCore->GetElementData(Element[0], UED_NFRAMES);
	q = rectMinimize.right;
	MOVERECT(rectMinimize, rectClose.left - (SYSBUTTON_SPACE + q), SYSBUTTON_OFFSET);

	Element[0] = UCB_ONTOP;
	pRenderData[0] = &rectOnTop;
	uCore->GetControlBitmap(&bmpOnTop, 0, 1, Element, pRenderData);
	rectOnTop.right /= uCore->GetElementData(Element[0], UED_NFRAMES);
	q = rectOnTop.right;
	MOVERECT(rectOnTop, rectMinimize.left - (SYSBUTTON_SPACE + q), SYSBUTTON_OFFSET);

	// calc title
	CopyRect(&rectTitle, &emap[CAPTIONBAR]);
	rectTitle.right = rectOnTop.left;
	SCALERECT(rectTitle, -(HDRCTRL_SPACE >> 1), 0);
	if (!(flags & (UPC_FLAGS_BYPASS | UPC_FLAGS_MUTE))) rectTitle.left += (HDRCTRL_SPACE >> 1);

	// apply texture and bevel
	CBitmap texture;
	CBitmap bevel;

	if (texture_id) 
	{
		#ifndef UINEXE
			HINSTANCE PrevResHandle = AfxGetResourceHandle();
			AfxSetResourceHandle(PluginHandle);
		#endif // UINEXE
		texture.LoadBitmap(texture_id);
		#ifndef UINEXE
			AfxSetResourceHandle(PrevResHandle);
		#endif // UINEXE
	}
	if (texture.m_hObject == NULL)
	{
		Element[0] = UCB_TEXTURE;
		SETRECT(emap[CAPTIONBAR], 0, 0, 0, 0);
		pRenderData[0] = &emap[CAPTIONBAR];
		uCore->GetControlBitmap(&texture, 0, 1, Element, pRenderData);
	}

	kxuApplyTexture((HBITMAP) bkgbitmap, &BkgSize, (HBITMAP) texture, 0, 0, NULL, texture_depth);

	// bevel
	Element[0] = UCB_FRAMEBEVEL;
	CopyRect(&emap[CAPTIONBAR], &BkgSize);
	pRenderData[0] = &emap[CAPTIONBAR];
	uCore->GetControlBitmap(&bevel, 0, 1, Element, pRenderData);
	kxuBlend((HBITMAP) bkgbitmap, &BkgSize, (HBITMAP) bevel, 0, 0, BLEND_OVERLAY, 480);

	// render view
	if (!(flags & UPC_FLAGS_CUSTOMBKGND))
	{
		Element[0] = UCB_BACK;
		pRenderData[0] = plgviewrect;
		uCore->GetControlBitmap(&pluginview->bkgbitmap, 0, 1, Element, pRenderData);
	}

	// pluginview->__custom_draw_(); // temporary undocumented

	kxuApplyTexture((HBITMAP) pluginview->bkgbitmap, plgviewrect, (HBITMAP) texture, 0, HEADER_HEIGHT, NULL, texture_depth);

	// bevel
	Element[0] = UCB_FRAMEBEVEL;
	CopyRect(&emap[CAPTIONBAR], plgviewrect); emap[CAPTIONBAR].bottom++;
	pRenderData[0] = &emap[CAPTIONBAR];
	uCore->GetControlBitmap(&bevel, 0, 1, Element, pRenderData);
	kxuBlend((HBITMAP) pluginview->bkgbitmap, plgviewrect, (HBITMAP) bevel, 0, 0, BLEND_OVERLAY, 480);

	// resize all windows
	CopyRect(&emap[CAPTIONBAR], &Size); // CalcWindowRect(&emap[CAPTIONBAR]);	
	if (GetSafeHwnd()) SetWindowPos(0, EXPANDRECTWH(emap[CAPTIONBAR]), SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER);

	MOVERECT(*plgviewrect, 0, HEADER_HEIGHT);
	if (pluginview->GetSafeHwnd()) pluginview->SetWindowPos(0, EXPANDRECTWH(*plgviewrect), 0);

	return 0;
}

uintptr_t uPluginContainer::extended_gui(int command, uintptr_t /* p1 = 0 */, uintptr_t /* p2 = 0 */)
{
    trace(__FUNCTION__ "(), %i\n", command);
		switch (command)
		{
		case PLUGINGUI_FOREGROUND:
            ShowWindow(SW_RESTORE);
			SetForegroundWindow();
			break;
		case PLUGINGUI_GET_CLASS:
			return 0; // PLUGINGUI_CLASS_KXU;
		}
		return 0;
};

BOOL uPluginContainer::OnEraseBkgnd(CDC* pDC)
{
	/* since this function can be requested by one of the
	   child controls we must be sure that background
	   is already drawn */
	if (bkgbitmap.GetSafeHandle() == NULL) BigInit();

	RECT rect; pDC->GetClipBox(&rect);
	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);
	MemDC.SelectObject(&bkgbitmap);

	// Draw
	if (RECTH(rect) > HEADER_HEIGHT) rect.bottom = rect.top + HEADER_HEIGHT;
	pDC->BitBlt(EXPANDRECTWH(rect), &MemDC, EXPANDRECTLT(rect), SRCCOPY);

	return true;
}

void uPluginContainer::OnPaint() 
{
	RECT rect;
	GetClientRect(&rect);

	CPaintDC dc(this); // device context for painting

	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		
		int x = (RECTW(rect) - cxIcon + 1) >> 1;
		int y = (RECTH(rect) - cyIcon + 1) >> 1;

		// Draw the icon
		dc.DrawIcon(x, y, icon[0]);
	}
	else
	{
		// do nothing
	}
}

int uPluginContainer::param_changed(int id, kxparam_t value)
{
	if (state & UPC_BYPMUTE_LOCKED) 
        return true;

	if ((flags & UPC_FLAGS_BYPASS) && (bypass_id == id)) 
        chkBypass.SetCheck(int(value)); 
	else if ((flags & UPC_FLAGS_MUTE) && (mute_id == id)) 
        chkMute.SetCheck(int(value)); 

	return true;
}

LRESULT uPluginContainer::uControlChanged(WPARAM id, LPARAM code)
{
	MENUITEMINFO mi = {sizeof MENUITEMINFO, MIIM_STATE, 0, 0, 0};
	int rv = 0;
	CWnd* w;
	int i = 0;
	int f;
	char text1[256];
	char text2[256];

	switch (id)
	{
	case CMB_PRESET_ID | UCBN_DROPDOWN: // before dropdown: fill combo with presets
		cmbPreset.ResetItems();
		cmbPreset.AddItem("Save...");
		cmbPreset.AddItem("Delete");
		cmbPreset.AddItem(NULL, MF_SEPARATOR);
		#define PRESETS_OFFSET 3
		cmbPreset.__setitemsoffset(PRESETS_OFFSET);
		while (!presetman.EnumPresets(i, MAX_ECOMBO_STRING, text1))
		{
			f = (strcmp(text1, "[Default]")) ? 0 : UECAI_INSERT_FIRST;
			if (((i + PRESETS_OFFSET - 1) & 31) == 31) f |=  MF_MENUBARBREAK;
			cmbPreset.AddItem(text1, f);
			i++;
		}
		cmbPreset.AddItem(NULL, MF_SEPARATOR);
		_import_presets_offset = 
		cmbPreset.AddItem("Import...");
		cmbPreset.AddItem("Export...");
		cmbPreset.__sethiddenitemsoffset(_import_presets_offset);
		break;

	case CMB_PRESET_ID:
		if (code >= _import_presets_offset) code += 0x1000 - _import_presets_offset;
		switch (code)
		{
		case 0: // save preset
			cmbPreset.GetWindowText(text2, 256);
			if (!strcmp(text2, "[Default]")) text2[0] = 0;
			else if (!strcmp(text2, "Custom")) text2[0] = 0;
			rv = AskForString(this, "Save Preset", text1, text2);
			if (rv == IDOK)
			{
				if (!strcmp(text1, "Custom")) 
				{
					sprintf(text2, "The \"%s\" name is reserved and cannot be used for the new preset.\n", text1);
					GetWindowText(text1, 256);
					MessageBox(text2, text1, MB_ICONEXCLAMATION);
					break;
				}
				rv = presetman.ReadPreset(text1, plugin->get_param_count(), (int*) text2, (int*) &i);
				if (i == PRESETID_FACTORY)
				{
					sprintf(text2, "The \"%s\" is a factory preset and cannot be replaced.\n", text1);
					GetWindowText(text1, 256);
					MessageBox(text2, text1, MB_ICONEXCLAMATION);
					break;
				}
				// save
				plugin->get_all_params((int*) text2);
				rv = presetman.WritePreset(text1, plugin->get_param_count(), (int*) text2);
				if (rv == 0) {cmbPreset.SetWindowText(text1);}
			}
			break;
		case 1: // delete preset
			cmbPreset.GetWindowText(text2, 256);
			sprintf(text1, "Delete the \"%s\" preset?", text2);
			rv = MessageBox(text1, "Delete Preset", MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2);
			if (rv == IDYES)
			{
				rv = presetman.ReadPreset(text2, plugin->get_param_count(), (int*) text1, (int*) &i);
				if (i == PRESETID_FACTORY)
				{
					sprintf(text1, "The \"%s\" is a factory preset and cannot be deleted.\n", text2);
					GetWindowText(text2, 256);
					MessageBox(text1, text2, MB_ICONEXCLAMATION);
					break;
				}
				if (strcmp(text2, "Custom")) presetman.RemovePreset(text2);
				state |= UPC_BYPMUTE_LOCKED;
				plugin->set_defaults();
				state ^= UPC_BYPMUTE_LOCKED;
			}
			break;

		case 0x1000: // import presets
			{
				text1[0] = 0;
				OPENFILENAME ofn = 
				{
					sizeof OPENFILENAME,
					m_hWnd,
					NULL, "*.upp\0*.upp\0", NULL, 0, 0,
					text1, 256, NULL, 0,
					NULL, "Import Presets",
					OFN_PATHMUSTEXIST | OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
					0, 0, "upp", 
					NULL, NULL, NULL
				};
				if (GetOpenFileName(&ofn)) 
				{
					FILE* file = fopen(ofn.lpstrFile, "rt");	
					if (!file) 
					{
						sprintf(text2, "Can't open %s", text1);
						MessageBox(text2, "Import Presets", MB_ICONEXCLAMATION); 
						break;
					}
					i = presetman.ImportPresets(m_hWnd, file);
					sprintf(text2, "\"%s\"\n%i user preset(s) imported.", text1, i);
					MessageBox(text2, "Import Presets", MB_OK);
					fclose(file);
				}
			}
			break;

		case 0x1001: // export presets
			{
				time_t date; time(&date);
				tm* ldate = localtime(&date);
				DWORD x = 256; 
				strcpy(text2, "Unknown");
				GetUserName(text2, &x); // fixme, sometimes fails
				sprintf(text1, "%s, %s at %02i%02i%02i", text2, plugin->name, ldate->tm_year - 100, ldate->tm_mon, ldate->tm_mday);
				OPENFILENAME ofn = 
				{
					sizeof OPENFILENAME,
					m_hWnd,
					NULL, "*.upp\0*.upp\0", NULL, 0, 0,
					text1, 256, NULL, 0,
					NULL, "Export Presets",
					OFN_ENABLESIZING | OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN | OFN_HIDEREADONLY,
					0, 0, "upp", 
					NULL, NULL, NULL
				};
				if (GetSaveFileName(&ofn)) 
				{
					FILE* file = fopen(ofn.lpstrFile, "wt");	
					if (!file) 
					{
						sprintf(text2, "Can't open %s", text1);
						MessageBox(text2, "Export User Presets", MB_ICONEXCLAMATION); 
						break;
					}
					i = presetman.ExportPresets(m_hWnd, file);
					sprintf(text2, "\"%s\"\n%i user preset(s) exported.", text1, i);
					MessageBox(text2, "Export User Presets", MB_OK);
					fclose(file);
				}
			}
			break;

		default: // select preset here
			cmbPreset.GetWindowText(text1, MAX_ECOMBO_STRING);
			if (!presetman.ReadPreset(text1, plugin->get_param_count(), (int*) text2))
			{
				state |= (UPC_PRESET_LOCKED | UPC_BYPMUTE_LOCKED);
				plugin->set_all_params((int*) text2);
				state ^= (UPC_PRESET_LOCKED | UPC_BYPMUTE_LOCKED);
			}
			break;
		}
		break;

	case BTN_CLOSE_ID:
		DestroyWindow();
		break;

	case BTN_MINIMIZE_ID:
        ReleaseCapture();
		CloseWindow();
		break;

	case BTN_ONTOP_ID:
		GetMenuItemInfo(hmenu, 1, FALSE, &mi);
		if (mi.fState & MFS_CHECKED)
		{
			i = 0;
			w = (CWnd*) &wndNoTopMost; 
		}
		else
		{
			i = 1;
			w = (CWnd*) &wndTopMost;
		}
		if (btnOnTop.m_hWnd) btnOnTop.SetCheck(i);
		mi.fState = ((mi.fState & MFS_CHECKED) ^ MFS_CHECKED);
		i = SetMenuItemInfo(hmenu, 1, FALSE, &mi);
		// CheckMenuItem(hmenu, 0, MF_BYCOMMAND | mi.fState); 
		SetWindowPos(w, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		break;

	case BTN_BYPASS_ID:
		plugin->set_param(bypass_id, chkBypass.GetCheck());
		break;

	case BTN_MUTE_ID:
		plugin->set_param(mute_id, chkMute.GetCheck());
		break;
	}

	return true;
}

void uPluginContainer::SwitchNakedWindow() 
{
	DWORD add, rem;
	RECT rect;
	int x;
	int sysbtnshow = SW_HIDE;

	GetWindowRect(&rect);
	
	state ^= UPC_NOWIN;
	if (state & UPC_NOWIN)
	{
		add = 0;
		rem = WS_CAPTION;
		x = GetSystemMetrics(SM_CXFIXEDFRAME);
		rect.left += x;
		rect.right -= x;
		x = GetSystemMetrics(SM_CYFIXEDFRAME);
		rect.top += x;
		rect.bottom -= x;
		rect.top += GetSystemMetrics(SM_CYCAPTION);
		sysbtnshow = SW_SHOW;
		char name[256]; GetWindowText(name, 256);
		char* delim = (title[0]) ? (char*) titledelim : (char*) nullstr;
		lblTitle.printf("%s%s%s", title, delim, name);
	}
	else
	{
		add = WS_CAPTION;
		rem = 0;
		x = GetSystemMetrics(SM_CXFIXEDFRAME);
		rect.left -= x;
		rect.right += x;
		x = GetSystemMetrics(SM_CYFIXEDFRAME);
		rect.top -= x;
		rect.bottom += x;
		rect.top -= GetSystemMetrics(SM_CYCAPTION);
		lblTitle.SetWindowText(title);
	}
		
		ModifyStyle(rem, add, SWP_FRAMECHANGED); 
		btnClose.ShowWindow(sysbtnshow);
		btnMinimize.ShowWindow(sysbtnshow);
		btnOnTop.ShowWindow(sysbtnshow);
		SetWindowPos(NULL, EXPANDRECTWH(rect), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
}

BOOL uPluginContainer::OnToolTip(UINT /* id */, NMHDR *pnm, LRESULT *pResult)
{
	*pResult = 0;

	LPTOOLTIPTEXT ptt = (LPTOOLTIPTEXT) pnm;

	ASSERT(ptt->uFlags & TTF_IDISHWND);
	::GetWindowText((HWND) pnm->idFrom, ptt->szText, 64);
	ptt->hinst = NULL;

	return TRUE;
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR uPluginContainer::OnQueryDragIcon()
{
	return (HCURSOR) icon[0];
}

void  uPluginContainer::OnNcRButtonDown(UINT nHitTest, CPoint point)
{
	CWnd::OnNcRButtonDown(nHitTest, point);
	if (nHitTest == HTCAPTION)
	{
		UINT cmd = TrackPopupMenu(hmenu, TPM_RETURNCMD, point.x, point.y, 0, m_hWnd, NULL);
		::SendMessage(m_hWnd, WM_SYSCOMMAND, cmd, 0);
	}
}

UINT uPluginContainer::OnNcHitTest(CPoint point)
{
	CPoint pt(point);
	if (state & UPC_NOWIN)
	{
		ScreenToClient(&pt);
		if (PtInRegion(MoveRegion, pt.x, pt.y)) 
		{
			CWnd::OnNcHitTest(point);
			return HTCAPTION;
		}
	}
	return CWnd::OnNcHitTest(point);
}

void uPluginContainer::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID > SC_SIZE) 
        nID &= 0xFFF0;

	switch(nID)
	{
	case 1:
		uControlChanged(BTN_ONTOP_ID, 0);
		break;

	case SC_MOVE:
		RedrawWindow(NULL, NULL, RDW_UPDATENOW|RDW_ALLCHILDREN);
	default:
		CWnd::OnSysCommand(nID, lParam);
		break;
	}
}

void uPluginContainer::OnMove(int x, int y)
{
	#ifndef UINEXE
	plugin->settings->set("tweakz_x", (DWORD) x);
	plugin->settings->set("tweakz_y", (DWORD) y);
    trace("plugview move pos %i, %i\n", x, y);
	#endif UINEXE
}

void uPluginContainer::OnActivate(UINT nState, CWnd* /*pWndOther*/, BOOL /*bMinimized*/)
{
	if (lblTitle.m_hWnd) lblTitle.SetColor((nState) ? 0xD0D0D0 : 0x909090);  
}

void uPluginContainer::PostNcDestroy() 
{
	plugin->cp = NULL;
	plugin->view = NULL;
	plugin = NULL;
	delete this;
	CWnd::PostNcDestroy();
}
