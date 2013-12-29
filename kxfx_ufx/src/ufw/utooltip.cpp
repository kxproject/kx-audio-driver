
#include "ufw.h"
#include "utooltip.h"

extern HMODULE hmodule;
#define PluginHandle hmodule

//.............................................................................
// uToolTip

uToolTip::uToolTip() 
{
	handle = 0;
	hparent = 0;
};

uToolTip::~uToolTip() 
{
	
};

int uToolTip::Create(DWORD /*dwStyle*/, HWND hparent)
{
	uToolTip::hparent = hparent;
	handle = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
							WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
							CW_USEDEFAULT, CW_USEDEFAULT,
							CW_USEDEFAULT, CW_USEDEFAULT,
							hparent, NULL, PluginHandle,
							NULL);

	/* SetWindowPos(handle, HWND_TOPMOST, 0, 0, 0, 0,
             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE); */ // - hmm, fixme, forces parent to be TOPMOST too for some reason

	return 0;
}

int uToolTip::SetTipString(uControl* control, const char* tip, int flags)
{
	if (!handle) return -1;
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
	if (flags & UTT_CENTERTIP) ti.uFlags |= TTF_CENTERTIP;
	ti.hwnd   = hparent;
	ti.uId    = UINT_PTR(control->GetSafeHwnd());
	ti.hinst  = PluginHandle;
	ti.lpszText = 0;

	UINT msg = (::SendMessage(handle, TTM_GETTOOLINFO, 0, (LPARAM) &ti)) ? TTM_SETTOOLINFO : TTM_ADDTOOL;
	ti.lpszText  = (flags & UTT_AUTO) ? LPSTR_TEXTCALLBACK : (char*) tip;
	::SendMessage(handle, msg, 0, (LPARAM) &ti);

	return 0;
}