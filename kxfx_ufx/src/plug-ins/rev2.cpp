
//#define _SCI_

#include "plugins.h"
#include "rev2.h"

//.............................................................................
// Plugin Implementation
//.............................................................................

// dsp data
#ifdef _SCI_
#include "rev2sci_dsp_sci.h" 
DECLARE_DSP_PROGRAM(dsp_pgm_data, rev2sci_dsp); 

#else // not SCI
#include "rev2_dsp_.h" 
DECLARE_DSP_PROGRAM(dsp_pgm_data, rev2); 

#endif _SCI_
#include "rev2_dsp_tags.h"

const word constpool[] = 
// 0.0,  0.125,  0.25,   0.50,   0.62,   0.707, (*0.06, *0.20, *0.32, *0.40)
{0x2040, 0x204b, 0x204c, 0x204d, 0x2053, 0x2054, R_CP0, R_CP1, R_CP2, R_CP3};

#define f(v) ((int) (v * 0x7fffffff))

static rev_algorithm_t algorithms[] =
{
	{
		{0x293, 0xaf9, 0x153b, 0x1acc, 0x2b96, 0x37ab, 0, 0,	// times 
		 0x34c, 0xb1c, 0x1514, 0x2234, 0x275d, 0x388e, 0, 0},
		// {-0.20, 0.32, 0.40, -0.20, -0.12, -0.12, 0.12, 0,	// taps 
		{-7, 8, 9, -7, -1, -1,  1, 0,
		//  -0.08, 0.25, 0.40, -0.12, 0.12, 0.20, -0.12, 0},
		 -6, 2, 9, -1,  1,  7, -1, 0},
		{f(0.08), f(0.20), f(0.32), f(0.40)},					// user consts
		{0x7},													// 1110
		0, 0, 0, 0,
		"Type I"
	},

	{
		{0x3fd, 0xba7, 0x1795, 0x1889, 0x1acc, 0x388e, 0, 0,	// times 
		 0x38f, 0xc90, 0x1917, 0x1a0d, 0x2234, 0x3643, 0, 0},
		// {-0.12, 0.25, 0.40, -0.20, 0.25, -0.20,  0.08, 0,	// taps 
		{-1, 2, 9, -7, 2, -7,  6, 0,
		//  -0.12, 0.25, 0.50, -0.25, 0.32, -0.25, -0.08, 0},
		 -1, 2, 3, -2, 8, -2, -6, 0},
		{f(0.08), f(0.20), f(0.32), f(0.40)},					// user consts
		{0x7},													// 1110
		0, 0, 0, 0,
		"Type II"
	},

	{
		{0x122, 0x6c2, 0x1031, 0x1d5c, 0x211d, 0x367b, 0, 0,	// times 
		 0x14f, 0x7f0, 0x1283, 0x2063, 0x30f9, 0x388e, 0, 0},
		// {0.06, 0.12, 0.20, -0.12, 0.25, -0.25, 0.50, 0,		// taps 
		{6, 1, 7, -1, 2, -2, 3, 0,
		//  0.06, 0.12, 0.20, -0.12, 0.25, -0.25, 0.50, 0},
		 6, 1, 7, -1, 2, -2, 3, 0},
		{f(0.06), f(0.20), f(0.32), f(0.40)},					// user consts
		{0xd},													// 1011
		0, 0, 0, 0,
		"Type III"
	}
};

const int alg_count = sizeof algorithms / sizeof rev_algorithm_t;

#undef f

//.............................................................................

int rev2_entry( int command, uintptr_t* ret )
{																						
	switch(command)																		
	{																					
	case KXPLUGIN_GET_NAME:		 // title of plugin to be shown in menus, kx dsp etc.
		strncpy((char*) ret, dsp_pgm_data.strings[0], KX_MAX_STRING);						
  		return 0;																		
	case KXPLUGIN_GET_GUID:		 // guid to identify plugin							
		strncpy((char*) ret, dsp_pgm_data.strings[5], KX_MAX_STRING);						
  		return 0;																		
	case KXPLUGIN_INSTANTIATE:	 // actual request to create plugin					
		*ret = uintptr_t(new Rev2);												
		return 0;																		
	default:																			
		*ret = NULL;																	
		return 1;																		
	}															
}

//.............................................................................

// factory presets
typedef Preset <params_count> preset;
const preset factory_presets[] = 
{
	{"[Default]", 0, 0, 0, 3, 1, 31, 1, 1, 68, 0, 0, 1, 0, -20, 20, 63, 75, 3, 22, 0, 25, 3, 30, 33, 18, 0},
	{"*Small Room", 0, 0, 0, 3, 1, 22, 1, 1, 10, 0, 0, 1, 0, 0, 2, 14, 100, 0, 40, 1, 16, 0, 0, 40, 20, 40},
	{"*Brite Room", 0, 0, 0, 3, 1, 38, 1, 1, 75, 0, 0, 1, 0, 0, 2, 14, 100, 0, 40, 1, 16, 5, 30, 40, 20, 40},
	{"*Dark Room", 0, 0, 0, 3, 1, 22, 1, 1, 30, 0, 0, 0, 0, -7, 2, 28, 68, 5, 29, 5, 16, 0, 30, 0, 20, 40},
	{"*Another Room", 0, 0, 0, 3, 1, 22, 1, 1, 75, 0, 0, 0, 0, 0, 12, 18, 76, 0, 40, 0, 32, 1, 30, 33, 20, 40},
	{"*Noname Box", 0, 0, 0, 3, 1, 3, 1, 1, 75, 0, 0, 0, 0, 0, 32, 63, 52, 0, 40, 5, 40, 5, 0, 40, 0, 0},
	{"*Light Hall", 0, 0, 0, 3, 1, 13, 0, 1, 100, 0, 0, 0, 0, 0, 5, 14, 100, 0, 40, 0, 17, 2, 30, 25, 20, 40},
	{"*Plastic Hall", 0, 0, 0, 3, 1, 22, 1, 1, 88, 0, 0, 0, 0, -20, 2, 32, 100, 5, 25, 0, 17, 1, 12, 17, 20, 40},
	{"*Large Hall", 0, 0, 0, 3, 1, 31, 1, 1, 68, 0, 0, 1, 0, -20, 20, 63, 75, 3, 22, 0, 25, 3, 30, 33, 18, 0},
	{"*Empty Hall", 0, 0, 0, 3, 1, 12, 2, 1, 34, 0, 0, 2, 0, -4, 11, 63, 75, 5, 9, 0, 40, 0, 8, 2, 15, 25},
	{"*Arena", 0, 0, 0, 3, 1, 3, 1, 1, 45, 0, 0, 1, 0, -20, 82, 78, 21, 0, 40, 5, 40, 5, 30, 18, 4, 0},
	{"*Plate", 0, 0, 0, 3, 1, 23, 1, 1, 70, 0, 0, 1, 0, 0, 2, 78, 100, 0, 40, 0, 8, 5, 6, 0, 8, 33},
	{"*Church", 0, 0, 0, 3, 1, 3, 1, 1, 77, 0, 0, 0, 0, 20, 20, 56, 100, 0, 40, 0, 17, 0, 17, 0, 20, 40},
	{"*Cathedral", 0, 0, 0, 3, 1, 3, 1, 1, 80, 0, 0, 1, 0, -1, 15, 82, 100, 5, 0, 0, 40, 0, 5, 32, 16, 0,},
	{"*Unreal Garage", 0, 0, 0, 3, 1, 18, 2, 1, 68, 0, 0, 2, 0, -20, 11, 75, 84, 0, 40, 5, 25, 1, 14, 25, 13, 17},
	{"*Bumerang Snare", 0, 0, 0, 3, 1, 3, 1, 1, 50, 0, 0, 2, 0, 13, 2, 14, 73, 2, 0, 1, 8, 1, 17, 26, 20, 40},
	{"*Alone On The Moon", 0, 0, 0, 3, 1, 3, 2, 1, 100, 0, 0, 2, 0, -20, 154, 99, 62, 0, 40, 3, 16, 3, 30, 25, 20, 40},
};

const int presets_count = (sizeof factory_presets / sizeof preset);

// parameter descriptions
const kx_fxparam_descr param_descr[params_count] = 
{
	{"Bypass",				0, 0, 1},
	{"Mute",				0, 0, 1},
	{"Dry On/Off",			0, 0, 1},
	{"DryMix",				0, 99, 3},
	{"Wet On/Off",			0, 0, 1},
	{"WetMix",				0, 99, 3},
	{"Early On/Off",		0, 0, 1},
	{"Rev On/Off",			0, 0, 1},
	{"Early/Rev",			0, 0, 100},
	{"[reserved]",			0, 0, 0},
	{"[reserved]",			0, 0, 0},
	{"Rev Algorithm",		0, 0, 127},
	{"[reserved]",			0, 0, 0},
	{"Rev Azimuth",			0, -20, +20},
	{"PreDelay",			0, 20, 200},
	{"Decay",				0, 14, 100},
	{"Diffuse",				0, 0, 100},
	{"Early HiCut",			0, 5, 0},
	{"Early HiCutFreq",		0, 0, 40},
	{"Early LoCut",			0, 5, 0},
	{"Early/Rev LoCutFreq",	0, 0, 40},
	{"Rev LoCut",			0, 5, 0},
	{"Rev HiCut",			0, 30, 0},
	{"Rev HiCutFreq",		0, 0, 40},
	{"Decay HiRatio",		0, 0, 20},
	{"Decay HiRatioFreq",	0, 0, 40},
};

const char* const io_tooltips[] =
{
	"Left In",
	"Right In",

	"Left Out",
	"Right Out",
	"Rear Left Out",
	"Rear Right Out",
};

//.............................................................................

Rev2::Rev2()
{
	set_options(UP_HAS_VIEW);
	set_dsp_pgm((kx_dsp_pgm*) &dsp_pgm_data);
	set_factory_presets(presets_count, params_count, factory_presets); 
	set_io_tooltips(sizeof(io_tooltips) / sizeof(char*), io_tooltips);
	memcpy(params, factory_presets[0].value, sizeof kxparam_t * params_count);
	algorithm = algorithms;
	lock = 0;

	#ifdef _SCI_
		lblShow = 0;
	#endif
}

int Rev2::get_param_count()
{
	return params_count;
}

int Rev2::describe_param(int id, kx_fxparam_descr *descr)
{
	if (id == ID_EARLYTYPE) return 1;
	else if (id == ID_EARLYSHAPE) return 1;
	else if (id == ID_REVSIZE) return 1;
	memcpy(descr, &param_descr[id], sizeof kx_fxparam_descr); 
	return 0;
}

int Rev2::get_param(int id, kxparam_t *value)
{
	*value = params[id]; 
	return 0;
}

#ifndef UINEXE
	#define read_gpr_	   read_gpr
	#define write_gpr_(a, b)     write_gpr(a, b)
	#define write_instr_op_ write_instr_op
	#define write_instr_a_ write_instr_a
	#define write_instr_y_ write_instr_y
	#define write_instr_x_ write_instr_x
	#define get_tram_addr_ get_tram_addr
	#define set_tram_addr_ set_tram_addr
	#define _write_instr_tap(offset, op, y) write_instr(offset, op, 0, 0, 0, y, VALID_OP | VALID_Y)
	#define write_instr_(offset, op, r, a, x, y, valid) write_instr(offset, op, r, a, x, y, valid)
#else
	#define read_gpr_
	#define write_gpr_
	#define write_instr_op_
	#define write_instr_a_
	#define write_instr_y_
	#define write_instr_x_
	#define get_tram_addr_
	#define set_tram_addr_
	#define _write_instr_tap
	#define write_instr_
#endif // UINEXE 

const word filteregs[] = 
{0, R_HICUTEF, 0, R_LOCUTF, 0, R_HICUTREVK, R_HICUTREVF, R_HIDECK, R_HIDECF, 0};

const word locutks[] = {0x2040, 0x204c, 0x204d, 0x2053, 0x2054, 0x204f}; 
const word hicutks[] = {0x2040, 0x204b, 0x204c, 0x204d, 0x2054, 0x204f};

const word bypass_data[2][8] = 
{
	{0x2056u, R_L, R_T1, R_DESLR0, 0x2056u, R_R, R_T1, R_DESRR0},
	{0x2040u, R_INL, 0x2040u, 0x2040u, 0x2040u, R_INR, 0x2040u, 0x2040u}
};

int Rev2::set_param(int id, kxparam_t value)
{
	double d = 0., f = 0., q = 0.;
	int l = 0, t = 0; 
	int e = 0, r = 0;
	word w = 0;

	params[id] = int(value);

	switch(id)
	{
	case ID_BYPASS:
		l = int(value) & 1;
		write_instr_(84, 0, 0, bypass_data[l][0], bypass_data[l][1], 0, VALID_A | VALID_X);
		write_instr_(85, 0, 0, bypass_data[l][2], bypass_data[l][3], 0, VALID_A | VALID_X);
		write_instr_(88, 0, 0, bypass_data[l][4], bypass_data[l][5], 0, VALID_A | VALID_X);
		write_instr_(89, 0, 0, bypass_data[l][6], bypass_data[l][7], 0, VALID_A | VALID_X);
		break;

	case ID_MUTE:
		lock = 1;
		set_param(ID_WETMUTE, params[ID_WETMUTE]);
		set_param(ID_DRYMUTE, params[ID_DRYMUTE]);
		lock = 0;
		break;	
		
	case ID_DRYMUTE:
	case ID_DRYMIX:
		if ((params[ID_DRYMIX] < 99) & (params[ID_DRYMUTE] != 0) & (params[ID_MUTE] == 0))
		{
			l = params[ID_DRYMIX];
			l = -((l * l) >> 4);
			l = double2fixed(cBtoG(l));
		}
		write_gpr_(R_DRYMIX, l);
		break;

	case ID_WETMUTE:
	case ID_WETMIX:
	case ID_ERBALANCE:
	case ID_EARLYMUTE:
	case ID_REVMUTE:
		if ((params[ID_WETMUTE] != 0) && (params[ID_MUTE] == 0))
		{
			l = params[ID_WETMIX];
			l = -((l * l) >> 4);
			d = (1. / (2.25 - 1.)) * cBtoG(l);
			if (params[ID_EARLYMUTE] != 0)
			{
				f = (100 - params[ID_ERBALANCE]) * 0.01;
				f = (2.25 - f) * f;
				e = double2fixed(f * d);
			}
			if (params[ID_REVMUTE] != 0)
			{
				f = params[ID_ERBALANCE] * 0.01;
				f = (2.25 - f) * f;
				r = double2fixed(f * d);
			}
		}
		write_gpr_(R_ELEV, e);
		write_gpr_(R_REVLEV, r);
		break;

	case ID_REVALGORITHM:
		value %= alg_count;
		params[ID_REVALGORITHM] = int(value);
		algorithm = &algorithms[int(value)];
		apply_algorithm();
		break;

	case ID_PREDELAY:
	case ID_AZIMUTH:
		l = params[ID_PREDELAY];
		r = abs(params[ID_AZIMUTH]);
		if ((l + r) > 200) {l = 200; e = 200 - (r << 1);}
		else if ((l - r) < 1) {e = 1; l = 1 + (r << 1);}
		else {e = l - r; l = l + r;}
		if (params[ID_AZIMUTH] < 0) {r = e; e = l; l = r;}
		// trace("<rev2> predelay %i, azim %i, l %i, e %i\n", params[ID_PREDELAY], params[ID_AZIMUTH], l, e);
		get_tram_addr_(R_DPL, (dword*) &t);
		set_tram_addr_(R_DPL_, t + (l * (FS / 1000)));
		get_tram_addr_(R_DPR, (dword*) &t);
		set_tram_addr_(R_DPR_, t + (e * (FS / 1000)));
		break;

	case ID_DIFFUSE:
	case ID_DECAY:
		d = params[ID_DIFFUSE] * (100 - params[ID_DECAY]) * .00011627;
		f = params[ID_DECAY] - 14;
		f = f + 10. - (10. * d);
		f = sqrt(f) * 3.919;
		write_gpr_(R_DECAY, double2fixed(f * .01));

		q = params[ID_DECAY] * .01; q = q * q;
		d = (params[ID_DIFFUSE] * (.5 - (q * .1))) + 10.;
		write_gpr_(R_DFSN, double2fixed(d * .01));

		_show("dfs %.1f, dcy %.1f", d, f);
		break;

	case ID_EARLY_HIFREQ:
	case ID_DECAY_HIFREQ:
		if (l < 40)
		{
			l = params[id];
			d = pow(2., ((l + 106) / 10.6363));
			d = exp(-pi * ((double) d / FN));
			l = double2fixed(d);
		}
		write_gpr_(filteregs[id - ID_EARLY_HICUT], l);
		break;

	case ID_REV_HIFREQ:
		if (l < 40)
		{
			l = params[id];
			d = pow(2., ((l + 177) / 16.1192));
			d = exp(-pi * ((double) d / FN));
			l = double2fixed(d);
		}
		write_gpr_(R_HICUTREVF, l);
		break;

	case ID_EARLY_LOCUT:
		w = locutks[params[id]];
		write_instr_x_(I_LOCUTEKL, w);
		write_instr_x_(I_LOCUTEKR, w);
		break;

	case ID_EARLY_HICUT:
		w = hicutks[5 - params[id]];
		write_instr_x_(R_HICUTEKL, w);
		write_instr_x_(R_HICUTEKR, w);
		break;

	case ID_REV_LOCUT:
		w = locutks[params[id]];
		write_instr_x_(R_LOCUTREVKL, w);
		write_instr_x_(R_LOCUTREVKR, w);
		break;

	case ID_REV_HICUT:
		l = params[id];
		if (l > 24) l = 0x7fffffff;
		else l = double2fixed(1. - cBtoG((double) l * -5.));
		write_gpr_(R_HICUTREVK, l);
		break;

	case ID_LOFREQ:
		l = params[ID_LOFREQ];
		if (l >= 4)
		{
			d = pow(2., ((l + 40) / 8.075187));
			d = exp(-pi * ((double) d / FN));
			l = double2fixed(d);
			write_gpr_(R_LOCUTF, l);
		}
		else
		{
			// fixme
			write_gpr_(R_LOCUTF, 0x7fffffff);
			// set filter state to zero
			write_gpr_(R_LOCUTZL, 0); write_gpr_(R_LOCUTZR, 0);
		}
		break;

	case ID_DECAY_HICUT:
		l = params[ID_DECAY_HICUT];
		if (l > 24) l = 0x7fffffff;
		else l = double2fixed((20L - l) * .05);
		write_gpr_(R_HIDECK, l);
		break;

	default:
		return 0;
	}

	if (!lock) post_param_changed(id, value);

	return 0;
}

int Rev2::apply_algorithm()
{ // return 0;
	int i, j;
	int a, b, x;
	word addr_index;

	// trace("apply_algorithm: %s\n", algorithm->name);

	// user consts
	for (i = 0; i < 4; i++)
	{
		write_gpr_(constpool[6 + i], algorithm->user_consts[i]);
	//	trace("const: %x = 0x%x\n", constpool[6 + i], algorithm->user_consts[i]);
	}

	// taps
	for(j = 0; j < 2; j++)
	{
		for(i = 0; i < 7; i++)
		{
			a = 0; /* macs */
			b = algorithm->tap_levels[j][i];
			if (b < 0) {b = -b; a = 1; /* macsn */}
			b = constpool[b];
			_write_instr_tap(I_TAPS[j][i], (word) a, (word) b);
			//trace("\ttap[%i][%i]: ofs %i, op %i, y %x\n", j, i, I_TAPS[j][i], a, b);
		}
	}

	// times
	for (i = 0; i < 2; i++) {
		addr_index = (word) ((i) ? R_DA1RW : R_DA1LW);
		get_tram_addr_(addr_index, (dword*) &a);
		for (j = 0; j < 6; j++)
		{
			x = 2 + (j << 2);
			b = algorithm->delay_times[i][j];
			set_tram_addr_((word) (addr_index + x), a + b); // set read
			//trace("\tread [%i][%i] %x, %5x\n", i, j, addr_index + x, b);
			if (j < 5)
			{
				x += 2;
				set_tram_addr_((word) (addr_index + x), a + b + 1); // next write
				//trace("\twrite[%i][%i] %x, %5x\n", i, j + 1, addr_index + x, b + 1);
			}
		}
	}

	// fdbk phases
	a = algorithm->fdbk_phases;
	for (i = 0; i < 4; i++) {write_instr_op_(I_FBPHASE[i], (word) (a & 1)); a >>= 1;}

	return 0;
}

//.............................................................................

uPluginView* Rev2::create_view()
{
	uPluginView* pv = (uPluginView*) new Rev2View(this);
	return pv;
}

//.............................................................................
// Plugin View Implementation
//.............................................................................

Rev2View::Rev2View(uPlugin* plugin_)
{
	/* set plugin this view is created for */
	plugin = plugin_;
	lock = 0;

	#ifdef _SCI_
		((Rev2*) plugin)->lblShow = &lblShow;
	#endif
}

Rev2View::~Rev2View()
{
	#ifdef _SCI_
		((Rev2*) plugin)->lblShow = 0;
	#endif
}

int Rev2View::open()
{
	/* set title string */
	// set_title("Reverb R");

	/* load custom background bitmap */
	bkgbitmap.LoadBitmap(IDB_REV2BACK);

	/* set bypass and mute */
	set_bypass(ID_BYPASS);
	set_mute(ID_MUTE);
	
	/* create controls */
	chkDryMute.Create("", WS_VISIBLE, 215, 26, 12, 8, this, ID_DRYMUTE);
	chkDryMute.SetLampColor(UC_GOOD); set_control_tooltip(&chkDryMute, param_descr[ID_DRYMUTE].name);
	chkWetMute.Create("", WS_VISIBLE, 215, 59, 12, 8, this, ID_WETMUTE);
	chkWetMute.SetLampColor(UC_NEUTRAL); set_control_tooltip(&chkWetMute, param_descr[ID_WETMUTE].name);
	chkEarlyMute.Create("", WS_VISIBLE, 201, 92, 12, 8, this, ID_EARLYMUTE);
	chkEarlyMute.SetLampColor(0xB0D0ff); set_control_tooltip(&chkEarlyMute, param_descr[ID_EARLYMUTE].name);
	chkRevMute.Create("", WS_VISIBLE, 215, 92, 12, 8, this, ID_REVMUTE);
	chkRevMute.SetLampColor(0xE0ffB0); set_control_tooltip(&chkRevMute, param_descr[ID_REVMUTE].name);
	chkX10.Create("", WS_VISIBLE, 201, 180, 12, 8, this, params_count + 1);
	set_control_tooltip(&chkX10, "PreDelay x 10");

	fdrDryMix.Create(WS_VISIBLE, 19, 36, 203, 18, this, ID_DRYMIX, 99, 3);
	fdrWetMix.Create(WS_VISIBLE, 19, 69, 203, 18, this, ID_WETMIX, 99, 3);
	fdrEarlyRev.Create(WS_VISIBLE, 19, 102, 203, 18, this, ID_ERBALANCE, 0, 100);
	fdrPreDelay.Create(WS_VISIBLE, 19, 190, 218, 18, this, ID_PREDELAY, 21, 200);
	fdrDecay.Create(WS_VISIBLE, 19, 223, 218, 18, this, ID_DECAY, 14, 100);

	knbSize.Create(WS_VISIBLE, 24 + 6, 259 + 4, this, ID_REVSIZE, 0, 100);
	knbSize.EnableWindow(FALSE);
	knbDiffuse.Create(WS_VISIBLE, 103 + 6, 259 + 4, this, ID_DIFFUSE, 0, 100);
	knbEHiCut.Create(WS_VISIBLE, 313 + 6, 64 + 4, this, ID_EARLY_HICUT, 5, 0 );
	knbEHiFreq.Create(WS_VISIBLE, 248 + 6, 41 + 4, this, ID_EARLY_HIFREQ, 0, 40);
	knbELoCut.Create(WS_VISIBLE, 447 + 6, 110 + 4, this, ID_EARLY_LOCUT, 5, 0);
	knbLoFreq.Create(WS_VISIBLE, 378 + 10, 88 + 3, this, ID_LOFREQ, 0, 40);
	knbRLoCut.Create(WS_VISIBLE, 447 + 6, 180 + 4, this, ID_REV_LOCUT, 5, 0);
	knbRHiCut.Create(WS_VISIBLE, 382 + 6, 199 + 4, this, ID_REV_HICUT, 30, 0);
	knbRHiFreq.Create(WS_VISIBLE, 319 + 6, 218 + 4, this, ID_REV_HIFREQ, 0, 40);
	knbDecHiCut.Create(WS_VISIBLE, 183 + 6, 259 + 4, this, ID_DECAY_HICUT, 0, 20);
	knbDecHiFreq.Create(WS_VISIBLE, 248 + 6, 238 + 4, this, ID_DECAY_HIFREQ, 0, 40);
	knbAzimuth.Create(WS_VISIBLE, 447 + 6, 245 + 4, this, ID_AZIMUTH, -20, 20); 
	knbShape.Create(WS_VISIBLE, 447 + 6, 33 + 4, this, ID_EARLYSHAPE, 0, 100);
	knbShape.EnableWindow(FALSE);
	
	lblOutputPrint.Create(NULL, ULS_BOX | WS_VISIBLE, 116, 134, 95, 19, this, params_count + 2);
	lblEarlyPrint.Create(NULL, ULS_BOX | WS_VISIBLE, 249, 134, 113, 19, this, params_count + 3);
	lblRevPrint.Create(NULL, ULS_BOX | WS_VISIBLE, 318, 294, 123, 19, this, params_count + 4);

	cmbRevAlgorithm.Create(NULL, UECS_DROPDOWN | WS_VISIBLE, 249, 190, 113, 19, this, ID_REVALGORITHM);
	for (int i = 0; i < alg_count; i++) cmbRevAlgorithm.AddItem(algorithms[i].name);
#ifdef _SCI_
	lblShow.Create(NULL, ULS_BOX | WS_VISIBLE, 248, 315, 272, 19, this, params_count + 11);

	btnAlgo.Create("AlgDesign", WS_VISIBLE, 18, 134, 80, 19, this, params_count + 12);
#endif
	return true;
}

int Rev2View::ucontrol_changed(int id, int /* data */)
{
	kxparam_t value;

	id &= UC_NOTIFY_ID_MASK;

	switch(id)
	{
	case ID_DRYMUTE:
	case ID_WETMUTE:
	case ID_EARLYMUTE:
	case ID_REVMUTE:
		value = ((uCheckBox*) GetDlgItem(id))->GetCheck();
		break;

	case (params_count + 1):
		id = ID_PREDELAY;
		value = fdrPreDelay.GetPos();
		if (!chkX10.GetCheck()) value /= 10;
		break;

	case ID_PREDELAY:
		id = ID_PREDELAY;
		value = fdrPreDelay.GetPos();
		if (!chkX10.GetCheck()) value /= 10;
		lock = 1;
		plugin->set_param(id, value);
		lock = 0;
		return true;

	case ID_DRYMIX:
	case ID_WETMIX:
	case ID_ERBALANCE:
	case ID_DECAY:
		value = ((uHFader*) GetDlgItem(id))->GetPos();
		break;

	case ID_REVALGORITHM:
		value = ((uECombo*) GetDlgItem(id))->GetSelItem();
		break;

	case ID_DIFFUSE:
	case ID_AZIMUTH:
	case ID_EARLY_HICUT:
	case ID_EARLY_HIFREQ:
	case ID_EARLY_LOCUT:
	case ID_LOFREQ:
	case ID_REV_LOCUT:
	case ID_REV_HICUT:
	case ID_REV_HIFREQ:
	case ID_DECAY_HICUT:
	case ID_DECAY_HIFREQ:
		value = ((uAKnob*) GetDlgItem(id))->GetPos();
		break;

#ifdef _SCI_
	case (params_count + 12):
		if (!wndAlgo.m_hWnd) wndAlgo.Create(plugin, this);
		wndAlgo.ShowWindow(SW_SHOW);
		// no break
#endif
	
	default:
		return true;
	}

	//lock = 1;
	plugin->set_param(id, value);
	//lock = 0;

	return true;
}

const char* const mutes[] = {"Dry", 0, "Wet", 0, "Early", "Rev"};
const char* const param_prints[] = 
{
	"HiCut",
	"HiCutFreq",
	"LoCut",	
	"LoCutFreq",
	"LoCut",
	"HiCut",
	"HiCutFreq",	
	"DecayHiRatio",
	"DecayHiFreq",
};

int Rev2View::param_changed(int id, kxparam_t value_)
{
	char str[64];
    int value = int(value_);

	switch(id)
	{
	case ID_DRYMUTE:
	case ID_WETMUTE:
	case ID_EARLYMUTE:
	case ID_REVMUTE:
		((uCheckBox*) GetDlgItem(id))->SetCheck(value);
		lblOutputPrint.printf("%s %s", mutes[id - ID_DRYMUTE], (value) ? "On" : "Off");
		break;

	case ID_DRYMIX:
	case ID_WETMIX:
		((uHFader*) GetDlgItem(id))->SetPos(value);
		value = -((value * value) >> 4);
		if (value < -600) lblOutputPrint.printf("%s: -Inf.", param_descr[id].name);
		else lblOutputPrint.printf("%s: %.1fdB", param_descr[id].name, value * .1);
		break;

	case ID_ERBALANCE:
		fdrEarlyRev.SetPos(value);
		lblOutputPrint.printf("Early/Rev: %i/%i", 100 - value, value );
		break;

	case ID_REVALGORITHM:
		cmbRevAlgorithm.SelectItem(value);
		break;

	case ID_PREDELAY:
		lblRevPrint.printf("PreDelay: %ims", value);
		if (value <= 20) {value *= 10; chkX10.SetCheck(false);}
		else chkX10.SetCheck(true);
		if (!lock) fdrPreDelay.SetPos(value);	
		break;

	case ID_AZIMUTH:
		str[1] = 0;
		if (value) str[0] = (value > 0) ? 'R' : 'L';
		else str[0] = 0;
		lblRevPrint.printf("Azimuth: %.1f%s", abs(value) * .4, str);
		knbAzimuth.SetPos(value);	
		break;

	case ID_DECAY:
		fdrDecay.SetPos(value);
		if (value >= 32) lblRevPrint.printf("Decay: %.1fs", (value * value) * .001);
		else lblRevPrint.printf("Decay: %.2fs", (value * value) * .001);
		break;

	case ID_DIFFUSE:
		knbDiffuse.SetPos(value);
		lblRevPrint.printf("Diffuse: %i%%", value);
		break;

	case ID_EARLY_HICUT:
	case ID_EARLY_LOCUT:
		((uAKnob*) GetDlgItem(id))->SetPos(value);
		if (0) /*(value == 0)*/ lblEarlyPrint.printf("%s: OFF", param_prints[id - ID_EARLY_HICUT]);
		// else if (value > 24) _show("%s: CUT", param_prints[id - ID_EARLY_HICUT]);
		else lblEarlyPrint.printf("%s: %i.0dB", param_prints[id - ID_EARLY_HICUT], value * 3);
		break;

	case ID_REV_LOCUT:
		((uAKnob*) GetDlgItem(id))->SetPos(value);
		if (0) /*(value == 0)*/ lblRevPrint.printf("%s: OFF", param_prints[id - ID_EARLY_HICUT]);
		// else if (value > 24) _show("%s: CUT", param_prints[id - ID_EARLY_HICUT]);
		else lblRevPrint.printf("%s: %i.0dB", param_prints[id - ID_EARLY_HICUT], value * 3);
		break;

	case ID_REV_HICUT:
		((uAKnob*) GetDlgItem(id))->SetPos(value);
		if (0) /*(value == 0)*/ lblRevPrint.printf("%s: OFF", param_prints[id - ID_EARLY_HICUT]);
		// else if (value > 24) _show("%s: CUT", param_prints[id - ID_EARLY_HICUT]);
		else lblRevPrint.printf("%s: %.1fdB", param_prints[id - ID_EARLY_HICUT], value * .5);
		break;

	case ID_EARLY_HIFREQ:
		((uAKnob*) GetDlgItem(id))->SetPos(value);
		if (value > 38) lblEarlyPrint.printf("%s: FLAT", param_prints[id - ID_EARLY_HICUT]);
		else {
			value = (int) (pow(2., ((value + 106) / 10.6363)));
			lblEarlyPrint.printf("%s: %iHz", param_prints[id - ID_EARLY_HICUT], value);
		}
		break;

	case ID_DECAY_HIFREQ:
		((uAKnob*) GetDlgItem(id))->SetPos(value);
		if (value > 38) lblRevPrint.printf("%s: FLAT", param_prints[id - ID_EARLY_HICUT]);
		else {
			value = (int) (pow(2., ((value + 106) / 10.6363)));
			lblRevPrint.printf("%s: %iHz", param_prints[id - ID_EARLY_HICUT], value);
		}
		break;

	case ID_REV_HIFREQ:
		((uAKnob*) GetDlgItem(id))->SetPos(value);
		if (value > 38) lblRevPrint.printf("%s: FLAT", param_prints[id - ID_EARLY_HICUT]);
		else {
			value = (int) (pow(2., ((value + 177) / 16.1192)));
			lblRevPrint.printf("%s: %iHz", param_prints[id - ID_EARLY_HICUT], value);
		}
		break;

	case ID_LOFREQ:
		knbLoFreq.SetPos(value);
		if (value < 4) {sprintf(str, "%s: FLAT", param_prints[id - ID_EARLY_HICUT]);}
		else {
			value = (int) (pow(2., ((value + 40) / 8.075187)));
			sprintf(str,"%s: %iHz", param_prints[id - ID_EARLY_HICUT], value);
		}
		lblEarlyPrint.SetWindowText(str);
		lblRevPrint.SetWindowText(str);
		break;

	case ID_DECAY_HICUT:
		knbDecHiCut.SetPos(value);
		lblRevPrint.printf("%s: %.2f", param_prints[id - ID_EARLY_HICUT], value * .05);
		break;
	}
	return true;
}
