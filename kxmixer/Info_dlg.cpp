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

#include "info_dlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int get_os_version(char *buff)
{
 // check windows version
 OSVERSIONINFOEX ver;
 memset(&ver,0,sizeof(ver));
 ver.dwOSVersionInfoSize=sizeof(ver);

 if(!GetVersionEx((OSVERSIONINFO *)&ver)) // failed?
 {
  memset(&ver,0,sizeof(ver));
  ver.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
  if(!GetVersionEx((OSVERSIONINFO *)&ver)) // again failed?
  {
   return -3;
  }
 }
 OSVERSIONINFO ver2;
 memset(&ver2,0,sizeof(ver2));
 ver2.dwOSVersionInfoSize=sizeof(ver2);
 GetVersionEx(&ver2);

 sprintf(buff,"OS version: [%d %d %d %s; %d %d %d %s; %d %d]\r\n",ver.dwPlatformId,ver.dwMajorVersion,ver.dwMinorVersion,ver.szCSDVersion,
   ver2.dwPlatformId,ver.dwMajorVersion,ver.dwMinorVersion,ver.szCSDVersion,
   sizeof(ver),sizeof(ver2));

 return 0;
}

void CInfoDialog::init()
{
	kString out;
	kString tmp;

	generic_init("about");

	SetIcon(AfxGetApp()->LoadIcon(IDR_ICON), TRUE);			// Set big icon
//	SetIcon(AfxGetApp()->LoadIcon(IDR_ICON), FALSE);		// Set small icon

	char str[KX_MAX_STRING];
	dword val;

	out.Format("       kX Project\r\nExpanding the Potential Exponentially\r\n"
	           "       www.kxproject.com\r\n"
        "----------------------------------------------------\r\n"
	"kX Audio Driver, kX Mixer "
 #ifndef KX_DEBUG
  " (Release)"
 #else
  " (Debug)"
 #endif
	"\r\n"KX_COPYRIGHT_STR_R"\r\n%s\r\n%s: "KX_VERSION_STR"\r\n"
		"%s: "__DATE__" "__TIME__"\r\n\r\n"
 "kX DSP effects library (c) Eugene Gavrilov,\r\n"
 " Hanz Petrov and Max Mikhailov, 2001-2008.\r\n\r\n"
 "kX VSTi / kX Automation (c) Eugene Gavrilov\r\n and Max Mikhailov, 2002-2008.\r\n\r\n"
 "Portions of code\r\n Copyright (c) Sergey Komarinsky, 2002-2005.\r\n\r\n"
 "Dane Assembler Copyright (c) 2001-2008 \r\n"
 " by Max Mikhailov and Eugene Gavrilov.\r\n\r\n"
 "kX Help file Copyright (c)\r\n"
 " Hanz Petrov, Eugene Gavrilov and \r\n"
 " Soeren Bovbjerg, 2002-2005.\r\n\r\n"
 "See file readme.html for the license agreement\r\n"
		"----------------------------------------------------\r\n",
		mf.get_profile("info","allrights"),
                mf.get_profile("info","version"),
                mf.get_profile("info","compiled")
		);

	out+="ASIO Technology by Steinberg\r\n";
        out+="CodeMax2.0 Copyright 1997-2000 Barry Allyn\r\n";
        out+="inflate 1.1.3 Copyright 1995-1998 Mark Adler\r\n";
        out+="sfArk SoundFont compression Copyright melodymachine.com\r\n";

    out+="----------------------------------------------------\r\n";

 tmp.Format("Credits\r\n----------------------------------------------------\r\n  Thanks to:\r\n"
 	"   Max Mikhailov\r\n"
 	"   Hanz Petrov\r\n"
 	"   Daniel Bertrand\r\n"
 	"   Soeren Bovbjerg\r\n"
 	"   LeMury\r\n"
 	"   Sergey Komarinsky\r\n"
 	"   Friedemann Wolpert\r\n"
 	"   Dennis Samuelsson\r\n"
 	"   Luki Goldschmidt\r\n"
 	"   Fabio Bizzetti\r\n"
 	"   Matt Young\r\n"
 	"   Tom Weber\r\n"
 	"   Cybercurve\r\n"
 	"   eYagos\r\n"
 	"   Robert Mazur\r\n"
 	"   Martijn van Eeten\r\n"
 	"   Remy TROTIN\r\n"
 	"   WORM\r\n"
 	"   moudi\r\n"
 	"   Treasure\r\n"
 	"   David Descheneau\r\n"
 	"   stachooo@poczta.fm\r\n"
 	"   Guayabo\r\n"
 	"   DK\r\n"
 	"   DM\r\n"
 	"   Vedran Miletic\r\n"
 	"   E. Sokol\r\n"
    "   Martin Borisov\r\n"
 	"   Russ\r\n"
    "   Dmitry Gorelov\r\n"
 	" full list is available here:\r\n"
 	"  www.kxproject.com/credits.php\r\n"
 	);
 out+=tmp;

     out+="----------------------------------------------------\r\n";

	char tmp_str[512];
        if(!mf.get_profile("skin","name",tmp_str,sizeof(tmp_str)))
        {
         tmp.Format("%s: %s\r\n",mf.get_profile("info","curskin"),tmp_str);
         out+=tmp;
        }
        if(!mf.get_profile("skin","author",tmp_str,sizeof(tmp_str)))
        {
         tmp.Format("%s: %s\r\n",mf.get_profile("info","author"),tmp_str);
         out+=tmp;
        }

    hw_info="----Hardware / Software configuration---------------\r\n";
	if(!ikx_t->get_string(KX_STR_DRIVER_NAME,str))
	{
     tmp.Format("%s:%s\r\n",mf.get_profile("info","driver"),str);
	 hw_info+=tmp;
	}

	if(!ikx_t->get_string(KX_STR_DRIVER_DATE,str))
	{
     tmp.Format("%s: %s\r\n",mf.get_profile("info","drvdate"),str);
	 hw_info+=tmp;
	}

	if(!ikx_t->get_string(KX_STR_DRIVER_VERSION,str))
	{
     tmp.Format("%s: %s\r\n",mf.get_profile("info","drvversion"),str);
	 hw_info+=tmp;
	}

	if(!ikx_t->get_string(KX_STR_DB_NAME,str))
	{
     tmp.Format("%s:%s\r\n",mf.get_profile("info","dbname"),str);
	 hw_info+=tmp;
	}

	 hw_info+=ikx_t->get_device_name();
	 hw_info+="\r\n";

        dword dev=0,subsys=0,rev=0;
        ikx_t->get_dword(KX_DWORD_DEVICE,&dev);
        ikx_t->get_dword(KX_DWORD_SUBSYS,&subsys);
        ikx_t->get_dword(KX_DWORD_CHIPREV,&rev);
        tmp.Format("PCI Information: \r\nDevice: %x Subsys: %x ChipRev: %x\r\n",
         dev,subsys,rev);
        hw_info+=tmp;


	int is_10k2=0;
	if(!ikx_t->get_dword(KX_DWORD_IS_10K2,(dword *)&is_10k2))
	{
		if(is_10k2)
		{
			tmp.Format("%s\r\n",mf.get_profile("info","is10k2"));
			hw_info+=tmp;
		}
	}

	int have_mpu=0;
	if(!ikx_t->get_dword(KX_DWORD_HAVE_MPU,(dword *)&have_mpu))
	{
		if(have_mpu)
		{
			tmp.Format("Card has MPU device\r\n");
			hw_info+=tmp;
		}
	}


	int is_51=0;
	if(!ikx_t->get_dword(KX_DWORD_IS_51,(dword *)&is_51))
	{
		if(is_51)
		{
			tmp.Format("%s\r\n",mf.get_profile("info","is51"));
			hw_info+=tmp;
		}
	}

	dword is_aps=0;
	if(!ikx_t->get_dword(KX_DWORD_IS_APS,&is_aps))
	{
		if(is_aps)
		{
			tmp.Format("Card is EMU APS\r\n");
			hw_info+=tmp;
		}
	}
	dword is_a2=0;
	if(!ikx_t->get_dword(KX_DWORD_IS_A2,&is_a2))
	{
		if(is_a2)
		{
			tmp.Format("Card is Audigy2-compatible\r\n");
			hw_info+=tmp;
		}
	}
	dword is_a2zsnb=0;
	if(!ikx_t->get_dword(KX_DWORD_IS_A2ZSNB,&is_a2zsnb))
	{
		if(is_a2zsnb)
		{
			tmp.Format("Card is Audigy2 ZS Notebook (PCMCIA)\r\n");
			hw_info+=tmp;
		}
	}
	dword is_bad_22x=0;
	if(!ikx_t->get_dword(KX_DWORD_IS_BAD_SB22X,&is_bad_22x))
	{
		if(is_bad_22x)
		{
			tmp.Format("Card requires SB22x hack\r\n");
			hw_info+=tmp;
		}
	}

	dword is_k2ex=0;
	if(!ikx_t->get_dword(KX_DWORD_IS_A2EX,&is_k2ex))
	{
		if(is_k2ex)
		{
			tmp.Format("Card is [Audigy2] Platinum Ex\r\n");
			hw_info+=tmp;
		}
	}

    dword is_k8=0;
	if(!ikx_t->get_dword(KX_DWORD_IS_K8,&is_k8))
	{
		if(is_k8)
		{
			tmp.Format("Card is p17V / A2Value-compatible [dev8]\r\n");
			hw_info+=tmp;
		}
	}

    dword is_a4=0;
	if(!ikx_t->get_dword(KX_DWORD_IS_A4,&is_a4))
	{
		if(is_a4)
		{
			tmp.Format("Card is Audigy4 Value\r\n");
			hw_info+=tmp;
		}
	}

	int have_ac97=0;
	if(!ikx_t->get_dword(KX_DWORD_AC97_PRESENT,&val))
	{
		have_ac97=val;
	}
	tmp.Format("Card %s AC97 codec\r\n",have_ac97?"HAS":"HAS NO");
	hw_info+=tmp;

	if(have_ac97)
	{
		if(!ikx_t->get_string(KX_STR_AC97CODEC_NAME,str))
		{
			tmp.Format("    Codec name: %s\r\n",str);
			hw_info+=tmp;
		}
		
		if(!ikx_t->get_string(KX_STR_AC973D_NAME,str))
		{
			tmp.Format("    3D Extension: %s\r\n",str);
			hw_info+=tmp;
		}

		if(!ikx_t->get_dword(KX_DWORD_AC97_IS20,&val))
		{
			if(val)
				tmp.Format("    Codec is 2.0 compliant\r\n");
			else
				tmp.Format("    Codec is 1.0 compliant\r\n");
			hw_info+=tmp;
		}

		if(!ikx_t->get_dword(KX_DWORD_AC97_CAPS,&val))
		{
			dword cap=val;

			tmp.Format("    Capabilities[%x]     :%s%s%s%s%s%s\r\n",
				cap,
				cap & 0x0001 ? " -dedicated MIC PCM IN channel-" : "",
				cap & 0x0002 ? " -reserved1-" : "",
				cap & 0x0004 ? " -bass & treble-" : "",
				cap & 0x0008 ? " -simulated stereo-" : "",
				cap & 0x0010 ? " -headphone out-" : "",
				cap & 0x0020 ? " -loudness-" : "");
			hw_info+=tmp;

			dword val = cap & 0x00c0;
			tmp.Format("    DAC resolutions  :%s%s%s\r\n",
				" -16-bit-",
				val & 0x0040 ? " -18-bit-" : "",
				val & 0x0080 ? " -20-bit-" : "");
			hw_info+=tmp;

			val = cap & 0x0300;
			tmp.Format("    ADC resolutions  :%s%s%s\r\n",
				" -16-bit-",
				val & 0x0100 ? " -18-bit-" : "",
				val & 0x0200 ? " -20-bit-" : "");
			hw_info+=tmp;
		}

		if(!ikx_t->get_dword(KX_DWORD_AC97_EXTID,&val))
		{
				dword cap=val;
				tmp.Format("    Ext Capabilities [%x]:%s%s%s%s%s%s%s\r\n",
					cap,
					cap & 0x0001 ? " -var rate PCM audio-" : "",
					cap & 0x0002 ? " -2x PCM audio out-" : "",
					cap & 0x0008 ? " -var rate MIC in-" : "",
					cap & 0x0040 ? " -PCM center DAC-" : "",
					cap & 0x0080 ? " -PCM surround DAC-" : "",
					cap & 0x0100 ? " -PCM LFE DAC-" : "",
					cap & 0x0200 ? " -slot/DAC mappings-" : "");
				hw_info+=tmp;
		}
	}

	int port=0;
	int irq=0;
	if(!ikx_t->get_dword(KX_DWORD_CARD_PORT,&val))
		port=val;
	if(!ikx_t->get_dword(KX_DWORD_CARD_IRQ,&val))
		irq=val;
	tmp.Format("Port: %x Irq: 0x%x(%d)\r\n",port,irq,irq);
	hw_info+=tmp;

    int	buf_size;
    if(!ikx_t->get_buffers(KX_PLAYBACK_BUFFER,&buf_size))
    {
    	tmp.Format("Playback buffer: %x\r\n",buf_size);
    	hw_info+=tmp;
    }
    if(!ikx_t->get_buffers(KX_RECORD_BUFFER,&buf_size))
    {
    	tmp.Format("Record buffer: %x\r\n",buf_size);
    	hw_info+=tmp;
    }
    if(!ikx_t->get_buffers(KX_AC3_BUFFERS,&buf_size))
    {
    	tmp.Format("Number of AC3 buffers: %x\r\n",buf_size);
    	hw_info+=tmp;
    }
    if(!ikx_t->get_buffers(KX_TANKMEM_BUFFER,&buf_size))
    {
    	tmp.Format("Tank memory: %d kb\r\n",buf_size/1024);
    	hw_info+=tmp;
    }
    if(!ikx_t->get_buffers(KX_GSIF_BUFFER,&buf_size))
    {
    	tmp.Format("GSIF buffer: %d samples\r\n",buf_size);
    	hw_info+=tmp;
    }

    char os_ver[128];
    if(get_os_version(os_ver)==0)
    {
     hw_info+=os_ver;
    }

    out+=hw_info;

 tmp.Format("----------------------------------------------------\r\n");
 out+=tmp;

 unsigned int transp_color=0x0;
 unsigned int alpha=90;
 unsigned int cc_method=0;

 if(mf.get_profile("about","background",tmp_str,sizeof(tmp_str)))
 {
  tmp_str[0]=0;
 }
 gui_set_background(this,&mf,tmp_str,transp_color,alpha,cc_method,_T("mixer/infodlg.jpg"));

 if(!mf.get_profile("about","copy_button",tmp_str,sizeof(tmp_str)))
 {
  gui_create_button(this,&b_copy,IDg_COPY,tmp_str,mf);
  b_copy.show();
 }

 RECT r; memset(&r,0,sizeof(r));

 // label
 if(!mf.get_profile("about","label",tmp_str,sizeof(tmp_str)))
 {
  dword fg=0,bk=0;
  sscanf(tmp_str,"%d %d %d %d %x %x",&r.left,&r.top,&r.right,&r.bottom,&fg,&bk);
  label.create((char *)(LPCTSTR)mf.get_profile("about","label.text"),r.left,r.top,r.right,r.bottom,this,fg,bk);
  label.show();
 }

 GetWindowRect(&r);
 ScreenToClient(&r);
 r.left+=5;
 r.top+=5;
 r.bottom-=25;
 r.right-=110;

 // position
 if(!mf.get_profile("about","listbox",tmp_str,sizeof(tmp_str)))
 {
  sscanf(tmp_str,"%d %d %d %d",&r.left,&r.top,&r.right,&r.bottom);
 }

 SetWindowText(mf.get_profile("lang","about.name")+" - "+ikx_t->get_device_name());
 set_dragging(gui_get_show_content());

 // reposition window
 CenterWindow(NULL);
 
 b_edit.Create(ES_MULTILINE | ES_AUTOVSCROLL | WS_BORDER |
             ES_READONLY | WS_VSCROLL,r,this,0 ); // 0- any ID number
 b_edit.SetWindowText((LPCTSTR)out);
 b_edit.ShowWindow(SW_SHOW);

 b_edit.SetFont(font);
}

int CInfoDialog::on_command(int wParam, int lParam)
{
	if(wParam==IDg_COPY)
	{
         if(::OpenClipboard(get_wnd()))
         {
          HGLOBAL mem=GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE|GMEM_ZEROINIT,strlen((LPCTSTR)hw_info)+1);
          if(mem)
          {
           LPVOID m=GlobalLock(mem);
           strcpy((char *)m,(LPCTSTR)hw_info);

           EmptyClipboard();
           
           SetClipboardData(CF_TEXT, mem);
           GlobalUnlock(mem);
          }
          CloseClipboard();
         }
	}
	return CKXDialog::on_command(wParam,lParam);
}
