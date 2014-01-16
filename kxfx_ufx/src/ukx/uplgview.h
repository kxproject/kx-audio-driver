
#ifndef _U_PLUGINVIEW_H_
#define _U_PLUGINVIEW_H_

#define UCC_CONTROL_MASK	0x0000ffff
#define UCC_EVENT_MASK		0xffff0000

//.............................................................................

enum 
{
	UPVP_STYLE,
	UPVP_TEXTURE,
	UPVP_ICON,
	
	upvp_nprops
};

//.............................................................................
// uPluginView window

class uPluginView : public CWnd
{
public:
	uPluginView();
	virtual ~uPluginView();

public:
	virtual int open();
	virtual void close();

protected:
	friend class uPlugin;
	friend class uPluginContainer;

protected:
	uPlugin* plugin;
	uPluginContainer* pParentContainer;

	virtual int ucontrol_changed(int id, int data) {id = id; data = data; return true;};
	virtual int param_changed(int id, kxparam_t value) {id = id; value = value; return true;};

	int set_title(const char* title);
	int set_property(int id, int adata = 0, int bdata = 0);

	int set_bypass(int id);
	int set_mute(int id);

	int set_control_tooltip(uControl* control, const char* tip, int flags = 0) {return tooltip.SetTipString(control, tip, flags);}
	int printf_control_tooltip(uControl* control, LPCTSTR format, ...)
	{
		char msg[64];
		va_list arglist;
		va_start(arglist, format);
		vsprintf(msg, format, arglist);
		return set_control_tooltip(control, msg);
	}

protected:
	CBitmap bkgbitmap;
	RECT size;
	uToolTip tooltip;

	// virtual void __custom_draw_() {};
	
protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	// afx_msg BOOL OnToolTip(UINT /* id */, NMHDR *pnm, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT /* nFlags */, CPoint /* point */) {SetFocus();};
	afx_msg void OnRButtonDown(UINT /* nFlags */, CPoint /* point */) {SetFocus();};
	DECLARE_MESSAGE_MAP()

};

#endif // _U_PLUGINVIEW_H_
