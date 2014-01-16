#ifndef BOXCONTROLS_H
#define BOXCONTROLS_H

// ok, here are buggy and dirty controls i've wrote in 2006 to meet some new kx features/issues
// i do not want to bother with original ufw controls anymore

class Arrow : public uControl
{
    CBitmap img[2];
    COLORREF c;

    uPluginBoxContainer* box;
    
public:
    Arrow()  {kxuRegisterWindowClass("UFX-BoxArrow");}
    ~Arrow() {CWnd::DestroyWindow();}

    void color(COLORREF c_) 
    {
        c = c_;
    }

    void state(int s) 
    {
        State = s ? UC_STATE_DEFAULT : UC_STATE_CHECKED;
    }
    
    BOOL create(DWORD dwStyle, int x, int y, int w, int h, 
        uPluginBoxContainer* box_, CWnd* parent, UINT id)
    {
        ASSERT(parent);
    	pParentWindow = parent;
        box = box_;
    	mID = id;
    
    	Style = dwStyle & UCS_CONTROL_MASK;
    	State = UC_STATE_DEFAULT;
    
    	SETRECT(Size, 0, 0, w, h);
    
    	RECT* rect = &Size;
	    int imgId = UCB_ARROW_SOUTH;
    	uCore->GetControlBitmap(&img[0], 0, 1, &imgId, &rect);
        imgId = UCB_ARROW_NORTH;
        uCore->GetControlBitmap(&img[1], 0, 1, &imgId, &rect);

        c = uCore->GetColor(_UC_WHITE);
    	
    	BOOL ret = CWnd::Create("UFX-BoxArrow", 0, dwStyle & UCS_WINDOW_MASK,
    							CRect(x, y, x + w, y + h),
    							parent, id);
    	return ret;    
    }
    
protected:
	afx_msg BOOL OnEraseBkgnd(CDC*) {return 1;}
	
	afx_msg void OnPaint() 
    {
        CPaintDC dc(this); 
        CDC tmpdc;
        if (!tmpdc.CreateCompatibleDC(&dc)) 
            return;

        CBrush brush;
        brush.CreateSolidBrush(c);
        CBrush* dcold = dc.SelectObject(&brush);
        CBitmap* tmpold = tmpdc.SelectObject(&img[State & UC_STATE_CHECKED]);

        dc.BitBlt(EXPANDRECTWH(Size), &tmpdc, 0, 0, 0x00B8074A);

        tmpdc.SelectObject(tmpold);
        dc.SelectObject(dcold);
    }
    
	afx_msg void OnLButtonDown(UINT /*nFlags*/, CPoint /*point*/) 
    {
	    State ^= UC_STATE_CHECKED;
        box->notify(mID, State & UC_STATE_CHECKED);
    }

	// afx_msg void OnLButtonUp(UINT nFlags, CPoint point) {}
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(Arrow, uControl)
//{{AFX_MSG_MAP(uCheckBox)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	// ON_WM_LBUTTONUP()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#endif // BOXCONTROLS_H