
#include "plugins.h"
#include "gainx.h"

//.............................................................................
// Plugin Implementation
//.............................................................................

// dsp data
#include "gainx_dsp.h" 

DECLARE_DSP_PROGRAM(dsp_pgm_data, gainx_dsp); 

//.............................................................................

int gainx_entry( int command, uintptr_t* ret )
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
		*ret = uintptr_t(new GainX);												
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
	{"[Default]", 0},
};

const int presets_count = (sizeof factory_presets / sizeof preset);

// parameter descriptions
const kx_fxparam_descr param_descr[params_count] = 
{
	{"Gain", NULL, -120, 120},
};

const char* const io_tooltips[] =
{
	"Left In",
	"Right In",
	"Left Out",
	"Right Out",
};

//.............................................................................

GainX::GainX()
{
	set_options(UP_HAS_BOXVIEW);
	set_dsp_pgm((kx_dsp_pgm*) &dsp_pgm_data);
	// set_factory_presets(presets_count, params_count, factory_presets); 
	set_io_tooltips(sizeof(io_tooltips) / sizeof(char*), io_tooltips);
	memcpy(params, factory_presets[0].value, sizeof kxparam_t * params_count);
}

int GainX::get_param_count()
{
	return params_count;
}

int GainX::describe_param(int id, kx_fxparam_descr *descr)
{
	memcpy(descr, &param_descr[id], sizeof kx_fxparam_descr); 
	return 0;
}

int GainX::get_param(int id, kxparam_t *value)
{
	*value = params[id]; 
	return 0;
}

#define gain_log_magic	31

int GainX::set_param(int id, kxparam_t value)
{
	params[id] = int(value);

	#ifndef UINEXE
	double g;
	int x;
	word s;
	switch(id)
	{
	case ID_GAIN:
		s = 0x2041u;
		if (value == -120) x = 0;
		else
		{
			g = value;
			if (g < -gain_log_magic) g = (g / gain_log_magic) * abs(g);
			else if (g > 95) {g = g - 120; s = 0x2044u;}
			else if (g > 60) {g = g - 95; s = 0x2043u;}
			else if (g > 0) {g = g - 60; s = 0x2042u;}
			x = double2fixed(-cBtoG(g));
		}
		write_gpr(R_K, x);
		write_instr_y(1, s);
		write_instr_y(3, s);
		break;
	}
	#endif // UINEXE

	post_param_changed(id, value); // send parameter change notification to pluginview

	return 0;
}

//.............................................................................

uPluginBoxView* GainX::create_boxview()
{
	return (uPluginBoxView*) new GainXBoxView(this);
}

//.............................................................................
// Plugin BoxView Implementation
//.............................................................................

GainXBoxView::GainXBoxView(uPlugin* plugin)
{
	/* set plugin this view is created for */
	GainXBoxView::plugin = plugin;

	/* load custom background bitmap */
	// bkgbitmap.LoadBitmap(IDB_GAINXBACK);
}

int GainXBoxView::open()
{
	/* create controls */
	knbGain.Create(WS_VISIBLE | UKS_SMALL, 0, 1, this, ID_GAIN, -120, 120);
	return true;
}

int GainXBoxView::ucontrol_changed(int id, int data)
{
	double g;
	switch(id)
	{
	case ID_GAIN:
		g = data;
		if (g == -120) {set_control_tooltip(&knbGain, "-Inf.");}
		else {g = (g < -gain_log_magic) ? (g / gain_log_magic) * abs(g) : g; printf_control_tooltip(&knbGain, "%.1fdB", g * .1);}
		break;
	default: 
		return true;
	}

	plugin->set_param(id, data);
	return true;
}

int GainXBoxView::param_changed(int id, kxparam_t value)
{
	switch(id)
	{
	case ID_GAIN: 
		knbGain.SetPos(int(value)); 
		break;
	}
	return true;
}

int GainXBoxView::redraw()
{
    knbGain.PreDrawKnob();
    return 0;
}

