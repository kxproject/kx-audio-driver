// kX Driver
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


#include "kx.h"

#define n_pages(a) (((a)/KX_PAGE_SIZE)+(((a)%KX_PAGE_SIZE)?1:0))

static int kx_bufmgr_alloc(kx_hw *hw,dword size,dword addr)
{
	kx_pagetable_t *pagetable = hw->pagetable;
	int index = 0;

	int numpages = n_pages((int)size);

	unsigned long flags=0;
	kx_lock_acquire(hw,&hw->pt_lock, &flags);

	while(index < MAXPAGES-1) 
	{
		if(pagetable[index].usage) 
		{
			if((pagetable[index].addr==addr) && (pagetable[index].sz==numpages))
			{
			        debug(DBUFF,"kx wdm debug: pagetable re-used [%x; %x; %x; %x]\n",
			         index,
			         pagetable[index].addr,
			         pagetable[index].usage,
			         pagetable[index].sz);

				kx_lock_release(hw,&hw->pt_lock, &flags);
				pagetable[index].usage++;
				return index;
			}

			// block is occupied
		} 
		 else
		{
			// found free
			if(pagetable[index].sz >= numpages) 
			{
				// Block size is OK
				// if free block is larger than the block requested
                                // - resize the remaining block
				if(pagetable[index].sz > numpages)
					pagetable[index + numpages].sz = pagetable[index].sz - numpages;
                                        // pagetable[index + numpages].usage should be '0'!

				pagetable[index].sz = (word)numpages;

				pagetable[index].addr=addr;
				pagetable[index].usage=1;

				kx_lock_release(hw,&hw->pt_lock, &flags);

				return index;
			}

			// too small -- skip this block
		}
		index += pagetable[index].sz;
	}

	kx_lock_release(hw,&hw->pt_lock, &flags);

	return -1;
}

static void kx_bufmgr_free(kx_hw *hw, int index)
{
	kx_pagetable_t *pagetable = hw->pagetable;
	word origsize = 0;
	unsigned long flags=0;

	if(index<0)
	 return;

	kx_lock_acquire(hw,&hw->pt_lock, &flags);

	if(pagetable[index].usage) 
	{
		pagetable[index].usage--;
		if(pagetable[index].usage==0)
		{
        		// allocated: mark as free -- already done by 'usage--;'
        		origsize=pagetable[index].sz;
        		pagetable[index].addr=0;

        		// if the next block is free, too, concatenate the blocks
        		if(!(pagetable[index + origsize].usage))
        			pagetable[index].sz += pagetable[index + origsize].sz;
        	}
	}

	kx_lock_release(hw,&hw->pt_lock, &flags);

	return;
}

int kx_bufmgr_init(kx_hw *hw)
{
	memset(hw->pagetable,0,sizeof(hw->pagetable));

        // first block: used; size=1
	hw->pagetable[0].sz = 1;
	hw->pagetable[0].usage = 1;

        // second block: unused; size=rest
        hw->pagetable[1].usage = 0;
	hw->pagetable[1].sz = MAXPAGES - 1;

	return 0;
}

int kx_bufmgr_close(kx_hw *hw)
{
 // re-test if blocks are O.K.
 if(hw->pagetable[0].sz==1 &&
    hw->pagetable[1].sz==MAXPAGES-1 &&
    hw->pagetable[0].usage==1 &&
    hw->pagetable[1].usage==0)
   return 0;
 else
 {
  // strange, but this does not work: probably, something is not freed?..
  // FIXME
  // e.g. win7 returns: 1(3), 1(0)
  // debug(DERR,"error in bufmgr_close [invalid blocks found: sz=%d(%d),usage=%d(%d)]\n",
  //  hw->pagetable[0].sz,hw->pagetable[1].sz,hw->pagetable[0].usage,hw->pagetable[1].usage);
  return -1;
 }
}


int kx_alloc_buffer(kx_hw *hw,int num)
{
	if(num<0 || num >= KX_NUMBER_OF_VOICES)
	{
		debug(DERR,"!! kx_alloc_buffer voice=%d\n",num);
		return -1;
	}

	kx_voice_buffer *buffer=&hw->voicetable[num].buffer;
	dword pageindex, pagecount;
	dword pages=n_pages(buffer->size);

	for(int t=0;t<16;t++) // try up to 16 times...
	{
        	buffer->pageindex = kx_bufmgr_alloc(hw,pages * KX_PAGE_SIZE, buffer->physical);

        	if(buffer->pageindex>=0) // success
        	   break;

            // no free PTE items
            // try to steal voices here...
            kx_steal_voices(hw,hw->voicetable[num].usage,KX_STEAL_SIZE);
    }

    if(buffer->pageindex<0)
    {
        		debug(DERR,"!! kx_alloc_buffer failed [size=%d] (addx failed)\n",buffer->size);
        		return -1;
    }

	debug(DBUFF,"kx wdm debug: allocated pagetable space: %x\n",buffer->pageindex);

	if(!(hw->voicetable[num].usage&VOICE_FLAGS_16BIT))
	{
	 if(buffer->pageindex>MAXPAGES/2)
	 {
	  debug(DLIB,"!! no <16MB area for 8bit data playback [%x]\n",buffer->pageindex);
	  kx_bufmgr_free(hw, buffer->pageindex);
	  return -2;
	 }
	}

	// Fill-in the pagetable
	if(hw->pagetable[buffer->pageindex].usage==1) // first time only
	{
	  for(pagecount=0;pagecount<pages;pagecount++) 
	  {
			dword physical;

            __int64 a;
            hw->cb.get_physical(hw->cb.call_with,buffer,pagecount*PAGE_SIZE,&a);
            physical=(dword)a;

			pageindex = buffer->pageindex + pagecount;
			((dword *) hw->virtualpagetable.addr)[pageindex] = (physical << 1) | pageindex;

			debug(DBUFF,"kx wdm debug: re-mapped logical: %p physical: %x index: %x\n",
			  (byte *)buffer->addr+pagecount*PAGE_SIZE,
			  physical,
			  pageindex);
	  }
	}

	return 0;
}

void kx_free_buffer(kx_hw *hw,int num)
{
	if(num<0 || num >= KX_NUMBER_OF_VOICES)
	{
		debug(DERR,"!! kx_free_buffer voice=%d\n",num);
		return;
	}

	kx_voice_buffer *buffer=&hw->voicetable[num].buffer;
	if(buffer->pageindex < 0)
		return;

	dword pagecount, pageindex;

	dword pages=n_pages(buffer->size);

	if(hw->pagetable[buffer->pageindex].usage==1) // last one
	{
        	for(pagecount=0;pagecount<pages;pagecount++) 
        	{
        			pageindex = buffer->pageindex + pagecount;
        			((dword *) hw->virtualpagetable.addr)[pageindex] = (hw->silentpage.dma_handle << 1) | pageindex;
        	}
        }

	kx_bufmgr_free(hw, buffer->pageindex);
	buffer->pageindex = -1;
}
