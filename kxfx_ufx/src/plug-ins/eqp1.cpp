
#include "plugins.h"
#include "eqpx.h"
#include "eqp1.h"

//.............................................................................
// Plugin Implementation
//.............................................................................

// dsp data
#include "eqp1_dsp_.h" 

DECLARE_DSP_PROGRAM(dsp_pgm_data, eqp1); 
DECLARE_DSP_PROGRAM(dsp_pgm_datam, eqp1m); 

//.............................................................................

int Eqp1m_entry( int command, uintptr_t* ret )
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
		*ret = uintptr_t(new Eqp1(1));												
		return 0;																		
	default:																			
		*ret = NULL;																	
		return 1;																		
	}															
}

//.............................................................................

int Eqp1_entry( int command, uintptr_t* ret )
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
		*ret = uintptr_t(new Eqp1);												
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
	{"[Default]", 0, 0, 0, 0, 1, 0, 24576, 0, 101},
	{"Bass Boost", 0, 0, -5, 0, 1, 1, 13568, 60, 76},
	{"DC Removal", 0, 0, 0, 0, 1, 6, 128, 60, 76},
	{"Add Air", 0, 0, 0, 0, 1, 0, 45824, 40, 72},
	{"SubBass Boost", 0, 0, 0, 0, 1, 1, 6528, 60, 76},
	{"Resonant LP (tweak Freq to 'wah')", 0, 0, -35, 0, 1, 5, 18560, 60, 0},
	{"50Hz Hum Notch", 0, 0, 0, 0, 1, 8, 5248, 0, 35},
	{"60Hz Hum Notch", 0, 0, 0, 0, 1, 8, 6528, 0, 35},
	{"Voice: Go away from Mike", 0, 0, 0, 0, 1, 0, 21248, -40, 63},
	{"Make Me Older", 1, 0, 0, 0, 1, 7, 24704, 0, 116},
	{"Darker? Darker!", 0, 0, 10, 0, 1, 5, 36480, 0, 62},
	{"Brighter", 0, 0, -5, 0, 1, 2, 29440, 30, 127},
	{"Valver", 0, 0, -15, 0, 1, 3, 27904, 40, 30},
};

const int presets_count = (sizeof factory_presets / sizeof preset);

// parameter descriptions
const kx_fxparam_descr param_descr[params_count] = 
{
	{"Bypass", NULL, 0, 1},
	{"Mute",   NULL, 0, 1},
	{"I/O Gain",   NULL, -150, 150},
	{"Phase Reverse",   NULL, 0, 1},
	{"(Reserved)",   NULL, 0, 1},
	{"Filter Type",   NULL, 0, 8},
	{"Frequency",   NULL, 128, 49152},
	{"Gain",   NULL, -180, +180},
	{"Bandwidth",   NULL, 0, 127},
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

Eqp1::Eqp1(int mono)
{
	Eqp1::mono = mono;
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
    reset_biquad(&biquad);
	reset_biquad((biquad_t*) k);

	htimer = CreateWaitableTimer(NULL, FALSE, "eq_timer");
}

Eqp1::~Eqp1()
{
	if (htimer) CloseHandle(htimer);
}

int Eqp1::get_param_count()
{
	return params_count;
}

int Eqp1::describe_param(int id, kx_fxparam_descr *descr)
{
	memcpy(descr, &param_descr[id], sizeof kx_fxparam_descr); 
	return 0;
}

int Eqp1::get_param(int id, kxparam_t *value)
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

int Eqp1::update_iogain(double* ib, int* g)
{
	double G, IB;

	G = params[ID_IGAIN];
	if (G < 0) G *= 2.f;
	IB = cBtoG(G);

	IB = IB * k[0];

	G = ceil(fabs(IB));
	IB /= G;
	IB = (params[ID_IPHASE] == 0) ? IB : -IB;

	trace("ig: %.8f %.8f\n", IB, G);

	ib[0] = IB; g[0] = (int) G;
	
	return 0;
}

#define __wait(t) if (t > 0) WaitForSingleObject(htimer, t);

int Eqp1::set_all_params(kxparam_t* values, int /* reserved */)
{
	int i, id;
	operation |= UPOP_SET_ALL_PARAMS;

	// for (i = 0; i < params_count; i++) set_param(i, values[i]);
    /*
	write_gpr(R_D, 0x00300000);
	write_gpr(R_BX, 0);
	__wait(150);
	*/
	set_param(ID_MUTE, 1);
	params[ID_IGAIN] = -3000;
	
	// hello from eqp5 (overhead code), fixme
	id = ID_ON;
	params[id] = int(values[id]); id++;
	params[id] = int(values[id]); id++;
	params[id] = int(values[id]); id++;
	params[id] = int(values[id]); id++;
	params[id] = int(values[id]); id++;
	set_param(ID_ON, values[ID_ON]);

	if (view) 
	{
		Eqp1View* _view = (Eqp1View*) view;
		_view->lock_plot();
		for (i = (ID_ON + 1); i < params_count; i++) _view->param_changed(i, values[i]);
		_view->lock_plot(0);
		_view->param_changed(ID_ON, values[ID_ON]); // draw plot here
	}

	operation &= ~UPOP_SET_ALL_PARAMS;

	params[ID_IGAIN] = int(values[ID_IGAIN]); 
    post_param_changed(ID_IGAIN, int(values[ID_IGAIN]));
	set_param(ID_BYPASS, int(values[ID_BYPASS]));
	set_param(ID_MUTE, int(values[ID_MUTE]));
	set_param(ID_IPHASE, int(values[ID_IPHASE]));
    
	return 0;
}

const word outops[][2] = {R_OUT0, R_G, R_IN0, 0x2041u, R_OUT0, 0x2040, R_OUT0, 0x2040};
const int dcfix[] = {0, 0, 0, 0, 0, 0, 0, 1, 1, 1};

int Eqp1::set_param(int id, kxparam_t value)
{
	params[id] = int(value);

	double IB;
	float f, g, q;
	int x, y, e, t, l, w;
	int G;

	int UNTICLICK = 0;

	if ((id >= ID_ON) & (id < params_count))
	{
		if (((operation & UPOP_SET_ALL_PARAMS) == 0) && ((id == ID_ON) || (id == ID_TYPE))) UNTICLICK = 1;

		trace("UNTICLICK %x\n", UNTICLICK);

		e = 1;
		f = f_param_to_hz(params[ID_FREQ]);
		g = params[ID_GAIN] * .1f;
		t = filter_types_table[params[ID_TYPE]];
		q = bw_param_to_q(t, params[ID_BW]);
		
		if (e == 0) reset_biquad(&biquad);
		else compute_biquad(&biquad, t, f, g, q);
		eqp_transform_biquad(&biquad, k, &l);

		trace("ks: %.8f  %.8f %.8f %.8f %x\n", k[1], k[2], k[4], k[5], l);

		cache_e = e; cache_t = t; cache_g = g; 
		cache_f = f; cache_q = q; cache_l = l;
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
			
			//write_gpr(R_D, 0x00300000);
			write_gpr(R_B0, 0);
			write_gpr(R_G, 0);
			//post_param_changed(id, value); // ???
			w = 100; 
			//if ((e == 1) && (t == LO_PASS)) w += ((49152 - params[ID_FREQ + x]) >> 9) - 20;
			trace("wait: %ims\n", w);
			__wait(w);
			//

			// "bypass" band
			//write_instr_xy(6, 0x2040u, 0x2040u);
			//write_instr_xy(13, 0x2040u, 0x2040u);
			
		}
		else 
		{
			if ((operation & UPOP_SET_ALL_PARAMS) == 0) post_param_changed(id, value);
		}

		write_gpr((word) (R_B2), double2fixed(k[2]));
		write_gpr((word) (R_A2), double2fixed(k[5]));
		write_gpr((word) (R_B1), double2fixed(k[1]));
		write_gpr((word) (R_A1), double2fixed(k[4]));

		if (e)
		{/*
			x = R_Z001 + (band << 1);
			write_instr_x(01 + (band * 5), (word) x);
			x += (R_Z101 - R_Z001);
			write_instr_x(28 + (band * 5), (word) x);*/
		}

		// big unmute
		//x = params[ID_BYPASS] + (params[ID_MUTE] << 1);
		//write_instr_xy(26, (word) (outops[x][0] + 0), outops[x][1]);
		//write_instr_xy(53, (word) (outops[x][0] + 1), outops[x][1]);

		//__wait(200);

		/*if ((operation & UPOP_SET_ALL_PARAMS) == 0)
		{
			//write_gpr(R_D, 0x00080000);
			write_gpr(R_B0, double2fixed(IB));
		}*/

		// dc fix
		x = 0x2040; y = 0x2040;
		if (l && (k[4] > 0.)) {x = 0x2041; y = R_G;}
		write_instr_a(04, (word) x);
		write_instr_a(06, (word) y);
		if (!mono) 
		{
			write_instr_a(11, (word) x);
			write_instr_a(13, (word) y);
		}

		if (e)
		{
			// reset Z's
			if (UNTICLICK)
			{
				write_gpr((word) R_Z1, 0);
				write_gpr((word) R_Z2, 0);
				if (!mono) 
				{
					write_gpr((word) R_Z3, 0);
					write_gpr((word) R_Z4, 0);
				}
			}

			// enable feedback ?
			write_instr_y(1, (word) (l ? R_Z1 : 0x2040u));
			if (!mono) write_instr_y(8, (word) (l ? R_Z3 : 0x2040u));
		}

		write_gpr(R_B0, double2fixed(IB));
		write_gpr(R_G, G);
		

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
				write_instr_xy(6,  (word) (outops[x][0] + 0), outops[x][1]);
				if (!mono) write_instr_xy(13, (word) (outops[x][0] + 1), outops[x][1]);
			#endif
			break;

		case ID_IGAIN:
			update_iogain(&IB, &G);
			#ifndef UINEXE
				//write_gpr(R_D, 0x7fffffff);
				write_gpr(R_B0, double2fixed(IB));
				write_gpr(R_G, G);
			#endif
			break;
		case ID_IPHASE:
			update_iogain(&IB, &G);
			#ifndef UINEXE
				//write_gpr(R_D, 0x00200000);
				write_gpr(R_B0, double2fixed(IB));
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

uPluginView* Eqp1::create_view()
{
	uPluginView* pv = (uPluginView*) new Eqp1View(this);
	return pv;
}

//.............................................................................
// Plugin View Implementation
//.............................................................................

Eqp1View::Eqp1View(uPlugin* plugin)
{
	/* set plugin this view is created for */
	Eqp1View::plugin = plugin;

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

int Eqp1View::open()
{
	/* set title string */
	set_title("Parametric EQ");

	/* load custom background bitmap */
	bkgbitmap.LoadBitmap(IDB_EQP1BACK);

	/* set bypass and mute */
	set_bypass(ID_BYPASS);
	set_mute(ID_MUTE);
	
	/* create controls */

	int j, x;

	bmpPlotBack.LoadBitmap(IDB_EQPLOT_BACK);
	bmpPlotFore.LoadBitmap(IDB_EQPLOT_FORE);
	dbxPlot.Create(WS_VISIBLE, &bmpPlotBack, 78, 10, 136, 99, this, ID_PLOT);
	dbxPlot.SetShapeFore(&bmpPlotFore, 13, 6, 118, 83);
	init_curve();

	knbIGain.Create(WS_VISIBLE | UKS_SMALL, 21, 40, this, ID_IGAIN, -150, 150);
	set_control_tooltip(&knbIGain, "In/Out Gain");
	knbIGain.SetPageStepSize(10);
	IGPrint.Create(NULL, WS_VISIBLE | ULS_BOX | ULS_RIGHT, 10, 77, 50, 19, this, ID_EXTRA + 0);
	chkIPhase.Create("", WS_VISIBLE, 8, 10, 12, 8, this, ID_IPHASE);
	set_control_tooltip(&chkIPhase, "Phase Reverse");

	for (j = 0; j < 9; j++) bmp_curve[j].LoadBitmap(IDB_CURVE1 + j);

	cmbType.Create(NULL, WS_VISIBLE, 234, 10, 39, 19, this, ID_TYPE);
	set_control_tooltip(&cmbType, "Filter Type");
	x = is9X(); x <<= 1;
	for (j = 0; j < 5; j++) cmbType.AddItem(&str_filter_type[j][x], &bmp_curve[j]);
	cmbType.AddItem(NULL, MF_SEPARATOR);
	for (j; j < 9; j++) cmbType.AddItem(&str_filter_type[j][x], &bmp_curve[j]);

	knbFreq.Create(WS_VISIBLE | UKS_SMALL, 268, 51, this, ID_FREQ, 0, 127);
	set_control_tooltip(&knbFreq, "Frequency");
	fdrFreqScale.Create(WS_VISIBLE | UFS_SWITCH, 244, 51, 13, 30, this, ID_FREQSCALE, 0, 2);
	set_control_tooltip(&fdrFreqScale, "Frequency Range");
	fdrFreqScale.SetPos(0);
	FPrint.Create(NULL, WS_VISIBLE | ULS_BOX | ULS_RIGHT, 234, 90, 50, 19, this, ID_EXTRA + 1);

	knbBW.Create(WS_VISIBLE | UKS_SMALL, 312, 18, this, ID_BW, 0, 127);
	set_control_tooltip(&knbBW, "Bandwidth/Slope/Resonance");
	QPrint.Create(NULL, WS_VISIBLE | ULS_BOX | ULS_CENTER, 346, 23, 50, 19, this, ID_EXTRA + 2);

	knbGain.Create(WS_VISIBLE | UKS_SMALL, 312, 72, this, ID_GAIN, -180, +180, -135, 270);
	knbGain.SetPageStepSize(10);
	set_control_tooltip(&knbGain, "Boost/Cut Gain");
	GPrint.Create(NULL, WS_VISIBLE | ULS_BOX | ULS_RIGHT, 346, 77, 50, 19, this, ID_EXTRA + 3);

	return true;
}

int Eqp1View::ucontrol_changed(int id, int data)
{
	kxparam_t value;

	if (id == ID_FREQSCALE) id = ID_FREQ;
	else if (id >= params_count) return false;
	

	switch(id)
	{
	case ID_TYPE:
		value = cmbType.GetSelItem();
		if (value > 4) value--;
		break;

	case ID_FREQ:
		value = ((knbFreq.GetPos() + 1) << 7) + (fdrFreqScale.GetPos() << 14);
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

	return true;
}

int Eqp1View::param_changed(int id, kxparam_t value)
{
	Eqp1* plg = (Eqp1*) plugin;

	float g = 0, f;
	int x;
	int fix_curve = 0;
	int upd = 0;
	#define UPD_G	1
	#define UPD_Q	2

	if ((id >= ID_ON) & (id < params_count))
	{
		switch (id)
		{
		case ID_ON:
			// do nothing
			break;

		case ID_TYPE:
			if (value > 4) value++;
			cmbType.SelectItem(int(value));
			switch (plg->cache_t)
			{
				case LO_S_SHELF:
				case HI_S_SHELF:
				case LO_R_SHELF:
				case HI_R_SHELF:
					knbBW.SetLimitMin(30);
					knbGain.EnableWindow();
					break;
				case BELL2:
					knbBW.SetLimit();
					knbGain.EnableWindow();
					break;
				default:
					knbBW.SetLimit();
					knbGain.EnableWindow(FALSE);
					break;
			}
			upd = UPD_G | UPD_Q;
			break;

		case ID_GAIN:
			knbGain.SetPos(int(value));
			upd = UPD_G;
			break;

		case ID_FREQ: 
			x = (int(value) - 1) >> 14;
			fdrFreqScale.SetPos(x);
			value = ((value  - (x << 14)) >> 7) - 1;
			knbFreq.SetPos(int(value));
			f = plg->cache_f;
			if (f > 9999) FPrint.printf("%.1fk", .001 * f);
			else if (f > 999) FPrint.printf("%.2fk", .001 * f);
			else FPrint.printf("%.1f", f);
			break;

		case ID_BW:
			knbBW.SetPos(int(value));
			upd = UPD_Q;
			break;

		default:
			_asm int 3;
			return false;
		}

		if (upd & UPD_G) GPrint.printf(knbGain.IsWindowEnabled() ? "%.1fdB" : "", plg->cache_g);
		if (upd & UPD_Q) QPrint.printf("%.2f", plg->cache_q);

		switch (plg->cache_t)
		{
			case BELL2:
				g = plg->cache_g;
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

		update_curve(&plg->biquad, plg->cache_e, 0, fix_curve, plg->cache_f, g);

	}

	else
	{
		switch(id)
		{
		case ID_BYPASS:
		case ID_MUTE:
			break;

		case ID_IGAIN:
			knbIGain.SetPos(int(value));
			if (value < 0) value <<= 1;
			IGPrint.printf("%.1fdB", value * .1);
			break;

		case ID_IPHASE:
			chkIPhase.SetCheck(int(value));
			break;

		default:
			return false;
		}

	}

	return true;
}

int Eqp1View::init_curve()
{
	for (int i = 0; i < fresp_points; i++) {curvepoints[i].x = i;}
	curvepoints[fresp_points].x = fresp_points - 1;
	curvepoints[fresp_points].y = 0;
	curvepoints[fresp_points + 1].x = 0;
	curvepoints[fresp_points + 1].y = 0;
	return 0;
}

int Eqp1View::update_curve(biquad_t* biquad, int e, int /* band */, int fix, float f, float g)
{
	#define scale	2.02f // ~2 pixels per dB

	float m;
	int p;
	int y;

	if (e)
	{
		compute_fresp_log(mag, biquad);
		if (fix)
		{
			p = find_nearest_fresp_point(f);
			mag[p] = (float) g;
		}
	}
	else ZM(mag);

	if (lockplot) return 0;

	for (int i = 0; i < fresp_points; i++) 
	{
		m = mag[i];
		y = (LONG) (-.0f + (-scale * m));
		y += __sign(y);
		curvepoints[i].y = y;
	}

	const int freq_scale_shift[] = {0, 42, 78};
	y = fdrFreqScale.GetPos(); y = freq_scale_shift[y];
	dbxPlot.SetRegionPoints(curvepoints, fresp_points /* + 2*/, y);

	return 0;
}

