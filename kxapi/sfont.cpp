// kX Driver Interface
// Copyright (c) Eugene Gavrilov, 2001-2011.
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

#include <sfman/sfdevman.h>
#include <sfman/sfdevdta.h>
#include <sfman/sfman.h>

extern "C" LRESULT __declspec(dllexport) WINAPI kx_sfont_entry(SFDEVHANDLE user_id,UINT msg,LPARAM param1,LPARAM param2);

#define SF_MAGIC 0x94837557

#define USE_MIDIOUT
#define DOUBLE_DEVICES 1

typedef struct sample_list_t_s
{
 struct sample_list_t_s *next,*prev;
 CViSmplObject *sample;
}sample_list_t;

typedef struct
{
 dword magic;
 LPARAM ori_dev; // device after Open() -- logical ( /2 -> physical kX)
 iKX *ikx;
#ifdef USE_MIDIOUT
 HMIDIOUT midi;
#endif

 sample_list_t *sample_list; int samples_modified;

 int have_preset;
 int have_sf2;
}sf_device;

LRESULT WINAPI kx_sfont_entry(SFDEVHANDLE user_id,UINT msg,LPARAM param1,LPARAM param2)
{
 sf_device *sfd=(sf_device *)user_id;
 int is_vienna=0;

// debug("sfont: event: %d\n",msg);
// DebugBreak();

 switch(msg)
 {
	case SFDEV_VIENNA_START:
		debug("sfont: ViennaStart\n");
		// param2 - device #
		is_vienna=1;

		// fall through
 	case SFDEV_OPEN:
 		{
         		sfd=(sf_device *)malloc(sizeof(sf_device));
         		if(sfd==0)
         		{
         		  debug("sfont: FATAL: no more memory in DevOpen\n");
         		  return SFDEVERR_SYSMEM_INSUFFICIENT;
         		}
         		memset(sfd,0,sizeof(sf_device));
         		sfd->ikx=new iKX;
         		if(sfd->ikx->init((int)(DOUBLE_DEVICES?param2/2:param2))!=0) // failed
         		{
         		 delete sfd->ikx;
         		 free(sfd);
         		 debug("sfont: Incorrect device num in DevOpen (param2=%d)\n",param2);
         		 return SFDEVERR_DEVICE_DRV_INVALID;
         		}
 			debug("sfont: DevOpen ok %d (x2)\n",param2);
                        sfd->magic=SF_MAGIC;
         		sfd->ori_dev=param2;

         		sfd->have_preset=0;
         		sfd->have_sf2=0;

         		sfd->sample_list=0;
         		sfd->samples_modified=0;

         		*(SFDEVHANDLE *)param1=(SFDEVHANDLE)sfd;

         		char dev_name[32];
			if(DOUBLE_DEVICES && (param2&1))
                            strncpy(dev_name,sfd->ikx->get_winmm_name(KX_WINMM_SYNTH2),32);
                        else
                            strncpy(dev_name,sfd->ikx->get_winmm_name(KX_WINMM_SYNTH),32);

#ifdef USE_MIDIOUT
			sfd->midi=NULL;
if(is_vienna)
{

			if(midiOutGetNumDevs()) // to avoid '0' case
			for(dword i=0;i<midiOutGetNumDevs();i++)
			{
				MIDIOUTCAPS caps;
				if(midiOutGetDevCaps(i,&caps,sizeof(caps))==MMSYSERR_NOERROR)
				{
				 if(strncmp(caps.szPname,dev_name,32)==NULL)
				 {
                        	   if(midiOutOpen(&sfd->midi,i,NULL,NULL,CALLBACK_NULL)==MMSYSERR_NOERROR)
                        	   {
                        	    debug("sfont: midiOut opened [%s]\n",dev_name);
                        	    break;
                        	   } else debug("sfont: midiOutOpen failed: %x ['%s']\n",GetLastError(),caps.szPname);
                        	 }
                        	}
			}
			if(sfd->midi==0)
			 debug("!! sfont: midiOut failed to open\n");
}
#endif
                }
 		break;

	case SFDEV_VIENNA_END:
		debug("sfont: ViennaEnd\n");
                // fall through
                is_vienna=1;

 	case SFDEV_CLOSE:
 		{
 			if(sfd==0)
 			{
                         // try 'param1':
                         int ret=1;

                         try
                         {
                          debug("sfont: possible invalid SFDEV_CLOSE event\n");

                          sfd=(sf_device *)param1;
                          if(sfd->magic==SF_MAGIC)
                           ret=0;
                         }
                         catch(...)
                         {
                          debug("sfont: exception when trying 'param1'\n");
                          ret=1;
                         }

                         if(ret==1)
                         {
                          debug("sfont: Bad UserID for DevClose (%x; %x %x)\n",user_id,param1,param2);
 			  return SFDEVERR_USERID_INVALID;
 			 }
 			}

 			if(sfd->magic!=SF_MAGIC)
 			{
 			 debug("sfont: Bad UserID for Dev_Close (userid=%x)\n",sfd);
 			 return SFDEVERR_USERID_INVALID;
 			}
 			debug("sfont: DevClose ok (userid=%x) [dev=%d]\n",sfd,sfd->ori_dev);
 			if(sfd->ikx)
 			{
 			 // free preset_list and sample_list
 			 sample_list_t *sample=sfd->sample_list;
 			 if(sample)
 			  debug("sfont: WARNING: not all presets were freed - autofree\n");
 			 while(sample)
 			 {
 			   sample_list_t *tmp=sample->next;
 			   if(sample->sample)
 			    free(sample->sample);
 			   free(sample);
 			   sample=tmp;
 			 }

 			 sfd->ikx->close();
 			 delete sfd->ikx;
 			 sfd->ikx=0;
 			}

#ifdef USE_MIDIOUT
if(is_vienna)
{
 			if(sfd->midi)
 			{
 			 debug("sfont: closing midiOut\n");
 			 midiOutClose(sfd->midi);
 			 sfd->midi=NULL;
 			}
}
#endif
			memset(sfd,0,sizeof(sf_device));
 			free(sfd);
 		}
 		break;
 	case SFDEV_GET_NUM_DEVS:
 		{
                        int cnt;
                        cnt=0;
                        while(1)
                        {
                         iKX ikx;
                         if(ikx.init(cnt)==0) // ok
                         {
                            ikx.close();
                            cnt++;
                         } else 
                          break;
                        }
                        *(word *)param1=(word)(DOUBLE_DEVICES?cnt*2:cnt); // 2 synths per device
                        debug("sfont: GetNumDevs request (ret=%d)\n",DOUBLE_DEVICES?cnt*2:cnt);
 		}
 		break;
 	case SFDEV_GET_DEVICE_CAPS:
 		{
 			CSFDevObject10x *info;
 			iKX ikx;
 			if(ikx.init((int)(DOUBLE_DEVICES?param1/2:param1))==0)
 			{
 			  // init ok; fill-in structure;
                          info=(CSFDevObject10x *)param2;
                          dword port=0;
                          ikx.get_dword(KX_DWORD_CARD_PORT,&port);
                          if(info->m_SizeOf>=SFDEV_CAPSIZE_100)
                          {
                           info->m_BaseAddr=port+(dword)(DOUBLE_DEVICES?(param1&1):0);

                           info->m_DevNode=port+(dword)(DOUBLE_DEVICES?(param1&1):0);
                           // e.g. c2a12160
                           info->m_RomId=0;
                           info->m_RomVer=0;
                           info->m_hTask=(DWORD_PTR)GetModuleHandle(NULL);
                           // e.g. 1530000

                           // caps:
                           // 31 dynamic memory: 0 - NOT shared		0x80000000
                           // 30 software synth: 0 - hardware
                           // 29 hidden device:  0 - not hidden
                           // 28 sf only:        1 - only sfs [no wavs] 0x10000000
                           // 27 version:        1 - > soundfont 2.0	0x08000000

                           // APS manager: 0x873e0200 0xe130fe9b 0xd280fea0
                           // Vienna extensions				0x00000200

                           info->m_DevCaps=// SFMANCAP_DYNAMIC_MEM_CAP| //0x80000000
                           		// 0x08000000|	// v2.0 only
                           		0x10000000| 	// sf only
                           		SFMANCAP_VIENNA_EXTENSIONS_CAP // 0x200
                           		;

                           if(DOUBLE_DEVICES && (param1&1) )
                             strncpy(info->m_DevName,ikx.get_winmm_name(KX_WINMM_SYNTH2),32);
                           else
                             strncpy(info->m_DevName,ikx.get_winmm_name(KX_WINMM_SYNTH),32);
                           sprintf(info->m_SndEngine,"10kX Engine");
                           sprintf(info->m_SysExData,"\0");
                          } else debug("sfont: bad m_SizeOf (%d)\n",info->m_SizeOf);

                          if(info->m_SizeOf>=SFDEV_CAPSIZE_102)
                          {
                           info->m_SFVerLow=0x20000;
                           info->m_SFVerHigh=0x2000a;
                           debug("sfont: ver 1.02 caps\n");
                          }
                          debug("sfont: GetDevCaps ok (id=%d); name=%s\n",param1,info->m_DevName);
 			} 
 			 else 
 			{
 			 debug("sfont: Incorrect UserId in GetDevCaps\n");
 			 return SFDEVERR_USERID_INVALID;
 			}
 		}
 		break;
 	case SFDEV_QUERY_SYNTH_SUPPORT:
 		{
 			CSFDevBufferObject *b=(CSFDevBufferObject *)param1;
 			if(b && (b->m_Size>=strlen(/*"General MIDIxGSxMT 32x*/"kX Synthxx")))
 			{
                         memcpy(b->m_Buffer,/*"General MIDI\0GS\0MT 32\0*/"kX Synth\0\0",
                        	b->m_Size);
                         b->m_Var.m_SizeUsed=(dword)strlen(/*"General MIDIxGSxMT 32x*/"kX Synthxx");
                         b->m_Flag=1; // 4;
                         debug("sfont: QuerySynthSupport ok\n");
                        }
                        else
                        {
                         b->m_Var.m_SizeUsed=(dword)strlen(/*"General MIDIxGSxMT 32x*/"kX Synthxx");
                         b->m_Flag=1;// 4;
                         debug("sfont: QuerySynthSupport failed [size=%d]\n",b->m_Size);

                         return SFDEVERR_RESOURCE_INSUFFICIENT;
                        }
 		}
 		break;
 	case SFDEV_GET_SYNTH_EMULATION:
 		{
 			CSFDevBufferObject *b=(CSFDevBufferObject *)param1;
 			if(b && (b->m_Size>=strlen("kX Synthx")))
 			{
                         strncpy(b->m_Buffer,"kX Synth",
                        	b->m_Size);
                         b->m_Var.m_SizeUsed=(dword)strlen("kX Synthx");
                         b->m_Flag=0; // 3;
                         debug("sfont: GetSynthEmul ok\n");
                        }
                        else
                        {
                         b->m_Var.m_SizeUsed=(dword)strlen("kX Synthx");
                         b->m_Flag=0; // 3;
                         debug("sfont: GetSynthEmulation failed [size=%d]\n",b->m_Size);
                        }
 		}
 		break;
        case SFDEV_SELECT_SYNTH_EMULATION:
        	{
        		if(param1!=0) // 3
        		{
        		  debug("sfont: SelectSynthEmul failed (param!=0 = %d)\n",param1);
        		  return SFDEVERR_BANK_INDEX_INVALID;
        		}
        		debug("sfont: SelectSynthEmul ignored (=0) [%d]\n",param1);
        	}
        	break;
        case SFDEV_LOAD_BANK:
        	{
        		dword bank=(dword)param1;
                        CSFDevBufferObject *b=(CSFDevBufferObject *)param2;
                        if(b->m_Flag==SFDEVMANFLAG_OPER_MEMORY)
                        {
                         // FIXME !!!
                         debug("sfont: LoadBank with RAM -- NOT SUPPORTED yet\n");
                         return SFDEVERR_SOUND_MEM_INSUFFICIENT;
                        }
                        else
                         if(b->m_Flag==SFDEVMANFLAG_OPER_FILE)
                         {
                          int id=sfd->ikx->parse_soundfont(b->m_Buffer,NULL,bank);
                          if(id<=0)
                          {
                           debug("sfont: LoadBank from file FAILED [ret=%d]\n",id);
                           return SFDEVERR_PATHNAME_INVALID;
                           }
                          debug("sfont: LoadBank from file OK (id=%d)\nfile: %s\n",id,b->m_Buffer);
                         }
                          else 
                          {
                          debug("sfont: LoadBank: m_flag=%d -- ERROR\n",b->m_Flag);
                          return SFDEVERR_UNDEFINE_ERROR;
                          }
        	}
        	break;
        case SFDEV_GET_BANK_DESCRIPTOR:
        	{
        		dword bank=(dword)param1; // Section 3.1.1.4
                        CSFDevBufferObject *b=(CSFDevBufferObject *)param2;
 			if(sfd->magic!=SF_MAGIC)
 			{
 			 debug("sfont: GetBankDescr: bad magic\n");
 			 return SFDEVERR_USERID_INVALID;
 			}
 			if(sfd->ikx==0)
 			{
 			 debug("sfont: GetBankDescr: kX not initialized\n");
 			 return SFDEVERR_USERID_INVALID;
 			}

 			// debug("sfont: get bank descr: %d\n",bank);

                        int sz=sfd->ikx->enum_soundfonts(0,0);
                        if(sz>0)
                        {
                             sfHeader *hdr;
                             hdr=(sfHeader *)malloc(sz);
                             if(hdr)
                             {
                                if(!sfd->ikx->enum_soundfonts(hdr,sz))
                                {
                                   for(dword i=0;i<sz/sizeof(sfHeader);i++)
                                   {
                                     if(hdr[i].sfman_id==bank) // FIXME: actually, the bank may be empty! or unrelated to the [i] bank
                                     {
                                        // return 'INAM' section
                                        b->m_Var.m_SizeUsed=(dword)strlen(hdr[i].name)+2; // strlen of string above
                                        // note: -TWO- consecutive \0
                                        if(b->m_Size>=(dword)strlen(hdr[i].name)+2)
                                        {
                                         memset(b->m_Buffer,0,b->m_Var.m_SizeUsed);
                                         strncpy(b->m_Buffer,hdr[i].name,b->m_Size);
                                         // debug("sfont: GetBankDescr. ok: %s\n",hdr[i].name);
                                         free(hdr);
                                         return 0; // ok
                                        }
                                        else
                                        {
                                         free(hdr);
                                         debug("sfont: GetBankDescr. Buffer too small\n");
                                         return SFDEVERR_RESOURCE_INSUFFICIENT;
                                        }
                                     }
                                    } // for

                                     // check, if this bank is used by other 'real' banks
                                     
                                     {
                                       char tmp_name[32];
                                       if(sfd->ikx->get_preset_description(bank,-1,tmp_name)==0)
                                       {
                                        dword i=*(dword *)tmp_name;
                                        if(i<sz/sizeof(sfHeader))
                                        {
                                            // return 'INAM' section
                                            b->m_Var.m_SizeUsed=(dword)strlen(hdr[i].name)+7; // strlen of string above
                                            if(b->m_Size>=((dword)strlen(hdr[i].name)+7))
                                            {
                                             memset(b->m_Buffer,0,b->m_Var.m_SizeUsed);
                                             sprintf(b->m_Buffer,"*%s %03d",hdr[i].name,bank);
                                             // debug("sfont: GetBankDescr. ok: %s (*)\n",hdr[i].name);
                                             free(hdr);
                                             return 0; // ok
                                            }
                                            else
                                            {
                                             free(hdr);
                                             debug("sfont: GetBankDescr. Buffer too small (*)\n");
                                             return SFDEVERR_RESOURCE_INSUFFICIENT;
                                            }
                                         }
                                         else
                                          debug("sfont: GetBank/PresetDescr failed [%x]\n",i);
                                       }
                                     }
                                     
                                }
                                free(hdr);
                             } else debug("sfont: GetBankDescr: mem  failed [%d]\n",sz);
                        }
                        debug("sfont: GetBankDescr failed - no such bank\n");
                        return SFDEVERR_BANK_INDEX_INVALID;
        	}
        	break;
        case SFDEV_GET_BANK_PATHNAME:
        	{
 			if(sfd->magic!=SF_MAGIC)
 			 return SFDEVERR_USERID_INVALID;
 			if(sfd->ikx==0)
 			 return SFDEVERR_USERID_INVALID;

                        CSFDevBufferObject *b=(CSFDevBufferObject *)param1;
                        if(b==0)
                        {
                         debug("sfont: invalid devbuffer object\n");
                         return SFDEVERR_BANK_INDEX_INVALID;
                        }
        		dword bank=b->m_Flag; // 3.1.5.3

 			debug("sfont: get bank pathname: %d\n",bank);

                        int sz=sfd->ikx->enum_soundfonts(0,0);
                        if(sz>0)
                        {
                             sfHeader *hdr;
                             hdr=(sfHeader *)malloc(sz);
                             if(hdr)
                             {
                                if(!sfd->ikx->enum_soundfonts(hdr,sz))
                                {
                                   for(dword i=0;i<sz/sizeof(sfHeader);i++)
                                   {
                                     if(hdr[i].sfman_id==bank)
                                     {
                                        // return filename section
                                        b->m_Var.m_SizeUsed=(dword)strlen(hdr[i].sfman_file_name)+2; // strlen of string above
                                        if(b->m_Size>=(dword)strlen(hdr[i].sfman_file_name)+2)
                                        {
                                         memset(b->m_Buffer,0,b->m_Var.m_SizeUsed);

                                         strncpy(b->m_Buffer,hdr[i].sfman_file_name,b->m_Size);
                                         debug("sfont: GetBankPath ok: %s\n",hdr[i].sfman_file_name);
                                         free(hdr);
                                         b->m_Flag=0; // index to the emulation
                                         return 0; // ok
                                        }
                                        else
                                        {
                                         free(hdr);
                                         debug("sfont: GetBankPath failed; buf too small\n");
                                         return SFDEVERR_RESOURCE_INSUFFICIENT;
                                        }
                                     }
                                   }
                                }
                                free(hdr);
                             }
                        }
                        debug("sfont: GetBankPath failed: bad bank id\n");
                        return SFDEVERR_BANK_INDEX_INVALID;
        	}
        	break;
        case SFDEV_CLEAR_BANK:
        	{
        		dword bank=(dword)param1;
 			if(sfd->magic!=SF_MAGIC)
 			 return SFDEVERR_USERID_INVALID;
 			if(sfd->ikx==0)
 			 return SFDEVERR_USERID_INVALID;

 			// debug("sfont: clear bank: %d\n",bank);

                        int sz=sfd->ikx->enum_soundfonts(0,0);
                        if(sz>0)
                        {
                             sfHeader *hdr;
                             hdr=(sfHeader *)malloc(sz);
                             if(hdr)
                             {
                                if(!sfd->ikx->enum_soundfonts(hdr,sz))
                                {
                                   for(dword i=0;i<sz/sizeof(sfHeader);i++)
                                   {
                                     if(hdr[i].sfman_id==bank)
                                     {
                                        if(sfd->ikx->unload_soundfont(hdr[i].rom_ver.minor)==0)
                                        {
                                          debug("sfont: Bank cleared (%d)\n",hdr[i].rom_ver.minor);
                                          free(hdr);
                                          return 0; // ok
                                          }
                                        else
                                        {
                                          debug("sfont: Clear bank failed in kX\n");
                                          free(hdr);
                                          return SFDEVERR_BANK_INDEX_INVALID;
                                          }
                                     }
                                   }
                                }
                                free(hdr);
                             }
                        }
                        // debug("sfont: ClearBank failed: bad bank\n");
                        return SFDEVERR_BANK_INDEX_INVALID;
        	}
        	break;

	case SFDEV_VIENNA_GET_DRAM_SIZE:
		{
        	 dword *max_mem=(dword *)param1;
        	 dword *used_mem=(dword *)param2;

                 if(sfd->magic!=SF_MAGIC)
                  return SFDEVERR_USERID_INVALID;
                 if(sfd->ikx==0)
                  return SFDEVERR_USERID_INVALID;

                 int sz;
                 sz=sfd->ikx->enum_soundfonts(0,0);
                 dword total_sf_size=0;
                 if(sz>0)
                 {
                      sfHeader *hdr;
                      hdr=(sfHeader *)malloc(sz);
                      if(hdr)
                      {
                         if(!sfd->ikx->enum_soundfonts(hdr,sz))
                         {
                            for(dword i=0;i<sz/sizeof(sfHeader);i++)
                            {
                                total_sf_size+=hdr[i].sample_len;
                            }
                         }
                         free(hdr);
                      }
                 }
                 if(used_mem)
                  *used_mem=total_sf_size;
                 else
                  debug("!! sfont: used_mem=0\n");

                 MEMORYSTATUS st;
                 st.dwLength=sizeof(MEMORYSTATUS);
                 GlobalMemoryStatus(&st);

                 if(max_mem)
                   *max_mem=(dword)(st.dwTotalPhys/3); // approximation; see kxmixer maindlg.cpp FIXME
                 else
                   debug("!! sfont: max_mem=0\n");

                 debug("sfont: GetDramSize: %d %d\n",max_mem?*max_mem:-1,used_mem?*used_mem:-1);

                 // Vienna seems to ignore correct values -- change them
                 if(used_mem)
                 	(*used_mem)+=(*max_mem);// 1*1024*1024;

                 debug("sfont: lie: GetDramSize: %d %d\n",max_mem?*max_mem:-1,used_mem?*used_mem:-1);
		}
		break;
        case SFDEV_QUERY_STATIC_SMEM_SIZE:
        	{
        	 debug("sfont: query_static_smem: ret error (per specs)\n");
        	 return SFDEVERR_INVALID_MEMORY_TYPE; // per specs
        	}
        	break;
        case SFDEV_IS_MIDI_BANK_USED:
        	{
        		word *bank=(word *)param1;
 			if(sfd->magic!=SF_MAGIC)
 			 return SFDEVERR_USERID_INVALID;
 			if(sfd->ikx==0)
 			 return SFDEVERR_USERID_INVALID;

 			// if(*bank==0)
 			// debug("sfont: quering bank 0!\n");
                        // Sonar doesn't query bank 0!

                        int sz=sfd->ikx->enum_soundfonts(0,0);
                        if(sz>0)
                        {
                             sfHeader *hdr;
                             hdr=(sfHeader *)malloc(sz);
                             if(hdr)
                             {
                                if(!sfd->ikx->enum_soundfonts(hdr,sz))
                                {
                                   for(dword i=0;i<sz/sizeof(sfHeader);i++)
                                   {
                                     if(hdr[i].sfman_id==(dword)(*bank))
                                     {
                                     	  // leave it unchanged: the whole bank is loaded
                                     	  // debug("sfont: bank [%d] is fully used\n",
                                     	  //  *bank);
                                          free(hdr);
                                          return 0; // ok
                                     }
                                   }
                                }
                                free(hdr);
                             } else debug("sfont: isbankused: no mem [sz=%d]\n",sz);
                        } // else debug("sfont: isbankused: no sfbanks [sz=0]\n");
                        char name[32];
                        if(sfd->ikx->get_preset_description((int)(*bank),-1,name)==0)
                        {
                         // *bank=(word)(127-(int)(*bank)); // 'Changed': bank isn't free; possibly by another bank
                         // FIXME NOW: 1/11/2003
                         // debug("sfont: bank [%d] is partially used\n",*bank);

                         // FIXME NOW: 1/03/2004 -- try '0'
                         // *bank=0; // bank's parent is 'synthesizer' bank [awe specs?]
                         // debug("sfont: bank [%d] marked as synth-used\n",*bank);

                         // well, 'unchanged' (3/03/2004)

                         return 0; 
                        }
                        else
                        {
                         // debug("sfont: IsMidibankUsed %d? ret: -1\n",*bank);
                         *bank=(word)-1;
                        }
        	}
        	break;
        case SFDEV_GET_MANAGER_VERSION:
        	{
        		word *major=(word *)param1;
        		word *minor=(word *)param2;
                        *major=2;
                        *minor=0xa;
                        debug("sfont: GetVersion\n");
        	}
        	break;
        case SFDEV_ISHANDLE:
        	{
        		sf_device *handle=(sf_device *)param1;
        		debug("sfont: IsHandle call?\n");
        		if(handle)
        		{
        		 if(handle->magic==SF_MAGIC)
        		 {
        		  return 0;
        		 }
        		}
        		return SFDEVERR_USERID_INVALID;
        	}
        	break;
        case SFDEV_IS_DEVICE_FREE:
        	{
        		debug("sfont: is_device_free: assume yes(?)\n");
        		// assume always free
        		return 0;
        	}
        	break;
        case SFDEV_GET_PRESET_DESCRIPTOR:
        	{
                 if(sfd->magic!=SF_MAGIC)
                 {
                 debug("sfont: Bad UserID for GetPresetDescriptor (userid=%x)\n",sfd);
                 return SFDEVERR_USERID_INVALID;
                 }
        	CSFDevParamObject *request=(CSFDevParamObject *)param1;
        	// debug("sfont: GetPresetDescr: %d.%d\n",request->m_BankIndex,request->m_PresetIndex);
        	CSFDevBufferObject *response=(CSFDevBufferObject *)param2;

        	int bnk=request->m_BankIndex;
        	int prs=request->m_PresetIndex;

        	if(request->m_BankIndex>127)
        	{
                  //debug("sfont: invalid bank [%d]; preset=%d\n",request->m_BankIndex,request->m_PresetIndex);
                  return SFDEVERR_PRESET_INDEX_INVALID;
        	}
                // translate drum bank
                if(request->m_BankIndex==0 && (int)request->m_PresetIndex>127)
                {
                  // 'target'
                  bnk=128;
                  prs-=128;
                }
                if(prs>127)
                {
                  //debug("sfont: invalid preset [%d]; bnk=%d\n",request->m_PresetIndex,request->m_BankIndex);
                  return SFDEVERR_PRESET_INDEX_INVALID;
                }                  

                if(response->m_Size>0)
                  response->m_Buffer[0]=0;

                char name[32];
                if(sfd->ikx->get_preset_description(bnk,prs,name)==0)
                {
                  if(response->m_Size>=strlen(name)+2)
                  {
                  	memset(response->m_Buffer,0,strlen(name)+2);

                  	strncpy(response->m_Buffer,name,response->m_Size);
                  	response->m_Var.m_SizeUsed=(dword)strlen(name)+2;
                  }
                   else 
                  {
                   	response->m_Var.m_SizeUsed=(dword)strlen(name)+2;
                   	debug("sfont: not enought mem [%d] -- '%s'\n",response->m_Var.m_SizeUsed,name);

                   	return SFDEVERR_RESOURCE_INSUFFICIENT;
		  }
                 } else return SFDEVERR_PRESET_INDEX_INVALID;
        	}
                break;

	case SFDEV_VIENNA_NOTE_ON:
                 if(sfd->magic==SF_MAGIC)
                 {
                  if((sfd->have_preset==0) || (sfd->samples_modified))
                   debug("sfont: WARNING: preset data incomplete\n");

//                  debug("sfont: Vienna NoteON: %x\n",0x90|((param1&0xff)<<8)|(param1&0xff0000));
                  if(sfd->ikx->send_synth((int)(DOUBLE_DEVICES?(sfd->ori_dev&1):0),(dword)(0x90|((param1&0xff)<<8)|(param1&0xff0000)))!=0)
                   debug("sfont: note_on failed\n");
                  return 0;
                 }
                return SFDEVERR_MSG_INVALID;
	case SFDEV_VIENNA_NOTE_OFF:
                 if(sfd->magic==SF_MAGIC)
                 {
//		  debug("sfont: Vienna NoteOFF\n");
		  sfd->ikx->send_synth((int)(DOUBLE_DEVICES?(sfd->ori_dev&1):0),(dword)(0x80|((param1&0xff)<<8)|(param1&0xff0000)));
		  return 0;
		 }
                return SFDEVERR_MSG_INVALID;
	case SFDEV_VIENNA_CONTROLLER:
                 if(sfd->magic==SF_MAGIC)
                 {
//		  debug("sfont: Vienna Controller\n");
		  sfd->ikx->send_synth((int)(DOUBLE_DEVICES?(sfd->ori_dev&1):0),(dword)(0xb0|((param1&0xff)<<8)|(param1&0xff0000)));
		  return 0;
		 }
                return SFDEVERR_MSG_INVALID;
	case SFDEV_VIENNA_PITCHBEND:
                 if(sfd->magic==SF_MAGIC)
                 {
//		  debug("sfont: Vienna PitchBend\n");
		  sfd->ikx->send_synth((int)(DOUBLE_DEVICES?(sfd->ori_dev&1):0),(dword)(0xe0|((param1&0xff)<<8)|(param1&0xff0000)));
		  return 0;
		 }
                return SFDEVERR_MSG_INVALID;
	case SFDEV_VIENNA_PRESSURE:
                 if(sfd->magic==SF_MAGIC)
                 {
//		  debug("sfont: Vienna ChPressure\n");
		  sfd->ikx->send_synth((int)(DOUBLE_DEVICES?(sfd->ori_dev&1):0),(dword)(0xd0|((param1&0xff)<<8)));
		  return 0;
		 }
                return SFDEVERR_MSG_INVALID;
	case SFDEV_VIENNA_SYSEX:
                 if(sfd->magic==SF_MAGIC)
                 {
		  debug("sfont: Vienna SysEX -- IGNORED\n");
                  // FIXME
		  return 0;
		 }
                return SFDEVERR_MSG_INVALID;

	// Vienna API
	case SFDEV_VIENNA_LOAD_PRESET:

//		debug("sfont: Vienna LoadPreset\n");
		if(sfd->have_preset)
		 debug("sfont: WARNING: double LoadPreset call!\n");

		sfd->have_preset=1;
		sfd->samples_modified=0;
		{
		 char tmp[32];
		 sprintf(tmp,"mem://%x",param1);
		 sfd->have_sf2=sfd->ikx->parse_soundfont(tmp,NULL/*"c:\\ttt"*/,VIENNA_DYNAMIC); // Vienna Extension
		}
                // set bank, instrument: VIENNA_DYNAMIC+1, 0x77

                sfd->ikx->send_synth((int)(DOUBLE_DEVICES?(sfd->ori_dev&1):0),0xb0|((0)<<8)|(0x7f0000));
                sfd->ikx->send_synth((int)(DOUBLE_DEVICES?(sfd->ori_dev&1):0),0xb0|((32)<<8)|(0x7f0000));

		sfd->ikx->send_synth((int)(DOUBLE_DEVICES?(sfd->ori_dev&1):0),0xc0|(0x77<<8));

                return SFDEVERR_NO_ERR;

	case SFDEV_VIENNA_FREE_PRESET:
		{
//		debug("sfont: Vienna FreePreset (%d %d)\n",param1,param2);

//       		void *sf2=(void *)param1;
       		dword *max_mem=(dword *)param2;

                MEMORYSTATUS st;
                st.dwLength=sizeof(MEMORYSTATUS);
                GlobalMemoryStatus(&st);

                if(max_mem)
                {
                  *max_mem=(dword)(st.dwTotalPhys/3); // approximation; see kxmixer maindlg.cpp FIXME
                  debug("sfont: free_preset call INVALID\n");
                }

                sfd->have_preset=0;

                if(sfd->have_sf2)
                 sfd->ikx->unload_soundfont(sfd->have_sf2); // Vienna Extension
                sfd->have_sf2=0;

                // actually, none of the parameters is used
                return 0xcb;
                }
	case SFDEV_VIENNA_LOAD_SAMPLE:
//		debug("sfont: Vienna LoadSample\n");
		{

		CViSmplObject *p=(CViSmplObject *)param1;
		if(p)
		{
		 if(p->dwcbSize>0)
		 {
		  // first time...
		  if((p->dwSampleHandle==0)||(p->dwSampleHandle==0xffffffff))
		  {
		    CViSmplObject *copy=(CViSmplObject *)malloc(sizeof(CViSmplObject)+p->dwSampleSize);
                    if(copy)
                    {
                     sample_list_t *sample=(sample_list_t *)malloc(sizeof(sample_list_t));
                     if(sample)
                     {
                       sample->next=sfd->sample_list;
                       sample->prev=NULL;
                       sample->sample=copy;

                       if(sfd->sample_list)
                        sfd->sample_list->prev=sample;

                       sfd->sample_list=sample;

                       memcpy(copy,p,
                          ((p->dwcbSize)>=(sizeof(CViSmplObject)+p->dwSampleSize))?
                            (sizeof(CViSmplObject)+p->dwSampleSize):(p->dwcbSize));

                       sfd->samples_modified=1;
                       p->dwSampleHandle=(DWORD_PTR)sample;
                       copy->dwSampleHandle=(DWORD_PTR)sample;
                       copy->dwSampleOffset=p->dwcbSize-sizeof(CViSmplObject)+2;

//                       debug("sfont: LoadSample: %x [%d bytes / total: %d bytes; next offset: %d bytes]\n",
//                         p->dwSampleHandle,p->dwcbSize,p->dwSampleSize,copy->dwSampleOffset);

                       return SFDEVERR_NO_ERR;
                     }
                     free(copy);
                    }
                   } // first time?
                   else // not first time:
                   {
                    sample_list_t *sample=(sample_list_t *)p->dwSampleHandle;
                    CViSmplObject *ori=sample->sample;

                    if(ori->dwSampleOffset!=0xffffffff)
                    {
//                     debug("sfont: continue: %x [%d bytes starting at byte %d out of %d bytes (?? %d)]\n",
//                      p->dwSampleHandle,p->dwcbSize,ori->dwSampleOffset,ori->dwSampleSize,p->dwSampleSize);

                     if(ori->dwSampleOffset+p->dwcbSize-sizeof(CViSmplObject)-sizeof(word)<ori->dwSampleSize)
                     {
                      memcpy(&ori->iSample[ori->dwSampleOffset/2],
                             &p->iSample[0],
                             p->dwcbSize-sizeof(CViSmplObject)+2);
                      ori->dwSampleOffset+=p->dwcbSize-sizeof(CViSmplObject)+2;
//                      debug("-- so, let it be offset: %d ori: %d\n",ori->dwSampleOffset,ori->dwSampleSize);
                     }
                     else
                     {
                      memcpy(&ori->iSample[ori->dwSampleOffset/2],
                             &p->iSample[0],
                             ori->dwSampleSize-ori->dwSampleOffset);
//                      debug("-- final\n");
                      ori->dwSampleOffset=0xffffffff;
                     }
                    }
                    return SFDEVERR_NO_ERR;
                   }
		 }
		}
		}
                return SFDEVERR_RESOURCE_INSUFFICIENT;
	case SFDEV_VIENNA_FREE_SAMPLE:
//		debug("sfont: Vienna FreeSample\n");
		{
		 sample_list_t *s=sfd->sample_list;
		 while(s)
		 {
		 	if(s->sample && (s->sample->dwSampleHandle==(DWORD)param1))
		 	{
		 	 if(s->next)
		 	  s->next->prev=s->prev;
		 	 if(s->prev)
		 	  s->prev->next=s->next;
//		 	 debug("sfont: FreeSample: %x [%d bytes]\n",s->sample->dwSampleHandle,s->sample->dwcbSize);

                         if(s==sfd->sample_list)
                          sfd->sample_list=s->next;

                         if(s->sample)
                          free(s->sample);

                         free(s);

                         sfd->samples_modified=1;

		 	 return SFDEVERR_NO_ERR;
		 	}
		 	s=s->next;
		 }
		}
                return SFDEVERR_MSG_INVALID;
	case SFDEV_VIENNA_PLAY_SAMPLE:
		debug("sfont: Vienna PlaySample\n");
                return SFDEVERR_MSG_INVALID;

// -----------------

        case SFDEV_LOAD_PRESET:
        	debug("sfont: LoadPreset! - fail\n");
                return SFDEVERR_SUPPORT_INVALID;
        case SFDEV_CLEAR_PRESET:
        	debug("sfont: ClearPreset! - fail\n");
                return SFDEVERR_SUPPORT_INVALID;
        case SFDEV_LOAD_WAVEFORM:
        	debug("sfont: LoadWaveform! - fail\n");
                return SFDEVERR_SUPPORT_INVALID;
        case SFDEV_CLEAR_WAVEFORM:
        	debug("sfont: ClearPreset! - fail\n");
                return SFDEVERR_SUPPORT_INVALID;
 	default:
 		debug("sfont: Invalid message %d - fail\n",msg);
 		return SFDEVERR_MSG_INVALID;
 }

 return 0;
}
