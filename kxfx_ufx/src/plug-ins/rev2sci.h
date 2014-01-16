
#ifndef _U_REV2SCI_H_
#define _U_REV2SCI_H_

//.............................................................................
// Rev2Sci window

#include "../uwnd.h"
#include "upresetman.h"

class Rev2Sci : public uWnd
{
// Construction
public:
	Rev2Sci();
	virtual ~Rev2Sci();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Rev2Sci)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL Create(uPlugin* Plugin, CWnd* pParentWnd);

protected:
	uPlugin* plugin;

	uPresetManager RevPreset;

	uECombo cmbRevPreset;
	uButton btnPrintPreset;

	uHFader fdrTime[6][2];
	uLabel  lblTime[6][2];
	uVFader fdrLev[7][2];
	uLabel  lblLev[7][2];
	uCheckBox chkFbPhase[4];

	int params[2][2][16]; // time/lev, left/right, index

	int lockdsp;

	int ucontrol_changed(int id, int data);
	int read_dsp();
	int save_rev_state(int* state);
	int load_rev_state(int* state);
	int print_rev_state();

	// Generated message map functions
protected:
	//{{AFX_MSG(Rev2Sci)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif // _U_REV2SCI_H_
