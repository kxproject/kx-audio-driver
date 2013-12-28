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

#include "speaker_dlg.h"

#include "dsutil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDTEST_ALL	0x8000
#define IDTEST_X	0x8001

int routings_table[TOTAL]=
{
DEF_AC3_CENTER_ROUTING,
DEF_AC3_LEFT_ROUTING,
DEF_3D_LEFT_ROUTING,
DEF_AC3_SLEFT_ROUTING,
DEF_AC3_SCENTER_ROUTING,
DEF_AC3_SRIGHT_ROUTING,
DEF_3D_RIGHT_ROUTING,
DEF_AC3_RIGHT_ROUTING,
DEF_AC3_SUBWOOFER_ROUTING
};

static char *sides_table[TOTAL]=
{
"n","nw","w","sw","s","se","e","ne","sub" };

int CSpeakerDialog::on_command(int wparam,int lparam)
{
 if(wparam==IDTEST_ALL)
 {
  show_channel(-1);

  for(int i=0;i<TOTAL;i++)
  {
   if(show_channel(i)==0) // valid
    test_channel(i);
  }

  show_channel(-1);
  return 1;
 }
 if(wparam>=IDTEST_X && wparam<(IDTEST_X+TOTAL))
 {
   show_channel(wparam-IDTEST_X);
   test_channel(wparam-IDTEST_X);

   show_channel(-1);
   return 1;
 }
 return CKXDialog::on_command(wparam,lparam);
}

void CSpeakerDialog::init()
{
	CKXDialog::init("sptest");

	//SetClassLong(m_hWnd,GCL_STYLE,GetClassLong(m_hWnd,GCL_STYLE)|0x00020000); // CS_DROPSHADOW

	SetIcon(AfxGetApp()->LoadIcon(IDR_ICON), TRUE);			// Set big icon
//	SetIcon(AfxGetApp()->LoadIcon(IDR_ICON), FALSE);		// Set small icon

        unsigned int transp_color=0x0;
        unsigned int alpha=90;
        unsigned int cc_method=0;

        char tmp_str[128];
        if(mf.get_profile("sptest","background",tmp_str,sizeof(tmp_str)))
        {
         tmp_str[0]=0;
        }

        gui_set_background(this,&mf,tmp_str,transp_color,alpha,cc_method,_T("mixer/speaker.jpg"));

        if(!mf.get_profile("sptest","quit_button",tmp_str,sizeof(tmp_str)))
        {
         gui_create_button(this,&b_ok,IDOK,tmp_str,mf);
         b_ok.ShowWindow(SW_SHOW);
        }
        if(!mf.get_profile("sptest","test_button",tmp_str,sizeof(tmp_str)))
        {
         gui_create_button(this,&b_test,IDTEST_ALL,tmp_str,mf);
         b_test.show();
        }

        // label
        if(!mf.get_profile("sptest","label",tmp_str,sizeof(tmp_str)))
        {
         dword fg=0,bk=0;
         kRect r; r.left=r.right=r.top=r.bottom=0;
         sscanf(tmp_str,"%d %d %d %d %x %x",&r.left,&r.top,&r.right,&r.bottom,&fg,&bk);
         label.create((char *)(LPCTSTR)mf.get_profile("sptest","label.text"),r.left,r.top,r.right,r.bottom,this,fg,bk);
         label.ShowWindow(SW_SHOW);
        }

        // create buttons here!
        if(!mf.get_profile("sptest","n_test_button",tmp_str,sizeof(tmp_str)))
        {
         gui_create_button(this,&b_8ps[0],IDTEST_X+0,tmp_str,mf);
//         b_8ps[0].show();
        }
        if(!mf.get_profile("sptest","nw_test_button",tmp_str,sizeof(tmp_str)))
        {
         gui_create_button(this,&b_8ps[1],IDTEST_X+1,tmp_str,mf);
//         b_8ps[1].show();
        }
        if(!mf.get_profile("sptest","w_test_button",tmp_str,sizeof(tmp_str)))
        {
         gui_create_button(this,&b_8ps[2],IDTEST_X+2,tmp_str,mf);
//         b_8ps[2].show();
        }
        if(!mf.get_profile("sptest","sw_test_button",tmp_str,sizeof(tmp_str)))
        {
         gui_create_button(this,&b_8ps[3],IDTEST_X+3,tmp_str,mf);
//         b_8ps[3].show();
        }
        if(!mf.get_profile("sptest","s_test_button",tmp_str,sizeof(tmp_str)))
        {
         gui_create_button(this,&b_8ps[4],IDTEST_X+4,tmp_str,mf);
//         b_8ps[4].show();
        }
        if(!mf.get_profile("sptest","se_test_button",tmp_str,sizeof(tmp_str)))
        {
         gui_create_button(this,&b_8ps[5],IDTEST_X+5,tmp_str,mf);
//         b_8ps[5].show();
        }
        if(!mf.get_profile("sptest","e_test_button",tmp_str,sizeof(tmp_str)))
        {
         gui_create_button(this,&b_8ps[6],IDTEST_X+6,tmp_str,mf);
//         b_8ps[6].show();
        }
        if(!mf.get_profile("sptest","ne_test_button",tmp_str,sizeof(tmp_str)))
        {
         gui_create_button(this,&b_8ps[7],IDTEST_X+7,tmp_str,mf);
//         b_8ps[7].show();
        }

        if(!mf.get_profile("sptest","sub_test_button",tmp_str,sizeof(tmp_str)))
        {
         gui_create_button(this,&b_8ps[8],IDTEST_X+8,tmp_str,mf);
//         b_8ps[8].show();
        }

	set_dragging(gui_get_show_content());
        CenterWindow(NULL);

        // re-init
	select_device();
}

int CSpeakerDialog::select_device(int dev_)
{
	CKXDialog::select_device(dev_);


        SetWindowText(mf.get_profile("lang","sptest.name")+" - "+ikx_t->get_device_name());

        // perform additional tests for 'real speaker settings'
        is_a2=0;
	ikx_t->get_hw_parameter(KX_HW_8PS,(dword *)&is_a2);
	is_a2&=KX_HW_8PS_ON;

	show_channel(-1); // show all
          
        return 0;
}

void CSpeakerDialog::test_channel(char * id,dword routing)
{
	CSound *s=NULL;
	CSoundManager *s_m=NULL;
	IDirectSoundBuffer *buff=NULL;

	int error; size_t size;
        byte *mem=(byte *)mf.load_data(id,&size,&error);
	if((mem==NULL) || error)
	{
		MessageBox((LPCTSTR)mf.get_profile("errors","sptest"),"kX Mixer");
		return;
	}
	if(size>16)
	 size-=16;

	s_m=new CSoundManager;

    if( FAILED( s_m->Initialize( ikx_t->get_dsound_guid(KX_WINMM_WAVE),this->m_hWnd, DSSCL_PRIORITY, 1, 32000, 16 ) ) )
    {
        MessageBox((LPCTSTR)mf.get_profile("errors","dx0"),"kX Mixer");
		goto FAILED_ERROR;
    }

	WAVEFORMATEX format;
	format.cbSize=sizeof(format);
	format.nChannels=1;
	format.nSamplesPerSec=32000;
	format.wBitsPerSample=16;

	format.nBlockAlign=format.nChannels*format.wBitsPerSample/8;
	format.nAvgBytesPerSec=format.nSamplesPerSec*format.wBitsPerSample/8;
	format.wFormatTag=1; // PCM

    if( FAILED(s_m->CreateFromMemory( &s, mem, 
		size,&format,
		DSBCAPS_LOCHARDWARE | DSBCAPS_GLOBALFOCUS, GUID_NULL,1 ) ) )
    {
		MessageBox((LPCTSTR)mf.get_profile("errors","dx7"),"kX Mixer",MB_OK|MB_ICONEXCLAMATION);
		goto FAILED_ERROR;
    }

    buff = s->GetBuffer( 0 );
    if(buff)
    {
          int rt;
          rt=ikx_t->set_dsound_routing(buff,routing);
          if(rt)
          {
			   	MessageBox((LPCTSTR)mf.get_profile("errors","cp_req"),
			   	"kX Mixer");
	  }
    }
    else
    {
    	MessageBox((LPCTSTR)mf.get_profile("errors","dx8"),"kX Mixer",MB_OK|MB_ICONEXCLAMATION);
    }

    s->Play(0,0);
{
    int delay=2000;

    kString t=mf.get_profile("sptest","sound_length");
    sscanf((LPCTSTR)t,"%d",&delay);

    Sleep(delay);
}
    s->Stop();
    s->Reset();

FAILED_ERROR:
	if(s)
		delete s;
	if(s_m)
		delete s_m;
	if(mem)
		free(mem);

    // FIXME !! MessageBox("Speaker test is not supported for your platform","kX Mixer");
}


int CSpeakerDialog::show_channel(int flag)
{
	int ret=-1;

	for(int i=0;i<TOTAL;i++)
	{
	 if(flag==i || flag==-1)
	 {
	  if(is_a2 || ((!is_a2) && (i!=SHOW_W && i!=SHOW_E && i!=SHOW_S)))
	  {
	   b_8ps[i].show();
	   ret=0;
	  }
	  else
	   b_8ps[i].hide();
	 }
	 else
	  b_8ps[i].hide();
	}
	redraw();

	return ret;
}

void CSpeakerDialog::test_channel(int chn) 
{
	dword routing=0;

	if( !ikx_t || ikx_t->get_routing(routings_table[chn],&routing))
	{
			MessageBox((LPCTSTR)mf.get_profile("errors","cp_req2"),
			  "kX Mixer");
			return;
	}

	SetCursor(LoadCursor(NULL,IDC_WAIT));

	show_channel(chn);

	char file_name[MAX_PATH];
	char test[KX_MAX_STRING]; sprintf(test,"%s_test_sound",sides_table[chn]);

	mf.get_profile("sptest",test,file_name,sizeof(file_name));

	debug("kXMixer: testing (#%d) def_: %d [file: %s] routing %x\n",chn,routings_table[chn],file_name,routing);

	test_channel(file_name,routing);

	SetCursor(LoadCursor(NULL,IDC_ARROW));
}
