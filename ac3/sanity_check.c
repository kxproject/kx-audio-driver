// KX Driver
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


// source code license / origin is unknown; probably, public domain

#include "stdafx.h"

void sanity_check_init(syncinfo_t *syncinfo, bsi_t *bsi, audblk_t *audblk)
{
	syncinfo->magic = AC3_MAGIC_NUMBER;
	bsi->magic = AC3_MAGIC_NUMBER;
	audblk->magic1 = AC3_MAGIC_NUMBER;
	audblk->magic2 = AC3_MAGIC_NUMBER;
	audblk->magic3 = AC3_MAGIC_NUMBER;
}

void sanity_check(syncinfo_t *syncinfo, bsi_t *bsi, audblk_t *audblk,int *error_flag,ac3_state *state)
{
	int i;

	if(syncinfo->magic != AC3_MAGIC_NUMBER)
	{
		// debug(DAC3,"\n** Sanity check failed -- syncinfo magic number **");
		*error_flag = 1;
	}
	
	if(bsi->magic != AC3_MAGIC_NUMBER)
	{
		// debug(DAC3,"\n** Sanity check failed -- bsi magic number **");
		*error_flag = 1;
	}

	if(audblk->magic1 != AC3_MAGIC_NUMBER)
	{
		// debug(DAC3,"\n** Sanity check failed -- audblk magic number 1 **"); 
		*error_flag = 1;
	}

	if(audblk->magic2 != AC3_MAGIC_NUMBER)
	{
		// debug(DAC3,"\n** Sanity check failed -- audblk magic number 2 **"); 
		*error_flag = 1;
	}

	if(audblk->magic3 != AC3_MAGIC_NUMBER)
	{
		// debug(DAC3,"\n** Sanity check failed -- audblk magic number 3 **"); 
		*error_flag = 1;
	}

	for(i = 0;i < 5 ; i++)
	{
		if (audblk->fbw_exp[i][255] !=0 || audblk->fbw_exp[i][254] !=0 || 
				audblk->fbw_exp[i][253] !=0)
		{
			// debug(DAC3,"\n** Sanity check failed -- fbw_exp out of bounds **"); 
			*error_flag = 1;
		}

		if (audblk->fbw_bap[i][255] !=0 || audblk->fbw_bap[i][254] !=0 || 
				audblk->fbw_bap[i][253] !=0)
		{
			// debug(DAC3,"\n** Sanity check failed -- fbw_bap out of bounds **"); 
			*error_flag = 1;
		}

	}

	if (audblk->cpl_exp[255] !=0 || audblk->cpl_exp[254] !=0 || 
			audblk->cpl_exp[253] !=0)
	{
		// debug(DAC3,"\n** Sanity check failed -- cpl_exp out of bounds **"); 
		*error_flag = 1;
	}

	if (audblk->cpl_bap[255] !=0 || audblk->cpl_bap[254] !=0 || 
			audblk->cpl_bap[253] !=0)
	{
		// debug(DAC3,"\n** Sanity check failed -- cpl_bap out of bounds **"); 
		*error_flag = 1;
	}

	if (audblk->cplmant[255] !=0 || audblk->cplmant[254] !=0 || 
			audblk->cplmant[253] !=0)
	{
		// debug(DAC3,"\n** Sanity check failed -- cpl_mant out of bounds **"); 
		*error_flag = 1;
	}

	if ((audblk->cplinu == 1) && (audblk->cplbegf > (audblk->cplendf+2)))
	{
		// debug(DAC3,"\n** Sanity check failed -- cpl params inconsistent **"); 
		*error_flag = 1;
	}

	for(i=0; i < bsi->nfchans; i++)
	{
		if((audblk->chincpl[i] == 0) && (audblk->chbwcod[i] > 60))
		{
			// debug(DAC3,"\n** Sanity check failed -- chbwcod too big **"); 
			*error_flag = 1;
		}
	}

	return;
}	
