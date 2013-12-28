// kX WDM Audio Driver
// Copyright (c) Eugene Gavrilov, 2001-2014.
// CDmaChannel implementation by Markus Hosch <markus.hosch@gmx.net> (c) 2009
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


#include "common.h"
#include "wdm/CDmaChannel.h"
#include <winerror.h>

extern "C" NTKERNELAPI PHYSICAL_ADDRESS MmGetPhysicalAddress (__in PVOID BaseAddress);

IDmaChannel* CreateCDmaChannel(ULONG size, PHYSICAL_ADDRESS highestAddress)
{
   return CDmaChannel::Create(size, highestAddress);
}

IDmaChannel* CDmaChannel::Create(ULONG size, PHYSICAL_ADDRESS highestAddress)
{
	PVOID buffer = MmAllocateContiguousMemory(size, highestAddress);
   
	if(buffer != NULL)
	{
		return new CDmaChannel(buffer, size);
	}
	else
	{
        debug(DWDM,"CDmaChannel::Create: Allocating continguous memory failed\n");
		return NULL;
	}
}

void* CDmaChannel::operator new(size_t n)
{
   void* mem = ExAllocatePoolWithTag(NonPagedPool, n, memTag);
   RtlZeroMemory(mem, n);
   return mem;
}

void CDmaChannel::operator delete(void* mem)
{
   if(mem != NULL)
   {
      ExFreePoolWithTag(mem, memTag);
   }
}

CDmaChannel::CDmaChannel(PVOID buffer, ULONG size)
:  buffer(buffer), allocatedSize(size), physicalBufstart(MmGetPhysicalAddress(buffer)),
   bufferSize(size), ref(1)
{
   // debug(DWDM,"CDmaChannel::CDmaChannel: system addr: %p physical: %llx\n",buffer,physicalBufstart.QuadPart);
}

CDmaChannel::~CDmaChannel()
{
	FreeBuffer();
}

STDMETHODIMP_(NTSTATUS) CDmaChannel::AllocateBuffer(ULONG newBufferSize, PPHYSICAL_ADDRESS constraint OPTIONAL)
{
    // free current buffer
    FreeBuffer();

    // re-allocate
    buffer = MmAllocateContiguousMemory(newBufferSize,*constraint);
    if(buffer)
    {
        allocatedSize=newBufferSize;
        bufferSize=newBufferSize;
        physicalBufstart=MmGetPhysicalAddress(buffer);

        return STATUS_SUCCESS;
    }
    else
    {
        debug(DWDM,"!! CDmaChannel::AllocateBuffer: could not re-allocate buffer (%d->%d)\n",allocatedSize,bufferSize);
        allocatedSize=0;
        bufferSize=0;

        return STATUS_INSUFFICIENT_RESOURCES;
    }
}

STDMETHODIMP_(ULONG) CDmaChannel::AllocatedBufferSize()
{
	return allocatedSize;
}

STDMETHODIMP_(ULONG) CDmaChannel::BufferSize()
{
	return bufferSize;
}

// possible sanity checks: see if source is within the common buffer, check count
void CDmaChannel::CopyFrom(PVOID destination, PVOID source, ULONG count)
{
	if(buffer != NULL)
	{
		RtlCopyMemory(destination, source, count);
	}
}

// possible sanity checks: see if destination is within the common buffer, check count
void CDmaChannel::CopyTo(PVOID destination, PVOID source, ULONG count)
{
	if(buffer != NULL)
	{
		RtlCopyMemory(destination, source, count);
	}
}

void CDmaChannel::FreeBuffer()
{
	if(buffer != NULL)
	{
		MmFreeContiguousMemory(buffer);
		buffer = NULL;
	}
}

// stub; function is not required by NewStream and we do not have an adapter object anyways
STDMETHODIMP_(PADAPTER_OBJECT) CDmaChannel::GetAdapterObject()
{
	return NULL;
}

STDMETHODIMP_(ULONG) CDmaChannel::MaximumBufferSize()
{
	return allocatedSize;
}

STDMETHODIMP_(PHYSICAL_ADDRESS) CDmaChannel::PhysicalAddress()
{
	return physicalBufstart;
}

void CDmaChannel::SetBufferSize(ULONG size)
{
	bufferSize = size;
}

PVOID CDmaChannel::SystemAddress()
{
	return buffer;
}

// stub since it is not required by NewStream
STDMETHODIMP_(ULONG) CDmaChannel::TransferCount()
{
	return 0;
}

STDMETHODIMP_(HRESULT) CDmaChannel::QueryInterface(REFIID iid, void** obj)
{
   if(!obj) return E_POINTER;
   *obj = NULL;

	if(IsEqualGUIDAligned(iid, IID_IDmaChannel))
	{
		*obj = this;
	}
	else if(IsEqualGUIDAligned(iid, IID_IUnknown))
	{
		*obj = static_cast<IUnknown*>(this);
	}

   if(*obj)
   {
      ++ref;
      return S_OK;
   }
	else return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CDmaChannel::AddRef()
{
	return ++ref;
}

STDMETHODIMP_(ULONG) CDmaChannel::Release()
{
	if(--ref == 0)
	{
		delete this;
		return 0;
	}
	else return ref;
}
