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


#include "common.h"

int allocate_large_memory(lmem *m,size_t size,MEMORY_CACHING_TYPE cache)
{
 memset(m,0,sizeof(lmem));

 PHYSICAL_ADDRESS zero={0,0};
 PHYSICAL_ADDRESS limit; limit.LowPart=0x7fffffff; limit.HighPart=0; // 2Gb limit

 m->mdl=MmAllocatePagesForMdl(zero,limit,zero,size);

 if(m->mdl)
 {
//  print MDL info:
//  debug(DWDM,"lmem: allocated mdl: size: %d fl: %x va: %x start: %x bytecnt: %d byteoff: %d\n",
//   m->mdl->Size,m->mdl->MdlFlags,m->mdl->MappedSystemVa,m->mdl->StartVa,m->mdl->ByteCount,m->mdl->ByteOffset);

  if(m->mdl->ByteCount<size)
  {
   debug(DWDM,"lmem: allocation failed [allocated only %d bytes]\n",m->mdl->ByteCount);
   free_large_memory(m);
   return -3;
  }

  m->mdl->MdlFlags|=MDL_MAPPING_CAN_FAIL;
  m->mdl->MdlFlags|=MDL_PAGES_LOCKED;

  __try 
  {
    m->addr=MmMapLockedPagesSpecifyCache(m->mdl,KernelMode,cache,NULL,FALSE,NormalPagePriority);
  }
   __except(EXCEPTION_EXECUTE_HANDLER)
  {
    m->addr=NULL;
  }

  if(m->addr!=0)
  {
//   debug(DWDM,"lmem: allocated successfully\n");
//   print MDL info:
//   debug(DWDM,"lmem: mapped mdl: size: %d fl: %x va: %x start: %x bytecnt: %d byteoff: %d\n",
//    m->mdl->Size,m->mdl->MdlFlags,m->mdl->MappedSystemVa,m->mdl->StartVa,m->mdl->ByteCount,m->mdl->ByteOffset);

   return 0;
  }
  else
  {
   debug(DWDM,"lmem: pages could not be mapped\n");
   free_large_memory(m);
   return -1;
  }
 }
  else
   debug(DWDM,"lmem: cannot allocate MDL\n");

 return -2;
}

int free_large_memory(lmem *m)
{
 if(m->mdl)
 {
  if(m->addr)
  {
   MmUnmapLockedPages(m->addr,m->mdl);
   m->addr=NULL;
  }

  MmFreePagesFromMdl(m->mdl);
  ExFreePool(m->mdl);
  m->mdl=NULL;
 }
 return 0;
}
