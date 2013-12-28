// kX Driver
// Copyright (c) Eugene Gavrilov, 2001-2014
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



	printf("Full 10kx state dump -------\n\nPCI STATE -----\n");
	printf("HCFG= %x\n",kx_readfn0(hw,HCFG_K1));
        printf("IPR= %x\n",kx_readfn0(hw,IPR));
        printf("WC[first]= %x\n",kx_readfn0(hw,WC));
	printf("INTE: %x\n",kx_readfn0(hw,INTE));

        printf("MUDATA= %x [dword] -- set DAC?\n",kx_readfn0(hw,MUDATA_K1));
        printf("  MUDATA[byte]: %x\n",inp(hw->port + MUSTAT_K1));
        printf("  MUCmd[byte]: %x\n",inp(hw->port + MUCMD_K1));
        printf("  TIMER[word]: %x\n",inpw(hw->port + TIMER));
        printf("  TIMER[byte]: %x\n",inp(hw->port + TIMER));
        printf("  AC97[byte]: %x\n",inp(hw->port + AC97ADDRESS));
        printf("  @1f[byte]: %x\n",inp(hw->port + 0x1f));
        printf("dword @AC97DATA: %x\n",inp(hw->port + AC97DATA));

        printf("\n\n");

	printf("AC97:\n");
	for(i=0;i<0x7f;i+=2)
	 printf("  [%x] = %x\n",i,kx_ac97read(hw,(byte)i));

	printf("\nA97Slot/PCB: %x\n",kx_readptr(hw,PCB,0));

        printf("WC[last]= %x\n",kx_readfn0(hw,WC));

        printf("\n---- Complete PTR list\n\n");

        for(int chn=0;chn<2;chn++)
        {
         for(i=0;i<0x100;i++)
          printf("[reg=%x] val=%x (chn=%d)\n",i,kx_readptr(hw,i,chn),chn);
        }

        printf("\nThat's all! :)\n");

