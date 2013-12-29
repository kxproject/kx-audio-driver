
//.............................................................................
#ifdef COLLECT_DLL_PLUGINS

&rev2_entry, // :)

//.............................................................................
#else

//.............................................................................
#ifdef PLUGIN_ENTRY

int rev2_entry(int command, uintptr_t* ret);

//.............................................................................
#else // PLUGIN_ENTRY

#ifndef _REV2_H_
#define _REV2_H_

//.............................................................................

enum {
	ID_BYPASS,
	ID_MUTE,
	ID_DRYMUTE,
	ID_DRYMIX,
	ID_WETMUTE,
	ID_WETMIX,
	ID_EARLYMUTE,
	ID_REVMUTE,
	ID_ERBALANCE,
	ID_EARLYTYPE,		// reserved
	ID_EARLYSHAPE,		// reserved
	ID_REVALGORITHM,
	ID_REVSIZE,			// reserved
	ID_AZIMUTH,			
	ID_PREDELAY,
	ID_DECAY,
	ID_DIFFUSE,
	ID_EARLY_HICUT,
	ID_EARLY_HIFREQ,
	ID_EARLY_LOCUT,
	ID_LOFREQ,
	ID_REV_LOCUT,
	ID_REV_HICUT,
	ID_REV_HIFREQ,
	ID_DECAY_HICUT,
	ID_DECAY_HIFREQ,

	params_count
};

typedef struct {
	int delay_times[2][8]; // as read addr
	int tap_levels[2][8];	// index to constpool (!incl sign!)
	int user_consts[4];	// user constants in the pool
	int fdbk_phases;		// as bits: ll, lr, rr, rl
	int dfsn_range;
	int dfsn_bias;
	int dfsn_to_revlev_fact;
	int azm_bias;

	char* name;
} rev_algorithm_t;

//.............................................................................
// Plugin

class Rev2 : public uPlugin
{
public:
	Rev2();

public:
	int get_param_count();
	int describe_param(int id, kx_fxparam_descr *descr);
	int get_param(int id, kxparam_t *value);
	int set_param(int id, kxparam_t value);

	uPluginView* create_view();

	#ifdef _SCI_
		uLabel*  lblShow;
		#define _show if (lblShow) lblShow->printf
	#else
		#define _show
	#endif

private:
	int params[params_count];
	rev_algorithm_t* algorithm;
	int lock;

	int apply_algorithm();
};

//.............................................................................
// Plugin View

#ifdef _SCI_
	#include "rev2sci.h"
#endif _SCI_

class Rev2View : public uPluginView
{
public:
	Rev2View(uPlugin* plugin_);
	virtual ~Rev2View();

	int open();

protected:
	int lock;

	int ucontrol_changed(int id, int data);
	int param_changed(int id, kxparam_t value);

	uCheckBox chkDryMute;
	uHFader   fdrDryMix;
	uCheckBox chkWetMute;
	uHFader   fdrWetMix;
	uCheckBox chkEarlyMute;
	uCheckBox chkRevMute;
	uHFader   fdrEarlyRev;
	uCheckBox chkX10;
	uHFader   fdrPreDelay;
	uHFader   fdrDecay;
	uAKnob    knbDiffuse;
	uAKnob    knbEHiCut;
	uAKnob    knbEHiFreq;
	uAKnob    knbELoCut;
	uAKnob    knbLoFreq;
	uAKnob    knbRLoCut;
	uAKnob    knbRHiCut;
	uAKnob    knbRHiFreq;
	uAKnob    knbDecHiCut;
	uAKnob    knbDecHiFreq;
	uAKnob    knbAzimuth;
	uAKnob	  knbSize;
	uAKnob	  knbShape;

	uLabel	  lblOutputPrint;
	uLabel	  lblEarlyPrint;
	uLabel	  lblRevPrint;

	uECombo	  cmbRevAlgorithm;

	#ifdef _SCI_
		uLabel  lblShow;
		uButton btnAlgo;
		Rev2Sci wndAlgo;
	#endif
};

//.............................................................................

#endif // _REV2_H_
#endif // PLUGIN_ENTRY
#endif // COLLECT_DLL_PLUGINS


