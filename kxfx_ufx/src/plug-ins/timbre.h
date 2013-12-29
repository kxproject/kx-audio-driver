
//.............................................................................
#ifdef COLLECT_DLL_PLUGINS

&timbre_entry, // :)

//.............................................................................
#else // COLLECT_DLL_PLUGINS

//.............................................................................
#ifdef PLUGIN_ENTRY

int timbre_entry(int command, uintptr_t* ret);

//.............................................................................
#else // PLUGIN_ENTRY

#ifndef _TIMBRE_H_
#define _TIMBRE_H_

//.............................................................................

enum {
	ID_BASS,
	ID_TREBLE,

	params_count
};

//.............................................................................
// Plugin

class Timbre : public uPlugin
{
public:
	Timbre();

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

class TimbreView : public uPluginView
{
public:
	TimbreView(uPlugin* plugin);
	int open();

protected:
	int ucontrol_changed(int id, int data);
	int param_changed(int id, kxparam_t value);

	uAKnob knbBass;
	uAKnob knbTreble;
};

//.............................................................................

#endif // _TIMBRE_H_
#endif // PLUGIN_ENTRY
#endif // COLLECT_DLL_PLUGINS


