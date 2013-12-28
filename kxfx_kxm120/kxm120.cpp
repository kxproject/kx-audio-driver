// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2001-2006.
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

// --------------- the above should be a 'standard' beginning of any .cpp file

#include "kxm120.h"
#include "da_kxm120.cpp"


// global midi callback (definitions in mmsystem.h, windef.h) 
void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    iKXM120Plugin * pPlugin = (iKXM120Plugin *)dwInstance;
    pPlugin->OnMidiIn(hMidiIn, wMsg, dwParam1, dwParam2);  // pass data to plugin function
}

// definition of parameter keys
typedef enum _params_id{
	CV_VAL_ID,
	GATE_VAL_ID,
	MIDIIN_COMBO_ID,
	MIDICHAN_COMBO_ID
};

// set defaults
static kxparam_t default_params[KXM120_PARAMS_COUNT] = {0, 0, 0, 0};

int iKXM120Plugin::set_defaults() 
{
	set_all_params(default_params); 
	return 0;
}


/* Plugin Implemenation */

int iKXM120Plugin::get_param(int ndx, kxparam_t *value) // ndx corresponds to 'logical' parameter id
{
	*value = _params[ndx]; 
	return 0;
}

// set_param sets 'logical' values by writing to (one or a group of) 'physical' registers

int iKXM120Plugin::set_param(int ndx, kxparam_t value)
{
	_params[ndx] = value;

	double cv = 0;
	double v = (double)value;
	double d1 = pow(2.0, 1.0 / 12.0);
	double d2 = pow(d1, 128.0);

 	switch (ndx)
	{
		case CV_VAL_ID:
			cv = pow(d1, v) / d2;
			set_dsp_register(CV_VAL_P, _dbl_to_dspword(cv));
			break;

		case GATE_VAL_ID:
			set_dsp_register(GATE_VAL_P, (dword)value);
            debug("kX Demo: the user selected item %d\n",value);
			break;

        case MIDIIN_COMBO_ID:
		// here we can change a set of parameters depending on the plugin logic
        // 'value' represents the selected item (0-first, 1-second etc..)

			set_param(GATE_VAL_ID, 0);

			if (m_hMidiIn != NULL)
			{				
				midiInStop(m_hMidiIn);
				midiInClose(m_hMidiIn);				
				while (m_bMidiInOpen)
					Sleep(100);
				set_param(GATE_VAL_ID, 0);
				m_hMidiIn = NULL;
			}

			if (value != 0)
			{
        		if (midiInOpen(&m_hMidiIn, (UINT)(value - 1), (DWORD_PTR)MidiInProc, (DWORD_PTR)this, CALLBACK_FUNCTION) == MMSYSERR_NOERROR)
        			midiInStart(m_hMidiIn);
				else // error opening midi port
					_params[ndx] = 0; // set port selection to none
			}

       	break;

		case MIDICHAN_COMBO_ID:
		// here we can change a set of parameters depending on the plugin logic
                // 'value' represents the selected item (0-first, 1-second etc..)
	   		set_param(GATE_VAL_ID, 0);
			nChannel=(int)value;			
		break;

        // we need do synchronize the GUI part, if set_param was called by the mixer or kX Automation
     	if (cp) ((iKXM120PluginDlg*) cp)->sync(ndx);

	
	}

 	return 0;
}

int iKXM120Plugin::set_all_params(kxparam_t* values)
{
	for (int i = 0; i < KXM120_PARAMS_COUNT; i++) set_param(i, values[i]);
	return 0;
}

int iKXM120Plugin::request_microcode() 
{

	m_hMidiIn = NULL;
	m_bMidiInOpen = FALSE;

	publish_microcode(kxm120);

	return 0;
}

const char *iKXM120Plugin::get_plugin_description(int id)
{
	plugin_description(kxm120);
}


int iKXM120Plugin::get_user_interface()
{
	return IKXPLUGIN_OWNER;
}

int iKXM120Plugin::event(int event)
{
	switch (event)
	{
	case IKX_UNLOAD:
		set_param(GATE_VAL_ID, 0);
		if (m_hMidiIn != NULL)
		{			
			midiInStop(m_hMidiIn);
			midiInClose(m_hMidiIn);
			while (m_bMidiInOpen)
				Sleep(100);			
			set_param(GATE_VAL_ID, 0);
			m_hMidiIn = NULL;
		}
		break;
	}

	return 0;
}

void iKXM120Plugin::OnMidiIn(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	//int nChannel = 2;

	switch (wMsg)
	{
		case MIM_CLOSE:
			m_bMidiInOpen = FALSE;
			break;
		case MIM_DATA:
		{
			union 
			{ 
				DWORD pData; 
				DWORD dwData; 
				BYTE bData[4]; 
			} uMidiData; 

			uMidiData.pData = (DWORD)dwParam1;

			if ((uMidiData.bData[0] & 0xf) != nChannel - 1)
				break;

			switch (uMidiData.bData[0] & 0xF0)
			{
				case 0x80:	// Note Off
					set_param(GATE_VAL_ID, 0);
					break;
				case 0x90:  // Note On	
					if (uMidiData.bData[2] == 0) // Note off
						set_param(GATE_VAL_ID, 0);
					else
					{						
						set_param(CV_VAL_ID, (int)uMidiData.bData[1]);					
						set_param(GATE_VAL_ID, 0x7fffffff);
					}
					break;						
				default:
					break;
			}			
		}
		break;
		case MIM_ERROR:
			OutputDebugString("MIM_ERROR\r\n");
			break;
		case MIM_OPEN:
			m_bMidiInOpen = TRUE;
			break;
		default:
			break;
	}
}

iKXPluginGUI *iKXM120Plugin::create_cp(kDialog *parent, kFile *mf)
{
	iKXM120PluginDlg *tmp;
	tmp = new iKXM120PluginDlg(this, parent,mf);
	tmp->create();
	return tmp;
}

/* Plugin UI Implementation */

void iKXM120PluginDlg::init()
{
	CKXPluginGUI::init(); // this is necessary

	// custom dialog initialization here...
	// (title, Width, Height); standard width = 360 pix
	create_dialog(plugin_name(kxm120), 360, 50);

    // you can add any label; the ID should be >PARAM_COUNT
    create_label(midiin_label,KXM120_PARAMS_COUNT+1,"MidiIn",15,20,100);

    int nMidiInDevices = midiInGetNumDevs();

    // (kCombo &combo,UINT ctrl_id,const char *label,int x,int y,int wd,int n_items,int dropped_width)
	create_combo(midiin_combo,MIDIIN_COMBO_ID,"MidiInDevice",50,15,180,nMidiInDevices + 1,180);

	midiin_combo.add_string("-");
	
	MIDIINCAPS mic;
	for (int i=0; i<nMidiInDevices; i++)
	{		
		HRESULT hr = midiInGetDevCaps(i, &mic, sizeof(MIDIINCAPS));
		if (hr == MMSYSERR_NOERROR)		
			midiin_combo.add_string(mic.szPname);
	}
	
	// you can add any label; the ID should be >PARAM_COUNT
    create_label(midichan_label,KXM120_PARAMS_COUNT+1,"Channel",245,20,100);

	     // (kCombo &combo,UINT ctrl_id,const char *label,int x,int y,int wd,int n_items,int dropped_width)
	create_combo(midichan_combo,MIDICHAN_COMBO_ID,"MidiChannel",295,15,40,17,40);

	midichan_combo.add_string("-");
   	midichan_combo.add_string("1");
	midichan_combo.add_string("2");
	midichan_combo.add_string("3");
	midichan_combo.add_string("4");
	midichan_combo.add_string("5");
    midichan_combo.add_string("6");
	midichan_combo.add_string("7");
	midichan_combo.add_string("8");
	midichan_combo.add_string("9");
    midichan_combo.add_string("10");
	midichan_combo.add_string("11");
	midichan_combo.add_string("12");
    midichan_combo.add_string("13");
	midichan_combo.add_string("14");
	midichan_combo.add_string("15");
	midichan_combo.add_string("16");	

	controls_enabled = 1; // enable updating GUI when the parameters are changed

	sync(EVERYPM_ID); // update all 'GUI' elements to the preset plugin parameters
}

void iKXM120PluginDlg::sync(int ndx)
{

	// this function should 'synchronize' any labels with the parameters

	switch (ndx)
	{ 
	case EVERYPM_ID:

	case MIDIIN_COMBO_ID:
		if(controls_enabled) midiin_combo.set_selection((int)plugin->_params[MIDIIN_COMBO_ID]);
      	if (ndx != EVERYPM_ID) break;
		// fallthru

    case MIDICHAN_COMBO_ID:
		if(controls_enabled) midichan_combo.set_selection((int)plugin->_params[MIDICHAN_COMBO_ID]);
      	if (ndx != EVERYPM_ID) break;
		// fallthru
	}
}
