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

#ifndef _KX_PM_H
#define _KX_PM_H

// -----------------------------------------------------
// kX Plugin Manager API (version 1.0)
// -----------------------------------------------------

// do not include this file directly, use 'kxapi.h' instead

class kSettings;
class iKXManager;
class iKXMidiParser;

// plugin list
// the format of the structure can change; add-ons should not rely on its content 
// found after 'host-specific data', nor should they allocate/copy/compare structures
// 'reserved' field is not used by the kX Manager and can be used by add-ons
typedef struct plugin_list_t_s
{
 struct plugin_list_t_s *next;

 int size_of;	// sizeof(plugin_list_t)

 iKXPlugin *plugin;
 
 // cached values
 char guid[KX_MAX_STRING]; 	// after plugin->get_plugin_description()
 int plugin_type; 		// after get_plugin_type()
 char file_name[MAX_PATH];
 dword reserved;

 // host-specific data [for instance, kX DSP uses it this way: ]
 int x,y;	// kX DSP coordinates
 		// add-ons should not access these fields directly 
 		// and should use find_plugin_ex instead
}plugin_list_t;

class KXMANAGER_CLASS_TYPE iKXPluginManager
{
public:
      // find currently uploaded plugin; load if necessary
      // GUID is optional; pgm_id should be valid (see load_plugin below)
      iKXPlugin *find_plugin(int pgm_id,const char *guid);
      iKXPlugin *find_plugin_ex(int pgm_id,const char *guid,int **x,int **y);

      // find only; don't load:
      iKXPlugin *find_plugin_by_id(int pgm_id,const char *guid=NULL,plugin_list_t **pl=NULL);
      iKXPlugin *find_plugin_by_guid(const char *guid,int *pgm_id=NULL,plugin_list_t **pl=NULL);
        // pgm_id is ignored on input; filled-in on output

      int have_plugin(const char *guid); // returns pgm_id if plugin 'guid' exists and is uploaded
      				   // if fails, returns 0

      iKXPlugin *enum_plugin(int number); // 0..inf -- NOT pgm_id
      int describe_plugin(int number,plugin_list_t *info); // 0..inf -- NOT pgm_id

      int get_plugin_type(int pgm_id,const char *guid);
      int get_plugin_file(int pgm_id,const char *guid,char *fname); // fname is supposed to be at least MAX_PATH

      // returns number of registered plugins; fname can be NULL for GUI-based dialog
      int register_plugin(const char *fname,int gui=1); 

      int unregister_plugin(int ndx); // ndx is MENU_PLUGIN_BASE-based (e.g. returned by create_plugins_menu)
      int load_plugin(int ndx,iKXPlugin **plugin); // --""--
       // return value: 0 - ok, [see load_plugin below]
       // returns plugin, too
      int perform_translate(kDialog *that,iKXPlugin *plugin,int gui_based=1);
       // plugin should be already uploaded
       // will display 'enchanced' dialog box, if gui_based==1 && 'enhanced dsp' is active
       // will display a necessary error message, if gui_based==1
       // return value: 0 ok; -10 - user cancelled; <0 - error [message already displayed, if gui_based==1]

      int load_plugin(const char *guid,int *pgm_id,iKXPlugin **plg,plugin_list_t *pl=NULL);
      // if *pgm_id<=0, load microcode after loading plugin
      // if ==0 - with any pgm_id
      // if <0 - with '-(*pgm_id)' pgm_id (forced);
      // return value: 0 ok; <-10 - critical error [no message displayed]; 0.<ret<.-10 -- error message displayed

      int unload_plugin(iKXPlugin *plg);

      int move_plugin_window(iKXPlugin *plg,int x,int y);
      int move_plugin_window(int pgm_id,int x,int y);
       // move window inside kX DSP window; user will need to call kXDSP::redraw_window()

      int save_all_plugin_settings(kSettings &cfg);
      int load_all_plugin_settings(kSettings &cfg);
      int reset_all_plugin_settings();

      int save_plugin_settings(iKXPlugin *plg,kSettings &cfg);
      int load_plugin_settings(iKXPlugin *plg,kSettings &cfg,int load_connections=0);
       // since not all plugins might be already uploaded, load process is performed in two
       // stages: actual load & inter-connection

      int connect(iKXPlugin *plg1,int reg1,iKXPlugin *plg2,int reg2,int multiple=1);
       // if plg1==NULL, pgm_id1 will be '-1'
       // if plg2==NULL, pgm_id2 will be '-1'
       // NOTE: reg1/reg2 are pointers into 'info[]' array, not (word) info[].num!

      int tweak_plugin(int pgm_id,kDialog *parent=0);
      int tweak_plugin(const char *guid,kDialog *parent=0);

      int init_plugins(void);
      int close_plugins(void);
      int update_plugins(int where=0); // update plugin lists / parameters / refresh kX DSP
      int realign_plugin(plugin_list_t *new_plugin);
      int realign_plugin(int num);

      int clear_dsp(void);

      iKXPluginManager() { plugin_list=NULL; ikx_t=NULL; parser_t=NULL; };
      virtual ~iKXPluginManager() { close_plugins(); };

      iKXManager *get_manager();

      int create_plugins_menu(kMenu *menu); // should already be created()

      virtual int notify(iKXPlugin *plg,int event);
       // a way for iKXPlugin to notify kX Manager / kX Plugin Manager
       // see 'iKXPlugin::notify'
      virtual int notify_ex(iKXPlugin *plg,int event, void *p1);

      int get_version(); // returns KXPLUGINMANAGER_VERSION

      iKX *ikx_t;
      iKXMidiParser *parser_t;

private:
      iKXPlugin *find_plugin(int pgm_id,const char *guid,plugin_list_t **pl);
      int load_plugin(const char *guid,int *pgm_id,iKXPlugin **plg,plugin_list_t **pl);

      plugin_list_t *plugin_list;
};

#define KXPLUGINMANAGER_VERSION		0x1	// should be changed when fundamental modifications occur
  
#endif
