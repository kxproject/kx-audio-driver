
//.............................................................................
#ifdef COLLECT_DLL_PLUGINS

&eqp5_entry, // :)
&eqp5m_entry,

//.............................................................................
#else // COLLECT_DLL_PLUGINS

//.............................................................................
#ifdef PLUGIN_ENTRY

int eqp5_entry(int command, uintptr_t* ret);
int eqp5m_entry(int command, uintptr_t* ret);

//.............................................................................
#else // PLUGIN_ENTRY

#ifndef _EQP5_H_
#define _EQP5_H_

//.............................................................................

#define N_BANDS				5
#define N_PARAM_PER_BAND	5

enum {
	ID_BYPASS,
	ID_MUTE,
	ID_IGAIN,
	ID_IPHASE,
	
	ID_ON,
	ID_TYPE,
	ID_FREQ,
	ID_GAIN,
	ID_BW,

	params_count = ID_BW + (5 * (N_BANDS - 1)) + 1,

	// extra ids
	ID_PLOT,
	ID_FREQSCALE,
	ID_EXTRA = ID_FREQSCALE + N_BANDS
};

//.............................................................................
// Plugin

class Eqp5 : public uPlugin
{
public:
	Eqp5(int mono = 0);
	~Eqp5();

public:
	int get_param_count();
	int describe_param(int id, kx_fxparam_descr *descr);
	int set_all_params(kxparam_t* values, int reserved);
	int get_param(int id, kxparam_t *value);
	int set_param(int id, kxparam_t value);

	uPluginView* create_view();

	// VOID CALLBACK timer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

	biquad_t biquad[N_BANDS];
	int		cache_e[N_BANDS];
	int		cache_t[N_BANDS];
	int		cache_l[N_BANDS];
	float	cache_g[N_BANDS];
	float	cache_f[N_BANDS];
	float	cache_q[N_BANDS];

private:
	int update_iogain(double* ib, int* g);

	double k[N_BANDS][6];
	int params[params_count];
	int mono;

	HANDLE htimer;

};

//.............................................................................
// Plugin View

class Eqp5View : public uPluginView
{
public:
	Eqp5View(uPlugin* plugin);
	int open();

	void lock_plot(int lock = 1) {lockplot = lock;}
	int param_changed(int id, kxparam_t value);

protected:
	int ucontrol_changed(int id, int data);
	int init_curve();
	int update_curve(biquad_t* biquad, int e, int band, int fix, float f, float g);

	int lockplot;

	uAKnob knbIGain;
	uAKnob knbGain[N_BANDS];
	uAKnob knbFreq[N_BANDS];
	uAKnob knbBW[N_BANDS];

	uCheckBox chkIPhase;
	uCheckBox chkOn[N_BANDS];

	uVFader fdrFreqScale[N_BANDS];
	uLabel	IGPrint;
	uLabel	GPrint[N_BANDS];
	uLabel	FPrint[N_BANDS];
	uLabel	QPrint[N_BANDS];

	uECombo cmbType[N_BANDS];
	uDrawBox dbxPlot;

	CBitmap bmpPlotBack;
	CBitmap bmpPlotFore;

	CBitmap bmp_curve[9];

	float mag[N_BANDS][fresp_points];

	POINT curvepoints[fresp_points + 2];
};

//.............................................................................

#endif // _EQP5_H_
#endif // PLUGIN_ENTRY
#endif // COLLECT_DLL_PLUGINS


