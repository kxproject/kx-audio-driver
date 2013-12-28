////////////////////////////////////////////////////////////////////////////////
// kX 3538 Skin wrapper class - LeMury 2005
//
// This will 'skin' all default kX plugin items and 'draw' the background.
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ColorCtrl.h"
#include "FontCtrl.h"
#include "resource.h"

#define MAXSEP	100		// max num of sep lines

void dbg(const char *format, ...);
//-------------------------------------------------------------------------
struct lKader
{
	int left;	// left
	int top;	// top
	
};

class mySkin	: public kDialog//: public kWindow
{
private:

int	bw, bh;					// frame width, hight

// fonts and statics to replace default kX items
CFont cardfont;				// caption header font
CFont logofont;				// logo font
CFont sliderfont;			// slider value font
CColorCtrl<CFontCtrl<CStatic> > fxlogo;		// Bold fx name
CColorCtrl<CFontCtrl<CStatic> > cardlbl;	// card label

HICON hmyicon;

public:
kColor	txtcol;
COLORREF skin_color;	// color extracted from skin

CBitmap backmap;	// our background Image made from tiles
CBitmap bclose,bclose_o;
CBitmap bmini,bmini_o;
CBitmap breset,breset_o;
CBitmap bmute,bmute_o,bmute_s,bmute_s_o;
CBitmap bbypass,bbypass_o,bbypass_s,bbypass_s_o;


int sepline[MAXSEP];	// array of x -coordinate of lines
int sep_cnt;			// num of sep lines in use
lKader kaders[20];
int kad_cnt;			// num kaders in use
//-----------------------------------------------------
~mySkin()	// destructor
{
	DeleteObject(hmyicon);
	backmap.DeleteObject();
	bclose.DeleteObject();
	bclose_o.DeleteObject();
	bmini.DeleteObject();
	bmini_o.DeleteObject();
	breset.DeleteObject();
	breset_o.DeleteObject();
	bmute.DeleteObject();
	bmute_o.DeleteObject();
	bmute_s.DeleteObject();
	bmute_s_o.DeleteObject();
	bbypass.DeleteObject();
	bbypass_o.DeleteObject();
	bbypass_s.DeleteObject();
	bbypass_s_o.DeleteObject();
};

//-----------------------------------------------------
mySkin()	// constructor
{
	bw=bh=0;
	sep_cnt=0;
	kad_cnt=0;
	txtcol	= RGB(135,135,135);
};
//-------------------------------------------------------------------------------
int addBoxline(int left_x)	// adds one box separation line.
{
	if(sep_cnt>=MAXSEP) return -1;	// any boxes left or out of range?
	sepline[sep_cnt] = left_x;
	++sep_cnt;
	return 0;
};

int addKader(int left, int top)	// adds one kader
{
	if(kad_cnt>=MAXSEP) return -1;	// any kaders left?
	kaders[kad_cnt].left = left;
	kaders[kad_cnt].top = top;
	++kad_cnt;
	return 0;
};
//-----------------------------------------------------------------------------
void skin_frame(CKXPluginGUI* parent,CWnd *pWnd, int width, int hight,HMODULE hmodule,
				 char *plugname,char *devicename)
{
	bw=width;
	bh=hight;

	// Load Custom Icon and use it
	hmyicon	=LoadIcon(hmodule, MAKEINTRESOURCE(IDI_ICON1) );
	parent->SetIcon(hmyicon, false);

	//*** Make background bitmap ***//

	// Load background skin parts image file
	HBITMAP hskin	=LoadBitmap(hmodule, MAKEINTRESOURCE(IDB_BACKRED) );
	HBITMAP hbuttons=LoadBitmap(hmodule, MAKEINTRESOURCE(IDB_BUTTONS) );

	CPaintDC dc(pWnd);

	CDC tempDC;
	CBitmap skinbmp;
	skinbmp.Attach(hskin);
	tempDC.CreateCompatibleDC(&dc);
	tempDC.SelectObject( &skinbmp );	// source bitmap

	skin_color = GetPixel(tempDC, 5,5);	// Get skin color from bitmap

	CDC imageDC;
	imageDC.CreateCompatibleDC( &dc );
	backmap.CreateCompatibleBitmap( &dc, bw, bh );
	imageDC.SelectObject( &backmap );	// destination bitmap

	// copy the background tiles into the memory bitmap
	int nrows = (bh/10)+1;	// 10 pixels
	int ncols = (bw/10)+1;
	int vy = 66;			// version offset

	for(int c=0; c<nrows; c++)
	{
		for(int i=0; i<ncols; i++)
		{
			imageDC.BitBlt(	i*10, c*10,		// x,y coordinate of destination rectangle's upper-left 
							10,10,			//width and hight of destination rectangle
							&tempDC,
							4,vy+4,			//x,y coordinate of source rectangle's upper-left 
							SRCCOPY);		// raster operation code
		}
	}

	// copy left and right side lines
	for(int i=0; i<nrows; i++)
	{
		// left side
		imageDC.BitBlt(	0,(i*10),	// x,y coordinate of destination rectangle's upper-left 
						4,10,		//width and hight of destination rectangle
						&tempDC,
						0,vy+25,		//x,y coordinate of source rectangle's upper-left 
						SRCCOPY);
		// right side
		imageDC.BitBlt(	bw-5,(i*10),	// x,y coordinate of destination rectangle's upper-left 
						4,10,		//width and hight of destination rectangle
						&tempDC,
						29,vy+25,		//x,y coordinate of source rectangle's upper-left 
						SRCCOPY);
	}

	// copy the Bar tiles
	for( int i=0; i<ncols; i++)
	{
		// top bar
		imageDC.BitBlt(	(i*10),0,	// x,y coordinate of destination rectangle's upper-left 
						10,26,		//width and hight of destination rectangle
						&tempDC,
						5,vy,		//x,y coordinate of source rectangle's upper-left 
						SRCCOPY);
		// bottom bar
		imageDC.BitBlt(	(i*10),bh-29,// x,y coordinate of destination rectangle's upper-left 
						10,28,		//width and hight of destination rectangle
						&tempDC,
						5,vy+37,		//x,y coordinate of source rectangle's upper-left 
						SRCCOPY);	
	}

	// copy top left corner
		imageDC.BitBlt(	0,0,	// x,y coordinate of destination rectangle's upper-left 
						4,26,		//width and hight of destination rectangle
						&tempDC,
						0,vy,		//x,y coordinate of source rectangle's upper-left 
						SRCCOPY);	
	// copy bottom left corner
		imageDC.BitBlt(	0,bh-29,	// x,y coordinate of destination rectangle's upper-left 
						4,28,		//width and hight of destination rectangle
						&tempDC,
						0,vy+37,		//x,y coordinate of source rectangle's upper-left 
						SRCCOPY);
	// copy top right corner
		imageDC.BitBlt(	bw-5,0,	// x,y coordinate of destination rectangle's upper-left 
						4,26,		//width and hight of destination rectangle
						&tempDC,
						29,vy,		//x,y coordinate of source rectangle's upper-left 
						SRCCOPY);
	// copy bottom right corner
		imageDC.BitBlt(	bw-5,bh-29,	// x,y coordinate of destination rectangle's upper-left 
						4,26,		//width and hight of destination rectangle
						&tempDC,
						29,vy+37,		//x,y coordinate of source rectangle's upper-left 
						SRCCOPY);

	// add separation lines if any
	if(sep_cnt!=0)
	{
		for(int i=0; i<sep_cnt; i++)
		{
			int x = sepline[i]; // get left offset
			 if( x >=bw) break;
			//add top separator T-piece
			imageDC.BitBlt(	x-2,21,	// x,y coordinate of destination rectangle's upper-left 
							4,20,		//width and hight of destination rectangle
							&tempDC,
							19,vy+21,		//x,y coordinate of source rectangle's upper-left 
							SRCCOPY);
			//add bottom separator T-piece
			imageDC.BitBlt(	x-2,(bh-21)-20,	// x,y coordinate of destination rectangle's upper-left 
							4,25,		//width and hight of destination rectangle
							&tempDC,
							19,vy+25,		//x,y coordinate of source rectangle's upper-left 
							SRCCOPY);
			
			//add seperator lines
			int h=30;
			while( h<(bh-40) )
			{
				imageDC.BitBlt(	x-2,h,	// x,y coordinate of destination rectangle's upper-left 
								4,10,		//width and hight of destination rectangle
								&tempDC,
								19,vy+25,		//x,y coordinate of source rectangle's upper-left
								SRCCOPY);
				h+=10;
			}			
		}
	}

	// add kaders if any
	if(kad_cnt!=0)
	{
		for(int i=0; i<kad_cnt; i++)
		{
			int x = kaders[i].left; // get left offset
			int y = kaders[i].top;
			if( x >=bw || y>=bh) break;

			//dbg("kader %d: %d,%d",i,x,y);

			// add it
			imageDC.BitBlt(	x,y,	// x,y coordinate of destination rectangle's upper-left 
							25,13,		//width and hight of destination rectangle
							&tempDC,
							1,134,		//x,y coordinate of source rectangle's upper-left 
							SRCCOPY);	
		}
	}



	skinbmp.Detach();
	skinbmp.DeleteObject();	// delete source bitmap when done

	// make default buttons

	CBitmap butmap;
	butmap.Attach(hbuttons);			// buttons source bitmap
	tempDC.SelectObject( &butmap );

	bclose.CreateCompatibleBitmap( &dc, 19, 15 );	// size off button
	imageDC.SelectObject( &bclose );					// destination bitmap
	imageDC.BitBlt(	0,0,	// x,y coordinate of destination rectangle's upper-left 
					19,15,		//width and hight of destination rectangle
					&tempDC,
					1,65,		//x,y coordinate of source rectangle's upper-left
					SRCCOPY);
	bclose_o.CreateCompatibleBitmap( &dc, 19, 15 );	// size off button
	imageDC.SelectObject( &bclose_o );					// destination bitmap
	imageDC.BitBlt(	0,0,	// x,y coordinate of destination rectangle's upper-left 
					19,15,		//width and hight of destination rectangle
					&tempDC,
					21,65,		//x,y coordinate of source rectangle's upper-left
					SRCCOPY);

	bmini.CreateCompatibleBitmap( &dc, 19, 15 );	// size off button
	imageDC.SelectObject( &bmini );					// destination bitmap
	imageDC.BitBlt(	0,0,	// x,y coordinate of destination rectangle's upper-left 
					19,15,		//width and hight of destination rectangle
					&tempDC,
					41,65,		//x,y coordinate of source rectangle's upper-left
					SRCCOPY);
	bmini_o.CreateCompatibleBitmap( &dc, 19, 15 );	// size off button
	imageDC.SelectObject( &bmini_o );					// destination bitmap
	imageDC.BitBlt(	0,0,	// x,y coordinate of destination rectangle's upper-left 
					19,15,		//width and hight of destination rectangle
					&tempDC,
					61,65,		//x,y coordinate of source rectangle's upper-left
					SRCCOPY);

	breset.CreateCompatibleBitmap( &dc, 19, 15 );	// size off button
	imageDC.SelectObject(&breset);					// destination bitmap
	imageDC.BitBlt(	0,0,	// x,y coordinate of destination rectangle's upper-left 
					19,15,		//width and hight of destination rectangle
					&tempDC,
					1,97,		//x,y coordinate of source rectangle's upper-left
					SRCCOPY);
	breset_o.CreateCompatibleBitmap( &dc, 19, 15 );	// size off button
	imageDC.SelectObject(&breset_o );					// destination bitmap
	imageDC.BitBlt(	0,0,	// x,y coordinate of destination rectangle's upper-left 
					19,15,		//width and hight of destination rectangle
					&tempDC,
					21,97,		//x,y coordinate of source rectangle's upper-left
					SRCCOPY);

	bmute.CreateCompatibleBitmap( &dc, 19, 15 );	// size off button
	imageDC.SelectObject(&bmute);					// destination bitmap
	imageDC.BitBlt(	0,0,	// x,y coordinate of destination rectangle's upper-left 
					19,15,		//width and hight of destination rectangle
					&tempDC,
					41,113,		//x,y coordinate of source rectangle's upper-left
					SRCCOPY);
	bmute_o.CreateCompatibleBitmap( &dc, 19, 15 );	// size off button
	imageDC.SelectObject(&bmute_o );					// destination bitmap
	imageDC.BitBlt(	0,0,	// x,y coordinate of destination rectangle's upper-left 
					19,15,		//width and hight of destination rectangle
					&tempDC,
					61,113,		//x,y coordinate of source rectangle's upper-left
					SRCCOPY);
	bmute_s.CreateCompatibleBitmap( &dc, 19, 15 );	// size off button
	imageDC.SelectObject(&bmute_s);					// destination bitmap
	imageDC.BitBlt(	0,0,	// x,y coordinate of destination rectangle's upper-left 
					19,15,		//width and hight of destination rectangle
					&tempDC,
					81,113,		//x,y coordinate of source rectangle's upper-left
					SRCCOPY);
	bmute_s_o.CreateCompatibleBitmap( &dc, 19, 15 );	// size off button
	imageDC.SelectObject(&bmute_s_o );					// destination bitmap
	imageDC.BitBlt(	0,0,	// x,y coordinate of destination rectangle's upper-left 
					19,15,		//width and hight of destination rectangle
					&tempDC,
					101,113,		//x,y coordinate of source rectangle's upper-left
					SRCCOPY);

	bbypass.CreateCompatibleBitmap( &dc, 19, 15 );	// size off button
	imageDC.SelectObject(&bbypass);					// destination bitmap
	imageDC.BitBlt(	0,0,	// x,y coordinate of destination rectangle's upper-left 
					19,15,		//width and hight of destination rectangle
					&tempDC,
					41,97,		//x,y coordinate of source rectangle's upper-left
					SRCCOPY);
	bbypass_o.CreateCompatibleBitmap( &dc, 19, 15 );	// size off button
	imageDC.SelectObject(&bbypass_o );					// destination bitmap
	imageDC.BitBlt(	0,0,	// x,y coordinate of destination rectangle's upper-left 
					19,15,		//width and hight of destination rectangle
					&tempDC,
					61,97,		//x,y coordinate of source rectangle's upper-left
					SRCCOPY);
	bbypass_s.CreateCompatibleBitmap( &dc, 19, 15 );	// size off button
	imageDC.SelectObject(&bbypass_s);					// destination bitmap
	imageDC.BitBlt(	0,0,	// x,y coordinate of destination rectangle's upper-left 
					19,15,		//width and hight of destination rectangle
					&tempDC,
					81,97,		//x,y coordinate of source rectangle's upper-left
					SRCCOPY);
	bbypass_s_o.CreateCompatibleBitmap( &dc, 19, 15 );	// size off button
	imageDC.SelectObject(&bbypass_s_o );					// destination bitmap
	imageDC.BitBlt(	0,0,	// x,y coordinate of destination rectangle's upper-left 
					19,15,		//width and hight of destination rectangle
					&tempDC,
					101,97,		//x,y coordinate of source rectangle's upper-left
					SRCCOPY);

	butmap.Detach();
	butmap.DeleteObject();	// delete temp map


	// create some custom fonts
	logofont.CreatePointFont(85,"Tahoma");
	cardfont.CreatePointFont(80,"Tahoma");
	sliderfont.CreatePointFont(70,"Tahoma");

	// **** Change default kX items ********
	parent->w_label.destroy();	// kill card info label
	parent->grpbox.destroy();	// kill kX 'group' control
	parent->b_close.hide();		// temporarly hide the next items
	parent->b_minimize.hide();
	parent->b_mute.hide();
	parent->b_reset.hide();
	parent->preset.hide();
	parent->b_bypass.hide();

	int bw2 = bw-26;
	parent->b_close.reposition(		kRect(bw2,2, bw2+20,20)			);
	parent->b_close.set_bitmaps(bclose,bclose_o,bclose_o);

	parent->b_minimize.reposition(	kRect(bw2-20,2,(bw2-20)+20,20)	);
	parent->b_minimize.set_bitmaps(bmini,bmini_o,bmini_o);

	int bh2 = bh-20;
	parent->b_reset.reposition(		kRect(5 ,bh2 , 5+20, bh2+15)	);
	parent->b_reset.set_bitmaps(breset,breset_o,breset_o);

	parent->b_mute.reposition(		kRect(5+20 ,bh2,5+40,bh2+15)	);
	parent->b_mute.set_bitmaps(bmute,bmute_o,bmute_s,bmute_s_o);

	parent->b_bypass.reposition(	kRect(5+40 ,bh2 , 5+60,bh2+15)	);
	parent->b_bypass.set_bitmaps(bbypass,bbypass_o,bbypass_s,bbypass_s_o);

	parent->preset.reposition(		kRect(68 ,bh2+1, bw-20,bh2+15)	);
	parent->preset.set_font(cardfont);	// change 'preset button' font

	// this does not work. colors are private
/*	parent->preset.changecolors(skin_color,		// bk
								skin_color,		// fg
								skin_color,		// text_bk
								RGB(150,150,150),	// text_fg
								skin_color,		// bk_o
								skin_color,		// fg_o
								skin_color,		// text_bk_o
								RGB(200,200,200) );	// text_fg_o
*/

	// re-show default items
	parent->b_close.show();
	parent->b_minimize.show();
	parent->b_reset.show();
	parent->b_mute.show();
	parent->preset.show();
//	parent->b_bypass.show();


#define FXNAMEWIDTH	100
#define FXNAMELEFT	9
	// Effect name 'logo' Bold
	char tmp[MAX_LABEL_STRING];
	fxlogo.Create(	plugname ,WS_CHILD|WS_VISIBLE|SS_LEFT,
					CRect(CPoint(FXNAMELEFT,4),CSize(FXNAMEWIDTH,15)), parent, 0);
	fxlogo.SetBkColor(CLR_NONE);
	fxlogo.SetTextColor(RGB(187,184,176)); // light  Silver = RGB(187,184,176)
	fxlogo.SetFont(&logofont,NULL);
	fxlogo.ChangeFontStyle(FC_FONT_BOLD);

	int len = fxlogo.GetWindowTextLength();
	len = FXNAMELEFT+(len*8);

	// Card label
	cardlbl.Create("",WS_CHILD | WS_VISIBLE | SS_LEFT ,CRect(CPoint(len,5),CSize(150,14)), parent, 0);
	cardlbl.SetBkColor(CLR_NONE);
	cardlbl.SetTextColor(txtcol);
	cardlbl.SetFont(&cardfont,NULL);
	sprintf(tmp, " - %s",  devicename);
	cardlbl.SetWindowText(tmp);
	
	// change 'tooltip' font
	kFont toolf;
	toolf.set_font(this,logofont);
	parent->set_tool_font(toolf);


};


};
////////////////////////////////////////////////////////////////////////////////
