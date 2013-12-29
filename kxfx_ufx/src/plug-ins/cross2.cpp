
#include "plugins.h"
#include "cross2.h"

//.............................................................................
// Plugin Implementation
//.............................................................................

// dsp data
#include "cross2_dsp_.h" 

DECLARE_DSP_PROGRAM(dsp_pgm_data, cross2); 

//.............................................................................

int cross2_entry(int command, uintptr_t* ret)										
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
		*ret = uintptr_t(new Cross2);												
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
	{"[Default]", 358},
};

const int presets_count = (sizeof factory_presets / sizeof preset);

// parameter descriptions
const kx_fxparam_descr param_descr[params_count] = 
{
	{"Split Frequency", NULL, 113, 1600},
};

const char* const io_tooltips[] =
{
	"Left In",
	"Right In",
	"Left Low Out",
	"Left High Out",
	"Right Low Out",    
	"Right High Out",
};

//.............................................................................

Cross2::Cross2()
{
	set_options(UP_HAS_VIEW);
	set_dsp_pgm((kx_dsp_pgm*) &dsp_pgm_data);
	set_factory_presets(presets_count, params_count, factory_presets); 
	set_io_tooltips(sizeof(io_tooltips) / sizeof(char*), io_tooltips);
	memcpy(params, factory_presets[0].value, sizeof kxparam_t * params_count);
}

int Cross2::get_param_count()
{
	return params_count;
}

int Cross2::describe_param(int id, kx_fxparam_descr *descr)
{
	memcpy(descr, &param_descr[id], sizeof kx_fxparam_descr); 
	return 0;
}

int Cross2::get_param(int id, kxparam_t *value)
{
	*value = params[id]; 
	return 0;
}

int Cross2::set_param(int id, kxparam_t value)
{
	params[id] = int(value);

	#ifndef UINEXE
	int x;
	double t, k, f;

	switch(id)
	{
		case ID_FREQ:
			x = params[ID_FREQ];
			f = (((x*x) + 32) >> 5) * .1;

			t 	= tan(pi * f / FS);
			k = (1. - t) / (1. + t);
			
			write_gpr(R_K , double2fixed(k));
			break;

		/*case ID_DC1:
			write_gpr(R_DC1, value);
			break;

		case ID_DC2:
			write_gpr(R_DC2, value);
			break;*/
	}
	#endif // UINEXE

	post_param_changed(id, value); // send parameter change notification to pluginview

	return 0;
}

//.............................................................................

uPluginView* Cross2::create_view()
{
	uPluginView* pv = (uPluginView*) new Cross2View(this);
	return pv;
}

//.............................................................................
// Plugin View Implementation
//.............................................................................

Cross2View::Cross2View(uPlugin* plugin)
{
	/* set plugin this view is created for */
	Cross2View::plugin = plugin;
}

int Cross2View::open()
{
	/* set title string */
	set_title("12dB/oct");

	/* load custom background bitmap */
	// bkgbitmap.LoadBitmap(IDB_CROSS2BACK);
	size.right	= 272;
	size.bottom = 52;
	
	/* create controls */
	fdrFreq.Create(WS_VISIBLE, 12, 0 + 4 + 16, 164, 19, this, ID_FREQ, 113, 1600);
	fdrFreq.SetPageStepSize(24);
	lblFreq[0].Create("Split Freq", WS_VISIBLE | UBS_FATSMALLFONT, 12, 0 + 4, 110, 16, this, params_count + 0);
	lblFreq[1].Create("", WS_VISIBLE | ULS_BOX | ULS_RIGHT, 12 + 164 + 4, 0 + 4 + 16, 80, 19, this, params_count + 1);

	/*fdrDC1.Create(WS_VISIBLE, 12, 0 + 4 + 16 + 36, 164, 19, this, ID_DC1, -16384*4, +16384*4);
	fdrFreq.SetPageStepSize(100);
	lblDC1.Create("", WS_VISIBLE | ULS_BOX | ULS_RIGHT, 12 + 164 + 4, 0 + 4 + 16 + 36, 80, 19, this, ID_DC1 + 10);

	fdrDC2.Create(WS_VISIBLE, 12, 0 + 4 + 16 + 72, 164, 19, this, ID_DC2, -16384*4, +16384*4);
	fdrFreq.SetPageStepSize(100);
	lblDC2.Create("", WS_VISIBLE | ULS_BOX | ULS_RIGHT, 12 + 164 + 4, 0 + 4 + 16 + 72, 80, 19, this, ID_DC2 + 10);*/

	return true;
}

int Cross2View::ucontrol_changed(int id, int data)
{
	kxparam_t value;

	switch(id)
	{
	case ID_FREQ:
	//case ID_DC1:
	//case ID_DC2:
		value = data;
		break;
	default:
		return true;
	}

	plugin->set_param(id, value);

	return true;
}

int Cross2View::param_changed(int id, kxparam_t value)
{
	switch(id)
	{
	case ID_FREQ:
		fdrFreq.SetPos(int(value));
		lblFreq[1].printf("%.1f Hz", (((value*value) + 32) >> 5) * .1);
		break;
	/*case ID_DC1:
		fdrDC1.SetPos(value);
		lblDC1.printf("%i", value);
		break;
	case ID_DC2:
		fdrDC2.SetPos(value);
		lblDC2.printf("%i", value);
		break;*/
	}
	return true;
}

