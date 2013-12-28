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
#include <tlhelp32.h>

#include "notify.h"
#include "midirouter.h"

static BOOL CALLBACK MyEnumWindowsProc(
    HWND hwnd,	// handle to parent window
    LPARAM  lparam	// application-defined value
   )
{
        LONG ex_c=GetWindowLong(hwnd,GWL_EXSTYLE);
        LONG c=GetWindowLong(hwnd,GWL_STYLE);
	TCHAR name[1024];
	GetWindowText(hwnd,name,1024);

//        if(!((ex_c&WS_EX_APPWINDOW) || ((c&(WS_VISIBLE|WS_SYSMENU))==(WS_VISIBLE|WS_SYSMENU))))
//	{
//         return TRUE;
//      }


	if(
	   (_tcsstr(name,_T("Windows Media Player"))!=NULL) ||
	   (_tcsstr(name,_T("WinDVD"))!=NULL) ||
	   (_tcsstr(name,_T("foobar2000"))!=NULL)
	   )
	{
	  *(HWND *)(lparam)=hwnd;
	  return FALSE;
	} 
	return TRUE;
}


static char *names[]=
{
"0",
"1",
"2",	
"3",
"4",
"5",
"6",
"7",
"8",
"9",
"stop/eject",
"play/pause",
"slow",
"prev",
"next",
"step",
"mute",
"vol+",
"vol-",
"eax",
"options",
"display",
"return",
"start/menu",
"close/cancel",
"up",
"down",
"fw",
"rw",
"select/ok",
"power",
"cmss",
"rec",

"x_cmss",
"x_volincr",
"x_voldecr",
"x_mute",

"mm_volincr",
"mm_voldecr",
"mm_mute",
"mm_prev",
"mm_next",
"mm_stop",
"mm_play",

"e-mail",
"media",
"search",
"favourites",
"home",
NULL
};

void process_custom(iKXNotifier *that,int msg)
{
 msg&=0xff; // 'speaker' flag
 debug("RemoteIR/MMKeyboard message: %s\n",names[msg]);
 HWND window=0;

 // redundancy...
 if(msg==KXR_KEYB_PREV) msg=KXR_PREV;
 if(msg==KXR_KEYB_NEXT) msg=KXR_NEXT;
 if(msg==KXR_KEYB_STOP) msg=KXR_STOP;
 if(msg==KXR_KEYB_PLAY) msg=KXR_PLAY;
 if(msg==KXR_X_CMSS) msg=KXR_CMSS;

 
 if(msg==KXR_PREV || msg==KXR_NEXT || msg==KXR_STOP || msg==KXR_PLAY)
 {
     // check for WinAmp...
     window=FindWindow("Winamp v1.x",NULL); 

     // find the player
     if(window!=NULL) // winamp 1.x
     {
       switch(msg)
       {
        case KXR_PLAY: 
        	{
        		LRESULT state=SendMessage(window,WM_USER,0,104);

        		if(state==1) // pause if playing
        		{ 
        		   that->osd_notify(ICON_PAUSE,""); 
        		   SendMessage(window,WM_COMMAND,40046,0);
        		}
        		    else
        		if(state==3) // 'unpause' if paused
        		{ 
        		   that->osd_notify(ICON_PLAY,""); 
        		   SendMessage(window,WM_COMMAND,40046,0);
        		}
        		else
        		{ // 'play' otherwise
        		   that->osd_notify(ICON_PLAY,""); 
        		   SendMessage(window,WM_COMMAND,40045,0);
        		}
        	}
        	break;
        case KXR_STOP: that->osd_notify(ICON_STOP,""); SendMessage(window,WM_COMMAND,40047,0); break;
        case KXR_PREV: that->osd_notify(ICON_PREV,""); SendMessage(window,WM_COMMAND,40044,0); break;
        case KXR_NEXT: that->osd_notify(ICON_NEXT,""); SendMessage(window,WM_COMMAND,40048,0); break;
       }
     }
     else
     {
      EnumWindows(MyEnumWindowsProc,(LPARAM)&window);

     if(window!=0)
     {
      // process 'play' etc. functions...
      TCHAR name[512];
      GetWindowText(window,name,512);

      static int prev_state=-1;
      if(msg==KXR_PLAY)
      {
       if(prev_state==-1) prev_state=0; else prev_state=1-prev_state;
      } else prev_state=-1;

      if(_tcsstr(name,_T("Media Player"))!=NULL)
      {
        // mediaplayer: 'rw: b pl: p pause: p stop:s forw: f' + ctrl
        int vk1=0;
        int vk2=0;
        int vk3=0;
        int vk4=0;
        switch(msg)
        {
         case KXR_PLAY: vk1=VK_MEDIA_PLAY_PAUSE; vk2=0x50; vk3=0x10; vk4=0x19; that->osd_notify(prev_state==0?ICON_PLAY:ICON_PAUSE,""); break; 
         case KXR_STOP: vk1=VK_MEDIA_STOP; vk2=0x53; vk3=0x13; vk4=0x1f; that->osd_notify(ICON_STOP,""); break;      
         case KXR_PREV: vk1=VK_MEDIA_PREV_TRACK; vk2=0x42; vk3=0x2; vk4=0x30; that->osd_notify(ICON_PREV,""); break;      
         case KXR_NEXT: vk1=VK_MEDIA_NEXT_TRACK; vk2=0x46; vk3=0x6; vk4=0x21; that->osd_notify(ICON_NEXT,""); break;      
        }
        INPUT in;

        SetForegroundWindow(window);

        in.type=INPUT_KEYBOARD;
        in.ki.wVk=VK_CONTROL;
        in.ki.wScan=0;
        in.ki.dwFlags=0;
        in.ki.time=0;
        in.ki.dwExtraInfo=0;
        SendInput(1,&in,sizeof(INPUT));

/*        PostMessage(window,WM_KEYDOWN,vk1,1);
        PostMessage(window,WM_KEYDOWN,VK_CONTROL,1);
        PostMessage(window,WM_KEYDOWN,vk2,1);
        PostMessage(window,WM_CHAR,vk3,(vk4<<16)|1);
        PostMessage(window,WM_KEYUP,vk2,1|0xc0000000);
        PostMessage(window,WM_KEYUP,VK_CONTROL,1|0xc0000000);
        PostMessage(window,WM_KEYUP,vk1,1|0xc0000000);
*/
        in.type=INPUT_KEYBOARD;
        in.ki.wVk=(WORD)vk2;
        in.ki.wScan=0;
        in.ki.dwFlags=0;
        in.ki.time=0;
        in.ki.dwExtraInfo=0;
        SendInput(1,&in,sizeof(INPUT));

        in.type=INPUT_KEYBOARD;
        in.ki.wVk=(WORD)vk2;
        in.ki.wScan=0;
        in.ki.dwFlags=KEYEVENTF_KEYUP;
        in.ki.time=0;
        in.ki.dwExtraInfo=0;
        SendInput(1,&in,sizeof(INPUT));

        in.type=INPUT_KEYBOARD;
        in.ki.wVk=VK_CONTROL;
        in.ki.wScan=0;
        in.ki.dwFlags=KEYEVENTF_KEYUP;
        in.ki.time=0;
        in.ki.dwExtraInfo=0;
        SendInput(1,&in,sizeof(INPUT));

      } // media player
       else
      if(_tcsstr(name,_T("WinDVD"))!=NULL)
      {
        // mediaplayer: 'rw: b pl: p pause: p stop:s forw: f' + ctrl
        int vk1=0;
        int vk2=0;

        switch(msg)
        {
         case KXR_PLAY: vk1=VK_MEDIA_PLAY_PAUSE; if(prev_state==0) vk2=VK_RETURN; else vk2=VK_SPACE; that->osd_notify(prev_state==0?ICON_PLAY:ICON_PAUSE,""); break; 
         case KXR_STOP: vk1=VK_MEDIA_STOP; vk2=VK_END; that->osd_notify(ICON_STOP,""); break;      
         case KXR_PREV: vk1=VK_MEDIA_PREV_TRACK; vk2=VK_PRIOR; that->osd_notify(ICON_PREV,""); break;      
         case KXR_NEXT: vk1=VK_MEDIA_NEXT_TRACK; vk2=VK_NEXT; that->osd_notify(ICON_NEXT,""); break;      
        }
        INPUT in;

        SetForegroundWindow(window);
/*
        PostMessage(window,WM_KEYDOWN,vk1,1);
        PostMessage(window,WM_KEYDOWN,vk2,1);
        PostMessage(window,WM_KEYUP,vk2,1|0xc0000000);
        PostMessage(window,WM_KEYUP,vk1,1|0xc0000000);
*/
        in.type=INPUT_KEYBOARD;
        in.ki.wVk=(WORD)vk2;
        in.ki.wScan=0;
        in.ki.dwFlags=0;
        in.ki.time=0;
        in.ki.dwExtraInfo=0;
        SendInput(1,&in,sizeof(INPUT));

        in.type=INPUT_KEYBOARD;
        in.ki.wVk=(WORD)vk2;
        in.ki.wScan=0;
        in.ki.dwFlags=KEYEVENTF_KEYUP;
        in.ki.time=0;
        in.ki.dwExtraInfo=0;
        SendInput(1,&in,sizeof(INPUT));

      } // WinDVD
       else
      if(_tcsstr(name,_T("foobar2000"))!=NULL)
      {
       DWORD proc_id=0;
       GetWindowThreadProcessId(window,&proc_id);
       if(proc_id)
       {
        HANDLE hth=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,proc_id);
        if(hth)
        {
           MODULEENTRY32 mde;
           memset(&mde,0,sizeof(mde));
           mde.dwSize=sizeof(mde);
           if(Module32First(hth,&mde)==TRUE)
           {
            debug("kxmixer: foobar found: %s [%s]\n",mde.szExePath,name);
            char path[MAX_PATH+16];
            sprintf(path,"\"%s\" ",mde.szExePath);
            switch(msg)
            {
             case KXR_PLAY: strcat(path,"/playpause"); that->osd_notify(prev_state==0?ICON_PLAY:ICON_PAUSE,""); break;
             case KXR_STOP: strcat(path,"/stop"); that->osd_notify(ICON_STOP,""); break;
             case KXR_PREV: strcat(path,"/prev"); that->osd_notify(ICON_PREV,""); break;
             case KXR_NEXT: strcat(path,"/next"); that->osd_notify(ICON_NEXT,""); break;
            }
            WinExec(path,SW_SHOWNA);
           }
           CloseHandle(hth);
        }
       }       
      } // Foobar
     } // player window found
    } // not winamp window
 } // play/stop/rev/next
 else
 {
 	int key=0,mod=0;

 	switch(msg)
 	{
 		case KXR_UP:    key=VK_UP; break;
 		case KXR_DOWN:  key=VK_DOWN; break;
 		case KXR_FW:    key=VK_RIGHT; break;
 		case KXR_RW:    key=VK_LEFT; break;
 		case KXR_SELECT:key=VK_RETURN; break;
 		case KXR_CLOSE: key=VK_ESCAPE; break;
 		case KXR_0:	key=0x30; break;
 		case KXR_1:	key=0x31; break;
 		case KXR_2:	key=0x32; break;
 		case KXR_3:	key=0x33; break;
 		case KXR_4:	key=0x34; break;
 		case KXR_5:	key=0x35; break;
 		case KXR_6:	key=0x36; break;
 		case KXR_7:	key=0x37; break;
 		case KXR_8:	key=0x38; break;
 		case KXR_9:	key=0x39; break;
 		case KXR_STEP:	key=VK_TAB; break;
 		case KXR_SLOW:  key=VK_ESCAPE; mod=1; break;
 		case KXR_RETURN:key=VK_BACK; break;
 		case KXR_OPTIONS:key=VK_APPS; break;
 		case KXR_DISPLAY: key=VK_MENU; break;
 		case KXR_START: key=VK_LWIN; break;
        }
        if(key)
        {
         INPUT in;

         if(mod)
         {
          in.type=INPUT_KEYBOARD;
          in.ki.wVk=VK_MENU;
          in.ki.wScan=0;
          in.ki.dwFlags=0;
          in.ki.time=0;
          in.ki.dwExtraInfo=0;
          SendInput(1,&in,sizeof(INPUT));
         }

         in.type=INPUT_KEYBOARD;
         in.ki.wVk=(WORD)key;
         in.ki.wScan=0;
         in.ki.dwFlags=0;
         in.ki.time=0;
         in.ki.dwExtraInfo=0;
         SendInput(1,&in,sizeof(INPUT));

         in.type=INPUT_KEYBOARD;
         in.ki.wVk=(WORD)key;
         in.ki.wScan=0;
         in.ki.dwFlags=KEYEVENTF_KEYUP;
         in.ki.time=0;
         in.ki.dwExtraInfo=0;
         SendInput(1,&in,sizeof(INPUT));

         if(mod)
         {
          in.type=INPUT_KEYBOARD;
          in.ki.wVk=VK_MENU;
          in.ki.wScan=0;
          in.ki.dwFlags=KEYEVENTF_KEYUP;
          in.ki.time=0;
          in.ki.dwExtraInfo=0;
          SendInput(1,&in,sizeof(INPUT));
         }
        }
        else
        {
         if(msg==KXR_POWER)
         {
          extern void kx_shutdown(void);

          kx_shutdown();
         }
        }
/* 
 KXR_CMSS	
 KXR_REC	
 KXR_EAX
*/
 } // not 'play/stop'...
}

