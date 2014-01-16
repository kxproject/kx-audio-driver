
#include "ukx.h"
#include "../ufw/boxcontrols.h"
#include "uplgboxcont.h"

//.............................................................................

#define KXU_PLUGINBOXVIEW_CLASSNAME _T("kXUPluginBoxView")

//.............................................................................

#define SHADOW_SIZE	4

enum
{
    COLLAPSE_ID = 3
};

uPluginBoxContainer::uPluginBoxContainer(kDialog* parent_, kWindow* that_, uPlugin* plugin, kFile* mf_) :
iKXDSPWindow(parent_, that_, (iKXPlugin*) plugin, mf_)
{
	uPluginBoxContainer::plugin = plugin;
	SETRECT(rect, 0, 0, 0, 0);
	state = 3333;
    pin_size = 0;
	ipins = plugin->pgm_info.ipins;
	opins = plugin->pgm_info.opins;
	pin_rect = new RECT[ipins + opins];

	// boxview
	if (plugin->options & UP_HAS_BOXVIEW)
	{
		uPluginBoxView* boxview = (uPluginBoxView*) plugin->create_boxview();
		boxview->container = this;
		plugin->boxview = boxview;
		kxuRegisterWindowClass(KXU_PLUGINBOXVIEW_CLASSNAME);
		boxview->CreateEx(0, KXU_PLUGINBOXVIEW_CLASSNAME, NULL, WS_POPUP, boxview->size, NULL, NULL);	
		SetParent(boxview->m_hWnd, that_->get_wnd());
		boxview->tooltip.Create(0, that_->get_wnd() /*boxview->m_hWnd*/ );
	}

    collapse = new Arrow;
    collapse->create(WS_VISIBLE, -20, -20, 7, 4, this, that_, COLLAPSE_ID);

    kxarrow = 0;
}

uPluginBoxContainer::~uPluginBoxContainer()
{
    delete collapse;
	delete [] pin_rect;
	if (plugin->boxview != NULL) 
	{
		plugin->boxview->destroy();
		plugin->boxview = NULL;
	}
}

int uPluginBoxContainer::draw(kDraw* dc, int x, int y, int flag, const char* name, int /* pgm_id */)
{
	// if ((x == 0) & (y == 0)) return 0;
	// return iKXDSPWindow::draw(dc, x, y, flag, name, pgm_id);

	// trace("uPluginBoxContainer::draw, flag %08x\n", flag);

	if (state != flag)
	    render(name, flag);
	
	HDC dstdc = dc->get_dc();
	// trace("uPluginBoxContainer::draw(), x %i, y %i, dc %x\n", x, y, dc);
	HDC srcdc = CreateCompatibleDC(dstdc);
	HBITMAP old_bitmap = (HBITMAP) SelectObject(srcdc, bitmap);

	// shadow only
	if (!((x == 0) && (y == 0))) 
	{
		BitBlt(srcdc, rect.right, 0, SHADOW_SIZE, rect.bottom + SHADOW_SIZE, dstdc, x + rect.right, y, SRCCOPY);
		BitBlt(srcdc, 0, rect.bottom, rect.right + SHADOW_SIZE, SHADOW_SIZE, dstdc, x, y + rect.bottom, SRCCOPY);
		SelectObject(srcdc, old_bitmap);
		render_shadow();
		SelectObject(srcdc, bitmap);
	}
	// end of shadow only

	BitBlt(dstdc, x, y, rect.right + SHADOW_SIZE, rect.bottom + SHADOW_SIZE, srcdc, 0, 0, SRCCOPY);

	SelectObject(srcdc, old_bitmap);
	DeleteDC(srcdc);

    // kx38 workaround (hiding min/max button)
    kxarrow = ::FindWindowEx(that->get_wnd(), 0, WC_BUTTON, 0);
    if (::IsWindowVisible(kxarrow))
        ::ShowWindow(kxarrow, SW_HIDE);

    //trace(__FUNCTION__" f %08x, s %08x\n", flag, state);
	return 0; 
}

void uPluginBoxContainer::notify(int id, int /*state*/)
{
    if (id == COLLAPSE_ID)
    {
        HWND parent = ::GetParent(collapse->m_hWnd);
        int id = ::GetDlgCtrlID(kxarrow);
        ::SendMessage(parent, WM_COMMAND, 
            MAKEWPARAM(id, BN_CLICKED), (LPARAM) kxarrow);
        // ::SendMessage(kxarrow, BM_SETSTATE, TRUE, 0);
    }
}

int uPluginBoxContainer::find_gpr(const kPoint* pt, int* gpr, int /*flag*/, char* name)
{
	// if (name == 0) _asm int 3;
	// return iKXDSPWindow::find_gpr(pt, gpr, name);
	// trace("uPluginBoxContainer::find_gpr() - %04x %s\n", *gpr, name);
	const int pins = (ipins + opins);
	for (int i = 0; i < pins; i++)
	{
		if (PtInRect(&pin_rect[i], *((LPPOINT) pt)))
		{
			if (!name) 
				i = i;
			else 
                strcpy(name, plugin->pgm_info.tooltips[i]);
			*gpr = i; // pgm_info.id[i];
			//trace("\tret: %04x %s\n", *gpr, name);
			return 0;
		}
	}
	return -1;
}

int uPluginBoxContainer::get_window_rect(char* name, int flag, kSize* sz)
{
	//trace("uPluginBoxContainer::get_window_rect()\n");
	//char x[64];
	//iKXDSPWindow::get_window_rect(x, sz);
	render(name, flag);
    //trace(__FUNCTION__" f %08x, s %08x\n", flag, state);
	sz->cx = rect.right	/* + SHADOW_SIZE*/; sz->cy = rect.bottom /* + SHADOW_SIZE */; 
	return 0; 
}

int uPluginBoxContainer::get_connection_rect(int io_num, int type, int /*flag*/, kRect* pos)
{
	//trace("uPluginBoxContainer::get_connection_rect()\n");
	// return iKXDSPWindow::get_connection_rect(io_num, flag, pos);
	
	switch (type)
	{
	case 0:
		CopyRect((LPRECT) pos, &pin_rect[io_num - 1]);
		pos->right = pos->left + 0;
		break;
	case 1:
		CopyRect((LPRECT) pos, &pin_rect[ipins + io_num - 1]);
		pos->left = pos->right - -2;
        break;
	default:
		return -1;
	}

    //trace(__FUNCTION__" f %08x, s %08x\n", flag, state);
    //trace("conn_rec %i, %i, %i, %i\n", pos->left, pos->top, pos->right, pos->bottom);

	return 0;
}

int uPluginBoxContainer::set_connector_size(int sz) 
{
    pin_size = sz + 1;
	return 0;
}

int uPluginBoxContainer::render(const char* name, int flag)
{
	// trace("uPluginBoxContainer::render()\n");
    state = flag;
	CBitmap texture;
	kxuCore* uCore = uGetCore();
	char capt[64];
	strcpy(capt, name);
	RECT capt_rect = {0, 0, 0, 0};
	int capt_length = int(strlen(capt));
	RECT pgmid_rect = {0, 0, 0, 0};
	char pgmid_str[8];
	sprintf(pgmid_str, "[%i]", plugin->pgm_id);
	int pgmid_length = int(strlen(pgmid_str));
	int i;
	int w, h, hx, y;
	const int pins = (ipins + opins);

	// dirty fix (kludge?) for renaming
	strcpy(plugin->name, name);

    #define HEADER_HEIGHT 17
	#define BOX_LEFT	  18
	int BOX_TOP = HEADER_HEIGHT; // + 2 * pin_size;  //FIXME - need to modify uPluginBoxView::OnEraseBkgnd()
	int PIN_HSPACE = 11 + 1 * pin_size;

	// prepare
	HDC dc = CreateCompatibleDC(NULL);
	HFONT old_font = (HFONT) SelectObject(dc, *(uCore->GetFont(UF_HEADERNARROW)));

	// boxview rect
	int wbybv = 0;
	int hbybv = 0;
	if (plugin->boxview)
	{
		LPRECT bvrect = &plugin->boxview->size;
		wbybv = bvrect->right + ((BOX_LEFT << 1) - 4);
		hbybv = bvrect->bottom + (BOX_TOP) + 0;
	}

    // width
    int no_pgmid = 0;
    if (wbybv)
    {
        no_pgmid = 1;
        capt_length = sprintf(capt, "%s %s", capt, pgmid_str);
    }
    GetTextExtentPoint32(dc, capt, capt_length, (LPSIZE) &capt_rect.right);
    w = capt_rect.right + 12 + 7 + 2;
    w = max(max(w, wbybv), 42);
	
	// height
    h = (ipins * PIN_HSPACE);
    hx = (opins * PIN_HSPACE);
    if (ipins >= opins) {y = h; h = hx; hx = y;}
    if (no_pgmid == 0)
    {
	    GetTextExtentPoint32(dc, pgmid_str, pgmid_length, (LPSIZE) &pgmid_rect.right);
	    h += pgmid_rect.bottom /* + 2 */;
    }
    if (h < hx) h = hx;
    h += HEADER_HEIGHT + 8 + 1;
    if (h < hbybv) h = hbybv;

    if (flag & MICROCODE_MINIMIZED) 
    {
        h = HEADER_HEIGHT;
        no_pgmid = 1;
    }
    
	// box
	int* Elements = new int[3 + pins];
	Elements[0] = UCB_BACK; Elements[1] = UCB_PLGBOX_HEADER; Elements[2] = UCB_PLGBOX;
	RECT tmp_rect[3] = 
	{
		{0, 0, w + SHADOW_SIZE, h + SHADOW_SIZE}, 
		{0, 0, w, HEADER_HEIGHT}, 
		{0, HEADER_HEIGHT, w, h}
	};
	MOVERECT(capt_rect, 7, 3);
	LPRECT* pRenderData = new LPRECT[3 + pins];
	for (i = 0; i < 3; i++) pRenderData[i] = &tmp_rect[i];
	RECT prect; uCore->GetElementData(UCB_PLGBOX_PIN, UED_SIZERECT, sizeof prect, &prect);

    prect.right  += 3 * min(1, pin_size);
    prect.bottom += 2 * pin_size;

    // fixme
    for (i = 0; i < ipins; i++) 
    {
	    Elements[i + 3] = UCB_PLGBOX_PIN;
	    SETRECT(pin_rect[i], -1, 1 + HEADER_HEIGHT + ((i + 1) * PIN_HSPACE) - prect.bottom, prect.right - 1, 1 + HEADER_HEIGHT + ((i + 1) * PIN_HSPACE));
	    pRenderData[i + 3] = &pin_rect[i];
    }
    for (i = 0; i < opins; i++) 
    {
	    Elements[ipins + i + 3] = UCB_PLGBOX_PIN;
	    SETRECT(pin_rect[ipins + i], w - prect.right + 1, 1 + HEADER_HEIGHT + ((i + 1) * PIN_HSPACE) - prect.bottom, w + 1, 1 + HEADER_HEIGHT + ((i + 1) * PIN_HSPACE));
	    pRenderData[ipins + i + 3] = &pin_rect[ipins + i];
    }
    uCore->GetControlBitmap(&bitmap, 0, 3 + pins, Elements, pRenderData);

    if (flag & MICROCODE_MINIMIZED)
    {
        const int pad = 3;
        int x = (HEADER_HEIGHT - 2*pad) / max(1, ipins);
        for (i = 0; i < ipins; i++)
        {
            pin_rect[i].left  += 1;
            pin_rect[i].right += 6;
            pin_rect[i].top = x * i + pad;
            pin_rect[i].bottom = pin_rect[i].top + x;
        }
        x = (HEADER_HEIGHT - 2*pad) / max(1, opins);
        for (i = ipins; i < pins; i++)
        {
            pin_rect[i].left  -= 6;
            pin_rect[i].right -= 1;
            pin_rect[i].top = x * (i - ipins) + pad;
            pin_rect[i].bottom = pin_rect[i].top + x;
        }
    }

	// shadow
	Elements[0] = UCB_SHADOW1;
	uCore->GetControlBitmap(&shadow, 0, 1, Elements, pRenderData);
	RESIZERECT(tmp_rect[0], 0, 0, -SHADOW_SIZE, -SHADOW_SIZE);

	// texture
	Elements[0] = UCB_TEXTURE;
	CopyRect(&tmp_rect[2], &tmp_rect[0]);
	uCore->GetControlBitmap(&texture, 0, 1, Elements, &pRenderData[2]);
	kxuApplyTexture((HBITMAP) bitmap, &tmp_rect[2], (HBITMAP) texture, 0, 0, NULL, 160);

	// caption
	HBITMAP old_bitmap = (HBITMAP) SelectObject(dc, bitmap);
	SetBkMode(dc, TRANSPARENT);
	SetTextColor(dc, 0x404040);
	DrawText(dc, capt, capt_length, &capt_rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	MOVERECT(capt_rect, -1, -1);
	SetTextColor(dc, 0xD0D0D0);
	DrawText(dc, capt, capt_length, &capt_rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

	#define PGMID_OFFSET 5
	// pgm id
	if (no_pgmid == 0)
	{
		pgmid_rect.top = h - pgmid_rect.bottom - PGMID_OFFSET;
		pgmid_rect.bottom = h - PGMID_OFFSET;
		if (ipins >= opins) pgmid_rect.left = w - pgmid_rect.right - PGMID_OFFSET;
		else pgmid_rect.left = PGMID_OFFSET;
		pgmid_rect.right = pgmid_rect.left + pgmid_rect.right;
		MOVERECT(pgmid_rect, 0, 0);
		SetTextColor(dc, 0x606060);
		DrawText(dc, pgmid_str, pgmid_length, &pgmid_rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		MOVERECT(pgmid_rect, 1, 1);
		SetTextColor(dc, 0xD0D0D0);
		DrawText(dc, pgmid_str, pgmid_length, &pgmid_rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	}

	// set box info 
	rect.right = tmp_rect[0].right;
    rect.bottom = tmp_rect[0].bottom;
	
	// pins
    if ((flag & MICROCODE_MINIMIZED) == 0)
    {
        for (i = 0; i < ipins; i++) {RESIZERECT(pin_rect[i], 1, -2, 6, 2);}
	    for (i = ipins; i < pins; i++) {RESIZERECT(pin_rect[i], -6, -2, -1, 2);}
	    SelectObject(dc, *(uCore->GetFont(UF_DEFAULT)));
	    char (*label)[8] = plugin->pgm_info.names;
	    for (i = 0; i < ipins; i++)
	    {
		    SETRECT(capt_rect, prect.right + 2, pin_rect[i].top - 1, w - prect.right - 2, pin_rect[i].bottom);
		    SetTextColor(dc, 0x404040);
		    DrawText(dc, label[i], 1, &capt_rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	    }
	    GetTextExtentPoint32(dc, "R", 1, (LPSIZE) &tmp_rect[1].right);
	    h = tmp_rect[1].right;
	    for (i = ipins; i < pins; i++)
	    {
		    capt_length = int(strlen(label[i]));
		    GetTextExtentPoint32(dc, &label[i][capt_length - 1], 1, (LPSIZE) &tmp_rect[2].right);
		    hx = tmp_rect[2].right;
		    y = 0;
		    if (hx < h) y = h - hx;
		    SETRECT(capt_rect, prect.right + 2, pin_rect[i].top - 1, w - prect.right - y + 1, pin_rect[i].bottom);
		    SetTextColor(dc, 0x404040);
		    DrawText(dc, label[i], capt_length, &capt_rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	    }
    }

	// cleanup
	SelectObject(dc, old_bitmap);
	SelectObject(dc, old_font);
	DeleteDC(dc);

	// disabled
	if ((flag & BOX_MICROCODE_MASK) != BOX_ENABLED)
	{
		Elements[0] = UCB_BLACKBACK;
		uCore->GetControlBitmap(&texture, 0, 1, Elements, &pRenderData[0]);
		kxuBlend(bitmap, &tmp_rect[0], texture, 0, 0, BLEND_MULTIPLY);
	}

	// boxview
	if (plugin->boxview)
	{
		uPluginBoxView* boxview = plugin->boxview;
		if (boxview->state == 0) 
		{
			boxview->open(); 
			boxview->state = 1;

			// synchronize boxview to plugin state
			int n = plugin->get_param_count();
			int v = 0;
			for (int i = 0; i < n; i++) 
			{
				plugin->get_param(i, &v);
				boxview->param_changed(i, v);
			}
		}
        int left = 2 + (w - boxview->size.right) / 2;
		boxview->MoveWindow(max(left, BOX_LEFT), BOX_TOP, boxview->size.right, boxview->size.bottom);
        boxview->redraw();
		boxview->ShowWindow((flag == BOX_ENABLED) ? SW_SHOW : SW_HIDE);
	}

    // arrow
    collapse->color(((flag & BOX_MICROCODE_MASK) == BOX_ENABLED) ? 0xD0D0D0 : 0x9C9C9C);
    collapse->state(flag & MICROCODE_MINIMIZED);
    collapse->MoveWindow(rect.right - 5 - 7, 7, 7, 4);

    //trace(__FUNCTION__" f %08x, s %08x\n", flag, state);

	// more cleanup
	delete [] Elements;
	delete [] pRenderData;
	return 0;
}

int uPluginBoxContainer::render_shadow()
{
	RECT sh_rect = {rect.left, rect.top, rect.right + SHADOW_SIZE, rect.bottom + SHADOW_SIZE};
	kxuBlend(bitmap, &sh_rect, shadow, 0, 0, BLEND_MULTIPLY); 
	return 0;
}