
#ifndef _U_TOOLTIP_H_
#define _U_TOOLTIP_H_

//.............................................................................
// uToolTip styles

#define	UTT_DEFAULT		0
#define	UTT_AUTO		1
#define	UTT_CENTERTIP	2

class uToolTip
{
public:
	uToolTip();
	virtual ~uToolTip();
	
	int Create(DWORD dwStyle, HWND hparent);
	int SetTipString(uControl* control, const char* tip, int flags = UTT_DEFAULT);
	
protected:
	HWND handle;
	HWND hparent;
	TOOLINFO ti;
};


#endif // _U_TOOLTIP_H_
