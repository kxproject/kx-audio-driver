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
#include "interface/idane.h"

iDane::iDane(char *name_,char *guid_)
{
	code=NULL;
	info=NULL;
	itramsize=0;
	xtramsize=0;
	
	last_info=0;
	last_code=0;
	
	strncpy(copyright,"Copyright (c) kX Project, 2001-2005",KX_MAX_STRING);
	if(name_)
		strncpy(name,name_,KX_MAX_STRING);
	else
		name[0]=0;
	
	strncpy(engine,"kX",KX_MAX_STRING);
	comment[0]=0;
	created[0]=0;
	if(guid_)
		strncpy(guid,guid_,KX_MAX_STRING);
	else
		guid[0]=0;
}

iDane::~iDane()
{
	if(code)
	{
		free(code);
		code=NULL;
	}
	if(info)
	{
		free(info);
		info=NULL;
	}
	itramsize=xtramsize=last_info=last_code=0;
	
	copyright[0]=0;
	name[0]=0;
	engine[0]=0;
	created[0]=0;
	guid[0]=0;
	comment[0]=0;
}

void iDane::reset(int what) // the same flags IKX_UPDATE_...
{
	if(what&IKX_UPDATE_CODE)
		if(code)
		{
			free(code);
			code=NULL;
			last_code=0;
		}
	
	if(what&IKX_UPDATE_REGS)
		if(info)
		{
			free(info);
			info=NULL;
			last_info=0;
		}
	
	if(what&IKX_UPDATE_RESOURCES)
		itramsize=xtramsize=0;
	
	if(what&IKX_UPDATE_COPYRIGHT)
		strncpy(copyright,"Copyright (c) kX Project, 2001-2005",KX_MAX_STRING);
	if(what&IKX_UPDATE_NAME)
		name[0]=0;
	if(what&IKX_UPDATE_ENGINE)
		strncpy(engine,"kX",KX_MAX_STRING);
	if(what&IKX_UPDATE_CREATED)
		created[0]=0;
	if(what&IKX_UPDATE_GUID)
		guid[0]=0;
	if(what&IKX_UPDATE_COMMENT)
		comment[0]=0;
}

word iDane::declare(char *name_,word id_,byte type_,dword p_)
{
	if(last_info==0) // need to allocate
	{
		info=(dsp_register_info *)malloc(sizeof(dsp_register_info));
	}
	else
	{
		info=(dsp_register_info *)realloc(info,(last_info+1)*sizeof(dsp_register_info));
	}
	
	if(info)
	{
		// calculate number...
		if(id_==0)
		{
			switch(type_)
			{
				case GPR_INPUT:
					id_=0x4000-1;
					break;
				default:
					id_=0x8000-1;
					break;
			}
			for(int i=0;i<last_info;i++)
			{
				if((info[i].type==GPR_INPUT) && (type_==GPR_INPUT))
					id_++;
				if((info[i].type!=GPR_INPUT) && (type_!=GPR_INPUT))
					id_++;
			}
		AGAIN:
			id_++;
			
			// check if it exists...
			for(int i=0;i<last_info;i++)
			{
				if(info[i].num==id_)
				{
					goto AGAIN;
				}
			}
		}
		
		info[last_info].num=id_;
		
		strncpy(info[last_info].name,name_,MAX_GPR_NAME);
		info[last_info].type=type_;
		info[last_info].translated=DSP_REG_NOT_TRANSLATED;
		info[last_info].p=p_;
		
		last_info++;
		return info[last_info].num;
	}
	else
		return 0;
}

int iDane::op(byte mne,word r,word a,word x,word y)
{
	if(last_code==0) // need to allocate
	{
		code=(dsp_code *)malloc(sizeof(dsp_code));
	}
	else
	{
		code=(dsp_code *)realloc(code,(last_code+1)*sizeof(dsp_code));
	}
	
	if(code)
	{
		code[last_code].op=mne;
		code[last_code].r=r;
		code[last_code].a=a;
		code[last_code].x=x;
		code[last_code].y=y;
		last_code++;
		return last_code-1;
	}
	else 
		return -1;
}


// this function populates -references- for code&info massives,
//  but duplicates any text strings used (KX_MAX_STRING)
int iDane::populate(char *name_,dsp_code* &code_,int &code_size_,
					dsp_register_info* &info_,int &info_size_,int &itramsize_,int &xtramsize_,
					char *copyright_,
					char *engine_,
					char *created_,
					char *comment_,
					char *guid_)
{
	code_=code;
	info_=info;
	info_size_=last_info*(int)sizeof(dsp_register_info);
	code_size_=last_code*(int)sizeof(dsp_code);
	
	itramsize_=itramsize;
	xtramsize_=xtramsize;
	
	strncpy(copyright_,copyright,KX_MAX_STRING);
	strncpy(name_,name,KX_MAX_STRING);
	strncpy(comment_,comment,KX_MAX_STRING);
	strncpy(engine_,engine,KX_MAX_STRING);
	strncpy(created_,created,KX_MAX_STRING);
	strncpy(guid_,guid,KX_MAX_STRING);
	
	return 0;
}
