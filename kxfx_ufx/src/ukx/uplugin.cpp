
#include "ukx.h"
#include "uplugin.h"

//.............................................................................
// Global Stuff

HMODULE hmodule;
static kxuCore *ucore;

kxuCore* uGetCore() {return ucore;} 
int uInit() {ucore = new kxuCore; return 0;}
int uEnd() {delete ucore;  return 0;}

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID /* lpvReserved */)
{
	switch (fdwReason)
    {
		case DLL_PROCESS_ATTACH:
            hmodule = hinstDLL; 
			uInit();
			break;

		case DLL_PROCESS_DETACH:
			uEnd();
			break;
    }

	return TRUE;
}

//.............................................................................
// Dummy DSP Program

dsp_code dummy_code = {0x0,0x2040,0x2040,0x2040,0x2040};

kx_dsp_pgm dummy_dsp_pgm = {	
	"© Max Mikhailov, 2002-2014",						
	"kX", "Dummy Plugin (check you code!)",
	__DATE__,						
	"3c2bc5c0-b214-11d6-8f89-fcde82579545",						
	NULL, NULL,							
	"Dummy Plugin (check you code!)",				
	0, 1, 0, 0,	0, 0, 0, 0,							
	NULL,						
	&dummy_code,						
	};

//.............................................................................

uPlugin::uPlugin()
{
	options = 0;
	operation = 0;
	dsp_pgm = (kx_dsp_pgm*) &dummy_dsp_pgm;
	view = NULL;
	boxview = 0;
	settings = 0;
	presets_guid = 0;
	
}

uPlugin::~uPlugin()
{
	if (settings) delete settings;
	#ifndef NO_Z
	delete [] pgm_info.names;
	delete [] pgm_info.tooltips;
	#endif
}

int uPlugin::uinit()
{
	settings = new uSettings(this);

	return 0;
}

iKXPluginGUI* uPlugin::create_cp(kDialog* parent, kFile* /* mf */)
{
	uPluginView* pv = (uPluginView*) create_view();

	if (pv == NULL) /* fixme, hmm */
	{
		pv = new uPluginView; 
		pv->plugin = this;
	}
	
	uPluginContainer* pc = new uPluginContainer(parent, pv);
	if (!pc->create()) return 0;

	return (iKXPluginGUI*) pc;
}

#ifndef NO_Z

int uPlugin::init_ex_info()
{
	int pins, ins = 0, outs = 0;
	int i = 0, j = 0;
	int type;
	int regs = dsp_pgm->rsrc[0] / sizeof dsp_register_info;

	for (i = 0; i < regs; i++)
	{
		switch(dsp_pgm->registers[i].type)
		{
		case GPR_INPUT:	 ins++;  break;
		case GPR_OUTPUT: outs++; break;
		}
	}

	pins = ins + outs;

	pgm_info.names = new char[pins][8];
	pgm_info.tooltips = new char[pins][24];

	for (i = 0; i < regs; i++)
	{
		type = dsp_pgm->registers[i].type;
		if ((type == GPR_INPUT) | (type == GPR_OUTPUT))
		{
			strncpy(pgm_info.names[j], dsp_pgm->registers[i].name, 7);
			strncpy(pgm_info.tooltips[j], dsp_pgm->registers[i].name, 23);
			j++;
		}
	}

	pgm_info.ipins = ins;
	pgm_info.opins = outs;

	return 0;
}

int uPlugin::set_io_tooltips(int count, const char* const tips[])
{
	int pins = pgm_info.ipins + pgm_info.opins;
	if (count > pins) count = pins;
	for (int i = 0; i < count; i++) strncpy(pgm_info.tooltips[i], tips[i], 23);
	return 0;
}

iKXDSPWindow* uPlugin::create_dsp_wnd(kDialog *parent_, kWindow *that_, kFile *mf_)
{
    if (!ucore->IsOK())
	{
		::MessageBox(NULL, "Can't load " RESOURCELIB "!\nPlease, reinstall UFX.", "UFX", MB_ICONERROR);
        return uPlugin::iKXPlugin::create_dsp_wnd(parent_, that_, mf_);
	}

	uPluginBoxContainer* pbv = new uPluginBoxContainer(parent_, that_, this, mf_);
	return (iKXDSPWindow*) pbv;
}
#endif

int uPlugin::request_microcode()
{
	 info = dsp_pgm->registers;                              	
	 code = dsp_pgm->code;                              	
	 info_size = dsp_pgm->rsrc[0];                 	
	 code_size = dsp_pgm->rsrc[1];                 	
	 itramsize = dsp_pgm->rsrc[2];                    	
	 xtramsize = dsp_pgm->rsrc[3];                     	
	 strncpy(name, dsp_pgm->strings[0], sizeof(name));
	 return 0;
}

const char* uPlugin::get_plugin_description(int id)
{
	if (settings == 0) 
	{ // fixme, this is hack actually, uinit should be called when pgm_id is already known!
		if (pgm_id >= 0) uinit(); 
	}

	if (id < 8) return dsp_pgm->strings[id];
	return NULL;
}

int uPlugin::set_factory_presets(int npresets, int nparams, const void* data)
{
	char path[MAX_PATH];
	GetModuleFileName(hmodule, path, sizeof path);
    uPresetManager pman;
	pman.SelectPlugin(presets_guid, dsp_pgm->strings[0], CharLower(path));
	pman.WriteFactory(npresets, nparams, (const char*) data);
	return 0;
}

void uPlugin::post_param_changed(int id, kxparam_t value)
{
	if (view)
	{
		view->pParentContainer->param_changed(id, value);
		view->param_changed(id, value);
	}
}

int uPlugin::set_all_params(kxparam_t* values, int /* reserved */)
{
	int params_count = get_param_count();
	operation |= UPOP_SET_ALL_PARAMS;
	for (int i = 0; i < params_count; i++) set_param(i, values[i]);
	operation &= ~UPOP_SET_ALL_PARAMS;
	return 0;
}

int uPlugin::set_all_params(kxparam_t* values)
{
	if (view) 
	{
		uPluginContainer* pc = view->pParentContainer;
		pc->SetPresetCaption("Custom");
		if (pc->flags & UPC_BYPMUTE_LOCKED)
		{
			if (pc->flags & UPC_FLAGS_BYPASS) get_param(pc->bypass_id, &values[pc->bypass_id]);
			if (pc->flags & UPC_FLAGS_MUTE) get_param(pc->mute_id, &values[pc->mute_id]);
		}
	}

	int ret = set_all_params(values, 0);
	
	return ret;
}

int uPlugin::get_all_params(kxparam_t* values, int /* reserved */)
{
	int params_count = get_param_count();
	for (int i = 0; i < params_count; i++) get_param(i, &values[i]);
	return 0;
}

int uPlugin::set_defaults() 
{
	kx_fxparam_descr descr;
	int params_count = get_param_count();
    char* data = new char[sizeof(const char*) + params_count * sizeof(kxparam_t)];
	kxparam_t* default_params = (int*) (data + sizeof(const char*));
	
	uPresetManager pman;
	pman.SelectPlugin(presets_guid, dsp_pgm->strings[0]);

	if (pman.ReadPreset("[Default]", params_count, default_params))
	{ // default preset not found, trying create it automatically
		for (int i = 0; i < params_count; i++)
		{
			int r = describe_param(i, &descr);
			default_params[i] = (r) ? 0 : ((descr.min_value + descr.max_value) >> 1);
		}
		*(const char**) data = "[Default]";
		pman.WriteFactory(1, params_count, (const char*) data);
	}

	if (view) view->pParentContainer->state |= UPC_PRESET_LOCKED; // prevent plugincontainer from changing preset name

	set_all_params(default_params);

	if (view) 
	{
		view->pParentContainer->state ^= UPC_PRESET_LOCKED;
		view->pParentContainer->SetPresetCaption("[Default]");
	}
	
	delete [] data;
	return 0;
}

int uPlugin::event(int event) 
{
		switch (event)
		{
		case IKX_BYPASS_ON:
			ikx->set_microcode_bypass(pgm_id, 0); 
			break;
		}		    
		return 0;
};

//.............................................................................

int kx_get_ndevices()
{
	int i = 0;
	iKX ikx;
	while (ikx.init(i) == 0)
	{
		ikx.close();
		i++;
	}
	return i;
}

//.............................................................................
