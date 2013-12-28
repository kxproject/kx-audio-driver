//////////////////////////////////////////////////////////////
// MyMenu	LeMury
//////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ColorCtrl.h"
#include "FontCtrl.h"

class myMenu	: public kWindow
{
private:
public:
	CMenu			popmenu;				// our popup menu
	kCheckButton	butt;					// for button and tooltip
	kStatic			back;					// for holding textbox bitmap
	CColorCtrl<CFontCtrl<CStatic> > txtbox;	// displays selection
	HBITMAP			backbmp;
	int				nItems;					// number of items
	int				curr_sel;				// current selected item
	
	~myMenu()
	{
		popmenu.DestroyMenu();
	};
	myMenu()
	{
		nItems=0;
		curr_sel=0;
	};

	// Load  bitmaps and get their dimensions
	void setbitmaps(HBITMAP b1, HBITMAP b2, HBITMAP txtbox  )
	{
		butt.set_bitmaps(	b1,		//HBITMAP unchecked,
							b2,		//HBITMAP unchecked_over,
							b1,		//HBITMAP checked,
							b2);	//HBITMAP checked_over
		backbmp = txtbox;			// text box bitmap
	}

	// Create the Control
		//              this,             ID,      name, txt font,                   left,    top, width
	void create(CKXPluginGUI* parent,UINT ID, char* name,CFont* sfont,int num_items,
							int left, int top, int wd)
	{
		// add the kX static bitmap holder
		back.create("",left,top,backbmp,parent,kMETHOD_TRANS);
		back.show();

		// add the kX button
		parent->create_checkbox(butt, ID, name, left+wd, top, 20);
		butt.set_method(kMETHOD_TRANS);
		butt.set_check(0);
		butt.show();

		// add the text label
		txtbox.Create("",WS_CHILD|WS_VISIBLE|SS_LEFT,
					CRect(CPoint(left+5,top+4),CSize(wd-5,12)), parent, NULL);
		txtbox.SetFont(sfont,NULL);	
		//txtbox.SetTextColor(RGB(241,159,3));
		txtbox.SetBkColor(CLR_NONE);

		// create the popmenu
		nItems = num_items;
		popmenu.CreatePopupMenu();
		for(int i=0; i<nItems; i++)	// add items
		{
			char tmp[64];
			sprintf(tmp,"%d -- Item --", i);
			popmenu.AppendMenu(MF_STRING, i+100, tmp);
		}
		popmenu.CheckMenuItem(100,MF_CHECKED);	// check first item
		curr_sel=0;
	};

	//-----------------------------------------------------------------------------
	void set_itemtext(int id, char* itemtxt)	// set menu item text
	{
		popmenu.ModifyMenu(id+100,MF_BYCOMMAND|MF_STRING, id+100, itemtxt );
	};

	void set_value(int id)	// sets selected menu item
	{
		for(int i=0; i<nItems; i++) popmenu.CheckMenuItem(i+100,MF_UNCHECKED);
		popmenu.CheckMenuItem(id+100,MF_CHECKED);
		CString	txt;
		popmenu.GetMenuString(id+100, txt, 24 );
		back.redraw();
		butt.redraw();
		txtbox.SetWindowText(txt);
		curr_sel=id+100;
	};

	int track(CKXPluginGUI* parent)	// PopUp the menu and track it
	{
		POINT pp;
		kRect butt_rect;
		butt.get_rect_abs(butt_rect);
		pp.x = butt_rect.x1+1;
		pp.y = butt_rect.y2 - 1;

		int id = popmenu.TrackPopupMenu(TPM_LEFTBUTTON|TPM_RIGHTBUTTON|TPM_RETURNCMD,
										pp.x,pp.y,parent,0);
		// ret - 0; canceled
		// ret - 100 to 100+num_items; selection
		if(id!=0 && id!=curr_sel)
		{
			if(id<100 || id>(100+nItems)) return -1;	//error
			set_value(id-100);
			return id-100;
		}
		return -1;
	};

};
/////////////////////////////////////////////////////////////////////////////////////////
// Simple menu used for 'List/Launch FX plugins
/////////////////////////////////////////////////////////////////////////////////////////
class myMenu2	: public kWindow
{
private:
public:
	CMenu			popmenu;				// our popup menu
	kCheckButton	butt;					// for button and tooltip
	
	~myMenu2()
	{
		popmenu.DestroyMenu();
	};
	myMenu2()
	{

	};

	// Load bitmaps
	void setbitmaps(HBITMAP b1, HBITMAP b2=0, HBITMAP b3=0,HBITMAP b4=0)
	{
		butt.set_bitmaps(	b1,//HBITMAP unchecked,
							b3,//NULL,//HBITMAP unchecked_over,
							b2,//HBITMAP checked,
							b4);//NULL//HBITMAP checked_over
	}

	//              this,             ID,      name, txt font,       left,    top
	void create(CKXPluginGUI* parent,UINT ID, char* name,CFont* sfont, int left, int top)
	{
		// add the check switch
		parent->create_checkbox(butt, ID, name, left, top, 20);
		butt.set_method(kMETHOD_TRANS);
		butt.set_check(0);
		butt.show();
	};

	//-----------------------------------------------------------------------------
	void create()	// create the popmenu
	{		
		popmenu.CreatePopupMenu();
	};
	void add_item(int id, char* itemtxt)	// add menu item
	{
		popmenu.AppendMenu(MF_STRING, id+100, itemtxt);
	};
	void destroy()		// destroy menu
	{
		popmenu.DestroyMenu();
	};

	void set_itemtext(int id, char* itemtxt)	// set menu item text
	{
		popmenu.ModifyMenu(id+100,MF_BYCOMMAND|MF_STRING, id+100, itemtxt );
	};

	int track(CKXPluginGUI* parent)	// PopUp the menu and track it
	{
		POINT pp;
		kRect butt_rect;
		butt.get_rect_abs(butt_rect);
		pp.x = butt_rect.x1;
		pp.y = butt_rect.y2;

		int id = popmenu.TrackPopupMenu(TPM_LEFTBUTTON|TPM_RIGHTBUTTON|TPM_RETURNCMD,
										pp.x,pp.y,parent,0);
		// ret - 0; canceled
		// ret - 100 to 100+num_items; selection
		if(id!=0 )
		{
			//if(id<100 || id>(100+nItems)) return -1;	//error
			return id-100;
		}
		return -1;
	};

};
/////////////////////////////////////////////////////////////////////////////////////////