// kX ASIO Driver
// Copyright (c) Eugene Gavrilov, 2001-2008.
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


// inlined

typedef struct
{
 ASIOSampleRate sampleRate;
 int bps;
 int num_inputs,num_outputs;
 int block_frames;
 int asio_method;
}driver_parameters_t;

struct method_list_t
{
 int mask;
 char *name;
};

#define MAX_KX_CAPS     16

static int tmp_device=0;
static driver_parameters_t driver_parameters;
static kx_caps device_caps[MAX_KX_CAPS];
static int total_caps=0;

#define add_item(combo,a) \
  SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)a);

static BOOL CALLBACK MyEnumWindowsProc(HWND hwnd,LPARAM lParam)
{
	int *kx_found=(int *)lParam;
	char name[1024];
	GetWindowText(hwnd,name,1024);
	if(strcmp(name,KX_DEF_WINDOW)==NULL) // linked
	{
	  SendMessage(hwnd,WM_KXMIXER_TWEAK_ROUTER,tmp_device,0);
	  (*kx_found)++;
	}
	return TRUE;
}

method_list_t method_list[]=
{
 { KXASIO_METHOD_DEFAULT,"Default" },

 { KXASIO_METHOD_NEW|KXASIO_METHOD_SLEEP,"User/Sleep" },
 { KXASIO_METHOD_NEW|KXASIO_METHOD_THREAD,"User/Thread" },
 { KXASIO_METHOD_NEW|KXASIO_METHOD_TIMER,"User/Timer" },
 { KXASIO_METHOD_NEW|KXASIO_METHOD_NOWAIT,"User/SMP" },

 { KXASIO_METHOD_NEW|KXASIO_METHOD_WAIT_IN_KERNEL|KXASIO_METHOD_SLEEP,"Krnl/Sleep" },
 { KXASIO_METHOD_NEW|KXASIO_METHOD_WAIT_IN_KERNEL|KXASIO_METHOD_NOWAIT,"Krnl/SMP" },

 { KXASIO_METHOD_NEW|KXASIO_METHOD_MAP_TOGGLE|KXASIO_METHOD_USE_HWIRQ|KXASIO_METHOD_SLEEP,"Map/Irq/Sleep (10k2)" },
 { KXASIO_METHOD_NEW|KXASIO_METHOD_MAP_TOGGLE|KXASIO_METHOD_USE_HWTIMER|KXASIO_METHOD_SLEEP,"Map/Timer/Sleep" },

 { KXASIO_METHOD_NEW|KXASIO_METHOD_MAP_TOGGLE|KXASIO_METHOD_USE_HWIRQ|KXASIO_METHOD_NOWAIT,"Map/Irq/SMP (10k2)" },

 { KXASIO_METHOD_NEW|KXASIO_METHOD_MAP_TOGGLE|KXASIO_METHOD_USE_HWTIMER|KXASIO_METHOD_SEND_EVENT,"Map/Timer/Event" },
 { KXASIO_METHOD_NEW|KXASIO_METHOD_MAP_TOGGLE|KXASIO_METHOD_USE_HWIRQ|KXASIO_METHOD_SEND_EVENT,"Map/Irq/Event (10k2)" },
 { 0, NULL }
};

INT_PTR CALLBACK DialogProc(
    HWND hwndDlg,	// handle to dialog box
    UINT uMsg,	// message
    WPARAM wParam,	// first message parameter
    LPARAM lParam 	// second message parameter
   )
{

 WPARAM wp=IDC_FORMAT|(CBN_SELCHANGE<<16);

 switch(uMsg)
 {
 	case WM_ERASEBKGND:
 		HDC dc;
 		dc=(HDC)wParam;
 		RECT r;
 		GetWindowRect(hwndDlg,&r);

 		HBRUSH c_b,prev_b;
 		c_b=CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
        prev_b=(HBRUSH)SelectObject(dc,c_b);
        Rectangle(dc,0,0,r.right-r.left,r.bottom-r.top);
        SelectObject(dc,prev_b);                
        DeleteObject(c_b);
 		return 1; // erased

 	case WM_COMMAND:
 		if(wParam==wp) // format has changed?
 		{
                // update buffer size after format change:
        		HWND bufsize_combo=GetDlgItem(hwndDlg,IDC_BUFFER_SIZE);
        		LRESULT bufsize_selection=SendMessage(bufsize_combo, CB_GETCURSEL,0,0);
        		SendMessage(bufsize_combo, CB_RESETCONTENT,0,0);

        		float sr;

        		LRESULT cursel=SendMessage(GetDlgItem(hwndDlg,IDC_FORMAT), CB_GETCURSEL,0,0);
                sr = (float)device_caps[cursel].sample_rate;

         		int l_1=32,l_2=48;
                for(int l=0;l<9;l++)
                {
                            char txt[64];

                            sprintf(txt,"%0.2f ms (%d samples)",l_1*1000.0f/sr,l_1);
                            add_item(bufsize_combo,txt);
                            l_1=l_1*2;

                            sprintf(txt,"%0.2f ms (%d samples)",l_2*1000.0f/sr,l_2);
                            add_item(bufsize_combo,txt);
                            l_2=l_2*2;
                 }

                 SendMessage(bufsize_combo, CB_SETCURSEL, bufsize_selection, 0);
 		}
 		else
 		switch(wParam)
 		{
 		 case IDOK:
 		    {
 		 	     LRESULT b_fr=SendMessage(GetDlgItem(hwndDlg,IDC_BUFFER_SIZE),CB_GETCURSEL,0,0);
        		 
        		 int id=0;
         		 int l_1=32,l_2=48;

                 for(int l=0;l<9;l++)
                 {
                            if(id==b_fr)
                            {
                             b_fr=l_1;
                             break;
                            }
                            l_1=l_1*2;
                            id++;

                            if(id==b_fr)
                            {
                             b_fr=l_2;
                             break;
                            }
                            l_2=l_2*2;
                            id++;
                 }
                 driver_parameters.block_frames=(int)b_fr;


                 LRESULT mode=SendMessage(GetDlgItem(hwndDlg,IDC_FORMAT),CB_GETCURSEL,0,0);
                 if(mode<0) mode=0;

                 driver_parameters.sampleRate=(ASIOSampleRate)device_caps[mode].sample_rate;
                 driver_parameters.bps=device_caps[mode].bps;
                 driver_parameters.num_inputs=device_caps[mode].n_ins;
                 driver_parameters.num_outputs=device_caps[mode].n_outs;


                 LRESULT method=SendMessage(GetDlgItem(hwndDlg,IDC_SYNC_METHOD),CB_GETCURSEL,0,0);
                 if(method_list[method].name!=0 && method_list[method].mask!=0)
                   driver_parameters.asio_method=method_list[method].mask;

 		         EndDialog(hwndDlg,1);
 		     }
 		     break;
 		 case IDCANCEL:
 		     EndDialog(hwndDlg,-1);
 		     break;
 		 case IDDEFAULTS:
 		     {
 		         SendMessage(GetDlgItem(hwndDlg,IDC_FORMAT), CB_SETCURSEL, 0, 0);
 		         SendMessage(GetDlgItem(hwndDlg,IDC_BUFFER_SIZE), CB_SETCURSEL, 7, 0);
 		         SendMessage(GetDlgItem(hwndDlg,IDC_SYNC_METHOD), CB_SETCURSEL, 0, 0);
 		     }
 		  break;
 		 case IDASIO:
 		  {
       		   // try running kX Mixer
       		   int kx_found=0;
                 EnumWindows(MyEnumWindowsProc,(LPARAM)&kx_found); // look for KX Windows
                 if(kx_found==0)
                 {
                   MessageBox(NULL,"Launch kX Mixer first!","kX ASIO",MB_OK|MB_ICONINFORMATION);
                   // launch kX Mixer -now-
                   EnumWindows(MyEnumWindowsProc,0); // look for KX Windows
                 }
       	  }
          break;
 		} // switch wParam

 		return TRUE;
 	case WM_INITDIALOG:
 		 SetDlgItemText(hwndDlg,IDC_COPYRIGHT,
 		     "kX ASIO Driver Version:"KX_DRIVER_VERSION_STR"\n"KX_COPYRIGHT_STR_R"\nASIO Technology by Steinberg");
 		
 		 KXAsio *pseudo_this=(KXAsio *)lParam;
 		 tmp_device=pseudo_this->device_num;

         float cur_sr=(float)pseudo_this->sampleRate;
         int cur_buf_size=pseudo_this->get_config(KXASIO_LATENCY);

         // buffer size combo:
         {
                 HWND bufsize_combo=GetDlgItem(hwndDlg,IDC_BUFFER_SIZE);
                 int l_1=32,l_2=48;
                 for(int l=0;l<9;l++)
                 {
                   char txt[64];
                   LRESULT id=0;

                   sprintf(txt,"%0.2f ms (%d samples)",l_1*1000.0f/cur_sr,l_1);
                   id=add_item(bufsize_combo,txt);
                   if(cur_buf_size==l_1)
                    SendMessage(bufsize_combo, CB_SETCURSEL, id, 0);
                   l_1=l_1*2;

                   sprintf(txt,"%0.2f ms (%d samples)",l_2*1000.0f/cur_sr,l_2);
                   id=add_item(bufsize_combo,txt);
                   if(cur_buf_size==l_2)
                    SendMessage(bufsize_combo, CB_SETCURSEL, id, 0);
                   l_2=l_2*2;
                 }
                 SendMessage(bufsize_combo, CB_SETDROPPEDWIDTH, 40, 0);
                 ShowWindow(bufsize_combo,SW_SHOW);
         }

         // format combo:
         {
                 HWND fmt_combo=GetDlgItem(hwndDlg,IDC_FORMAT);
                 EnableWindow(fmt_combo,TRUE);

                 int caps_sz=sizeof(device_caps);
                 if(pseudo_this->ikx->get_device_caps(device_caps,&caps_sz)==0 && caps_sz>0)
                 {
                   total_caps=caps_sz/sizeof(kx_caps);

                   for(int i=0;i<total_caps;i++)
                   {
                       LRESULT id=add_item(fmt_combo,device_caps[i].friendly_name);

                       if(pseudo_this->bps==device_caps[i].bps &&
                          (int)pseudo_this->sampleRate==device_caps[i].sample_rate &&
                          pseudo_this->num_outputs==device_caps[i].n_outs &&
                          pseudo_this->num_inputs==device_caps[i].n_ins)
                       {
                         SendMessage(fmt_combo, CB_SETCURSEL, id, 0);
                       }
                   }

                 } else EnableWindow(fmt_combo,FALSE);

                 SendMessage(fmt_combo, CB_SETDROPPEDWIDTH, 40, 0);
                 ShowWindow(fmt_combo,SW_SHOW);
         }

         // cardname
         {
                 HWND card=GetDlgItem(hwndDlg,IDC_CARD_NAME);
                 // actually, edit
                 SetWindowText(card,pseudo_this->ikx->get_device_name());
         }

         // method
         {
                 HWND sync_method=GetDlgItem(hwndDlg,IDC_SYNC_METHOD);
                 EnableWindow(sync_method,TRUE);

                 int cur_method=pseudo_this->get_config(KXASIO_ASIO_METHOD);

                 int i=0;
                 while(method_list[i].name)
                 {
                    LRESULT cur_item=SendMessage(sync_method, CB_ADDSTRING, 0, (LPARAM)method_list[i].name);
                    if(method_list[i].mask==cur_method)
                    {
                     SendMessage(sync_method, CB_SETCURSEL, cur_item, 0);
                     cur_method=-2;
                    }
                    i++;
                 }

                 if(cur_method!=-2) // not found?
                 {
                     LRESULT cur_item=SendMessage(sync_method, CB_ADDSTRING, 0, (LPARAM)"(Custom)");
                     SendMessage(sync_method, CB_SETCURSEL, cur_item, 0);
                 }

                 SendMessage(sync_method, CB_SETDROPPEDWIDTH, 40, 0);
                 ShowWindow(sync_method,SW_SHOW);
         }

 		 BringWindowToTop(hwndDlg);

 		 return TRUE;
 }

 return FALSE;
}

extern HINSTANCE hinstance;
//---------------------------------------------------------------------------------------------
ASIOError KXAsio::controlPanel()
{
	INT_PTR ret=DialogBoxParam(hinstance,MAKEINTRESOURCE(IDD_DIALOG),NULL,&DialogProc,(LPARAM)this);

	if(ret==1)
	{
	 	if(
	 	     (driver_parameters.block_frames!=blockFrames) 
	 	  || (driver_parameters.sampleRate!=sampleRate)
	 	  || (driver_parameters.bps!=bps) 
	 	  || (driver_parameters.num_inputs!=num_inputs) 
	 	  || (driver_parameters.num_outputs!=num_outputs) 
	 	  || (driver_parameters.asio_method!=get_config(KXASIO_ASIO_METHOD)))
	 	{
                  set_config(KXASIO_LATENCY,driver_parameters.block_frames);
                  set_config(KXASIO_FREQ,(int)driver_parameters.sampleRate);
                  set_config(KXASIO_BPS,(int)driver_parameters.bps);
                  set_config(KXASIO_N_PB_CHN,(int)driver_parameters.num_outputs);
                  set_config(KXASIO_N_REC_CHN,(int)driver_parameters.num_inputs);
                  set_config(KXASIO_ASIO_METHOD,driver_parameters.asio_method);

                  if(callbacks)
                  {
                   if(started)
                    stop();

                   if(!callbacks->asioMessage(kAsioResetRequest,0,0,0))
                  	MessageBox(NULL,"ASIO parameters have been changed.\nYou may need to restart your ASIO application or reset its Audio Engine\nfor the change to take effect.\n","kX ASIO",MB_OK|MB_ICONSTOP);
                  }
                  else
                  {
                   MessageBox(NULL,"ASIO parameters have been changed.\nYou may need to restart your ASIO application or reset its Audio Engine\nfor the change to take effect.\n","kX ASIO",MB_OK|MB_ICONSTOP);
	 	          }
	 	}
	}

	return ASE_OK;
}

int KXAsio::set_defaults(int save_settings)
{
        // defaults:
        num_inputs=16;
        num_outputs=16;
    	bps=16;
    	blockFrames=384;
    	sampleRate=48000.;

        int fr=48000;
		if(ikx)
			ikx->get_clock(&fr);
        sampleRate=(float)fr;

        if(save_settings)
        {
    			set_config(KXASIO_LATENCY,blockFrames);
				set_config(KXASIO_BPS,bps);
				set_config(KXASIO_FREQ,(int)sampleRate);
				set_config(KXASIO_N_PB_CHN,num_outputs);
				set_config(KXASIO_N_REC_CHN,num_inputs);
        }

        return 0;
}
