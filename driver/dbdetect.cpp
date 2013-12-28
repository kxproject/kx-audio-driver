// kX Driver
// Copyright (c) Eugene Gavrilov, 2001-2014.
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


#include "kx.h"

// LiveDrive (CT4860),
// LiveDrive II (CT4861 - w/optical), 
// LiveDrive IR (SB0010), 
// AudigyDrive (SB0012)
// sb0110, sb0110b - audigy Ex drive (+ interface card)
// sb250 - audigy2 drive
// sb290 - audigy2ex drive

// digital io db: ct4710, ct4770
// external digital io module: ct4800

// 'digital i/o card II' : db8

#define MAX_RESPONSE	9

static byte buffer1[MAX_RESPONSE]=
{ 0xf0,0x00,0x20,0x21,0x60,0x0,0x02,0x00,0xf7 };
static byte buffer2[MAX_RESPONSE]=
{ 0xf0,0x00,0x20,0x21,0x61,0x0,0x02,0x01,0xf7 };

static byte buffer3[MAX_RESPONSE]=
{ 0xf0,0x00,0x20,0x21,0x60,0x0,0x02,0x03,0xf7 };

#define MAX_REQUEST	11
static byte buffer4[MAX_REQUEST]=
{ 0xf0,0x00,0x20,0x21,0x61,0x0,0x00,0x00,0x7f,0x0,0xf7 };

KX_API(byte,kx_get_gp_inputs(kx_hw *hw))
{
 if(hw->is_edsp)
  return 0xff;

 dword ret;
 if(hw->is_10k2)
 {
 	ret=kx_readfn0(hw,HCFG_K2);
        ret=(ret>>8)&0xff;
 }
 else
 {
 	dword tmp=0;
 	ret=kx_readfn0(hw,HCFG_K1);
 	if(ret&HCFG_GPINPUT1_K1) tmp=2;
 	if(ret&HCFG_GPINPUT0_K1) tmp|=1;
 	ret=tmp;
 }
 return (byte)ret;
}

KX_API(void,kx_set_gp_outputs(kx_hw *hw,byte output))
{
 if(hw->is_edsp)
 {
    debug(DLIB,"!! kx_set_gp_outputs: should not be called for E-DSP!\n");
    return;
 }
 dword tmp;
 if(hw->is_10k2)
 {
 	output=output&0xff;

 	tmp=kx_readfn0(hw,HCFG_K2);
 	kx_writefn0(hw,HCFG_K2,(tmp&0xffffff00)|(output));
 }
 else
 {
 	output=output&0x7;

 	tmp=kx_readfn0(hw,HCFG_K1);
 	kx_writefn0(hw,HCFG_K1,(tmp&0xffffe3ff)|(output<<0xa));
 }
}

// declaring it inside detect_db() causes stack overflow in Win98se
static char name[256];

dword detect_db(kx_hw *hw)
{
 byte original_state;
 byte first_state,second_state;
 int had_response=0;
 int timeout;
 int resp_counter=0; int resp_counter2=0;
 byte resp_buffer[MAX_RESPONSE]; memset(resp_buffer,0,sizeof(resp_buffer));
 byte resp_buffer2[MAX_RESPONSE]; memset(resp_buffer2,0,sizeof(resp_buffer2));
 dword result=0;

 // disable irq and save the state
 dword old_inte=kx_readfn0(hw,INTE); // FIXME NOW: not the best choice
 dword old_hcfg_k1=kx_readfn0(hw,HCFG_K1);
 dword old_hcfg_k2=kx_readfn0(hw,HCFG_K2);

 kx_irq_disable(hw,INTE_MIDIRXENABLE);
 if(hw->is_10k2)
  kx_irq_disable(hw,INTE_K2_MIDIRXENABLE);

 byte tmp;
 for(int i=0;i<(hw->is_10k2?MAX_MPU_DEVICES:1);i++)
   while(kx_mpu_read_data(hw,&tmp,i)==0) ; // flush current queue

 original_state=kx_get_gp_inputs(hw);
 debug(DDB,"original inputs: %xh\n",original_state);

 kx_set_gp_outputs(hw,0);
 kx_set_gp_outputs(hw,0);

 hw->cb.usleep(200);
 
 // first, try to write buffer3 to the mpu device
 if(hw->is_a2)
 {
   debug(DDB,"checking for a2 ex drive...\n");
   byte mpu_ret;

   for(int j=0;j<MAX_REQUEST;j++)
   {
    kx_mpu_write_data(hw,buffer4[j],1);
    kx_wcwait(hw,25);
   }
   timeout=0x200;
   while(timeout--)
   {
    kx_wcwait(hw,25);
    if(kx_mpu_read_data(hw,&mpu_ret,1)==0)
    {
     resp_buffer2[resp_counter2]=mpu_ret;
     resp_counter2++;
     timeout=0x200;
    }
    if(resp_buffer2[resp_counter2]==0xf7)
     break;
    if((size_t)resp_counter2>=sizeof(resp_buffer2)) // all was read
     break;
   }

   if(resp_counter2)
   {
    debug(DDB,"dbdetect: got a2ex response [%d]:\n",resp_counter2);

    for(int j=0;j<resp_counter2;j++)
     debug(DDB,"%x ",resp_buffer2[j]);

    debug(DDB,"\n");

    if(memcmp(resp_buffer2,buffer3,resp_counter2)==0)
    {
     result|=0x100;
     debug(DDB,"dbdetect: response correct; A2Ex drive found\n");
     goto END;
    } 
    debug(DDB,"dbdetect: response incorrect\n");
   } // else - no response

   for(int k=0;k<(hw->is_10k2?MAX_MPU_DEVICES:1);k++)
     while(kx_mpu_read_data(hw,&tmp,k)==0) ; // flush current queue
 }

 // 43b9a
 kx_set_gp_outputs(hw,0);
 hw->cb.usleep(200);
 first_state=kx_get_gp_inputs(hw);
 kx_set_gp_outputs(hw,1);
 hw->cb.usleep(200);
 second_state=kx_get_gp_inputs(hw);
 kx_set_gp_outputs(hw,0);
 debug(DDB,"out(0); w; out(1); w; out(0) in=%xh;%xh (first&second state)\n",first_state,second_state);

 timeout=0x200;
 resp_counter=0;
 resp_counter2=0;
 while(timeout--)
 {
 	byte mpu_ret;
 	if(kx_mpu_read_data(hw,&mpu_ret,0)==0)
 	{
 		resp_buffer[resp_counter]=mpu_ret;
 		resp_counter++;
 		timeout=0x200;
 	}
 	if(hw->is_10k2)
 	{
         	if(kx_mpu_read_data(hw,&mpu_ret,1)==0)
         	{
         		resp_buffer2[resp_counter2]=mpu_ret;
         		resp_counter2++;
         		timeout=0x200;
         	}
        }
        if(resp_buffer[resp_counter]==0xf7)
         break;
        if(resp_buffer2[resp_counter2]==0xf7)
         break;
 	if((size_t)resp_counter>=sizeof(resp_buffer)) // all was read
 	 break;
 	if((size_t)resp_counter2>=sizeof(resp_buffer2)) // all was read
 	 break;
 }
 if((timeout<=0) && (resp_counter==0) && (resp_counter2==0))
  goto NO_RESPONSE;

 debug(DDB,"there _was_ midi response [%d/%d bytes]; timeout=%d\n resp1:",resp_counter,resp_counter2,timeout);

 for(int k=0;k<resp_counter;k++)
   debug(DDB,"%x ",resp_buffer[k]);
 debug(DDB,"\n resp2:");
 
 for(int l=0;l<resp_counter2;l++)
   debug(DDB,"%x ",resp_buffer2[l]);
 debug(DDB,"\n");

 if(memcmp(resp_buffer,buffer1,8)==0) // same
 {
        result=0x20;
        had_response=1;
 }
 if(memcmp(resp_buffer,buffer2,8)==0)
 {
        result=0x80;
        had_response=1;
 }
 if(memcmp(resp_buffer2,buffer1,8)==0) // same
 {
        result=0x20;
        had_response=1;
 }
 if(memcmp(resp_buffer2,buffer2,8)==0)
 {
        result=0x80;
        had_response=1;
 }
 if(had_response)
 {
 	// 43e25
 	debug(DDB,"db_detect: response recognized [%x]\n",result);
 	kx_set_gp_outputs(hw,2);
 	hw->cb.usleep(200);
 	first_state=kx_get_gp_inputs(hw);
 	kx_set_gp_outputs(hw,0);
 	hw->cb.usleep(200);
 	second_state=kx_get_gp_inputs(hw);
 	kx_set_gp_outputs(hw,2);
 	hw->cb.usleep(200);

        // 43e88
 	if((!(first_state&2)) && (second_state&2)) // bit 2 changed 0->1
 	{
 		// 44170
 		result|=0x40;
 		goto END;
 	}
        // 43e98
 	kx_set_gp_outputs(hw,0);
 	hw->cb.usleep(200);
 	first_state=kx_get_gp_inputs(hw);
 	kx_set_gp_outputs(hw,2);
 	hw->cb.usleep(200);
 	second_state=kx_get_gp_inputs(hw); // actually, tmp ret

 	// 43ed2
 	if(!(first_state&2))
 	{
 	 if(second_state&2) // bit 2 changed: 0->1
 	 {
 	  if(original_state&4)
 	  {
 	   result|=8;
 	  }
 	  else
 	  {
 	   result|=4;
 	  }
 	  goto END;
 	 }
 	}
        // 43efd
 	if(original_state&4)
 	 result|=0x10;
 	goto END;
 } else debug(DDB,"response invalid?\n");

NO_RESPONSE:
	// 43f13
	if(!(first_state&2) && (second_state&2)) // bit 2 changed 0->1
	{
		debug(DDB,"first&2 was 0 and second&2 was 1 @%x\n",__LINE__);
		result=2;
		kx_set_gp_outputs(hw,0);
		hw->cb.usleep(200);            
		first_state=kx_get_gp_inputs(hw);
		kx_set_gp_outputs(hw,2);
		hw->cb.usleep(200);
		debug(DDB,"performing: out(0); w; out(2); w;...\n");

                // 43f65: jump to -> 43fd5

                // 43fd5
                second_state=kx_get_gp_inputs(hw);
		if((first_state&2) || (!(second_state&2))) // bit 2 changed: 1->0
		{
		   debug(DDB,"first&2 was 0 and second&2 was 1 @%x\n",__LINE__);
		   debug(DDB,"Bit Changed (%x)\n",__LINE__);
                   // 43efd:
		   if(original_state&4)
		    result|=0x10;
		   goto END;
		}
		
		if(!(original_state&4))
			result|=4;
		else
			result|=8;
		goto END;
        }
        // 43f73
        if(first_state==0)
        {
        	// 4418f
        	debug(DDB,"first_state was 0 (%x); try out(2);\n",first_state);

        	kx_set_gp_outputs(hw,2);
        	hw->cb.usleep(200);

        	if(!(kx_get_gp_inputs(hw)&2))
        	{
        	 // 441df:
        	 debug(DDB,"bit 2 stayed 0 (%x)\n",__LINE__);
        	 if(original_state&1)
        	  result=0x10;
        	 else
        	  debug(DDB,"but ori state wasn't &1\n");
        	}
        	else // bit 2 input set bit 2 output
        	{
        	 debug(DDB,"ins: bit 2 set\n");
        	 if(!(original_state&3))
        	  result=4; // 441c4:
        	 else // 441cd:
        	  if(original_state&1)
        	   result=8;
        	}
		goto END;
        }

        // 43f75
        if(!(second_state&2))
        {
        	// 43f85: !(second_state&2)
        	debug(DDB,"second_state: %x(&2)=0; res=1; try: out(1);(3);\n",second_state);
        	result=1;
        	kx_set_gp_outputs(hw,1);
        	hw->cb.usleep(200);
        	first_state=kx_get_gp_inputs(hw);
        	kx_set_gp_outputs(hw,3);
        	hw->cb.usleep(200);
        	second_state=kx_get_gp_inputs(hw);

                // 43fdb
        	if((first_state&2) || (!(second_state&2))) // bit 2 changed: 1->0
        	{
        		// 43efd:
        		debug(DDB,"was 2 became 0 (%x)\n",__LINE__);
        		if(original_state&4)
        		 result|=0x10;
        		goto END;
        	}
        	if(!(original_state&4))
        	  result|=4;
        	else
        	  result|=8;
		goto END;
        }

        // 44005 
        debug(DDB,"got here?; try: out(2); out(3); out(2);\n");

      	kx_set_gp_outputs(hw,2);
      	hw->cb.usleep(200);
      	first_state=kx_get_gp_inputs(hw);
      	kx_set_gp_outputs(hw,3);
      	hw->cb.usleep(200);
      	second_state=kx_get_gp_inputs(hw);
      	kx_set_gp_outputs(hw,2);
      	hw->cb.usleep(200);

        // 44063:
      	if((!(first_state&2)) && (second_state&2))
      	{
      		debug(DDB,"bit changed (&2 was 0; became 2) (%x); try out(2);(0);(2);\n",__LINE__);
      		kx_set_gp_outputs(hw,2);
      		hw->cb.usleep(200);
      		first_state=kx_get_gp_inputs(hw);
      		kx_set_gp_outputs(hw,0);
      		hw->cb.usleep(200);
      		second_state=kx_get_gp_inputs(hw);
      		kx_set_gp_outputs(hw,2);
      		hw->cb.usleep(200);

                // jmp 44160

                // 44160:
      		if((!(first_state&2)) && (second_state&2))
      		{
      		 debug(DDB,"bit changed again (&2 was 0; became 2) (%x)\n",__LINE__);
      		 result|=0x40;
      		}
		goto END;
        }

        if(first_state!=0) // 440e7
        {
        	// 440ef:
        	debug(DDB,"first_state was not 0 (%x)\n",__LINE__);
              	if(!(second_state&2))
              	{
              	 debug(DDB,"try out(3)(1)(3) [%x]\n",__LINE__);
              	 // 440f9
              	 result=1;
              	 kx_set_gp_outputs(hw,3);
              	 hw->cb.usleep(200);
              	 first_state=kx_get_gp_inputs(hw);
              	 kx_set_gp_outputs(hw,1);
              	 hw->cb.usleep(200);
              	 second_state=kx_get_gp_inputs(hw);
              	 kx_set_gp_outputs(hw,3);
              	 hw->cb.usleep(200);

                 // 44160:
      		 if((!(first_state&2)) && (second_state&2))
      		 {
                         result|=0x40;
      		 } else  debug(DDB,"Bit NOT changed (%x)\n",__LINE__);

      		 goto END;

      		} else  debug(DDB,"Bit NOT changed (second state) (%x)\n",__LINE__);

                // 44179
      		if(first_state!=0) // nonsense :)
      		 goto END;
      	}
        // 4417d
      	if(!(second_state&2))
      	 result|=0x40;
      	goto END;
END:
 // 441ef
 kx_set_gp_outputs(hw,0);

 name[0]=0;
 if(result==0)
  strncpy(name,"none ",KX_MAX_STRING);
 else
 {
     if(hw->is_aps)
     {
      if(result&1)
       strncat(name,"EDrive ",KX_MAX_STRING-strlen(name)-1);
      if(result&2)
       strncat(name,"MOC ",KX_MAX_STRING-strlen(name)-1);
     }
     else
     {
      if(result&1)
       strncat(name,"LiveBay1 ",KX_MAX_STRING-strlen(name)-1);
      if(result&2)
       strncat(name,"LiveBay0 ",KX_MAX_STRING-strlen(name)-1);
     }

      if(result&0x100)
       strncat(name,"Aud2ELiveBay ",KX_MAX_STRING-strlen(name)-1);
      if(result&0x80)
       strncat(name,"ELiveBay0 ",KX_MAX_STRING-strlen(name)-1);
      if(result&0x20)   
       strncat(name,"LiveBay2 ",KX_MAX_STRING-strlen(name)-1);
      if(result&4)
       strncat(name,"db8 ",KX_MAX_STRING-strlen(name)-1);
      if(result&8)
       strncat(name,"db2 ",KX_MAX_STRING-strlen(name)-1);
      if(result&0x10)
       strncat(name,"db1 ",KX_MAX_STRING-strlen(name)-1);
      if(result&0x40)
       strncat(name,"db10 ",KX_MAX_STRING-strlen(name)-1);
 }

 name[strlen(name)-1]=0;

 debug(DLIB,"db type: %s [%x]\n",name,result);
 strncpy(hw->db_name,name,KX_MAX_STRING);

 kx_writefn0(hw,INTE,old_inte);
 kx_writefn0(hw,HCFG_K1,old_hcfg_k1);
 kx_writefn0(hw,HCFG_K2,old_hcfg_k2);

 // the following enables remoteIR on LiveDrive IR
 if((!hw->is_aps) && (!hw->is_10k2))
 {
  // FIXME NOW: do we really need this?..
  kx_writefn0(hw, HCFG_K1, kx_readfn0(hw,HCFG_K1) | HCFG_GPOUT0_K1);
  kx_wcwait(hw,48000/4);
  kx_writefn0(hw, HCFG_K1, kx_readfn0(hw,HCFG_K1) | HCFG_GPOUT1_K1);
 }

 return result;
}
