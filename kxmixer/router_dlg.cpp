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

#include "router_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(CRouterDlg, CKXDialog)
        ON_WM_VSCROLL()
	ON_NOTIFY( TVN_SELCHANGED, 0x1000, OnTreeNotify )
END_MESSAGE_MAP()

void CRouterDlg::init()
{
	generic_init("router");

	select_device();
}

int CRouterDlg::select_device(int dev_)
{
	CKXDialog::select_device(dev_);

	// first: cleanup
	// destroy all the controls here...
	if(tree.m_hWnd) tree.DestroyWindow();

        // now, create
        kRect r;
        r.left=11; r.top=16;
        r.bottom=299; r.right=199;

        // list1
        char tmp_str[1024];
        if(!mf.get_profile("router","list1",tmp_str,sizeof(tmp_str)))
        {
         sscanf(tmp_str,"%d %d %d %d",&r.left,&r.top,&r.right,&r.bottom);
        }

        RECT rr; rr.left=r.left; rr.right=r.right; rr.top=r.top; rr.bottom=r.bottom;
        tree.Create(WS_VISIBLE|TVS_HASLINES|TVS_LINESATROOT|
          TVS_HASBUTTONS|TVS_SHOWSELALWAYS|
          TVS_DISABLEDRAGDROP,rr,this,0x1000);

        if(font.get_font())
         tree.SetFont(font);

        tree_items[0]=tree.InsertItem("Wave");
        tree_items[1]=tree.InsertItem("AC-3 / 3-D");
        tree_items[2]=tree.InsertItem("Synth1");
        tree_items[3]=tree.InsertItem("Synth2");
        tree_items[4]=tree.InsertItem("ASIO");

        tree.SetItemData(tree.InsertItem("Wave 0/1",tree_items[0]),DEF_WAVE01_ROUTING);
        tree.SetItemData(tree.InsertItem("Wave 2/3",tree_items[0]),DEF_WAVE23_ROUTING);
        tree.SetItemData(tree.InsertItem("Wave 4/5",tree_items[0]),DEF_WAVE45_ROUTING);
        tree.SetItemData(tree.InsertItem("Wave 6/7",tree_items[0]),DEF_WAVE67_ROUTING);
        tree.SetItemData(tree.InsertItem("Wave AC3 Passthru",tree_items[0]),DEF_AC3PASSTHROUGH_ROUTING);

        tree.SetItemData(tree.InsertItem("AC-3 Left",tree_items[1]),DEF_AC3_LEFT_ROUTING);
        tree.SetItemData(tree.InsertItem("AC-3 Center",tree_items[1]),DEF_AC3_CENTER_ROUTING);
        tree.SetItemData(tree.InsertItem("AC-3 Right",tree_items[1]),DEF_AC3_RIGHT_ROUTING);
        tree.SetItemData(tree.InsertItem("AC-3 Surround Left",tree_items[1]),DEF_AC3_SLEFT_ROUTING);
        tree.SetItemData(tree.InsertItem("AC-3 Surround Right",tree_items[1]),DEF_AC3_SRIGHT_ROUTING);
        tree.SetItemData(tree.InsertItem("AC-3 Subwoofer",tree_items[1]),DEF_AC3_SUBWOOFER_ROUTING);

        tree.SetItemData(tree.InsertItem("3-D Surround Center",tree_items[1]),DEF_AC3_SCENTER_ROUTING);
        tree.SetItemData(tree.InsertItem("3-D Left",tree_items[1]),DEF_3D_LEFT_ROUTING);
        tree.SetItemData(tree.InsertItem("3-D Right",tree_items[1]),DEF_3D_RIGHT_ROUTING);
	    tree.SetItemData(tree.InsertItem("3-D Top",tree_items[1]),DEF_3D_TOP_ROUTING);

        tree.SelectItem(tree_items[0]);

        for(int i=0;i<16;i++)
        {
         char tmp[32];
         sprintf(tmp,"Channel %d",i+1);
         tree.SetItemData(tree.InsertItem(tmp,tree_items[2]),DEF_SYNTH1_ROUTING+i);
         tree.SetItemData(tree.InsertItem(tmp,tree_items[3]),DEF_SYNTH2_ROUTING+i);
         sprintf(tmp,"kX Out %d",i);
         tree.SetItemData(tree.InsertItem(tmp,tree_items[4]),DEF_ASIO_ROUTING+i);
        }

        tree.SetItemData(tree.InsertItem("All Channels",tree_items[2]),ROUTING_LAST+100);
        tree.SetItemData(tree.InsertItem("All Channels",tree_items[3]),ROUTING_LAST+101);

        tree.RedrawWindow();
        empty_controls();

        device_init("router");

	if(!mf.get_profile("lang","router.name",tmp_str,sizeof(tmp_str)))
	 SetWindowText((kString)tmp_str+" - "+ikx_t->get_device_name());

        return 0;
}

void CRouterDlg::on_destroy()
{
        // destroy all the controls here...
        if(tree.m_hWnd) tree.DestroyWindow();
	if(card_name.m_hWnd) card_name.DestroyWindow();

	CKXDialog::on_destroy();
}

int CRouterDlg::on_command(int wParam, int lParam)
{
	if(wParam>=0x2000 && wParam<=0x2000+MAX_BUSES)
	{
		dword routing=0x3f3f3f3f,xrouting=0x3f3f3f3f;
		int is_10k2=ikx_t->get_dsp();
		int totals=0;

   		if((cur_item!=ROUTING_LAST+100) && (cur_item!=ROUTING_LAST+101))
   		  ikx_t->get_routing(cur_item,&routing,&xrouting);

	   		kMenu menu;

	   		menu.create();

   		 	for(unsigned int i=0;i<(is_10k2?64U:16U);i++)
   		 	{
                if(cur_item>=DEF_ASIO_ROUTING && cur_item<DEF_ASIO_ROUTING+MAX_ASIO_OUTPUTS)
                {
                   // for ASIO, limit number of FXBuses to 16
                   if(i>15)
                    break;
                }

                if((cur_item==DEF_WAVE01_ROUTING) ||
                     (cur_item==DEF_WAVE23_ROUTING) ||
                     (cur_item==DEF_WAVE45_ROUTING) ||
                     (cur_item==DEF_WAVE67_ROUTING))
                {
                   // for 3-D audio, limit number of FXBuses to 16
                   if(i>15)
                    break;
                }

   		 		if((i!=KX_GET_SEND_A(routing) &&
   		 		   i!=KX_GET_SEND_B(routing) &&
   		 		   i!=KX_GET_SEND_C(routing) &&
   		 		   i!=KX_GET_SEND_D(routing)) || (i==63))
   		 		{
   		 		   if((!is_10k2) || 
   		 		      (is_10k2 &&
   		 		       (
                                        (
                                          i!=KX_GET_SEND_E(xrouting) &&
                                          i!=KX_GET_SEND_F(xrouting) &&
                                          i!=KX_GET_SEND_G(xrouting) &&
                                          i!=KX_GET_SEND_H(xrouting)
                                        )
                                        || (i==63)
                                       )
                                      )
   		 		     )
   		 		   {
   		 		     kString tmp;
   		 		     tmp.Format("FXBus %d",i);
   		 		     menu.add(tmp,i+1000,(totals>16?MF_MENUBARBREAK:0));
   		 		     if(totals>16)
   		 		     {
   		 		      totals=0;
   		 		     }
   		 		     totals++;
   		 		   }
   		 		}
   		 	} // for

   		 	RECT r;
   		 	fx_routings[wParam-0x2000].GetWindowRect(&r);
   		 	extern HWND systray;
   		 	int ret=menu.track(TPM_LEFTALIGN|TPM_RETURNCMD,r.left,r.bottom,(uintptr_t)systray);

   		 	if(ret>=1000)
   		 	{
   		 	 if((cur_item!=ROUTING_LAST+100) && (cur_item!=ROUTING_LAST+101))
   		 	 {
              if((cur_item>=DEF_ASIO_ROUTING && cur_item<DEF_ASIO_ROUTING+MAX_ASIO_OUTPUTS) && is_10k2)
              {
                // modify ret
                routing=KX_MAKE_ROUTING((ret-1000)*2+32,(ret-1000)*2+32+1,KX_GET_SEND_C(routing),KX_GET_SEND_D(routing));
              }
              else
   		 	  switch(wParam)
   		 	  {
   		 	   case 0x2000: routing=KX_MAKE_ROUTING(ret-1000,KX_GET_SEND_B(routing),KX_GET_SEND_C(routing),KX_GET_SEND_D(routing)); break;
   		 	   case 0x2001: routing=KX_MAKE_ROUTING(KX_GET_SEND_A(routing),ret-1000,KX_GET_SEND_C(routing),KX_GET_SEND_D(routing)); break;
   		 	   case 0x2002: routing=KX_MAKE_ROUTING(KX_GET_SEND_A(routing),KX_GET_SEND_B(routing),ret-1000,KX_GET_SEND_D(routing)); break;
   		 	   case 0x2003: routing=KX_MAKE_ROUTING(KX_GET_SEND_A(routing),KX_GET_SEND_B(routing),KX_GET_SEND_C(routing),ret-1000); break;

   		 	   case 0x2004: xrouting=KX_MAKE_XROUTING(ret-1000,KX_GET_SEND_B(xrouting),KX_GET_SEND_C(xrouting),KX_GET_SEND_D(xrouting)); break;
   		 	   case 0x2005: xrouting=KX_MAKE_XROUTING(KX_GET_SEND_A(xrouting),ret-1000,KX_GET_SEND_C(xrouting),KX_GET_SEND_D(xrouting)); break;
   		 	   case 0x2006: xrouting=KX_MAKE_XROUTING(KX_GET_SEND_A(xrouting),KX_GET_SEND_B(xrouting),ret-1000,KX_GET_SEND_D(xrouting)); break;
   		 	   case 0x2007: xrouting=KX_MAKE_XROUTING(KX_GET_SEND_A(xrouting),KX_GET_SEND_B(xrouting),KX_GET_SEND_C(xrouting),ret-1000); break;
   		 	  }
   		 	  ikx_t->set_routing(cur_item,routing,xrouting);
   		 	 }
   		 	 else
   		 	 {
   		 	  for(int j=0;j<16;j++)
   		 	  {
   		 	   ikx_t->get_routing(((cur_item==ROUTING_LAST+100)?DEF_SYNTH1_ROUTING:DEF_SYNTH2_ROUTING)+j,&routing,&xrouting);
                           switch(wParam)
                           {
                            case 0x2000: routing=KX_MAKE_ROUTING(ret-1000,KX_GET_SEND_B(routing),KX_GET_SEND_C(routing),KX_GET_SEND_D(routing)); break;
                            case 0x2001: routing=KX_MAKE_ROUTING(KX_GET_SEND_A(routing),ret-1000,KX_GET_SEND_C(routing),KX_GET_SEND_D(routing)); break;
                            case 0x2002: routing=KX_MAKE_ROUTING(KX_GET_SEND_A(routing),KX_GET_SEND_B(routing),ret-1000,KX_GET_SEND_D(routing)); break;
                            case 0x2003: routing=KX_MAKE_ROUTING(KX_GET_SEND_A(routing),KX_GET_SEND_B(routing),KX_GET_SEND_C(routing),ret-1000); break;

                            case 0x2004: xrouting=KX_MAKE_XROUTING(ret-1000,KX_GET_SEND_B(xrouting),KX_GET_SEND_C(xrouting),KX_GET_SEND_D(xrouting)); break;
                            case 0x2005: xrouting=KX_MAKE_XROUTING(KX_GET_SEND_A(xrouting),ret-1000,KX_GET_SEND_C(xrouting),KX_GET_SEND_D(xrouting)); break;
                            case 0x2006: xrouting=KX_MAKE_XROUTING(KX_GET_SEND_A(xrouting),KX_GET_SEND_B(xrouting),ret-1000,KX_GET_SEND_D(xrouting)); break;
                            case 0x2007: xrouting=KX_MAKE_XROUTING(KX_GET_SEND_A(xrouting),KX_GET_SEND_B(xrouting),KX_GET_SEND_C(xrouting),ret-1000); break;
                           }
                           if(((KX_GET_SEND_A(routing)==KX_GET_SEND_B(routing))&&(KX_GET_SEND_A(routing)!=0x3f)) ||
                              ((KX_GET_SEND_A(routing)==KX_GET_SEND_C(routing))&&(KX_GET_SEND_A(routing)!=0x3f)) ||
                              ((KX_GET_SEND_A(routing)==KX_GET_SEND_D(routing))&&(KX_GET_SEND_A(routing)!=0x3f)) ||
                              ((KX_GET_SEND_B(routing)==KX_GET_SEND_C(routing))&&(KX_GET_SEND_B(routing)!=0x3f)) ||
                              ((KX_GET_SEND_B(routing)==KX_GET_SEND_D(routing))&&(KX_GET_SEND_B(routing)!=0x3f)) ||
                              ((KX_GET_SEND_C(routing)==KX_GET_SEND_D(routing))&&(KX_GET_SEND_C(routing)!=0x3f)) ||
                              ((KX_GET_SEND_A(xrouting)==KX_GET_SEND_B(xrouting))&&(KX_GET_SEND_A(xrouting)!=0x3f)) ||
                              ((KX_GET_SEND_A(xrouting)==KX_GET_SEND_C(xrouting))&&(KX_GET_SEND_A(xrouting)!=0x3f)) ||
                              ((KX_GET_SEND_A(xrouting)==KX_GET_SEND_D(xrouting))&&(KX_GET_SEND_A(xrouting)!=0x3f)) ||
                              ((KX_GET_SEND_B(xrouting)==KX_GET_SEND_C(xrouting))&&(KX_GET_SEND_B(xrouting)!=0x3f)) ||
                              ((KX_GET_SEND_B(xrouting)==KX_GET_SEND_D(xrouting))&&(KX_GET_SEND_B(xrouting)!=0x3f)) ||
                              ((KX_GET_SEND_C(xrouting)==KX_GET_SEND_D(xrouting))&&(KX_GET_SEND_C(xrouting)!=0x3f)))
                               continue;

    		 	   ikx_t->set_routing(((cur_item==ROUTING_LAST+100)?DEF_SYNTH1_ROUTING:DEF_SYNTH2_ROUTING)+j,routing,xrouting);
   		 	  }
   		 	 }
   		 	  OnTreeNotify(NULL,NULL);
   		 	  return 1;
   		 	}
	}
	return CKXDialog::on_command(wParam,lParam);
}


void CRouterDlg::OnTreeNotify( NMHDR * pNotifyStruct, LRESULT * result )
{
 // remove prev. controls
 empty_controls();

 HTREEITEM sel=tree.GetSelectedItem();
 if(sel)
 {
  HTREEITEM parent_item=tree.GetParentItem(sel);
  if(parent_item)
  {
   kString parent,child;
   parent=tree.GetItemText(parent_item);
   child=tree.GetItemText(sel);

   // create controls here...
   dword routing,xrouting;
   int is_10k2=ikx_t->get_dsp();
   int special=0;

   cur_item=(int)tree.GetItemData(sel);
   if((cur_item!=ROUTING_LAST+100) && (cur_item!=ROUTING_LAST+101))
   {
    if(ikx_t->get_routing(cur_item,&routing,&xrouting)!=0)
     return;
   }  
   else 
   {
     // check all the 16 channels
     dword r,xr;
     ikx_t->get_routing((cur_item==ROUTING_LAST+100)?DEF_SYNTH1_ROUTING:DEF_SYNTH2_ROUTING
        ,&routing,&xrouting);
     for(int j=1;j<16;j++)
     {
       ikx_t->get_routing(((cur_item==ROUTING_LAST+100)?DEF_SYNTH1_ROUTING:DEF_SYNTH2_ROUTING)+j,&r,&xr);
       if(KX_GET_SEND_A(routing)!=KX_GET_SEND_A(r)) { special|=1; }
       if(KX_GET_SEND_B(routing)!=KX_GET_SEND_B(r)) { special|=2; }
       if(KX_GET_SEND_C(routing)!=KX_GET_SEND_C(r)) { special|=4; }
       if(KX_GET_SEND_D(routing)!=KX_GET_SEND_D(r)) { special|=8;  }
       if(KX_GET_SEND_E(xrouting)!=KX_GET_SEND_E(xr)) { special|=0x10; }
       if(KX_GET_SEND_F(xrouting)!=KX_GET_SEND_F(xr)) { special|=0x20; }
       if(KX_GET_SEND_G(xrouting)!=KX_GET_SEND_G(xr)) { special|=0x40; }
       if(KX_GET_SEND_H(xrouting)!=KX_GET_SEND_H(xr)) { special|=0x80; }
     }
   }

     char tmp_str[256];
     // re-instantiate device
     dword p[]=
     {
       0xb0b000,0xffff00,0xb0b000,0xffffff,
       0xb0b000,0xffff00,0xb0b000,0x660000
     };

     if(!mf.get_profile("router","fxbus",tmp_str,sizeof(tmp_str)))
     {
      sscanf(tmp_str,"%x %x %x %x %x %x %x %x",
       &p[0],&p[1],&p[2],&p[3],&p[4],&p[5],&p[6],&p[7]);
     }

     for(int i=0;i<8;i++)
     {
         if((i==4) && (!is_10k2))
            break;

         if( (cur_item==DEF_WAVE01_ROUTING) ||
             (cur_item==DEF_WAVE23_ROUTING) ||
             (cur_item==DEF_WAVE45_ROUTING) ||
             (cur_item==DEF_WAVE67_ROUTING) ||
             (cur_item==DEF_AC3PASSTHROUGH_ROUTING))
         {
           if(i>=2)
            break; // do not show send_c,d,e,f,g,h for stereo/multichannel Wave devices [3544]
         }

         if( (cur_item==DEF_AC3_LEFT_ROUTING) || 
             (cur_item==DEF_AC3_CENTER_ROUTING) || 
             (cur_item==DEF_AC3_RIGHT_ROUTING) || 
             (cur_item==DEF_AC3_SLEFT_ROUTING) || 
             (cur_item==DEF_AC3_SRIGHT_ROUTING) || 
             (cur_item==DEF_AC3_SUBWOOFER_ROUTING) || 
             (cur_item==DEF_AC3_SCENTER_ROUTING) || 
             (cur_item==DEF_3D_LEFT_ROUTING) || 
             (cur_item==DEF_3D_RIGHT_ROUTING) || 
             (cur_item==DEF_3D_TOP_ROUTING))
         {
           if(i>=1)
            break; // do not show send_ for 3-D sources
         }

         if ( cur_item>=DEF_ASIO_ROUTING && cur_item<(DEF_ASIO_ROUTING+MAX_ASIO_OUTPUTS))
         {
           if(i>=1)
            break; // do not show send_ for ASIO

           // modify routing [to hide from user 'advanced' stuff
           if(is_10k2)
            routing=(routing&0xffffff00)+((routing&0xff)-0x20)/2;
         }

     	 kRect r;
     	 kString name,tooltip;
     	 r.left=220+46*(i>3?(i-4):i);
     	 r.right=r.left+45;
     	 r.top=25+(i>3?140:0);
     	 r.bottom=r.top+18;

     	 if(!(special&(1<<i)))
     	 {
     	  if(i>3)
     	  {
     	    name.Format("%d",(xrouting>>((i-4)*8))&0xff);
     	  }
     	  else
     	  {
     	    name.Format("%d",(routing>>(i*8))&0xff);
     	  }
     	 }
     	 else
     	 {
     	    name.Format("?");
     	 }
     	 tooltip.Format("Send %c FXBus Routing",i+'A');

         fx_routings[i].create(name,r,this,0x2000+i,
          p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);

         add_tool(tooltip,&fx_routings[i],0x2000+i);

         fx_routings[i].ShowWindow(SW_SHOW);

         // ---
         int to_do=-1;
         byte amount;
         if((cur_item>=DEF_SYNTH1_ROUTING)&&(cur_item<DEF_SYNTH2_ROUTING+16)) // synth1 || synth2
         {
           to_do=DEF_SYNTH_AMOUNT_C;
         }

         if(to_do!=-1)
         {
          if(to_do>=0) // get 'amount'
          {
           if((i==0)||(i==1)) // no SendA & SendB for wave/synth
            continue;

           ikx_t->get_send_amount(to_do+i-2,&amount);
          }

          r.top=r.bottom+5;
          r.bottom=r.top+127;
          fx_amounts[i].set_bitmaps(mf.load_image(mf.get_profile("sliders.default","default_slider")),
           mf.load_image(mf.get_profile("sliders.default","slider_thumb")),
           mf.load_image(mf.get_profile("sliders.default","slider_thumb_d")),
           mf.load_image(mf.get_profile("sliders.default","slider_thumb_s")));
          tooltip.Format("Send %c Amount",i+'A');
          fx_amounts[i].create((char *)(LPCTSTR)tooltip,TBS_VERT | TBS_RIGHT | TBS_NOTICKS,
             	r,this,IDg_ASIO_FXAMOUNT+i,kMETHOD_TRANS);
          fx_amounts[i].SetWindowText((LPCTSTR)tooltip);
          fx_amounts[i].set_range(0,255);
          fx_amounts[i].set_pos(255-amount);
          fx_amounts[i].ShowWindow(SW_SHOW);
         }
     }
         return;
    }
   }
}

void CRouterDlg::empty_controls()
{
 for(int i=0;i<MAX_BUSES;i++)
 {
  if(fx_amounts[i].m_hWnd)
   fx_amounts[i].DestroyWindow();
  if(fx_routings[i].m_hWnd)
   fx_routings[i].DestroyWindow();
 }
 cur_item=-1;
}

void CRouterDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
 kSlider *w=(kSlider *)pScrollBar;

 kString name;
 w->GetWindowText(name);
 int fx=name[5];

 int to_do=-1;

 if((cur_item>=DEF_SYNTH1_ROUTING)&&(cur_item<DEF_SYNTH2_ROUTING+16)) // synth1 || synth2
 {
   to_do=DEF_SYNTH_AMOUNT_C;
 }

 if(to_do!=-1)
 {
  ikx_t->set_send_amount(to_do+fx-'A'-2,255-w->get_pos());
 }

 kDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}
