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

#include "stdafx.h"

#include "..\edspctrl\edsplib.h"
#include "edsp.h"

#define printf debug
#include "..\edspctrl\edsplib.cpp"

#include "edspgui.h"
#include "interface/kxmixer.h"

int iEDSPControlAddOn::menu(int menu_stage,kMenu *mnu)
{
 switch(menu_stage)
 {
  case KXADDON_MENU_TOP:
    {
    kString port_name;
    dword port=0;
    ikx->get_dword(KX_DWORD_CARD_PORT,&port);
    port_name.Format(" [%04x]",port);
  	mnu->add(kx_manager->get_default_skin()->get_profile("lang","edspctrl.name") + port_name,menu_base+1);
  	break;
    }
  default:
  	return 0; // mnu not changed
 }
 return -1; // option added
}

int iEDSPControlAddOn::init(int p)
{
 edsp_dialog=NULL;

 if(check_versions())
  return -1;

 dword is_edsp=0;

 ikx=kx_manager->get_ikx(p);

 if(ikx && ikx->get_dword(KX_DWORD_IS_EDSP,&is_edsp)==0 && is_edsp)
 {
  init_firmware();
  return 0;
 }
 else
  return -2;
}

int iEDSPControlAddOn::close()
{
 return 0;
}

int iEDSPControlAddOn::save_settings(int where,kSettings &cfg)
{
 if(where!=KXADDON_PER_DEVICE)
  return 0;

 if(!is_fpga_programmed(ikx))
 {
   debug("!! edspctrl: cannot save settings of unprogrammed E-DSP FPGA\n");
   return 0;
 }

 byte val=0;

 if(ikx->fpga_read(EMU_HANA_DEFCLOCK,&val)==0)
  cfg.write("edsp","def_clock",val);
 if(ikx->fpga_read(EMU_HANA_SPDIF_MODE,&val)==0)
  cfg.write("edsp","spdif_mode",val);
 if(ikx->fpga_read(EMU_HANA_OPTICAL_TYPE,&val)==0)
  cfg.write("edsp","optical_type",val);
 if(ikx->fpga_read(EMU_HANA_MIDI_IN,&val)==0)
  cfg.write("edsp","midi_in",val);
 if(ikx->fpga_read(EMU_HANA_MIDI_OUT,&val)==0)
  cfg.write("edsp","midi_out",val);
 if(ikx->fpga_read(EMU_HANA_ADC_PADS,&val)==0)
  cfg.write("edsp","adc_pads",val);
 if(ikx->fpga_read(EMU_HANA_DAC_PADS,&val)==0)
  cfg.write("edsp","dac_pads",val);
 if(ikx->fpga_read(EMU_HANA_DOCK_MISC,&val)==0)
  cfg.write("edsp","misc",val);

  /*
 if(ikx->fpga_read(,&val)==0)
  cfg.write("edsp","",val);
  */
 return 0;
}

int iEDSPControlAddOn::load_settings(int where,kSettings &cfg)
{
 if(where!=KXADDON_PER_DEVICE)
  return 0;

 if(!is_fpga_programmed(ikx))
 {
   debug("!! edspctrl: cannot load settings into unprogrammed E-DSP FPGA\n");
   return 0;
 }

 dword val=0;

 ikx->fpga_write(EMU_HANA_UNMUTE,EMU_MUTE);

 if((cfg.read("edsp","def_clock",&val)==0))
   ikx->fpga_write(EMU_HANA_DEFCLOCK,(byte)val);
 if((cfg.read("edsp","spdif_mode",&val)==0))
   ikx->fpga_write(EMU_HANA_SPDIF_MODE,(byte)val);
 if((cfg.read("edsp","optical_type",&val)==0))
   ikx->fpga_write(EMU_HANA_OPTICAL_TYPE,(byte)val);
 if((cfg.read("edsp","midi_in",&val)==0))
   ikx->fpga_write(EMU_HANA_MIDI_IN,(byte)val);
 if((cfg.read("edsp","midi_out",&val)==0))
   ikx->fpga_write(EMU_HANA_MIDI_OUT,(byte)val);
 if((cfg.read("edsp","adc_pads",&val)==0))
   ikx->fpga_write(EMU_HANA_ADC_PADS,(byte)val);
 if((cfg.read("edsp","dac_pads",&val)==0))
   ikx->fpga_write(EMU_HANA_DAC_PADS,(byte)val);
 if((cfg.read("edsp","misc",&val)==0))
   ikx->fpga_write(EMU_HANA_DOCK_MISC,(byte)val);
 /*
 if((cfg.read("edsp","",&val)==0))
   ikx->fpga_write(,(byte)a);
 */

 ikx->fpga_write(EMU_HANA_UNMUTE,EMU_UNMUTE);

 return 0;
}

int iEDSPControlAddOn::reset_settings(int where)
{
 if(where!=KXADDON_PER_DEVICE)
  return 0;

 load_defaults(ikx);

 return 0;
}


int iEDSPControlAddOn::event(int event,void *p1,void *p2)
{
 switch(event)
 {
  case KXADDON_EVENT_MENU:
  	{
  	 intptr_t item=(intptr_t)p1; // menu_base-based
  	 item-=menu_base;

  	 switch(item)
  	 {
  	  case 1:
        activate();
  	  	return 0; // message processed
  	 }
  	}
  	break;
  case KXADDON_EVENT_GENERIC:
    {
     intptr_t item=(intptr_t)p1;
     switch(item)
     {
      case IKX_EDSP_CONFIG_CHANGED:
        {
           init_firmware();

           kSettings cfg(ikx->get_device_name());
           load_settings(1,cfg);
        }
        break;
     }
    }
    break;
  case KXADDON_EVENT_POWER:
    {
      switch((intptr_t)p1)
      {
        case KXADDON_EVENT_POWER_SLEEP:
           {
            debug("kxmixer: saving E-DSP state before entering 'sleep' mode\n");
            kSettings cfg(ikx->get_device_name());
            save_settings(1,cfg);
           }
           break;
        case KXADDON_EVENT_POWER_RESUME:
           {
            debug("kxmixer: restoring E-DSP state after leaving 'sleep' mode\n");
            init_firmware(1); // force update of E-DSP FPGA microcode

            kSettings cfg(ikx->get_device_name());
            load_settings(1,cfg);
           }
           break;
      }
    }
    break;
 }

 return -1;
}

int iEDSPControlAddOn::activate()
{
    if(!edsp_dialog)
    {
     HWND systray;

     systray = FindWindow(KX_DEF_WINDOW_CLASS,KX_DEF_WINDOW);  // linked with 'kxsetup.exe'  & Enum...

     edsp_dialog=new EDSPDialog(kx_manager->get_default_skin(),this);
     edsp_dialog->create((kDialog *)kDialog::FromHandle(systray));
     edsp_dialog->set_that((kDialog **)&edsp_dialog);
     edsp_dialog->show();
    } 
     else 
    {
     edsp_dialog->show();
    }

    return 0;
}

int iEDSPControlAddOn::init_firmware(int force)
{
      debug("kxmixer: E-mu E-DSP initialization\n");

      int reload_defaults=0;
      int ret=0;

      if(!is_fpga_programmed(ikx) || force)
      {
       debug("kxmixer: FPGA is not programmed, upload FPGA firmware...\n");
       ret|=upload_card_firmware(ikx);

       // load_defaults(ikx);
       // reload_defaults=0;
      }
      else
       debug("kxmixer: FPGA appears to be already programmed\n");

      if(is_dock_connected(ikx))
      {
       if(!is_dock_online(ikx) || force)
       {
         debug("kxmixer: Dock is connected, but appears offline, upload dock firmware...\n");

         ret|=upload_dock_firmware(ikx);
         // load_defaults(ikx);
         // reload_defaults=0;

       } else debug("kxmixer: Dock is connected and is functioning properly\n");
      } else debug("kxmixer: Dock is not connected\n");

      if(force)
       load_defaults(ikx);

      if(ret)
       MessageBox(NULL,"E-DSP initialization failed. Please check debug log for details","kX E-DSP Control",MB_OK|MB_ICONSTOP);

      return 0;
}
