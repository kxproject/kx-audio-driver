
//.............................................................................
#ifdef COLLECT_DLL_PLUGINS

&Eqp1_entry,
&Eqp1m_entry,

//.............................................................................
#else // COLLECT_DLL_PLUGINS

//.............................................................................
#ifdef PLUGIN_ENTRY

int Eqp1_entry(int command, uintptr_t* ret);
int Eqp1m_entry(int command, uintptr_t* ret);

//.............................................................................
#else // PLUGIN_ENTRY

#ifndef _EQP1_H_
#define _EQP1_H_

//.............................................................................

enum {
	ID_BYPASS,
	ID_MUTE,
	ID_IGAIN,
	ID_IPHASE,
	
	ID_ON, // reserved - don't use
	ID_TYPE,
	ID_FREQ,
	ID_GAIN,
	ID_BW,

	params_count,

	// extra ids
	ID_PLOT,
	ID_FREQSCALE,
	ID_EXTRA
};

//.............................................................................
// Plugin

class Eqp1 : public uPlugin
{
public:
	Eqp1(int mono = 0);
	~Eqp1();

public:
	int get_param_count();
	int describe_param(int id, kx_fxparam_descr *descr);
	int set_all_params(kxparam_t* values, int reserved);
	int get_param(int id, kxparam_t *value);
	int set_param(int id, kxparam_t value);

	uPluginView* create_view();

	// VOID CALLBACK timer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

	biquad_t biquad;
	int		cache_e;
	int		cache_t;
	int		cache_l;
	float	cache_g;
	float	cache_f;
	float	cache_q;

private:
	int update_iogain(double* ib, int* g);

	double k[6];
	int params[params_count];
	int mono;

	HANDLE htimer;

};

//.............................................................................
// Plugin View

class Eqp1View : public uPluginView
{
public:
	Eqp1View(uPlugin* plugin);
	int open();

	void lock_plot(int lock = 1) {lockplot = lock;}
	int param_changed(int id, kxparam_t value);

protected:
	int ucontrol_changed(int id, int data);
	int init_curve();
	int update_curve(biquad_t* biquad, int e, int band, int fix, float f, float g);

	int lockplot;

	uAKnob knbIGain;
	uAKnob knbGain;
	uAKnob knbFreq;
	uAKnob knbBW;

	uCheckBox chkIPhase;

	uVFader fdrFreqScale;
	uLabel	IGPrint;
	uLabel	GPrint;
	uLabel	FPrint;
	uLabel	QPrint;

	uECombo cmbType;
	uDrawBox dbxPlot;

	CBitmap bmpPlotBack;
	CBitmap bmpPlotFore;

	CBitmap bmp_curve[9];

	float mag[fresp_points];

	POINT curvepoints[fresp_points + 2];
};

//.............................................................................

#endif // _EQP1_H_
#endif // PLUGIN_ENTRY
#endif // COLLECT_DLL_PLUGINS


