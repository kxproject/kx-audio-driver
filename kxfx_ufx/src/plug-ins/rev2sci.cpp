
#include "_kxu.h"
#include "rev2sci.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define _REV2SCI_
#define _NO_STATICS
#include "rev2sci_dsp_sci.cpp" 
#include "rev2_dsp_tags.h"

/////////////////////////////////////////////////////////////////////////////
// Rev2Sci

Rev2Sci::Rev2Sci()
{
	ZM(params);
	#define DMAXTIME	0x388e
	params[0][0][7] = DMAXTIME + 1;
	params[0][1][7] = DMAXTIME + 1;
	lockdsp = 0;
}

Rev2Sci::~Rev2Sci()
{
}

BEGIN_MESSAGE_MAP(Rev2Sci, uWnd)
	//{{AFX_MSG_MAP(Rev2Sci)
	ON_MESSAGE(WM_UCONTROL_MESSAGE, ucontrol_changed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL Rev2Sci::Create(uPlugin* Plugin, CWnd* pParentWnd)
{
	ASSERT(pParentWnd);
	plugin = Plugin;

	RevPreset.SelectPlugin("3e6150fe-3ef4-4b41-8d07-b43ed564c276");

	int r = CreateEx(WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT,
		"AlgDesign",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, 0, 800, 286,
		pParentWnd->m_hWnd, NULL);

	cmbRevPreset.Create("Default", UECS_DROPDOWN | UECS_HIDDENNOTIFIES | WS_VISIBLE, 
			492, 8, 160, 19, this, 0x1000);

	btnPrintPreset.Create("Print", WS_VISIBLE, 492, 236, 160, 19, this, 0x1001);

	#define ID_FTIME 0x000
	#define ID_LTIME 0x100

	int y = 8;
	int i, j;
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 6; j++)
		{
			fdrTime[j][i].Create(WS_VISIBLE, 130, y, 141, 18, this, ID_FTIME + j + (i << 4), 0, DMAXTIME);
			lblTime[j][i].Create(NULL, ULS_BOX | WS_VISIBLE, 8, y, 120, 18, this, ID_LTIME + j + (i << 4));
			y += 20;
		}
		y += 8;
	}

	#define ID_FLEV	0x200
	#define ID_LLEV	0x300

	int x, y2 = 116;
	y = 8;

	for (i = 0; i < 2; i++) {
		x = 280;
		for (j = 0; j < 7; j++)
		{
			fdrLev[j][i].Create(WS_VISIBLE, x, y, 18, 107, this, ID_FLEV + j + (i << 4), -75, 75);
			lblLev[j][i].Create(NULL, ULS_RIGHT | WS_VISIBLE, x, y2, 16, 14, this, ID_LLEV + j + (i << 4));
			if (j == 3) lblLev[j][i].SetColor(0x00000080);
			if (j == 5) lblLev[j][i].SetColor(0x00800000);
			x += 26;
		}
		y2 += 16;
		y += 139;
	}

	#define ID_CFBPHS	0x400
	y = 109;
	for (i = 0; i < 4; i++)
	{
		chkFbPhase[i].Create(NULL, WS_VISIBLE, 464, y, 80, 8, this, ID_CFBPHS + i);
		y += 12;
	}

	read_dsp();

	return r;
}

#ifndef UINEXE
	#define read_gpr_	   plugin->read_gpr
	#define write_gpr_     plugin->write_gpr
	#define read_instr_op_ plugin->read_instr_op
	#define write_instr_op_ plugin->write_instr_op
	#define write_instr_a_ plugin->write_instr_a
	#define write_instr_y_ plugin->write_instr_y
	#define get_tram_addr_ plugin->get_tram_addr
	#define set_tram_addr_ plugin->set_tram_addr
#else
	#define read_gpr_
	#define write_gpr_
	#define read_instr_op_ 
	#define write_instr_op_
	#define write_instr_a_
	#define write_instr_y_
	#define get_tram_addr_
	#define set_tram_addr_
#endif // UINEXE 

#define revstatecount	30

int Rev2Sci::print_rev_state()
{

	int v = 0; 
	word ti = 0;
	int to = 0;
	int i, j;

	trace("{\n");
	// times
	trace("\t{");
	for (i = 0; i < 2; i++) {
		ti = (word) ((i) ? R_DA1RW : R_DA1LW);
		get_tram_addr_(ti, (dword*) &to);
		for (j = 0; j < 6; j++)
		{
			get_tram_addr_((word) (ti + 2 + (j << 2)), (dword*) &v);
			trace(" 0x%x,", (dword) v - to);
		}
		trace((i) ? "},\n" : " // times \n\t");
	}

	// levels
	trace("\t{");
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 7; j++)
		{
			read_gpr_((word) (R_AL1 + j + (i * 7)), (dword*) &v);
			trace(" %.2f,", (double) v / 0x7fffffff);
		}
		trace((i) ? "},\n" : " // taps \n\t");
	}

	trace("\t{0, 0, 0, 0}, // user consts\n");

	// feedback phases
	trace("\t{");
	to = 0;
	for (i = 0; i < 4; i++)
	{
		read_instr_op_(I_FBPHASE[i], &ti);
		to += (ti << i);
	}
	trace("0x%x}, // %i%i%i%i\n", to, to & 1, (to >> 1) & 1, (to >> 2) & 1, (to >> 3) & 1);

	trace("}\n");
	return 0;
}

int Rev2Sci::read_dsp()
{
	int v = 0; 
	word ti = 0;
	int to = 0;
	int i, j;

	lockdsp = 1;

	// levels
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 7; j++)
		{
			read_gpr_((word) (R_AL1 + j + (i * 7)), (dword*) &v);
			fdrLev[j][i].SetPos(v / (0x7fffffff / 100));
			ucontrol_changed(ID_FLEV + j + (i << 4), 0);
		}
	}

	// times
	for (i = 0; i < 2; i++) {
		ti = (word) ((i) ? R_DA1RW : R_DA1LW);
		get_tram_addr_(ti, (dword*) &to);
		trace("to: %x\n", to);
		for (j = 5; j >= 0; j--)
		{
			get_tram_addr_((word) (ti + 2 + (j << 2)), (dword*) &v);
			trace("get: %x, %x\n", (word) (ti + 2 + (j << 2)), (dword) v - to);
			fdrTime[j][i].SetPos(v - to);
			ucontrol_changed(ID_FTIME + j + (i << 4), 0);
		}
	}

	// feedback phases
	for (i = 0; i < 4; i++)
	{
		read_instr_op_(I_FBPHASE[i], &ti);
		chkFbPhase[i].SetCheck(ti);
		ucontrol_changed(ID_CFBPHS + i, 0);
	}

	lockdsp = 0;

	return 0;
}

int Rev2Sci::save_rev_state(int* state)
{
	int p = 0;
	int i, j;

	// levels
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 7; j++)
		{
			state[p] = fdrLev[j][i].GetPos();
			p++;
		}
	}

	// times
	for (i = 0; i < 2; i++) {
		for (j = 5; j >= 0; j--)
		{
			state[p] = fdrTime[j][i].GetPos();
			p++;
		}
	}

	// feedback phases
	for (i = 0; i < 4; i++)
	{
		state[p] = chkFbPhase[i].GetCheck();
		p++;
	}

	return 0;
}

int Rev2Sci::load_rev_state(int* state)
{
	int p = 0;
	int i, j;

	// levels
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 7; j++)
		{
			fdrLev[j][i].SetPos(state[p]);
			ucontrol_changed(ID_FLEV + j + (i << 4), 0);
			p++;
		}
	}

	// times
	for (i = 0; i < 2; i++) {
		for (j = 5; j >= 0; j--)
		{
			fdrTime[j][i].SetPos(state[p]);
			ucontrol_changed(ID_FTIME + j + (i << 4), 0);
			p++;
		}
	}

	// feedback phases
	for (i = 0; i < 4; i++)
	{
		chkFbPhase[i].SetCheck(state[p]);
		ucontrol_changed(ID_CFBPHS + i, 0);
		p++;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Rev2Sci message handlers

_inline int _limit(int v, int min, int max)
{
	if (v < min) v = min;
	if (v > max) v = max;
	return v;
}


int Rev2Sci::ucontrol_changed(int id, int data)
{
	char text[MAX_ECOMBO_STRING];
	int revstate[revstatecount];

	int rv;

	int t = 0xf & (id >> 9);
	int c = 0xf & (id >> 4);
	int i = 0xf & id;

	int* pparam = params[t][c];

	int to;
	word ti;

	switch (t)
	{
	case 0: // time
		ti = (word) ((c) ? R_DA1RW : R_DA1LW);
		get_tram_addr_(ti, (dword*) &to);
		data = _limit(fdrTime[i][c].GetPos(), pparam[i], pparam[i + 2] - 1);
		pparam[i + 1] = data;
		data -= pparam[i];
		lblTime[i][c].printf("%.1f, %.1f, %i", pparam[i + 1] / 48., data / 48., data);
		if (!lockdsp) set_tram_addr_((word) (ti + 2 + (i << 2)), (dword) (to + pparam[i + 1]));
		trace("setR: %x, %x\n", (word) (ti + 2 + (i << 2)), (dword) pparam[i + 1]);
		if (i < 5)
		{
			data = pparam[i + 2] - (pparam[i + 1] + 1);
			lblTime[i + 1][c].printf("%.1f, %.1f, %i", pparam[i + 2] / 48., data / 48., data);
			if (!lockdsp) set_tram_addr_((word) (ti + 4 + (i << 2)), (dword) (to + pparam[i + 1] + 1));
			trace("setW: %x, %x\n", (word) (ti + 4 + (i << 2)), (dword) (pparam[i + 1] + 1));
		}
		break;

	case 1: // lev
		data = fdrLev[i][c].GetPos();
		pparam[i] = data;
		lblLev[i][c].printf("%i", data);
		if (!lockdsp) write_gpr_((word) (R_AL1 + i + (c*7)), data * (0x7fffffff / 100));
		break;

	case 2: // feedphase
		data = chkFbPhase[i].GetCheck();
		if (!lockdsp) write_instr_op_(I_FBPHASE[i], (word) data);
		break;

	case 8: // other controls
		switch (id)
		{
		case 0x1000 | UCBN_DROPDOWN: // rev preset // before dropdown: fill combo with presets 
			cmbRevPreset.ResetItems();
			cmbRevPreset.AddItem("Save...");
			cmbRevPreset.AddItem("Delete");
			cmbRevPreset.AddItem(NULL, MF_SEPARATOR);
			#define PRESETS_OFFSET 3
			cmbRevPreset.__setitemsoffset(PRESETS_OFFSET);
			i = 0;
			while (1)
			{
				rv = RevPreset.EnumPresets(i, MAX_ECOMBO_STRING, text);
				if (rv) break;
				cmbRevPreset.AddItem(text);
				i++;
			}
			break;

		case 0x1000: // rev preset
			switch (data)
			{
			case 0: // save preset
				rv = AskForString(this, "Save Preset", text);
				if (rv == IDOK)
				{
					rv = RevPreset.ReadPreset(text, revstatecount, revstate);
					save_rev_state(revstate);
					rv = RevPreset.WritePreset(text, revstatecount, revstate);
					if (rv == 0) cmbRevPreset.SetWindowText(text);
				}
				break;
			case 1: // delete preset
				cmbRevPreset.GetWindowText(text, MAX_ECOMBO_STRING);
				rv = MessageBox("Delete preset?", "AlgDesign", MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2);
				if (rv == IDYES) 
				{
					RevPreset.RemovePreset(text);
					cmbRevPreset.SetWindowText("Custom");
				}
				break;
			default: // select preset here
				cmbRevPreset.GetWindowText(text, MAX_ECOMBO_STRING);
				if (!RevPreset.ReadPreset(text, revstatecount, revstate)) load_rev_state(revstate);
				break;
			}
			break;

		case 0x1001: // print
			print_rev_state();
			break;
		}
	}

	return true;
}


//.............................................................................
