
#ifndef _U_METER_H_
#define _U_METER_H_

//.............................................................................
// uMeter styles

#define UMS_DEFAULT	0

//.............................................................................
// uMeter Class

class uMeter : public uControl
{
public:
	uMeter();
	virtual ~uMeter();

public:
	int SetMeterBitmap(const CBitmap* bitmap, int x, int y, int offset);
	BOOL Create(DWORD dwStyle, const CBitmap* bkgbitmap, int x, int y, CWnd* pParentWnd, UINT nID);

	void SetLabel(unsigned int id, const char* l, int c);
	void SetValue(int v1, int v2, int h1, int h2);
	void start();
	void stop();
	
protected:

	// CBitmap bmpBox;
	CBitmap* bkgbitmap;
	CBitmap* bmp_meter;
	RECT meterrect[2];
	int value[2];
	int tail[2];
	int clip[2];
	char label[2][8];
	UINT_PTR htimer;

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* /* pDC */) {return 1;};
	afx_msg void OnTimer(UINT);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

#endif // _U_METER_H_

