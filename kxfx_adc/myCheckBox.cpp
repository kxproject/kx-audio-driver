/////////////////////////////////////////////////////////////////////////
// skinned checkbox - LeMury 2005
/////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

//-----------------------------------------------------------------------------
class myCheckBox : public kButton
{
private:

public:
	kCheckButton	chk;

		~myCheckBox(){};

	myCheckBox()
	{};

	// Load bitmaps and get their dimensions
	void setbitmaps(HBITMAP b1, HBITMAP b2=0, HBITMAP b3=0,HBITMAP b4=0)
	{
		chk.set_bitmaps(	b1,//HBITMAP unchecked,
							b3,//NULL,//HBITMAP unchecked_over,
							b2,//HBITMAP checked,
							b4);//NULL//HBITMAP checked_over
	}

	//----------------------------------------------------------------------
	//              this,             id,      name,          left,    top, width
	void create(CKXPluginGUI* parent,UINT ID, char* name, int left, int top, int wd)
	{
		parent->create_checkbox(chk, ID, name, left, top, wd);
		chk.set_method(kMETHOD_TRANS);
		chk.set_check(0);
	};


};
//--------------------------------------------------------------------------
