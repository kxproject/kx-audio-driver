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

#define is_valid_gpr(a) ( ((a)!=DSP_REG_NOT_TRANSLATED) && ((a)>=E10K1_GPR_BASE) && ((a)<hw->first_instruction) )
#define is_register(a) (a&0xd000)

#undef set_bit
#undef get_bit
#undef clear_bit

#define get_bit(a,b) ((a[(b)/32]&(1<<((b)%32)))?1:0)
#define set_bit(a,b) (a[(b)/32])|=(1<<(((b)%32)))
#define clear_bit(a,b) (a[(b)/32])&=~(1<<(((b)%32)))

#define TRAM_CLEAR	0x1000
#define TRAM_ALIGN	0x2000

inline int upload_instruction(kx_hw *hw,dsp_microcode *m,int i);

inline int e10k1_IntWriteAlignBit( int instr, int tbuffer ) {
    return (((instr)>=((tbuffer)*3))?1:0);
 }
inline int e10k1_ExtWriteAlignBit( int instr, int tbuffer ) {
    return (((((int)(instr)-128))>=((tbuffer)*4))?1:0);
 }
inline int e10k1_IntReadAlignBit( int instr, int tbuffer ) {
    return (((((int)(instr)-  1))<=((tbuffer)*3))?1:0);
 }
inline int e10k1_ExtReadAlignBit( int instr, int tbuffer ) {
    return (((((int)(instr)-127))<=((tbuffer)*4))?1:0);
 }
inline int e10k2_IntWriteAlignBit( int instr, int tbuffer ) {
    return (instr > (tbuffer*4-3)) ? 1 : 0;
 }
inline int e10k2_ExtWriteAlignBit( int instr, int tbuffer ) {
    return (instr > ((tbuffer+192)*4-3)) ? 1 : 0;
 }
inline int e10k2_IntReadAlignBit( int instr, int tbuffer ) {
    return (instr < ((tbuffer+1)*4+1)) ? 1 : 0;
 }
inline int e10k2_ExtReadAlignBit( int instr, int tbuffer ) {
    return (instr < ((tbuffer+193)*4+1)) ? 1 : 0;
 }

#define TANKMEMFLAG_READ_K2V	0x1
#define TANKMEMFLAG_CLEAR_K2V	0x2
#define TANKMEMFLAG_WRITE_K2V	0x4
#define TANKMEMFLAG_ALIGN_K2V	0x8

static dword emu10k2_tram_opcodes[]=
{
 0x0,
   0x2, // 1: 0001 read
 0x0, // 2: 0010 clear
   0x3, // 3: 0011 read+clear
   0x6, // 4: 0100 write
   0xa, // 5: 0101 write+read (sum)
 0x6, // 6: 0110 write+clear // not correct
   0xb, // 7: 0111 read+sum&write+clear
 0x0, // 8: 1000 align
   0x4, // 9: 1001 read+align
 0x0, // a: 1010 align+clear
   0x5, // b: 1011 read+align+clear
   0x7, // c: 1100 write+align
   0xc, // d: 1101 read+sum&write+align
 0x6, // e: 1110 write+align+clear // not correct
   0xd // f: 1111 read+sum&write+align+clear
};
// 0x8 - Tram table read
// 0x9 - Tram table write
// |=0x10 - Uncompressed data: 
//   Uncompressed data stored to TRAM truncates the LS 16 bits [sh]

inline dword calc_tram_flag(kx_hw *hw,dword flag)
{
 if(hw->is_10k2)
 {
  dword tmp=0;
  if(flag&TRAM_READ) tmp|=TANKMEMFLAG_READ_K2V;
  if(flag&TRAM_WRITE) tmp|=TANKMEMFLAG_WRITE_K2V;
  if(flag&TRAM_CLEAR) tmp|=TANKMEMFLAG_CLEAR_K2V;
  if(flag&TRAM_ALIGN) tmp|=TANKMEMFLAG_ALIGN_K2V;

  return emu10k2_tram_opcodes[tmp&0xf];
 }
 else
 {
  return ((flag&TRAM_READ)?TRAM_READ_K1:0)|
  ((flag&TRAM_WRITE)?TRAM_WRITE_K1:0)|
  ((flag&TRAM_CLEAR)?TRAM_CLEAR_K1:0)|
  ((flag&TRAM_ALIGN)?TRAM_ALIGN_K1:0);
 }
}

inline dword get_tram_flag(kx_hw *hw,dword tmp)
{
  dword ret=0;

  if(hw->is_10k2)
  {
 	  for(int i=0;i<16;i++)
 	   if(tmp==emu10k2_tram_opcodes[i])
 	   {
 	    if(emu10k2_tram_opcodes[i]&TANKMEMFLAG_WRITE_K2V)
 	      ret|=TRAM_WRITE;
            if(emu10k2_tram_opcodes[i]&TANKMEMFLAG_READ_K2V)
 	      ret|=TRAM_READ;
 	    break;
 	   }
  }
  else
  {
   if(tmp&TRAM_WRITE_K1) ret|=TRAM_WRITE;
   if(tmp&TRAM_READ_K1) ret|=TRAM_READ;
  }
  return ret;
}

// for MacOS X we need our own strstr()
#if defined(__APPLE__) || defined(__MACH__)	
char *strstr(const char *str1, const char *str2);
char *strstr(const char *str1, const char *str2)
{
	char *cp = (char *) str1;
	char *s1, *s2;
	
	if ( !*str2 )
		return((char *)str1);
	
	while (*cp)
	{
		s1 = cp;
		s2 = (char *) str2;
		
		while ( *s1 && *s2 && !(*s1-*s2) )
			s1++, s2++;
		
		if (!*s2)
			return(cp);
		
		cp++;
	}
	
	return(NULL);
	
}
#endif

dsp_register_info *find_dsp_register(kx_hw *hw,int pgm_id,word id,dsp_microcode **out_m=NULL);
dsp_register_info *find_dsp_register(kx_hw *hw,int pgm_id,const char *name,dsp_microcode **out_m=NULL);

inline dsp_register_info *find_dsp_register_in_m(kx_hw *hw,dsp_microcode *m,word id)
{
   for(dword i=0;i<m->info_size/sizeof(dsp_register_info);i++)
   	if(m->info[i].num==id)
   		return &m->info[i];
   return NULL;
}

inline dsp_register_info *find_dsp_register_in_m(kx_hw *hw,dsp_microcode *m,const char *name)
{
   for(dword i=0;i<m->info_size/sizeof(dsp_register_info);i++)
 	if(strncmp(name,&m->info[i].name[0],MAX_GPR_NAME)==NULL)
   		return &m->info[i];
   return NULL;
}

// returns 0 if not found
dsp_register_info *find_dsp_register(kx_hw *hw,int pgm_id,word id,dsp_microcode **out_m)
{
  struct list *item;

  if(!(hw->initialized&KX_DSP_INITED))
  {
   debug(DLIB,"EFX find_dsp() w/o being initialized\n");
   return NULL;
  }

  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->dsp_lock, &flags);

 int pgm=pgm_id;

  for_each_list_entry(item, &hw->microcodes) 
  {
  	dsp_microcode *m;
        m = list_item(item, dsp_microcode, list);
        if(!m)
         continue;
        if(m->pgm!=pgm)
          continue;
        // found pgm; search through info
        dsp_register_info *ret;
        ret=find_dsp_register_in_m(hw,m,id);
        if(ret)
        {
             kx_lock_release(hw,&hw->dsp_lock,&flags);
             if(out_m)
                 *out_m=m;
             return ret;
        }
        break;
  }

  kx_lock_release(hw,&hw->dsp_lock,&flags);

  return 0;
}

dsp_register_info *find_dsp_register(kx_hw *hw,int pgm_id,const char *name,dsp_microcode **out_m)
{
  struct list *item;

  if(!(hw->initialized&KX_DSP_INITED))
  {
   debug(DLIB,"EFX find_dsp() w/o being initialized\n");
   return NULL;
  }

  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->dsp_lock, &flags);

 int pgm=pgm_id;

  for_each_list_entry(item, &hw->microcodes) 
  {
  	dsp_microcode *m;
        m = list_item(item, dsp_microcode, list);
        if(!m)
         continue;
        if(m->pgm!=pgm)
         continue;
        // found pgm; search through info
        dsp_register_info *ret;
        ret=find_dsp_register_in_m(hw,m,name);
        if(ret)
        {
      		kx_lock_release(hw,&hw->dsp_lock,&flags);
                if(out_m)
                   *out_m=m;
       		return ret;
        }
        break;
  }

  kx_lock_release(hw,&hw->dsp_lock,&flags);

  return 0;
}


KX_API(int,kx_set_dsp_register(kx_hw *hw,int pgm,const char *name,dword val))
{
 dsp_register_info *info;
 info=find_dsp_register(hw,pgm,name);
 if(info)
 {
 	info->p=val;
 	if(is_valid_gpr(info->translated))
 	{
 	 kx_writeptr(hw,info->translated,0,val);
 	}
 	return 0;
 }
 debug(DLIB,"!! set_dsp failed [%d.%s]\n",pgm,name);
 return -1;
}

KX_API(int,kx_get_dsp_register(kx_hw *hw,int pgm,const char *name,dword *val))
{
 dsp_register_info *info;
 info=find_dsp_register(hw,pgm,name);
 if(info)
 {
 	*val=info->p;
 	if(is_valid_gpr(info->translated))
 	{
 	 *val=kx_readptr(hw,info->translated,0);
 	}
 	return 0;
 }
 debug(DLIB,"!! get_dsp failed [%d.%s]\n",pgm,name);
 return -1;
}

KX_API(int,kx_set_dsp_register(kx_hw *hw,int pgm,word id,dword val))
{
 dsp_register_info *info;
 info=find_dsp_register(hw,pgm,id);
 if(info)
 {
 	info->p=val;
 	if(is_valid_gpr(info->translated))
 	{
 	 kx_writeptr(hw,info->translated,0,val);
 	}
 	return 0;
 }
 debug(DLIB,"!! set_dsp failed [%d.%x]\n",pgm,id);
 return -1;
}

KX_API(int,kx_get_dsp_register(kx_hw *hw,int pgm,word id,dword *val))
{
 dsp_register_info *info;
 info=find_dsp_register(hw,pgm,id);
 if(info)
 {
 	*val=info->p;
 	if(is_valid_gpr(info->translated))
 	{
 	 *val=kx_readptr(hw,info->translated,0);
 	}
 	return 0;
 }
 debug(DLIB,"!! get_dsp failed [%d.%x]\n",pgm,id);
 return -1;
}

static int get_tram_addr(kx_hw *hw,dsp_microcode *m,dsp_register_info *info,dword *addr)
{
 	*addr=info->p;
 	if(is_valid_gpr(info->translated))
 	{
         if(((info->type&GPR_MASK)==GPR_ITRAM) || ((info->type&GPR_MASK)==GPR_XTRAM))
 	 {
 	  *addr=kx_readptr(hw,info->translated+0x100,0);
 	 }
 	 else
 	 {
 	  debug(DLIB,"!! get_tram_addr(): not a TRAM!\n");
 	  return -1;
 	 }
 	 if(hw->is_10k2)
 	  *addr=(*addr)>>0xb;
 	 else
 	  *addr=(*addr)&TANKMEMADDRREG_ADDR_MASK_K1;

 	 if((info->type&GPR_MASK)==GPR_ITRAM)
          *addr-=m->itram_start;
         else
          *addr-=m->xtram_start;
 	}
 	return 0;
}

static int set_tram_addr(kx_hw *hw,dsp_microcode *m,dsp_register_info *info,dword addr)
{
        info->p=addr;

        if((info->type&GPR_MASK)==GPR_ITRAM)
         addr+=m->itram_start;
        else
         addr+=m->xtram_start;

        if(hw->is_10k2)
         addr=addr<<0xb;
        else
         addr=addr&TANKMEMADDRREG_ADDR_MASK_K1;

 	if(is_valid_gpr(info->translated))
 	{
         if(((info->type&GPR_MASK)==GPR_ITRAM) || ((info->type&GPR_MASK)==GPR_XTRAM))
 	 {
 	  if(hw->is_10k2)
 	   kx_writeptr(hw,info->translated+0x100,0,addr);
 	  else
 	  {
 	   dword tmp=kx_readptr(hw,info->translated+0x100,0);
 	   tmp=tmp&(~TANKMEMADDRREG_ADDR_MASK_K1);
 	   kx_writeptr(hw,info->translated+0x100,0,tmp|addr);
 	  }
 	 }
 	 else
 	 {
 	  debug(DLIB,"!! set_tram_addr(): not a TRAM!\n");
 	  return -1;
 	 }
 	}
 	return 0;
}

static int set_tram_flag(kx_hw *hw,dsp_microcode *m,dsp_register_info *info,dword flag)
{
 	if(is_valid_gpr(info->translated))
 	{
 	 dword reg;
 	 if((info->type&GPR_MASK)==GPR_TRAMA)
 	 {
 	  debug(DLIB,"!! set_tram_flag(): TRAMA, not ITRAM/XTRAM\n");
 	  return -1;
 	 }
         if(((info->type&GPR_MASK)==GPR_ITRAM) || ((info->type&GPR_MASK)==GPR_XTRAM))
 	 {
 	  reg=info->translated+0x100;
 	 }
 	 else
 	 {
 	  debug(DLIB,"!! get_tram_flag(): not a TRAM!\n");
 	  return -1;
 	 }
 	 if(hw->is_10k2)
 	  reg-=0x200;

 	 dword tmp=0;
         // recalculate 'ALIGN' instruction position
         int instr_num=-1;
         for(dword j=0;j<m->code_size/sizeof(dsp_code);j++)
         {
          if( (m->code[j].r==info->num) ||
              (m->code[j].a==info->num) ||
              (m->code[j].x==info->num) ||
              (m->code[j].y==info->num) )
          { instr_num=j; break; }
         }
         if(instr_num==-1)
         {
          // debug(DLIB,"!! set_tram_flag: tram instruction not found! [%d; %s]\n",m->pgm,m->name);
          instr_num=0;
         }
 	 if(hw->is_10k2)
 	 {
          // re-calc align flag
          if((info->type&GPR_MASK)==GPR_ITRAM)
          {
           if(flag&TRAM_WRITE)
           {
            if(e10k2_IntWriteAlignBit(instr_num,info->translated-TANKMEMDATAREGBASE))
             flag|=TRAM_ALIGN;
           }
           else
           {
            if(e10k2_IntReadAlignBit(instr_num,info->translated-TANKMEMDATAREGBASE))
             flag|=TRAM_ALIGN;
           }
          }
          else // xtram
          {
           if(flag&TRAM_WRITE)
           {
            if(e10k2_ExtWriteAlignBit(instr_num,info->translated-TANKMEMDATAREGBASE))
              flag|=TRAM_ALIGN;
           }
           else
           {
            if(e10k2_ExtReadAlignBit(instr_num,info->translated-TANKMEMDATAREGBASE))
              flag|=TRAM_ALIGN;
           }
          }
          tmp=calc_tram_flag(hw,flag);
 	 }
 	 else
 	 {
 	  tmp=kx_readptr(hw,reg,0);

 	  tmp&=~TRAM_WRITE_K1;
 	  tmp&=~TRAM_READ_K1;
          tmp&=~TRAM_CLEAR_K1;
          tmp&=~TRAM_ALIGN_K1;
          
          // re-calc align flag
          if((info->type&GPR_MASK)==GPR_ITRAM)
          {
           if(flag&TRAM_WRITE)
           {
            if(e10k1_IntWriteAlignBit(instr_num,info->translated-TANKMEMDATAREGBASE))
             flag|=TRAM_ALIGN;
           }
           else
           {
            if(e10k1_IntReadAlignBit(instr_num,info->translated-TANKMEMDATAREGBASE))
             flag|=TRAM_ALIGN;
           }
          }
          else // xtram
          {
           if(flag&TRAM_WRITE)
           {
            if(e10k1_ExtWriteAlignBit(instr_num,info->translated-TANKMEMDATAREGBASE))
             flag|=TRAM_ALIGN;
           }
           else
           {
            if(e10k1_ExtReadAlignBit(instr_num,info->translated-TANKMEMDATAREGBASE))
             flag|=TRAM_ALIGN;
           }
          }
          tmp|=calc_tram_flag(hw,flag);
 	 }
         if(flag&TRAM_CLEAR)
         {
          kx_writeptr(hw,info->translated,0,info->p);
         }
 	 kx_writeptr(hw,reg,0,tmp);

         // set 30 bit of DBG register
         if(flag&TRAM_CLEAR)
         {
          if(!hw->is_10k2)
           kx_writeptr(hw,DBG_10K1,0,DBG_10K1_ZC|kx_readptr(hw,DBG_10K1,0));
          else
           kx_writeptr(hw,DBG_10K2,0,DBG_10K2_ZC|kx_readptr(hw,DBG_10K2,0));

          // fixme: do we need a delay here?

          if(!hw->is_10k2)
           kx_writeptr(hw,DBG_10K1,0,kx_readptr(hw,DBG_10K1,0)&(~DBG_10K1_ZC));
          else
           kx_writeptr(hw,DBG_10K2,0,kx_readptr(hw,DBG_10K2,0)&(~DBG_10K2_ZC));
         }
 	}
        info->type&=~(TRAM_READ|TRAM_WRITE);
        if(flag&TRAM_READ) info->type|=TRAM_READ;
        if(flag&TRAM_WRITE) info->type|=TRAM_WRITE;
 	return 0;
}

static int get_tram_flag(kx_hw *hw,dsp_microcode *m,dsp_register_info *info,dword *flag)
{
 	if(is_valid_gpr(info->translated))
 	{
 	 dword reg;
 	 if((info->type&GPR_MASK)==GPR_TRAMA)
 	 {
 	  reg=info->translated;
 	 }
 	 else
         if(((info->type&GPR_MASK)==GPR_ITRAM) || ((info->type&GPR_MASK)==GPR_XTRAM))
 	 {
 	  reg=info->translated+0x100;
 	 }
 	 else
 	 {
 	  debug(DLIB,"!! get_tram_flag(): not a TRAM!\n");
 	  return -1;
 	 }
 	 if(hw->is_10k2)
 	  reg-=0x200;
 	 dword tmp=kx_readptr(hw,reg,0);
         *flag=0;

         *flag=get_tram_flag(hw,tmp);
 	}
 	else // not translated yet
 	{
 	 if((info->type&GPR_MASK)==GPR_TRAMA)
 	 {
 	  debug(DLIB,"!! get_tram_flag failed: trama instead of itram/xtram\n");
 	  return -1;
 	 }
         *flag=info->type&(TRAM_READ|TRAM_WRITE);
 	}
 	return 0;
}

KX_API(int,kx_get_tram_addr(kx_hw *hw,int pgm,const char *name,dword *addr))
{
 dsp_microcode *m=NULL;
 dsp_register_info *info;
 info=find_dsp_register(hw,pgm,name,&m);
 if(info)
 {
        if((info->type&GPR_MASK)==GPR_TRAMA)
        {
         // *addr=kx_readptr(hw,info->translated,0);
         debug(DLIB,"!! get_tram_addr(): TRAMA; should be iTRAM/XTRAM\n");
         return -1;
        }

        return get_tram_addr(hw,m,info,addr);
 }
 debug(DLIB,"!! get_tram_addr failed\n");
 return -1;
}

KX_API(int,kx_set_tram_addr(kx_hw *hw,int pgm,const char *name,dword addr))
{
 dsp_microcode *m=NULL;
 dsp_register_info *info;
 info=find_dsp_register(hw,pgm,name,&m);
 if(info)
 {
        if((info->type&GPR_MASK)==GPR_TRAMA)
        {
         debug(DLIB,"!! set_tram_addr(): is TRAMA, should be ITRAM/XTRAM\n");
         return -1;
        }
        return set_tram_addr(hw,m,info,addr);
 }
 debug(DLIB,"!! set_tram_addr failed\n");
 return -1;
}

KX_API(int,kx_get_tram_flag(kx_hw *hw,int pgm,const char *name,dword *flag))
{
 dsp_register_info *info;
 dsp_microcode *m;

 info=find_dsp_register(hw,pgm,name,&m);
 
 if(info)
 {
 	return get_tram_flag(hw,m,info,flag);
 }
 debug(DLIB,"!! get_tram_flag failed\n");
 return -1;
}

KX_API(int,kx_set_tram_flag(kx_hw *hw,int pgm,const char *name,dword flag))
{
 dsp_register_info *info;
 dsp_microcode *m;
 info=find_dsp_register(hw,pgm,name,&m);
 if(info)
 {
 	return set_tram_flag(hw,m,info,flag);
 }
 debug(DLIB,"!! get_tram_addr failed\n");
 return -1;
}

KX_API(int,kx_get_tram_addr(kx_hw *hw,int pgm,word id,dword *addr))
{
 dsp_microcode *m=NULL;
 dsp_register_info *info;
 info=find_dsp_register(hw,pgm,id,&m);
 if(info)
 {
        if((info->type&GPR_MASK)==GPR_TRAMA)
        {
         // *addr=kx_readptr(hw,info->translated,0);
         debug(DLIB,"!! get_tram_addr(): TRAMA; should be iTRAM/XTRAM\n");
         return -1;
        }

        return get_tram_addr(hw,m,info,addr);
 }
 debug(DLIB,"!! get_tram_addr failed\n");
 return -1;
}

KX_API(int,kx_set_tram_addr(kx_hw *hw,int pgm,word id,dword addr))
{
 dsp_microcode *m=NULL;
 dsp_register_info *info;
 info=find_dsp_register(hw,pgm,id,&m);
 if(info)
 {
        if((info->type&GPR_MASK)==GPR_TRAMA)
        {
         debug(DLIB,"!! set_tram_addr(): is TRAMA, should be ITRAM/XTRAM\n");
         return -1;
        }
        return set_tram_addr(hw,m,info,addr);
 }
 debug(DLIB,"!! set_tram_addr failed\n");
 return -1;
}

KX_API(int,kx_get_tram_flag(kx_hw *hw,int pgm,word id,dword *flag))
{
 dsp_register_info *info;
 dsp_microcode *m;

 info=find_dsp_register(hw,pgm,id,&m);
 
 if(info)
 {
 	return get_tram_flag(hw,m,info,flag);
 }
 debug(DLIB,"!! get_tram_flag failed\n");
 return -1;
}

KX_API(int,kx_set_tram_flag(kx_hw *hw,int pgm,word id,dword flag))
{
 dsp_register_info *info;
 dsp_microcode *m;
 info=find_dsp_register(hw,pgm,id,&m);
 if(info)
 {
 	return set_tram_flag(hw,m,info,flag);
 }
 debug(DLIB,"!! get_tram_addr failed\n");
 return -1;
}

KX_API(int,kx_write_instruction(kx_hw *hw,int pgm,int offset,word op,word r,word a,word x,word y,int valid))
{
  struct list *item;

  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->dsp_lock, &flags);

  for_each_list_entry(item, &hw->microcodes)
  {
  	dsp_microcode *m;
        m = list_item(item, dsp_microcode, list);
        if(!m)
         continue;
        if((m->pgm==pgm) && (m->code_size>0))
        {
            if((dword)offset>m->code_size/sizeof(dsp_code))
            {
             debug(DLIB,"!! write_instr: offset overflow(2) [%d %d %d]\n",offset,m->code_size,m->pgm);
             kx_lock_release(hw,&hw->dsp_lock,&flags);
             return -6;
            }
            if(valid&VALID_OP) m->code[offset].op=(byte)op;
            if(valid&VALID_X) m->code[offset].x=x;
            if(valid&VALID_Y) m->code[offset].y=y;
            if(valid&VALID_R) m->code[offset].r=r;
            if(valid&VALID_A) m->code[offset].a=a;

            if(m->flag&MICROCODE_TRANSLATED)
             upload_instruction(hw,m,offset);

            kx_lock_release(hw,&hw->dsp_lock,&flags);
            return 0;
        }
  }
  kx_lock_release(hw,&hw->dsp_lock,&flags);
  debug(DLIB,"!! write_instruction: pgm not found\n");
  return -1; // not found
}

KX_API(int,kx_read_instruction(kx_hw *hw,int pgm,int offset, word *op,word *r,word *a,word *x,word *y))
{
  struct list *item;

  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->dsp_lock, &flags);

  for_each_list_entry(item, &hw->microcodes)
  {
  	dsp_microcode *m;
        m = list_item(item, dsp_microcode, list);
        if(!m)
         continue;
        if((m->pgm==pgm) && (m->code_size>0))
        {
            if((dword)offset>m->code_size/sizeof(dsp_code))
            {
             debug(DLIB,"!! read_instr: offset overflow(3) [%d %d %d]\n",offset,m->code_size,m->pgm);
             kx_lock_release(hw,&hw->dsp_lock,&flags);
             return -6;
            }
            if(op) *op=m->code[offset].op;
            if(r) *r=m->code[offset].r;
            if(a) *a=m->code[offset].a;
            if(x) *x=m->code[offset].x;
            if(y) *y=m->code[offset].y;

            kx_lock_release(hw,&hw->dsp_lock,&flags);
            return 0;
        }
  }

  kx_lock_release(hw,&hw->dsp_lock,&flags);
  debug(DLIB,"!! read_instruction: pgm not found\n");
  return -1; // not found
}

static inline void check_const(kx_hw *hw,word &a)
{
 if((a>=KX_CONST)&&(a<KX_IN(0)))  // const
 { 
   a=a-KX_CONST+(hw->is_10k2?0x80:0);
   if(hw->is_10k2)
   {
    if(a>0xdb && a!=0xeb)
    {
     debug(DWDM,"!! constant invalid [%x] - 10k2\n",a);
     a=0xc0;
    }
   }
   else
   {
    if(a>0x5b)
    {
     debug(DWDM,"!! constant invalid [%x] - 10k1\n",a);
     a=0x40;
    }
   }
 }
 else
  if((a>=KX_IN(0))&&(a<KX_OUT(0))) // inputs
  {
   if(!hw->is_10k2)
   {
    a=a-KX_IN(0)+0x10;
    if(a>=0x20)
    {
     debug(DLIB,"invalid 10k1 input [%x]\n",a);
     a=0x40;
    }
   }
   else
   {
    // swap 4:5 <--> 6:7 for 10k2
    if(a==KX_IN(6)) // 6
     a=KX_IN(4); // 4
    else
     if(a==KX_IN(7)) // 7
      a=KX_IN(5); // 5
      else
       if(a==KX_IN(4)) // 4
        a=KX_IN(6); // 6
       else
        if(a==KX_IN(5)) // 5
         a=KX_IN(7); // 7

     if(a>=KX_IN(0) && a<=KX_IN(0xe))
     {
      a=a-KX_IN(0)+0x40;
     }
     else
     {
    	 debug(DLIB,"Unknown 10k2 input (%x)\n",a);
    	 a=0xc0;
     }
   }
  }
  else
   if((a>=KX_OUT(0))&&(a<KX_FX(0))) // outputs
   {
    if(!hw->is_10k2)
    {
     a=a-KX_OUT(0)+0x20;

     if(a>0x34) // 0x31/32 - 5.1 center/lfe
     		    // 0x33/34 - ac97 sb22x rear
     {
     	debug(DLIB,"invalid 10k1 output [%x]\n",a);
     	a=0x40;
     }
    }
    else // not 10k1
    {
     if(hw->is_k8)
     {
        // mapping for a2 value and a2zsnb
            switch(a)
            {
               // FRONT Left/Right
               // direct translation:
               case 0x2300: a=0x60; break; // front l (i2s 0) (breakout front + headphones for a2zsnb)
               case 0x2301: a=0x61; break; // front r (i2s 0)
               // pseudo translation - mirror digital/analogue/ac97
               case 0x2320: a=0x70; break; // AC97 DAC Front L
               case 0x2321: a=0x71; break; // AC97 DAC Front R

               // also 'generic' spdif out
               case 0x2302: a=0x68; break; // din front l (spdif 0)
               case 0x2303: a=0x69; break; // din front r (spdif 0)

               // DIN center/sub
               // direct translation
               case 0x2304: a=0x6a; break; // din center (spdif 1)
               case 0x2305: a=0x6b; break; // din sub	  (spdif 1)

               // LD Headphones
               case 0x2306: a=0x6c; break; // headphones l (spdif 2)
               case 0x2307: a=0x6d; break; // headphones r (spdif 2)

               // REAR Left/Right
               // direct translation:
               case 0x2308: a=0x66; break; // rear l (i2s 3)
               case 0x2309: a=0x67; break; // rear r (i2s 3)
               // pseudo
               case 0x2328: a=0x6e; break; // din rear l (spdif 3)
               case 0x2329: a=0x6f; break; // din rear r (spdif 3)

               // Analog center/sub
               // direct traslation
               case 0x2311: a=0x62; break; // center (i2s 1)
               case 0x2312: a=0x63; break; // sub (i2s 1)

               // side left/right
               case 0x2330: a=0x64; break; // (i2s 2) l
               case 0x2331: a=0x65; break; // (i2s 2) r

               // currently unused
               case 0x2372: a=0x72; break; // AC97 DAC Center (ac97 2.0 only)
               case 0x2373: a=0x73; break; // AC97 DAC Rear L (ac97 2.0 only)
               case 0x2374: a=0x74; break; // AC97 DAC Rear R (ac97 2.0 only)
               case 0x2375: a=0x75; break; // AC97 DAC Subwoofer (ac97 2.0 only)

               // Recording ADC buffers
               case 0x230a: a=0x76; break;
               case 0x230b: a=0x77; break;

               case 0x230c: a=0x78; break; // Mic 8kHz src output

               default:
                if(a>=0x2360 && a<=0x23ff)
                {
                 debug(DLIB,"! using unknown/physical 10kx output [%x]\n",a-0x2300);
                 a-=0x2300;
                }
                else
                {
                 debug(DLIB,"! unknown 10k1/2 output [%x]\n",a);
                 a=0xc0; 
                }
            }
     } // is k4?
     else // not k4?
     switch(a)
     {
     	// FRONT Left/Right
        // direct translation:
        case 0x2300: a=0x68; break; // front l (i2s 0)
        case 0x2301: a=0x69; break; // front r (i2s 0)
        // pseudo translation - mirror digital/analogue/ac97
        case 0x2320: a=0x70; break; // AC97 DAC Front L
        case 0x2321: a=0x71; break; // AC97 DAC Front R

        // also 'generic' spdif out
        case 0x2302: a=0x60; break; // din front l (spdif 0)
        case 0x2303: a=0x61; break; // din front r (spdif 0)

        // DIN center/sub
        // direct translation
        case 0x2304: a=0x62; break; // din center (spdif 1)
        case 0x2305: a=0x63; break; // din sub	  (spdif 1)

        // LD Headphones
        case 0x2306: a=0x64; break; // headphones l (spdif 2)
        case 0x2307: a=0x65; break; // headphones r (spdif 2)

        // REAR Left/Right
        // direct translation:
        case 0x2308: a=0x6e; break; // rear l (i2s 3)
        case 0x2309: a=0x6f; break; // rear r (i2s 3)
        // digital rear:
        case 0x2328: a=0x66; break; // din rear l (spdif 3)
        case 0x2329: a=0x67; break; // din rear r (spdif 3)

        // Analog center/sub
        // direct traslation
        case 0x2311: a=0x6a; break; // center (i2s 1)
        case 0x2312: a=0x6b; break; // sub (i2s 1)
        // side left/right:
        case 0x2330: a=0x6c; break; // (i2s 2) l
        case 0x2331: a=0x6d; break; // (i2s 2) r

        // currently unused
        case 0x2372: a=0x72; break; // AC97 DAC Center (ac97 2.0 only)
        case 0x2373: a=0x73; break; // AC97 DAC Rear L (ac97 2.0 only)
        case 0x2374: a=0x74; break; // AC97 DAC Rear R (ac97 2.0 only)
        case 0x2375: a=0x75; break; // AC97 DAC Subwoofer (ac97 2.0 only)

        // Recording ADC buffers
        case 0x230a: a=0x76; break;
        case 0x230b: a=0x77; break;

        case 0x230c: a=0x78; break; // Mic 8kHz src output

        default:
         if(a>=0x2360 && a<=0x23ff)
         {
          debug(DLIB,"! using unknown/physical 10kx output [%x]\n",a-0x2300);
          a-=0x2300;
         }
         else
         {
          debug(DLIB,"! unknown 10k1/2 output [%x]\n",a);
          a=0xc0; 
         }
     }
    } // 10k1
   }
  else
   if((a>=KX_FX(0))&&(a<KX_FX2(0))) // FX1 - FX busses
   {
     a=a-KX_FX(0);
     if(a>=0x40)
     {
      debug(DLIB,"invalid fx1 value [%x]\n",a);
      a=0x40+(hw->is_10k2?0x80:0);
     }
   }
  else
   if((a>=KX_FX2(0))&&(a<KX_E32IN(0))) // FX2 - temporary registers [asio recording]
   {
    if(!hw->is_10k2)
    {
     a=a-KX_FX2(0)+0x30;
     if(a>=0x40)
     {
      debug(DLIB,"invalid kx_fx2 for 10k1 [%x]\n",a);
      a=0x40;
     }
    }
    else
    {
     a=a-KX_FX2(0)+0x80;
     if(a>=0xc0)
     {
      debug(DLIB,"invalid kx_fx2 for 10k2 [%x]\n",a);
      a=0xc0;
     }
    }
   }
   else
    if((a>=KX_E32IN(0))&&(a<KX_E32IN(0x3f)))
    {
     if(hw->is_10k2)
     {
      if(hw->is_k8)
      {
           a=a-KX_E32IN(0)+0x160;
      }
      else
      {
           a=a-KX_E32IN(0)+0x50;
           if(a>=0x60)
           {
            debug(DLIB,"invalid p16v / kx_e32in constand [%x]\n",a);
            a=0xc0;
           }
      }
     }
     else
     {
      debug(DLIB,"kx_e32in [p16v in] is not supported for non-10k2 cards\n");
      a=0x40;
     }
    }
    else
     if((a>=KX_E32OUT(0)) && (a<KX_E32OUT(0x3f)))
     {
       if(hw->is_10k2)
       {
         if(hw->is_k8)
         {
           a=a-KX_E32OUT(0)+0x1e0;
         }
         else
         {
           if(a<KX_E32OUT(0x10)) // lower
             a=a-KX_E32OUT(0)+0xb0;
           else
            if(a<KX_E32OUT(0x20)) // higher
              a=a-KX_E32OUT(0x10)+0xa0;
            else
            {
              debug(DLIB,"invalid e32out constant [%x]\n",a);
              a=0xc0;
            }
         }
       }
       else
       {
          debug(DLIB,"kx_e32out is not supported for non-10k2 cards\n");
          a=0x40;
       }
     }
}

KX_API(int,kx_set_volume(kx_hw *hw,int pgm_id,word reg,dword val,dword max))
{
	if(pgm_id!=-1)
         if(kx_set_dsp_register(hw,pgm_id,reg,calc_volume(hw,val,max)))
         {
           debug(DLIB,"!!! Control not found in kx_set_volume() (%x; pgm_id=%x)\n",reg,pgm_id);
           return -5;
         }
         return 0;
}

KX_API(int,kx_set_volume(kx_hw *hw,const char *pgm_id,const char *name,dword val,dword max))
{
  struct list *item;

  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->dsp_lock, &flags);

  for_each_list_entry(item, &hw->microcodes) 
  {
  	dsp_microcode *m;
        m = list_item(item, dsp_microcode, list);
        if(!m)
         continue;
        if(strncmp(m->name,pgm_id,KX_MAX_STRING)==NULL)
        {
         kx_lock_release(hw,&hw->dsp_lock,&flags);
         if(kx_set_dsp_register(hw,m->pgm,name,calc_volume(hw,val,max)))
         {
           debug(DLIB,"!!! Control not found in kx_set_volume() (%s; id=%s)\n",name,pgm_id);
           return -5;
         }
         return 0;
        }
 }
 kx_lock_release(hw,&hw->dsp_lock,&flags);

 return 0;
}

inline int upload_instruction(kx_hw *hw,dsp_microcode *m,int i)
{
   word z,w,x,y;
   dsp_register_info *tmp;
   int have_outputs=0;

   if(!(m->flag&MICROCODE_TRANSLATED))
   {
    debug(DLIB,"!!! uploading not-uploaded microcode [%x]\n",m->pgm);
    return -1;
   }

   if(is_register(m->code[i].r))
   {
      tmp=find_dsp_register_in_m(hw,m,m->code[i].r);
      if(!tmp || (tmp->translated==DSP_REG_NOT_TRANSLATED)) { debug(DLIB,"!! Internal error 1 in upload() %x\n",tmp?tmp->translated:0xdeadbeef); return -1; }
      z=tmp->translated;
      if((tmp->type&GPR_MASK)==GPR_OUTPUT)
      {
        have_outputs|=1;
      }
   }
   else z=m->code[i].r;

   if(is_register(m->code[i].a))
   {
       tmp=find_dsp_register_in_m(hw,m,m->code[i].a);
       if(!tmp || (tmp->translated==DSP_REG_NOT_TRANSLATED)) { debug(DLIB,"!! Internal error 2 in upload() %x\n",tmp?tmp->translated:0xdeadbeef); return -1; }
       w=tmp->translated;
       if((tmp->type&GPR_MASK)==GPR_OUTPUT)
       {
         have_outputs|=2;
       }
   }
   else w=m->code[i].a;

   if(is_register(m->code[i].x))
   {
       tmp=find_dsp_register_in_m(hw,m,m->code[i].x);
       if(!tmp || (tmp->translated==DSP_REG_NOT_TRANSLATED)) { debug(DLIB,"!! Internal error 3 in upload() %x\n",tmp?tmp->translated:0xdeadbeef); return -1; }
       x=tmp->translated;
       if((tmp->type&GPR_MASK)==GPR_OUTPUT)
       {
         have_outputs|=4;
       }
   }
   else x=m->code[i].x;

   if(is_register(m->code[i].y))
   {
        tmp=find_dsp_register_in_m(hw,m,m->code[i].y);
        if(!tmp || (tmp->translated==DSP_REG_NOT_TRANSLATED)) { debug(DLIB,"!! Internal error 4 in upload() %x\n",tmp?tmp->translated:0xdeadbeef); return -1; }
        y=tmp->translated;
        if((tmp->type&GPR_MASK)==GPR_OUTPUT)
        {
          have_outputs|=8;
        }
   }
   else y=m->code[i].y;

   if(m->offset!=DSP_MICROCODE_NOT_TRANSLATED)
   {
   	word operation=m->code[i].op;

        // perform some 'bypass' / 'mute' stuff here
        // transform 'any_op out, a, b, c' -> 'macs out, in, 0, 0'
        // transform 'any_op out, a, b, c' -> 'macs out, 0, 0, 0'
        if((m->flag&MICROCODE_BYPASS)||(!(m->flag&MICROCODE_ENABLED)))
        {
         if(have_outputs&1) // only 'z' operand is interesting for us...
         {
          operation=MACS;
          w=C_0; // just in case...
          x=C_0;
          y=C_0;

          if(m->flag&MICROCODE_BYPASS) // bypass
          {
           // search for the corresponding input...
           // UNEFFICIENT code FIXME
           int n_outs=0,n_ins=0;
           int this_out=-1;

           for(dword j=0;j<m->info_size/sizeof(dsp_register_info);j++)
           {
            if((m->info[j].type&GPR_MASK)==GPR_OUTPUT)
            {
             n_outs++;
             if(z==m->info[j].translated)
              this_out=n_outs;
            }
            if((m->info[j].type&GPR_MASK)==GPR_INPUT)
             n_ins++;
           }
		   
           if((n_outs>0) && (n_ins>0))
           {
            if(this_out>0)
            {
             // this_out is the number of the output; let it be the # of the input, too :)
             while(this_out>n_ins)
              this_out-=n_ins;

             n_ins=0;

             for(int j=0;(size_t)j<m->info_size/sizeof(dsp_register_info);j++)
             {
              if((m->info[j].type&GPR_MASK)==GPR_INPUT)
              {
               n_ins++;
               if(n_ins==this_out) // found
               {
                w=m->info[j].translated;
                break;
               }
              }
             }
             if(w==C_0)
              debug(DLIB,"!! internal error: empty input_gpr list. strange [%d]\n",m->pgm);

            } else debug(DLIB,"!! internal error in bypass/upload_instr() [pgm=%d]\n",m->pgm);
           } else debug(DLIB,"note: bypass incorrect (ins: %d outs: %d)\n",n_ins,n_outs);
          } // for bypass
         } // output is 'z'
        } // special cases?..

   	check_const(hw,z);
        check_const(hw,w);
        check_const(hw,x);
        check_const(hw,y);

   	hwOP(i+m->offset,operation,z,w,x,y);
   }
   else
   {
    debug(DLIB,"!! internal error II in upload()\n");
    return -2;
   }

   return 0;
}

inline int allocate_register(kx_hw *hw,dsp_microcode *m,int reg)
{
 if((m->info[reg].type&GPR_MASK)==GPR_INPUT ||
    (m->info[reg].type&GPR_MASK)==GPR_TRAMA)
 {
 	debug(DERR,"!!! (internal error):: allocate_register() :: input??\n");
 	return -21;
 }

 int from,to;

 // 0x100-0x1ff - TRAM flags

 // 10k2 GPRs: 0x400-0x5ff (-0x200 (!))
 if(hw->is_10k2)
 {
  from=0x200/32; // linked
  to=from+0x200/32;
 }
 else // GPR: 0x100.0x1ff (-0x100)
 {
  from=0;
  to=0x100/32; 
 }

 // internal TRAM: 0x200.0x27f (128 locations); (-0x100); 10k2: 192 locations
 int xtram_off=128;
 int xtram_count=32;
 if(hw->is_10k2)
 {
  xtram_off=192;
  xtram_count=64;
 }

 if((m->info[reg].type&GPR_MASK)==GPR_ITRAM)
 {
  from=0x100/32;
  to=from+xtram_off/32;
 }

 // external TRAM: 0x280..0x29f (32 locations); (-0x100); 10k2: 64 locations
 if((m->info[reg].type&GPR_MASK)==GPR_XTRAM)
 {
  from=0x100/32+xtram_off/32;
  to=from+xtram_count/32;
 }

 for(int i=from;i<to;i++)
  if(hw->fx_regs_usage[i]!=0xffffffff)
  {
       	for(int j=0;j<32;j++)
       	  if(!(get_bit(hw->fx_regs_usage,i*32+j)))
       	  {
       	  	set_bit(hw->fx_regs_usage,i*32+j);
       	  	m->info[reg].translated=i*32+j+0x100;
       	  	if(from==0x200/32) // 10k2 GPR register
       	  	 m->info[reg].translated+=0x100;
       	  	return 0;
       	  }
  	debug(DERR,"!!! (internal error) alocate_register() algorythm is fake\n");
  	return -11;
  }

 debug(DLIB,"(..) no more GPRs/TRAMs in allocate_register\n");
 return -10;
}

inline int free_register(kx_hw *hw,dsp_microcode *m,int reg)
{
	if((dword)reg>m->info_size/sizeof(dsp_register_info))
	{
		debug(DERR,"!! (internal): free_register:: reg=%xh (size=%d)\n",
		 reg,m->info_size/sizeof(dsp_register_info));
		return -2;
	}
	if(m->info[reg].translated!=DSP_REG_NOT_TRANSLATED &&
	   (m->info[reg].type&GPR_MASK)!=GPR_INPUT &&
	   (m->info[reg].type&GPR_MASK)!=GPR_TRAMA)
	{
	        if(is_valid_gpr(m->info[reg].translated))
	        {
	         if(m->info[reg].translated>=0x400) // 10k2 GPR II
                   clear_bit(hw->fx_regs_usage,m->info[reg].translated-0x200);
                 else
		   clear_bit(hw->fx_regs_usage,m->info[reg].translated-0x100);
		}
		m->info[reg].translated=DSP_REG_NOT_TRANSLATED;
		return 0;
	}
	else
	{
	 debug(DERR,"!!! (internal error) EFX::free_register() - free not allocated reg or input or TRAM (%x)\n",
	  m->info[reg].type);
	 return -1;
	}
}


inline int allocate_microcode(kx_hw *hw,dsp_microcode *m,int pos,int pos_pgm)
{
	int size=(int)(m->code_size/sizeof(dsp_code));

	if(m->code_size==0 || m->code[0].op==0xff)
	{
	 debug(DLIB,"!! note: allocate_microcode called for empty microcode\n");
	}

	int initial=0;
	int direction=1;
	int final=hw->microcode_size;
    int offset;

	if(pos!=KX_MICROCODE_ABSOLUTE)
	{
            if(pos_pgm!=0) // before or after pos_pgm
            {
               struct list *item;
               for_each_list_entry(item, &hw->microcodes) 
               {
                    dsp_microcode *mm;
                    mm = list_item(item, dsp_microcode, list);
                    if(!mm)
                     continue;
                    if(mm->pgm==pos_pgm)
                    {
                     if(pos==KX_MICROCODE_BEFORE)
                      final=mm->offset;
                     else
                      initial=mm->offset+mm->code_size-1;
                    }
               }
            }
            if(pos==KX_MICROCODE_BEFORE)
            {
                    direction=-1;
            }
            offset=initial;
            if(pos==KX_MICROCODE_BEFORE)
             offset=final-1;
        }
        else // absolute position
        {
            initial=pos_pgm;
            offset=initial;
            final=offset+size;

            if(offset>=hw->microcode_size)
            {
             debug(DLIB,"!! note: microcode absolute position is incorrect [%d]\n",offset);
             return -10;
            }
            if(final>hw->microcode_size)
            {
             debug(DLIB,"!! warning: incorrect microcode position [absolute]\n");
             final=hw->microcode_size;
            }
            if(initial>=hw->microcode_size)
            {
             debug(DLIB,"!! warning: incorrect microcode position [absolute2]\n");
             initial=0;
            }
        }

        while(1)
	{
		if(direction==1)
		{
		 if(offset>=final) break;
		}
		else
		{
		 if(offset<initial) break;
		}

		if(!(get_bit(hw->fx_microcode_usage,offset))) // found '0'
		{
			int i;
			for(i=0;i<size && ((offset+i)<hw->microcode_size);i++)
			 if(get_bit(hw->fx_microcode_usage,offset+i))
			  break;
			
			if(i==size) // found enough room
			{
				for(i=0;i<size;i++)
				 set_bit(hw->fx_microcode_usage,offset+i);
				m->offset=offset;
				return 0;
			}
			if(direction==1)
			 offset+=i-1;
                        // FIXME: else offset-= ???
		}
		offset+=direction;
	}
	debug(DLIB,"(..) no more microcode memory\n");
        return -10;
}

inline int free_microcode(kx_hw *hw,dsp_microcode *m)
{
	if(m->offset!=DSP_MICROCODE_NOT_TRANSLATED)
	{
		for(dword i=m->offset;i<m->offset+m->code_size/sizeof(dsp_code);i++)
		 clear_bit(hw->fx_microcode_usage,i);
		m->offset=DSP_MICROCODE_NOT_TRANSLATED;
	}
	return 0;
}

inline int disconnect_microcode(kx_hw *hw,dsp_microcode *src,word src_gpr,dsp_register_info *reg)
{
  if(!(hw->initialized&KX_DSP_INITED))
  {
   debug(DLIB,"EFX disconnect w/o being initialized\n");
   return -1;
  }

  if(reg)
  {
   if((reg->type&GPR_MASK)==GPR_INPUT) // try to disconnect input
   {
    if(strcmp(src->guid,"85e97848-0004-4006-a500-5a6a03b1bf09")!=0 &&
      strcmp(src->guid,"f88a3e59-ed54-4fb6-9b7d-4e213ed150f2")!=0 )
    {
        reg->translated=C_0;
        if(src->flag&MICROCODE_TRANSLATED)
        {
           for(dword cd=0;cd<src->code_size/sizeof(dsp_code);cd++)
           {
            if( (src->code[cd].a==reg->num) ||
                (src->code[cd].r==reg->num) ||
                (src->code[cd].x==reg->num) ||
                (src->code[cd].y==reg->num))
            {
             upload_instruction(hw,src,cd);
            }
           }
        }
    }
    return 0;
   }
   if((reg->type&GPR_MASK)!=GPR_OUTPUT)
   {
    debug(DLIB,"EFX disconnect(): parameter is neither input, nor output!\n");
    return -5;
   }
  }

 // for each uploaded microcode:
 struct list *item;
 for_each_list_entry(item, &hw->microcodes) 
 {
     dsp_microcode *m;
     m = list_item(item, dsp_microcode, list);
     if(!m)
       continue;
     if(!(m->flag&MICROCODE_TRANSLATED))
       continue;
     for(dword rg=0;rg<m->info_size/sizeof(dsp_register_info);rg++)
     {
      if((m->info[rg].type&GPR_MASK)==GPR_INPUT)
      {
        // find all inputs that were connected to this output
        if(m->info[rg].translated==src_gpr)
        {
           if(strcmp(m->guid,"85e97848-0004-4006-a500-5a6a03b1bf09")!=0 &&
             strcmp(m->guid,"f88a3e59-ed54-4fb6-9b7d-4e213ed150f2")!=0 )
           {
        	m->info[rg].translated=C_0;
        	for(dword cd=0;cd<m->code_size/sizeof(dsp_code);cd++)
        	{
        	 if( (m->code[cd].a==m->info[rg].num) ||
        	     (m->code[cd].r==m->info[rg].num) ||
                     (m->code[cd].x==m->info[rg].num) ||
                     (m->code[cd].y==m->info[rg].num))
                 {
                  upload_instruction(hw,m,cd);
                 }
        	}
           }	
        }
      }
     }
 }
 return 0;
}

KX_API(int,kx_disconnect_microcode(kx_hw *hw,int pgm,word src))
{
 dsp_microcode *src_m=NULL;
 dsp_register_info *source=NULL;
 word src_reg;

 if(pgm!=-1)
 {
  source=find_dsp_register(hw,pgm,src,&src_m);
  if(!source || !src_m ||
    ( ((source->type&GPR_MASK)!=GPR_OUTPUT) &&
      ((source->type&GPR_MASK)!=GPR_INPUT)
    ))
      {
      	debug(DLIB,"!! No such register pgm(src) in disconnect() (pgm:%d,reg:0x%x) or it is not an input/output reg\n",pgm,src);
   	return -1;
      }	
  src_reg=source->translated;
 }
 else
  src_reg=src;

 return disconnect_microcode(hw,src_m,src_reg,source);
}

KX_API(int,kx_disconnect_microcode(kx_hw *hw,int pgm,const char *src))
{
 dsp_register_info *source;
 source=find_dsp_register(hw,pgm,src,NULL);
 if(!source)
 {
  debug(DLIB,"!! disconnect: regs not found\n");
  return -1;
 }
 return kx_disconnect_microcode(hw,pgm,source->num);
}

inline int untranslate_microcode(kx_hw *hw,dsp_microcode *m)
{
        // clear all the code
        if(m->offset!=DSP_MICROCODE_NOT_TRANSLATED)
        {
          for(dword i=0;i<m->code_size/sizeof(dsp_code);i++)
       	  {
       		  hwOP(i+m->offset,ACC3,C_0-0x2000+(hw->is_10k2?0x80:0),
       		                        C_0-0x2000+(hw->is_10k2?0x80:0),
       		                        C_0-0x2000+(hw->is_10k2?0x80:0),
       		                        C_0-0x2000+(hw->is_10k2?0x80:0));
       	  }
          free_microcode(hw,m);
        }

        // clear all the GPRs
        for(dword i=0;i<m->info_size/sizeof(dsp_register_info);i++)
        {
        	if((m->info[i].type&GPR_MASK)==GPR_OUTPUT)
        	{
        		// we should 'disconnect' everything
        		if(is_valid_gpr(m->info[i].translated))
        		  disconnect_microcode(hw,m,m->info[i].translated,&m->info[i]);
        	}
                if((m->info[i].type&GPR_MASK)==GPR_INPUT)
                {
                	m->info[i].translated=C_0;
                }
       		if((m->info[i].translated!=DSP_REG_NOT_TRANSLATED) &&
       		   ((m->info[i].type&GPR_MASK)!=GPR_INPUT))
       		{
       		   // zero register, if it is not input (other pgm) and if it is translated
       		   if(is_valid_gpr(m->info[i].translated))
       		   {
       		      kx_writeptr(hw,m->info[i].translated,0,0);

                      // for TRAM registers:
                      if( ((m->info[i].type&GPR_MASK)==GPR_ITRAM) ||
                          ((m->info[i].type&GPR_MASK)==GPR_XTRAM) )
                      {
                        // clear control flag
       		        if(hw->is_10k2)
       		         kx_writeptr(hw,m->info[i].translated-0x100,0,0);
       		        // clear address register
       		        kx_writeptr(hw,m->info[i].translated+0x100,0,0);
       		      }
       		   }
       		   if((m->info[i].type&GPR_MASK)!=GPR_TRAMA) // trama is freed automatically
      		     free_register(hw,m,i);
       		}
        }
        m->flag&=~MICROCODE_TRANSLATED;
        m->flag&=~MICROCODE_ENABLED;
        m->flag&=~MICROCODE_BYPASS;

	return 0;
}

KX_API(int,kx_untranslate_microcode(kx_hw *hw,int pgm))
{
  if(!(hw->initialized&KX_DSP_INITED))
  {
   debug(DLIB,"EFX untranslate w/o being initialized\n");
   return -1;
  }

  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->dsp_lock, &flags);

  struct list *item;
  for_each_list_entry(item, &hw->microcodes) 
  {
  	dsp_microcode *m;
        m = list_item(item, dsp_microcode, list);
        if(!m)
         continue;
        if(m->pgm==pgm)
        {
            int ret=untranslate_microcode(hw,m);
            kx_lock_release(hw,&hw->dsp_lock,&flags);
            return ret;
        }
  }

  kx_lock_release(hw,&hw->dsp_lock,&flags);

  debug(DLIB,"!! no microcode to untranslate (%d)\n",pgm);

  return -1; // not found
}

inline int kx_translate_microcode(kx_hw *hw,dsp_microcode *m,int place,int pos_pgm)
{
 m->itram_start=0;
 m->xtram_start=0;

 // handle one special case: FXBUS microcode:
 if(strcmp(m->guid,"2b8b7fa8-98b9-4f6e-81a0-400d3ba39c6f")==0)
 {
 	for(word i=0;i<m->info_size/sizeof(dsp_register_info);i++)
 	{
 		m->info[i].translated=i;
 	}
 }
  else
 // handle one special case: FXBUS2 microcode:
 if(strcmp(m->guid,"131f1059-f384-4403-abd0-16ef6025bb9b")==0)
 {
 	for(word i=0;i<m->info_size/sizeof(dsp_register_info);i++)
 	{
 		m->info[i].translated=i+32;
 	}
 }
  else
 // handle one special case: FXBUSx microcode:
 if(strcmp(m->guid,"d25a7874-7c00-47ca-8ad3-1b13106bde91")==0)
 {
    // 3544:
 	for(word i=16;i<32;i++)
 	{
 		m->info[i].translated=i;
 	}
 }
  else
 if(strcmp(m->guid,"06f1854e-8e8f-465f-8d9c-966cfcc20dc7")==0) // prolog lite
 {
 	for(word i=0;i<m->info_size/sizeof(dsp_register_info);i++)
 	{
 		m->info[i].translated=KX_IN(i);
 	}
 }
  else
 if(strcmp(m->guid,"85e97848-0004-4006-a500-5a6a03b1bf09")==0) // epilog k1 lite
 {
 	for(word i=0;i<m->info_size/sizeof(dsp_register_info);i++)
 	{
 		m->info[i].translated=KX_OUT(i);
 	}
 }
  else
 if(strcmp(m->guid,"f88a3e59-ed54-4fb6-9b7d-4e213ed150f2")==0) // epilog k2 lite
 {
  if(m->info_size/sizeof(dsp_register_info)==0x29)
  {
	  word i;
 	for(i=0;i<0x19;i++)
 	{
 		m->info[i].translated=0x60+i;
 	}
 	for(i=0x19;i<0x29;i++)
 	{
 		m->info[i].translated=KX_FX2(i-0x19);
 	}
  }
   else
    debug(DWDM,"kx: incorrect epilog2 lite microcode [%d]\n",m->info_size/sizeof(dsp_register_info));
 }

 if(m->itramsize!=0) // need itram?
 {
     int found=1;
     int start=0;
     int end=m->itramsize;
     //debug(DLIB,"iTram request: %d samples\n",m->itramsize);

       while(1)
       {
       	     struct list *item;
             for_each_list_entry(item, &hw->microcodes) 
             {
              	    dsp_microcode *mm;
                    mm = list_item(item, dsp_microcode, list);
                    if(!mm)
                     continue;
                    if(!(mm->flag&MICROCODE_TRANSLATED))
                     continue;
                    if((mm->itram_start==0) && (mm->itramsize==0))
                     continue;
                    if( ( (start>=mm->itram_start) && (start<=(mm->itram_start+mm->itramsize)) ) ||
                        ( (end>=mm->itram_start) && (end<=(mm->itram_start+mm->itramsize)) )|| 
                        ( (start<=mm->itram_start) && (end>=mm->itram_start+mm->itramsize)) )
                    {
                     // intersection
                     found=0;
                     break;
                    }
             }
             if(found==0) // intersection
             {
               start+=16;
               end+=16;
               found=1;
             } else break; // found
             if(end>=8192) // FIXME: check with 10k2?..
             {
             	debug(DLIB,"no more free iTRAM memory\n");
             	return -1;
             }
       } // while 1
       //debug(DLIB,"iTram request: found %d samples @ %x\n",m->itramsize,start);
       m->itram_start=start;
 } // need itram?..

 if(m->xtramsize!=0) // need xtram?
 {
     int found=1;
     int start=0;
     int end=m->xtramsize;

       //debug(DLIB,"xTram request: %d samples\n",m->xtramsize);
       while(1)
       {
             struct list *item;
             for_each_list_entry(item, &hw->microcodes) 
             {
              	    dsp_microcode *mm;
                    mm = list_item(item, dsp_microcode, list);
                    if(!mm)
                     continue;
                    if(!(mm->flag&MICROCODE_TRANSLATED))
                     continue;
                    if((mm->xtram_start==0) && (mm->xtramsize==0))
                     continue;
                    if( ( (start>=mm->xtram_start) && (start<=(mm->xtram_start+mm->xtramsize)) ) ||
                        ( (end>=mm->xtram_start) && (end<=(mm->xtram_start+mm->xtramsize)) ) || 
                        ( (start<=mm->xtram_start) && (end>=mm->xtram_start+mm->xtramsize)) )
                    {
                     // intersection
                     found=0;
                     break;
                    }
             }
             if(found==0) // intersection
             {
               start+=512;
               end+=512;
               found=1;
             } else break; // found
             if(end>=hw->cb.tram_size/2)
             {
             	debug(DLIB,"no more free xTRAM memory (end=%d size: %x)\n",end,hw->cb.tram_size);
             	return -2;
             }
       } // while 1
       m->xtram_start=start;
       //debug(DLIB,"xTram request: %d samples @%x\n",m->xtramsize,start);
 } // need xtram?..

 dword i;

 // find all !translated registers & try to add them;
 for(i=0;i<m->info_size/sizeof(dsp_register_info);i++)
 {
  if(m->info[i].translated==DSP_REG_NOT_TRANSLATED)
  {
  	switch(m->info[i].type&GPR_MASK)
  	{
  		case GPR_INPUT:	// try to translate unconnected microcode
  			m->info[i].translated=C_0; // 0x40+(hw->is_10k2?0x80:0);
  			break;
                case GPR_STATIC:
                case GPR_CONST:
                case GPR_TEMP:
                case GPR_CONTROL:
                case GPR_OUTPUT:
                	if(allocate_register(hw,m,i))
                	{
                		debug(DLIB,"!! Translation of register %d failed\n",
                		  i);
                		untranslate_microcode(hw,m);
                		return -3;
                	}
                        if(is_valid_gpr(m->info[i].translated))
                	  kx_writeptr(hw,m->info[i].translated,0,
                	    m->info[i].p);
                	break;
                case GPR_ITRAM:
                case GPR_XTRAM:
                	if((m->info[i+1].type&GPR_MASK)!=GPR_TRAMA)
                	{
                	 debug(DLIB,"!!! TRAM data w/o TRAM addr: i=%d; m->info:%x.%s; type=%x; +1: %x.%s [type=%x]\n",
                	   i,m->info[i].num,m->info[i].name,m->info[i].type,
                	     m->info[i+1].num,m->info[i+1].name,m->info[i+1].type);
                	 untranslate_microcode(hw,m);
                	 return -4;
                	}
                	if(allocate_register(hw,m,i))
                	{
                		debug(DLIB,"!! Translation of register %d failed (in TRAM)\n",
                		  i);
                		untranslate_microcode(hw,m);
                		return -5;
                	}
                        if(is_valid_gpr(m->info[i].translated))
                        {
                          m->info[i+1].translated=m->info[i].translated+0x100;

                          if(hw->is_10k2)
                          {
                           kx_writeptr_multiple(hw,0,
                             m->info[i].translated,0,
                             m->info[i+1].translated,0,
                             m->info[i].translated-0x100,0,
                             m->info[i].translated,0,
                             m->info[i+1].translated,0,
                             m->info[i].translated-0x100,0,
                             REGLIST_END);
                          }
                          else
                          {
                           kx_writeptr_multiple(hw,0,
                             m->info[i].translated,0,
                             m->info[i+1].translated,0,
                             m->info[i].translated,0, // black magic?
                             m->info[i+1].translated,0,
                             REGLIST_END);
                          }
                          dword fl=0;
                          if(m->info[i].type&TRAM_READ) // NB! if neither read, nor write - do not use clear
                            fl=TRAM_CLEAR;

                          set_tram_addr(hw,m,&m->info[i],m->info[i].p);
                          set_tram_flag(hw,m,&m->info[i],fl|(m->info[i].type&(TRAM_READ|TRAM_WRITE)));
                          i++;
                	}
                	else
                	{
                		debug(DLIB,"!! TRAM was never translated\n");
                		untranslate_microcode(hw,m);
                		return -6;
                	}
                	break;
                case GPR_TRAMA:
                	debug(DLIB,"!!! TRAM addr without TRAM data reg\n");
                        untranslate_microcode(hw,m);
                        return -4;
                case 0:
                	// dummy register
                	m->info[i].translated=C_0;
                	break;
                default:
                	debug(DERR,"!!! (internal error) kx_translate_microcode(inlined): default switch\n");
                        untranslate_microcode(hw,m);
                	return -4;
  	}
        if(m->info[i].translated==DSP_REG_NOT_TRANSLATED)
        {
        	debug(DERR,"!!! (internal error): register still untranslated!\n");
                untranslate_microcode(hw,m);
        	return -6;
        }
  }
 }

 // now, set m->offset
 if((m->offset==DSP_MICROCODE_NOT_TRANSLATED) && (m->code_size>0))
  if(allocate_microcode(hw,m,place,pos_pgm))
  {
 	debug(DLIB,"!!! No free mem in microcode memory for current microcode [place=%d; pos=%d]\n",place,pos_pgm);
 	untranslate_microcode(hw,m);
 	return -7;
  }

 m->flag|=MICROCODE_TRANSLATED;

 for(i=0;i<m->code_size/sizeof(dsp_code);i++)
 {
 	upload_instruction(hw,m,i);
 }

 return 0;
}

KX_API(int,kx_translate_microcode(kx_hw *hw,int pgm,int place,int pos_pgm))
{
  if(!(hw->initialized&KX_DSP_INITED))
  {
   debug(DLIB,"EFX translate w/o being initialized\n");
   return -1;
  }

  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->dsp_lock, &flags);

  struct list *item;
  for_each_list_entry(item, &hw->microcodes) 
  {
  	dsp_microcode *m;
        m = list_item(item, dsp_microcode, list);
        if(!m)
         continue;
        if(m->pgm==pgm)
        {
            int ret=kx_translate_microcode(hw,m,place,pos_pgm);
            kx_lock_release(hw,&hw->dsp_lock,&flags);
            return ret;
        }
  }

  kx_lock_release(hw,&hw->dsp_lock,&flags);

  debug(DLIB,"!! no microcode to translate (%d)\n",pgm);

  return -1; // not found
}


KX_API(int,kx_enable_microcode(kx_hw *hw,int pgm))
{
  if(!(hw->initialized&KX_DSP_INITED))
  {
   debug(DLIB,"EFX enable w/o being initialized\n");
   return -1;
  }

  struct list *item;

  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->dsp_lock, &flags);

  for_each_list_entry(item, &hw->microcodes) 
  {
  	dsp_microcode *m;
        m = list_item(item, dsp_microcode, list);
        if(!m)
         continue;
        if(m->pgm==pgm)
        {
            if((!(m->flag&MICROCODE_ENABLED))&&(m->flag&MICROCODE_TRANSLATED))
            {
            	m->flag|=MICROCODE_ENABLED;
            	m->flag&=(~MICROCODE_BYPASS);

                for(dword i=0;i<m->code_size/sizeof(dsp_code);i++)
                {
                  dsp_register_info *tmp1,*tmp2,*tmp3,*tmp4;
                  tmp1=find_dsp_register_in_m(hw,m,m->code[i].r);
                  tmp2=find_dsp_register_in_m(hw,m,m->code[i].a);
                  tmp3=find_dsp_register_in_m(hw,m,m->code[i].x);
                  tmp4=find_dsp_register_in_m(hw,m,m->code[i].y);

                  if(tmp1 && (tmp1->translated!=DSP_REG_NOT_TRANSLATED) && ((tmp1->type&GPR_MASK)==GPR_OUTPUT))
                     upload_instruction(hw,m,i);
                  else
                  if(tmp2 && (tmp2->translated!=DSP_REG_NOT_TRANSLATED) && ((tmp2->type&GPR_MASK)==GPR_OUTPUT))
                     upload_instruction(hw,m,i);
                  else
                  if(tmp3 && (tmp3->translated!=DSP_REG_NOT_TRANSLATED) && ((tmp3->type&GPR_MASK)==GPR_OUTPUT))
                     upload_instruction(hw,m,i);
                  else
                  if(tmp4 && (tmp4->translated!=DSP_REG_NOT_TRANSLATED) && ((tmp4->type&GPR_MASK)==GPR_OUTPUT))
                     upload_instruction(hw,m,i);
            	}
            }
            kx_lock_release(hw,&hw->dsp_lock,&flags);
            return 0;
        }
  }

  kx_lock_release(hw,&hw->dsp_lock,&flags);

  debug(DLIB,"!! no microcode to enable (%d)\n",pgm);

  return -1; // not found
}

KX_API(int,kx_disable_microcode(kx_hw *hw,int pgm))
{
  if(!(hw->initialized&KX_DSP_INITED))
  {
   debug(DLIB,"EFX disable w/o being initialized\n");
   return -1;
  }

  struct list *item;

  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->dsp_lock, &flags);

  for_each_list_entry(item, &hw->microcodes) 
  {
  	dsp_microcode *m;
        m = list_item(item, dsp_microcode, list);
        if(!m)
         continue;
        if(m->pgm==pgm)
        {
            if((m->flag&MICROCODE_ENABLED) && (m->offset!=DSP_MICROCODE_NOT_TRANSLATED))
            {
                        m->flag&=(~MICROCODE_ENABLED);
                        m->flag&=(~MICROCODE_BYPASS);

                        for(dword i=0;i<m->code_size/sizeof(dsp_code);i++)
                        {
                          dsp_register_info *tmp1,*tmp2,*tmp3,*tmp4;
                          tmp1=find_dsp_register_in_m(hw,m,m->code[i].r);
                          tmp2=find_dsp_register_in_m(hw,m,m->code[i].a);
                          tmp3=find_dsp_register_in_m(hw,m,m->code[i].x);
                          tmp4=find_dsp_register_in_m(hw,m,m->code[i].y);

                          if(tmp1 && (tmp1->translated!=DSP_REG_NOT_TRANSLATED) && ((tmp1->type&GPR_MASK)==GPR_OUTPUT))
                             upload_instruction(hw,m,i);
                          else
                          if(tmp2 && (tmp2->translated!=DSP_REG_NOT_TRANSLATED) && ((tmp2->type&GPR_MASK)==GPR_OUTPUT))
                             upload_instruction(hw,m,i);
                          else
                          if(tmp3 && (tmp3->translated!=DSP_REG_NOT_TRANSLATED) && ((tmp3->type&GPR_MASK)==GPR_OUTPUT))
                             upload_instruction(hw,m,i);
                          else
                          if(tmp4 && (tmp4->translated!=DSP_REG_NOT_TRANSLATED) && ((tmp4->type&GPR_MASK)==GPR_OUTPUT))
                             upload_instruction(hw,m,i);
                    	}
            }
            kx_lock_release(hw,&hw->dsp_lock,&flags);
            return (m->flag&MICROCODE_ENABLED)?-20:0;
        }
  }

  kx_lock_release(hw,&hw->dsp_lock,&flags);

  debug(DLIB,"!! no microcode to disable (%d)\n",pgm);

  return -1; // not found
}

KX_API(int,kx_set_microcode_bypass(kx_hw *hw,int pgm,int state))
{
  if(!(hw->initialized&KX_DSP_INITED))
  {
   debug(DLIB,"EFX bypass w/o being initialized\n");
   return -1;
  }

  struct list *item;

  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->dsp_lock, &flags);

  for_each_list_entry(item, &hw->microcodes) 
  {
  	dsp_microcode *m;
        m = list_item(item, dsp_microcode, list);
        if(!m)
         continue;
        if(m->pgm==pgm)
        {
            if((m->offset!=DSP_MICROCODE_NOT_TRANSLATED) && (m->flag&MICROCODE_ENABLED))
            {
                // check 'no bypass' option
                if(strstr(m->comment,"$nobypass")!=0)
                {
                 if(state)
                  debug(DLIB,"Cannot 'bypass' system microcode(s)\n");

                  m->flag&=(~MICROCODE_BYPASS);

                  kx_lock_release(hw,&hw->dsp_lock,&flags);
                  return -30;
                }

            	if(state)
                   m->flag|=MICROCODE_BYPASS;
                else
                   m->flag&=(~MICROCODE_BYPASS);

                        for(dword i=0;i<m->code_size/sizeof(dsp_code);i++)
                        {
                          dsp_register_info *tmp1,*tmp2,*tmp3,*tmp4;
                          tmp1=find_dsp_register_in_m(hw,m,m->code[i].r);
                          tmp2=find_dsp_register_in_m(hw,m,m->code[i].a);
                          tmp3=find_dsp_register_in_m(hw,m,m->code[i].x);
                          tmp4=find_dsp_register_in_m(hw,m,m->code[i].y);

                          if(tmp1 && (tmp1->translated!=DSP_REG_NOT_TRANSLATED) && ((tmp1->type&GPR_MASK)==GPR_OUTPUT))
                             upload_instruction(hw,m,i);
                          else
                          if(tmp2 && (tmp2->translated!=DSP_REG_NOT_TRANSLATED) && ((tmp2->type&GPR_MASK)==GPR_OUTPUT))
                             upload_instruction(hw,m,i);
                          else
                          if(tmp3 && (tmp3->translated!=DSP_REG_NOT_TRANSLATED) && ((tmp3->type&GPR_MASK)==GPR_OUTPUT))
                             upload_instruction(hw,m,i);
                          else
                          if(tmp4 && (tmp4->translated!=DSP_REG_NOT_TRANSLATED) && ((tmp4->type&GPR_MASK)==GPR_OUTPUT))
                             upload_instruction(hw,m,i);
                    	}
            }
            kx_lock_release(hw,&hw->dsp_lock,&flags);
            if(state)
             return (m->flag&MICROCODE_BYPASS)?0:-20;
            else
             return (m->flag&MICROCODE_BYPASS)?-20:0;
        }
  }

  kx_lock_release(hw,&hw->dsp_lock,&flags);

  debug(DLIB,"!! no microcode to set bypass for (%d)\n",pgm);

  return -1; // not found
}

KX_API(int,kx_set_microcode_flag(kx_hw *hw,int pgm,dword flag))
{
  if(!(hw->initialized&KX_DSP_INITED))
  {
   debug(DLIB,"EFX get_flag w/o being initialized\n");
   return -1;
  }

  struct list *item;

  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->dsp_lock, &flags);

  for_each_list_entry(item, &hw->microcodes) 
  {
  	dsp_microcode *m;
        m = list_item(item, dsp_microcode, list);
        if(!m)
         continue;
        if(m->pgm==pgm)
        {
        	kx_lock_release(hw,&hw->dsp_lock,&flags);

                // NOTE: not very safe: (microcode might get modified between calls)
                int ret=0;

        	// setting flag...
        	if((flag&MICROCODE_TRANSLATED) && (!(m->flag&MICROCODE_TRANSLATED)))
        	 ret+=kx_untranslate_microcode(hw,pgm);

        	if((!(flag&MICROCODE_TRANSLATED)) && (m->flag&MICROCODE_TRANSLATED))
        	 ret+=kx_translate_microcode(hw,pgm);

        	if((flag&MICROCODE_ENABLED) && (!(m->flag&MICROCODE_ENABLED)))
        	 ret+=kx_disable_microcode(hw,pgm);

        	if((!(flag&MICROCODE_ENABLED)) && (m->flag&MICROCODE_ENABLED))
        	 ret+=kx_enable_microcode(hw,pgm);

        	if((flag&MICROCODE_BYPASS) && (!(m->flag&MICROCODE_BYPASS)))
        	 ret+=kx_set_microcode_bypass(hw,pgm,0);

        	if((!(flag&MICROCODE_BYPASS)) && (m->flag&MICROCODE_BYPASS))
        	 ret+=kx_set_microcode_bypass(hw,pgm,1);

        	m->flag=flag;
        	
        	return ret;
        }
  }

  kx_lock_release(hw,&hw->dsp_lock,&flags);

  debug(DLIB,"!! no microcode to set_flag (%d)\n",pgm);

  return -1; // not found
}

KX_API(int,kx_get_microcode_flag(kx_hw *hw,int pgm,dword *flag))
{
  if(!(hw->initialized&KX_DSP_INITED))
  {
   debug(DLIB,"EFX get_flag w/o being initialized\n");
   return -1;
  }

  struct list *item;

  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->dsp_lock, &flags);

  for_each_list_entry(item, &hw->microcodes) 
  {
  	dsp_microcode *m;
        m = list_item(item, dsp_microcode, list);
        if(!m)
         continue;
        if(m->pgm==pgm)
        {
        	if(flag)
        	 *flag=m->flag;
        	kx_lock_release(hw,&hw->dsp_lock,&flags);
        	return 0;
        }
  }

  kx_lock_release(hw,&hw->dsp_lock,&flags);

  debug(DLIB,"!! no microcode to get_flag (%d)\n",pgm);

  return -1; // not found
}


// returns pgm id or 0 if failed
KX_API(int,kx_load_microcode(kx_hw *hw,const char *name,dsp_code *code2,int code_size,
   dsp_register_info *info2,int info_size,
   int itramsize,int xtramsize,const char *copyright,const char *engine,const char *created,const char *comment,const char *guid,
   int force_pgm_id))
{
  if(!(hw->initialized&KX_DSP_INITED))
  {
   debug(DLIB,"EFX load w/o being initialized\n");
   return -1;
  }

 dsp_code *code=NULL;
 dsp_register_info *info=NULL;
 dsp_microcode *microcode=NULL;  
 int pgm=0;
 int i;

 (hw->cb.malloc_func)(hw->cb.call_with,sizeof(dsp_microcode),(void **)&microcode,KX_NONPAGED);
 if(microcode)
 {
   if(code_size)
     (hw->cb.malloc_func)(hw->cb.call_with,code_size,(void **)&code,KX_NONPAGED);
   if((code_size==0) || code)
   {
     if(info_size)
       (hw->cb.malloc_func)(hw->cb.call_with,info_size,(void **)&info,KX_NONPAGED);
     if((info_size==0) || info)
     {
     	 unsigned long flags=0;
     	 kx_lock_acquire(hw,&hw->dsp_lock,&flags);

         memset(microcode,0,sizeof(dsp_microcode));
         if(code) 
          memcpy(&code[0],&code2[0],code_size);
         if(info)
          memcpy(&info[0],&info2[0],info_size);

         if(name) strncpy(microcode->name,name,KX_MAX_STRING);
         if(copyright) strncpy(microcode->copyright,copyright,KX_MAX_STRING);
         if(engine) strncpy(microcode->engine,engine,KX_MAX_STRING);
         if(created) strncpy(microcode->created,created,KX_MAX_STRING);
         if(comment) strncpy(microcode->comment,comment,KX_MAX_STRING);
         if(guid) strncpy(microcode->guid,guid,KX_MAX_STRING);
         microcode->code=&code[0];
         microcode->code_size=code_size;
         microcode->info=&info[0];
         microcode->info_size=info_size;

         microcode->itramsize=itramsize;
         microcode->xtramsize=xtramsize;

         if(force_pgm_id==0)
         {
                 // find free pgm number
                 for(i=1;i<MAX_PGM_NUMBER;i++)
                 {
                 	struct list *item;
                        pgm=i;         	
        	 	for_each_list_entry(item, &hw->microcodes) 
        	 	{
        	 		dsp_microcode *m;
                                m = list_item(item, dsp_microcode, list);
                                if(!m)
                                 continue;
                                if(pgm==m->pgm) // found duplicate
                                 pgm=0;
        	 	}
        	 	if(pgm!=0) // found free
        	 	  break;
        	 }
         }
          else pgm=force_pgm_id;


         if(pgm!=0) // found free pgms         
         {
	        // insert into microcode list
         	microcode->pgm=pgm;
         	list_add(&microcode->list, &hw->microcodes);

         	for(dword j=0;j<microcode->info_size/sizeof(dsp_register_info);j++)
         	  microcode->info[j].translated=DSP_REG_NOT_TRANSLATED;

         	microcode->offset=DSP_MICROCODE_NOT_TRANSLATED;

                kx_lock_release(hw,&hw->dsp_lock,&flags);
         	return pgm;
         }

         kx_lock_release(hw,&hw->dsp_lock,&flags);
     }
   }
 }
 // no spin here
 if(info)
   (hw->cb.free_func)(hw->cb.call_with,info);
 if(code)
   (hw->cb.free_func)(hw->cb.call_with,code);
 if(microcode)
   (hw->cb.free_func)(hw->cb.call_with,microcode);
 return -1;
}


KX_API(int,kx_unload_microcode(kx_hw *hw,int pgm))
{
  if(!(hw->initialized&KX_DSP_INITED))
  {
   debug(DLIB,"EFX unload w/o being initialized\n");
   return -1;
  }

  struct list *item;

  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->dsp_lock, &flags);

  for_each_list_entry(item, &hw->microcodes) 
  {
  	dsp_microcode *m;
        m = list_item(item, dsp_microcode, list);
        if(!m)
         continue;
        if(m->pgm==pgm)
        {
            untranslate_microcode(hw,m);

            list_del(&m->list);
            kx_lock_release(hw,&hw->dsp_lock,&flags);

            // AFTER spin_release
            if(m->info)
              (hw->cb.free_func)(hw->cb.call_with,m->info);
            if(m->code)
              (hw->cb.free_func)(hw->cb.call_with,m->code);

            (hw->cb.free_func)(hw->cb.call_with,m);

            return 0;
        }
  }

  kx_lock_release(hw,&hw->dsp_lock,&flags);

  return -1; // not found
}

KX_API(int,kx_connect_microcode(kx_hw *hw,int pgm1,word src,int pgm2,word dst))
{
  if(!(hw->initialized&KX_DSP_INITED))
  {
   debug(DLIB,"EFX connect w/o being initialized\n");
   return -1;
  }

 dsp_register_info *source=NULL;
 dsp_register_info *destination=NULL;
 dsp_register_info *process=NULL;
 dsp_microcode *src_m=NULL,*dst_m=NULL,*m=NULL; // one to be modified
 word dst_reg=0,src_reg=0,process_reg=0;
 int src_type=-1,dst_type=-1;

 int check_input=0;

 if(pgm1!=-1)
 {
  source=find_dsp_register(hw,pgm1,src,&src_m);
  if(!source || !src_m ||
    ( ((source->type&GPR_MASK)!=GPR_INPUT) &&
      ((source->type&GPR_MASK)!=GPR_OUTPUT) ) )
      {
      	debug(DLIB,"!! No such register pgm1(source) in connect() (pgm:%d,reg:0x%x) or it is not input/output reg\n",pgm1,src);
   	return -1;
      }	
  src_reg=source->translated;
  src_type=source->type&GPR_MASK;

  if(src_m)
   if(strcmp(src_m->guid,"85e97848-0004-4006-a500-5a6a03b1bf09")==0 ||
      strcmp(src_m->guid,"f88a3e59-ed54-4fb6-9b7d-4e213ed150f2")==0 )
   {
      src_type=-1;
      src=src_reg; // transform 'logical' src into physical 'translated'
      check_input=1;
   }
 }
 else
  src_reg=src;

 if(pgm2!=-1)
 {
  destination=find_dsp_register(hw,pgm2,dst,&dst_m);
  if(!destination || !dst_m ||
    ( ((destination->type&GPR_MASK)!=GPR_INPUT) &&
      ((destination->type&GPR_MASK)!=GPR_OUTPUT) ) )
      {
      	debug(DLIB,"!! No such register pgm2(destination) in connect() (pgm:%d,reg:0x%x) or it is not input/output reg [from %d,0x%x]\n",pgm2,dst,pgm1,src);
   	return -2;
      }	
  dst_reg=destination->translated;
  dst_type=destination->type&GPR_MASK;

  if(dst_m)
   if(strcmp(dst_m->guid,"85e97848-0004-4006-a500-5a6a03b1bf09")==0 ||
      strcmp(dst_m->guid,"f88a3e59-ed54-4fb6-9b7d-4e213ed150f2")==0 )
   {
      dst_type=-1;
      dst=dst_reg; // transform 'logical' dst into physical 'translated'
      check_input=1;
   }
 }
 else
  dst_reg=dst;

 if(src_type==-1)
 {
  if(dst_type!=GPR_OUTPUT && dst_type!=GPR_INPUT) // src is physical; dst should be in or out
  {
 	debug(DLIB,"!! connect: both are -1 (%d:0x%x - %d:0x%x)\n",pgm1,src,pgm2,dst);
 	return -3;
  }
  process=destination;
  process_reg=src; // src is physical
  m=dst_m;
 }
 else
 if(dst_type==-1)
 {
  if(src_type!=GPR_OUTPUT && src_type!=GPR_INPUT) // dst is physical; src should be in or out
  {
  	debug(DLIB,"!! connect: both are -1 (%d:0x%x - %d:0x%x) ii\n",pgm1,src,pgm2,dst);
 	return -4;
  }
  process=source;
  process_reg=dst; // dst is physical
  m=src_m;
 }
 else
 if(src_type==GPR_INPUT)
 {
  if(dst_type!=GPR_OUTPUT)
  {
 	debug(DLIB,"!! trying to connect non input-output pair (%d:0x%x -> %d:0x%x)\n",
 	 pgm1,src,pgm2,dst);
 	return -5;
  }
  process=source;
  process_reg=dst_reg;
  m=src_m;
 }
 else
 if(src_type==GPR_OUTPUT)
 {
  if(dst_type!=GPR_INPUT)
  {
 	debug(DLIB,"!! trying to connect non input-output pair (%d:0x%x -> %d:0x%x)\n",
 	 pgm1,src,pgm2,dst);
 	return -6;
  }
  process=destination;
  process_reg=src_reg;
  m=dst_m;
 }

 if(check_input)
 {
  if(strcmp(m->guid,"06f1854e-8e8f-465f-8d9c-966cfcc20dc7")==0 ||       // prologlt
     strcmp(m->guid,"2b8b7fa8-98b9-4f6e-81a0-400d3ba39c6f")==0 ||       // fxbus
     strcmp(m->guid,"131f1059-f384-4403-abd0-16ef6025bb9b")==0 ||         // fxbus2
     strcmp(m->guid,"2b8b7fa8-98b9-4f6e-81a0-400d3ba39c6f")==0      // fxbusx
    )
  {
       debug(DLIB,"!! cannot connect fxbus/prologlt directly to epiloglt!\n");
       return -11;
  }
 }

 if(process_reg!=DSP_REG_NOT_TRANSLATED)
 {
   if((process->type&GPR_MASK)==GPR_OUTPUT) // now, we can free previous GPR
   {
    if(is_valid_gpr(process->translated))
    {
		dword j;
      for(j=0;j<m->info_size/sizeof(dsp_register_info);j++)
      {
       if(m->info[j].num==process->num)
       {
  	  free_register(hw,m,j);
  	  break;
       }
      }
	  
      if(j==m->info_size/sizeof(dsp_register_info))
      {
       debug(DLIB,"!! error in dsp_connect: num=%d type=%d pgm=%d\n",
         process->num,process->type,m->pgm);
       debug(DLIB," [dst_reg=%x src_reg=%x dst_type=%x src_type=%x process_reg=%x\n",
        dst_reg,src_reg,dst_type,src_type,process_reg);
       if(process)
        debug(DLIB," [process: num:%x translated: %x type: %x\n",
         process->num,process->translated,process->type);
      }
    } 
   }
   process->translated=process_reg;

   if(m->flag&MICROCODE_TRANSLATED) // microcode is already in emu10kx
   {
      // now, scan input microcode for changes
      unsigned long flags=0;
      kx_lock_acquire(hw,&hw->dsp_lock, &flags);

      for(dword i=0;i<m->code_size/sizeof(dsp_code);i++) 
      {
        if( (m->code[i].r==process->num) ||
            (m->code[i].a==process->num) ||
            (m->code[i].x==process->num) ||
            (m->code[i].y==process->num) )
        {
        	upload_instruction(hw,m,i);
        }
      }

      kx_lock_release(hw,&hw->dsp_lock, &flags);
   }
   return 0;
 }
 debug(DLIB,"!! destination not resolved (%d %x; %d %x)\n",pgm1,src,pgm2,dst);
 debug(DLIB," [dst_reg=%x src_reg=%x dst_type=%x src_type=%x process_reg=%x\n",
  dst_reg,src_reg,dst_type,src_type,process_reg);
 if(process)
  debug(DLIB," [process: num:%x translated: %x type: %x\n",
   process->num,process->translated,process->type);

 return -10; // output was not translated
}

KX_API(int,kx_connect_microcode(kx_hw *hw,int pgm1,const char *src,int pgm2,const char *dst))
{
  if(!(hw->initialized&KX_DSP_INITED))
  {
   debug(DLIB,"EFX connect w/o being initialized\n");
   return -1;
  }

 word src_num=0xffff,dst_num=0xffff;
 if(pgm1==-1)
 {
  src_num=(word)(uintptr_t)src;
 }
 else
 {
  dsp_register_info *source;
  source=find_dsp_register(hw,pgm1,src,NULL);
  if(source) src_num=source->num;
 }

 if(pgm2==-1)
 {
  dst_num=(word)(uintptr_t)dst;
 }
 else
 {
  dsp_register_info *dest;
  dest=find_dsp_register(hw,pgm2,dst,NULL);
  if(dest) dst_num=dest->num;
 }

 if((src_num==0xffff) || (dst_num==0xffff))
 {
  debug(DLIB,"!! connect: regs not found (pgm1: %d; (%s); pgm2: %d; (%s)\n",
   pgm1,pgm1==-1?"(phys)":src,pgm2,pgm2==-1?"(phys)":dst);
  return -1;
 }

 return kx_connect_microcode(hw,pgm1,src_num,pgm2,dst_num);
}

KX_API(int,kx_connect_microcode(kx_hw *hw,int pgm1,const char *src,int pgm2,word dst_n))
{
  if(!(hw->initialized&KX_DSP_INITED))
  {
   debug(DLIB,"EFX connect w/o being initialized\n");
   return -1;
  }

 word src_num=0xffff;
 if(pgm1==-1)
 {
  src_num=(word)(uintptr_t)src;
 }
 else
 {
  dsp_register_info *source;
  source=find_dsp_register(hw,pgm1,src,NULL);
  if(source) src_num=source->num;
 }

 if(src_num==0xffff)
 {
  debug(DLIB,"!! connect: regs not found (pgm1: %d; (%s); pgm2: %d; (%x)\n",
   pgm1,pgm1==-1?"(phys)":src,pgm2,dst_n);
  return -1;
 }

 return kx_connect_microcode(hw,pgm1,src_num,pgm2,dst_n);
}


KX_API(int,kx_enum_microcode(kx_hw *hw,int pgm,dsp_microcode *mc))
{
  if(!(hw->initialized&KX_DSP_INITED))
  {
   debug(DLIB,"EFX enum w/o being initialized\n");
   return -1;
  }

  struct list *item;

  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->dsp_lock, &flags);

  for_each_list_entry(item, &hw->microcodes) 
  {
  	dsp_microcode *m;
        m = list_item(item, dsp_microcode, list);
        if(!m)
         continue;
        if(m->pgm==pgm)
        {
            memcpy(mc,m,sizeof(dsp_microcode));
            mc->code=NULL;
            mc->info=NULL;
            memset(&mc->list,0,sizeof(mc->list));

            kx_lock_release(hw,&hw->dsp_lock,&flags);
			
			// debug(DLIB,"enum microcode: %s, %s; %d",mc->name,mc->guid,mc->flag);

            return 0;
        }
  }

  kx_lock_release(hw,&hw->dsp_lock,&flags);

  return -1; // not found
}

KX_API(int,kx_enum_microcode(kx_hw *hw,char *pgm_id,dsp_microcode *mc))
{
  if(!(hw->initialized&KX_DSP_INITED))
  {
   debug(DLIB,"EFX enum w/o being initialized\n");
   return -1;
  }

  struct list *item;

  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->dsp_lock, &flags);

  for_each_list_entry(item, &hw->microcodes) 
  {
  	dsp_microcode *m;
        m = list_item(item, dsp_microcode, list);
        if(!m)
         continue;
        if(strncmp(m->name,pgm_id,KX_MAX_STRING)==NULL)
        {
            memcpy(mc,m,sizeof(dsp_microcode));
            mc->code=NULL;
            mc->info=NULL;
            memset(&mc->list,0,sizeof(mc->list));

            kx_lock_release(hw,&hw->dsp_lock,&flags);

            return 0;
        }
  }

  kx_lock_release(hw,&hw->dsp_lock,&flags);

  return -1; // not found
}

KX_API(int,kx_enum_microcode(kx_hw *hw,dsp_microcode *mc_ret,int size))
{
  if(!(hw->initialized&KX_DSP_INITED))
  {
   debug(DLIB,"EFX enum w/o being initialized\n");
   return -1;
  }
  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->dsp_lock, &flags);
  int total=0;

  struct list *item;
  for_each_list_entry(item, &hw->microcodes) 
  {
  	dsp_microcode *mc;
        mc = list_item(item, dsp_microcode, list);
        total++; // just count soundfonts
  }

  if(size==0) // just tell how much mem is needed
  {
   kx_lock_release(hw,&hw->dsp_lock,&flags);
   return (int)(total*sizeof(dsp_microcode));
  }

  if(((dword)size!=total*sizeof(dsp_microcode)) || (!mc_ret)) // not match
  {
   kx_lock_release(hw,&hw->dsp_lock,&flags);
   return -1;
  }

  total=0;
  for_each_list_entry(item, &hw->microcodes)
  {
  	dsp_microcode *mc;
        mc = list_item(item, dsp_microcode, list);
        memcpy(&mc_ret[total],mc,sizeof(dsp_microcode));
        total++;
  }

  kx_lock_release(hw,&hw->dsp_lock,&flags);
  return 0;
}


KX_API(int,kx_get_microcode(kx_hw *hw,int pgm,dsp_code *code,int code_size,
		dsp_register_info *info,int info_size))
{
  struct list *item;

  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->dsp_lock, &flags);

  for_each_list_entry(item, &hw->microcodes) 
  {
  	dsp_microcode *m;
        m = list_item(item, dsp_microcode, list);
        if(!m)
         continue;
        if(m->pgm==pgm)
        {
            if(code_size!=m->code_size)
            {
             kx_lock_release(hw,&hw->dsp_lock,&flags);
             return -2;
            }
            if(info_size!=m->info_size)
            {
             kx_lock_release(hw,&hw->dsp_lock,&flags);
             return -3;
            }
            memcpy(code,m->code,code_size);
            memcpy(info,m->info,info_size);

            kx_lock_release(hw,&hw->dsp_lock,&flags);
            return 0;
        }
  }

  kx_lock_release(hw,&hw->dsp_lock,&flags);

  return -1; // not found
}

KX_API(int,kx_get_routing(kx_hw *hw,int id,dword *routing,dword *xrouting))
{
	if(id<0 || id>ROUTING_LAST)
	 return -1;
        if(routing)
         *routing=hw->cb.def_routings[id];
        if(xrouting)
         *xrouting=hw->cb.def_xroutings[id];
        return 0;
}

KX_API(int,kx_set_routing(kx_hw *hw,int id,dword routing,dword xrouting))
{
	if(id<0 || id>ROUTING_LAST)
	{
	 debug(DLIB,"kx: set_routing invalid [%d.%x.%x]\n",id,routing,xrouting);
	 return -1;
	}
        hw->cb.def_routings[id]=routing;
        hw->cb.def_xroutings[id]=xrouting;

        if(((routing&0xff)==0xff)||
           ((routing&0xff00)==0xff00)||
           ((routing&0xff0000)==0xff0000)||
           ((routing&0xff000000)==0xff000000))
        {
         debug(DLIB,"kx: set_routing invalid [%d.%x.%x]\n",id,routing,xrouting);
        }

        for(int i=0;i<KX_NUMBER_OF_VOICES;i++)
        {
           if(!hw->voicetable[i].usage) // free voice
            continue;

           if((hw->voicetable[i].usage&VOICE_USAGE_ASIO) && hw->voicetable[i].asio_id) // asio
           {
        	if(hw->voicetable[i].asio_channel==(dword)(id-DEF_ASIO_ROUTING))
        	{
    		  kx_wave_set_parameter(hw,i,0,KX_VOICE_ROUTINGS,routing);
    		  kx_wave_set_parameter(hw,i,0,KX_VOICE_XROUTINGS,xrouting);
        	}
           }	
	   else
	    if((hw->voicetable[i].usage&VOICE_USAGE_MIDI) && hw->voicetable[i].synth_id) // synth
	    {
	     kx_midi_state *midi=(kx_midi_state *)hw->voicetable[i].synth_id;
	     if( (midi->synth_num==0) && ((id-DEF_SYNTH1_ROUTING)==hw->voicetable[i].channel)) // 1
	     {
    	      kx_wave_set_parameter(hw,i,0,KX_VOICE_ROUTINGS,routing);
    	      kx_wave_set_parameter(hw,i,0,KX_VOICE_XROUTINGS,xrouting);

              debug(DLIB,"(remove): updating routing for %d synth1 / %d hw channel %x %x\n",
        	   hw->voicetable[i].channel,i,routing,xrouting);
	     }
	     else
	     if( (midi->synth_num==1) && ((id-DEF_SYNTH2_ROUTING)==hw->voicetable[i].channel)) // 2
	     {
    	      kx_wave_set_parameter(hw,i,0,KX_VOICE_ROUTINGS,routing);
    	      kx_wave_set_parameter(hw,i,0,KX_VOICE_XROUTINGS,xrouting);

              debug(DLIB,"(remove): updating routing for %d synth2 / %d hw channel %x %x\n",
        	   hw->voicetable[i].channel,i,routing,xrouting);
	     }
	    }
        }
        return 0;
}
KX_API(int,kx_get_fx_amount(kx_hw *hw,int id,byte *amount))
{
	if(id<0 || id>AMOUNT_LAST)
	 return -1;
        *amount=hw->cb.def_amount[id];
        return 0;
}

KX_API(int,kx_set_fx_amount(kx_hw *hw,int id,byte amount))
{
	if(id<0 || id>AMOUNT_LAST)
	 return -1;

    hw->cb.def_amount[id]=amount;

    return 0;
}

KX_API(int,kx_dsp_stop(kx_hw *hw))
{
 if(hw->is_10k2)
  kx_writeptr(hw,DBG_10K2,0,DBG_10K2_SINGLE_STEP);
 else
  kx_writeptr(hw,DBG_10K1,0,DBG_10K1_SINGLE_STEP);
  return 0;
}

KX_API(int,kx_dsp_go(kx_hw *hw))
{
   if(hw->is_10k2)
    kx_writeptr(hw, DBG_10K2, 0, 0); 
   else
    kx_writeptr(hw, DBG_10K1, 0, 0);
   return 0;
}

KX_API(int,kx_dsp_clear(kx_hw *hw))
{
 if(!(hw->initialized&KX_DSP_INITED))
 {
   debug(DLIB,"!!! dsp_clear() w/o being inited\n");
   return -5;
 } 

 kx_dsp_stop(hw);
 int i;
 for(i=0;i<MAX_PGM_NUMBER;i++)
  kx_unload_microcode(hw,i);

 if(hw->microcodes.next!=&hw->microcodes || hw->microcodes.prev!=&hw->microcodes)
 {
  debug(DLIB,"!!! dsp_clear: microcode list not empty!\n");
 }

 // clear microcode
 for(i = 0; i < hw->microcode_size; i++)
 {
 	hwOP(i,ACC3,C_0-0x2000+(hw->is_10k2?0x80:0),
 	            C_0-0x2000+(hw->is_10k2?0x80:0),
 	            C_0-0x2000+(hw->is_10k2?0x80:0),
 	            C_0-0x2000+(hw->is_10k2?0x80:0));
 }


 // clear GPRs, TRAMs...
 for(i = 0; i < 256; i++)
 {
 	kx_writeptr(hw, E10K1_GPR_BASE+i,0,0);
 	kx_writeptr(hw, TANKMEMADDRREGBASE+i,0,0);
 	kx_writeptr(hw, E10K1_GPR_BASE+i,0,0);
 	kx_writeptr(hw, TANKMEMADDRREGBASE+i,0,0);
 }


 if(hw->is_10k2)
 {
  for(i=0; i<512; i++)
   kx_writeptr(hw, E10K2_GPR_BASE+i, 0, 0);
 }

 dword j;
 for(j=0;j<FX_REGISTER_MASSIVE_SIZE;j++)
  if(hw->fx_regs_usage[j]!=0)
   debug(DLIB,"!!! FX register @%x not freed\n",j);

 for(j=0;j<FX_MICROCODE_MASSIVE_SIZE;j++)
  if(hw->fx_microcode_usage[j]!=0)
   debug(DLIB,"!!! FX microcode @%x not freed\n",j);

 memset(&hw->fx_regs_usage[0],0,sizeof(hw->fx_regs_usage));
 memset(&hw->fx_microcode_usage[0],0,sizeof(hw->fx_microcode_usage));

 return 0;
}

KX_API(int,kx_set_microcode_name(kx_hw *hw,int pgm_id,const char *str,int what))
{
  if(!(hw->initialized&KX_DSP_INITED))
  {
   debug(DLIB,"EFX set_name w/o being initialized\n");
   return -1;
  }

  struct list *item;

  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->dsp_lock, &flags);

  for_each_list_entry(item, &hw->microcodes) 
  {
  	dsp_microcode *m;
        m = list_item(item, dsp_microcode, list);
        if(!m)
         continue;
        if(m->pgm==pgm_id)
        {
            switch(what)
            {
             case KX_MICROCODE_NAME: strncpy(m->name,str,KX_MAX_STRING); break;
             case KX_MICROCODE_COPYRIGHT: strncpy(m->copyright,str,KX_MAX_STRING); break;
             case KX_MICROCODE_COMMENT: strncpy(m->comment,str,KX_MAX_STRING); break;
             case KX_MICROCODE_ENGINE: strncpy(m->engine,str,KX_MAX_STRING); break;
             case KX_MICROCODE_CREATED: strncpy(m->created,str,KX_MAX_STRING); break;
             case KX_MICROCODE_GUID: strncpy(m->guid,str,KX_MAX_STRING); break;
            }

            kx_lock_release(hw,&hw->dsp_lock,&flags);

            return 0;
        }
  }

  kx_lock_release(hw,&hw->dsp_lock,&flags);

  return -1; // not found
}

KX_API(int,kx_get_connections(kx_hw *hw,int pgm,kxconnections *out,int size))
{
  if(!(hw->initialized&KX_DSP_INITED))
  {
   debug(DLIB,"EFX get_connections() w/o being initialized\n");
   return -1;
  }

     int needed_size=0;
     dsp_microcode *m=0;

     struct list *item;
     unsigned long flags=0;
     kx_lock_acquire(hw,&hw->dsp_lock, &flags);

     for_each_list_entry(item, &hw->microcodes) 
     {
        m = list_item(item, dsp_microcode, list);
        if(!m)
            continue;
        if(m->pgm==pgm)
        {
         if(!(m->flag&MICROCODE_TRANSLATED))
         {
           kx_lock_release(hw,&hw->dsp_lock,&flags);
           debug(DLIB,"get_connections(): microcode not translated[%d]\n",pgm);
           return -4;
         }

         for(dword i=0;i<m->info_size/sizeof(dsp_register_info);i++)
         {
          if((m->info[i].type&GPR_MASK)==GPR_INPUT)
              needed_size+=(int)sizeof(kxconnections);
         }

         if(size==0)
         {
           kx_lock_release(hw,&hw->dsp_lock,&flags);
           return needed_size;
         }

         if(size!=needed_size)
         {
           kx_lock_release(hw,&hw->dsp_lock,&flags);
           debug(DLIB,"get_connections(): size=%d != %d\n",size,needed_size);
           return -5;
         }

         break;
        }
     }
     kx_lock_release(hw,&hw->dsp_lock,&flags);
     if(needed_size==0)
     {
      debug(DLIB,"get_connections(): bad pgm_id [%d]\n",pgm);
      return -1; // bad pgm_id
     }

     int cnt=0;

     for(dword i=0;i<m->info_size/sizeof(dsp_register_info);i++)
     {
      if((m->info[i].type&GPR_MASK)==GPR_INPUT)
      {
       out[cnt].this_num=m->info[i].num;
       out[cnt].pgm_id=-1;
       out[cnt].num=C_0;
       if(is_valid_gpr(m->info[i].translated))
       {
          kx_lock_acquire(hw,&hw->dsp_lock, &flags);

          dsp_microcode *m2=0;

          for_each_list_entry(item, &hw->microcodes) 
          {
             m2 = list_item(item, dsp_microcode, list);
             if(!m2)
                 continue;
             if(m2->flag&MICROCODE_TRANSLATED)
             {
              for(dword j=0;j<m2->info_size/sizeof(dsp_register_info);j++)
              {
               if((m2->info[j].translated==m->info[i].translated)&&((m2->info[j].type&GPR_MASK)==GPR_OUTPUT))
               {
                out[cnt].pgm_id=m2->pgm;
                out[cnt].num=m2->info[j].num;
               }
              }
             }
          }
          if(out[cnt].pgm_id==-1)
           debug(DLIB,"!! get_connections(): incorrect connection (%x; %x = %x)\n",pgm,m->info[i].num,m->info[i].translated);

          kx_lock_release(hw,&hw->dsp_lock,&flags);
       }
       else
       {
        out[cnt].pgm_id=-1;
        out[cnt].num=m->info[i].translated;
       }
       cnt++;
      }
     }

     return 0;
}


#define my_alloc(hw,ret,sz)  (hw->cb.malloc_func)(hw->cb.call_with,sz,(void **)ret,KX_NONPAGED)
#define my_free(hw,h) (hw->cb.free_func)(hw->cb.call_with,h)

KX_API(int,kx_dsp_reload_epilog(kx_hw *hw))
{
    	const char *microcode_name;
    	int last_gpr=0x8000;
    	int last_input_gpr=0x4000;
    	int itramsize=0;
    	int xtramsize=0;
    	int code_size=0;
    	int info_size=0;
    	dsp_code *dyn_code=0; dsp_register_info *dyn_info=0;
    	int cur_info=0;
    	int cur_code=0;

        int is_aps=hw->is_aps;
        int is_10k2=hw->is_10k2;
        dword dsp_flags=hw->dsp_flags;
        int is_51=hw->is_51;
        int can_passthru=hw->can_passthru;
        dword is_doo=0;
        int is_bad_sb22x=hw->is_bad_sb22x;
		int is_edsp=hw->is_edsp;
        // int is_k8=hw->is_k8;
		int output_r=0;
        int input_r=0;

        kx_get_hw_parameter(hw,KX_HW_DOO,&is_doo);

        int need_spdif_swap=0;

        if((hw->is_a2 || (hw->is_a2==0 && hw->cb.subsys==0x00521102)) && !hw->is_k8 && !hw->is_edsp)
        {
         // any audigy2-based board + sb0160 [a1]
         // FIXME !! need to check if is_k8 are affected, too
         need_spdif_swap=1;
        }

        if(hw->ext_flags&KX_HW_FORCE_SPDIF_SWAP)
         need_spdif_swap=1-need_spdif_swap;

             struct list *item;

             unsigned long flags=0;
             kx_lock_acquire(hw,&hw->dsp_lock, &flags);

             for_each_list_entry(item, &hw->microcodes) 
             {
             	dsp_microcode *m;
                   m = list_item(item, dsp_microcode, list);
                   if(!m)
                    continue;

                   if(strcmp(m->guid,"ceffc302-ea28-44df-873f-d3df1ba31736")==0) // epilog
                   {
                    #define EPILOG_SOURCE
					#define EPILOG_DESCR

					// int output_r=0;
                    // int input_r=0;
					   
                	#include "legacy/prepare.h"
                	begin_microcode("epilog");
                    #include "../kxfxlib/da_epilog.cpp"
                	end_microcode();

                	#include "legacy/process.h"
                	begin_microcode("epilog");
                    #include "../kxfxlib/da_epilog.cpp"

                   	if(m->code_size==code_size) // ok
                   	{
                   	 memcpy(m->code,dyn_code,code_size);
                   	 for(dword i=0;i<m->code_size/sizeof(dsp_code);i++)
                   	 {
                   	 	upload_instruction(hw,m,i);
                   	 }
                   	 break;
                   	}
                   	else
                   	 debug(DLIB,"!!! new epilog size mismatch!\n");
                   	break;
                   }
             }
             kx_lock_release(hw,&hw->dsp_lock,&flags);

	my_free(hw,dyn_code);
	my_free(hw,dyn_info);

 return 0;
}

KX_API(int,kx_translate_text2id(kx_hw *hw,kx_text2id *ti))
{
 if(ti)
  switch(ti->what)
  {
   case KX_TEXT2ID:
   	// enum all effects. search for 'pgm_name'
   	{
      debug(DWDM,"! kx_translate_text2id: not implemented\n");
   	}
   	break;
   case KX_ID2TEXT:
   	// enum all effects. search for 'pgm_name'
   	{
            struct list *item;

            unsigned long flags=0;
            kx_lock_acquire(hw,&hw->dsp_lock, &flags);

            for_each_list_entry(item, &hw->microcodes) 
            {
                  dsp_microcode *m;
                  m = list_item(item, dsp_microcode, list);
                  if(!m)
                   continue;
                  if(m->pgm==ti->pgm)
                  {
                   // store the name
                   strncpy(ti->pgm_name,m->name,KX_MAX_STRING);

                   for(dword i=0;i<m->info_size/sizeof(dsp_register_info);i++)
                    if(m->info[i].num==ti->reg)
                    {
                     strncpy(ti->reg_name,m->info[i].name,KX_MAX_STRING); // note: reg_name is KX_MAX_STRING, but m->info[i].name is only MAX_GPR_NAME

                     kx_lock_release(hw,&hw->dsp_lock,&flags);
                     return 0;
                    }
                  }
            }
            // not found
            kx_lock_release(hw,&hw->dsp_lock,&flags);
            return -3;
   	}
   	break;
   default:
    return -2;
  }
 return -1;
}

KX_API(int,kx_update_microcode(kx_hw *hw,int pgm_id,const char *name,dsp_code *code,int code_size,
   		dsp_register_info *info,int info_size,int itramsize,int xtramsize,
   		const char *copyright,
   		const char *engine,
   		const char *created,
   		const char *comment,
   		const char *guid,
   		unsigned int flag))
{
 debug(DLIB,"update microcode: pgm: %d; name: '%s'; code/info: %d/%d; flag: %x\n",
  pgm_id,name,code_size,info_size,flag);

  if(!(hw->initialized&KX_DSP_INITED))
  {
   debug(DLIB,"EFX update w/o being initialized\n");
   return -1;
  }

  struct list *item;

  unsigned long flags=0;
  kx_lock_acquire(hw,&hw->dsp_lock, &flags);

  for_each_list_entry(item, &hw->microcodes) 
  {
  	dsp_microcode *m;
        m = list_item(item, dsp_microcode, list);
        if(!m)
         continue;
        if(m->pgm==pgm_id)
        {
            // UPDATE HERE
            if((flag&IKX_UPDATE_NAME) && name) strncpy(m->name,name,KX_MAX_STRING);
            if((flag&IKX_UPDATE_COPYRIGHT) && copyright) strncpy(m->copyright,copyright,KX_MAX_STRING);
            if((flag&IKX_UPDATE_ENGINE) && engine) strncpy(m->engine,engine,KX_MAX_STRING);
            if((flag&IKX_UPDATE_CREATED) && created) strncpy(m->created,created,KX_MAX_STRING);
            if((flag&IKX_UPDATE_COMMENT) && comment) strncpy(m->comment,comment,KX_MAX_STRING);
            if((flag&IKX_UPDATE_GUID) && guid) strncpy(m->guid,guid,KX_MAX_STRING);

            kx_lock_release(hw,&hw->dsp_lock,&flags);

            // FIXME: this is incorrect kx_lock_release

            // save 'ins' the connections:
            kxconnections ins[64];
            int size=kx_get_connections(hw,pgm_id,NULL,0);

            if(flag&IKX_UPDATE_DSP)
            {
                if(size>=0 && (size_t)size<sizeof(ins) && (kx_get_connections(hw,pgm_id,ins,size)==0))
                {
                 // get 'outs' connections
                 // ... FIXME

                 // save stats
                 dword old_flag=m->flag;

                 kx_untranslate_microcode(hw,pgm_id);

                 if((flag&IKX_UPDATE_CODE) && code)
                 {
                    if(m->code)
                     (hw->cb.free_func)(hw->cb.call_with,m->code);
                    m->code=0;
                    m->code_size=code_size;
                    if(code_size)
                      (hw->cb.malloc_func)(hw->cb.call_with,code_size,(void **)&m->code,KX_NONPAGED);
                    if(m->code)
                     memcpy(m->code,code,code_size);
                 }
                 if((flag&IKX_UPDATE_REGS) && info)
                 {
                    m->info=0;
                    m->info_size=info_size;
                    if(m->info)
                     (hw->cb.free_func)(hw->cb.call_with,m->info);
                    if(info_size)
                      (hw->cb.malloc_func)(hw->cb.call_with,info_size,(void **)&m->info,KX_NONPAGED);
                    if(m->info)
                     memcpy(m->info,info,info_size);
                 }

                 if(flag&IKX_UPDATE_RESOURCES)
                 {
                   m->itramsize=itramsize;
                   m->xtramsize=xtramsize;
                 }

                 if((!(old_flag&MICROCODE_TRANSLATED)) || (kx_translate_microcode(hw,m,KX_MICROCODE_ANY,0)==0) )
                 {
                    // restore connections
                    if(old_flag&MICROCODE_TRANSLATED)
                     for(unsigned int i=0;i<size/sizeof(kxconnections);i++)
                     {
                      kx_connect_microcode(hw,pgm_id,ins[i].this_num,
                      		ins[i].pgm_id,ins[i].num);
                     }

                    if(old_flag&MICROCODE_ENABLED)
                     kx_enable_microcode(hw,pgm_id);

                    if(old_flag&MICROCODE_BYPASS)
                     kx_set_microcode_bypass(hw,pgm_id,1);

                    // kx_lock_release(hw,&hw->dsp_lock,&flags);

                    return 0; // ok

                 } else debug(DLIB,"error re-translating microcode\n");
                }
                 else debug(DLIB,"error saving microcode connections\n");

                 // kx_lock_release(hw,&hw->dsp_lock,&flags);
                 // FIXME (see above -- invalid spinlock)

                 return -1; // failure
            } // IKX_UPDATE_DSP?

            // kx_lock_release(hw,&hw->dsp_lock,&flags);

            return 0; // ok -- no need to update the DSP
        }
  }

  debug(DLIB,"invalid microcode # to update (%d)\n",pgm_id);

  kx_lock_release(hw,&hw->dsp_lock,&flags);

  return -2; // not found
}

// cannot use separate source, since ac3pt.cpp uses certain 'inlines'...
#include "ac3pt.cpp"

