
#ifndef _U_DRAWBOX_H_
#define _U_DRAWBOX_H_

//.............................................................................
// uDrawBox styles

#define UDBS_BOX				0x0100

//.............................................................................
// uDrawBox Class

class uDrawBox : public uControl
{
public:
	uDrawBox();
	virtual ~uDrawBox();

public:
	int SetRegionPoints(LPPOINT pts, int npoints, int pts_shift = 0);
	int SetShapeFore(const CBitmap* bitmap, int x, int y, int w, int h);
	// BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
	// {return Create(dwStyle, EXPANDRECTWH(rect), pParentWnd, nID);}
	// BOOL Create(DWORD dwStyle, int x, int y, int w, int h, CWnd* pParentWnd, UINT nID);
	BOOL Create(DWORD dwStyle, const CBitmap* bkgbitmap, int x, int y, int w, int h, CWnd* pParentWnd, UINT nID);
	
protected:
	int BoxBorder;

	// CBitmap bmpBox;
	CBitmap* bkgbitmap;
	CBitmap* bmp_shapefore;
	CBitmap bmp_predraw;
	RECT shaperect;
	HRGN rgn;
	LPPOINT pts;
	int npoints;
	int pts_shift;

	int PrepareMask();

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* /* pDC */) {return 1;};
	DECLARE_MESSAGE_MAP()
};

#endif // _U_DRAWBOX_H_

