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

#undef CDC
#include "gui/kGui.h"
#include "interface/kxplugingui.h"

#include "dane/danesrc.h"

int iKXDaneSource::init(const char *fname,iKX *ikx_)
{
	iKXPlugin::init();

 	ikx=ikx_;
 	kxdevice=ikx->get_device_num();
 	instance=NULL;

 	FILE *f;
 	f=fopen(fname,"rb");
 	if(f==NULL)
 	{
 	 debug("iKXDaneSource: init - %s not found\n",fname);
 	 return -4;
 	}

 	size_t size;
        fseek(f,0L,SEEK_END);
        size=ftell(f);
        fseek(f,0L,SEEK_SET);

 	if(size<=0)
 	{
 	  debug("iKXDaneSource: init - %s size=%d\n",fname,size);
 	  fclose(f);
 	  return -5;
 	}

 	char *buff=(char *)malloc(size);
 	if(!buff)
 	{
 		debug("iKXDaneSource: init - malloc (%d) failed\n",size);
 		fclose(f);
 		return -7;
 	}
 	if(fread(buff,1,size,f)<=0)
 	{
 		debug("iKXDaneSource: init - fread failed (%x)\n",GetLastError());
 		fclose(f);
		free(buff);
 		return -8;
 	}
 	fclose(f);

        kString err;

 	switch(ikx->get_object_type(fname))
 	{
 	  case KX_OBJECT_DANE: // dane source
            {
                dsp_code *tmp_code;
                dsp_register_info *tmp_info;

                if(ikx->assemble_microcode(buff,&err,name_,
                  &tmp_code,&code_size,&tmp_info,&info_size,&itramsize,&xtramsize,
                		copyright,
                		engine,
                		created,
                		comment,
                		guid)==0)
                {
                	strncpy(name,name_,KX_MAX_STRING);
                	free(buff);

                    code=tmp_code;
                    info=tmp_info;

                	return 0;
                } else debug("iKXDaneSource: assemble failed\n");
            }
            break;
          case KX_OBJECT_RIFX: // rifx
            {
                dsp_code *tmp_code;
                dsp_register_info *tmp_info;

            	if(ikx->parse_rifx(buff,(int)size,name,&tmp_code,&code_size,&tmp_info,&info_size,
        	       	&itramsize,&xtramsize,
                		copyright,
                		engine,
                		created,
                		comment,
                		guid)==0)
                {
                	free(buff);

                    code=tmp_code;
                    info=tmp_info;

                	return 0;
                } else debug("iKXDaneSource: parse_rifx failed\n");
            }
                break;

          default:
          	debug("iKXDaneSource: invalid source type!\n");
        }

        free(buff);
        return -9;
}

int iKXDaneSource::close()
{
 if(info)
 {
  LocalFree((HLOCAL)info);
  info=NULL;
 }
 if(code)
 {
  LocalFree((HLOCAL)code);
  code=NULL;
 }
 return 0;
}

const char *iKXDaneSource::get_plugin_description(int id)
{
 switch(id)
 {
  case IKX_PLUGIN_NAME:
  	return name_; // 'original'
  case IKX_PLUGIN_COPYRIGHT:
  	return copyright;
  case IKX_PLUGIN_ENGINE:
  	return engine;
  case IKX_PLUGIN_CREATED:
  	return created;
  case IKX_PLUGIN_COMMENT:
  	return comment;
  case IKX_PLUGIN_GUID:
  	return guid;
  default:
  	return NULL;
 }
}
