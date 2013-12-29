
#ifndef _U_WND_H_
#define _U_WND_H_

//.............................................................................
// uWnd window

class uWnd : public CWnd
{

public:
	uWnd();
	virtual ~uWnd();

public:
	BOOL CreateEx( DWORD dwExStyle, LPCTSTR lpszWindowName, DWORD dwStyle, 
		int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu, LPVOID lpParam = NULL);

protected:
	CBitmap bkgbitmap;
	RECT size;
	kxuCore* uCore;
	HWND hParentWindow;

	void PrepareBckgnd();

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
};

#endif // _U_WND_H_
