
#ifndef _U_PLUGINBOXVIEW_H_
#define _U_PLUGINBOXVIEW_H_

//.............................................................................
// uPluginBoxView window

class uPluginBoxView : public CWnd
{
// Construction
public:
	uPluginBoxView();
	virtual ~uPluginBoxView();

// Implementation
public:
	void destroy() {close(); DestroyWindow();};

	virtual int open();
	virtual void close();
    virtual int redraw();

protected:
	friend class uPlugin;
	friend class uPluginBoxContainer;

protected:
	uPlugin* plugin;
	uPluginBoxContainer* container;

	CBitmap bkgbitmap;
	RECT size;

	uToolTip tooltip;
	int set_control_tooltip(uControl* control, const char* tip, int flags = 0) {return tooltip.SetTipString(control, tip, flags);}
	int printf_control_tooltip(uControl* control, LPCTSTR format, ...)
	{
		char msg[64];
		va_list arglist;
		va_start(arglist, format);
		vsprintf(msg, format, arglist);
		return set_control_tooltip(control, msg);
	}

	int state;
	
	virtual int ucontrol_changed(int id, int data) {id = id; data = data; return true;};
	virtual int param_changed(int id, kxparam_t value) {id = id; value = value; return true;};
	
protected:
	virtual void PostNcDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()

};

#endif // _U_PLUGINBOXVIEW_H_
