/////////////////////////////////////////////////////////////////////////
// Vertical Slider - LeMury 2005
/////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ColorCtrl.h"
#include "FontCtrl.h"

//-----------------------------------------------------------------------------
class mySlider : public kSlider//kWindow
{
public:
	kSlider		slider;
	CColorCtrl<CFontCtrl<CStatic> > sval;	// slider value label
	CColorCtrl<CFontCtrl<CStatic> > slab;	// slider name label
	int wd, ht;								// bitmap widht and hight

		~mySlider(){};

	mySlider()
	{

	};

	// Load bitmamps 
	void setbitmaps(HBITMAP b1, HBITMAP b2, HBITMAP b3=NULL)
	{
		slider.set_bitmaps(	b1,		//slider background
							b2,		//thumb up
							b3);	//thumb down
	}

	//----------------------------------------------------------------------
	//              this,             id,      name,       font,        min, max,      left,    top
	void create(CKXPluginGUI* parent,UINT ID,char* tool,char* name, CFont* sfont,int min,int max,int left,int top,
				int w, int h, int lab_w, int lab_off=0)
	{
		wd=w;
		ht=h;
		parent->create_vslider(slider, ID, tool, min, max,  left, top, wd, ht);
		slider.set_method(kMETHOD_TRANS);

		// add value label
		int l = left + (wd - lab_w)+ lab_off;
		sval.Create("",WS_CHILD|WS_VISIBLE|SS_CENTER,
					CRect(CPoint(l,top-9),CSize(lab_w,10)), parent, NULL);
		sval.SetFont(sfont,NULL);				// set font
		sval.SetTextColor(RGB(200,200,200));	// set text color

		// add name label
		slab.Create("",WS_CHILD|WS_VISIBLE|SS_CENTER,
					CRect(CPoint(l,top+h-2),CSize(lab_w,10)), parent, NULL);
		slab.SetFont(sfont,NULL);				// set font
		slab.SetBkColor(RGB(123,0,0));	// set text color
		slab.SetWindowText(name);
		slab.ChangeFontStyle(FC_FONT_BOLD);
	};


	void setfont(CFont* sfont)	// Set Slider value's text font
	{
		sval.SetFont(sfont,NULL);				// set font

	};

	void setvalue(int val)	//set slider value
	{
		VSSETPOS(slider, val);
		slider.redraw();
	}


};
//--------------------------------------------------------------------------
/*
	//----------------------------------------------------------------------
	//              this,             id,      name,       min, max,      left, top, width, hight
	void create2(CKXPluginGUI* parent,UINT ID, char* name,int min,int max, int x, int y, int w, int h, BOOL style=true)
	{
		parent->create_vslider(slider, ID, name, min, max,  x, y, w, h);
		slider.set_method(kMETHOD_TRANS);

		// add value label
		sval.Create("",WS_CHILD | WS_VISIBLE | SS_CENTER,
					CRect(CPoint(x+5,y+14),CSize(w,12)), parent, NULL); //CPoint(x+6,y+13)
		sval.SetFont(&sfont,NULL);				// set font
		sval.SetTextColor(RGB(200,200,200));	// set text color
		sval.SetBkColor(RGB(37,37,37));			// set back color
		
		if(style)
		{
			// add name label
			slabel.Create("",WS_CHILD | WS_VISIBLE | SS_CENTER,
						CRect(CPoint(x,y+h+25),CSize(w,12)), parent, NULL);
			slabel.SetFont(&sfont,NULL);			// set font
			slabel.SetTextColor(RGB(241,159,3));	// set text color
			slabel.SetBkColor(RGB(37,37,37));		// set back color
			slabel.SetWindowText(name);
		}
	};
*/