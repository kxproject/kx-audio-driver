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


#include "stdafx.h"
#include "stdinc.h"

#include "asio/asiosys.h"
#include "asio/asio.h"
#include "asio/asiodrivers.h"

#include "debug.h"

extern AsioDrivers* asioDrivers;
bool __stdcall loadAsioDriver(char *name);

static long asioMessages(long , long , void* , double* )
{
 return 1;
}

void launch_asio(void)
{
 static int in_use=0;

 debug("kxmixer: launch asio\n");

 if(in_use==0)
 {
        in_use=1;

        char asio_name[KX_MAX_STRING];

        sprintf(asio_name,"kX ASIO %s",manager->get_ikx()->get_device_name());

	if (loadAsioDriver (asio_name))
	{
		ASIODriverInfo driverInfo;
		ASIOBufferInfo bufferInfos[2];

		// initialize the driver
		if (ASIOInit (&driverInfo) == ASE_OK)
		{
		 ASIOCallbacks asioCallbacks;
		 memset(&asioCallbacks,0,sizeof(asioCallbacks));

		 asioCallbacks.asioMessage = &asioMessages;

		 long minSize=0,maxSize=0,preferredSize=0,granularity=0;

		 if(ASIOGetBufferSize(&minSize, &maxSize, &preferredSize, &granularity) == ASE_OK)
		 {
         		 if(ASIOCreateBuffers(bufferInfos,0,
         		         preferredSize, &asioCallbacks)==ASE_OK)
         		 {
         		   ASIOControlPanel();
         		   ASIOExit();
         		 }
         	 }
		}
		asioDrivers->removeCurrentDriver();
	}
	else
	 debug("kxmixer: cannot launch asio driver\n");

        delete asioDrivers;
        asioDrivers=NULL;

	in_use=0;
 }	
}
