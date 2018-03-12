// AudioClient.h

//  kX Project audio driver for Mac OS X
//  Created by Eugene Gavrilov.
//	Copyright 2008-2014 Eugene Gavrilov. All rights reserved.
//  https://github.com/kxproject/ (previously www.kxproject.com)

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


#ifndef _KXAUDIOCLIENT_H
#define _KXAUDIOCLIENT_H

#include "interface/kx_ioctl.h"

// #define USE_TIGER_IPC

class kXUserClient : public IOUserClient
{
		OSDeclareDefaultStructors( kXUserClient );

	private:
		kXAudioDevice		*device;
		// task_t				fTask;
		// SInt32				fOpenCount;
		
	public:
		// IOService overrides
		virtual bool start( IOService * provider );
		virtual void stop( IOService * provider );
		virtual bool willTerminate(IOService* provider, IOOptionBits options);
		virtual bool didTerminate(IOService* provider, IOOptionBits options, bool* defer);
		
		// IOUserClient overrides
		virtual bool initWithTask( task_t owningTask, void * securityID, UInt32 type,  OSDictionary * properties );
		virtual IOReturn clientClose( void );
		virtual IOReturn clientDied( void );
		
		#if defined(USE_TIGER_IPC)
		// get methods (10.4)
		static const IOExternalMethod			sMethods[kNumMethods];
		virtual IOExternalMethod* getTargetAndMethodForIndex(IOService** targetP, UInt32 index);
		#else
		static const IOExternalMethodDispatch	sMethods[kNumMethods];
		// KPI for supporting access from both 32-bit and 64-bit user processes beginning with Mac OS X 10.5.
		virtual IOReturn externalMethod(uint32_t selector, IOExternalMethodArguments* arguments,
									IOExternalMethodDispatch* dispatch, OSObject* target, void* reference);
		static IOReturn sOpenUserClient(kXUserClient* target, void* reference, IOExternalMethodArguments* arguments);
		static IOReturn sCloseUserClient(kXUserClient* target, void* reference, IOExternalMethodArguments* arguments);
		static IOReturn sStructIStructO(kXUserClient* target, void* reference, IOExternalMethodArguments* arguments);
		#endif
	
		// implementation
		virtual IOReturn StructIStructO(const void* inStruct, void* outStruct,uint32_t inStructSize, const uint32_t* outStructSize);
		virtual IOReturn openUserClient(void);
		virtual IOReturn closeUserClient(void);
};

#endif /* _KXAUDIOCLIENT_H */
