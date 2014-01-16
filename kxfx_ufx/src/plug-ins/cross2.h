
//.............................................................................
#ifdef COLLECT_DLL_PLUGINS

&cross2_entry, // :)

//.............................................................................
#else // COLLECT_DLL_PLUGINS

//.............................................................................
#ifdef PLUGIN_ENTRY

int cross2_entry(int command, uintptr_t *ret);

//.............................................................................
#else // PLUGIN_ENTRY

#ifndef _CROSS2_H_
#define _CROSS2_H_

//.............................................................................

enum {
	ID_FREQ,
		/*ID_DC1,
		ID_DC2,*/

	params_count
};

//.............................................................................
// Plugin

class Cross2 : public uPlugin
{
public:
	Cross2();

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

class Cross2View : public uPluginView
{
public:
	Cross2View(uPlugin* plugin);
	int open();

protected:
	int ucontrol_changed(int id, int data);
	int param_changed(int id, kxparam_t value);

	uHFader fdrFreq;
	uLabel	lblFreq[2];
	/*uHFader fdrDC1;
	uLabel	lblDC1;
	uHFader fdrDC2;
	uLabel	lblDC2;*/
};

//.............................................................................

#endif // _CROSS2_H_
#endif // PLUGIN_ENTRY
#endif // COLLECT_DLL_PLUGINS


