
//.............................................................................
#ifdef COLLECT_DLL_PLUGINS

&gainx_entry, // :)

//.............................................................................
#else // COLLECT_DLL_PLUGINS

//.............................................................................
#ifdef PLUGIN_ENTRY

int gainx_entry(int command, uintptr_t* ret);

//.............................................................................
#else // PLUGIN_ENTRY

#ifndef _GAINX_H_
#define _GAINX_H_

//.............................................................................

enum {
	ID_GAIN,

	params_count
};

//.............................................................................
// Plugin

class GainX : public uPlugin
{
public:
	GainX();

public:
	int get_param_count();
	int describe_param(int id, kx_fxparam_descr *descr);
	int get_param(int id, kxparam_t *value);
	int set_param(int id, kxparam_t value);

	uPluginBoxView* create_boxview();

private:
	int params[params_count];
};

//.............................................................................
// Plugin View

class GainXBoxView : public uPluginBoxView
{
public:
	GainXBoxView(uPlugin* plugin);

	int open();
    int redraw(); // FIXME big, big kludge

protected:
	int ucontrol_changed(int id, int data);
	int param_changed(int id, kxparam_t value);

	uAKnob knbGain;
};

//.............................................................................

#endif // _GAINX_H_
#endif // PLUGIN_ENTRY
#endif // COLLECT_DLL_PLUGINS


