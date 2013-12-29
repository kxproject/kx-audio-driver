
#include "plugins.h"
#include "timbre.h"

//.............................................................................
// Plugin Implementation
//.............................................................................

// dsp data
#include "timbre_dsp.h" 

DECLARE_DSP_PROGRAM(dsp_pgm_data, timbre); 

//.............................................................................

int timbre_entry( int command, uintptr_t* ret )
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
		*ret = uintptr_t(new Timbre);												
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
	{"[Default]", 0, 0},
	{"Harder", 30, 30},
	{"Softer", -30, -30},
	{"Darker", 30, -30},
	{"Brighter", -30, 30},
	{"Tighter", -90, -90},
	{"burN 'eM", 90, 90},
};

const int presets_count = (sizeof factory_presets / sizeof preset);

// parameter descriptions
const kx_fxparam_descr param_descr[params_count] = 
{
	{"Bass", NULL, -100, 100},
	{"Treble",   NULL, -100, 100},
};

const char* const io_tooltips[] =
{
	"Left In",
	"Right In",
	"Left Out",
	"Right Out",
};

//.............................................................................

Timbre::Timbre()
{
	set_options(UP_HAS_VIEW);
	set_dsp_pgm((kx_dsp_pgm*) &dsp_pgm_data);
	set_factory_presets(presets_count, params_count, factory_presets); 
	set_io_tooltips(sizeof(io_tooltips) / sizeof(char*), io_tooltips);
	memcpy(params, factory_presets[0].value, sizeof kxparam_t * params_count);
}

int Timbre::get_param_count()
{
	return params_count;
}

int Timbre::describe_param(int id, kx_fxparam_descr *descr)
{
	memcpy(descr, &param_descr[id], sizeof kx_fxparam_descr); 
	return 0;
}

int Timbre::get_param(int id, kxparam_t *value)
{
	*value = params[id]; 
	return 0;
}

#define BF	150
#define TF	6800

int Timbre::set_param(int id, kxparam_t value)
{
	params[id] = int(value);

	#ifndef UINEXE
	double N, F1, F2, B0, B1, A1; 

	switch(id)
	{
		case ID_BASS:
			F1 = BF;
			F2 = BF * pow(10., abs(int(value)) / 200.);
			if (value < 0) {F1 = F2; F2 = BF;};
			B1 = -exp((-pi * F2) / FN);
			A1 = -exp((-pi * F1) / FN);

			set_dsp_register(R_LB1, double2fixed(B1));
			set_dsp_register(R_LA1, double2fixed(A1));
			break;

		case ID_TREBLE:
			
			F1 = TF;
			F2 = TF / pow(10., abs(int(value)) / 200.);
			if (value < 0) {F1 = F2; F2 = TF;};
			B0 = -1.;
			B1 = exp((-pi * F2) / FN);
			A1 = -exp((-pi * F1) / FN);
			N = (1. + A1) / ((-1. + B1) * 4.);
			B0 = B0 * N; B1 = B1 * N;

			set_dsp_register(R_HB0, double2fixed(B0));
			set_dsp_register(R_HB1, double2fixed(B1));
			set_dsp_register(R_HA1, double2fixed(A1));
			break;
	}
	#endif // UINEXE

	post_param_changed(id, value); // send parameter change notification to pluginview

	return 0;
}

//.............................................................................

uPluginView* Timbre::create_view()
{
	uPluginView* pv = (uPluginView*) new TimbreView(this);
	return pv;
}

//.............................................................................
// Plugin View Implementation
//.............................................................................

TimbreView::TimbreView(uPlugin* plugin)
{
	/* set plugin this view is created for */
	TimbreView::plugin = plugin;
}

int TimbreView::open()
{
	/* set title string */
	// set_title("Tone Control");

	/* load custom background bitmap */
	bkgbitmap.LoadBitmap(IDB_TIMBREBACK);
	
	/* create controls */
	knbBass.Create(WS_VISIBLE, 25, 17, this, ID_BASS, -100, +100);
	knbBass.SetPageStepSize(10);
	knbTreble.Create(WS_VISIBLE, 88, 17, this, ID_TREBLE, -100, 100);
	knbTreble.SetPageStepSize(10);

	return true;
}

int TimbreView::ucontrol_changed(int id, int /* data */)
{
	kxparam_t value;

	switch(id)
	{
	case ID_BASS:
		value = knbBass.GetPos();
		break;
	case ID_TREBLE:
		value = knbTreble.GetPos();
		break;
	default:
		return true;
	}

	plugin->set_param(id, value);

	return true;
}

int TimbreView::param_changed(int id, kxparam_t value)
{
	switch(id)
	{
	case ID_BASS:
		knbBass.SetPos(int(value));
		printf_control_tooltip(&knbBass, "%.1f dB", value * .1);
		break;
	case ID_TREBLE:
		knbTreble.SetPos(int(value));
		printf_control_tooltip(&knbTreble, "%.1f dB", value * .1);
		break;
	}
	return true;
}

