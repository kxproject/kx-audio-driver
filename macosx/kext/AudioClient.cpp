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


#include "AudioDevice.h"
#include "AudioEngine.h"
#include "AudioClient.h"

#define DBGCLASS "kXUClient"

#undef debug
#include "cedebug.h"

#define super IOUserClient
OSDefineMetaClassAndStructors( kXUserClient , IOUserClient );

// Sentinel values for the method dispatch table
enum {
    kMethodObjectThis = 0,
    kMethodObjectProvider
};

#if defined(USE_TIGER_IPC)
const IOExternalMethod kXUserClient::sMethods[kNumMethods] = 
{
	{   // kClientOpen
		(IOService *) kMethodObjectThis,							// Target object is this user client.
		(IOMethod) &kXUserClient::openUserClient,					// Method pointer.
		kIOUCScalarIScalarO,										// Scalar Input, Scalar Output.
		0,															// No scalar input values.
		0															// No scalar output values.
	},
	{   // kClientClose
		(IOService *) kMethodObjectThis,							// Target object is this user client.
		(IOMethod) &kXUserClient::closeUserClient,					// Method pointer.
		kIOUCScalarIScalarO,										// Scalar Input, Scalar Output.
		0,															// No scalar input values.
		0															// No scalar output values.
	},
	{   // kTransaction
		(IOService *) kMethodObjectThis,							// Target object is this user client.
		(IOMethod) &kXUserClient::StructIStructO,					// Method pointer.
		kIOUCStructIStructO,										// Struct Input, Struct Output.
		kIOUCVariableStructureSize,									// The size of the input struct.
		kIOUCVariableStructureSize									// The size of the output struct.
	}
};

// This is the legacy approach which only supports 32-bit user processes.
IOExternalMethod* kXUserClient::getTargetAndMethodForIndex(IOService** target, UInt32 index)
{
    // Make sure that the index of the function we're calling actually exists in the function table.
    if (index < (UInt32) kNumMethods) {
		if (sMethods[index].object == (IOService *) kMethodObjectThis) {
			*target = this;	   
        }
		else {
			*target = device;	   // this is always false, because we have no kMethodObjectProvider methods
		}
		return (IOExternalMethod *) &sMethods[index];
    }
	else {
		*target = NULL;
		return NULL;
	}
}

#else // MAC_OS_X_VERSION_10_5
const IOExternalMethodDispatch kXUserClient::sMethods[kNumMethods] = {
	{   // kClientOpen
		(IOExternalMethodAction) &kXUserClient::sOpenUserClient,				// Method pointer.
		0,																		// No scalar input values.
		0,																		// No struct input value.
		0,																		// No scalar output values.
		0																		// No struct output value.
	},
	{   // kClientClose
		(IOExternalMethodAction) &kXUserClient::sCloseUserClient,				// Method pointer.
		0,																		// No scalar input values.
		0,																		// No struct input value.
		0,																		// No scalar output values.
		0																		// No struct output value.
	},
	{   // kTransaction
		(IOExternalMethodAction) &kXUserClient::sStructIStructO,				// Method pointer.
		0,																		// No scalar input values.
		kIOUCVariableStructureSize,												// The size of the input struct.
		0,																		// No scalar output values.
		kIOUCVariableStructureSize												// The size of the output struct.
	}
};

IOReturn kXUserClient::externalMethod(uint32_t selector, IOExternalMethodArguments* arguments,
												   IOExternalMethodDispatch* dispatch, OSObject* target, void* reference)

{
	// IOLog("%s[%p]::%s(%d, %p, %p, %p, %p)\n", getName(), this, __FUNCTION__,
	//	  selector, arguments, dispatch, target, reference);
	
    if (selector < (uint32_t) kNumMethods) {
        dispatch = (IOExternalMethodDispatch *) &sMethods[selector];
        
		if (!target) {
			target=this;	// we have no kMethodObjectProvider methods
		}
    }
	
	return super::externalMethod(selector, arguments, dispatch, target, reference);
}

#endif

// There are two forms of IOUserClient::initWithTask, the second of which accepts an additional OSDictionary* parameter.
// If your user client needs to modify its behavior when it's being used by a process running using Rosetta,
// you need to implement the form of initWithTask with this additional parameter.
// initWithTask is called as a result of the user process calling IOServiceOpen.
bool kXUserClient::initWithTask( task_t owningTask, void * securityID, UInt32 type,  OSDictionary * properties )
{
    // debug(DBGCLASS"[%p]::initWithTask(type %x)\n", this, (unsigned)type);
    
	device = NULL;
	
    return super::initWithTask( owningTask, securityID, type, properties );
}

// start is called after initWithTask as a result of the user process calling IOServiceOpen.
bool kXUserClient::start( IOService * provider )
{
    // debug(DBGCLASS"[%p]::start\n",this);
	
    if( !super::start( provider ))
        return( false );
	
    // Our provider is the kXAudioDevice object.
    assert( OSDynamicCast( kXAudioDevice, provider ));
    device = (kXAudioDevice *) provider;
	
	// It's important not to call super::start if some previous condition
	// (like an invalid provider) would cause this function to return false. 
	// I/O Kit won't call stop on an object if its start function returned false.
	
    return true;
}

// stop will be called during the termination process, and should free all resources
// associated with this client.
void kXUserClient::stop( IOService * provider )
{
    // debug(DBGCLASS"[%p]::stop\n",this);
	
	// free resources here
	
    super::stop( provider );
}


// Kill ourselves off if the client closes its connection or the client dies.
IOReturn kXUserClient::clientClose( void )
{
	// Defensive coding in case the user process called IOServiceClose
	// without calling closeUserClient first.
    closeUserClient();
    
	// Inform the user process that this user client is no longer available. This will also cause the
	// user client instance to be destroyed.
	//
	// terminate would return false if the user process still had this user client open.
	// This should never happen in our case because this code path is only reached if the user process
	// explicitly requests closing the connection to the user client.
	bool success = terminate();
	if (!success) {
		debug("!! "DBGCLASS"[%p]::clientClose: terminate() failed\n", this);
	}
	
    // DON'T call super::clientClose, which just returns kIOReturnUnsupported.
    
    return kIOReturnSuccess;
}

IOReturn kXUserClient::clientDied( void )
{
    IOReturn result = kIOReturnSuccess;
	
	debug(DBGCLASS"[%p]::clientDied\n",this);
	
    // The default implementation of clientDied just calls clientClose.
    result = super::clientDied();
	
    return result;
}

// willTerminate is called at the beginning of the termination process. It is a notification
// that a provider has been terminated, sent before recursing up the stack, in root-to-leaf order.
//
// This is where any pending I/O should be terminated. At this point the user client has been marked
// inactive and any further requests from the user process should be returned with an error.
bool kXUserClient::willTerminate(IOService* provider, IOOptionBits options)
{
	debug(DBGCLASS"[%p]::willTerminate (%p, %x)\n", this, provider, (unsigned)options);
	
	return super::willTerminate(provider, options);
}


// didTerminate is called at the end of the termination process. It is a notification
// that a provider has been terminated, sent after recursing up the stack, in leaf-to-root order.
bool kXUserClient::didTerminate(IOService* provider, IOOptionBits options, bool* defer)
{
	debug(DBGCLASS"[%p]::didTerminate (%p, %x, %p)\n", this, provider, (unsigned)options, defer);
	
	// If all pending I/O has been terminated, close our provider. If I/O is still outstanding, set defer to true
	// and the user client will not have stop called on it.
	
	closeUserClient();
	*defer = false;
	
	return super::didTerminate(provider, options, defer);
}

#if !defined(USE_TIGER_IPC)
IOReturn kXUserClient::sOpenUserClient(kXUserClient* target, void* reference, IOExternalMethodArguments* arguments)
{
    return target->openUserClient();
}
#endif

IOReturn kXUserClient::openUserClient(void)
{
    IOReturn	result = kIOReturnSuccess;
	
	// debug(DBGCLASS"[%p]::openUserClient()\n", this);
    
    if (device == NULL || isInactive())
	{
		// Return an error if we don't have a provider. This could happen if the user process
		// called openUserClient without calling IOServiceOpen first. Or, the user client could be
		// in the process of being terminated and is thus inactive.
		debug("!! "DBGCLASS"[%p]::openUserClient: inactive\n",this);
        result = kIOReturnNotAttached;
	}
    else if (!device->open(this))
	{
		debug("!! "DBGCLASS"[%p]::openUserClient: exclusive access\n",this);
		
		// The most common reason this open call will fail is because the provider is already open
		// and it doesn't support being opened by more than one client at a time.
		result = kIOReturnExclusiveAccess;
	}
	
    return result;
}

#if !defined(USE_TIGER_IPC)
IOReturn kXUserClient::sCloseUserClient(kXUserClient* target, void* reference, IOExternalMethodArguments* arguments)
{
    return target->closeUserClient();
}
#endif

IOReturn kXUserClient::closeUserClient(void)
{
    IOReturn	result = kIOReturnSuccess;
	
    if (device == NULL)
	{
		// Return an error if we don't have a provider. This could happen if the user process
		// called closeUserClient without calling IOServiceOpen first. 
		result = kIOReturnNotAttached;
		debug("!! "DBGCLASS"[%p]::closeUserClient(): not opened\n", this);
	}
	else if (device->isOpen(this))
	{
		// debug(DBGCLASS"[%p]::closeUserClient()\n", this);
		
		// Make sure we're the one who opened our provider before we tell it to close.
		device->close(this);
	}
	else {
		result = kIOReturnNotOpen;
		// debug("!! "DBGCLASS"[%p]::closeUserClient(): returning kIOReturnNotOpen.\n", this);
	}
	
    return result;
}

#if !defined(USE_TIGER_IPC)
IOReturn kXUserClient::sStructIStructO(kXUserClient* target, void* reference, IOExternalMethodArguments* arguments)
{
    return target->StructIStructO(arguments->structureInput,
								  arguments->structureOutput,
								  (uint32_t) arguments->structureInputSize,
								  (const uint32_t*) &arguments->structureOutputSize);
}
#endif


IOReturn kXUserClient::StructIStructO(const void* inStruct, void* outStruct,uint32_t inStructSize, const uint32_t* outStructSize)
{
	IOReturn	result;
	
	if (device == NULL || isInactive())
	{
		// Return an error if we don't have a provider. This could happen if the user process
		// called StructIStructO without calling IOServiceOpen first. Or, the user client could be
		// in the process of being terminated and is thus inactive.
		result = kIOReturnNotAttached;
	}
	else if (!device->isOpen(this))
	{
		// Return an error if we do not have the driver open. This could happen if the user process
		// did not call openUserClient before calling this function.
		result = kIOReturnNotOpen;
	}
	else
	{
		result = device->user_request(inStruct, outStruct, inStructSize, outStructSize);
	}
    
    return result;
}
