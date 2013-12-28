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


#if !defined(AFX_CMainDialog_H)
#define AFX_CMainDialog_H

#pragma once

#include "Spectrum_Dlg.h"

typedef struct
{
	char name[KX_MAX_STRING];
	int vol;
	int pan;
	int mute;
	int omni;
	int phase;

	kSlider *vol_s,*pan_s;
	kButton *mute_b;
	kButton *omni_b;
	kButton *center;
	kStatic *name_s;
	kIndicator *indi_l,*indi_r;
	kKnob *knob;
	CEdit *edit;

	int type;
// types:
#define TYPE_STEREO		0x1	// disp pan part
#define TYPE_MUTE		0x2	// enable mute ctrl
#define TYPE_NOVOL		0x4	// dont disp vol part
#define TYPE_NAMED		0x8 	// with name field
#define TYPE_OMNI		0x10

#define TYPE_MIXER		0x1000
#define TYPE_AC97		0x2000
#define TYPE_DSP		0x4000
#define TYPE_AMOUNT		0x8000
#define TYPE_AGC		0x10000
#define TYPE_DB			0x20000	// extended epilog/prolog-like dB-based (x4 assumed)
#define TYPE_NEGATIVE   0x40000 // extended control with negative values (uses MACSN)
	int id;
	int bits;		// bitness for ac97
	char gpr1[KX_MAX_STRING],gpr2[KX_MAX_STRING];
}slider_t;

class CMainDialog;
extern CMainDialog *mixer_window;

class CMainDialog : public CKXDialog
{
public:
	CMainDialog();// standard constructor

	const char *get_class_name() { return "kXMain"; };

	int is_aps;
	int is_a2;
	int is_k2ex;
	int is_51;
	int is_10k2;
	int is_doo;
	int has_ac97;
	int is_zsnb;

	int select_device(int dev_=-1);
	int mixer_notify();
	int mixer_notify_lock;

	int check_device(const char *str);

	int cur_page;
	int destroy_page();
	int create_page(int page);
        int create_page(char *page);
	void redraw_window(int page)
	{ PostMessage(WM_COMMAND,0xf0f0,page);	};
	int create_button(kButton *b,UINT id,char *param,char *guid=0);
	int create_combo(kCombo *b,UINT id, char *param);
	int create_slider(char *param);

	int get_slider_profile(char *ref_slider,char *templ_slider,char *sl,
	              char *tmp_str,int tmp_str_size);

	int get_volume(char *l,char *r,int i,int patch);
	int set_volume(char *l,char *r,int i,int patch);

        // main buttons and special controls
        kButton b_sfload,b_sfunload,b_sfreload,b_sfunloadall,
                b_sfcsave,b_sfcload;
        kButton b_specmode;
        kButton b_ac97_reset;

        // checkboxes
        kCheckButton b_doo,b_swap_rear,b_bypass,/*b_phones,*/b_p_csw,b_spdif_decode;
        kCheckButton b_ac97_micboost,b_ac97_a2;

        #define MAX_BUTTONS 50
        kButton buttons[MAX_BUTTONS];
        char button_guids[MAX_BUTTONS][KX_MAX_STRING];

        #define MAX_IMAGES 80
        kStatic static_images[MAX_IMAGES];
        int cur_static;

        #define MAX_SLIDERS 80
        slider_t sliders[MAX_SLIDERS];
        int cur_slider;

        void update_slider_info(int i);
        int update_indi_info(int i);

        // SoundFont special controls
	    kList sf_list;
	    kList sf_info;
	    kProgress sf_progress;

        // AC97 Combo boxes
        kCombo ac97_source;
        kCombo ac97_3dse;
        kCombo ac3_passthru;
        // kCombo bypass_1; FIXME
        // kCombo bypass_2;
        // E-Card SPDIF select
        kCombo ecard_spdif0;
        kCombo ecard_spdif1;
        kCombo spdif_recording;
        kCombo p16v_recording;
        kCombo zsnb_source;

	void OnChangeAC97Source();
	void OnChangeAC973DSE();
	void OnChangeAC3Passthru();
	void OnChangeECardSpdif();
	void OnChangeSpdifRecording();
    void OnChangeZsnbSource();
	void OnChangeP16vRecording();

        // DSP
        kCombo dsp_freq;
        void OnChangeDSPFreq();

        // spectrum special control
	CKXSpectrum spectrum;

	virtual void init();
	virtual void on_destroy();

        int on_command(int wParam, int lParam);

protected:
        afx_msg void OnSelectSoundfont();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	DECLARE_MESSAGE_MAP()
};

#define IDg_SFLOAD		(WM_USER+0x27)
#define IDg_SFUNLOAD		(WM_USER+0x28)
#define IDg_SPECMODE		(WM_USER+0x29)

// special control messages
#define IDg_SFLIST		(WM_USER+0x2c)
#define IDg_SFINFO		(WM_USER+0x2d)
#define IDg_SFMEM		(WM_USER+0x2e)
#define IDg_AC97SOURCE		(WM_USER+0x30)
#define IDg_AC973DSE		(WM_USER+0x31)
#define IDg_DSPFREQ		(WM_USER+0x32)
#define IDg_AC97RESET		(WM_USER+0x33)
#define IDg_DOO			(WM_USER+0x35)
#define IDg_SWAPREAR		(WM_USER+0x36)
#define IDg_BYPASS		(WM_USER+0x37)
#define IDg_P_CSW		(WM_USER+0x39)
#define IDg_AC97_MICBOOST	(WM_USER+0x3a)

#define IDg_PASSTHRU		(WM_USER+0x3e)
#define IDg_ECARD_SPDIF0	(WM_USER+0x3f)
#define IDg_ECARD_SPDIF1	(WM_USER+0x40)

#define IDg_SFRELOAD		(WM_USER+0x41)
#define IDg_SFCLOAD		(WM_USER+0x42)
#define IDg_SFCSAVE		(WM_USER+0x43)

#define IDg_SPDIF_DECODE	(WM_USER+0x45)

#define IDg_SPDIF_RECORDING	(WM_USER+0x46)
#define IDg_P16V_RECORDING	(WM_USER+0x47)

#define IDg_SFUNLOADALL		(WM_USER+0x49)

#define IDg_COPY		(WM_USER+0x4a)
#define IDg_AC97_A2		(WM_USER+0x4b)

#define IDg_ZSNB_SOURCE (WM_USER+0x4c)

// common controls
#define IDg_BUTTON		(WM_USER+0x200)	// + button# [an index to guid[] of CMainDialog]
#define IDg_CONTROL		(WM_USER+0x300)	// + slider#
#define IDg_CONTROL2		(WM_USER+0x400) // + slider# (for stereo)
#define IDg_CONTROL_MUTE	(WM_USER+0x500)
#define IDg_CONTROL_OMNI	(WM_USER+0x600)
#define IDg_CONTROL_CENTER	(WM_USER+0x700)
#define IDg_CONTROL_LAST	(IDg_CONTROL_CENTER+MAX_SLIDERS+1)

#endif
