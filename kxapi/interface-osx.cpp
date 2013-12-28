//  kX Project audio driver for Mac OS X
//  Created by Eugene Gavrilov.
//	Copyright 2008 Eugene Gavrilov. All rights reserved.
//  www.kxproject.com

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

// #define debug(a...) printf("[kxapi]"a)
#define debug(a...)

#include <AvailabilityMacros.h>

iKX::iKX()
{
    iterator=0;
	service=0;
	connect=0;
	
    is_10k2=(dword)-1;
    device_num=-1;
    device_name[0]=0;
}

iKX::~iKX()
{
    close();
}

int iKX::init(int id)
{
    close();
	
    int prev_id=id;
    kern_return_t	ret;
	
    // Look up the objects we wish to open. This example uses simple class
    // matching (IOServiceMatching()) to find instances of the class defined by the kext.
	// Because Mac OS X has no weak-linking support in the kernel, the only way to
	// support mutually-exclusive KPIs is to provide separate kexts. This in turn means that the
	// separate kexts must have their own unique CFBundleIdentifiers and I/O Kit class names.
	// This sample shows how to do this in the SimpleUserClient and SimpleUserClient_10.4 Xcode targets.
	// From the userland perspective, a process must look for any of the class names it is prepared to talk to.
	
    // This creates an io_iterator_t of all instances of our driver that exist in the I/O Registry.
    ret = IOServiceGetMatchingServices(kIOMasterPortDefault, IOServiceMatching("com_kXProject_driver_kXAudioDriver"), &iterator);
    
	if(ret == KERN_SUCCESS && iterator)
	{
		while ((service = IOIteratorNext(iterator)) != IO_OBJECT_NULL)
		{
            if(id!=0)
            { id--; continue; }
			
			// found?
			// Instantiate a connection to the user client.
			ret = IOServiceOpen(service, mach_task_self(), 0, &connect);
			if (ret == KERN_SUCCESS)
			{
				// execute 'userOpen'
#if MAC_OS_X_VERSION_MIN_REQUIRED <= MAC_OS_X_VERSION_10_4
				ret = IOConnectMethodScalarIScalarO(connect, kOpenUserClient, 0, 0, NULL, NULL);
#else
				ret = IOConnectCallScalarMethod(connect, kOpenUserClient, NULL, 0, NULL, NULL);
#endif
				if(ret==0)
				{
					// success
					// initialize instance data
					
					device_num=prev_id;
					get_string(KX_STR_CARD_NAME,device_name);
					get_dword(KX_DWORD_IS_10K2,&is_10k2);
					
					return 0;
				}
				
				debug("[create] failed to userOpen\n");
			}
			else
				debug("[create] failed to open service\n");
			
			break;
		}
		
		// not found:
		
		debug("[create] No matching drivers found\n");
		
		// Release the io_iterator_t now that we're done with it.
		IOObjectRelease(iterator);
	}
	else
	{
		debug("[create] IOServiceGetMatchingServices returned 0x%08x, iterator: %d\n", ret, iterator);
	}
    return -1;
}


int iKX::close(void)
{
	// send userClose
	if(connect)
	{
#if MAC_OS_X_VERSION_MIN_REQUIRED <= MAC_OS_X_VERSION_10_4
		IOConnectMethodScalarIScalarO(connect, kCloseUserClient, 0, 0, NULL, NULL);
#else
		IOConnectCallScalarMethod(connect, kCloseUserClient, NULL, 0, NULL, NULL);
#endif
	}
	// free interface
	if(connect)
	{
		IOServiceClose(connect);
		connect=0;
	}
	
	// free iterator
	if(iterator)
	{
		IOObjectRelease(iterator);
		iterator=0;
	}
	
	debug("[close]\n");
	
	return 0;
}

int iKX::ctrl(dword prop,void *buff,int bsize,int *ret_bytes)
{
	dword *mem=(dword *)malloc(bsize+sizeof(dword));
	if(mem)
	{
		*mem=prop;
		memcpy(mem+1,buff,bsize);
		
		size_t out_size=bsize+sizeof(dword);
		
#if MAC_OS_X_VERSION_MIN_REQUIRED <= MAC_OS_X_VERSION_10_4
		int ret = IOConnectMethodStructureIStructureO(connect,				// an io_connect_t returned from IOServiceOpen().
													  kTransaction,			// selector of the function to be called via the user client.
													  bsize+sizeof(dword),		// the size of the input structure parameter.		   
													  &out_size,				// pointer to the size of the output structure parameter.		   
													  mem,						// pointer to the input struct parameter.
													  mem						// pointer to the output struct parameter.
													  );
#else
		int ret = IOConnectCallStructMethod(connect,
											kTransaction,
											mem,
											bsize+sizeof(dword),
											mem,
											&out_size);																						
											
#endif
		out_size-=sizeof(dword);
		
		if(ret_bytes && ret==0)
		{
			memcpy(buff,mem+1,bsize);
			*ret_bytes=(int)out_size;
		}
		free(mem);
		
		return ret;
	}
	else	
		return -ENOMEM;
}

int iKX::generate_guid(char *guid) // should be at lease KX_MAX_STRING
{
    // FIXME!
	
	strcpy(guid,"UNIMPLEMENTED");
	return -1;
}

// =======================
// end of OS-specific code
// =======================
