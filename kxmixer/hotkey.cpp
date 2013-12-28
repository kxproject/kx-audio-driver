// kX Mixer
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


#include "stdinc.h"

#include "main_dlg.h"
#include "notify.h"

extern CMainDialog *mixer_window;
extern HWND systray;

void translate_keyboard(UINT ch,UINT rep,UINT fl)
{
// debug("keyb: %x %x %x\n",ch,rep,fl);
 if((ch>='1') && (ch<='7'))
 {
  if(mixer_window)
  {
   manager->launch(LAUNCH_MIXER);
   if(ch=='4')
   {
    if(mixer_window->is_aps)
     ch='8';
    if(mixer_window->is_k2ex)
     ch='1';
   }

   mixer_window->redraw_window(ch-'0'-1);
  }
 }
 switch(ch)
 {
  case 'd':
  case 'D':
  	manager->launch(LAUNCH_DSP);
  	break;
  case 'r':
  case 'R':
  	manager->launch(LAUNCH_ROUTER);
  	break;
  case 'a':
  case 'A':
  	manager->launch(LAUNCH_MIDI);
  	break;
  case 'e':
  case 'E':
  	manager->launch(LAUNCH_EDITOR,-1,-1,NULL);
  	break;
  case 'm':
  case 'M':
  	manager->launch(LAUNCH_MIXER);
 }
}



void update_mm_keyboard(dword onoff)
{
	if(onoff==-1)
	{
          kSettings cfg;
          onoff=0;
          cfg.read_abs("Mixer","MMKeyboard",&onoff);
	}

	  if(onoff)
	  {
/*
	  RegisterHotKey(systray,VK_BROWSER_SEARCH     ,0,VK_BROWSER_SEARCH     );
	  RegisterHotKey(systray,VK_BROWSER_FAVORITES  ,0,VK_BROWSER_FAVORITES  );
	  RegisterHotKey(systray,VK_BROWSER_HOME       ,0,VK_BROWSER_HOME       );
	  RegisterHotKey(systray,VK_LAUNCH_MAIL        ,0,VK_LAUNCH_MAIL        );
	  RegisterHotKey(systray,VK_LAUNCH_MEDIA_SELECT,0,VK_LAUNCH_MEDIA_SELECT);
*/
	  RegisterHotKey(systray,VK_VOLUME_MUTE        ,0,VK_VOLUME_MUTE        );
	  RegisterHotKey(systray,VK_VOLUME_DOWN        ,0,VK_VOLUME_DOWN        );
	  RegisterHotKey(systray,VK_VOLUME_UP          ,0,VK_VOLUME_UP          );
	  RegisterHotKey(systray,VK_MEDIA_NEXT_TRACK   ,0,VK_MEDIA_NEXT_TRACK   );
	  RegisterHotKey(systray,VK_MEDIA_PREV_TRACK   ,0,VK_MEDIA_PREV_TRACK   );
	  RegisterHotKey(systray,VK_MEDIA_STOP         ,0,VK_MEDIA_STOP         );
	  RegisterHotKey(systray,VK_MEDIA_PLAY_PAUSE   ,0,VK_MEDIA_PLAY_PAUSE   );
	  }
	  else
	  {
/*	  UnregisterHotKey(systray,VK_BROWSER_SEARCH     );
	  UnregisterHotKey(systray,VK_BROWSER_FAVORITES  );
	  UnregisterHotKey(systray,VK_BROWSER_HOME       );
	  UnregisterHotKey(systray,VK_LAUNCH_MAIL        );
	  UnregisterHotKey(systray,VK_LAUNCH_MEDIA_SELECT);
*/
	  UnregisterHotKey(systray,VK_VOLUME_MUTE        );
	  UnregisterHotKey(systray,VK_VOLUME_DOWN        );
	  UnregisterHotKey(systray,VK_VOLUME_UP          );
	  UnregisterHotKey(systray,VK_MEDIA_NEXT_TRACK   );
	  UnregisterHotKey(systray,VK_MEDIA_PREV_TRACK   );
	  UnregisterHotKey(systray,VK_MEDIA_STOP         );
	  UnregisterHotKey(systray,VK_MEDIA_PLAY_PAUSE   );
	  }
}

void signal_mm_keyboard(int msg)
{
 iKXNotifier *ctrl=manager->get_notifier(); // note: current device?
 if(!ctrl)
  return;

 kSettings cfg;
 dword mm_keyboard_enabled=0;
 cfg.read_abs("Mixer","MMKeyboard",&mm_keyboard_enabled);

 if(mm_keyboard_enabled)
  switch(msg)
  {
    case VK_BROWSER_SEARCH:	ctrl->process_message(KXR_SEARCH); break;
    case VK_BROWSER_FAVORITES:	ctrl->process_message(KXR_FAVOURITES); break;
    case VK_BROWSER_HOME:	ctrl->process_message(KXR_HOME); break;
    case VK_LAUNCH_MAIL:	ctrl->process_message(KXR_EMAIL); break;
    case VK_LAUNCH_MEDIA_SELECT:ctrl->process_message(KXR_MEDIA); break;

    case VK_VOLUME_MUTE:	ctrl->process_message(KXR_KEYB_MUTE); break;
    case VK_VOLUME_DOWN:	ctrl->process_message(KXR_KEYB_VOLDECR); break;
    case VK_VOLUME_UP:		ctrl->process_message(KXR_KEYB_VOLINCR); break;
    case VK_MEDIA_NEXT_TRACK:	ctrl->process_message(KXR_KEYB_NEXT); break;
    case VK_MEDIA_PREV_TRACK:	ctrl->process_message(KXR_KEYB_PREV); break;
    case VK_MEDIA_STOP:		ctrl->process_message(KXR_KEYB_STOP); break;
    case VK_MEDIA_PLAY_PAUSE:	ctrl->process_message(KXR_KEYB_PLAY); break;
  }
}
