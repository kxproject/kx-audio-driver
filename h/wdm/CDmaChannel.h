// kX WDM Audio Driver
// Copyright (c) Eugene Gavrilov, 2001-2014.
// CDmaChannel implementation by Markus Hosch <markus.hosch@gmx.net> (c) 2009
// updates by Eugene Gavrilov
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


#ifndef __cdmachannel_h__
#define __cdmachannel_h__

#define PC_IMPLEMENTATION

#include <ntddk.h>
#include <portcls.h>

#ifdef __cplusplus

class CDmaChannel : public IDmaChannel
{
public:
	static IDmaChannel* Create(ULONG size, PHYSICAL_ADDRESS highestAddress);
	~CDmaChannel();

	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP_(HRESULT) QueryInterface(REFIID iid, void** obj);

	IMP_IDmaChannel;

        void operator delete(void*);
        void* operator new(size_t n);

        static const ULONG memTag = '10K1';

private:
	CDmaChannel(PVOID buffer, ULONG size);

	ULONG ref;
	PVOID buffer;
	PHYSICAL_ADDRESS physicalBufstart;
	ULONG allocatedSize;
	ULONG bufferSize;
};

extern "C"
{

#endif // __cplusplus

IDmaChannel* CreateCDmaChannel(ULONG size, PHYSICAL_ADDRESS highestAddress);

#ifdef __cplusplus
}
#endif

#endif // __cdmachannel_h__

