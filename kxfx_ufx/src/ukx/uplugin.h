
#ifndef _U_PLUGIN_H_
#define _U_PLUGIN_H_

//.............................................................................

typedef struct {
	const char* strings[8];
	int	  rsrc[8];	
	const dsp_register_info* registers;
	const dsp_code* code;
} kx_dsp_pgm;

typedef struct {
	int ipins;
	int opins;
	char (*names)[8];
	char (*tooltips)[24];
} pgm_ex_info;


#define DECLARE_DSP_PROGRAM(data, name)	\
	const kx_dsp_pgm data = {			\
	name##_name,						\
	name##_copyright,					\
	name##_engine,						\
	name##_created,						\
	name##_comment,						\
	name##_guid,						\
	NULL, NULL,							\
	sizeof(name##_info),				\
	sizeof(name##_code),				\
	name##_itramsize,					\
	name##_xtramsize,					\
	0, 0, 0, 0,							\
	name##_info,						\
	name##_code,						\
	};
// end of DECLARE_DSP_PROGRAM

//.............................................................................

#define UP_HAS_VIEW			1
#define UP_HAS_BOXVIEW		2

//.............................................................................

class uPlugin : public iKXPlugin
{
public:
	uPlugin();
	virtual ~uPlugin();

public:
	virtual const char* get_plugin_description(int id);
	virtual int get_param_count(void) {return 0;}
	virtual int describe_param(int, kx_fxparam_descr*) {return -1;}
    virtual int set_param(int /* id */, kxparam_t /* value */) {return 0;}
    virtual int get_param(int /* id */, kxparam_t* value) {*value = 0; return 0;}
	virtual int set_all_params(kxparam_t* values, int reserved);
	virtual int get_all_params(kxparam_t* values, int reserved);
    
public:
	virtual uPluginView* create_view() {return NULL;};
	#ifndef NO_Z
	virtual uPluginBoxView* create_boxview() {return NULL;};
	#endif

protected:
	
	int set_dsp_pgm(kx_dsp_pgm* dsp_pgm) 
	{
		uPlugin::dsp_pgm = dsp_pgm; 

		if (presets_guid == 0) presets_guid = dsp_pgm->strings[5];

		#ifndef NO_Z
		init_ex_info();
		#endif

		return 0;
	};

	void set_options(int options) {uPlugin::options = options;};
	int set_factory_presets(int npresets, int nparams, const void* data);
	int set_io_tooltips(int count, const char* const tips[]);

	void post_param_changed(int id, kxparam_t value);

public:
	// fix/cleanup iKXPlugin functions
	int write_gpr(word id, dword val) {return set_dsp_register(id, val);}
	int write_gpr(char* id, dword val) {return set_dsp_register(id, val);}
	int read_gpr(word id, dword* val) {return get_dsp_register(id, val);}
	int read_gpr(char* id, dword* val) {return get_dsp_register(id, val);}
	int write_instr(int offset, word op, word r, word a, word x, word y, dword valid = ~0)
		{return write_instruction(offset, op, r, a, x, y, valid);}

	int write_instr(int offset, dsp_code* code, int count)
	{
		for (int i = 0; i < count; i++)
		{
			write_instruction(offset + i, code[i].op, code[i].a, code[i].r, code[i].x, code[i].y, ~0u);
		}
		return 1; 
	}
	#undef write_instr_op
	#undef write_instr_r
	#undef write_instr_a
	#undef write_instr_x
	#undef write_instr_y
	int write_instr_op(int offset, word op) {return write_instruction(offset, op, 0, 0, 0, 0, VALID_OP);}
	int write_instr_a(int offset, word a) {return write_instruction(offset, 0, 0, a, 0, 0, VALID_A);}
	int write_instr_r(int offset, word r) {return write_instruction(offset, 0, r, 0, 0, 0, VALID_R);}
	int write_instr_x(int offset, word x) {return write_instruction(offset, 0, 0, 0, x, 0, VALID_X);}
	int write_instr_y(int offset, word y) {return write_instruction(offset, 0, 0, 0, 0, y, VALID_Y);}
	int write_instr_xy(int offset, word x, word y) {return write_instruction(offset, 0, 0, 0, x, y, VALID_X | VALID_Y);}
	int write_instr_axy(int offset, word a, word x, word y) {return write_instruction(offset, 0, 0, a, x, y, VALID_A | VALID_X | VALID_Y);}
	int read_instr(int offset, word* op, word* r, word* a, word* x, word* y)
		{return read_instruction(offset, op, r, a, x, y);}
	int read_instr_op(int offset, word *op) {return read_instruction(offset, op, 0, 0, 0, 0);}

protected:
	friend class uPluginContainer;
	friend class uPluginBoxContainer;
	friend class uPluginBoxView;

protected:
	kx_dsp_pgm* dsp_pgm;
	uPluginView* view;

	int operation;
	#define UPOP_SET_ALL_PARAMS	0x2
	int options;
	uSettings* settings;
	const char* presets_guid;

	int init()  {return 0;};
	int close() {return 0;};
	int event(int event);
	int uinit();

	int request_microcode();
	int get_user_interface() {return options & IKXPLUGIN_OWNER;}
	const kxparam_t* get_plugin_presets() {return 0;}
	int set_all_params(kxparam_t* values);
	int get_all_params(kxparam_t* values) {return get_all_params(values, 0);}

	iKXPluginGUI* create_cp(kDialog* parent, kFile* mf);
	int set_defaults();

	#ifndef NO_Z
	uPluginBoxView* boxview;
	pgm_ex_info	pgm_info;
	int init_ex_info();
	iKXDSPWindow* create_dsp_wnd(kDialog* parent_, kWindow* that_, kFile* mf_= NULL);
	#endif
};

//.............................................................................

#define MACSN	MACS1
#define MACWN	MACW1
#define LIMITL	LIMIT1

//.............................................................................

int kx_get_ndevices();

//.............................................................................

#endif // _U_PLUGIN_H_
