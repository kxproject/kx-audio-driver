// kX Driver Interface
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

#include "stdafx.h"

#ifdef _MSC_VER
#pragma warning(disable:4244)
#endif

#if defined(WIN32)
	#define dsp_alloc(a) LocalAlloc(LMEM_FIXED|LMEM_ZEROINIT,a)
	#define dsp_free(a) LocalFree(a)
#elif defined(__APPLE__)
	#define dsp_alloc(a) malloc(a)
	#define dsp_free(a) free(a)
#endif

typedef struct
{
 char name[KX_MAX_STRING];
 char engine[KX_MAX_STRING];
 char engine_ver[KX_MAX_STRING];
 char copyright[KX_MAX_STRING];
 char created[KX_MAX_STRING];
 char comment[KX_MAX_STRING];

 #define MAX_REGS	0x600
 struct regs
 {
   word num;
   char name[KX_MAX_STRING];
   dword value;
   dword value2;
   word flag;
   word position;
   int type;
 }regs[MAX_REGS];
 int last;

 int warning;
 int error;

 int n_inputs;
 int n_outputs;
 int n_statics;
 int n_itrams;
 int n_xtrams;
 int n_temps;

 int itramsize;
 int xtramsize;

 word xtra;

 int code_size;
 byte code[E10K2_MAX_INSTRUCTIONS*9];

 int gprs_size,tram_size,inp_size,outp_size;
}patch_info;

#define TYPE_INPUT	0x1
#define TYPE_OUTPUT	0x2
#define TYPE_STATIC	0x3
#define TYPE_STATIC_I	0x4
#define TYPE_TRAM	0x5
#define TYPE_INTR	0x6
#define TYPE_TEMP	0x7
#define TYPE_CONTROL	0x8

#define TYPE_XTRAM	0x10

void parse_const(word &a);
void parse_const(word &a)
{
             if((a&0xf000)==0x2000) // 10k2 RIFX const
             {
              switch(a)
              {
              	case 0x2000: a=(word)C_0; break;
              	case 0x2001: a=(word)C_1; break;
              	case 0x2002: a=(word)C_2; break;
              	case 0x2003: a=(word)C_3; break;
              	case 0x2004: a=(word)C_4; break;
              	case 0x2008: a=(word)C_8; break;
                case 0x200b: a=(word)(C_1F); break; // 10k2 only
                case 0x2011: a=(word)(C_80000); break; // actually 0x800 for k2
              	case 0x2014: a=(word)C_00100000; break;
              	case 0x2019: a=(word)C_10000000; break;
              	case 0x201a: a=(word)C_20000000; break;
              	case 0x201c: a=(word)C_40000000; break;
              	case 0x201e: a=(word)C_4f1bbcdc; break;
              	case 0x201f: a=(word)C_5a7ef9db; break;
              	case 0x2020: a=(word)C_7fffffff; break;
              	case 0x2021: a=(word)C_80000000; break;
              	case 0x2025: a=(word)C_ffffffff; break;
              	case 0x2200: a=(word)CCR; break;
              	case 0x2201: a=(word)IRQREG; break;
                case 0x2202: a=(word)ACCUM; break;
                case 0x2203: a=(word)NOISE1; break; // FIXME: check it
                case 0x2204: a=(word)NOISE2; break; // FIXME: check it
                case 0x2205: a=(word)DBAC; break;
                default:
                	debug("kxrifx: unknown rifx [%x]\n",a);
              }
             } else 
             {
              if(a>=0x40 && a<=0x5b)
                a=a+KX_CONST;
              else
               if((a>=0x10)&&(a<0x20)) // logical inputs:
               {
                a=KX_IN(a-0x10);
               }
               else
                if((a>=0x20)&&(a<0x30)) // logical outputs
                {
                 a=KX_OUT(a-0x20);
                }
             }
}

static void my_itoa (
        unsigned long val,
        char *buf,
        unsigned radix
        )
{
        char *p;                /* pointer to traverse string */
        char *firstdig;         /* pointer to first digit */
        char temp;              /* temp char */
        unsigned digval;        /* value of digit */

        p = buf;

        firstdig = p;           /* save pointer to first digit */

        do {
            digval = (unsigned) (val % radix);
            val /= radix;       /* get next digit */

            /* convert to ascii and store */
            if (digval > 9)
                *p++ = (char) (digval - 10 + 'a');  /* a letter */
            else
                *p++ = (char) (digval + '0');       /* a digit */
        } while (val > 0);

        /* We now have the digit of the number in the buffer, but in reverse
           order.  Thus we reverse them now. */

        *p-- = '\0';            /* terminate string; p points to last digit */

        do {
            temp = *p;
            *p = *firstdig;
            *firstdig = temp;   /* swap *p and *firstdig */
            --p;
            ++firstdig;         /* advance to next two digits */
        } while (firstdig < p); /* repeat until halfway */
}

static void add_reg(patch_info &patch,word num,int type,const char *name,word position,dword value,dword value2=0,word flag=0)
{
 char tmp[32];
 patch.last++;
 patch.regs[patch.last].num=num;
 patch.regs[patch.last].type=type;

 strcpy(patch.regs[patch.last].name,name);
 my_itoa(type==TYPE_OUTPUT?num-0x8000:num,tmp,16);
 strcat(patch.regs[patch.last].name,tmp);

 patch.regs[patch.last].position=position;
 patch.regs[patch.last].value=value;
 patch.regs[patch.last].value2=value2;
 patch.regs[patch.last].flag=flag;

 if(type==TYPE_TRAM)
 {
   patch.last++;
   num++;
   patch.regs[patch.last].num=num;
   patch.regs[patch.last].type=type;

   strcpy(patch.regs[patch.last].name,"&");
   strcat(patch.regs[patch.last].name,name);
   
   my_itoa((type==TYPE_OUTPUT?num-0x8000:num)-1,tmp,16);
   strcat(patch.regs[patch.last].name,tmp);

   patch.regs[patch.last].position=position;
   patch.regs[patch.last].value=value;
   patch.regs[patch.last].value2=value2;
   patch.regs[patch.last].flag=flag;
 }
}

static int find_reg(word num,patch_info &patch)
{
 for(int i=1;i<=patch.last;i++)
  if(patch.regs[i].num==num)
   return i;
 return -1;
}

#define get_dword(a) (\
 ((*(unsigned char *)((a) + 3))<<0) + \
 ((*(unsigned char *)((a) + 2))<<8) + \
 ((*(unsigned char *)((a) + 1))<<16) + \
 ((*(unsigned char *)((a) + 0))<<24) )

#define get_word(a) (\
 ((*(unsigned char *)((a) + 1))<<0) + \
 ((*(unsigned char *)((a) + 0))<<8) )

#define get_byte(a) (*(unsigned char *)(a))

// ret:0 - ok
// ret:1 - container (do not skip)
// <0 - error
static int parse_tag(char *m,patch_info &patch)
{
     dword size=0;
     int i;

     if(strncmp(m,"ckid",4) ==0) 
     { 
	      strncpy(patch.name,m+8+strlen(m+8)+1+strlen(m+8+strlen(m+8)+1)+1,KX_MAX_STRING);
	      strncpy(patch.engine,m+8,KX_MAX_STRING);
	      strncpy(patch.engine_ver,m+8+strlen(m+8)+1,KX_MAX_STRING);
     } else

     if(strncmp(m,"rsrc",4) ==0) 
     { 
	      if(get_dword(m+4)!=0x1a)
	      {
	       patch.error=-200;
	       return -1;
	      }
	      patch.code_size=get_word(m+8);
	      patch.n_inputs=get_word(m+10);
	      patch.n_outputs=get_word(m+12);
	      patch.n_statics=get_word(m+14);
	      patch.n_itrams=get_word(m+16);
	      patch.n_xtrams=get_word(m+18);
	      patch.n_temps=get_word(m+20);
	      patch.itramsize=get_dword(m+22);
	      patch.xtramsize=get_dword(m+26);
     } else
     if(strncmp(m,"gprs",4) ==0) 
     { 
       size=get_dword(m+4);
       patch.gprs_size=size;

       for(i=0;i<(int)size;i+=6)
       {
           add_reg(patch,get_word(m+8+i),TYPE_STATIC_I,"sti",0x0,get_dword(m+8+i+2));
       }
     } else
     if(strncmp(m,"tram",4) ==0) 
     { 
       size=get_dword(m+4);
       patch.tram_size=size;

       for(i=0;i<(int)size;i+=12)
       {
           add_reg(patch,get_byte(m+8+i+1)+0x8000,TYPE_TRAM,"itr",
                       get_word(m+8+i+10), // pos
                       get_dword(m+8+i+2), // offset
                       get_dword(m+8+i+6), // size
                       get_byte(m+8+i)); // flag
       }
     } else
     if(strncmp(m,"xtra",4) ==0) 
     { 
       size=get_dword(m+4);
       patch.xtra=get_word(m+8);
       if(size!=2)
        patch.warning|=1;
     } else
     if(strncmp(m,"inp ",4) ==0) 
     { 
       size=get_dword(m+4);
       patch.inp_size=size;

       for(i=0;i<(int)size;i+=2)
       {
           add_reg(patch,0x4000+i/2,TYPE_INPUT,"in",get_word(m+8+i),0);
        }
     } else
     if(strncmp(m,"intr",4) ==0) 
     { 
       size=get_dword(m+4);
       for(i=0;i<(int)size;i+=2)
       {
        add_reg(patch,get_word(m+8+i),TYPE_INTR,"intr",0,0);
        }
     } else
     if(strncmp(m,"outp",4) ==0) 
     { 
       size=get_dword(m+4);
       patch.outp_size=size;

       for(i=0;i<(int)size;i+=4)
       {
        add_reg(patch,get_word(m+8+i),TYPE_OUTPUT,"out",
          get_dword(m+8+i+2),0);
       }
     } else
     if(strncmp(m,"wscl",4) ==0) 
     { 
       size=get_dword(m+4);
       for(i=0;i<(int)size;i+=2)
       {
        add_reg(patch,get_word(m+8+i),TYPE_CONTROL,"wet",0,0);
        }
     } else
     if(strncmp(m,"ICOP",4) ==0) 
     { 
     		strncpy(patch.copyright,m+8,KX_MAX_STRING);
     } else
     if(strncmp(m,"ICRD",4) ==0) 
     {
     		strncpy(patch.created,m+8,KX_MAX_STRING);
     } else
     if(strncmp(m,"ICMT",4) ==0)
     {
	     	strncpy(patch.comment,m+8,KX_MAX_STRING);
     } else
     if(strncmp(m,"code",4) == 0)
     { 
       size=get_dword(m+4);
       memcpy(patch.code,m+8,size);
     } else
     if(strncmp(m,"iscl",4) == 0)
     {
       size=get_dword(m+4);
       for(i=0;i<(int)size;i+=2)
       {
        add_reg(patch,get_word(m+8+i),TYPE_CONTROL,"iscl",0,0);
        }
     } else
     if(strncmp(m,"oscl",4) == 0)
     {
       size=get_dword(m+4);
       for(i=0;i<(int)size;i+=2)
       {
        add_reg(patch,get_word(m+8+i),TYPE_CONTROL,"oscl",0,0);
        }
     }
      else
     {
       return 1;
       }
  return 0;
}

#define fill_reg(where,name_,num_,type_,p_)		\
	do							\
	{							\
	 strncpy(where.name,name_,MAX_GPR_NAME);		\
	 where.num=num_;					\
	 where.type=type_;					\
	 where.translated=DSP_REG_NOT_TRANSLATED;		\
         where.p=p_;						\
         } while(0);

static int parse_rifx_2(patch_info &patch,void *rifx,int max_size,char *name,dsp_code **code,int *code_size,
		dsp_register_info **info,int *info_size,int *itramsize,int *xtramsize,
		char *copyright,
		char *engine,
		char *created,
		char *comment,
		char *guid)
{
	if(!rifx || !name || !code || !code_size || !info || !info_size || !itramsize || !xtramsize)
	 return -2;

	char *m=(char *)rifx;
	int i=0;
        if(strncmp(m,"RIFX",4)!=0)
        {
           debug("rifx: not rifx\n");
           return -1;
        }

        int sz=get_dword(m+4);
        if(sz>max_size)
          return -3;

        m+=8;
        i+=8;
        sz-=8;

        memset(&patch,0,sizeof(patch_info));

        AGAIN:
         while(sz>0)
         {
          char tag_name[10]={0,};
          if((i%2))
          {
           i++;
           m++;
          }
          strncpy(tag_name,m,4);
          if(strcmp(tag_name,"PTXT")==0 ||
             strcmp(tag_name,"INFO")==0 ||
             strcmp(tag_name,"gpri")==0 ||
             strcmp(tag_name,"patc")==0 ||
             strcmp(tag_name,"scal")==0 
             )
          {
           // empty tag
           m+=4;
           i+=4;
           sz-=4;
          }
          else
          {
           int tag_sz=get_dword(m+4);
           int ret=parse_tag(m,patch);
           if(ret<0)
           {
            free(&patch);
            return patch.error;
            }

           if(ret!=1)
           {
      	   m+=tag_sz;
       	   i+=tag_sz;
       	   sz-=tag_sz;
       	   }

           m+=8;
           sz-=8;
           i+=8;
          }
         }

         if((i%2))
         {
          i++;
          m++;
         }

         if(strncmp(m,"LIST",4)==0)
         {
          sz=get_dword(m+4);
          goto AGAIN;
         }

         if(guid) guid[0]=0;

         if(strncmp(m,"$kX$",4)==0) // $KX$ signature found
         {
           m+=5; // \n
           // m points to 'guid='
           if(strncmp(m,"guid=",5)==0)
           {
            debug("rifx: guid found (ok)\n");
            memset(guid,0,KX_MAX_STRING);
            strncpy(guid,m+5,36);
           }
         }
         // ------------ parse now

         strncpy(name,patch.name,KX_MAX_STRING);
/*
           fprintf(f,"// -- Engine: %s; Version: %s\n",patch.engine,patch.engine_ver);
           fprintf(f,"// -- Copyright: %s\n",patch.copyright);
           fprintf(f,"// -- Created: %s\n",patch.created);
           fprintf(f,"// -- Comment: %s\n\n",patch.comment);

           if(patch.itramsize!=0)
            fprintf(f,"idelay_line(idly,%d); // internal TRAM delay line\n",patch.itramsize);

           if(patch.xtramsize!=0)
            fprintf(f,"xdelay_line(xdly,%d); // external TRAM delay line\n",patch.xtramsize);
*/
           // process statics
           for(i=0;i<patch.n_statics;i++)
           {
            int p=find_reg(0x8000+patch.n_outputs+i,patch);
            if(p==-1)
            {
              add_reg(patch,0x8000+patch.n_outputs+i,TYPE_STATIC,"dyn",0,0);
            }
           }

           // process itram
           for(i=0;i<patch.n_itrams;i++)
           {
            int p=find_reg(0x8000+patch.n_outputs+patch.n_statics+i*2,patch);
            if(p==-1)
            {
              add_reg(patch,0x8000+patch.n_outputs+patch.n_statics+i*2,TYPE_TRAM,"tr_ni",0,0,0,0xff); //flag
            }
           }
           // process xtram
           for(i=0;i<patch.n_xtrams;i++)
           {
            int p=find_reg(0x8000+patch.n_outputs+patch.n_statics+patch.n_itrams*2+i*2,patch);
            if(p==-1)
            {
              add_reg(patch,0x8000+patch.n_outputs+patch.n_statics+patch.n_itrams*2+i*2,
                TYPE_TRAM,"trX_ni",0,0,0,0xff); //flag
              p=patch.last-1;
            }
            patch.regs[p].type|=TYPE_XTRAM;
            patch.regs[p].name[0]='x';
            patch.regs[p+1].type|=TYPE_XTRAM;
            patch.regs[p+1].name[1]='x';
           }

           // process temp regs
           for(i=0;i<patch.n_temps;i++)
           {
            add_reg(patch,0x8000+patch.n_outputs+patch.n_statics+patch.n_itrams*2+patch.n_xtrams*2+i,TYPE_TEMP,"tmp",0,0);
           }

           *info_size=patch.last*(int)sizeof(dsp_register_info);
		   *xtramsize=patch.xtramsize;
           *itramsize=patch.itramsize;
           *code_size=patch.code_size*(int)sizeof(dsp_code);

           *code=NULL;
           dsp_register_info *info_=(dsp_register_info *)dsp_alloc(*info_size);
           *info=info_;
           if(info_==NULL)
           {
            free(&patch);
            debug("rifx: error allocating info\n");
            return -9;
            }
           dsp_code *code_=(dsp_code *)dsp_alloc(*code_size);
           *code=code_;
           if(code_==NULL)
           {
            dsp_free(info_);
            *info=NULL;
            free(&patch);
            debug("rifx: error allocating code\n");
            return -9;
           }

           // process all the regs
           for(i=1;i<=patch.last;i++)
           {
            switch(patch.regs[i].type)
            {
             case TYPE_INPUT: 
                char tmp2[18];
                strcpy(patch.regs[i].name,"in");
                my_itoa(patch.regs[i].num-0x4000,tmp2,16);
                strcat(patch.regs[i].name,tmp2);
             	fill_reg((*info)[i-1],patch.regs[i].name,patch.regs[i].num,GPR_INPUT,0x0);
                break;
             case TYPE_OUTPUT: 
	        fill_reg((*info)[i-1],patch.regs[i].name,patch.regs[i].num,GPR_OUTPUT,0x0);
                break;
             case TYPE_CONTROL:
                fill_reg((*info)[i-1],patch.regs[i].name,patch.regs[i].num,GPR_CONTROL,0x0);
                break;
             case TYPE_STATIC: 
                fill_reg((*info)[i-1],patch.regs[i].name,patch.regs[i].num,GPR_STATIC,0x0);
         	break;
             case TYPE_STATIC_I: 
                fill_reg((*info)[i-1],patch.regs[i].name,patch.regs[i].num,GPR_STATIC,patch.regs[i].value);
                break;
             case TYPE_TRAM: 
             case TYPE_TRAM|TYPE_XTRAM:
             	    byte flag;

                    if(patch.regs[i].type&TYPE_XTRAM)
             	     flag=GPR_XTRAM;
             	    else
             	     flag=GPR_ITRAM;

             	    if(patch.regs[i].flag==0)
             	    {
             	      flag|=TRAM_READ;
             	      }
             	     else
             	    if(patch.regs[i].flag==0x10)
             	    {
             	      flag|=TRAM_WRITE;
             	      }

                    fill_reg((*info)[i-1],patch.regs[i].name,patch.regs[i].num,flag,patch.regs[i].value);
                    i++;
                    flag&=0xf0;
                    flag|=GPR_TRAMA;
                    fill_reg((*info)[i-1],patch.regs[i].name,patch.regs[i].num,flag,patch.regs[i].value);
                    break;

             case TYPE_INTR: 
             	    fill_reg((*info)[i-1],patch.regs[i].name,patch.regs[i].num,GPR_INTR,patch.regs[i].value);
             	    break;
             case TYPE_TEMP: 
             	    fill_reg((*info)[i-1],patch.regs[i].name,patch.regs[i].num,GPR_TEMP,patch.regs[i].value);
                    break;
             default:
             		debug("rifx: error: type unknown\n");
             		patch.error=-6;
             		dsp_free(*info);
             		dsp_free(*code);
                        *info=NULL;
                        *code=NULL;
                        free(&patch);
                        return -6;
            }
           }

           byte *mm=&patch.code[0];
           for(i=0;i<patch.code_size;i++)
           {
             byte op;
             word a,b,c,ret;

             op=(get_word(mm+i*9)>>8)&0xff;
             a=get_word(mm+i*9+1);
             b=get_word(mm+i*9+3);
             c=get_word(mm+i*9+5);
             ret=get_word(mm+i*9+7);

             parse_const(a);
             parse_const(b);
             parse_const(c);
             parse_const(ret);

             for(int j=1;j<=patch.last;j++)
             {
              if(patch.regs[j].type==TYPE_INPUT)
              {
               if((patch.regs[j].position&0xff)==i)
               {
                 if(patch.regs[j].position&0xff00)
                 {
                   word num=patch.regs[j].num;
                   patch.regs[j].num=patch.regs[j].position;

                   if((patch.regs[j].position&0xff00)==0x2000)
                   {
                     if((a&0xff)==0x40) a=num; else patch.warning|=1024;
                   }

                   if((patch.regs[j].position&0xff00)==0x4000)
                   {
                     if((b&0xff)==0x40 || b==0) b=num; else patch.warning|=2048;
                   }

                   if((patch.regs[j].position&0xff00)==0x6000)
                   {
                     if((c&0xff)==0x40) c=num; else patch.warning|=4096;
                   }

                   if((patch.regs[j].position&0xff00)==0x8000)
                   {
                     if((ret&0xff)==0x40) ret=num; else patch.warning|=8192;
                   }
        	 }
               }
              }
             }
             // prase input[]
             (*code)[i].op=op;
             (*code)[i].r=ret;
             (*code)[i].a=a;
             (*code)[i].x=b;
             (*code)[i].y=c;
           }
        int wrn=patch.warning;

        if(wrn)
         debug("rifx: result: warnings: %d\n",wrn);

         if(copyright) strncpy(copyright,patch.copyright,KX_MAX_STRING);
         if(engine) strncpy(engine,patch.engine,KX_MAX_STRING);
         if(created) strncpy(created,patch.created,KX_MAX_STRING);
         if(comment) strncpy(comment,patch.comment,KX_MAX_STRING);

        free(&patch);
        return wrn;
}



// returns >=0 if ok; app should call LocalFree for *code & *info
// <0 - error
int iKX::parse_rifx(void *rifx,int max_size,char *name,dsp_code **code,int *code_size,
		dsp_register_info **info,int *info_size,int *itramsize,int *xtramsize,
		char *copyright,
		char *engine,
		char *created,
		char *comment,
		char *guid)
{
        patch_info *patch; 
        patch = (patch_info *)malloc(sizeof(patch_info));
        if(patch==0)
        {
         debug("rifx: malloc failed (%d)\n",sizeof(patch_info));
         return -33;
        }
        memset(patch,0,sizeof(patch_info));
        int ret=parse_rifx_2(*patch,rifx,max_size,name,code,code_size,
          info,info_size,itramsize,xtramsize,
          copyright,engine,created,comment,guid);
        return ret;
}
