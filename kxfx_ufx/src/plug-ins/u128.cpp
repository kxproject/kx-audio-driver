
#include "stdafx.h"
#include "../uproto/resource.h"
#include "u128.h"

//.............................................................................
// Plugin Implementation
//.............................................................................

// dsp data
#include "_u128_dsp.cpp" 

DECLARE_DSP_PROGRAM(dsp_pgm_data, u128); 

//.............................................................................

int u128_entry(int command, dword *ret)										
{																						
	switch(command)																		
	{																					
	case KXPLUGIN_GET_NAME:		 // title of plugin to be shown in menus, kx dsp etc.
		lstrcpyn((char*) ret, dsp_pgm_data.strings[0], KX_MAX_STRING);						
  		return 0;																		
	case KXPLUGIN_GET_GUID:		 // guid to identify plugin							
		lstrcpyn((char*) ret, dsp_pgm_data.strings[5], KX_MAX_STRING);						
  		return 0;																		
	case KXPLUGIN_INSTANTIATE:	 // actual request to create plugin					
		*ret = (dword) new U128;												
		return 0;																		
	default:																			
		*ret = NULL;																	
		return 1;																		
	}															
}

//.............................................................................

// factory presets
typedef kxparam_t const preset[params_count + 1];
#define P (kxparam_t)

preset factory_presets[] = 
{
	{P "[Default]", 0, 0, 1},
	{P "NW", -45, 0, 1},
	{P "NE", 45, 0, 1},
	{P "SW", -135, 0, 1},
	{P "SE", 135, 0, 1},
	{P "N", 0, 0, 1},
	{P "S", 180, 0, 1},
	{P "W", -90, 0, 1},
	{P "E", 90, 0, 1},
};

const int presets_count = (sizeof factory_presets / sizeof preset);

// parameter descriptions
const kx_fxparam_descr param_descr[params_count] = 
{
	{"Angle", NULL, -180, 180},
	{"Rotate", NULL, 0, 1},
	{"Rate", NULL, 1, 15},
};

/*const char* const io_tooltips[] =
{
	"Left In",
	"Right In",
	"Left Out",
	"Right Out",
}; */

//.............................................................................

U128::U128()
{
	set_options(UP_HAS_VIEW);
	set_dsp_pgm((kx_dsp_pgm*) &dsp_pgm_data);
	set_factory_presets(presets_count, params_count, factory_presets); 
	// set_io_tooltips(sizeof(io_tooltips) / sizeof(char*), io_tooltips);
	memcpy(params, &factory_presets[0][1], sizeof kxparam_t * params_count);
}

int U128::get_param_count()
{
	return params_count;
}

int U128::describe_param(int id, kx_fxparam_descr *descr)
{
	memcpy(descr, &param_descr[id], sizeof kx_fxparam_descr); 
	return 0;
}

int U128::get_param(int id, kxparam_t *value)
{
	*value = params[id]; 
	return 0;
}

word x[] = {R_N, R_NE, R_E, R_SE, R_S, R_SW, R_W, R_NW};

int U128::set_param(int id, kxparam_t value)
{
	params[id] = value;
	#ifndef UINEXE
	
	int a, i;
	int v;
	double d;
	switch(id)
	{
		case ID_ANGLE:
			v = (value < 0) ? 360 + value : value;
			a = (v / 45) & 7;
			if (a & 1) {v = 45 - (v - (45 * a));}
			else {v = v - (45 * a);}
			d = (1. / 45.) * v;
			write_instr_r(1, x[(a + (a & 1)) & 7]);
			write_instr_r(2, x[a + ((a + 1) & 1)]);
			set_dsp_register(R_ANGLE, double2fixed(d));
			for (i = 2; i < 8; i++) set_dsp_register(x[(a + i) & 7], 0); // reset the rest outputs!
			break;
		case ID_ROTATE:
		case ID_RATE:
			break;
		default:
			return -1;
	}
	#endif // UINEXE

	post_param_changed(id, value); // send parameter change notification to pluginview

	return 0;
}

//.............................................................................

uPluginView* U128::create_view()
{
	uPluginView* pv = (uPluginView*) new U128View(this);
	return pv;
}

//.............................................................................
// Plugin View Implementation
//.............................................................................

BEGIN_MESSAGE_MAP(U128View, uPluginView)
	//{{AFX_MSG_MAP(U128View)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

U128View::U128View(uPlugin* plugin)
{
	/* set plugin this view is created for */
	U128View::plugin = plugin;

	htimer = 0;
}

int U128View::open()
{
	/* set title string */
	// set_title("Tone Control");
	size.right	= 128;
	size.bottom = 104;

	/* load custom background bitmap */
	// bkgbitmap.LoadBitmap(IDB_U128BACK);
	
	/* create controls */
	knbAngle.Create(WS_VISIBLE | UKS_WRAP, 16, 8, this, ID_ANGLE, -180, +180, -180, 360);
	knbAngle.SetPageStepSize(15);
	//knbAngle.SetArrowStepSize(5);
	lblAngle.Create("", WS_VISIBLE | ULS_BOX | ULS_RIGHT, 64, 16, 40, 19, this, params_count + 1);

	knbRate.Create(WS_VISIBLE, 48, 54, this, ID_RATE, 1, 15);
	chkRotate.Create("Rotate", WS_VISIBLE, 30, 62, 16, 16, this, ID_ROTATE);

	return true;
}

void U128View::close()
{
	param_changed(ID_ROTATE, 0); // disable timer
}

int U128View::ucontrol_changed(int id, int /* data */)
{
	kxparam_t value;

	switch(id)
	{
	case ID_ANGLE:
		value = knbAngle.GetPos();
		break;
	case ID_ROTATE:
		value = chkRotate.GetCheck();
		break;
	case ID_RATE:	
		value = knbRate.GetPos();
		break;
	default:
		return true;
	}

	plugin->set_param(id, value);

	return true;
}

int U128View::param_changed(int id, kxparam_t value)
{
	switch(id)
	{
	case ID_ANGLE:
		knbAngle.SetPos(value);
		lblAngle.printf("%i°", value);
		break;
	case ID_ROTATE:
		chkRotate.SetCheck(value);
		if (value) 
		{
			htimer = ::SetTimer(m_hWnd, 1, 75, NULL);
		}
		else 
		{
			if (htimer) 
			{
				::KillTimer(m_hWnd, htimer); 
				htimer = 0;
			}
		}
		break;
	case ID_RATE:
		knbRate.SetPos(value);
		break;
	default:
		return -1;
	}
	return true;
}

void U128View::OnTimer(UINT /* nIDEvent */)
{
	//trace(".\n");
	kxparam_t v = 0;
	plugin->get_param(ID_ANGLE, &v);
	v += knbRate.GetPos(); 
	if (v > 180) v -= 360;
	plugin->set_param(ID_ANGLE, v);
}

