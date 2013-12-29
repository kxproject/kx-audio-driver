
//.............................................................................
#ifdef COLLECT_DLL_PLUGINS

&eqg10_entry, // :)

//.............................................................................
#else // COLLECT_DLL_PLUGINS

//.............................................................................
#ifdef PLUGIN_ENTRY

int eqg10_entry(int command, uintptr_t* ret);

//.............................................................................
#else // PLUGIN_ENTRY

#ifndef _EQG10_H_
#define _EQG10_H_

#define BANDS_COUNT	10

//.............................................................................

enum {
	ID_BYPASS,
	ID_MUTE,
	ID_BAND,
	ID_GAIN = ID_BAND + BANDS_COUNT,
	
	params_count
};

//.............................................................................
// Plugin

class EQG10 : public uPlugin
{
public:
	EQG10();

public:
	int get_param_count();
	int describe_param(int id, kx_fxparam_descr *descr);
	int get_param(int id, kxparam_t *value);
	int set_param(int id, kxparam_t value);

	uPluginView* create_view();

private:
	int params[params_count];
};

//.............................................................................
// Plugin View

class EQG10View : public uPluginView
{
public:
	EQG10View(uPlugin* plugin_);

	int open();

protected:
	int ucontrol_changed(int id, int data);
	int param_changed(int id, kxparam_t value);

	uVFader fdrBand[BANDS_COUNT];
	uLabel  lblBand;
	uLabel  lblBandGain;
	uVFader fdrGain;
	uLabel  lblGain;
};

//.............................................................................

#endif // _EQG10_H_
#endif // PLUGIN_ENTRY
#endif // COLLECT_DLL_PLUGINS


