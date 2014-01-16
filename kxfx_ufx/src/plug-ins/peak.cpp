
#include "plugins.h"
#include "peak.h"

//.............................................................................
// Plugin Implementation
//.............................................................................

// dsp data
#include "peak_dsp.h" 

DECLARE_DSP_PROGRAM(dsp_pgm_data, peak); 

//.............................................................................

int peak_entry( int command, uintptr_t* ret )
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
		*ret = uintptr_t(new Peak);												
		return 0;																		
	default:																			
		*ret = NULL;																	
		return 1;																		
	}															
}

//.............................................................................

const char* const io_tooltips[] =
{
	"Left In",
	"Right In",
};

//.............................................................................

Peak::Peak()
{
	set_options(UP_HAS_VIEW);
	set_dsp_pgm((kx_dsp_pgm*) &dsp_pgm_data);
	set_io_tooltips(sizeof(io_tooltips) / sizeof(char*), io_tooltips);
}
/*
int Peak::get_param_count()
{
	return params_count;
}

int Peak::describe_param(int id, kx_fxparam_descr *descr)
{
	return -1;
}

int Peak::get_param(int id, kxparam_t *value)
{
	return -1;
}

int Peak::set_param(int id, kxparam_t value)
{
	params[id] = value;

	post_param_changed(id, value); // send parameter change notification to pluginview

	return 0;
}
*/
//.............................................................................

uPluginView* Peak::create_view()
{
	uPluginView* pv = (uPluginView*) new PeakView(this);
	return pv;
}

//.............................................................................
// Plugin View Implementation
//.............................................................................

#define TAIL_IMPULSE	0
#define TAIL_ACCELER	3

PeakView::PeakView(uPlugin* plugin)
{
	/* set plugin this view is created for */
	PeakView::plugin = plugin;

	maxpeak[0] = 0; maxpeak[1] = 0;
	tail[0] = 0; tail[1] = 0;
	tail_speed[0] = TAIL_IMPULSE; tail_speed[1] = TAIL_IMPULSE;

	#ifndef UINEXE
		plugin->write_gpr((word) (0x8000u), 0);
		plugin->write_gpr((word) (0x8001u), 0);
	#endif
}

int PeakView::open()
{
	/* set title string */
	// set_title("Peak");

	set_property(UPVP_STYLE, 0x80);

	/* load custom background bitmap */
	// bkgbitmap.LoadBitmap(IDB_PEAKBACK);

	size.right = 62;
	size.bottom = 311;

	bmpMeterBack.LoadBitmap(IDB_PEAKBACK);
	bmpMeterFore.LoadBitmap(IDB_PEAKFORE);

	peak.Create(WS_VISIBLE, &bmpMeterBack, 3, 3, this, 0);
	peak.SetMeterBitmap(&bmpMeterFore, 8, 16, 15);

	peak.start();
	
	return true;
}

#define _PXL_0DB	258
#define _PXL_6DB	210
#define _PXL_18DB	162

#define LEV_138DB	0x43
#define LEV_96DB	0x213c
#define LEV_18DB	0x4000000
#define LEV_6DB		0x10000000
#define LEV_P3_2DB	0x2E410000 // maximum

int PeakView::ucontrol_changed(int id, int data)
{
	float d;
	int x[2];
	int p = 0;
	dword i;
	char c[8];

	switch(id)
	{
	case 0:
		for (i = 0; i < 2; i++)
		{
			#ifndef UINEXE
				plugin->read_gpr((word) (0x8000u + i), (dword*) &p);
				plugin->write_gpr((word) (0x8000u + i), 0);
			#endif

			if (p < 0x43) x[i] = 0;
			else
			{
				d = 20.f * log10f(p * (1.f / 0x20000000));

				if (maxpeak[i] < p) 
				{
					maxpeak[i] = p;
					sprintf(c, (d > -10.0f) ? "%.1f " : "%.0f ", (double) d);
					peak.SetLabel(i, c, (p > 0x1fffffff) ? 1 : 0);
				}

				if (p < LEV_96DB) x[i] = 4;
				else if (p < LEV_18DB) x[i] = _PXL_18DB + (int) (2.f * (d + 18.f));
				else if (p < LEV_6DB) x[i] = _PXL_6DB + (int) (4.f * (d + 6.f));
				else if (p < LEV_P3_2DB) x[i] = _PXL_0DB + (int) (8.f * d);
				else x[i] = 284;	
				x[i] &= ~3;
			}
			if (x[i] >= ((tail[i] & ~3) - 4)) {tail[i] = x[i]; tail_speed[i] = TAIL_IMPULSE;}
			else
			{
				tail[i] = ((tail[i] << 1) - tail_speed[i]) >> 1;
				tail_speed[i] += TAIL_ACCELER;
			}
		}

		peak.SetValue(x[0], x[1], tail[0] & ~3, tail[1] & ~3);
		break;

	case UCN_SETRESET:
		maxpeak[data] = 0;
		peak.SetLabel(data, "-Inf. ", 0);
		break;

	default:
		return false;
	}

	return true;
}
/*
int PeakView::param_changed(int id, kxparam_t value)
{
	return true;
}
*/
