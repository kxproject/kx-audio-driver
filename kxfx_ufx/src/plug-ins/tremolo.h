
//.............................................................................
#ifdef COLLECT_DLL_PLUGINS

&tremolo_entry, // :)

//.............................................................................
#else // COLLECT_DLL_PLUGINS

//.............................................................................
#ifdef PLUGIN_ENTRY

int tremolo_entry(int command, dword *ret);

//.............................................................................
#else // PLUGIN_ENTRY

#ifndef _TREMOLO_H_
#define _TREMOLO_H_

//.............................................................................

enum {
	ID_BYPASS,
	ID_MUTE,
	ID_GAIN,
	ID_MODDEPTH,
	
	ID_LFO_RATE,
	ID_LFO_RATE_SUB,
	ID_LFO_WAVEFORM,
	ID_LFO_SHAPE,
	ID_LFO_PHASE,
	
	ID_ENV_MODTYPE,
	ID_ENV_TRASHHOLD,
	ID_ENV_TRIGTIME,
	
	// DEBUG
	
	ID_LFO_RAMP,
	ID_ENV_ATTACK,

	params_count
};

//.............................................................................
// Plugin

class Tremolo : public uPlugin
{
public:
	Tremolo();

public:
	int get_param_count();
	int describe_param(int id, kx_fxparam_descr *descr);
	int get_param(int id, kxparam_t *value);
	int set_param(int id, kxparam_t value);

	uPluginView* create_view();

private:
	kxparam_t params[params_count];
};

//.............................................................................
// Plugin View

class TremoloView : public uPluginView
{
public:
	TremoloView(uPlugin* plugin);
	int open();

protected:
	int ucontrol_changed(int id, int data);
	int param_changed(int id, kxparam_t value);

	uAKnob	knbGain;
	uLabel 	lblGain;
	uAKnob	knbModDepth;
	uLabel 	lblModDepth;
	uAKnob	knbPhase;
	uLabel 	lblPhase;
	
	uAKnob	knbLfoRate;
	uLabel 	lblLfoRate;
	uAKnob	knbLfoRateSub;
	uLabel 	lblLfoRateSub;
	uHFader fdrLfoWaveform;
	uLabel 	lblLfoWaveform;
	uAKnob	knbLfoShape;
	uLabel 	lblLfoShape;
	uAKnob	knbLfoPhase;
	uLabel 	lblLfoPhase;
	
	uCheckBox chkEnvModOn;
	uHFader fdrEnvModType;
	uLabel 	lblEnvModType;
	uAKnob	knbEnvTrashhold;
	uLabel 	lblEnvTrashhold;
	uAKnob	knbTrigTime;
	uLabel 	lblTrigTime;
	
	// Debug
	uAKnob	knbLfoRamp;
	uLabel 	lblLfoRamp;
	
	uAKnob	knbEnvAttack;
	uLabel 	lblEnvAttack;
	
};

//.............................................................................

#endif // _TREMOLO_H_
#endif // PLUGIN_ENTRY
#endif // COLLECT_DLL_PLUGINS


