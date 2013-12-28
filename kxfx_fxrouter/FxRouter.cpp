// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2001-2005.
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

#include "stdafx.h"

#pragma warning(disable:4242)

// --------------- the above should be a 'standard' beginning of any .cpp file

// effect class
#include "FxRouter.h"
// effect source
#include "da_FxRouter.cpp"

#define IDC_DUMMY		100
#define IDC_CLEAR		101
#define IDC_CLEAR_ALL	102
#define IDC_RESET		103
#define IDC_RESET_ALL	104			

#define IDC_LCD1 1001
#define IDC_LCD2 1002
#define IDC_LCD3 1003
#define IDC_LCD4 1004
#define IDC_LCD5 1005
#define IDC_LCD6 1006
#define IDC_LCD7 1007

#define IDC_SI1 2001
#define IDC_SI2 2002
#define IDC_SI3 2003
#define IDC_SI4 2004
#define IDC_SI5 2005
#define IDC_SI6 2006
#define IDC_SI7 2007

typedef enum _params_id{
	EFFECT1,
	EFFECT2,
	EFFECT3,
	EFFECT4,
	EFFECT5,
	EFFECT6,
	EFFECT7,
	BYPASS1,
	BYPASS2,
	BYPASS3,
	BYPASS4,
	BYPASS5,
	BYPASS6,
	BYPASS7
};

static int out_registers[16] = {
	SEND_1L,
	SEND_2L,
	SEND_3L,
	SEND_4L,
	SEND_5L,
	SEND_6L,
	SEND_7L,
	OUT_LEFT,
	SEND_1R,
	SEND_2R,
	SEND_3R,
	SEND_4R,
	SEND_5R,
	SEND_6R,
	SEND_7R,
	OUT_RIGHT
};

static int in_registers[16] = {
	IN_LEFT,
	RETURN_1L,
	RETURN_2L,
	RETURN_3L,
	RETURN_4L,
	RETURN_5L,
	RETURN_6L,
	RETURN_7L,
	IN_RIGHT,
	RETURN_1R,
	RETURN_2R,
	RETURN_3R,
	RETURN_4R,
	RETURN_5R,
	RETURN_6R,
	RETURN_7R
};

static kxparam_t default_params[FXROUTER_PARAMS_COUNT] = {0, 1, 2, 3, 4, 5, 6, 0, 0, 0, 0, 0, 0, 0};

int iFxRouterPlugin::set_defaults() 
{	
	set_all_params(default_params); 	
	return 0;
}


static kx_fxparam_descr _param_descr[FXROUTER_PARAMS_COUNT] = {
	{"Effect 1", KX_FXPARAM_USER,	0, 6},
	{"Effect 2", KX_FXPARAM_USER,	0, 6},
	{"Effect 3", KX_FXPARAM_USER,	0, 6},
	{"Effect 4", KX_FXPARAM_USER,	0, 6},
	{"Effect 5", KX_FXPARAM_USER,	0, 6},
	{"Effect 6", KX_FXPARAM_USER,	0, 6},
	{"Effect 7", KX_FXPARAM_USER,	0, 6},
	{"Bypass 1", KX_FXPARAM_SWITCH,	0,  1},
	{"Bypass 2", KX_FXPARAM_SWITCH,	0,  1},
	{"Bypass 3", KX_FXPARAM_SWITCH,	0,  1},
	{"Bypass 4", KX_FXPARAM_SWITCH,	0,  1},
	{"Bypass 5", KX_FXPARAM_SWITCH,	0,  1},
	{"Bypass 6", KX_FXPARAM_SWITCH,	0,  1},
	{"Bypass 7", KX_FXPARAM_SWITCH,	0,  1}
};

#define P (kxparam_t)

static kxparam_t presets[]=
{ 
	NULL
};

#undef P

/* Plugin Implemenation */

int iFxRouterPlugin::describe_param(int ndx, kx_fxparam_descr *descr)
{
	memcpy(descr, &_param_descr[ndx], sizeof kx_fxparam_descr); return 0;
}

const kxparam_t *iFxRouterPlugin::get_plugin_presets()
{
	return presets;
}

int iFxRouterPlugin::init()
{
	strDefault[0] = strCurrent[0] = "1: Slot One";
	strDefault[1] = strCurrent[1] = "2: Slot Two";
	strDefault[2] = strCurrent[2] = "3: Slot Three";
	strDefault[3] = strCurrent[3] = "4: Slot Four";
	strDefault[4] = strCurrent[4] = "5: Slot Five";
	strDefault[5] = strCurrent[5] = "6: Slot Six";
	strDefault[6] = strCurrent[6] = "7: Slot Seven";

	return 0;
}

int iFxRouterPlugin::get_param(int ndx, kxparam_t *value)
{
	*value = _params[ndx]; 
	return 0;
}

int iFxRouterPlugin::set_param(int ndx, kxparam_t value)
{	
	switch (ndx)
	{
		case EFFECT1:
		case EFFECT2:
		case EFFECT3:
		case EFFECT4:
		case EFFECT5:
		case EFFECT6:
		case EFFECT7:
		{
			int ndx_swap = -1;

			if ( value != _params[ndx] )
			{
				for (int i=0; i<FXROUTER_PARAMS_COUNT/2; i++)
				{
					if (value == _params[i])
						ndx_swap = i;			
				}	
			}     

			if (ndx_swap != -1) // no need to swap when setting defaults, etc
			{					
				kxparam_t tmp1 = _params[ndx_swap + BYPASS1];
				kxparam_t tmp2 = _params[ndx + BYPASS1];

				if (_params[ndx + BYPASS1] != _params[ndx_swap + BYPASS1])
				{
					_params[ndx + BYPASS1] = tmp1;
					_params[ndx_swap + BYPASS1] = tmp2;
					if (cp) 
					{
						((iFxRouterPluginDlg*) cp)->sync(ndx + BYPASS1);
						((iFxRouterPluginDlg*) cp)->sync(ndx_swap + BYPASS1);
					}
				}										
				
				_params[ndx_swap] = _params[ndx];	
				if (cp) ((iFxRouterPluginDlg*) cp)->sync(ndx_swap);
			}

			_params[ndx] = value;
			write_microcode();
			if (cp) ((iFxRouterPluginDlg*) cp)->sync(ndx);
				
			break;
		}
		case BYPASS1:
		case BYPASS2:
		case BYPASS3:
		case BYPASS4:
		case BYPASS5:
		case BYPASS6:
		case BYPASS7:
		{
			_params[ndx] = value;
			set_param(ndx - BYPASS1, _params[ndx - BYPASS1]);			
			if (cp) ((iFxRouterPluginDlg*) cp)->sync(ndx);
			break;
		}
	}

	return 0;
}


void iFxRouterPlugin::write_microcode()
{
	int last_nb = -1;  // last non-bypassed slot

	for (int i=0; i<8; i++)
	{
				
		if ( _params[i + BYPASS1] == 0 )
		{					

			if (last_nb == -1) 
			{
				write_instruction(i, MACS, out_registers[ _params[i] ], in_registers[0], C_0, C_0);
				write_instruction(i + 8, MACS, out_registers[ _params[i] + 8 ], in_registers[8], C_0, C_0);
			}
			else 
			{
				write_instruction(i, MACS, out_registers[ _params[i] ], in_registers[ last_nb ], C_0, C_0);
				write_instruction(i + 8, MACS, out_registers[ _params[i] + 8 ], in_registers[ last_nb + 8 ], C_0, C_0);						
			}		

			last_nb = (int)_params[i] + 1;
		}
		else
		{
			if (i == 7)
			{
				if (last_nb == -1)  // all bypassed
				{
					write_instruction(i, MACS, out_registers[i], in_registers[0], C_0, C_0);
					write_instruction(i + 8, MACS, out_registers[i + 8], in_registers[8], C_0, C_0);
				}
				else	
				{
					write_instruction(i, MACS, out_registers[i], in_registers[last_nb], C_0, C_0);
					write_instruction(i + 8, MACS, out_registers[i + 8], in_registers[last_nb + 8], C_0, C_0);
				}
			}
			else 
			{
				write_instruction(i, MACS, out_registers[ _params[i] ], C_0, C_0, C_0);
				write_instruction(i + 8, MACS, out_registers[ _params[i] + 8 ], C_0, C_0, C_0);
			}
		}
	}  		
}

int iFxRouterPlugin::set_all_params(kxparam_t* values)
{
	// all microcode is written at once, so just update _params and then
	// use set_param once (_params array needs to be populated to work right)
	for (int i=0; i<FXROUTER_PARAMS_COUNT; i++)
		_params[i] = values[i];	
	set_param(0, _params[0]);
	return 0;
}

int iFxRouterPlugin::request_microcode() 
{
	publish_microcode(FxRouter);
	return 0;
}

const char *iFxRouterPlugin::get_plugin_description(int id)
{
	plugin_description(FxRouter);
}

iKXPluginGUI *iFxRouterPlugin::create_cp(kDialog *parent, kFile *mf)
{
	iFxRouterPluginDlg *tmp;
	tmp = new iFxRouterPluginDlg(this, parent,mf);
	tmp->create();
	return tmp;
}

int iFxRouterPlugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;
}

int iFxRouterPlugin::save_plugin_settings(kSettings &cfg)
{
	 if(pgm_id>0)
	 {
		char section[128];
		sprintf(section,"pgm_%d",pgm_id);
		
		char strName[10];
		for (int i=0; i<FXROUTER_PARAMS_COUNT/2; i++)
		{
			sprintf(strName, "SR%d Text", i);
			cfg.write(section, strName, strCurrent[i].GetBuffer(strCurrent[i].GetLength()));
			strCurrent[i].ReleaseBuffer();
		}
	 }
	 return 0;
}

int iFxRouterPlugin::load_plugin_settings(kSettings &cfg)
{
	char strName[10];
	char strText[15];
	if(pgm_id>0)
	{		
		char section[128];
		sprintf(section,"pgm_%d",pgm_id);
		
		for (int i=0; i<FXROUTER_PARAMS_COUNT/2; i++)
		{
			sprintf(strName, "SR%d Text", i);
			if ( cfg.read(section, strName, strText, sizeof(strText)) == 0 )			
				strCurrent[i] = strText;
			else
				strCurrent[i] = strDefault[i];
		}		
	}
	else
		for (int i=0; i<FXROUTER_PARAMS_COUNT/2; i++)
			strCurrent[i] = strDefault[i];

	return 0;
}

/* Plugin UI Implementation */

BEGIN_MESSAGE_MAP(iFxRouterPluginDlg, CKXPluginGUI)
	ON_NOTIFY_RANGE(SI_SWAP, IDC_SI1, IDC_SI7, OnSwap )
	ON_NOTIFY_RANGE(LD_CHANGE, IDC_LCD1, IDC_LCD7, OnChange )	
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

void iFxRouterPluginDlg::init()
{
	FxRouter_skin.set_skin("FxRouter.kxs");
    mf.attach_skin(&FxRouter_skin);
    FxRouter_skin.attach_skin(kxmixer_skin);
    FxRouter_skin.set_attach_priority(1);


	CKXPluginGUI::init();

	create_dialog(plugin_name(FxRouter), 427, 611);

	grpbox.hide();
	b_mute.hide();
	
	HBITMAP hbmp[3];
	hbmp[0] = mf.load_image("si_norm.bmp");
	hbmp[1] = mf.load_image("si_drag.bmp");
	hbmp[2] = mf.load_image("si_drag_over.bmp");	

	for (int i = 0; i < FXROUTER_PARAMS_COUNT / 2; i++)
	{	
		// off is bypassed (checked)
		btnBypass[i].set_bitmaps(
		mf.load_image("btn_on.bmp"),  // unchecked
		mf.load_image("btn_on.bmp"),  // unchecked_over
		mf.load_image("btn_off.bmp"), // checked
		mf.load_image("btn_off.bmp"));// checked_over
		create_checkbox(btnBypass[i], BYPASS1 + i, "", 43, 27 + (i*79), 18);
		btnBypass[i].set_method(kMETHOD_COPY);

		lcd[i].Create(plugin->strCurrent[i],    CPoint(87,  50 + (i*79)) , this, IDC_LCD1 + i, 14, TRUE);			
		
		si[i].SetBitmaps(hbmp);
		si[i].Create(CPoint(344,  50 + (i*79)), this, IDC_SI1 + i);
	}

	controls_enabled = 1;

	sync(EVERYPM_ID);
}

void iFxRouterPluginDlg::sync(int ndx)
{
	switch (ndx)
	{ 
		case EVERYPM_ID:
		{			
			for (int i=0; i<FXROUTER_PARAMS_COUNT/2; i++)
			{
				if (controls_enabled) 
				{
					CString strText = plugin->strCurrent[plugin->_params[i]];
					lcd[i].SetWindowText(strText);
					btnBypass[i].set_check((int)plugin->_params[BYPASS1 + i]);
					btnBypass[i].redraw();
					lcd[i].SetBackLight(!plugin->_params[BYPASS1 + i]);
				}
			}
			break;
		}
		case EFFECT1:
		case EFFECT2:
		case EFFECT3:
		case EFFECT4:
		case EFFECT5:
		case EFFECT6:
		case EFFECT7:
			if (controls_enabled) 
			{
				CString strText = plugin->strCurrent[plugin->_params[ndx]];
				lcd[ndx].SetWindowText(strText);					
			}
			break;
		case BYPASS1:
		case BYPASS2:
		case BYPASS3:
		case BYPASS4:
		case BYPASS5:
		case BYPASS6:
		case BYPASS7:
			if (controls_enabled) 
			{
				btnBypass[ndx - BYPASS1].set_check((int)plugin->_params[ndx]);
				btnBypass[ndx - BYPASS1].redraw(); 
				lcd[ndx - BYPASS1].SetBackLight(!plugin->_params[ndx]);
			}
			break;
	}
}

void iFxRouterPluginDlg::OnSwap( UINT id, NMHDR * pnmhdr, LRESULT * pResult )
{

	SIHDR * sihdr = (SIHDR *)pnmhdr;
	
	int nID1 = sihdr->idDropSource;
	int nID2 = sihdr->idDropTarget;

	int ndx1 = nID1 - IDC_SI1;
	int ndx2 = nID2 - IDC_SI1;
	
	plugin->set_param(ndx1, plugin->_params[ndx2]);
	
	*pResult = 0;
}

void iFxRouterPluginDlg::OnChange( UINT id, NMHDR * pnmhdr, LRESULT * pResult )
{
	UINT_PTR ndx = pnmhdr->idFrom - IDC_LCD1;
	CString strText;
	lcd[ndx].GetWindowText(strText);
	plugin->strCurrent[plugin->_params[ndx]] = strText;
	*pResult = 0;
}

int iFxRouterPluginDlg::on_command(int lp,int rp)
{
    int check;
	int ndx = lp-kCONTROL_BASE;
	
	switch(ndx)
	{

		case BYPASS1:
		case BYPASS2:
		case BYPASS3:
		case BYPASS4:
		case BYPASS5:
		case BYPASS6:
		case BYPASS7:
			check = btnBypass[ndx - BYPASS1].get_check();
			plugin->set_param(ndx, !check);
			return 1;
			break;
		default:
			return CKXPluginGUI::on_command(lp,rp);
	}	
}

void iFxRouterPluginDlg::OnContextMenu( CWnd* pWnd, CPoint pos)
{
	int nID = pWnd->GetDlgCtrlID();
	int nDX = nID - IDC_LCD1;
	switch (nID)
	{
		case IDC_LCD1:
		case IDC_LCD2:
		case IDC_LCD3:
		case IDC_LCD4:
		case IDC_LCD5:
		case IDC_LCD6:
		case IDC_LCD7:
		{			
			CMenu context_menu;
			context_menu.CreatePopupMenu();
			context_menu.AppendMenu(MF_STRING | MF_GRAYED, IDC_DUMMY, plugin->strDefault[plugin->_params[nDX]]);
			context_menu.AppendMenu(MF_SEPARATOR);
			context_menu.AppendMenu(MF_STRING, IDC_CLEAR, _T("Clear Name"));
			context_menu.AppendMenu(MF_STRING, IDC_RESET, _T("Reset Name"));
			context_menu.AppendMenu(MF_SEPARATOR);
			context_menu.AppendMenu(MF_STRING, IDC_CLEAR_ALL, _T("Clear All Names"));
			context_menu.AppendMenu(MF_STRING, IDC_RESET_ALL, _T("Reset All Names"));

			DWORD dwSelectionMade = context_menu.TrackPopupMenu( (TPM_LEFTALIGN|TPM_LEFTBUTTON|
                                                      TPM_NONOTIFY|TPM_RETURNCMD),
                                                      pos.x, pos.y, this);
			context_menu.DestroyMenu();

			if(dwSelectionMade |= 0)
			{				
				switch (dwSelectionMade)
				{
					case IDC_CLEAR:				
						lcd[nDX].SetWindowText("");
						break;
					case IDC_RESET:				
						lcd[nDX].SetWindowText(plugin->strDefault[plugin->_params[nDX]]);
						break;
					case IDC_CLEAR_ALL:	
					{
						for (int i=0; i<FXROUTER_PARAMS_COUNT/2;i++)
						{
							lcd[i].SetWindowText("");
						}
					}
					break;
					case IDC_RESET_ALL:				
					{
						for (int i=0; i<FXROUTER_PARAMS_COUNT/2;i++)
						{
							lcd[i].SetWindowText(plugin->strDefault[plugin->_params[i]]);
						}
					}
						break;
				}
			}
		}
		default:
			return;
	}
}