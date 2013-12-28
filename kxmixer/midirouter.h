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


#if !defined(AFX_KXMIDIROUTER_H__8CF38337_77A4_4F06_8A5F_BB91831DFD59__INCLUDED_)
#define AFX_KXMIDIROUTER_H__8CF38337_77A4_4F06_8A5F_BB91831DFD59__INCLUDED_

class CMidiRouterDlg : public CKXDialog
{
public:
    const char *get_class_name() { return "kXAutomation"; };

    int select_device(int dev_=-1); // it is called when user changes the active device

    kStatic label;

    #define IDM_RESET	(WM_USER+0x20)
    kButton b_reset;

	virtual void init();
    	virtual void on_destroy();

    	int get_flag() { return KX_SAVED_AUTOMATION; };

	// --- any user ui-related functions here...
	// afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	int on_command(int wParam, int lParam);

// Max's stuff here:	
	afx_msg void OnListPluginSelect(LPNMLISTVIEW nmlv, LRESULT* pResult);
	afx_msg void OnListPluginDblClick(LPNMLISTVIEW nmlv, LRESULT *pResult);
	afx_msg void OnListParamSelect(LPNMLISTVIEW nmlv, LRESULT* pResult);
	afx_msg void OnOptNoneClick();
	afx_msg void OnOptRawClick();
	afx_msg void OnOptControllerClick();
	afx_msg void OnOptPitchBendClick();
	afx_msg void OnOptNoteClick();
	afx_msg void OnCmbChannelChange();
	afx_msg void OnCmbFunctionChange();
	afx_msg void OnCmbIndexChange();

	pluginparam *SelParam;

	#define CMBSET_DEFAULT		0 // "0"..."127"
	#define CMBSET_NONE		1
	#define CMBSET_NOTES		2 

	#define CMBSET_VALID_CHANNEL	1
	#define CMBSET_VALID_FUNCTION	2
	#define CMBSET_VALID_INDEX	4
	#define CMBSET_VALID_INDEXLABEL	8
	#define CMBSET_VALID_ALL	CMBSET_VALID_CHANNEL | CMBSET_VALID_FUNCTION | CMBSET_VALID_INDEX | CMBSET_VALID_INDEXLABEL

	void UpdateSelParamEventInfo();
	void SetParamEventInfo(int nItem, pluginparam* param);
	void SetEventCombos(int validmask, int channel, int function, int index, char* indexlabel = "Index:"); // see CMBSET_DEFAULT etc.
	void ActivateSelectedParam();
	void ShowParamControl(iKXPlugin *plugin = NULL, int ndx = 0);
	void ShowPluginParams(iKXPlugin* plugin);
	int select_plugin(iKXPlugin *plugin);
	int  SelectedPlugin;
	int  BuildPluginsList();
	int  CreateParserInterface();
	
	#define IDC_LIST_PLUGINS (WM_USER + 0x21)
	CListCtrl lvwPlugins;

	#define IDC_LIST_PARAMS (WM_USER + 0x22)
	CListCtrl lvwParams;

	#define IDC_GROUP_MIDIEVENT (WM_USER + 0x23)
	CStatic grpMidiEvent;

	#define IDC_OPT_NONE (WM_USER + 0x24)
	#define IDC_OPT_CONTROLLER (WM_USER + 0x25)
	#define IDC_OPT_PITCHBEND (WM_USER + 0x26)
	#define IDC_OPT_NOTE (WM_USER + 0x27)
	#define IDC_OPT_RAW (WM_USER + 0x2f)
	CButton optNone;
	CButton optController;
	CButton optPitchBend;
	CButton optNote;
	CButton optRaw;
	#define IDC_CMB_FUNCTION (WM_USER + 0x28)
	#define IDC_CMB_CHANNEL (WM_USER + 0x29)
	#define IDC_CMB_INDEX (WM_USER + 0x2a)
	CComboBox cmbFunction;
	CComboBox cmbChannel;
	CComboBox cmbIndex;
	#define IDC_LBL_FUNCTION (WM_USER + 0x2c)
	#define IDC_LBL_CHANNEL (WM_USER + 0x2d)
	#define IDC_LBL_INDEX (WM_USER + 0x2e)
	CStatic lblFunction;
	CStatic lblChannel;
	CStatic lblIndex;

	DECLARE_MESSAGE_MAP() 
};                                                   

extern CMidiRouterDlg *midi_window;

#define FUNCTION_NOTE_VELOCITY		0
#define FUNCTION_NOTE_KEY		1
#define FUNCTION_NOTE_ON_TYPE1		2
#define FUNCTION_NOTE_ON_TYPE2		3	
#define NUMBER_OF_NOTE_FUNCTIONS	4

#endif // !defined(AFX_KXMIDIROUTER_H__8CF38337_77A4_4F06_8A5F_BB91831DFD59__INCLUDED_)
