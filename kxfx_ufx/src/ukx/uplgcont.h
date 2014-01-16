
#ifndef _U_PLUGINCONTAINER_H_
#define _U_PLUGINCONTAINER_H_

/////////////////////////////////////////////////////////////////////////////
// uPluginContainer dialog

class uPluginContainer : public CWnd, public iKXPluginGUI
{
// Construction
public:
	uPluginContainer(kDialog* pParent, uPluginView* plgview);
	virtual ~uPluginContainer();
	int create();

protected:
	virtual void PostNcDestroy();

// Implementation
protected:
	friend class uPluginView;
	friend class uPlugin;
	LRESULT uControlChanged(WPARAM id, LPARAM code);
	int BigInit();
	int BigInit2();

	kxuCore* uCore;

	CWnd* pParentWindow;
	HMENU hmenu;

	HICON	icon[2];
	CBitmap bkgbitmap;
	const char* title;
	char chbuf[256];

	RECT Size;
	RECT BkgSize;
	int CaptionBarHeight;
	HRGN MoveRegion;

	uPluginView* pluginview;
	uPlugin* plugin;
	uPresetManager presetman;

	int state;

	#define UPC_NOWIN			0x0001
	#define UPC_PRESET_LOCKED	0x0010
	#define UPC_BYPMUTE_LOCKED	0x0020

	int flags;
	
	#define UPC_FLAGS_DISABLECONT	0x01 // not yet implemented
	#define UPC_FLAGS_CUSTOMBKGND	0x02
	#define UPC_FLAGS_CUSTOMTEXTURE	0x04
	#define UPC_FLAGS_CUSTOMICON	0x08
	#define UPC_FLAGS_BYPASS		0x10
	#define UPC_FLAGS_MUTE			0x20
	#define UPC_FLAGS_NOPRESETS		0x40 // not yet implemented
	#define UPC_FLAGS_TOOLSIZE		0x80
	
	#define UPC_FLAGS_NOLABEL		0x200
	#define UPC_FLAGS_NOWIN			0x400

	UINT texture_id;
	UINT icon_id;

	int texture_depth;
	int bypass_id;
	int mute_id;

	uToolTip	tooltip;

	#define BASE_CONTROL_ID	0x200

	uLabel	lblTitle;
	RECT	rectTitle;
	#define LBL_TITLE_ID (BASE_CONTROL_ID + 1)

	uECombo cmbPreset;
	RECT	rectPreset;
	#define CMB_PRESET_ID (BASE_CONTROL_ID + 2)

	uCheckBox chkBypass;
	RECT	rectBypass;
	#define BTN_BYPASS_ID (BASE_CONTROL_ID + 3)

	uCheckBox chkMute;
	RECT	rectMute;
	#define BTN_MUTE_ID	 (BASE_CONTROL_ID + 4)

	uImgButton btnClose;
	CBitmap bmpClose;
	RECT	rectClose;
	#define BTN_CLOSE_ID (BASE_CONTROL_ID + 5)

	uImgButton btnMinimize;
	CBitmap bmpMinimize;
	RECT	rectMinimize;
	#define BTN_MINIMIZE_ID (BASE_CONTROL_ID + 6)

	uImgButton btnOnTop;
	CBitmap bmpOnTop;
	RECT	rectOnTop;
	#define BTN_ONTOP_ID (BASE_CONTROL_ID + 7)

private:
	void SwitchNakedWindow();
	int _import_presets_offset;

public:
	int param_changed(int id, kxparam_t value);
	int SetPresetCaption(LPCSTR Caption) {
		if (!(state & UPC_PRESET_LOCKED)) 
		{
			cmbPreset.SetWindowText(Caption);
		} 
		return 0;
	};

	// kxapi stuff
	void init_gui() {};
	void show_gui() {ShowWindow(SW_SHOW);};
	void hide_gui() {ShowWindow(SW_HIDE);};
	void close_gui() {DestroyWindow();};
	uintptr_t extended_gui(int command, uintptr_t p1, uintptr_t p2);

	// ...................
	int x, y;

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	// afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
	afx_msg UINT OnNcHitTest(CPoint point);
	// afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	// afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnToolTip(UINT id, NMHDR *pnm, LRESULT *pResult);
	afx_msg void OnSysCommand(UINT, LONG);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnMove(int x, int y);
	DECLARE_MESSAGE_MAP()

};

#endif // _U_PLUGINCONTAINER_H_
