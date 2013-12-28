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

#include "p16v_r_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CP16VRouterDlg, CKXDialog)
        ON_CBN_SELCHANGE(IDg_P16V_PLAYBACK, OnChangeP16vPlayback)
END_MESSAGE_MAP()


void CP16VRouterDlg::init()
{
    generic_init("p16v_router");

    char tmp_str[256];

    for(int i=0;i<4;i++)
    {
        char tmp[3];
        sprintf(tmp,"b%d",i+1);

            if(!mf.get_profile("p16v_router",tmp,tmp_str,sizeof(tmp_str)))
            {
             gui_create_button(this,&b_router[i],P16V_ROUTER_R1+i,tmp_str,mf);
            }
        }

        kRect r;
        r.left=30; r.top=100; r.right=100+100; r.bottom=30+30;
        p16v_pb.create(r,CBS_DROPDOWNLIST/* | CBS_AUTOHSCROLL| WS_VSCROLL*/,IDg_P16V_PLAYBACK,this);
    p16v_pb.add_string("2/3");
    p16v_pb.add_string("4/5");
    p16v_pb.add_string("6/7");
    p16v_pb.add_string("hph");
        add_tool((char *)(LPCTSTR)mf.get_profile("locals","p16v_playback"),&p16v_pb,IDg_P16V_PLAYBACK);
        p16v_pb.set_dropped_width(100);

    select_device();
}

int CP16VRouterDlg::select_device(int dev_)
{
    CKXDialog::select_device(dev_);

    // first: cleanup

        // now, create

        device_init("p16v_router");

        char tmp_str[256];

    if(!mf.get_profile("lang","p16v_router.name",tmp_str,sizeof(tmp_str)))
     SetWindowText((kString)tmp_str+" - "+ikx_t->get_device_name());

    for(int i=0;i<4;i++)
     b_router[i].hide();

    p16v_pb.hide();

    // read config
    dword v=0;
    if(ikx_t->get_hw_parameter(KX_HW_P16V_PB_ROUTING,&v)==0)
    {
     for(int i=0;i<4;i++)
      b_router[i].show();

     if(v&P16V_ROUTE_P_I2S)
      b_router[0].set_check(1); else b_router[0].set_check(0);

     if(v&P16V_ROUTE_I2S)
      b_router[1].set_check(1); else b_router[1].set_check(0);

     if(v&P16V_ROUTE_P_SPDIF)
      b_router[2].set_check(1); else b_router[2].set_check(0);

     if(v&P16V_ROUTE_SPDIF)
      b_router[3].set_check(1); else b_router[3].set_check(0);


         // reposition
         kRect r;
         r.left=r.top=r.bottom=r.right=0;

         if(!mf.get_profile("p16v_router","p16v_pb",tmp_str,sizeof(tmp_str)))
         {
          sscanf(tmp_str,"%d %d %d %d",&r.left,&r.top,&r.right,&r.bottom);
          r.right+=r.left;
          r.bottom+=r.top;
          p16v_pb.reposition(r);
          p16v_pb.set_selection((v>>8)&3);
          p16v_pb.show();
         }
         else debug("kxmixer: skin invalid [24/96 router]\n");
    }

        return 0;
}

void CP16VRouterDlg::on_destroy()
{
        // destroy all the controls here...
    if(card_name.m_hWnd) card_name.DestroyWindow();

    CKXDialog::on_destroy();
}

int CP16VRouterDlg::on_command(int wParam, int lParam)
{
    dword v=0;
    if(ikx_t->get_hw_parameter(KX_HW_P16V_PB_ROUTING,&v)==0)
    {
            switch(wParam)
            {
             case P16V_ROUTER_R1: v^=P16V_ROUTE_P_I2S; b_router[0].set_check(1-b_router[0].get_check()); break;
             case P16V_ROUTER_R1+1: v^=P16V_ROUTE_I2S; b_router[1].set_check(1-b_router[1].get_check());break;
             case P16V_ROUTER_R1+2: v^=P16V_ROUTE_P_SPDIF; b_router[2].set_check(1-b_router[2].get_check());break;
             case P16V_ROUTER_R1+3: v^=P16V_ROUTE_SPDIF; b_router[3].set_check(1-b_router[3].get_check());break;
            }
            ikx_t->set_hw_parameter(KX_HW_P16V_PB_ROUTING,v);
        }

    return CKXDialog::on_command(wParam,lParam);
}

void CP16VRouterDlg::OnChangeP16vPlayback()
{
 dword val=(p16v_pb.get_selection())&3;

 dword v=0;

 if(ikx_t->get_hw_parameter(KX_HW_P16V_PB_ROUTING,&v)==0)
 {
  ikx_t->set_hw_parameter(KX_HW_P16V_PB_ROUTING,(v&(~P16V_PB_DEF_ROUTE_MASK))|(val<<8));
 }
}
