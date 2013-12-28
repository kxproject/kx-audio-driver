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

static void exp_unpack_ch(uint_16 type,uint_16 expstr,uint_16 ngrps,uint_16 initial_exp, 
		uint_16 exps[], uint_16 *dest, int *error_flag,ac3_state *state);

void exponent_unpack( bsi_t *bsi, audblk_t *audblk,int *error_flag,ac3_state *state)
{
	uint_16 i;

	for(i=0; i< bsi->nfchans; i++)
		exp_unpack_ch(UNPACK_FBW, audblk->chexpstr[i], audblk->nchgrps[i], audblk->exps[i][0], 
				&audblk->exps[i][1], audblk->fbw_exp[i],error_flag,state);

	if(audblk->cplinu)
		exp_unpack_ch(UNPACK_CPL, audblk->cplexpstr, audblk->ncplgrps, (uint_16)(audblk->cplabsexp << 1),
				audblk->cplexps, 
				&audblk->cpl_exp[audblk->cplstrtmant],error_flag,state);

	if(bsi->lfeon)
		exp_unpack_ch(UNPACK_LFE, audblk->lfeexpstr, 2, audblk->lfeexps[0], 
				&audblk->lfeexps[1], audblk->lfe_exp,error_flag,state);
}


static void exp_unpack_ch(uint_16 type,uint_16 expstr,uint_16 ngrps,uint_16 initial_exp, 
		uint_16 exps[], uint_16 *dest,int *error_flag,ac3_state *state)
{
	uint_16 i,j;
	sint_16 exp_acc;
	sint_16 exp_1,exp_2,exp_3;

	if(expstr == EXP_REUSE)
		return;

	/* Handle the initial absolute exponent */
	exp_acc = initial_exp;
	j = 0;

	/* In the case of a fbw channel then the initial absolute values is 
	 * also an exponent */
	if(type != UNPACK_CPL)
		dest[j++] = exp_acc;

	/* Loop through the groups and fill the dest array appropriately */
	for(i=0; i< ngrps; i++)
	{
		if(exps[i] > 124)
			goto error;

		exp_1 = exps[i] / 25;
		exp_2 = (exps[i] - (exp_1 * 25)) / 5;
		exp_3 = exps[i] - (exp_1 * 25) - (exp_2 * 5) ;

		exp_acc += (exp_1 - 2);

		switch(expstr)
		{
			case EXP_D45:
				dest[j++] = exp_acc;
				dest[j++] = exp_acc;
			case EXP_D25:
				dest[j++] = exp_acc;
			case EXP_D15:
				dest[j++] = exp_acc;
		}

		exp_acc += (exp_2 - 2);

		switch(expstr)
		{
			case EXP_D45:
				dest[j++] = exp_acc;
				dest[j++] = exp_acc;
			case EXP_D25:
				dest[j++] = exp_acc;
			case EXP_D15:
				dest[j++] = exp_acc;
		}

		exp_acc += (exp_3 - 2);

		switch(expstr)
		{
			case EXP_D45:
				dest[j++] = exp_acc;
				dest[j++] = exp_acc;
			case EXP_D25:
				dest[j++] = exp_acc;
			case EXP_D15:
				dest[j++] = exp_acc;
		}
	}	

	return;

			goto error;
error:
	if(!(*error_flag))
	{
		// debug(DAC3,"** Invalid exponent - skipping frame **\n");
	}
	*error_flag = 1;
}

