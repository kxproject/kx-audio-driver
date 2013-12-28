// kX SDK:
// kX Audio Driver / kX Audio Driver Interface / kX Audio Driver Effects Library / kX Audio Driver Plugin Library
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2001-2005.
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

#ifndef _KX_KXPLUGIN_GUI_H
#define _KX_KXPLUGIN_GUI_H

// ---------------------------------------------------------
// sample plugin GUI class based on iKXPluginGUI and kDialog
// sample plugin kX DSP GUI class based on iKXDSPWindow
// ---------------------------------------------------------

// this file is usually included in 'stdafx.h' found in the
// kxfxlib project


#ifndef _KX_GUI_H
 #error please, include kGui.h
#endif

typedef struct 
{
    kSlider     slider;
    CEdit       svalue; // will be changed to kEdit
    kStatic     slabel;
}kFader;

#define kCONTROL_TOTAL      0x1000

#define kCONTROL_BASE       0x1000
#define kHIDDEN_CONTROL_BASE    (kCONTROL_BASE+kCONTROL_TOTAL)
#define kCOMBO_CONTROL_BASE (kHIDDEN_CONTROL_BASE+kCONTROL_TOTAL)

#define kRESET_ID       (kCOMBO_CONTROL_BASE+kCONTROL_TOTAL)
#define kPRESET_ID      (kRESET_ID+1)
#define kMINI_ID        (kPRESET_ID+1)
#define kCLOSE_ID       (kMINI_ID+1)
#define kMUTE_ID        (kCLOSE_ID+1)
#define kBYPASS_ID      (kMUTE_ID+1)

#define EVERYPM_ID      -1      // used to update all controls

class KX_CLASS_TYPE CKXPluginGUI : public kDialog, public iKXPluginGUI
{
public:
    // do NOT override these functions:
        virtual void init_gui();
        virtual void show_gui();
        virtual void hide_gui();
        virtual void close_gui(); 
        virtual uintptr_t extended_gui(int command,uintptr_t p1=0,uintptr_t p2=0);

    // default items
    kWindow grpbox;
    kButton b_reset;
    kCheckButton b_mute,b_bypass;
    kButton b_close, b_minimize;
    kStatic w_label;

    int align_x,align_y;

    #define MAX_PRESET_NAME 40

    kTextButton preset;
    kString preset_name;

    kFile mf;
        // default skin management:
        // ------------------------
        // CKXPluginGUI::CKXPluginGUI() "attaches" kX Mixer's skin to CKXPluginGUI::mf kFile object
        //  (that is, mf->next=kX Mixer's mf)
        // CKXPluginGUI::init() "sets" <this.kxl> built-in skin where kxskin.ini is stored [if it exists].
        // CKXPluginGUI::init() also sets priority to '1', thus,
        //  kX Mixer's skin is always accessed -after- built-in skin

    int font_size,font_height,font_width;
    int controls_enabled; // prohibits UI Controls changes when False 

    void update_buttons(); // updates 'mute', 'bypass' etc. buttons

    CKXPluginGUI(kDialog * parent_,kFile *mf_);

    // do not override
        // ---------------
    int create();

    const char *get_class_name() { return "kXFXTweaker"; }; // don't change

        // these should be called within init()
    void create_dialog(const char *caption,int width, int height, int ext_height=-1,int flags=0);
    void resize_dialog(const char *caption,int width, int height, int ext_height=-1,int flags=0);
     #define KXFX_NO_RESET  1
     #define KXFX_NO_RESIZE 2
        void create_vfader(kFader &fader,UINT ctrl_id,const char *label,int minval,int maxval,int x,int y,int wd,int ht);
    void create_hfader(kFader &fader,UINT ctrl_id,const char *label,int minval,int maxval,int left,int top,int width,int label_width,int value_symbols=5);
        void create_vslider(kSlider &slider,UINT ctrl_id,const char *label,int minval,int maxval,int x,int y,int wd,int ht);
    void create_hslider(kSlider &slider,UINT ctrl_id,const char *label,int minval,int maxval,int left,int top,int wd,int ht);
    void create_checkbox(kCheckButton &checkbox,UINT ctrl_id,const char *caption,int left,int top,int width);
    void create_label(kStatic &label,UINT ctrl_id,const char *caption,int left,int top,int width);
    void create_button(kButton &button,UINT ctrl_id,const char *caption,int left,int top,int width);
    void create_combo(kCombo &combo,UINT ctrl_id,const char *label,int x,int y,int wd,int n_items=-1,int drop_width=-1);

        // events:
        virtual int on_command(int wparam,int lparam);
        virtual void on_sys_command(int wparam,int lparam);
        virtual void on_post_nc_destroy()  { get_plugin()->cp = NULL; delete this; };

        // to be overridden by any plugin:
        // -------------------------------
        virtual void init();
    virtual iKXPlugin *get_plugin(); // should simply return 'plugin' member
    virtual void sync(int what=EVERYPM_ID);

        // -------------------------------
        // legacy stuff: to be removed when MFC library is finally removed
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar); // for vertical sliders
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar); // for vertical sliders
    afx_msg void OnCombo(UINT id);
    
    DECLARE_MESSAGE_MAP()

    virtual int draw_nc(kDraw &);
private:
    void update_preset_name();
};

class KX_CLASS_TYPE CKXDSPWindow : public iKXDSPWindow
{
public:
 CKXDSPWindow(kDialog *parent_,kWindow *that_,iKXPlugin *plugin_,kFile *mf_);

 // some colors (from the skin)
 dword cc_trans_bk,cc_trans_pen,
    cc_untrans_bk,cc_untrans_pen,
    cc_enabled_text,cc_enabled_text_bk,
    cc_dis_trans_text,cc_dis_trans_bk,
    cc_dis_untrans_text,cc_dis_untrans_bk,
    cc_in_bk,cc_in_pen,
    cc_out_bk,cc_out_pen,cc_shadow;

 // the size of 'connection' and the height of the header
 int conn_size,header_height;

 // window size
 int wnd_wd,wnd_ht;

 virtual int draw(kDraw *dc,int x,int y,int flag,const char *name,int pgm_id);
 // flag: dsp_microcode.flag
 virtual int find_gpr(const kPoint *pt,int *gpr,int flag,char *name=NULL);
 virtual int get_window_rect(char *name,int flag,kSize *sz);
 virtual int get_connection_rect(int io_num,int type,int flag,kRect *pos);
 // flags: 0 - inputs; 1 - outputs

 virtual int configure(); // should load skin-related settings from kFile
 virtual int set_connector_size(int sz);
};


// retrieve position and set vertical slider (because it's inverted actually) with this macros
#define VSGETPOS(slider)    slider.get_range_max() - slider.get_pos() + slider.get_range_min()
#define VSSETPOS(slider,v)  do { slider.set_pos(slider.get_range_max() - v + slider.get_range_min()); } while(0)

#define MAX_LABEL_STRING    64

#endif // _KX_FXSTDGUI_H
