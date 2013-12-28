// kX Mixer
// Copyright (c) Eugene Gavrilov, 2001-2005.
// All rights reserved

/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */


#if !defined(AFX_KXREMOTECTRL_H__8CF38337_77A4_4F06_8A5F_BB91831DFD59__INCLUDED_)
#define AFX_KXREMOTECTRL_H__8CF38337_77A4_4F06_8A5F_BB91831DFD59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRemoteControl class

class COSDIcon;
class COSDVolume;

class iKXNotifier
{
public:
 // int message(int msg,int wparam,int lparam);

 iKXNotifier();
 ~iKXNotifier();

 int init(int device_num_);
 int close();

 int stop();
 int resume(int force=0);

 int device_num_t;
 iKX *ikx_t;
 iKXPluginManager *pm_t;
 iKXMidiParser *parser_t;
 dword is_edsp;

 HMIDIIN midi_in;
 MIDIHDR header;
 char buffer[1024];

 LRESULT callback_long(WPARAM,LPARAM);
 LRESULT callback_short(WPARAM,LPARAM);

 dword remote_ir_enabled;
 dword osd_enabled;

 int process_message(int message);

 // osd
/* int vol_step;
 int vol_cnt;
 dword last;
*/

 COSDIcon *osd_icon;
 COSDVolume *osd_volume;

 void process_gpio(byte msg,int where);
 void process_spdif();
 void process_notify(int what);
 void process_edsp();

 void osd_notify(int icon,const char *str);
 #define ICON_VOLUME		1
 #define ICON_AC3_PT_ON		2
 #define ICON_AC3_SW_ON		4
 #define ICON_AC3_PT_OFF	8
 #define ICON_AC3_SW_OFF	0x10
 #define ICON_SPEAKER		0x20
 #define ICON_SPDIF		0x40
 #define ICON_SPDIF_AC3		0x80
 // winamp-style
 #define ICON_PLAY		0x100
 #define ICON_PAUSE		0x200
 #define ICON_STOP		0x400
 #define ICON_NEXT		0x800
 #define ICON_PREV		0x1000
 // EAX/3-D stuff
 #define ICON_3D_START		0x2000
 #define ICON_3D_STOP		0x4000
 // E-DSP stuff
 #define ICON_EDSP      0x8000

 dword last_pt_on,last_sw_on,last_spdif,last_spdif_ac3,last_3d_on;

 // remote IR
/* int seq;
 int dir;
*/

 // mute headphones
 byte prev_gpio;
};

/*
#define VOL_STEP	0x300		// step: 0xffff/0x300=~85 unique values
#define VOL_LATENCY	400
#define VOL_TIMEOUT	600		// delay between separate key_down
#define SUBVOL_TIMER_ID		4321
*/
#define OSD_TIMER	3000		// OSD timer delay
#define OSDVOL_TIMER_ID		1234
#define OSDICON_TIMER_ID	4123

#define KXR_0		0
#define KXR_1		1
#define KXR_2		2
#define KXR_3		3
#define KXR_4		4
#define KXR_5		5
#define KXR_6		6
#define KXR_7		7
#define KXR_8		8
#define KXR_9		9
#define KXR_STOP	10
#define KXR_PLAY	11
#define KXR_SLOW	12
#define KXR_PREV	13
#define KXR_NEXT	14
#define KXR_STEP	15
#define KXR_MUTE	16
#define KXR_VOLINCR	17
#define KXR_VOLDECR	18
#define KXR_EAX		19
#define KXR_OPTIONS	20
#define KXR_DISPLAY	21
#define KXR_RETURN	22
#define KXR_START	23
#define KXR_CLOSE	24
#define KXR_UP		25
#define KXR_DOWN	26
#define KXR_FW		27
#define KXR_RW		28
#define KXR_SELECT	29
#define KXR_POWER	30
#define KXR_CMSS	31
#define KXR_REC		32

#define KXR_X_CMSS	33	// a2 platinum ex on-board
#define KXR_X_VOLINCR	34
#define KXR_X_VOLDECR	35
#define KXR_X_MUTE	36

#define KXR_IR_LAST	KXR_X_MUTE

// additional mm-keyboard events:
#define KXR_KEYB_VOLINCR	(KXR_IR_LAST+1)
#define KXR_KEYB_VOLDECR	(KXR_IR_LAST+2)
#define KXR_KEYB_MUTE		(KXR_IR_LAST+3)
#define KXR_KEYB_PREV		(KXR_IR_LAST+4)
#define KXR_KEYB_NEXT		(KXR_IR_LAST+5)
#define KXR_KEYB_STOP		(KXR_IR_LAST+6)
#define KXR_KEYB_PLAY		(KXR_IR_LAST+7)

#define KXR_KEYB_LAST		KXR_KEYB_PLAY

#define KXR_EMAIL	(KXR_KEYB_LAST+1)
#define KXR_MEDIA	(KXR_KEYB_LAST+2)
#define KXR_SEARCH	(KXR_KEYB_LAST+3)
#define KXR_FAVOURITES	(KXR_KEYB_LAST+4)
#define KXR_HOME	(KXR_KEYB_LAST+5)

#define KXR_LAST	KXR_HOME

#define KXR_SPEAKER	0x1000

#endif // !defined(AFX_KXREMOTECTRL_H__8CF38337_77A4_4F06_8A5F_BB91831DFD59__INCLUDED_)
