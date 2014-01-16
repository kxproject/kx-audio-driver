
#ifndef _U_SKIN_H_
#define _U_SKIN_H_
#include "../rsrc/ufxrc.rc.h"

//................................................................................

#define SIZEDATA(width, height) width, height
#define POINTDATA(left, top) left, top
#define MARGIN(left, top, right, bottom) left, top, right, bottom
#define MARGINWH(left, top, width, height) left, top, (left + width), (top + height)
#define SIZERECT(width, height) 0, 0, width, height
#define RECTDATA(left, top, right, bottom) left, top, right, bottom
#define RECTDATAWH(left, top, width, height) left, top, (left + width), (top + height)
#define NULLRECT 0, 0, 0, 0

//................................................................................

const PointFont uFonts[] =
{
	{-11, FW_REGULAR, "Tahoma"},
	{-11, FW_BOLD, "Verdana"},
	{-10, FW_REGULAR, "Tahoma"},
	{-10, FW_BOLD, "Verdana"},
	{-11, FW_BOLD, "Tahoma"},
	{-9, FW_REGULAR, "Small Fonts"},
};

const COLORREF uColors[] =
{
	0xe0e0e0,
	0xb0b0b0,
	0x909090,
	0x606060,
	0x303030,
	0xB0ffff,
	0xB0ffB0,
	0xB0B0ff
};

const BitmapElement uElements[] = 
{
	/* main background */
	{uColors[_UC_GREY], BET_SOLID},
	/* black background */
	{0xc0c0c0, BET_SOLID},
	/* texture */		
	{IDB_TEXTURE, BET_PLAINBITMAP | BET_DEFAULT, SIZERECT(256, 192)},
	/* frame bevel */	
	{IDB_BSKIN, BET_MARGINBITMAP | BET_DEFAULT, RECTDATAWH(4, 102, 17, 17), MARGINWH(9, 107, 7, 7)},
	/* caption bar right */	
	{IDB_BSKIN, BET_MARGINBITMAP | BET_DEFAULT, RECTDATAWH(73, 3, 41, 23), MARGINWH(78, 11, 28, 5)},
	/* caption bar left */	
	{IDB_BSKIN, BET_PLAINBITMAP | BET_DEFAULT, RECTDATAWH(50, 3, 23, 23)},
	/* screw */	
	{IDB_BSKIN, BET_PLAINBITMAP | BET_DEFAULT, RECTDATAWH(76, 54, 15, 15)},
	/* outer sunken */	
	{IDB_BSKIN, BET_MARGINBITMAP | BET_DEFAULT, RECTDATAWH(2, 7, 12, 8), MARGINWH(4, 9, 8, 4), 1},
	/* outer raised */	
	{IDB_BSKIN, BET_MARGINBITMAP | BET_DEFAULT, RECTDATAWH(16, 7, 12, 8), MARGINWH(18, 9, 8, 4), 1},
	/* box white */	
	{IDB_BSKIN, BET_MARGINBITMAP | BET_DEFAULT, RECTDATAWH(38, 29, 10, 10), MARGINWH(44, 35, 3, 3), 1},
	/* arrow south */	
	{IDB_BSKIN, BET_PLAINBITMAP | BET_DEFAULT, RECTDATAWH(41, 4, 7, 4)},
	/* arrow east */	
	{IDB_BSKIN, BET_PLAINBITMAP | BET_DEFAULT, RECTDATAWH(37, 4, 4, 7)},
	/* arrow north */	
	{IDB_BSKIN, BET_PLAINBITMAP | BET_DEFAULT, RECTDATAWH(41, 7, 7, 4)},
	/* arrow west */	
	{IDB_BSKIN, BET_PLAINBITMAP | BET_DEFAULT, RECTDATAWH(34, 4, 4, 7)},
	/* button raised */	
	{IDB_BSKIN, BET_MARGINBITMAP | BET_DEFAULT, RECTDATAWH(2, 29, 10, 10), MARGINWH(4, 31, 6, 6), 1},
	/* button sunken */	
	{IDB_BSKIN, BET_MARGINBITMAP | BET_DEFAULT, RECTDATAWH(2, 17, 10, 10), MARGINWH(4, 19, 6, 6), 1},
	/* lamp on */	
	{IDB_BSKIN, BET_PLAINBITMAP | BET_DEFAULT, RECTDATAWH(38, 13, 10, 6), NULLRECT, 1},
	/* lamp off */	
	{IDB_BSKIN, BET_PLAINBITMAP | BET_DEFAULT, RECTDATAWH(38, 21, 10, 6), NULLRECT, 1},
	/* close */	
	{IDB_BSKIN, BET_FILMBITMAP | BET_DEFAULT, RECTDATAWH(4, 79, 64, 16), NULLRECT, 0, 4},
	/* minimize */	
	{IDB_BSKIN, BET_FILMBITMAP | BET_DEFAULT, RECTDATAWH(4, 61, 64, 16), NULLRECT, 0, 4},
	/* on top */	
	{IDB_BSKIN, BET_FILMBITMAP | BET_DEFAULT, RECTDATAWH(4, 43, 64, 16), NULLRECT, 0, 4},
	/* label bypass */	
	{IDB_BSKIN, BET_PLAINBITMAP | BET_DEFAULT, RECTDATAWH(72, 87, 34, 11)},
	/* label mute */	
	{IDB_BSKIN, BET_PLAINBITMAP | BET_DEFAULT, RECTDATAWH(72, 97, 25, 10)},
	/* label preset */	
	{IDB_BSKIN, BET_PLAINBITMAP | BET_DEFAULT, RECTDATAWH(72, 76, 33, 11)},
	/* knob -180°...+165° */
	{IDB_KNOB, BET_FILMBITMAP | BET_DEFAULT, RECTDATA(0, 0, 1152, 48), NULLRECT, 0, 24},
	/* knob mask */
	{IDB_KNOB, BET_PLAINBITMAP | BET_DEFAULT, RECTDATAWH(1152, 0, 48, 48)},
	/* knob small -180°...+165° */
	{IDB_KNOBS, BET_FILMBITMAP | BET_DEFAULT, RECTDATA(0, 0, 768, 32), NULLRECT, 0, 24},
	/* knob small mask */
	{IDB_KNOBS, BET_PLAINBITMAP | BET_DEFAULT, RECTDATAWH(768, 0, 32, 32)},
	/* fader box */
	{IDB_BSKIN, BET_MARGINBITMAP | BET_DEFAULT, RECTDATAWH(51, 28, 12, 12), MARGINWH(58, 35, 3, 3), 2},
	/* vfader button */
	{IDB_BSKIN, BET_MARGINBITMAP | BET_FIXEDHEIGHT, RECTDATAWH(98, 36, 18, 35), MARGINWH(106, 36, 6, 35)},
	/* vfader switch */
	{IDB_BSKIN, BET_MARGINBITMAP | BET_FIXEDHEIGHT, RECTDATAWH(88, 115, 12, 18), MARGINWH(94, 119, 2, 6)},
	/* vfader line */
	{IDB_BSKIN, BET_MARGINBITMAP | BET_FIXEDWIDTH, RECTDATAWH(93, 28, 3, 12), MARGINWH(93, 30, 3, 8)},
	/* vfader line bottom shadow*/
	{IDB_BSKIN, BET_PLAINBITMAP | BET_DEFAULT, RECTDATAWH(80, 44, 3, 5)},
	/* vfader line top shadow*/
	{IDB_BSKIN, BET_PLAINBITMAP | BET_DEFAULT, RECTDATAWH(80, 47, 3, 1)},
	/* hfader button */
	{IDB_BSKIN, BET_MARGINBITMAP | BET_FIXEDWIDTH, RECTDATAWH(33, 100, 35, 18), MARGINWH(33, 109, 35, 5)},
	/* hfader line */
	{IDB_BSKIN, BET_MARGINBITMAP | BET_FIXEDHEIGHT, RECTDATAWH(77, 71, 12, 3), MARGINWH(79, 71, 8, 3)},
	/* hfader line right shadow*/
	{IDB_BSKIN, BET_PLAINBITMAP | BET_DEFAULT, RECTDATAWH(41, 127, 5, 3)},
	/* hfader line left shadow*/
	{IDB_BSKIN, BET_PLAINBITMAP | BET_DEFAULT, RECTDATAWH(44, 127, 1, 3)},
	/* plgbox */
	{IDB_BSKIN, BET_MARGINBITMAP | BET_DEFAULT, RECTDATAWH(73, 120, 12, 12), MARGINWH(76, 123, 6, 6)},
	/* plgbox header */
	{IDB_BSKIN, BET_MARGINBITMAP | BET_DEFAULT, RECTDATAWH(73, 108, 12, 12), MARGINWH(76, 111, 6, 6)},
	/* plgbox pin */
	// {IDB_BSKIN, BET_PLAINBITMAP | BET_DEFAULT, RECTDATAWH(98, 98, 7, 4)},
    {IDB_BSKIN, BET_MARGINBITMAP | BET_DEFAULT, RECTDATAWH(98, 98, 7, 4), MARGINWH(99, 99, 1, 1)},
	/* shadow 1*/
	{IDB_BSKIN, BET_MARGINBITMAP | BET_DEFAULT, RECTDATAWH(6, 121, 16, 16), MARGINWH(10, 125, 8, 8)},

};

//................................................................................

#endif // _U_SKIN_H_
