// kX WDM Audio Driver
// Copyright (c) Eugene Gavrilov, 2001-2014.
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


#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdarg.h>
#include <stdio.h>

#define PC_IMPLEMENTATION
#include <portcls.h>
#include <ksdebug.h>
#include <DMusicKS.h>
#include <dmerror.h>        // Error codes
#include <dmdls.h>          // DLS definitions
#include <kcom.h>

#include <drmk.h>
#include <stdunk.h> // assumes punknown.h

#include "driver/kx.h"
#include "driver/ac3.h"
// cannot declare this here due to precompiled headers via common.h
// #include "interface/guids.h"

#undef debug
#if defined(KX_DEBUG_FUNC) && defined(KX_DEBUG)
 #define debug debug_func
 int debug_func(int where,const char *__format, ... );
#else
 #define debug (void)
#endif

#ifndef SAFE_DESTRUCTORS
 #define SAFE_DESTRUCTORS void _self_delete(void){delete this;}
#endif

class CAdapterCommon;
class CMiniportUart;
class CMiniportTopology;
class CMiniportWave;
class CMiniportWaveHQ;
class CMiniportSynth;
class CMiniportCtrl;

#define WAVEINSTREAM_MAGIC	(0x23958373^KX_VERSION_DWORD)
#define WAVEOUTSTREAM_MAGIC	(0x58846932^KX_VERSION_DWORD)
#define WAVEHQSTREAM_MAGIC	(0x11284372^KX_VERSION_DWORD)
#define WAVE_MAGIC		(0x98492746^KX_VERSION_DWORD)
#define WAVEHQ_MAGIC		(0x33256416^KX_VERSION_DWORD)
#define ADAPTER_MAGIC		(0x28159738^KX_VERSION_DWORD)
#define TOPOLOGY_MAGIC		(0x93827641^KX_VERSION_DWORD)
#define SYNTH_MAGIC		(0x48827389^KX_VERSION_DWORD)
#define SYNTHSTREAM_MAGIC	(0x65991243^KX_VERSION_DWORD)
#define CTRL_MAGIC		(0x48832954^KX_VERSION_DWORD)
#define CTRLSTREAM_MAGIC	(0x39892342^KX_VERSION_DWORD)
#define ASIO_MAGIC		(0x54882173^KX_VERSION_DWORD)
#define GSIF_MAGIC		(0x96657482^KX_VERSION_DWORD)

#pragma hdrstop()

#define DMA_DEFAULT_NOTIFY		10

#define WAVE_NODE_WAVE_OUT		0x0
#define WAVE_NODE_OUTPUT		0x1
#define WAVE_NODE_WAVE_IN		0x2
#define WAVE_NODE_INPUT			0x3

#define WAVE_NODE_SPDIF			0x4
#define WAVE_NODE_SPDIFOUTPUT   0x5

#include "wdm/adapter.h"
#include "wdm/miniuart.h"
#include "wdm/minitopo.h"

#include "kx3d/kx3d.h"

#include "wdm/miniwave.h"
#include "wdm/minisynth.h"
#include "wdm/minictrl.h"

extern "C" IDmaChannel* CreateCDmaChannel(ULONG size, PHYSICAL_ADDRESS highestAddress);

NTSTATUS create_topology(OUT PUNKNOWN *Unknown,IN REFCLSID,IN PUNKNOWN OPTIONAL UnknownOuter,IN POOL_TYPE PoolType);
NTSTATUS create_wave(OUT PUNKNOWN *Unknown,IN REFCLSID,IN PUNKNOWN OPTIONAL UnknownOuter,IN POOL_TYPE PoolType);
NTSTATUS create_waveHQ(OUT PUNKNOWN *Unknown,IN REFCLSID,IN PUNKNOWN OPTIONAL UnknownOuter,IN POOL_TYPE PoolType);
NTSTATUS create_synth(OUT PUNKNOWN *Unknown,IN REFCLSID,IN PUNKNOWN OPTIONAL UnknownOuter,IN POOL_TYPE PoolType);
NTSTATUS create_uart(OUT PUNKNOWN *Unknown,IN REFCLSID,IN PUNKNOWN OPTIONAL UnknownOuter,IN POOL_TYPE PoolType);
NTSTATUS create_ctrl(OUT PUNKNOWN *Unknown,IN REFCLSID,IN PUNKNOWN OPTIONAL UnknownOuter,IN POOL_TYPE PoolType);
NTSTATUS process_property(CAdapterCommon *adapter,kx_hw *hw,PPCPROPERTY_REQUEST PropertyRequest,dword where);

NTSTATUS rest_registry(PREGISTRYKEY SettingsKey,PVOID KeyInfo,char *name, dword *value);

extern KDPC dpc_ac3,dpc_irq;
KDEFERRED_ROUTINE dpc_ac3_func;
KDEFERRED_ROUTINE dpc_irq_func;

void init_gsif(IN PDRIVER_OBJECT DriverObject,IN PDEVICE_OBJECT DeviceObject);
void register_gsif(CAdapterCommon *adapter);
void close_gsif(CAdapterCommon *adapter);

typedef struct
{
 void *addr;
 PMDL mdl;
 size_t size;
}lmem;

int allocate_large_memory(lmem *m,size_t size,MEMORY_CACHING_TYPE cache);
int free_large_memory(lmem *m);

#endif
