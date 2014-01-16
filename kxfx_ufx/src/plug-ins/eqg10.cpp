
#include "plugins.h"
#include "eqg10.h"

//.............................................................................
// Plugin Implementation
//.............................................................................

// dsp data
#include "eqg10_dsp.h" 

DECLARE_DSP_PROGRAM(dsp_pgm_data, eqg10); 

//.............................................................................

int eqg10_entry( int command, uintptr_t* ret )
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
		*ret = uintptr_t(new EQG10);												
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
	{"[Default]", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{"Rock",  0, 0, 30, 30, 15, 0, -15, -15, 0, 15, 30, 45, 0},
	{"Pop",  0, 0, -30, -15, 0, 15, 30, 30, 15, 0, -15, -30, 0},
	{"Jazz",  0, 0,  20, 30, 30, 20, 10, 0, -10, -20, -20, -10, 0},
	{"Classic",  0, 0, -10, -20, -20, -10, 0, 10, 20, 30, 30, 20, 0},
	{"Techno", 0, 0, 30, 60, 30, 0, -60, -60, 0, 30, 60, 30, 0},
	{"Disco", 0, 0, -60, 30, 30, -30, 0, -15, -30, -30, 0, 30, 0},
	{"Heavy", 0, 0, 0, 30, 30, 0, -32, -60, -30, 30, 60, 30, 0},
	{"Radio", 0, 0, -120, -120, -120, -120, 30, 60, 28, -120, -120, -120, 0},
	{"Acquaintance of Neighbours", 0, 0, 90, 40, 0, -10, -10, -10, -10, -10, 0, 40, 0}
};

const int presets_count = (sizeof factory_presets / sizeof preset);

// parameter descriptions
const kx_fxparam_descr param_descr[params_count] = 
{
	{"Bypass",      0, 0, 1},
	{"Mute",      	0, 0, 1},
	{" 31 Hz Band", 0, -120, 120},
	{" 62 Hz Band", 0, -120, 120},
	{"125 Hz Band", 0, -120, 120},
	{"250 Hz Band", 0, -120, 120},
	{"500 Hz Band", 0, -120, 120},
	{" 1 kHz Band", 0, -120, 120},
	{" 2 kHz Band", 0, -120, 120},
	{" 4 kHz Band", 0, -120, 120},
	{" 8 kHz Band", 0, -120, 120},
	{"16 kHz Band", 0, -120, 120},
	{"Input Gain",  0, -240, 0},
};

const char* const io_tooltips[] =
{
	"Left In",
	"Right In",
	"Left Out",
	"Right Out",
};

//.............................................................................

EQG10::EQG10()
{
	set_options(UP_HAS_VIEW);
	set_dsp_pgm((kx_dsp_pgm*) &dsp_pgm_data);
	set_factory_presets(presets_count, params_count, factory_presets); 
	set_io_tooltips(sizeof(io_tooltips) / sizeof(char*), io_tooltips);
	memcpy(params, factory_presets[0].value, sizeof kxparam_t * params_count);
}

int EQG10::get_param_count()
{
	return params_count;
}

int EQG10::describe_param(int id, kx_fxparam_descr *descr)
{
	memcpy(descr, &param_descr[id], sizeof kx_fxparam_descr); 
	return 0;
}

int EQG10::get_param(int id, kxparam_t *value)
{
	*value = params[id]; 
	return 0;
}

#define _WRITEBYPASS(i, a, y) write_instr(i, 0, 0, a, 0, y, VALID_A | VALID_Y)

int EQG10::set_param(int id, kxparam_t value)
{
	// temporary fix for old EQ compability
	if ((value > param_descr[id].max_value) || 
        (value < param_descr[id].min_value)) 
            value = factory_presets[0].value[id];

	params[id] = int(value);

	#ifndef UINEXE
	switch(id)
	{
	case ID_BYPASS:
		if (value) {_WRITEBYPASS(I_BYPS1, R_INL, C_0); _WRITEBYPASS(I_BYPS2, R_INR, C_0);}
		else {_WRITEBYPASS(I_BYPS1, R_N, C_2); _WRITEBYPASS(I_BYPS2, R_N, C_2);}
		break;

	case ID_MUTE:
	case ID_GAIN:
		if (params[ID_MUTE]) set_dsp_register(R_S, 0);
		else set_dsp_register(R_S, double2fixed(cBtoG(params[ID_GAIN])));
		break;

	default: // BANDS
		set_dsp_register((word)(R_GX + R_GSTEP * (id - ID_BAND)), double2fixed(0.25 * cBtoG(value) - 0.25));
		break;
	}
	#endif // UINEXE

	post_param_changed(id, value); // send parameter change notification to pluginview

	return 0;
}

//.............................................................................

uPluginView* EQG10::create_view()
{
	uPluginView* pv = (uPluginView*) new EQG10View(this);
	return pv;
}

//.............................................................................
// Plugin View Implementation
//.............................................................................

EQG10View::EQG10View(uPlugin* plugin_)
{
	/* set plugin this view is created for */
	plugin = plugin_;
}

const char* const bandname[] = {"31", "62", "125", "250", "500", "1K", "2K", "4K", "8K", "16K"};

int EQG10View::open()
{
	/* set title string */
	set_title("Graphic EQ");

	/* load custom background bitmap */
	bkgbitmap.LoadBitmap(IDB_EQG10BACK);

	// set_texture(NULL);

	/* set bypass and mute */
	set_bypass(ID_BYPASS);
	set_mute(ID_MUTE);

	#define ID_CTRL_BASE 100
	
	/* create controls */
	fdrGain.Create(WS_VISIBLE, 47, 27, 18, 127, this, ID_GAIN, -240, 0);
	fdrGain.SetPageStepSize(10);
	lblGain.Create(NULL, ULS_RIGHT | WS_VISIBLE, 35, 159, 36, 19, this, ID_CTRL_BASE);

	int l = 126;
	for (int i = 0; i < BANDS_COUNT; i++)
	{
		fdrBand[i].Create(WS_VISIBLE, l, 27, 18, 127, this, ID_BAND + i, -120, 120);
		fdrBand[i].SetPageStepSize(10);
		l += 25;
	}

	lblBand.Create(NULL, WS_VISIBLE, 144, 159, 42, 19, this, ID_CTRL_BASE + 1);
	lblBandGain.Create(NULL, ULS_RIGHT | WS_VISIBLE, 176, 159, 28, 19, this, ID_CTRL_BASE + 2);

	return true;
}

int EQG10View::ucontrol_changed(int id, int /* data */)
{
	kxparam_t value;

	switch(id)
	{
	case ID_BYPASS:
	case ID_MUTE:
		return true;
	case ID_GAIN:
		value = fdrGain.GetPos();
		break;
	default:
		int nc = id & UC_NOTIFY_CODE_MASK;
		id &= UC_NOTIFY_ID_MASK;
		if ((id >= ID_GAIN) | (id < ID_BAND)) return true;
		// BANDS
		value = fdrBand[id - ID_BAND].GetPos();
		switch (nc)
		{
		case UCN_DEFAULT:
			break;
		case UCN_SETFOCUS:
			lblBand.printf("%sHz:", bandname[id - ID_BAND]);
			lblBandGain.printf("%.1f", value / 10.);
			// no break;
		default:
			return true;
		}
	}

	plugin->set_param(id, value);

	return true;
}

int EQG10View::param_changed(int id, kxparam_t value)
{
	switch(id)
	{
	case ID_BYPASS:
	case ID_MUTE:
		return true;
	case ID_GAIN:
		fdrGain.SetPos(int(value));
		lblGain.printf("%.1f", value / 10.);
		break;
	default: // BANDS
		fdrBand[id - ID_BAND].SetPos(int(value));
		lblBand.printf("%sHz:", bandname[id - ID_BAND]);
		lblBandGain.printf("%.1f", value / 10.);
		break;
	}

	return true;
}

