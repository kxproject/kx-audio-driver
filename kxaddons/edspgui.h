// kX E-DSP Control Add-on
// Copyright (c) Eugene Gavrilov, 2008-2009.
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


#include "gui/kxdialog.h"

class EDSPDialog : public CKXDialog
{
public:
    iEDSPControlAddOn *addon;
    int current_ikx_device;

    EDSPDialog(kFile *mf_,iEDSPControlAddOn *addon_) { mf=mf_; addon=addon_; current_ikx_device=addon->ikx->get_device_num(); };
    const char *get_class_name() { return "kXEDSPControl"; };

    kFile *mf;

    int select_device(int dev_=-1);
    virtual void init();
    virtual void on_destroy();

    kCheckButton digi_in[4],digi_out[3],ana_in[4],ana_out[5];

    kCombo midi_out[4],headphones,clock;
    kCheckButton midi_out_loop;
    kCombo midi_in[2];

    kStatic status;

    kButton reload_firmware;

    int on_command(int wparam,int lparam);

    DECLARE_MESSAGE_MAP()
};

#define DIGI_IN_ADAT    (WM_USER+0x100)
#define DIGI_IN_SPDIF   (WM_USER+0x101)
#define DIGI_IN_PRO     (WM_USER+0x102)
#define DIGI_IN_VALID   (WM_USER+0x103)

#define DIGI_OUT_ADAT    (WM_USER+0x104)
#define DIGI_OUT_SPDIF   (WM_USER+0x105)
#define DIGI_OUT_PRO     (WM_USER+0x106)

#define ANA_IN_0202     (WM_USER+0x107)
#define ANA_IN_ADC1     (WM_USER+0x108)
#define ANA_IN_ADC2     (WM_USER+0x109)
#define ANA_IN_ADC3     (WM_USER+0x10a)

#define ANA_OUT_0202     (WM_USER+0x10b)
#define ANA_OUT_DAC1     (WM_USER+0x10c)
#define ANA_OUT_DAC2     (WM_USER+0x10d)
#define ANA_OUT_DAC3     (WM_USER+0x10e)
#define ANA_OUT_DAC4     (WM_USER+0x10f)

#define HEADPHONES      (WM_USER+0x110)
#define EDSP_CLOCK      (WM_USER+0x111)
#define MIDIOUT0        (WM_USER+0x112)
#define MIDIOUT1        (WM_USER+0x113)
#define MIDIOUT2        (WM_USER+0x114)
#define MIDIOUT3        (WM_USER+0x115)
#define MIDIIN0         (WM_USER+0x116)
#define MIDIIN1         (WM_USER+0x117)

#define RELOAD_FIRMWARE (WM_USER+0x118)

BEGIN_MESSAGE_MAP(EDSPDialog, CKXDialog)
        // ON_CBN_SELCHANGE(, OnChange)
END_MESSAGE_MAP()

void EDSPDialog::init()
{
        generic_init("edspctrl");

        char tmp_str[256];

        if(!mf->get_profile("edspctrl","di1",tmp_str,sizeof(tmp_str)))
           gui_create_button(this,&digi_in[0],DIGI_IN_ADAT,tmp_str,*mf);
        if(!mf->get_profile("edspctrl","di2",tmp_str,sizeof(tmp_str)))
           gui_create_button(this,&digi_in[1],DIGI_IN_SPDIF,tmp_str,*mf);
        if(!mf->get_profile("edspctrl","di3",tmp_str,sizeof(tmp_str)))
           gui_create_button(this,&digi_in[2],DIGI_IN_PRO,tmp_str,*mf);
        if(!mf->get_profile("edspctrl","di4",tmp_str,sizeof(tmp_str)))
           gui_create_button(this,&digi_in[3],DIGI_IN_VALID,tmp_str,*mf);

        if(!mf->get_profile("edspctrl","do1",tmp_str,sizeof(tmp_str)))
           gui_create_button(this,&digi_out[0],DIGI_OUT_ADAT,tmp_str,*mf);
        if(!mf->get_profile("edspctrl","do2",tmp_str,sizeof(tmp_str)))
           gui_create_button(this,&digi_out[1],DIGI_OUT_SPDIF,tmp_str,*mf);
        if(!mf->get_profile("edspctrl","do3",tmp_str,sizeof(tmp_str)))
           gui_create_button(this,&digi_out[2],DIGI_OUT_PRO,tmp_str,*mf);

        if(!mf->get_profile("edspctrl","ai1",tmp_str,sizeof(tmp_str)))
           gui_create_button(this,&ana_in[0],ANA_IN_0202,tmp_str,*mf);
        if(!mf->get_profile("edspctrl","ai2",tmp_str,sizeof(tmp_str)))
           gui_create_button(this,&ana_in[1],ANA_IN_ADC1,tmp_str,*mf);
        if(!mf->get_profile("edspctrl","ai3",tmp_str,sizeof(tmp_str)))
           gui_create_button(this,&ana_in[2],ANA_IN_ADC2,tmp_str,*mf);
        if(!mf->get_profile("edspctrl","ai4",tmp_str,sizeof(tmp_str)))
           gui_create_button(this,&ana_in[3],ANA_IN_ADC3,tmp_str,*mf);

        if(!mf->get_profile("edspctrl","ao1",tmp_str,sizeof(tmp_str)))
           gui_create_button(this,&ana_out[0],ANA_OUT_0202,tmp_str,*mf);
        if(!mf->get_profile("edspctrl","ao2",tmp_str,sizeof(tmp_str)))
           gui_create_button(this,&ana_out[1],ANA_OUT_DAC1,tmp_str,*mf);
        if(!mf->get_profile("edspctrl","ao3",tmp_str,sizeof(tmp_str)))
           gui_create_button(this,&ana_out[2],ANA_OUT_DAC2,tmp_str,*mf);
        if(!mf->get_profile("edspctrl","ao4",tmp_str,sizeof(tmp_str)))
           gui_create_button(this,&ana_out[3],ANA_OUT_DAC3,tmp_str,*mf);
        if(!mf->get_profile("edspctrl","ao5",tmp_str,sizeof(tmp_str)))
           gui_create_button(this,&ana_out[4],ANA_OUT_DAC4,tmp_str,*mf);

        // Headphones
        if(!mf->get_profile("edspctrl","headphones",tmp_str,sizeof(tmp_str)))
        {
           kRect r; r.left=r.top=r.bottom=r.right=0;
           sscanf(tmp_str,"%d %d %d",&r.left,&r.top,&r.right);
           r.right+=r.left;
           r.bottom=r.top+30;

           headphones.create(r,CBS_DROPDOWNLIST /*| CBS_AUTOHSCROLL| WS_VSCROLL*/,HEADPHONES,this);
           headphones.add_string("DAC1");
           headphones.add_string("DAC2");
           headphones.add_string("DAC3");
           headphones.add_string("DAC4");

           add_tool((char *)(LPCTSTR)mf->get_profile("locals","headphones_edsp_192"),&headphones,HEADPHONES);
           headphones.set_dropped_width(100);
        }

        // Clock
        if(!mf->get_profile("edspctrl","clock",tmp_str,sizeof(tmp_str)))
        {
           kRect r; r.left=r.top=r.bottom=r.right=0;
           sscanf(tmp_str,"%d %d %d",&r.left,&r.top,&r.right);
           r.right+=r.left;
           r.bottom=r.top+30;

           clock.create(r,CBS_DROPDOWNLIST /*| CBS_AUTOHSCROLL| WS_VSCROLL*/,EDSP_CLOCK,this);
           clock.add_string("48kHz internal");
           clock.add_string("44.1kHz internal");
           clock.add_string("1010 SPDIF");
           clock.add_string("1010 ADAT");
           clock.add_string("Sync BNC");
           clock.add_string("2nd HANA");
           clock.add_string("96kHz internal");
           clock.add_string("192kHz internal");
           clock.add_string("88.2kHz internal");
           clock.add_string("176.4kHz internal");

           add_tool((char *)(LPCTSTR)mf->get_profile("locals","edsp_clock"),&clock,EDSP_CLOCK);
           clock.set_dropped_width(100);
        }
        // MIDI-Out
        for(int k=0;k<4;k++)
        {
             char tmp_hdr[16];
             sprintf(tmp_hdr,"midi_out%d",k);

             if(!mf->get_profile("edspctrl",tmp_hdr,tmp_str,sizeof(tmp_str)))
             {
                kRect r; r.left=r.top=r.bottom=r.right=0;
                sscanf(tmp_str,"%d %d %d",&r.left,&r.top,&r.right);
                r.right+=r.left;
                r.bottom=r.top+30;

                midi_out[k].create(r,CBS_DROPDOWNLIST /*| CBS_AUTOHSCROLL| WS_VSCROLL*/,MIDIOUT0+k,this);
                midi_out[k].add_string("1");
                midi_out[k].add_string("2");

                clock.set_dropped_width(100);
             }
        }

        // MIDI-in
        if(!mf->get_profile("edspctrl","midi_in0",tmp_str,sizeof(tmp_str)))
        {
           kRect r; r.left=r.top=r.bottom=r.right=0;
           sscanf(tmp_str,"%d %d %d",&r.left,&r.top,&r.right);
           r.right+=r.left;
           r.bottom=r.top+30;

           midi_in[0].create(r,CBS_DROPDOWNLIST /*| CBS_AUTOHSCROLL| WS_VSCROLL*/,MIDIIN0,this);
           midi_in[0].add_string("None");
           midi_in[0].add_string("0202");
           midi_in[0].add_string("Dock-1");
           midi_in[0].add_string("Dock-2");

           clock.set_dropped_width(100);
        }
        if(!mf->get_profile("edspctrl","midi_in1",tmp_str,sizeof(tmp_str)))
        {
           kRect r; r.left=r.top=r.bottom=r.right=0;
           sscanf(tmp_str,"%d %d %d",&r.left,&r.top,&r.right);
           r.right+=r.left;
           r.bottom=r.top+30;

           midi_in[1].create(r,CBS_DROPDOWNLIST /*| CBS_AUTOHSCROLL| WS_VSCROLL*/,MIDIIN1,this);
           midi_in[1].add_string("None");
           midi_in[1].add_string("0202");
           midi_in[1].add_string("Dock-1");
           midi_in[1].add_string("Dock-2");

           clock.set_dropped_width(100);
        }

        kColor fg=0xffffff;
        kColor bk=0xb0b000;
        kRect r; r.left=r.top=r.bottom=r.right=0;

        if(status.m_hWnd) status.destroy();

        if(!mf->get_profile("edspctrl","status_line",tmp_str,sizeof(tmp_str)))
        {
           // x,y,wd,ht
           sscanf(tmp_str,"%d %d %d %d %x %x",&r.left,&r.top,&r.right,&r.bottom,&fg,&bk);
        }
        status.create("",r.left,r.top,r.right,r.bottom,this,fg,bk);
        status.show();

        if(!mf->get_profile("edspctrl","reload_firmware",tmp_str,sizeof(tmp_str)))
        {
          gui_create_button(this,&reload_firmware,RELOAD_FIRMWARE,tmp_str,*mf);
          reload_firmware.show();
        }

        // re-init
        select_device(current_ikx_device);
}

int EDSPDialog::select_device(int dev_)
{
    if(dev_==-1)
      dev_=current_ikx_device;
    if(dev_!=current_ikx_device)
    {
        debug("!! kxmixer: invalid device for E-DSP control [%d, %d]\n",current_ikx_device,dev_);
        dev_=current_ikx_device;
    }

    CKXDialog::select_device(dev_); // this sets ikx_t pointer to current_ikx_device

    device_init("edspctrl");
    SetWindowText(mf->get_profile("lang","edspctrl.name")+" - "+ikx_t->get_device_name());

    // disable card selection
    ::EnableWindow(card_name.get_wnd(),FALSE);

    byte val=0;

    if(ikx_t->fpga_read(EMU_HANA_SPDIF_MODE,&val)==0)
    {
       if(val&EMU_HANA_SPDIF_MODE_TX_PRO) digi_out[2].set_check(1); else digi_out[2].set_check(0);
       if(val&EMU_HANA_SPDIF_MODE_RX_PRO) digi_in[2].set_check(1); else digi_in[2].set_check(0);
       if(val&EMU_HANA_SPDIF_MODE_RX_INVALID) digi_in[3].set_check(0); else digi_in[3].set_check(1);

       digi_in[2].show(); digi_in[2].redraw();
       digi_out[2].show(); digi_out[2].redraw();
       digi_in[3].show(); digi_in[3].redraw();
    }

    if(ikx_t->fpga_read(EMU_HANA_OPTICAL_TYPE,&val)==0)
    {
       if(val&EMU_HANA_OPTICAL_IN_ADAT)
       { digi_in[0].set_check(1); digi_in[1].set_check(0); } else { digi_in[0].set_check(0); digi_in[1].set_check(1); }

       if(val&EMU_HANA_OPTICAL_OUT_ADAT)
       { digi_out[0].set_check(1); digi_out[1].set_check(0); } else { digi_out[0].set_check(0); digi_out[1].set_check(1); }

       digi_in[0].show();
       digi_in[1].show();
       digi_out[0].show();
       digi_out[1].show();

       digi_in[0].redraw();
       digi_in[1].redraw();
       digi_out[0].redraw();
       digi_out[1].redraw();
    }

    if(ikx_t->fpga_read(EMU_HANA_ADC_PADS,&val)==0)
    {
      if(val&EMU_HANA_0202_ADC_PAD1) ana_in[0].set_check(0); else ana_in[0].set_check(1);
      if(val&EMU_HANA_DOCK_ADC_PAD1) ana_in[1].set_check(0); else ana_in[1].set_check(1);
      if(val&EMU_HANA_DOCK_ADC_PAD2) ana_in[2].set_check(0); else ana_in[2].set_check(1);
      if(val&EMU_HANA_DOCK_ADC_PAD3) ana_in[3].set_check(0); else ana_in[3].set_check(1);

      ana_in[0].show();
      ana_in[1].show();
      ana_in[2].show();
      ana_in[3].show();

      ana_in[0].redraw();
      ana_in[1].redraw();
      ana_in[2].redraw();
      ana_in[3].redraw();
    }

    if(ikx_t->fpga_read(EMU_HANA_DAC_PADS,&val)==0)
    {
      if(val&EMU_HANA_0202_DAC_PAD1) ana_out[0].set_check(0); else ana_out[0].set_check(1);
      if(val&EMU_HANA_DOCK_DAC_PAD1) ana_out[1].set_check(0); else ana_out[1].set_check(1);
      if(val&EMU_HANA_DOCK_DAC_PAD2) ana_out[2].set_check(0); else ana_out[2].set_check(1);
      if(val&EMU_HANA_DOCK_DAC_PAD3) ana_out[3].set_check(0); else ana_out[3].set_check(1);
      if(val&EMU_HANA_DOCK_DAC_PAD4) ana_out[4].set_check(0); else ana_out[4].set_check(1);

      ana_out[0].show();
      ana_out[1].show();
      ana_out[2].show();
      ana_out[3].show();
      ana_out[4].show();

      ana_out[0].redraw();
      ana_out[1].redraw();
      ana_out[2].redraw();
      ana_out[3].redraw();
      ana_out[4].redraw();      
    }

    if(ikx_t->fpga_read(EMU_HANA_DOCK_MISC,&val)==0)
    {
      headphones.show();
      headphones.set_selection((val>>4)&3);
    }

    if(ikx_t->fpga_read(EMU_HANA_WCLOCK,&val)==0)
    {
      clock.show();

      byte data[]=
      {
       0,1,2,3,4,5,-1,-1,
       6,8,-1,-1,-1,-1,-1,-1, // with EMU_HANA_WCLOCK_2X
       7,9,-1,-1,-1,-1,-1,-1,
       -1,-1,-1,-1,-1,-1,-1,-1 };

      clock.set_selection(data[val&(EMU_HANA_WCLOCK_SRC_MASK|EMU_HANA_WCLOCK_MULT_MASK)]);
    }

    if(ikx_t->fpga_read(EMU_HANA_MIDI_IN,&val)==0)
    {
      midi_in[0].set_selection(val&3);
      midi_in[0].show();
      midi_in[1].set_selection((val>>3)&3);
      midi_in[1].show();
    }
    if(ikx_t->fpga_read(EMU_HANA_MIDI_OUT,&val)==0)
    {
      if(val&EMU_HANA_MIDI_OUT_0202) midi_out[0].set_selection(1); else midi_out[0].set_selection(0);
      if(val&EMU_HANA_MIDI_OUT_DOCK1) midi_out[1].set_selection(1); else midi_out[1].set_selection(0);
      if(val&EMU_HANA_MIDI_OUT_DOCK2) midi_out[2].set_selection(1); else midi_out[2].set_selection(0);
      if(val&EMU_HANA_MIDI_OUT_SYNC2) midi_out[3].set_selection(1); else midi_out[3].set_selection(0);

      midi_out[0].show();
      midi_out[1].show();
      midi_out[2].show();
      midi_out[3].show();
    }

    /*
    if(ikx_t->fpga_read(EMU_HANA_DOCK_MISC,&val)==0)
     cfg.write("edsp","misc",val);
    */

    byte options=0,id=0;
    byte v1,v2,v3,v4,v5;

    ikx_t->fpga_read(EMU_HANA_OPTION_CARDS,&options);
    ikx_t->fpga_read(EMU_HANA_ID,&id);
    ikx_t->fpga_read(EMU_HANA_MAJOR_REV,&v1);
    ikx_t->fpga_read(EMU_HANA_MINOR_REV,&v2);
    ikx_t->fpga_read(EMU_DOCK_MAJOR_REV,&v3);
    ikx_t->fpga_read(EMU_DOCK_MINOR_REV,&v4);
    ikx_t->fpga_read(EMU_DOCK_BOARD_ID,&v5);

    kString tmp;
    tmp.Format("%s%s%s%s ID: %x, Hana: %d.%d Dock: %d.%d [rev %d]",
      (options&EMU_HANA_OPTION_HAMOA)?"0202 ":"",
      (options&EMU_HANA_OPTION_SYNC)?"Sync ":"",
      (options&EMU_HANA_OPTION_DOCK_ONLINE)?"Dock ":"",
      (options&EMU_HANA_OPTION_DOCK_OFFLINE)?"Dock ":"",
      id,v1,v2,v3,v4,v5);

    status.SetWindowText(tmp);

    return 0;
}

void EDSPDialog::on_destroy()
{
        // destroy all the controls here...
    if(card_name.m_hWnd) card_name.DestroyWindow();

    CKXDialog::on_destroy();
}

int EDSPDialog::on_command(int wParam, int lParam)
{
    byte val=0;

    switch(wParam)
    {
     case MIDIIN0|(CBN_SELCHANGE<<16):
           {
             ikx_t->fpga_read(EMU_HANA_MIDI_IN,&val);

             val=(val&(~0x3)) | (midi_in[0].get_selection()&3);

             ikx_t->fpga_write(EMU_HANA_MIDI_IN,val);
           }
           break;
     case MIDIIN1|(CBN_SELCHANGE<<16):
           {
             ikx_t->fpga_read(EMU_HANA_MIDI_IN,&val);

             val=(val&(~(0x3<<3))) | ((midi_in[1].get_selection()&3)<<3);

             ikx_t->fpga_write(EMU_HANA_MIDI_IN,val);
           }
           break;

     case MIDIOUT0|(CBN_SELCHANGE<<16):
           {
             ikx_t->fpga_read(EMU_HANA_MIDI_OUT,&val);

             val&=(~EMU_HANA_MIDI_OUT_0202);
             if(midi_out[0].get_selection()==1)
              val|=EMU_HANA_MIDI_OUT_0202;

             ikx_t->fpga_write(EMU_HANA_MIDI_OUT,val);
           }
           break;
     case MIDIOUT1|(CBN_SELCHANGE<<16):
           {
             ikx_t->fpga_read(EMU_HANA_MIDI_OUT,&val);

             val&=(~EMU_HANA_MIDI_OUT_DOCK1);
             if(midi_out[1].get_selection()==1)
              val|=EMU_HANA_MIDI_OUT_DOCK1;

             ikx_t->fpga_write(EMU_HANA_MIDI_OUT,val);
           }
           break;
     case MIDIOUT2|(CBN_SELCHANGE<<16):
           {
             ikx_t->fpga_read(EMU_HANA_MIDI_OUT,&val);

             val&=(~EMU_HANA_MIDI_OUT_DOCK2);
             if(midi_out[2].get_selection()==1)
              val|=EMU_HANA_MIDI_OUT_DOCK2;

             ikx_t->fpga_write(EMU_HANA_MIDI_OUT,val);
           }
           break;
     case MIDIOUT3|(CBN_SELCHANGE<<16):
           {
             ikx_t->fpga_read(EMU_HANA_MIDI_OUT,&val);

             val&=(~EMU_HANA_MIDI_OUT_SYNC2);
             if(midi_out[3].get_selection()==1)
              val|=EMU_HANA_MIDI_OUT_SYNC2;

             ikx_t->fpga_write(EMU_HANA_MIDI_OUT,val);
           }
           break;

     case EDSP_CLOCK|(CBN_SELCHANGE<<16):
            {
               if(clock.get_selection()!=0)
                MessageBox("Not implemented yet\n");
            }
            break;

     case HEADPHONES|(CBN_SELCHANGE<<16):
            {
              ikx_t->fpga_read(EMU_HANA_DOCK_MISC,&val);

              val&=~(EMU_HANA_DOCK_PHONES_192_DAC1|EMU_HANA_DOCK_PHONES_192_DAC2|EMU_HANA_DOCK_PHONES_192_DAC3|EMU_HANA_DOCK_PHONES_192_DAC4);

              val|=(headphones.get_selection()<<4);

              ikx_t->fpga_write(EMU_HANA_DOCK_MISC,val);
            }
            break;

     case DIGI_IN_SPDIF:
            {
            ikx_t->fpga_read(EMU_HANA_OPTICAL_TYPE,&val);

            if(digi_in[1].get_check())
             val|=EMU_HANA_OPTICAL_IN_ADAT;
            else
             val&=(~EMU_HANA_OPTICAL_IN_ADAT);

            ikx_t->fpga_write(EMU_HANA_OPTICAL_TYPE,val);
            }
            break;
     case DIGI_IN_ADAT:
            {
            ikx_t->fpga_read(EMU_HANA_OPTICAL_TYPE,&val);

            if(digi_in[0].get_check())
             val&=(~EMU_HANA_OPTICAL_IN_ADAT);
            else
             val|=EMU_HANA_OPTICAL_IN_ADAT;

            ikx_t->fpga_write(EMU_HANA_OPTICAL_TYPE,val);
            }
            break;
     case DIGI_IN_PRO:
            {
            ikx_t->fpga_read(EMU_HANA_SPDIF_MODE,&val);

            if(digi_in[2].get_check())
             val&=(~EMU_HANA_SPDIF_MODE_RX_PRO);
            else
             val|=EMU_HANA_SPDIF_MODE_RX_PRO;

            ikx_t->fpga_write(EMU_HANA_SPDIF_MODE,val);
            }
            break;
     case DIGI_OUT_SPDIF:
            {
            ikx_t->fpga_read(EMU_HANA_OPTICAL_TYPE,&val);

            if(digi_out[1].get_check())
             val|=EMU_HANA_OPTICAL_OUT_ADAT;
            else
             val&=(~EMU_HANA_OPTICAL_OUT_ADAT);

            ikx_t->fpga_write(EMU_HANA_OPTICAL_TYPE,val);
            }
            break;
     case DIGI_OUT_ADAT:
            {
            ikx_t->fpga_read(EMU_HANA_OPTICAL_TYPE,&val);

            if(digi_out[0].get_check())
             val&=(~EMU_HANA_OPTICAL_OUT_ADAT);
            else
             val|=EMU_HANA_OPTICAL_OUT_ADAT;

            ikx_t->fpga_write(EMU_HANA_OPTICAL_TYPE,val);
            }
            break;
     case DIGI_OUT_PRO:
            {
            ikx_t->fpga_read(EMU_HANA_SPDIF_MODE,&val);

            if(digi_out[2].get_check())
             val&=(~EMU_HANA_SPDIF_MODE_TX_PRO);
            else
             val|=EMU_HANA_SPDIF_MODE_TX_PRO;

            ikx_t->fpga_write(EMU_HANA_SPDIF_MODE,val);
            }
            break;
     case ANA_IN_0202:
            {
            ikx_t->fpga_read(EMU_HANA_ADC_PADS,&val);

            if(!ana_in[0].get_check())
             val&=(~EMU_HANA_0202_ADC_PAD1);
            else
             val|=EMU_HANA_0202_ADC_PAD1;

            ikx_t->fpga_write(EMU_HANA_ADC_PADS,val);
            }
            break;
     case ANA_IN_ADC1:
            {
            ikx_t->fpga_read(EMU_HANA_ADC_PADS,&val);

            if(!ana_in[1].get_check())
             val&=(~EMU_HANA_DOCK_ADC_PAD1);
            else
             val|=EMU_HANA_DOCK_ADC_PAD1;

            ikx_t->fpga_write(EMU_HANA_ADC_PADS,val);
            }
            break;
     case ANA_IN_ADC2:
            {
            ikx_t->fpga_read(EMU_HANA_ADC_PADS,&val);

            if(!ana_in[2].get_check())
             val&=(~EMU_HANA_DOCK_ADC_PAD2);
            else
             val|=EMU_HANA_DOCK_ADC_PAD2;

            ikx_t->fpga_write(EMU_HANA_ADC_PADS,val);
            }
            break;
     case ANA_IN_ADC3:
            {
            ikx_t->fpga_read(EMU_HANA_ADC_PADS,&val);

            if(!ana_in[3].get_check())
             val&=(~EMU_HANA_DOCK_ADC_PAD3);
            else
             val|=EMU_HANA_DOCK_ADC_PAD3;

            ikx_t->fpga_write(EMU_HANA_ADC_PADS,val);
            }
            break;

     case ANA_OUT_0202:
            {
            ikx_t->fpga_read(EMU_HANA_DAC_PADS,&val);

            if(!ana_out[0].get_check())
             val&=(~EMU_HANA_0202_DAC_PAD1);
            else
             val|=EMU_HANA_0202_DAC_PAD1;

            ikx_t->fpga_write(EMU_HANA_DAC_PADS,val);
            }
            break;
     case ANA_OUT_DAC1:
            {
            ikx_t->fpga_read(EMU_HANA_DAC_PADS,&val);

            if(!ana_out[1].get_check())
             val&=(~EMU_HANA_DOCK_DAC_PAD1);
            else
             val|=EMU_HANA_DOCK_DAC_PAD1;

            ikx_t->fpga_write(EMU_HANA_DAC_PADS,val);
            }
            break;
     case ANA_OUT_DAC2:
            {
            ikx_t->fpga_read(EMU_HANA_DAC_PADS,&val);

            if(!ana_out[2].get_check())
             val&=(~EMU_HANA_DOCK_DAC_PAD2);
            else
             val|=EMU_HANA_DOCK_DAC_PAD2;

            ikx_t->fpga_write(EMU_HANA_DAC_PADS,val);
            }
            break;
     case ANA_OUT_DAC3:
            {
            ikx_t->fpga_read(EMU_HANA_DAC_PADS,&val);

            if(!ana_out[3].get_check())
             val&=(~EMU_HANA_DOCK_DAC_PAD3);
            else
             val|=EMU_HANA_DOCK_DAC_PAD3;

            ikx_t->fpga_write(EMU_HANA_DAC_PADS,val);
            }
            break;
     case ANA_OUT_DAC4:
            {
            ikx_t->fpga_read(EMU_HANA_DAC_PADS,&val);

            if(!ana_out[4].get_check())
             val&=(~EMU_HANA_DOCK_DAC_PAD4);
            else
             val|=EMU_HANA_DOCK_DAC_PAD4;

            ikx_t->fpga_write(EMU_HANA_DAC_PADS,val);
            }
            break;
     case RELOAD_FIRMWARE:
            {
              addon->init_firmware(1); // force
            }
            break;
      default:
            return CKXDialog::on_command(wParam,lParam);
    }

    select_device();

    return 0;
}
