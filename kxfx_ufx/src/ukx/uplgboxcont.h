
#ifndef _U_PLUGINBOXCONT_H_
#define _U_PLUGINBOXCONT_H_

//.............................................................................

#define BOX_MICROCODE_MASK 0xffff
#define BOX_ENABLED (MICROCODE_TRANSLATED | MICROCODE_ENABLED)
#define BOX_COLLAPSED 0x20000003

//.............................................................................
// uPluginBoxView window

class uPluginBoxContainer : public iKXDSPWindow
{
public:
	uPluginBoxContainer(kDialog* parent_, kWindow* that_, uPlugin* uplugin, kFile* mf_);
	~uPluginBoxContainer();

	int draw(kDraw* dc, int x, int y, int flag, const char* name, int pgm_id);
	int find_gpr(const kPoint* pt, int* gpr, int flag, char* name);
	int get_window_rect(char* name, int flag, kSize* sz);
	int get_connection_rect(int io_num, int type, int flag, kRect* pos);
	int set_connector_size(int sz);
	int configure() {return 0;};

    void notify(int id, int state);

protected:
	int render(const char* name, int flag);
	int render_shadow();

	RECT  rect;

	uPlugin* plugin;
    class Arrow* collapse;
    HWND kxarrow;
	int ipins;
	int opins;

	int state;
    int pin_size;

	CBitmap bitmap;
	CBitmap shadow;

	RECT	_prect[16];
	LPRECT pin_rect;

	friend class uPluginBoxView;
};

//.............................................................................

#endif // _U_PLUGINBOXCONT_H_
