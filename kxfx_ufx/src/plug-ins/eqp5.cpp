
#include "plugins.h"
#include "eqpx.h"
#include "eqp5.h"

//.............................................................................
// Plugin Implementation
//.............................................................................

// dsp data
#include "eqp5_dsp_.h" 

DECLARE_DSP_PROGRAM(dsp_pgm_data, eqp5_dsp); 
DECLARE_DSP_PROGRAM(dsp_pgm_datam, eqp5_dspm); 

//.............................................................................

int eqp5m_entry(int command, uintptr_t* ret )
{																						
	switch(command)																		
	{																					
	case KXPLUGIN_GET_NAME:		 // title of plugin to be shown in menus, kx dsp etc.
		strncpy((char*) ret, dsp_pgm_datam.strings[0], KX_MAX_STRING);	
		strcat((char*) ret, " (Mono)");
  		return 0;																		
	case KXPLUGIN_GET_GUID:		 // guid to identify plugin							
		strncpy((char*) ret, dsp_pgm_datam.strings[5], KX_MAX_STRING);	
  		return 0;																		
	case KXPLUGIN_INSTANTIATE:	 // actual request to create plugin					
		*ret = uintptr_t(new Eqp5(1));												
		return 0;																		
	default:																			
		*ret = NULL;																	
		return 1;																		
	}															
}

//.............................................................................

int eqp5_entry(int command, uintptr_t* ret )
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
		*ret = uintptr_t(new Eqp5);												
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
	{"[Default]", 0, 0, 0, 0, 0, 1, 10240, 0, 76, 0, 0, 15104, 0, 102, 0, 0, 24960, 0, 102, 0, 0, 34816, 0, 102, 0, 2, 39808, 0, 76},
	{"Paragraphic EQ", 0, 0, 0, 0, 1, 0, 6784, 0, 102, 1, 0, 16640, 0, 102, 1, 0, 26496, 0, 102, 1, 0, 36352, 0, 102, 1, 0, 46208, 0, 102},
	{"Mid Octaves EQ", 0, 0, 0, 0, 1, 0, 16768, 0, 76, 1, 0, 21632, 0, 76, 1, 0, 26624, 0, 76, 1, 0, 31488, 0, 76, 1, 0, 36480, 0, 76},
	{"Some EQ", 0, 0, 0, 0, 1, 0, 10880, 0, 101, 1, 0, 18688, 0, 101, 1, 0, 26624, 0, 101, 1, 0, 34432, 0, 101, 1, 0, 42240, 0, 101},
	{"Le Phone", 1, 0, 30, 0, 1, 6, 15104, 0, 76, 1, 6, 16384, 0, 64, 1, 8, 15104, 0, 102, 1, 5, 32256, 0, 68, 1, 5, 32256, 0, 76},
	{"#Enhance", 0, 0, 5, 0, 1, 1, 10240, -20, 127, 1, 1, 20096, -15, 127, 0, 0, 24960, 0, 127, 1, 2, 29952, 15, 127, 1, 2, 39808, 20, 127},
	{"#Smooth", 0, 0, -3, 0, 1, 1, 10240, 20, 127, 1, 1, 20096, 15, 127, 0, 0, 24960, 0, 127, 1, 2, 29952, -15, 127, 1, 2, 39808, -20, 127},
	{"@Presence", 0, 0, 0, 0, 0, 1, 13568, 0, 76, 0, 0, 23168, 0, 127, 1, 0, 33024, -60, 127, 1, 0, 33024, 120, 89, 0, 2, 43008, 0, 76},
	{"Brickwall - 22K", 0, 0, 0, 0, 1, 5, 48512, 0, 76, 1, 5, 48512, 0, 76, 1, 5, 48512, 0, 76, 1, 5, 48512, 0, 76, 1, 5, 48512, 0, 76},	
	{"AM Radio", 0, 0, 20, 0, 1, 6, 10240, 0, 76, 1, 6, 12544, 0, 64, 1, 8, 10240, 0, 102, 1, 5, 36480, 0, 68, 1, 5, 36480, 0, 76},
	{"Sharp Acoustic Guitar", 0, 0, -5, 0, 1, 3, 8576, -120, 51, 1, 0, 20096, 0, 102, 1, 0, 27264, 0, 102, 1, 0, 34432, 60, 102, 1, 2, 38016, 40, 127},
};

const int presets_count = (sizeof factory_presets / sizeof preset);

// parameter descriptions
const kx_fxparam_descr param_descr[params_count] = 
{
	{"Bypass", NULL, 0, 1},
	{"Mute",   NULL, 0, 1},
	{"In/Out Gain",   NULL, -150, 150},
	{"Phase Reverse",   NULL, 0, 1},
	{"Band 1 On/Off",   NULL, 0, 1},
	{"Band 1 Filter Type",   NULL, 0, 8},
	{"Band 1 Frequency",   NULL, 128, 49152},
	{"Band 1 Gain",   NULL, -180, +180},
	{"Band 1 Bandwidth",   NULL, 0, 127},
	{"Band 2 On/Off",   NULL, 0, 1},
	{"Band 2 Filter Type",   NULL, 0, 8},
	{"Band 2 Frequency",   NULL, 128, 49152},
	{"Band 2 Gain",   NULL, -180, +180},
	{"Band 2 Bandwidth",   NULL, 0, 127},
	{"Band 3 On/Off",   NULL, 0, 1},
	{"Band 3 Filter Type",   NULL, 0, 8},
	{"Band 3 Frequency",   NULL, 128, 49152},
	{"Band 3 Gain",   NULL, -180, +180},
	{"Band 3 Bandwidth",   NULL, 0, 127},
	{"Band 4 On/Off",   NULL, 0, 1},
	{"Band 4 Filter Type",   NULL, 0, 8},
	{"Band 4 Frequency",   NULL, 128, 49152},
	{"Band 4 Gain",   NULL, -180, +180},
	{"Band 4 Bandwidth",   NULL, 0, 127},
	{"Band 5 On/Off",   NULL, 0, 1},
	{"Band 5 Filter Type",   NULL, 0, 8},
	{"Band 5 Frequency",   NULL, 128, 49152},
	{"Band 5 Gain",   NULL, -180, +180},
	{"Band 5 Bandwidth",   NULL, 0, 127},
};

const char* const io_tooltips[] =
{
	"Left In",
	"Right In",
	"Left Out",
	"Right Out",
};

const char* const io_tooltipsm[] =
{
	"In",
	"Out",
};

//.............................................................................

Eqp5::Eqp5(int mono)
{
	Eqp5::mono = mono;
	set_options(UP_HAS_VIEW);
	
	if (mono)
	{
		set_dsp_pgm((kx_dsp_pgm*) &dsp_pgm_datam);
		set_io_tooltips(sizeof(io_tooltipsm) / sizeof(char*), io_tooltipsm);
		presets_guid = dsp_pgm_data.strings[5];
	}
	else
	{
		set_dsp_pgm((kx_dsp_pgm*) &dsp_pgm_data);
		set_io_tooltips(sizeof(io_tooltips) / sizeof(char*), io_tooltips);
	}

	set_factory_presets(presets_count, params_count, factory_presets); 
	memcpy(params, factory_presets[0].value, sizeof kxparam_t* params_count);
	for (int i = 0; i < N_BANDS; i++) 
	{
		reset_biquad(&biquad[i]);
		reset_biquad((biquad_t*) k[i]);
	}

	htimer = CreateWaitableTimer(NULL, FALSE, "eq_timer");
}

Eqp5::~Eqp5()
{
	if (htimer) CloseHandle(htimer);
}

int Eqp5::get_param_count()
{
	return params_count;
}

int Eqp5::describe_param(int id, kx_fxparam_descr *descr)
{
	memcpy(descr, &param_descr[id], sizeof kx_fxparam_descr); 
	return 0;
}

int Eqp5::get_param(int id, kxparam_t *value)
{
	*value = params[id]; 
	return 0;
}

const int filter_types_table[] =
{
	BELL2,
	LO_S_SHELF,
	HI_S_SHELF,
	LO_R_SHELF,
	HI_R_SHELF,
	LO_PASS,
	HI_PASS,
	BAND_PASS,
	NOTCH,
};

int Eqp5::update_iogain(double* ib, int* g)
{
	double G, IB;

	G = params[ID_IGAIN];
	if (G < 0) G *= 2.f;
	IB = cBtoG(G);

	IB = IB * k[0][0] * k[1][0] * k[2][0] * k[3][0] * k[4][0];

	G = ceil(fabs(IB));
	IB /= G;
	IB = (params[ID_IPHASE] == 0) ? IB : -IB;

	trace("ig: %.8f, %.8f\n", IB, G);

	ib[0] = IB; g[0] = (int) G;
	
	return 0;
}

#define __wait(t) if (t > 0) WaitForSingleObject(htimer, t);

int Eqp5::set_all_params(kxparam_t* values, int /* reserved */)
{
	operation |= UPOP_SET_ALL_PARAMS;

	// for (int i = 0; i < params_count; i++) set_param(i, values[i]);

	write_gpr(R_D, 0x00300000);
	write_gpr(R_BX, 0);
	__wait(150);

	set_param(ID_MUTE, 1);
	params[ID_IGAIN] = -3000;
	int i, id;
	for (i = ID_ON; i < params_count; i += N_PARAM_PER_BAND)
	{
		id = i;
		params[id] = int(values[id]); id++;
		params[id] = int(values[id]); id++;
		params[id] = int(values[id]); id++;
		params[id] = int(values[id]); id++;
		params[id] = int(values[id]); id++;
		set_param(i, int(values[i]));
	}

	if (view) 
	{
		Eqp5View* _view = (Eqp5View*) view;
		_view->lock_plot();
		for (i = (ID_ON + 1); i < params_count; i ++) _view->param_changed(i, values[i]);
		_view->lock_plot(0);
		_view->param_changed(ID_ON, values[ID_ON]); // draw plot here
	}

	operation &= ~UPOP_SET_ALL_PARAMS;

	params[ID_IGAIN] = int(values[ID_IGAIN]); 
    post_param_changed(ID_IGAIN, values[ID_IGAIN]);
	set_param(ID_BYPASS, values[ID_BYPASS]);
	set_param(ID_MUTE, values[ID_MUTE]);
	set_param(ID_IPHASE, values[ID_IPHASE]);

	return 0;
}

const word outops[][2] = {R_OUT0, R_G, R_IN0, 0x2041u, R_OUT0, 0x2040, R_OUT0, 0x2040};
const int dcfix[] = {0, 0, 0, 0, 0, 0, 0, 1, 1, 1};

int Eqp5::set_param(int id, kxparam_t value)
{
	params[id] = int(value);

	double IB;
	float f, g, q;
	int band, bpid;
	int e, x, t, l, w;
	int G;

	int UNTICLICK = 0;

	if ((id >= ID_ON) & (id < params_count))
	{
		band = (id - ID_ON) / N_PARAM_PER_BAND;
		x = band * N_PARAM_PER_BAND;
		bpid = id - x;

		if (((operation & UPOP_SET_ALL_PARAMS) == 0) && ((bpid == ID_ON) || (bpid == ID_TYPE))) UNTICLICK = 1;

		trace("UNTICLICK %x\n", UNTICLICK);

		e = params[ID_ON + x];
		f = f_param_to_hz(params[ID_FREQ + x]);
		g = params[ID_GAIN + x] * .1f;
		t = filter_types_table[params[ID_TYPE + x]];
		q = bw_param_to_q(t, params[ID_BW + x]);
		
		if (e == 0) reset_biquad(&biquad[band]);
		else compute_biquad(&biquad[band], t, f, g, q);
		eqp_transform_biquad(&biquad[band], k[band], &l);

		trace("ks: %.8f,  %.8f, %.8f, %.8f, %x\n", k[band][1], k[band][2], k[band][4], k[band][5], l);

		cache_e[band] = e; cache_t[band] = t; cache_g[band] = g; 
		cache_f[band] = f; cache_q[band] = q; cache_l[band] = l;
		post_param_changed(id, value);

		#ifndef UINEXE

		// big mute
		/*if ((params[ID_BYPASS] == 0) && (params[ID_MUTE] == 0))
		{	
			write_instr_xy(26, R_OUT0, 0x2040);
			write_instr_xy(53, R_OUT0, 0x2040);
		}*/

		update_iogain(&IB, &G);

		if (UNTICLICK)
		{
			
			write_gpr(R_D, 0x00300000);
			write_gpr(R_BX, 0);
			post_param_changed(id, value);
			w = 75; if ((e == 1) && (t == LO_PASS)) w += ((49152 - params[ID_FREQ + x]) >> 9) - 20;
			trace("wait: %ims\n", w);
			__wait(w);

			// "bypass" band
			write_instr_xy(01 + (band * 5), 0x2040u, 0x2040u);
			if (!mono) write_instr_xy(28 + (band * 5), 0x2040u, 0x2040u);
		}
		else 
		{
			if ((operation & UPOP_SET_ALL_PARAMS) == 0) post_param_changed(id, value);
		}

		write_gpr((word) (R_B02 + (band << 2)), double2fixed(k[band][2]));
		write_gpr((word) (R_A02 + (band << 2)), double2fixed(k[band][5]));
		write_gpr((word) (R_B01 + (band << 2)), double2fixed(k[band][1]));
		write_gpr((word) (R_A01 + (band << 2)), double2fixed(k[band][4]));

		if (e)
		{
			x = R_Z001 + (band << 1);
			write_instr_x(01 + (band * 5), (word) x);
			if (!mono)
			{	
				x += (R_Z101 - R_Z001);
				write_instr_x(28 + (band * 5), (word) x);
			}
		}

		write_gpr(R_G, G);

		// big unmute
		/*x = params[ID_BYPASS] + (params[ID_MUTE] << 1);
		write_instr_xy(26, (word) (outops[x][0] + 0), outops[x][1]);
		write_instr_xy(53, (word) (outops[x][0] + 1), outops[x][1]);*/

		//__wait(200);

		if ((operation & UPOP_SET_ALL_PARAMS) == 0)
		{
			write_gpr(R_D, 0x00080000);
			write_gpr(R_BX, double2fixed(IB));
		}

		// dc fix
		x = l ? 0x2041 : 0x2040;
		write_instr_a(04 + (band * 5), (word) x);
		if (!mono) write_instr_a(31 + (band * 5), (word) x);

		if (e)
		{
			// reset Z's
			if (UNTICLICK)
			{
				x = R_Z001 + (band << 1); write_gpr((word) x, 0);
				if (!mono) {x += (R_Z101 - R_Z001); write_gpr((word) x, 0);}
			}

			// enable feedback
			x = R_Z001 + (band << 1);
			write_instr_y(01 + (band * 5), (word) (l ? x : 0x2040u));
			if (!mono)
			{
				x += (R_Z101 - R_Z001);
				write_instr_y(28 + (band * 5), (word) (l ? x : 0x2040u));
			}
		}
		
		#endif

		return 0;
	}
	else
	{
		switch(id)
		{
		case ID_BYPASS:
		case ID_MUTE:
			#ifndef UINEXE
				x = params[ID_BYPASS] + (params[ID_MUTE] << 1);
				write_instr_xy(26, (word) (outops[x][0] + 0), outops[x][1]);
				if (!mono) write_instr_xy(53, (word) (outops[x][0] + 1), outops[x][1]);
			#endif
			break;

		case ID_IGAIN:
			update_iogain(&IB, &G);
			#ifndef UINEXE
				write_gpr(R_D, 0x7fffffff);
				write_gpr(R_BX, double2fixed(IB));
				write_gpr(R_G, G);
			#endif
			break;
		case ID_IPHASE:
			update_iogain(&IB, &G);
			#ifndef UINEXE
				write_gpr(R_D, 0x00200000);
				write_gpr(R_BX, double2fixed(IB));
				write_gpr(R_G, G);
			#endif
			break;

		default:
			return -1;
		}

	}

	if ((operation & UPOP_SET_ALL_PARAMS) == 0) post_param_changed(id, value);

	return 0;
}

//.............................................................................

uPluginView* Eqp5::create_view()
{
	uPluginView* pv = (uPluginView*) new Eqp5View(this);
	return pv;
}

//.............................................................................
// Plugin View Implementation
//.............................................................................

Eqp5View::Eqp5View(uPlugin* plugin)
{
	/* set plugin this view is created for */
	Eqp5View::plugin = plugin;

	ZM(mag);
	lockplot = 0;
}

const char* const str_filter_type[] =
{
	"  Bell",
	"  Low S-Shelf",
	"  High S-Shelf",
	"  Low R-Shelf",
	"  High R-Shelf",
	"  Low-Pass",
	"  High-Pass",
	"  Band-Pass",
	"  Notch"
};

int Eqp5View::open()
{
	/* set title string */
	set_title("Parametric EQ");

	/* load custom background bitmap */
	bkgbitmap.LoadBitmap(IDB_EQP5BACK);

	/* set bypass and mute */
	set_bypass(ID_BYPASS);
	set_mute(ID_MUTE);
	
	/* create controls */

	int i, j, x;

	bmpPlotBack.LoadBitmap(IDB_EQPLOT_BACK);
	bmpPlotFore.LoadBitmap(IDB_EQPLOT_FORE);
	dbxPlot.Create(WS_VISIBLE, &bmpPlotBack, 96, 8, 214, 99, this, ID_PLOT);
	dbxPlot.SetShapeFore(&bmpPlotFore, 13, 6, 196, 83);
	init_curve();

	knbIGain.Create(WS_VISIBLE, 23, 19, this, ID_IGAIN, -150, 150);
	set_control_tooltip(&knbIGain, "In/Out Gain");
	knbIGain.SetPageStepSize(10);
	IGPrint.Create(NULL, WS_VISIBLE | ULS_BOX | ULS_RIGHT, 18, 76, 51, 19, this, ID_EXTRA + 0);
	chkIPhase.Create("", WS_VISIBLE, 19, 100, 24, 8, this, ID_IPHASE);
	set_control_tooltip(&chkIPhase, "Phase Reverse");

	for (j = 0; j < 9; j++) bmp_curve[j].LoadBitmap(IDB_CURVE1 + j);

	int l = 0, idx = 0;
	for (i = 0; i < N_BANDS; i++)
	{
		cmbType[i].Create(NULL, WS_VISIBLE, 39 + l, 124, 38, 19, this, ID_TYPE + idx);
		set_control_tooltip(&cmbType[i], "Filter Type");
		x = is9X(); x <<= 1;
		for (j = 0; j < 5; j++) cmbType[i].AddItem(&str_filter_type[j][x], &bmp_curve[j]);
		cmbType[i].AddItem(NULL, MF_SEPARATOR);
		for (j; j < 9; j++) cmbType[i].AddItem(&str_filter_type[j][x], &bmp_curve[j]);

		chkOn[i].Create("", WS_VISIBLE, 12 + l, 118, 12, 20, this, ID_ON + idx);
		chkOn[i].SetLampColor(0xffcc99);
		set_control_tooltip(&chkOn[i], "Band On/Off");

		knbFreq[i].Create(WS_VISIBLE | UKS_SMALL, 12 + l, 169, this, ID_FREQ + idx, 0, 127);
		set_control_tooltip(&knbFreq[i], "Frequency");
		fdrFreqScale[i].Create(WS_VISIBLE | UFS_SWITCH, 50 + l, 169, 13, 30, this, ID_FREQSCALE + i, 0, 2);
		set_control_tooltip(&fdrFreqScale[i], "Frequency Range");
		fdrFreqScale[i].SetPos(0);
		FPrint[i].Create(NULL, WS_VISIBLE | ULS_BOX | ULS_RIGHT, 25 + l, 209, 52, 19, this, ID_EXTRA + 1 + idx);

		knbBW[i].Create(WS_VISIBLE | UKS_SMALL, 48 + l, 240, this, ID_BW + idx, 0, 127);
		set_control_tooltip(&knbBW[i], "Bandwidth/Slope/Resonance");
		QPrint[i].Create(NULL, WS_VISIBLE | ULS_BOX | ULS_LEFT, 9 + l, 245, 34, 19, this, ID_EXTRA + 2 + idx);

		knbGain[i].Create(WS_VISIBLE, 23 + l, 294, this, ID_GAIN + idx, -180, +180, -135, 270);
		knbGain[i].SetPageStepSize(10);
		set_control_tooltip(&knbGain[i], "Boost/Cut Gain");
		GPrint[i].Create(NULL, WS_VISIBLE | ULS_BOX | ULS_RIGHT, 27 + l, 340, 50, 19, this, ID_EXTRA + 3 + idx);
		idx += N_PARAM_PER_BAND;
		l += 80;
	}

	return true;
}

int Eqp5View::ucontrol_changed(int id, int data)
{
	kxparam_t value;

	if ((id >= ID_FREQSCALE) && (id < ID_EXTRA)) id = ID_FREQ + ((id - ID_FREQSCALE) * N_PARAM_PER_BAND);
	if (id >= params_count) return false;

	int band = (id - ID_ON) / N_PARAM_PER_BAND;
	int x = band * N_PARAM_PER_BAND;
	int bpid = id - x;

	switch(bpid)
	{
	case ID_TYPE:
		value = cmbType[band].GetSelItem();
		if (value > 4) value--;
		break;

	case ID_FREQ:
		value = ((knbFreq[band].GetPos() + 1) << 7) + (fdrFreqScale[band].GetPos() << 14);
		break;

	case ID_GAIN:
	case ID_BW:
		value = data;
		break;

	default:
		value = data;
		plugin->set_param(id, value);
		return true;
	}

	plugin->set_param(id, value);
	x += ID_ON;
	plugin->get_param(x, &value);
	if (value == 0) plugin->set_param(x, 1);

	return true;
}

int Eqp5View::param_changed(int id, kxparam_t value_)
{
	Eqp5* plg = (Eqp5*) plugin;

	float g = 0, f;
    int value = int(value_);
	int x;
	int fix_curve = 0;
	int band, bpid;
	int upd = 0;
	#define UPD_G	1
	#define UPD_Q	2

	if ((id >= ID_ON) & (id < params_count))
	{
		band = (id - ID_ON) / N_PARAM_PER_BAND;
		x = band * N_PARAM_PER_BAND;
		bpid = id - x;

		switch (bpid)
		{
		case ID_ON:
			chkOn[band].SetCheck(value);
			break;

		case ID_TYPE:
			if (value > 4) value++;
			cmbType[band].SelectItem(value);
			switch (plg->cache_t[band])
			{
				case LO_S_SHELF:
				case HI_S_SHELF:
				case LO_R_SHELF:
				case HI_R_SHELF:
					knbBW[band].SetLimitMin(30);
					knbGain[band].EnableWindow();
					break;
				case BELL2:
					knbBW[band].SetLimit();
					knbGain[band].EnableWindow();
					break;
				default:
					knbBW[band].SetLimit();
					knbGain[band].EnableWindow(FALSE);
					break;
			}
			upd = UPD_G | UPD_Q;
			break;

		case ID_GAIN:
			knbGain[band].SetPos(value);
			upd = UPD_G;
			break;

		case ID_FREQ: 
			x = (value - 1) >> 14;
			fdrFreqScale[band].SetPos(x);
			value = ((value  - (x << 14)) >> 7) - 1;
			knbFreq[band].SetPos(int(value));
			f = plg->cache_f[band];
			if (f > 9999) FPrint[band].printf("%.1fk", .001 * f);
			else if (f > 999) FPrint[band].printf("%.2fk", .001 * f);
			else FPrint[band].printf("%.1f", f);
			break;

		case ID_BW:
			knbBW[band].SetPos(value);
			upd = UPD_Q;
			break;

		default:
			_asm int 3;
			return false;
		}

		if (upd & UPD_G) GPrint[band].printf(knbGain[band].IsWindowEnabled() ? "%.1fdB" : "", plg->cache_g[band]);
		if (upd & UPD_Q) QPrint[band].printf("%.2f", plg->cache_q[band]);

		switch (plg->cache_t[band])
		{
			case BELL2:
				g = plg->cache_g[band];
				fix_curve = 1;
				break;
			case BAND_PASS:
				g = 0;
				fix_curve = 1;
				break;
			case NOTCH:
				g = -40;
				fix_curve = 1;
				break;
		}

		update_curve(&plg->biquad[band], plg->cache_e[band], band, fix_curve, plg->cache_f[band], g);

	}

	else
	{
		switch(id)
		{
		case ID_BYPASS:
		case ID_MUTE:
			break;

		case ID_IGAIN:
			knbIGain.SetPos(value);
			if (value < 0) value <<= 1;
			IGPrint.printf("%.1fdB", value * .1);
			break;

		case ID_IPHASE:
			chkIPhase.SetCheck(value);
			break;

		default:
			return false;
		}

	}

	return true;
}

int Eqp5View::init_curve()
{
	for (int i = 0; i < fresp_points; i++) {curvepoints[i].x = i;}
	curvepoints[fresp_points].x = fresp_points - 1;
	curvepoints[fresp_points].y = 0;
	curvepoints[fresp_points + 1].x = 0;
	curvepoints[fresp_points + 1].y = 0;
	return 0;
}

int Eqp5View::update_curve(biquad_t* biquad, int e, int band, int fix, float f, float g)
{
	const float scale =	2.02f; // ~2 pixels per dB

	float m;
	int p;
	int y;

	if (e)
	{
		compute_fresp_log(mag[band], biquad);
		if (fix)
		{
			p = find_nearest_fresp_point(f);
			mag[band][p] = (float) g;
		}
	}
	else ZM(mag[band]);

	if (lockplot) return 0;

	for (int i = 0; i < fresp_points; i++) 
	{
		m = mag[0][i] + mag[1][i] + mag[2][i] + mag[3][i] + mag[4][i];
		y = (LONG) (-.0f + (-scale * m));
		y += __sign(y);
		curvepoints[i].y = y;
	}

	dbxPlot.SetRegionPoints(curvepoints, fresp_points /* + 2*/);

	return 0;
}

