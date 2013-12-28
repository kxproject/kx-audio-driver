// kX Mixer GUI
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

// these functions are a temporary hack for creating Add-Ons with kxmixer-like windows...
extern iKXManager *manager;
extern KXMANAGER_CLASS_TYPE void translate_keyboard(UINT ch,UINT rep,UINT fl);
extern KXMANAGER_CLASS_TYPE int launch_menu();
extern KXMANAGER_CLASS_TYPE int process_menu(int ret);

class KXMANAGER_CLASS_TYPE CKXDialog : public kDialog
{
public:
	kDialog **that;
	iKX *ikx_t;
	iKXPluginManager *pm_t;
	iKXMidiParser *parser_t;

	CKXDialog():kDialog() { that=0; prefix=0; ikx_t=0; pm_t=0; parser_t=0; };

	void set_that(kDialog **t) { that=t; };

	void init(char *str);

	void generic_init(char *str,char *bk_section_name="background");
	void device_init(char *str);

	virtual int get_flag() { return 0; };
	virtual int select_device(int dev_=-1)
    { 
	      ikx_t=manager->get_ikx(dev_);
	      pm_t=manager->get_pm(dev_); 
          parser_t=manager->get_parser(dev_);
	      return 0; 
	};

    // common controls:
    kTextButton card_name;
    kButton b_quit,b_info,b_mini;
    kButton b_save_settings,b_load_settings;
    kStatic label;

    // save/restore window pos
	char *prefix;
	void save_position(char *prefix_=0);
	void rest_position(char *prefix_=0);

    void on_char(int ch,int rep,int fl) { translate_keyboard(ch,rep,fl); };
    void on_mouse_r_up(kPoint,int) { process_menu(launch_menu()); };

    void on_sys_command(int nID, int lParam);
    virtual int on_command(int wParam, int lParam);

 	void on_cancel() { PostMessage(WM_DESTROY); };
    void on_ok() { PostMessage(WM_DESTROY); };
    void on_post_nc_destroy() { if(that && (*that)) *that=0; delete this; };
    void on_destroy() { if(prefix) save_position(prefix); };
};
