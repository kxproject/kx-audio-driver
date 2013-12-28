// kX Mixer / FX Control - MIDI Router
// kX VSTi Interface
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2002-2005.
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


#include "stdinc.h"

#include "midirouter.h"
#include "midiparser.h"
#include "mstuff.h"
#include "notify.h"
#include "remote_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const char *str_midi_notes[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "H"};
static const char *str_note_option[] = 
{
	" Velocity",
	" Key",
	"On (Type 1)",
	"On (Type 2)"
};

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMidiRouterDlg, CKXDialog)
        // Max's stuff:
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PLUGINS, OnListPluginSelect)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PLUGINS, OnListPluginDblClick)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PARAMS, OnListParamSelect)
	ON_BN_CLICKED(IDC_OPT_NONE, OnOptNoneClick)
	ON_BN_CLICKED(IDC_OPT_RAW, OnOptRawClick)
	ON_BN_CLICKED(IDC_OPT_CONTROLLER, OnOptControllerClick)
	ON_BN_CLICKED(IDC_OPT_PITCHBEND, OnOptPitchBendClick)
	ON_BN_CLICKED(IDC_OPT_NOTE, OnOptNoteClick)
	ON_CBN_SELCHANGE(IDC_CMB_CHANNEL, OnCmbChannelChange)
	ON_CBN_SELCHANGE(IDC_CMB_FUNCTION, OnCmbFunctionChange)
	ON_CBN_SELCHANGE(IDC_CMB_INDEX, OnCmbIndexChange)
END_MESSAGE_MAP()

void CMidiRouterDlg::init()
{
	generic_init("midi");

	char tmp_str[256];

        if(!mf.get_profile("midi","reset_button",tmp_str,sizeof(tmp_str)))
        {
         gui_create_button(this,&b_reset,IDM_RESET,tmp_str,mf);
         b_reset.ShowWindow(SW_SHOW);
        }

	CreateParserInterface();

        select_device();
}

int CMidiRouterDlg::select_device(int dev_)
{
	CKXDialog::select_device(dev_);

        // re-instantiate device-dependent GUI elements; redraw then
        device_init("midi");

        // create any custom controls here...
        if(::IsWindow(get_wnd()) && get_wnd())
			SetWindowText(mf.get_profile("lang","automation.name")+" - "+ikx_t->get_device_name());

	BuildPluginsList();

        return 0;
}

void CMidiRouterDlg::on_destroy()
{
	if(card_name.m_hWnd) card_name.DestroyWindow();

	CKXDialog::on_destroy();
}

int CMidiRouterDlg::on_command(int wParam, int lParam)
{
	WORD wID = LOWORD(wParam);

	switch(wID)
	{
		case IDM_RESET:
			if(parser_t)
			{
			 parser_t->reset();
			 return 1;
			}
	}

	return CKXDialog::on_command(wParam,lParam);
}

int CMidiRouterDlg::CreateParserInterface()
{
	SelParam = NULL;

	// Create Plugins List -------------------------------
	char tmp[256];

	RECT rect; memset(&rect,0,sizeof(rect));
	mf.get_profile("midi","plugin_list",tmp,sizeof(tmp));
	sscanf((LPCTSTR)tmp,"%d %d %d %d",&rect.left,&rect.top,&rect.right,&rect.bottom);

	// fixme, all of DestroyWindow() calls are nesessary only when CreateParserInterface() is called from select_device()
	lvwPlugins.DestroyWindow();
	lvwPlugins.CreateEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL, 
			WS_TABSTOP | LVS_REPORT | LVS_NOLABELWRAP | LVS_SHOWSELALWAYS |
			LVS_SHAREIMAGELISTS | WS_CHILD | LVS_SINGLESEL,
			rect, this, IDC_LIST_PLUGINS);

	lvwPlugins.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	lvwPlugins.SetFont(font);
	lvwPlugins.InsertColumn(1, mf.get_profile("lang","midi.plugin"), LVCFMT_LEFT, rect.right - rect.left - 4);

	// Create Parameters List ----------------------------
	mf.get_profile("midi","param_list",tmp,sizeof(tmp));
	sscanf((LPCTSTR)tmp,"%d %d %d %d",&rect.left,&rect.top,&rect.right,&rect.bottom);

	lvwParams.DestroyWindow();
	lvwParams.CreateEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL, 
			WS_TABSTOP | LVS_REPORT | LVS_NOLABELWRAP | LVS_SHOWSELALWAYS |
			LVS_SHAREIMAGELISTS | WS_CHILD | LVS_SINGLESEL,
			rect, this, IDC_LIST_PARAMS);
	lvwParams.SetFont(font);

	lvwParams.SetExtendedStyle(LVS_EX_FULLROWSELECT);

#define PARAM_HEADER_WIDTH	128
#define CHNL_HEADER_WIDTH	70
	lvwParams.GetClientRect(&rect);
	lvwParams.InsertColumn(1, mf.get_profile("lang","midi.parameter"), LVCFMT_LEFT, PARAM_HEADER_WIDTH);
	lvwParams.InsertColumn(2, mf.get_profile("lang","midi.event"), LVCFMT_LEFT, rect.right - rect.left - PARAM_HEADER_WIDTH - CHNL_HEADER_WIDTH);
	lvwParams.InsertColumn(3, mf.get_profile("lang","midi.channel"), LVCFMT_LEFT, CHNL_HEADER_WIDTH);

	// Create Events Group Box --------------------------
	mf.get_profile("midi","settings",tmp,sizeof(tmp));
	sscanf((LPCTSTR)tmp,"%d %d %d %d",&rect.left,&rect.top,&rect.right,&rect.bottom);

	grpMidiEvent.DestroyWindow();
	grpMidiEvent.CreateEx(WS_EX_STATICEDGE, _T("STATIC"), NULL, WS_CHILD, rect, this, IDC_GROUP_MIDIEVENT);

	// Create MIDI Event Child Controls
	long l = rect.left; long t = rect.top;
	SetRect(&rect, 6, 6, 100, 20);
	OffsetRect(&rect, l, t);
	optNone.Create(mf.get_profile("lang","midi.none"), BS_AUTORADIOBUTTON | WS_CHILD, rect, this, IDC_OPT_NONE);
	optNone.SetFont(font, FALSE);

	SetRect(&rect, 6, 26, 100, 40);
	OffsetRect(&rect, l, t);
	optController.DestroyWindow();
	optController.Create(mf.get_profile("lang","midi.ctrl"), BS_AUTORADIOBUTTON | WS_CHILD, rect, this, IDC_OPT_CONTROLLER);
	optController.SetFont(font, FALSE);

	SetRect(&rect, 6, 46, 100, 60);
	OffsetRect(&rect, l, t);
	optPitchBend.DestroyWindow();
	optPitchBend.Create(mf.get_profile("lang","midi.pitchbend"), BS_AUTORADIOBUTTON | WS_CHILD, rect, this, IDC_OPT_PITCHBEND);
	optPitchBend.SetFont(font, FALSE);

	SetRect(&rect, 6, 66, 100, 80);
	OffsetRect(&rect, l, t);
	optNote.DestroyWindow();
	optNote.Create(mf.get_profile("lang","midi.note"), BS_AUTORADIOBUTTON | WS_CHILD, rect, this, IDC_OPT_NOTE);
	optNote.SetFont(font, FALSE);

	SetRect(&rect, 6, 86, 100, 100);
	OffsetRect(&rect, l, t);
	optRaw.DestroyWindow();
	optRaw.Create(mf.get_profile("lang","midi.raw"), BS_AUTORADIOBUTTON | WS_CHILD, rect, this, IDC_OPT_RAW);
	optRaw.SetFont(font, FALSE);

	SetRect(&rect, 175, 6, 320, 306);
	OffsetRect(&rect, l, t);
	cmbChannel.DestroyWindow();
	cmbChannel.Create(CBS_DROPDOWNLIST | WS_CHILD | WS_VSCROLL, rect, this, IDC_CMB_CHANNEL);
	cmbChannel.SetFont(font, FALSE);
	char str[4];
	for (int i = 1; i <= 16; i++) {wsprintf(str, "%i", i); cmbChannel.AddString(str);}

	SetRect(&rect, 175, 33, 320, 233);
	OffsetRect(&rect, l, t);
	cmbFunction.DestroyWindow();
	cmbFunction.Create(CBS_DROPDOWNLIST | WS_CHILD | WS_VSCROLL, rect, this, IDC_CMB_FUNCTION);
	cmbFunction.SetFont(font, FALSE);

	SetRect(&rect, 175, 60, 320, 260);
	OffsetRect(&rect, l, t);
	cmbIndex.DestroyWindow();
	cmbIndex.Create(CBS_DROPDOWNLIST | WS_CHILD | WS_VSCROLL, rect, this, IDC_CMB_INDEX);
	cmbIndex.SetFont(font, FALSE);

	SetRect(&rect, 106, 9, 170, 29);
	OffsetRect(&rect, l, t);
	lblChannel.DestroyWindow();
	lblChannel.Create(mf.get_profile("lang","midi.channel_"), SS_RIGHT | WS_CHILD, rect, this, IDC_LBL_CHANNEL);
	lblChannel.SetFont(font, FALSE);

	SetRect(&rect, 106, 36, 170, 56);
	OffsetRect(&rect, l, t);
	lblFunction.DestroyWindow();
	lblFunction.Create(mf.get_profile("lang","midi.function"), SS_RIGHT | WS_CHILD, rect, this, IDC_LBL_FUNCTION);
	lblFunction.SetFont(font, FALSE);

	SetRect(&rect, 106, 63, 170, 83);
	OffsetRect(&rect, l, t);
	lblIndex.DestroyWindow();
	lblIndex.Create(mf.get_profile("lang","midi.index"), SS_RIGHT | WS_CHILD, rect, this, IDC_LBL_INDEX);
	lblIndex.SetFont(font, FALSE);

	// fixme patched 0.1 ShowParamControl(NULL);

	// Show Controls -------------------------------
	lvwPlugins.ShowWindow(SW_SHOW);
	lvwParams.ShowWindow(SW_SHOW);
	grpMidiEvent.ShowWindow(SW_SHOW);
	optNone.ShowWindow(SW_SHOW);
	optController.ShowWindow(SW_SHOW);
	optPitchBend.ShowWindow(SW_SHOW);
	optNote.ShowWindow(SW_SHOW);
	optRaw.ShowWindow(SW_SHOW);
	cmbChannel.ShowWindow(SW_SHOW);
	cmbFunction.ShowWindow(SW_SHOW);
	cmbIndex.ShowWindow(SW_SHOW);
	lblChannel.ShowWindow(SW_SHOW);
	lblFunction.ShowWindow(SW_SHOW);
	lblIndex.ShowWindow(SW_SHOW);
	return TRUE; 
}

int CMidiRouterDlg::BuildPluginsList()
{
	int n;
	char plgname[KX_MAX_STRING + 8];

	lvwPlugins.DeleteAllItems();

	if(parser_t!=NULL)
	{
	  int num=0;
	  while(1)
	  {
	    iKXPlugin *plugin=pm_t->enum_plugin(num);
	    if(plugin==0)
	     break;
	    wsprintf(plgname, "[%i] %s ", plugin->pgm_id, plugin->name);
	    n=lvwPlugins.InsertItem(plugin->pgm_id, plgname);
	    lvwPlugins.SetItemData(n, (DWORD_PTR) plugin);
	    num++;
	   }
	}
	 else debug("!! BuildPluginList\n");

	ShowParamControl(NULL);

	return TRUE;
}

void CMidiRouterDlg::OnListPluginDblClick(LPNMLISTVIEW nmlv, LRESULT *pResult)
{
	*pResult = 0;
	if(nmlv->iItem==-1)
		return;
	iKXPlugin* plugin = (iKXPlugin *) lvwPlugins.GetItemData(nmlv->iItem);
	if (plugin == NULL) return;
	pm_t->tweak_plugin(plugin->pgm_id);
}

int CMidiRouterDlg::select_plugin(iKXPlugin *plugin)
{
        // select it on the left

        for(int i=0;i<lvwPlugins.GetItemCount();i++)
        {
         iKXPlugin *plg=(iKXPlugin *)lvwPlugins.GetItemData(i);
         if(plg==plugin)
         {
          ShowPluginParams(plugin);
          lvwPlugins.SetItem(i,0,LVIF_STATE,0,0,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED,0);
          return 0;
         }
        }

        debug("!! CMidiRouterDlg::select_plugin failed\n");
        return -1;
}

void CMidiRouterDlg::OnListPluginSelect(LPNMLISTVIEW nmlv, LRESULT *pResult)
{
	iKXPlugin* plugin = NULL;
	*pResult = 0;

	if ((nmlv->uNewState & 0xff) == (LVIS_SELECTED | LVIS_FOCUSED))
	{
		plugin = (iKXPlugin*) nmlv->lParam;
	}
	ShowPluginParams(plugin);
}

void CMidiRouterDlg::ShowPluginParams(iKXPlugin *plugin)
{
	kx_fxparam_descr descr; 
	int param_count, r, n; 
	RECT rect;

	lvwParams.DeleteAllItems();
	if (plugin != NULL)
	{
		param_count = plugin->get_param_count();
		for (int ndx = 0; ndx < param_count; ndx++)
		{
			r = plugin->describe_param(ndx, &descr);
			if (r >= 0)
			{
				n = lvwParams.InsertItem(ndx, descr.name);
				lvwParams.SetItemData(n, ndx);
				SetParamEventInfo(n, parser_t->get_param(plugin, ndx));
			}
		}
	}
	lvwParams.GetClientRect(&rect);
	lvwParams.SetColumnWidth(1, rect.right - PARAM_HEADER_WIDTH - CHNL_HEADER_WIDTH);
	ShowParamControl(NULL);
}

void CMidiRouterDlg::OnListParamSelect(LPNMLISTVIEW nmlv, LRESULT *pResult)
{
	intptr_t p ;int ndx = 0;
	iKXPlugin* plugin = NULL;
	*pResult = 0;

	if ((nmlv->uNewState & 0xff) == (LVIS_SELECTED | LVIS_FOCUSED))
	{
		p = ((intptr_t) (lvwPlugins.GetFirstSelectedItemPosition())) - 1;
		plugin = (iKXPlugin*) lvwPlugins.GetItemData((int)p);
		ndx = (int) nmlv->lParam;
		SelParam = NULL;
	}
	ShowParamControl(plugin, ndx);
}

void CMidiRouterDlg::ShowParamControl(iKXPlugin *plugin, int ndx)
{
	if (plugin == NULL) // disable
	{
		optNone.SetCheck(1);
		optNone.EnableWindow(TRUE);
		optRaw.SetCheck(0);
		optRaw.EnableWindow(FALSE);
		optController.SetCheck(0);
		optController.EnableWindow(FALSE);
		optPitchBend.SetCheck(0);
		optPitchBend.EnableWindow(FALSE);
		optNote.SetCheck(0);
		optNote.EnableWindow(FALSE);
		SetEventCombos(CMBSET_VALID_ALL, CMBSET_NONE, CMBSET_NONE, CMBSET_NONE);
		UpdateSelParamEventInfo();
		return;
	}

	SelParam = parser_t->get_param(plugin, ndx);

	if (SelParam == NULL) // parameter not active
	{
		optNone.SetCheck(1);
		optNone.EnableWindow(TRUE);
		optController.SetCheck(0);
		optController.EnableWindow(TRUE);
		optPitchBend.SetCheck(0);
		optPitchBend.EnableWindow(TRUE);
		optNote.SetCheck(0);
		optNote.EnableWindow(TRUE);
		optRaw.SetCheck(0);
		optRaw.EnableWindow(TRUE);
		SetEventCombos(CMBSET_VALID_ALL, CMBSET_NONE, CMBSET_NONE, CMBSET_NONE);
		UpdateSelParamEventInfo();
		return;
	}

	optNone.SetCheck(0);
	optNone.EnableWindow(TRUE);
	optController.SetCheck(0);
	optController.EnableWindow(TRUE);
	optPitchBend.SetCheck(0);
	optPitchBend.EnableWindow(TRUE);
	optNote.SetCheck(0);
	optNote.EnableWindow(TRUE);
	optRaw.SetCheck(0);
	optRaw.EnableWindow(TRUE);

	if((SelParam->flags&FLAG_FUNCTION_MASK)==FLAG_FUNCTION_RAW_DATA)
	{
		optRaw.SetCheck(1);
		OnOptRawClick();
	}
	else
	switch (getm_event(SelParam->msgmask))
	{
	case CONTROLCHANGE:
		optController.SetCheck(1);
		OnOptControllerClick();
		break;
	case PITCHBEND:
		optPitchBend.SetCheck(1);
		OnOptPitchBendClick();
		break;
	case NOTE_ON:
		optNote.SetCheck(1);
		OnOptNoteClick();
		break;
	}

}

void CMidiRouterDlg::ActivateSelectedParam()
{
	intptr_t p;
	kx_fxparam_descr descr;

	if (SelParam != NULL) return;
	SelParam = parser_t->add_param();
	p = ((intptr_t) (lvwPlugins.GetFirstSelectedItemPosition())) - 1;
	SelParam->plugin = (iKXPlugin*) lvwPlugins.GetItemData((int)p);
	p = ((intptr_t) (lvwParams.GetFirstSelectedItemPosition())) - 1;
	SelParam->ndx = (int) lvwParams.GetItemData((int)p);
	SelParam->plugin->describe_param(SelParam->ndx, &descr); // no check for return value here, it's checked when lvwParams is created
	SelParam->min = descr.min_value;
	SelParam->max = descr.max_value;
	SelParam->msgmask = 0;
	SelParam->flags = 0;
}

void CMidiRouterDlg::UpdateSelParamEventInfo()
{
	int n = (int)((intptr_t) (lvwParams.GetFirstSelectedItemPosition())) - 1;
	SetParamEventInfo(n, SelParam);
}

void CMidiRouterDlg::SetParamEventInfo(int nItem, pluginparam* param)
{
	char str[32] = "";
	int x;
	if (param != NULL) {wsprintf(str, "%i", getm_channel(param->msgmask) + 1);}
	lvwParams.SetItemText(nItem, 2, str);
	if (param != NULL) {

		if((param->flags&FLAG_FUNCTION_MASK)==FLAG_FUNCTION_RAW_DATA)
		{
			// nothing to do, channel is already displayed above
			wsprintf(str, "Raw");
		}
		else
		switch (getm_event(param->msgmask))
		{
		case CONTROLCHANGE:
			wsprintf(str, mf.get_profile("lang","midi.controller_"), getm_index(param->msgmask));
			break;
		case PITCHBEND:
			lstrcpy(str, mf.get_profile("lang","midi.pitchbend"));
			break;
		case NOTE_ON:
			x = getm_index(param->msgmask);
			switch (param->flags & 0xfff)
			{
			case FLAGS_NOTE_VELOCITY:
				wsprintf(str, "%s %s%i%s", mf.get_profile("lang","midi.note"),str_midi_notes[x % 12], (x / 12) - 1, str_note_option[FUNCTION_NOTE_VELOCITY]);
				break;
			case FLAGS_NOTE_KEY:
				lstrcpy(str, mf.get_profile("lang","midi.notekey"));
				break;
			case FLAGS_NOTE_ON_TYPE1:
				wsprintf(str, "%s %s%i %s", mf.get_profile("lang","midi.note"), str_midi_notes[x % 12], (x / 12) - 1, str_note_option[FUNCTION_NOTE_ON_TYPE1]);
				break;
			case FLAGS_NOTE_ON_TYPE2:
				wsprintf(str, "%s %s%i %s", mf.get_profile("lang","midi.note"), str_midi_notes[x % 12], (x / 12) - 1, str_note_option[FUNCTION_NOTE_ON_TYPE2]);
				break;
			}
			break;
		}
	}
	lvwParams.SetItemText(nItem, 1, str);
}

void CMidiRouterDlg::OnOptNoneClick()
{
	iKXPlugin* plugin = NULL;
	int ndx = 0;

	if (SelParam != NULL) {
		plugin = SelParam->plugin;
		ndx = SelParam->ndx;
		parser_t->remove_param(SelParam);
		SelParam = NULL;
	}
	ShowParamControl(plugin, ndx);
}

void CMidiRouterDlg::OnOptControllerClick()
{
	ActivateSelectedParam();
	SetEventCombos(CMBSET_VALID_ALL, CMBSET_DEFAULT, CMBSET_NONE, CMBSET_DEFAULT, (char *)(LPCTSTR)mf.get_profile("lang","midi.ctrl"));
	SelParam->flags = FLAGS_CONTROLCHANGE;
	SelParam->msgmask &= (CHANNEL_MASK | INDEX_MASK);
	SelParam->msgmask |= CONTROLCHANGE;
	cmbChannel.SetCurSel(getm_channel(SelParam->msgmask));
	cmbIndex.SetCurSel(getm_index(SelParam->msgmask));
	UpdateSelParamEventInfo();
}

void CMidiRouterDlg::OnOptRawClick()
{
	ActivateSelectedParam();
	SetEventCombos(CMBSET_VALID_ALL, CMBSET_DEFAULT, CMBSET_NONE, CMBSET_NONE, (char *)(LPCTSTR)mf.get_profile("lang","midi.raw"));
	SelParam->flags = FLAGS_RAW;
	SelParam->msgmask &= (CHANNEL_MASK);
	cmbChannel.SetCurSel(getm_channel(SelParam->msgmask));
	UpdateSelParamEventInfo();
}

void CMidiRouterDlg::OnOptPitchBendClick()
{
	ActivateSelectedParam();
	SetEventCombos(CMBSET_VALID_ALL, CMBSET_DEFAULT, CMBSET_NONE, CMBSET_NONE);
	SelParam->flags = FLAGS_PITCHBEND;
	SelParam->msgmask &= CHANNEL_MASK;
	cmbChannel.SetCurSel(SelParam->msgmask);
	SelParam->msgmask |= PITCHBEND;
	UpdateSelParamEventInfo();
}

void CMidiRouterDlg::OnOptNoteClick()
{
	int function, cmbindexopt;
	ActivateSelectedParam();
	SelParam->msgmask &= (CHANNEL_MASK | INDEX_MASK);
	cmbindexopt = CMBSET_NOTES;
	switch (SelParam->flags & 0xfff)
	{
	case FLAGS_NOTE_VELOCITY:
		function = FUNCTION_NOTE_VELOCITY;
		break;
	case FLAGS_NOTE_KEY:
		function = FUNCTION_NOTE_KEY;
		cmbindexopt = CMBSET_NONE;
		SelParam->msgmask &= CHANNEL_MASK;
		break;
	case FLAGS_NOTE_ON_TYPE1:
		function = FUNCTION_NOTE_ON_TYPE1;
		break;
	case FLAGS_NOTE_ON_TYPE2:
		function = FUNCTION_NOTE_ON_TYPE2;
		break;
	default:
		SelParam->flags = FLAGS_NOTE_VELOCITY;
		function = FUNCTION_NOTE_VELOCITY;
		break;
	}
	SelParam->msgmask |= NOTE_ON;
	SetEventCombos(CMBSET_VALID_ALL, CMBSET_DEFAULT, CMBSET_NOTES, cmbindexopt, (char *)(LPCTSTR)mf.get_profile("lang","midi.key"));
	cmbChannel.SetCurSel(getm_channel(SelParam->msgmask));
	cmbFunction.SetCurSel(function);
	cmbIndex.SetCurSel(getm_index(SelParam->msgmask));
	UpdateSelParamEventInfo();
}

void CMidiRouterDlg::OnCmbChannelChange()
{
	setm_channel(SelParam->msgmask, cmbChannel.GetCurSel());
	UpdateSelParamEventInfo();
}

void CMidiRouterDlg::OnCmbFunctionChange()
{
	switch (getm_event(SelParam->msgmask))
	{
	case NOTE_ON:
		switch (cmbFunction.GetCurSel())
		{
		case FUNCTION_NOTE_VELOCITY:
			SelParam->flags = FLAGS_NOTE_VELOCITY;
			break;
		case FUNCTION_NOTE_KEY:
			SelParam->flags = FLAGS_NOTE_KEY;
			break;
		case FUNCTION_NOTE_ON_TYPE1:
			SelParam->flags = FLAGS_NOTE_ON_TYPE1;
			break;
		case FUNCTION_NOTE_ON_TYPE2:
			SelParam->flags = FLAGS_NOTE_ON_TYPE2;
			break;
		}
		break;
	}
	OnOptNoteClick();
}

void CMidiRouterDlg::OnCmbIndexChange()
{
	setm_index(SelParam->msgmask, cmbIndex.GetCurSel());
	UpdateSelParamEventInfo();
}

void CMidiRouterDlg::SetEventCombos(int validmask, int channel, int function, int index, char* indexlabel)
{
	int i;
	int state = TRUE;
	char str[16];

	if (validmask & CMBSET_VALID_CHANNEL)
	{
		if (channel == CMBSET_NONE) state = FALSE;
		cmbChannel.SetCurSel(-1);
		cmbChannel.EnableWindow(state);
		lblChannel.EnableWindow(state);
	}

	if (validmask & CMBSET_VALID_FUNCTION)
	{
		cmbFunction.ResetContent();
		switch (function)
		{
		case CMBSET_NOTES:
			state = TRUE;
			for (i = 0; i < NUMBER_OF_NOTE_FUNCTIONS; i++) {wsprintf(str, "%s%s", mf.get_profile("lang","midi.note"),str_note_option[i]); cmbFunction.AddString(str);}
			break;
		case CMBSET_NONE:
		default:
			state = FALSE;
			break;
		}
		cmbFunction.EnableWindow(state);
		lblFunction.EnableWindow(state);
	}

	if (validmask & CMBSET_VALID_INDEX)
	{
		cmbIndex.ResetContent();
		switch (index)
		{
		case CMBSET_NONE:
			state = FALSE;
			break;
		case CMBSET_NOTES:
			state = TRUE;
			for (i = 0; i < 128; i++) {wsprintf(str, "%s%i  (%i)", str_midi_notes[i % 12], (i / 12) - 1, i); cmbIndex.AddString(str);}
			break;
		default:
			state = TRUE;
			for (i = 0; i < 128; i++) {wsprintf(str, "%i", i); cmbIndex.AddString(str);}
			break;
		}
		cmbIndex.EnableWindow(state);
		lblIndex.EnableWindow(state);
	}

	if (validmask & CMBSET_VALID_INDEX) lblIndex.SetWindowText(indexlabel);
}
