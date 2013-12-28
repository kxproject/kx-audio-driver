// kX SDK:
// kX API, kX Audio Driver Interface, kX Plugin Manager API
// Copyright (c) Eugene Gavrilov, 2001-2005.
// All rights reserved

/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#ifndef _KX_PLUGIN_H_
#define _KX_PLUGIN_H_

// ------------------------------------------------
// kX Plugin Interface
// iKXPlugin, iKXPluginGUI and iKXDSPWindow classes
// ------------------------------------------------
// do not include this file directly, use 'kxapi.h' instead

class iKXDSPWindow;
class iKXPlugin;
class iKXPluginGUI;
class iKXPluginManager;

class kSettings;

// plugin parameter
typedef intptr_t kxparam_t;

typedef struct kx_fxparam_descr_t
{
 char name[KX_MAX_STRING];      // note: DSP's dsp_register_info uses [MAX_GPR_NAME] instead
 int type;
 #define KX_FXPARAM_LEVEL	0	// -1..1 or 0..7fffffff
 #define KX_FXPARAM_USER	-1
 #define KX_FXPARAM_PERCENT	1	// 0   .. 100%
 #define KX_FXPARAM_ANGLE	2	// 0.0 .. 360.0
 #define KX_FXPARAM_DB		3	// any dB value
 #define KX_FXPARAM_MS		4	// any ms value
 #define KX_FXPARAM_S		5	// any s value (seconds)
 #define KX_FXPARAM_STRING	6	// this will be implemented later (through main plugin)
 #define KX_FXPARAM_HZ		7	// Hz
 #define KX_FXPARAM_KHZ		8	// kHz
 #define KX_FXPARAM_CB		9	// any cB value
 #define KX_FXPARAM_SWITCH	10	// 0 or 1
 #define KX_FXPARAM_RATIO	11	// 1..100
 #define KX_FXPARAM_SEMITONE	12	// e.g. -12.5 .. 12.5
 #define KX_FXPARAM_CENTS	13	// e.g. -1250 .. 1250
 #define KX_FXPARAM_SELECTION	14	// 0...(max_selection)
 #define KX_FXPARAM_RAW		15	// unparsed MIDI data

 kxparam_t min_value,max_value;	// 'min' and 'max' already defined in stdlib(?)
 // to add here
}kx_fxparam_descr;

// generic plugin GUI class
// (abstract class)
class KX_CLASS_TYPE iKXPluginGUI
{
public:
#ifdef _MSC_VER	
#pragma warning(disable:4100)
#endif

 // the main operations:
 virtual void init_gui() = 0;	// usually, 'nop'
 virtual void show_gui() = 0;	// ShowWindow(SW_SHOW);
 virtual void hide_gui() = 0;	// ShowWindow(SW_HIDE);
 virtual void close_gui() = 0; 	// DestroyWindow();
 						// note: close_gui() is supposed to 'delete this;'
                                                // for example, in on_post_nc_destroy() function
                                                // (not earlier!)
 virtual uintptr_t extended_gui(int command,uintptr_t p1=0,uintptr_t p2=0) = 0;

 // main commands:
 #define PLUGINGUI_SET_ICON	1	// SetIcon(p1,p2);
 #define PLUGINGUI_CENTER	2	// CenterWindow();
 #define PLUGINGUI_FOREGROUND	3	// SetForegroundWindow();
 #define PLUGINGUI_GET_CLASS	4	// returns ..._CKXPLUGIN for 'CKXPluginGUI'-based classes
 					// should return ..._KXU for 'kXU'-based classes
 #define PLUGINGUI_EVENT	5	// a plugin->event() signalled; p1 = parameter [IKX_ENABLE,...]
 // PLUGINGUI_RESTORE_POSITION (6) is no longer supported
 #define PLUGINGUI_SET_POSITION	7	// set position of the tweak window
 					// p1=x; p2=y
                                        // UPDATE: (3538 and later):
                                        // the plugin should not store the values, but
                                        // should just 'move' the window to the specified position
 #define PLUGINGUI_GET_POSITION	8 	// get position; p1=&x (int), p2=&y (int)
 #define PLUGINGUI_EVENT_EX	9	// a plugin->event_ex() signalled; p1 = parameter [event]; p2 = event_ex' p1
 #define PLUGINGUI_GET_HWND	10	// should return (int)HWND, if applicable

 #define PLUGINGUI_CLASS_CKXPLUGIN	1
 #define PLUGINGUI_CLASS_KXU		2

 iKXPluginGUI() {};
 virtual ~iKXPluginGUI() {};

#ifdef _MSC_VER	
#pragma warning(default:4100)
#endif
};

// generic plugin class
// (abstract class)

class KX_CLASS_TYPE iKXPlugin
{
public:
 // --- HOST PROVIDED PART --- should be inherited w/o modifications
 // it is advised that you don't access ikx directly (do not use *ikx);
 // pm interface is under development [see kxpm.h]

 // filled-in by Host (before init())
 iKX *ikx;
 iKXPluginManager *pm;	// use iKXPlugin::notify, not iKXPluginManager:: functions
 int kxdevice;
	
#if defined(WIN32)	
 HINSTANCE instance; // DLL instance; freed automatically by iKX::delete_object
#endif
	
 virtual int get_version(); // provided by kX Manager; returns KXPLUGIN_VERSION

 // filled by Host after request_microcode() - only if microcode is loaded
 int pgm_id;

 // callback functions (provided by host)
 int set_dsp_register(word id,dword val);
 int set_dsp_register(const char *id,dword val);
 int get_dsp_register(word id,dword *val);
 int get_dsp_register(const char *id,dword *val);
 int set_tram_addr(word id,dword addr); // relative to itram_start (i.e. logical)
 int get_tram_addr(word id,dword *addr);
 int set_tram_flag(word id,dword flag);
 int get_tram_flag(word id,dword *flag);
 // flags: TRAM_READ TRAM_WRITE (defined elsewhere)

 int write_instruction(int offset,word op,word z,word w,word x,word y,dword valid=0xffffffff);
 int read_instruction(int offset,word *op,word *z,word *w,word *x,word *y);
 #define write_instr_opcode(off,op) write_instruction(off,op,0,0,0,0,VALID_OP)
 #define write_instr_r(off,r) write_instruction(off,0,r,0,0,0,VALID_R)
 #define write_instr_a(off,a) write_instruction(off,0,0,a,0,0,VALID_A)
 #define write_instr_x(off,x) write_instruction(off,0,0,0,x,0,VALID_X)
 #define write_instr_y(off,y) write_instruction(off,0,0,0,0,y,VALID_Y)

 // --- PLUGIN PROVIDED PART ---
 // in general, you should not overload standard constructor/destructor
 iKXPlugin();
 virtual ~iKXPlugin();

 // plugin-provided: should be filled by request_microcode() call
 int itramsize,xtramsize;
 int code_size,info_size;
 const dsp_register_info *info;
 const dsp_code *code;
 char name[KX_MAX_STRING]; // this is actual plugin name (e.g. it can differ from 'original name'
 // plugin is responsible for *code & *info allocation and management (malloc/free/LocalAlloc/LocalFree)

 virtual int init(); // general initialization of plugin-specific data; microcode not loaded yet
 virtual int close(); // microcode is already unloaded
 virtual int request_microcode(); // generally is called after init()
 virtual const char *get_plugin_description(int id); // result values should be static or allocated by plugin
 #define IKX_PLUGIN_NAME	0		// this is original plugin name
 #define IKX_PLUGIN_COPYRIGHT	1
 #define IKX_PLUGIN_ENGINE	2
 #define IKX_PLUGIN_CREATED	3
 #define IKX_PLUGIN_COMMENT	4
 #define IKX_PLUGIN_GUID	5

 virtual const kxparam_t *get_plugin_presets(); // see examples for details

 // preset management [non-virtual functions!]
 //   menu generators
 int populate_presets(kMenu *mnu);	// 0 - success; -1 - failed; 1 - only 'built-in' presets
#if defined(WIN32)	
 int populate_presets(HMENU mnu); // winapi-only version
#endif
	
 #define IKXPLUGIN_PRESETS_BUILTIN	300	// menu index
 #define IKXPLUGIN_PRESETS_CUSTOM	100	// menu index
 #define IKXPLUGIN_MAX_PRESETS		200
 virtual int apply_preset(int ndx); // from the above created menu
 //   save/restore
 virtual int save_preset(const char *preset_name);
 virtual int delete_preset(const char *name);
 virtual int get_current_preset(char *name); // returns non zero if fails
 virtual int export_presets(const char *file_name);
 virtual int import_presets(const char *file_name);
 // accessing plugin presets:
 //  kX-managed presets: (get_all_params()):
 //     stored in section\preset_name binary value in a single massive
 //     e.g. cfg.write_bin_abs(section,preset_name,massive,size);
 //  additional preset data:
 //     e.g. cfg.write_bin_abs(strcat(section+preset_name),parameter_name,value,size)
 //  note: delete_preset() should remove corresponding values and subkeys, if any
 //  section: should be: "Plugins\\%s",get_plugin_description(IKX_PLUGIN_GUID));

 // [non-virtual; host-provided]
 int get_nth_register(int type,int n); // returns a pointer to info[]
 // type: GPR_MASK [GPR_INPUT, ...]
 // n is 0..#

 virtual int event(int event); // signalled only if managed by the kX mixer
 virtual int event_ex(int event, void *p1);
 #define IKX_ENABLE	1
 #define IKX_DISABLE	2
 #define IKX_TRANSLATE	3
 #define IKX_UNTRANSLATE 4
 #define IKX_CONNECT	5
 #define IKX_DISCONNECT 6
 #define IKX_UNLOAD	7
 #define IKX_LOAD	8
 #define IKX_BYPASS_ON	9
 #define IKX_BYPASS_OFF 10
 #define IKX_SAVE_SETTINGS	11
 #define IKX_LOAD_SETTINGS	12
 // note: load, enable, disable, translate, connect/disconnect, 
 //       bypass on/off are signalled -after- the event
 // untranslate,unload are signalled -before- the event
 // the default 'event' calls cp->extended_gui(PLUGINGUI_EVENT,event,0);

 int notify_ex(int event, void *p1);
 int notify(int event); // host-provided function; plugin should notify the host
                        // this function actually calls pm->notify(), which is virtual
                        // [this ensures plugins are binary-compatible with any new versions
                        //  of iKXPluginManager / iKXManager]
 			// events supported:
 #define IKX_CLOSE_CP	1	// plugin tweak window was closed by user
 				// (should be called =before= the window is actually destroyed!)
 #define IKX_DSP_CHANGED	2
 #define IKX_CLOSE_AND_REOPEN	3	// special function to close and re-open the plugin window

 virtual int set_defaults(); // resets GPRs to defaults; should call set_dsp_register(...) or set_parameter(...)

 // should be implemented for any plugin; default handlers work with 'GPR_CONTROL' registers
 // treating them as 'LEVEL's
 virtual int set_param(int ndx,kxparam_t value); // 0...count-1
 virtual int get_param(int ndx,kxparam_t *value);
 virtual int set_all_params(kxparam_t *values); // at least [count]
 virtual int get_all_params(kxparam_t *values); // at least [count]
 virtual int describe_param(int ndx,kx_fxparam_descr *descr); 
   // this function is used for generating 'automatic' tweaking GUIs and for kX Automation

 virtual int get_param_count(void);

 // user interface:
 virtual int get_user_interface();
 #define IKXPLUGIN_NONE		0		// no 'tweak' option
 #define IKXPLUGIN_OWNER	1		// plugin handles all CWnd-related stuff
 #define IKXPLUGIN_SIMPLE	2		// kX mixer draws Tweak Window

 // plugin-controlled kDialog window
 virtual iKXPluginGUI *create_cp(kDialog *parent,kFile *mf_=NULL);
 // create_cp _should not_ modify this->cp;

 // kX DSP window
 virtual iKXDSPWindow *create_dsp_wnd(kDialog *parent_,kWindow *that_,kFile *mf_=NULL);
 // create_dsp_wnd _should not_ modify this->dsp_wnd

 iKXPluginGUI *cp;
 iKXDSPWindow *dsp_wnd;

 // 3538 additions

 // save/restore plugin settings
 // note: all 'params' are already saved/restored
 //  use these functions for storing/restoring 'specific' parameters only, such as
 //  string values etc.
 //  save is called -after- and load is called -before- set/get_param(...)
 virtual int save_plugin_settings(kSettings &);
 virtual int load_plugin_settings(kSettings &);
};
                        
// the class is subject to change...
class KX_CLASS_TYPE iKXDSPWindow
{
public:
 iKXDSPWindow(kDialog *parent_,kWindow *that_,iKXPlugin *plugin_,kFile *mf_)
 { parent=parent_; that=that_; plugin=plugin_; mf=mf_; n_ins=0; n_outs=0; };

 virtual ~iKXDSPWindow() {};

 iKXPlugin *plugin;

 kDialog *parent;
 kWindow *that;
 kFile *mf;

 // number of ins and outs
 // these values are accessed outside the class by the mixer
 // make sure your code doesn't modify or cash them
 int n_ins,n_outs;

 virtual int draw(kDraw *dc,int x,int y,int flag,const char *name,int pgm_id)=0;
 // flag: dsp_microcode.flag
 virtual int find_gpr(const kPoint *pt,int *gpr,int flag,char *name=NULL)=0;
 virtual int get_window_rect(char *name,int flag,kSize *sz)=0;
 virtual int get_connection_rect(int io_num,int type,int flag,kRect *pos)=0;
 // type: 0 - inputs; 1 - outputs
 // flag: dsp_microcode.flag [don't cache it]

 virtual int configure()=0; // should load skin-related settings from kFile
 virtual int set_connector_size(int sz)=0; // -1: default; 0: large 1: huge
};

// kX Plugin publisher (should be exported by plugin DLL)
typedef int (KX_DEF_CALLBACK *kxplugin_publish_t)(int,uintptr_t,uintptr_t *);
#define declare_publish_plugins extern "C" int KX_DEF_EXPORT KX_DEF_CALLBACK publish_plugins(int command,uintptr_t param,uintptr_t *ret)

// defined commands; param is 0 or plugin id (0..get_count()-1)
#define KXPLUGIN_GET_COUNT	0
#define KXPLUGIN_INSTANTIATE	1
#define KXPLUGIN_GET_NAME	2
#define KXPLUGIN_GET_GUID	3
#define KXPLUGIN_GET_VERSION	4
#define KXPLUGIN_GET_FXLIB_VER	5	// for kxfxlib.dll only
#define KXPLUGIN_INSTANTIATE_BY_GUID	6

#define KXPLUGIN_VERSION	0x20	// should be changed when fundamental modifications occur
					// param is current host pluginAPI version

#endif
