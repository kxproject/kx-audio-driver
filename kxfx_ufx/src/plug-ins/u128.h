
//.............................................................................
#ifdef COLLECT_DLL_PLUGINS

&u128_entry, // :)

//.............................................................................
#else // COLLECT_DLL_PLUGINS

//.............................................................................
#ifdef PLUGIN_ENTRY

int u128_entry(int command, dword *ret);

//.............................................................................
#else // PLUGIN_ENTRY

#ifndef _U128_H_
#define _U128_H_

//.............................................................................

enum {
	ID_ANGLE,
	ID_ROTATE,
	ID_RATE,

	params_count
};

//.............................................................................
// Plugin

class U128 : public uPlugin
{
public:
	U128();

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

class U128View : public uPluginView
{
public:
	U128View(uPlugin* plugin);
	int open();
	void close();

protected:
	int ucontrol_changed(int id, int data);
	int param_changed(int id, kxparam_t value);

	uAKnob knbAngle;
	uLabel lblAngle;

	uCheckBox chkRotate;
	uAKnob knbRate;

	UINT_PTR htimer;

	//{{AFX_MSG(U128View)
		afx_msg void OnTimer(UINT);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//.............................................................................

#endif // _U128_H_
#endif // PLUGIN_ENTRY
#endif // COLLECT_DLL_PLUGINS


