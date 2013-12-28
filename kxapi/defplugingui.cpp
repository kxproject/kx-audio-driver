// kX Driver / kX Driver Interface / kX Driver Effects Library
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

#include "stdafx.h"

#include "gui/kGui.h"
#include "interface/kxplugingui.h"
#include "defplugingui.h"

void iPluginDlg::init()
{
	CKXPluginGUI::init();

	int cnt=plugin->get_param_count();

	#define SLIDER_HEIGHT		25
	#define SLIDER_WIDTH		250
	#define SLIDER_WIDTH_GAP	5
	#define SLIDER_HEIGHT_GAP	5
	#define MAX_HEIGHT		((SLIDER_HEIGHT+SLIDER_HEIGHT_GAP)*10)

	int x=10;
	int y=10;
	int max_y=y+SLIDER_HEIGHT_GAP+SLIDER_HEIGHT;
        int max_x=x+SLIDER_WIDTH_GAP+SLIDER_WIDTH;

        // re-calculate sizes
	for(int i=0;i<cnt;i++)
	{
	 if((i==cnt-1)) // last one?
	  break;

	 y+=SLIDER_HEIGHT+SLIDER_HEIGHT_GAP;
	 if(y>=MAX_HEIGHT)
	 {
	  y=10;
	  x+=SLIDER_WIDTH+SLIDER_WIDTH_GAP;
	 }
	 if(max_y<=y)
	  max_y=y+SLIDER_HEIGHT_GAP+SLIDER_HEIGHT;
	 if(max_x<=x)
	  max_x=x+SLIDER_WIDTH_GAP+SLIDER_WIDTH;
	}

	create_dialog(plugin->name, max_x+30, max_y+10);

	x=10; y=10;

	controls_enabled=1;

	for(int i=0;i<cnt;i++)
	{
	 kx_fxparam_descr descr;
	 plugin->describe_param(i,&descr);
	 create_hfader(faders[i], i, descr.name, 0, 0x7fffffff, x, y, SLIDER_WIDTH, 60, 7);

	 sync(i);

	 if((i==cnt-1)) // last one?
	  break;

	 y+=SLIDER_HEIGHT+SLIDER_HEIGHT_GAP;
	 if(y>=MAX_HEIGHT)
	 {
	  y=10;
	  x+=SLIDER_WIDTH+SLIDER_WIDTH_GAP;
	 }
	}
}


void iPluginDlg::sync(int what)
{
 char c[MAX_LABEL_STRING];

 if(what!=EVERYPM_ID)
 {
  kxparam_t param;
  plugin->get_param(what,&param);
  if(controls_enabled)
   faders[what].slider.set_pos((int)param);

  sprintf(c, "%.0f%%", (float)(param)/(float)21474836.47f);
  faders[what].svalue.SetWindowText(c);
 }
 else
 {
  int cnt=plugin->get_param_count();
  for(int i=0;i<cnt;i++)
  {
     kxparam_t param;
     plugin->get_param(i,&param);
     if(controls_enabled)
      faders[i].slider.set_pos((int)param);

     sprintf(c, "%.0f%%",(float)(param)/(float)21474836.47f);
     faders[i].svalue.SetWindowText(c);
  }
 }
}
