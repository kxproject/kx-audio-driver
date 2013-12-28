// kX Spy
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


#include "stdafx.h"

#define kbhit _kbhit
#define getch _getch

void gotoxy(int x,int y)
{
	COORD pos;
	pos.X=x-1;
	pos.Y=y-1;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos);
}

void clrscr(void)
{
	DWORD p;
	COORD pos;
	pos.X=0;
	pos.Y=0;
	FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE),
	 ' ',100*35,pos,&p);
}

void malloc_func(void *this_,int size, void **b,int )
{
 char *ptr=NULL;
 ptr = new char[size];

 (*b)=ptr;
}

void free_func(void *this_,void *b)
{
 if(b)
  delete b;
}

void usleep_func(int ms)
{
 for(int i=0;i<200;i++)
 {
  for(int j=0;j<ms;j++)
  {
   __asm
   {
    nop
   }
  }
 }
}

void timer_func(void *data)
{
}
void save_fpu_state(kx_fpu_state *)
{
}
void rest_fpu_state(kx_fpu_state *)
{
}

void get_physical(void *call_with,kx_voice_buffer *b,int,__int64 *a)
{
 *a=0;
}

int debug_func(int where,const char *__format, ... )
{
 static char my_internal_buf[4096];
 va_list ap;
 va_start(ap, __format);
 
 if(_vsnprintf(my_internal_buf, 4096, __format, ap))
 {
  if(where==DDB)
    printf("%s",my_internal_buf);
 }
 va_end(ap);
 return 0;
}

int pci_alloc(void *this_,struct memhandle *m,kx_cpu_cache_type_t cache_type)
{
 m->addr=new char[m->size];
 if(m->addr)
 {
  m->dma_handle=(dword)m->addr;
  return 0;
 } 
  else 
 {
  m->addr=0;
  m->dma_handle=0;
  m->size=0;
 }
 return -10;
}

void pci_free(void *this_,struct memhandle *m)
{
 if(m->addr)
 {
   delete m->addr;
   m->addr=0;
   m->dma_handle=0;
   m->size=0;
 }
}

#undef kx_lock_acquire
KX_API(void,kx_lock_acquire(struct kx_hw*,spinlock_t *l, unsigned long *irq,const char *,int))
{
// KeAcquireSpinLock((PKSPIN_LOCK)l,(PKIRQL)irq);
}

#undef kx_lock_release
KX_API(void,kx_lock_release(struct kx_hw*,spinlock_t *l, unsigned long *irq,const char *,int))
{
// KeAcquireSpinLock((PKSPIN_LOCK)l,(PKIRQL)irq);
}

KX_API(void,kx_spin_lock_init(struct kx_hw*,spinlock_t *l,const char *name))
{
// KeInitializeSpinLock((PKSPIN_LOCK)l);
}

#pragma warning(disable:4035)
static dword inpd(word __port)
{
 __asm {
  mov dx,__port
  in eax,dx
  };
}

dword dsp_data[0xf00];

void clean_dsp(kx_hw *hw)
{
 int total_mem;
 total_mem=0x400;

 for(int i=0;i<total_mem;i++)
 {
  kx_writeptr(hw,E10K2_MICROCODE_BASE+i,0,0x0);
 }
}

void save_dsp(kx_hw *hw)
{
 int total_mem;
 if(hw->is_10k2)
  total_mem=0xf00;
 else
  total_mem=0x800;
 for(int i=0;i<total_mem;i++)
  dsp_data[i]=kx_readptr(hw,0x100+i,0);
}

void rest_dsp(kx_hw *hw)
{
 int total_mem;
 if(hw->is_10k2)
  total_mem=0xf00;
 else
  total_mem=0x800;
 for(int i=0;i<total_mem;i++)
 {
  kx_writeptr(hw,0x100+i,0,dsp_data[i]);
 }
}


int __cdecl main(int argc,char **argv)
{
  HANDLE h;

  OSVERSIONINFO ver;

  ver.dwOSVersionInfoSize=sizeof(ver);
  GetVersionEx(&ver);
  if(ver.dwPlatformId & VER_PLATFORM_WIN32_NT)
  {
         h = CreateFile("\\\\.\\giveio", GENERIC_READ, 0, NULL,
       					OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
         if(h == INVALID_HANDLE_VALUE) 
         {
               printf("Couldn't access giveio device\n");
               return -1;
         }
         CloseHandle(h);
  }
  else // not NT
  {
  	printf("Win version is 95/98/Me: needs drivers patched\n");
  }

#define DEFAULT_TANKMEM_SIZE	(512*1024)
#define DEFAULT_STD_DMA_BUFFER		(4*2088) // from windrvrs 2088 (0x828) samples

// rec
#define DEFAULT_STD_REC_BUFFER		(16384)

#define DEFAULT_AC3_BUFFERS		8


  kx_hw *hw;
  kx_callbacks cb;

  cb.call_with=NULL;
  cb.io_base=0;

  cb.debug_func=&debug_func;
  cb.malloc_func=&malloc_func;
  cb.free_func=&free_func;
  cb.pci_alloc=&pci_alloc;
  cb.pci_free=&pci_free;
  cb.get_physical=&get_physical;
  cb.save_fpu_state=&save_fpu_state;
  cb.rest_fpu_state=&rest_fpu_state;
  cb.usleep=&usleep_func;

  cb.def_routings[DEF_WAVE01_ROUTING]=KX_MAKE_ROUTING(FXBUS0,FXBUS1,FXBUSD,FXBUSE);
  cb.def_xroutings[DEF_WAVE01_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);

  cb.def_routings[DEF_WAVE23_ROUTING]=KX_MAKE_ROUTING(FXBUS4,FXBUS5,FXBUSD,FXBUSE);
  cb.def_xroutings[DEF_WAVE23_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
  cb.def_routings[DEF_WAVE45_ROUTING]=KX_MAKE_ROUTING(FXBUS8,FXBUS9,FXBUSD,FXBUSE);
  cb.def_xroutings[DEF_WAVE45_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
  cb.def_routings[DEF_WAVE67_ROUTING]=KX_MAKE_ROUTING(FXBUS6,FXBUS7,FXBUSD,FXBUSE);
  cb.def_xroutings[DEF_WAVE67_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);

  cb.def_routings[DEF_AC3PASSTHROUGH_ROUTING]=KX_MAKE_ROUTING(0x1e,0x1f,0x3f,0x3f);
  cb.def_xroutings[DEF_AC3PASSTHROUGH_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);

  for(int i=0;i<16;i++)
  {
  	cb.def_routings[DEF_SYNTH1_ROUTING+i]=KX_MAKE_ROUTING(FXBUS2,FXBUS3,FXBUSD,FXBUSE);
        cb.def_xroutings[DEF_SYNTH1_ROUTING+i]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
  	cb.def_routings[DEF_SYNTH2_ROUTING+i]=KX_MAKE_ROUTING(FXBUS2,FXBUS3,FXBUSD,FXBUSE);
        cb.def_xroutings[DEF_SYNTH2_ROUTING+i]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
  }
  cb.def_routings[DEF_AC3_LEFT_ROUTING]=KX_MAKE_ROUTING(FXBUS4,FXBUS0,FXBUSD,FXBUSE);
  cb.def_xroutings[DEF_AC3_LEFT_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
  cb.def_routings[DEF_AC3_RIGHT_ROUTING]=KX_MAKE_ROUTING(FXBUS5,FXBUS0,FXBUSD,FXBUSE);
  cb.def_xroutings[DEF_AC3_RIGHT_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
  cb.def_routings[DEF_AC3_SLEFT_ROUTING]=KX_MAKE_ROUTING(FXBUS6,FXBUS0,FXBUSD,FXBUSE);
  cb.def_xroutings[DEF_AC3_SLEFT_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
  cb.def_routings[DEF_AC3_SRIGHT_ROUTING]=KX_MAKE_ROUTING(FXBUS7,FXBUS0,FXBUSD,FXBUSE);
  cb.def_xroutings[DEF_AC3_SRIGHT_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
  cb.def_routings[DEF_AC3_CENTER_ROUTING]=KX_MAKE_ROUTING(FXBUS8,FXBUS0,FXBUSD,FXBUSE);
  cb.def_xroutings[DEF_AC3_CENTER_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
  cb.def_routings[DEF_AC3_SUBWOOFER_ROUTING]=KX_MAKE_ROUTING(FXBUS9,FXBUS0,FXBUSD,FXBUSE);
  cb.def_xroutings[DEF_AC3_SUBWOOFER_ROUTING]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);

  for(int i=0;i<MAX_ASIO_OUTPUTS;i++)
  {
   cb.def_routings[DEF_ASIO_ROUTING+i]=KX_MAKE_ROUTING(i,i==0?FXBUSF:FXBUS0,i==0xd?FXBUSF:FXBUSD,i==0xe?FXBUSF:FXBUSE);
   cb.def_xroutings[DEF_ASIO_ROUTING+i]=KX_MAKE_ROUTING(0x3f,0x3f,0x3f,0x3f);
  }

  cb.def_amount[DEF_SYNTH_AMOUNT_C]=0x60;
  cb.def_amount[DEF_SYNTH_AMOUNT_D]=0x10;
  cb.def_amount[DEF_SYNTH_AMOUNT_E]=0x0;
  cb.def_amount[DEF_SYNTH_AMOUNT_F]=0x0;
  cb.def_amount[DEF_SYNTH_AMOUNT_G]=0x0;
  cb.def_amount[DEF_SYNTH_AMOUNT_H]=0x0;

  cb.pb_buffer=DEFAULT_STD_DMA_BUFFER;
  cb.rec_buffer=DEFAULT_STD_REC_BUFFER;
  cb.ac3_buffers=DEFAULT_AC3_BUFFERS;

  cb.tram_size=DEFAULT_TANKMEM_SIZE;

  int device=1;
  if(argv[1])
  {
   if(argv[1][0]=='$')
   {
    sscanf(&argv[1][1],"%d",&device);
    device++;
    argv++;
   }
  }

  int res=kx_init(&hw,&cb,device); // standalone

	// per-channel dumps
	dword reg=DCYSUSV;

	if(argv[1])
	 sscanf(argv[1],"%x",&reg);

	if(res)
	{
		switch(res)
		{
		case 1:
			printf("System init failed\n");
			break;
		case 2:
			printf("PCI init failed: no 10kx device\n");
			break;
		case 10:
			printf("Error allocating memory\n");
			break;
		case -1:
			printf("Incorrect DLL version\n");
			break;
		case -2:
			printf("Memory error\n");
			break;
		default:
			printf("Unknown error\n");
			break;
		}
		return -1;
	}

// hw->have_ac97=1; // should be set because kx_init()'s ac97 detection is bypassed
// kx_hal_init(hw,1);
//#include "code/state.h"


#if 0
	kx_dsp_stop(hw);
    printf("Microcode: size: %d,first: %x\n",hw->microcode_size,hw->first_instruction);
	#include "code/microcode.h"
	kx_dsp_go(hw);
#endif

#if 0
	while(1)
	{
	 printf(">");
	 char line[128];
	 gets(line);

	 char cmd[32];
	 int p1,p2,p3;

	 sscanf(line,"%s %x %x %x",cmd,&p1,&p2,&p3);
	 if(strstr(cmd,"gfn0")!=0)
	 {
	  printf("Fn0: [reg: %x]: %x\n",p1,kx_readfn0(hw,p1));
	 }
	  else
	   if(strstr(cmd,"sfn0")!=0)
	   {
	    printf("Set Fn0: [reg: %x] -> %x\n",p1,p2);
	    kx_writefn0(hw,p1,p2);
	   }
	   else
	    if(strstr(cmd,"gptr")!=0)
	    {
	     printf("Ptr: reg: %x chn: %x: %x\n",p1,p2,kx_readptr(hw,p1,p2));
	    }
	    else
	     if(strstr(cmd,"sptr")!=0)
	     {
	      printf("Set ptr: [reg: %x; chn: %x] -> %x\n",p1,p2,p3);
	      kx_writeptr(hw,p1,p2,p3);
	     }
	     else
	      if(strstr(cmd,"quit")!=0)
	       break;
	      else
	       printf("Invalid command. gfn0/sfn0; gptr/sptr [reg/chn/val]\n");
	}
#endif

#if 0
// generic register lists with kbhit

	clrscr();

	gotoxy(60,35);
	 printf("Card: '%s'  ",hw->card_name);

#if 1
// register dumps for a channel
	while(!kbhit())
	{
 	 for(int i=0;i<64;i++)
	 {
	 	int pos;
	 	if(i>=48)
	 	 pos=48;
	 	else
	 	 if(i>=32)
	 	  pos=32;
	 	 else
	 	  if(i>=16)
	 	   pos=16;
	 	  else
	 	   pos=1;
	 	gotoxy(pos,i%16+1);
                dword data=0;
                if(i<16)
                {
                 data=kx_readptr(hw,reg,i);
	 	 printf("(%02d):%08x",i,data);
	 	}
	 	else
	 	 if(i<32)
	 	{
                 data=kx_readptr(hw,reg,i);
	 	 printf("(%02d):%08x",i,data);
	 	}
	 	 else
	 	 if(i<48)
	 	{
                 data=kx_readptr(hw,reg,i);
	 	 printf("(%02d):%08x",i,data);
	 	}
	 }

         // 10k2 regs
         gotoxy(30,25);
         printf("hcfg1: %x hcfg2: %x EHC: %x  INTE: %x Timer: %x    ",kx_readfn0(hw,HCFG_K1),kx_readfn0(hw,HCFG_K2),
           kx_readptr(hw,EHC,0),kx_readfn0(hw,INTE),kx_readfn0(hw,TIMER));

         // dump spdif statuses:

	// ac97 / spdif /rec
	gotoxy(30,26);
	printf("ac97:%x  spdif_rec[SRi]:%08x/%08x/%02x  ADC/Mic/FX:%02x/%02x/%02x  ",
	 kx_readfn0(hw,AC97DATA),
	 kx_readptr(hw,SPRI,0),
	 kx_readptr(hw,SPRA,0),
	 kx_readptr(hw,SPRC,0),
	 kx_readptr(hw,ADCBS,0),
         kx_readptr(hw,MBS,0),
         kx_readptr(hw,FXBS,0));
        gotoxy(30,27);
        printf("dbg:%08x bypass:%4x slot:%02x",
         kx_readptr(hw,DBG_10K2,0),
         kx_readptr(hw,SOC,0),
         kx_readptr(hw,PCB,0)
	 );

     Sleep(1000);

	}
#endif

#if 0
// register dumps for [fn0]
	while(!kbhit())
	{               
 	 for(int i=0;i<=0x3f;i++)
	 {
	 	int pos;
	 	if(i>=128+32+32)
	 	 pos=64+16+16;
	 	else
	 	if(i>=128+32)
	 	 pos=64+16;
	 	else
	 	if(i>=128)
	 	 pos=64;
	 	else
	 	if(i>=96)
	 	 pos=48;
	 	else
	 	 if(i>=64)
	 	  pos=32;
	 	 else
	 	  if(i>=32)
	 	   pos=16;
	 	  else
	 	   pos=1;
	 	gotoxy(pos,i%32+1);
                dword data=0;

                data=kx_readfn0(hw,i);
	 	printf("(%02x):%08x",i,data);
	 }
	}
#endif

#if 0
// register dumps for a channel [ptr]
	while(!kbhit())
	{               
 	 for(int i=0;i<=0x7f;i++)
	 {
	 	int pos;
	 	if(i>=128+32+32)
	 	 pos=64+16+16;
	 	else
	 	if(i>=128+32)
	 	 pos=64+16;
	 	else
	 	if(i>=128)
	 	 pos=64;
	 	else
	 	if(i>=96)
	 	 pos=48;
	 	else
	 	 if(i>=64)
	 	  pos=32;
	 	 else
	 	  if(i>=32)
	 	   pos=16;
	 	  else
	 	   pos=1;
	 	gotoxy(pos,i%32+1);
                dword data=0;

                data=kx_readptr(hw,i,0);
	 	printf("(%02x):%08x",i,data);
	 }
	}
#endif

#if 0
	int quit=0;
        int filter=0;
        int subf=0;

	while(!quit)
	{
	 if(kbhit())
	 {
	  int ch=_getch();
	  if(ch==27) { break; };
	  if(ch==32)
	  { kx_dsp_stop(hw); save_dsp(hw); clean_dsp(hw); kx_dsp_go(hw); }
	  if((ch==13)||(ch==10))
	  { kx_dsp_stop(hw); rest_dsp(hw); kx_dsp_go(hw); }
	  if(ch=='s')
	  { kx_writeptr(hw,SCS0,0,0x2108506); };
	  if(ch=='f')
	  { filter=1; subf=1-subf; }
	 }

#if 0
// vtft-related
 	 for(int i=0;i<64;i++)
	 {
	 	int pos;
	 	if(i>=48)
	 	 pos=48;
	 	else
	 	 if(i>=32)
	 	  pos=32;
	 	 else
	 	  if(i>=16)
	 	   pos=16;
	 	  else
	 	   pos=1;
	 	gotoxy(pos,i%16+1);
                dword data=0;
                data=kx_readptr(hw,reg,i);
	 	printf("(%02d)%08x %s",i,data,kx_readptr(hw,VTFT,i)&0xffff0000?"Y":" ");

	 	if(filter)
	 	{
	 	 if(subf)
	 	 {
	 	  kx_writeptr(hw,FXRT1_K2,i,kx_readptr(hw,FXRT1_K2,i)|0x80808080);
	 	  if(kx_readptr(hw,PTAB_FXSENDAMOUNT_A,i)!=0)
	 	   kx_writeptr(hw,PTAB_FXSENDAMOUNT_A,i,0xff);
		  if(kx_readptr(hw,PTAB_FXSENDAMOUNT_B,i)!=0)
	 	   kx_writeptr(hw,PTAB_FXSENDAMOUNT_B,i,0xff);
	 	 }
	 	 else
	 	 {
	 	  kx_writeptr(hw,FXRT1_K2,i,kx_readptr(hw,FXRT1_K2,i)&(~0x80808080));
	 	  if(kx_readptr(hw,PTAB_FXSENDAMOUNT_A,i)!=0)
	 	   kx_writeptr(hw,PTAB_FXSENDAMOUNT_A,i,0xc0);
	 	  if(kx_readptr(hw,PTAB_FXSENDAMOUNT_B,i)!=0)
	 	   kx_writeptr(hw,PTAB_FXSENDAMOUNT_B,i,0xc0);
	 	 }
	 	 filter++;
	 	 if(filter==65) filter=0;
	 	}
	 }
#endif
#if 0
// hw registers
	gotoxy(1,32);

	printf("[io3]: %04x/%04x/%04x inte: %04x\n",
	  kx_readfn0(hw,HCFG_K1),
	  kx_readfn0(hw,HCFG_K2),
	  kx_readptr(hw,EHC,0),
	  kx_readfn0(hw,INTE));
#endif
	}
#endif

#if 0
// p16v
	while(!kbhit())
	{
	 dword reg;
	 for(int i=0;i<0x80;i+=6)
	 {
	  for(int j=0;j<6;j++)
	  {
	   gotoxy(1+(j*16),i/6+1);
	   reg=kx_readp16v(hw,i+j,0);
	   printf("(%02x): %08x",(i+j),reg);
	  }
	 }

	 for(int i=0x20;i<0x40;i+=4)
	 {
	  reg=kx_readfn0(hw,i);
	  gotoxy(1,i/4-0x20/4+25);
	  printf("(%02x):%08x ",i,reg);
	 }

         // 10k2 regs
         gotoxy(30,25);
         printf("hcfg1: %x hcfg2: %x EHC: %x  INTE: %x  ",kx_readfn0(hw,HCFG_K1),kx_readfn0(hw,HCFG_K2),
           kx_readptr(hw,EHC,0),kx_readfn0(hw,INTE));

         // dump spdif statuses:

	// ac97 / spdif /rec
	gotoxy(30,26);
	printf("ac97:%x  spdif_rec[SRi]:%08x/%08x/%02x  ADC/Mic/FX:%02x/%02x/%02x  ",
	 kx_readfn0(hw,AC97DATA),
	 kx_readptr(hw,SPRI,0),
	 kx_readptr(hw,SPRA,0),
	 kx_readptr(hw,SPRC,0),
	 kx_readptr(hw,ADCBS,0),
         kx_readptr(hw,MBS,0),
         kx_readptr(hw,FXBS,0));
        gotoxy(30,27);
        printf("dbg:%08x bypass:%4x slot:%02x",
         kx_readptr(hw,DBG_10K2,0),
         kx_readptr(hw,SOC,0),
         kx_readptr(hw,PCB,0)
	 );

	 gotoxy(30,28);
	 printf("spssx: %08x[%08x] %08x[%08x] %08x[%08x]    ",
	  kx_readptr(hw,SCS0,0),kx_readptr(hw,SCS0,1),
	  kx_readptr(hw,SCS1,0),kx_readptr(hw,SCS1,1),
	  kx_readptr(hw,SCS2,0),kx_readptr(hw,SCS2,1));
	 gotoxy(30,29);
	 printf("i2s status: %08x %08x %08x ",
	  kx_readptr(hw,SRT3,0),
	  kx_readptr(hw,SRT4,0),
	  kx_readptr(hw,SRT5,0)
	  );

	 gotoxy(30,30);
	 printf("cdcs: %08x gpscs: %08x ",
	  kx_readptr(hw,CDCS,0),
	  kx_readptr(hw,GPSCS,0));

	 gotoxy(30,31);
	 printf("cd/gp/zv/srcs: %08x %08x %08x ",
	  kx_readptr(hw,CDSRCS,0),
	  kx_readptr(hw,GPSRCS,0),
	  kx_readptr(hw,ZVSRCS,0)
	  );

	 gotoxy(30,32);
	 printf("p16v 0x17: %x   ",kx_readp16v(hw,p16vRecRate,0));

     Sleep(100);

	}
#endif


	// logging
	FILE *fo=0;
//	fo=fopen("c:\\log.txt","wt");
	int prev_a=-1,prev_b=-1;

#if 0
	dword min_delay=0;
	int quit=0;

	while(!quit)
	{
	 int pos=0;
	 for(i=0;i<64;i++)
	 {
	  if(!(kx_readptr(hw,VTFT,i)&0xffff0000))
	  {
	   if(i==8) prev_a=-1;
	   continue;
	  }
#if 0
// all regs:
	  for(int j=0x0;j<=0x1d;j++)
	  {
	 	gotoxy(pos*14+1,1+j);
                dword data=0;
                data=kx_readptr(hw,j,i);
/*                if(j==0x10) // VEV
                {
                 if((min_delay==0)&&(data!=0)&&(data!=0xfe00))
                 { min_delay=data; }
                 else
                 { if(data!=0) { if(data<min_delay) min_delay=data; else data=min_delay; } }
                }
*/
//		if(j!=1)
	 	 printf("(%02x)%08x %s",j,data,kx_readptr(hw,VTFT,i)&0xffff0000?"Y":" ");
/*                 
	 	else
	 	{
	 	 int a=(data&0xff00)>>8;
	 	 int b=data&0xff;
	 	 printf("(%02d)%02d %02d",j,a,b);
	 	 if((a!=prev_a) || (b!=prev_b))
	 	 {
	 	  prev_a=a;
	 	  prev_b=b;
	 	  if(fo)
	 	   fprintf(fo," 0x%x,   0x%x,   \n",a,b);
	 	 }
	 	}
*/
	  }	
	  for(int j=0x7c;j<=0x7e;j++)
	  {
	 	gotoxy(pos*14+1,1+j-0x7c+0x1d+1);
                dword data=0;
                data=kx_readptr(hw,j,i);
/*                if(j==0x10) // VEV
                {
                 if((min_delay==0)&&(data!=0)&&(data!=0xfe00))
                 { min_delay=data; }
                 else
                 { if(data!=0) { if(data<min_delay) min_delay=data; else data=min_delay; } }
                }
*/
	 	printf("(%02x)%08x %s",j,data,kx_readptr(hw,VTFT,i)&0xffff0000?"Y":" ");
	  }
          gotoxy(pos*14+1,34);
          printf("voice: %d\n",i);

          pos++;
          if(pos>5)
           break;
#else
 // only necessary
 	  dword r1=kx_readptr(hw,FXRT1_K2,i);
 	  dword r2=kx_readptr(hw,FXRT2_K2,i);
 	  dword am=kx_readptr(hw,FXAMOUNT_K2,i);

 	  gotoxy(1,1+pos*4); 	  
 	  printf("%02x %02x %02x %02x %02x %02x %02x %02x",
 	   kx_readptr(hw,PTAB_FXSENDAMOUNT_A,i),
 	   kx_readptr(hw,PTAB_FXSENDAMOUNT_B,i),
 	   kx_readptr(hw,SCSA_FXSENDAMOUNT_C,i),
 	   kx_readptr(hw,SDL_FXSENDAMOUNT_D,i),
 	   am&0xff,
 	   (am>>8)&0xff,
 	   (am>>16)&0xff,
 	   (am>>24)&0xff);

 	  gotoxy(1,2+pos*4);

 	  printf("%02x %02x %02x %02x %02x %02x %02x %02x",
 	   r1&0xff,
 	   (r1>>8)&0xff,
 	   (r1>>16)&0xff,
 	   (r1>>24)&0xff,
 	   r2&0xff,
 	   (r2>>8)&0xff,
 	   (r2>>16)&0xff,
 	   (r2>>24)&0xff);

 	  gotoxy(25,1+pos*4);
 	  printf("filter: %04x \n",kx_readptr(hw,VTFT,i)&0xffff);

 	  gotoxy(25,2+pos*4);
 	  printf("vol: %04x\n",kx_readptr(hw,VTFT_VOLUME,i));
          
          gotoxy(1,3+pos*4);
          printf("voice: %02d ",i);

          pos++;
#endif
	 }

	 if(kbhit())
	 {
	  if(getch()!=' ')
	   quit=1;
/*	  else
	   {
	     gotoxy(1,23);
	     dword data;
	     data=kx_readptr(hw,i,QKBCA);
	     unsigned int fq=(data&0xf0000000)>>28;
	     fq++;
	     fq&=0xf;
	     kx_writeptr(hw,QKBCA,i,(kx_readptr(hw,QKBCA,i)&0xfffffff)|(fq<<28));

	     printf("filterQ: %x - %x",(data&0xf0000000)>>28,fq);
	   }
*/
	 }
	}

	 if(fo)
	  fclose(fo);

#endif

	clrscr();
#endif

#if 0
// interactive per-timer microcode dump 
	for(int cnt=0;cnt<10;cnt++)
	{
	Sleep(5000);
	printf("\7");
	
	dword buffer[0x1000]; memset(buffer,0,sizeof(buffer));
	for(dword i=0x600;i<0xe00;i++)
	{
	 buffer[i-0x600]=kx_readptr(hw,i,0);
	}

	char name[10];
	sprintf(name,"dump%d.bin",cnt);
	FILE *f;
	f=fopen(name,"wb");
	fwrite(buffer,0x800,4,f);
	fclose(f);
       }	
#endif        

#if 0
// read_ptr
	if(argc>1)
	{
		dword reg,chn=0;
		sscanf(argv[1],"%x",&reg);
		if(argv[2])
		 sscanf(argv[2],"%x",&chn);
		printf("reg %x chn %x = %x\n",reg,chn,kx_readptr(hw,reg,chn));
	}
#endif

#if 0
	if(hw->is_aps==0)
	  kx_ac97_init(hw);
#endif

#if 0
	detect_db(hw);
	printf("Card Name: %s [subsys: %x chiprev: %x]\n",hw->card_name,hw->pci_subsys,hw->pci_chiprev);
	if(hw->have_ac97)
	{
	 printf("Codec Name: %s [id: %x:%x]\n",hw->ac97_codec_name,hw->ac97_id1,hw->ac97_id2);
	}
	printf("DB: %s\n",hw->db_name);
/*
#define MAX_REQUEST	11
byte buffer[MAX_REQUEST]={0xf0,0x00,0x20,0x21,0x61,0x0,0x00,0x00,0x7f,0x0,0xf7};

   for(int i=0;i<MAX_REQUEST;i++)
   {
    kx_mpu_write_data(hw,buffer[i],1);
    kx_wcwait(hw,25);
   }
*/
#endif

#if 0
while(_kbhit()) _getch();

clrscr();

while(!_kbhit())
{
	gotoxy(1,1);
	printf("ac97:%x [io3]: %x/%x/%x inte: %x spdif_rec:%x/%x/%x     \nrec:%x/%x/%x dbg:%x bp:%x sl:%x      \r",
	  kx_readfn0(hw,AC97DATA),
	  kx_readfn0(hw,HCFG_K1),
	  kx_readfn0(hw,HCFG_K2),
	  kx_readptr(hw,EHC,0),

	  kx_readfn0(hw,INTE),

	 kx_readptr(hw,SPRI,0),
	 kx_readptr(hw,SPRA,0),
	 kx_readptr(hw,SPRC,0),
	 kx_readptr(hw,ADCBS,0),
         kx_readptr(hw,MBS,0),
         kx_readptr(hw,FXBS,0),
         kx_readptr(hw,DBG_10K2,0),
         kx_readptr(hw,SOC,0),
         kx_readptr(hw,PCB,0)
	 );

#if 0
	gotoxy(1,21);
// ac97 / spdif /rec
	printf("ac97:%x  spdif_rec:%08x/%08x/%02x  rec:%02x/%02x/%02x  \ndbg:%08x bp:%4x sl:%02x",
	  kx_readfn0(hw,AC97DATA),

	 kx_readptr(hw,SPRI,0),
	 kx_readptr(hw,SPRA,0),
	 kx_readptr(hw,SPRC,0),
	 kx_readptr(hw,ADCBS,0),
         kx_readptr(hw,MBS,0),
         kx_readptr(hw,FXBS,0),
         kx_readptr(hw,DBG_10K2,0),
         kx_readptr(hw,SOC,0),
         kx_readptr(hw,PCB,0)
	 );

	 gotoxy(1,23);
	 printf("spssx: %08x[%08x] %08x[%08x] %08x[%08x] \ni2s status: %08x %08x %08x ",
	  kx_readptr(hw,SCS0,0),kx_readptr(hw,SCS0,1),
	  kx_readptr(hw,SCS1,0),kx_readptr(hw,SCS1,1),
	  kx_readptr(hw,SCS2,0),kx_readptr(hw,SCS2,1),
	  kx_readptr(hw,SRT3,0),
	  kx_readptr(hw,SRT4,0),
	  kx_readptr(hw,SRT5,0)
	  );

	 gotoxy(1,25);
	 printf("cdcs: %08x gpscs: %08x \ncd/gp/zv/srcs: %08x %08x %08x ",
	  kx_readptr(hw,CDCS,0),
	  kx_readptr(hw,GPSCS,0),
	  kx_readptr(hw,CDSRCS,0),
	  kx_readptr(hw,GPSRCS,0),
	  kx_readptr(hw,ZVSRCS,0)
	  );
#endif
}
#endif

//  printf("HCFGs: %x %x %x\n",kx_readfn0(hw,HCFG_K1),kx_readfn0(hw,HCFG_K2),kx_readptr(hw,EHC,0));

/*
        while(!_kbhit())
          printf("%x   \r",kx_readfn0(hw, HCFG_K2));
*/
/*
  for(int i=0;i<64;i++)
  {
	dword cr=kx_readptr(hw,0x7e,i);
	printf("Current: %x\n",cr);
	kx_writeptr(hw,0x7e,i,cr|0x80808080);
	cr=kx_readptr(hw,0x7e,i);
	printf("Set: %x\n",cr);
 }	
*/
 
#if 0
 while(kbhit()) getch();

 dword old_inte=kx_readfn0(hw,INTE);
 kx_irq_disable(hw,INTE_MIDIRXENABLE);
 if(hw->is_10k2)
  kx_irq_disable(hw,INTE_K2_MIDIRXENABLE);

 while(!kbhit())
 {
  byte data;
  if((kx_mpu_read_data(hw, &data,0)!=-1) || (kx_mpu_read_data(hw,&data,1)!=-1))
  {
   if(data==0xf0) printf("\n");
   printf("[%x] ",data);
   if(data==0xf7) printf("\n");
  }
 }

 kx_writefn0(hw,INTE,old_inte);
#endif

#if 0
 dword p1=0,p2=0;
 dword t1=0,t2=0;
 while(!kbhit())
 {
  t1=kx_readptr(hw,0x61,0);
  t2=kx_readptr(hw,0x51,0);
  if(p1!=t1 || p2!=t2)
  {
   printf("%08x %08x\n",t1,t2);
   p1=t1;
   p2=t2;
  }
 }
#endif

#if 0
  _getch();

  #define TESTSIZE	400

  i=0;
  while(!_kbhit())
  {
    extern void write_mpu_data(kx_hw *hw,byte data,int where);
    extern byte read_mpu_state(kx_hw *hw,int where);

	while((read_mpu_state(hw,1) & (byte)MUSTAT_ORDYN) != 0)
	 ;
        write_mpu_data(hw,0xf0,1);

        for(i=1;i<TESTSIZE-1;i++)
        {
        	while((read_mpu_state(hw,1) & (byte)MUSTAT_ORDYN) != 0)
        	 ;
                write_mpu_data(hw,i&0x7f,1);
        }

	while((read_mpu_state(hw,1) & (byte)MUSTAT_ORDYN) != 0)
	 ;
        write_mpu_data(hw,0xf7,1);

  }
#endif
 // kx_writeptr(hw,SCS0, 0, kx_readptr(hw,SCS0,0) | SCS_NOTAUDIODATA);

/* for(int i=0;i<25;i++) printf("\n");
 getch();
 printf("bypass: %x\n",kx_readptr(hw,SOC,0));
 getch();
 printf("bypass: %x\n",kx_readptr(hw,SOC,0));
 getch();
 printf("bypass: %x\n",kx_readptr(hw,SOC,0));
*/

#if 1
 clrscr();

 while(!_kbhit())
 {
 gotoxy(1,1);
 // E-DSP stuff
 printf("Card: '%s'\n",hw->card_name);

 dword val;

 val=kx_readfpga(hw, EMU_HANA_OPTION_CARDS);
 printf("Options: %x - %s %s %s %s\n",
   val,
   val&EMU_HANA_OPTION_HAMOA?"Hamoa":"",
   val&EMU_HANA_OPTION_SYNC?"Sync":"",
   val&EMU_HANA_OPTION_DOCK_ONLINE?"Dock_online":"",
   val&EMU_HANA_OPTION_DOCK_OFFLINE?"Dock_offline":"");

 val=kx_readfpga(hw, EMU_HANA_ID);
 printf("Hana ID: %x\n",val);

 dword v1,v2,v3,v4,v5;
 v1=kx_readfpga(hw,EMU_HANA_MAJOR_REV);
 v2=kx_readfpga(hw,EMU_HANA_MINOR_REV);
 v3=kx_readfpga(hw,EMU_DOCK_MAJOR_REV);
 v4=kx_readfpga(hw,EMU_DOCK_MINOR_REV);
 v5=kx_readfpga(hw,EMU_DOCK_BOARD_ID);

 printf("Versions: %d.%d, %d.%d, %d\n",v1,v2,v3,v4,v5);

 val=kx_readfpga(hw, EMU_HANA_DOCK_PWR);
 printf("Doc is %s. [0x%x]\n",val&EMU_HANA_DOCK_PWR_ON?"Powered-On":"Powered-off",val);

 char *wclock_list[]=
 {
  "Int 48",
  "Int 44.1",
  "HANA SPDIF-In",
  "HANA ADAT-In",
  "Sync BNC",
  "2nd Hana",
  "Reserved",
  "OFF" };
  
 val=kx_readfpga(hw, EMU_HANA_WCLOCK);
 printf("WClock: %x [%s]\n",val&EMU_HANA_WCLOCK_SRC_MASK,wclock_list[val&EMU_HANA_WCLOCK_SRC_MASK]);

 val=kx_readfpga(hw, EMU_HANA_DEFCLOCK);
 printf("Def clock: %d [%x]\n",(val==EMU_HANA_DEFCLOCK_48K)?48000:(val==EMU_HANA_DEFCLOCK_44_1K)?44100:-1,val);

 val=kx_readfpga(hw, EMU_HANA_UNMUTE);
 printf("HANA is %s [%x]\n",(val==EMU_MUTE)?"Muted":(val==EMU_UNMUTE)?"UnMuted":"Unknown!",val);

 val=kx_readfpga(hw, EMU_HANA_IRQ_ENABLE);
 printf("HANA IRQs: %x\n",val);

 val=kx_readfpga(hw, EMU_HANA_SPDIF_MODE);
 printf("SPDIF mode [%x]:\n\tOut: %s%s\n\tIn: %s%s%s\n",val,
   (val&EMU_HANA_SPDIF_MODE_TX_PRO)?"Professional":"Consumer",
   (val&EMU_HANA_SPDIF_MODE_TX_NOCOPY)?"; Copy-protected":"",
   (val&EMU_HANA_SPDIF_MODE_RX_PRO)?"; Professional":"Consumer",
   (val&EMU_HANA_SPDIF_MODE_RX_NOCOPY)?"; Copy-protected":"",
   (val&EMU_HANA_SPDIF_MODE_RX_INVALID)?"; Not Valid":"");

 val=kx_readfpga(hw, EMU_HANA_OPTICAL_TYPE);
 printf("Optical Type: [%x] -- Input: %s, Output: %s\n",val,
  (val&EMU_HANA_OPTICAL_IN_ADAT)?"ADAT":"SPDIF",
  (val&EMU_HANA_OPTICAL_OUT_ADAT)?"ADAT":"SPDIF");

 char *midi_src[]=
 { "none","0202","Dock1","Dock2" };

 val=kx_readfpga(hw, EMU_HANA_MIDI_IN);
 printf("MIDI In: A: %s B: %s [%x]\n",
  midi_src[val&0x3],midi_src[(val>>3)&3],val);

 val=kx_readfpga(hw,EMU_HANA_MIDI_OUT);
 printf("MIDI Out: %x\n",val);

 v1=kx_readfpga(hw,EMU_HANA_DOCK_LEDS_1);
 v2=kx_readfpga(hw,EMU_HANA_DOCK_LEDS_2);
 v3=kx_readfpga(hw,EMU_HANA_DOCK_LEDS_3);
 printf("LEDs: %x %x %x\n",v1,v2,v3);

 val=kx_readfpga(hw,EMU_HANA_ADC_PADS);
 printf("ADC pads: %x - %s%s%s%s\n",
  val,
  (val&EMU_HANA_DOCK_ADC_PAD1)?"Dock_1 ":"",
  (val&EMU_HANA_DOCK_ADC_PAD2)?"Dock_2 ":"",
  (val&EMU_HANA_DOCK_ADC_PAD3)?"Dock_3 ":"",
  (val&EMU_HANA_0202_ADC_PAD1)?"0202_1 ":"");

 val=kx_readfpga(hw,EMU_HANA_DAC_PADS);
 printf("DAC pads: %x - %s%s%s%s%s\n",
  val,
  (val&EMU_HANA_DOCK_DAC_PAD1)?"Dock_1 ":"",
  (val&EMU_HANA_DOCK_DAC_PAD2)?"Dock_2 ":"",
  (val&EMU_HANA_DOCK_DAC_PAD3)?"Dock_3 ":"",
  (val&EMU_HANA_DOCK_DAC_PAD4)?"Dock_4 ":"",
  (val&EMU_HANA_0202_DAC_PAD1)?"0202_1 ":"");

 val=kx_readfpga(hw, EMU_HANA_DOCK_MISC);
 printf("Dock misc: [%x] DAC mute: %d%d%d%d headphones: %d\n",val,
  (val&EMU_HANA_DOCK_DAC1_MUTE)?1:0,
  (val&EMU_HANA_DOCK_DAC2_MUTE)?1:0,
  (val&EMU_HANA_DOCK_DAC3_MUTE)?1:0,
  (val&EMU_HANA_DOCK_DAC4_MUTE)?1:0,
  val>>4);

 val=kx_readfpga(hw, 0x38);
 printf("Reg 0x38: %x\nReg 0x39: %x\n",val,kx_readfpga(hw,0x39));

 if ((val & 0x1) == 0) 
 {
    dword val1, val2;
    val1=kx_readfpga(hw, EMU_HANA_WC_ADAT_HI);
    val2=kx_readfpga(hw, EMU_HANA_WC_ADAT_LO);
    printf("ADAT Locked : %d\n", 0x1770000 / (((val1 << 5) | val2)+1));
 } else
     printf("ADAT Unlocked: %x.%x\n",kx_readfpga(hw, EMU_HANA_WC_ADAT_HI),kx_readfpga(hw, EMU_HANA_WC_ADAT_LO));

 val=kx_readfpga(hw, EMU_HANA_IRQ_STATUS);

 if ((val & EMU_HANA_IRQ_DOCK) == 0)  // ???
 {
     dword val1,val2;
     val1=kx_readfpga(hw, EMU_HANA_WC_SPDIF_HI);
     val2=kx_readfpga(hw, EMU_HANA_WC_SPDIF_LO);

	printf("SPDIF Locked : %d\n", 0x1770000 / (((val1 << 5) | val2)+1));
 } else
			printf("SPDIF Unlocked\n");

 if( 1 )
 {  
     dword val1,val2;
     val1=kx_readfpga(hw, EMU_HANA_WC_BNC_HI);
     val2=kx_readfpga(hw, EMU_HANA_WC_BNC_LO);

	printf("BNC Locked : %d\n", 0x1770000 / (((val1 << 5) | val2)+1));
 } else
    printf("Sync card unlocked\n");

 if ( 1 ) 
 {
     dword val1,val2;
     val1=kx_readfpga(hw, EMU_HANA2_WC_SPDIF_HI);
     val2=kx_readfpga(hw, EMU_HANA2_WC_SPDIF_LO);

	printf("SPDIF Locked : %d\n", 0x1770000 / (((val1 << 5) | val2)+1));
 } else
			printf("SPDIF Unlocked\n");

 printf("reg 14-1f:");  
 for(int i=0x14;i<0x20;i++)
 {
  val=kx_readfpga(hw,i);
  printf("[%x] ",val);
 }
  printf("\n");

 printf("reg 30-3f:");
 for(int i=0x30;i<0x3f;i++)
 {
  val=kx_readfpga(hw,i);
  printf("[%x] ",val);
 }
  printf("\n");
 Sleep(1000);
 clrscr();
 } // while(1)
 #endif

#if 0
 dword val=kx_readfpga(hw,EMU_HANA_WCLOCK);
 printf("\n\n\n\n\nHCFGs: %x %x %x; wclock: %x\n",kx_readfn0(hw,HCFG_K1),kx_readfn0(hw,HCFG_K2),kx_readptr(hw,EHC,0),val);
#endif

	kx_close(&hw);

	return 0;
}
