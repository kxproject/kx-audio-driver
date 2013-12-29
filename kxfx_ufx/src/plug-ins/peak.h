
//.............................................................................
#ifdef COLLECT_DLL_PLUGINS

&peak_entry, // :)

//.............................................................................
#else // COLLECT_DLL_PLUGINS

//.............................................................................
#ifdef PLUGIN_ENTRY

int peak_entry(int command, uintptr_t* ret);

//.............................................................................
#else // PLUGIN_ENTRY

#ifndef _PEAK_H_
#define _PEAK_H_

//.............................................................................

enum {
	params_count
};

//.............................................................................
// Plugin

class Peak : public uPlugin
{
public:
	Peak();

public:
	/*int get_param_count();
	int describe_param(int id, kx_fxparam_descr *descr);
	int get_param(int id, kxparam_t *value);
	int set_param(int id, kxparam_t value);*/

	uPluginView* create_view();

private:
	// int params[params_count];
};

//.............................................................................
// Plugin View

class PeakView : public uPluginView
{
public:
	PeakView(uPlugin* plugin);
	int open();

protected:
	int ucontrol_changed(int id, int data);
	// int param_changed(int id, kxparam_t value);

	CBitmap bmpMeterBack;
	CBitmap bmpMeterFore;

	uMeter peak;
	int maxpeak[2];
	int tail[2];
	int tail_speed[2];
};

//.............................................................................

#endif // _PEAK_H_
#endif // PLUGIN_ENTRY
#endif // COLLECT_DLL_PLUGINS


