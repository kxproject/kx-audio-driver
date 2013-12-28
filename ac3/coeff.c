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

//
//Lookup tables of 0.15 two's complement quantization values
//
static const uint_16 q_1[3] = 
{
	( -2 << 15)/3, 0,(  2 << 15)/3 
};

static const uint_16 q_2[5] = 
{
	( -4 << 15)/5,( -2 << 15)/5, 0,
	(  2 << 15)/5,(  4 << 15)/5
};

static const uint_16 q_3[7] = 
{
	( -6 << 15)/7,( -4 << 15)/7,( -2 << 15)/7, 0,
	(  2 << 15)/7,(  4 << 15)/7,(  6 << 15)/7
};

static const uint_16 q_4[11] = 
{
	(-10 << 15)/11,(-8 << 15)/11,(-6 << 15)/11, ( -4 << 15)/11,(-2 << 15)/11,  0,
	(  2 << 15)/11,( 4 << 15)/11,( 6 << 15)/11, (  8 << 15)/11,(10 << 15)/11
};

static const uint_16 q_5[15] = 
{
	(-14 << 15)/15,(-12 << 15)/15,(-10 << 15)/15,
	( -8 << 15)/15,( -6 << 15)/15,( -4 << 15)/15,
	( -2 << 15)/15,   0          ,(  2 << 15)/15,
	(  4 << 15)/15,(  6 << 15)/15,(  8 << 15)/15,
	( 10 << 15)/15,( 12 << 15)/15,( 14 << 15)/15
};

//
// Scale factors for convert_to_float
//

static const uint_32 u32_scale_factors[25] = 
{
	0x38000000, //2 ^ -(0 + 15)
	0x37800000, //2 ^ -(1 + 15)
	0x37000000, //2 ^ -(2 + 15)
	0x36800000, //2 ^ -(3 + 15)
	0x36000000, //2 ^ -(4 + 15)
	0x35800000, //2 ^ -(5 + 15)
	0x35000000, //2 ^ -(6 + 15)
	0x34800000, //2 ^ -(7 + 15)
	0x34000000, //2 ^ -(8 + 15)
	0x33800000, //2 ^ -(9 + 15)
	0x33000000, //2 ^ -(10 + 15)
	0x32800000, //2 ^ -(11 + 15)
	0x32000000, //2 ^ -(12 + 15)
	0x31800000, //2 ^ -(13 + 15)
	0x31000000, //2 ^ -(14 + 15)
	0x30800000, //2 ^ -(15 + 15)
	0x30000000, //2 ^ -(16 + 15)
	0x2f800000, //2 ^ -(17 + 15)
	0x2f000000, //2 ^ -(18 + 15)
	0x2e800000, //2 ^ -(19 + 15)
	0x2e000000, //2 ^ -(20 + 15)
	0x2d800000, //2 ^ -(21 + 15)
	0x2d000000, //2 ^ -(22 + 15)
	0x2c800000, //2 ^ -(23 + 15)
	0x2c000000  //2 ^ -(24 + 15)
};

static const float *scale_factor = (const float*)u32_scale_factors;

//Conversion from bap to number of bits in the mantissas
//zeros account for cases 0,1,2,4 which are special cased
static const uint_16 qnttztab[16] = { 0, 0, 0, 3, 0 , 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 16};

static void    coeff_reset(ac3_state *state);
static sint_16 coeff_get_mantissa(uint_16 bap, uint_16 dithflag,int *error_flag,ac3_state *state);
static void    coeff_uncouple_ch(float samples[],bsi_t *bsi,audblk_t *audblk,uint_32 ch,ac3_state *state);

//
// Convert a 0.15 fixed point number into IEEE single
// precision floating point and scale by 2^-exp
//
static inline float
convert_to_float(uint_32 exp, sint_32 mantissa) 
{
	float x;

	//the scale by 2^-15 is built into the scale factor table
        if(exp>25)
         exp=25;
	x = mantissa * scale_factor[exp];

	return x;
}

void coeff_unpack(bsi_t *bsi, audblk_t *audblk, stream_samples_t samples,int *error_flag,ac3_state *state)
{
	uint_16 i,j;
	uint_32 done_cpl = 0;
	sint_16 mantissa;

	coeff_reset(state);

	for(i=0; i< bsi->nfchans; i++)
	{
		for(j=0; j < audblk->endmant[i]; j++)
		{
			mantissa = coeff_get_mantissa(audblk->fbw_bap[i][j],audblk->dithflag[i],error_flag,state);
			samples[i][j] = convert_to_float(audblk->fbw_exp[i][j],mantissa);
		}

		if(audblk->cplinu && audblk->chincpl[i] && !(done_cpl))
		{
			// ncplmant is equal to 12 * ncplsubnd
			// Don't dither coupling channel until channel separation so that
			// interchannel noise is uncorrelated 
			for(j=audblk->cplstrtmant; j < audblk->cplendmant; j++)
				audblk->cplmant[j] = coeff_get_mantissa(audblk->cpl_bap[j],0,error_flag,state);
			done_cpl = 1;
		}
	}

	//uncouple the channel if necessary
	if(audblk->cplinu)
	{
		for(i=0; i< bsi->nfchans; i++)
		{
			if(audblk->chincpl[i])
				coeff_uncouple_ch(samples[i],bsi,audblk,i,state);
		}

	}

	if(bsi->lfeon)
	{
		// There are always 7 mantissas for lfe, no dither for lfe 
		for(j=0; j < 7 ; j++)
		{
			mantissa = coeff_get_mantissa(audblk->lfe_bap[j],0,error_flag,state);
			samples[5][j] = convert_to_float(audblk->lfe_exp[j],mantissa);
		}
	}
}

//
//Fetch a mantissa from the bitstream
//
//The mantissa returned is a signed 0.15 fixed point number
//
static sint_16 coeff_get_mantissa(uint_16 bap, uint_16 dithflag,int *error_flag,ac3_state *state)
{
	uint_16 mantissa;
	uint_16 group_code;

	//If the bap is 0-5 then we have special cases to take care of
	switch(bap)
	{
		case 0:
			if(dithflag)
				mantissa = dither_gen(state);
			else
				mantissa = 0;
			break;

		case 1:
			if(state->m_1_pointer > 2)
			{
				group_code = (uint_16)bitstream_get(5,state);

				if(group_code > 26)
					goto error;

				state->m_1[0] = group_code / 9; 
				state->m_1[1] = (group_code % 9) / 3; 
				state->m_1[2] = (group_code % 9) % 3; 
				state->m_1_pointer = 0;
			}
			mantissa = state->m_1[state->m_1_pointer++];
			mantissa = q_1[mantissa];
			break;
		case 2:

			if(state->m_2_pointer > 2)
			{
				group_code = (uint_16)bitstream_get(7,state);

				if(group_code > 124)
					goto error;

				state->m_2[0] = group_code / 25;
				state->m_2[1] = (group_code % 25) / 5 ;
				state->m_2[2] = (group_code % 25) % 5 ; 
				state->m_2_pointer = 0;
			}
			mantissa = state->m_2[state->m_2_pointer++];
			mantissa = q_2[mantissa];
			break;

		case 3:
			mantissa = (uint_16)bitstream_get(3,state);

			if(mantissa > 6)
				goto error;

			mantissa = q_3[mantissa];
			break;

		case 4:
			if(state->m_4_pointer > 1)
			{
				group_code = (uint_16)bitstream_get(7,state);

				if(group_code > 120)
					goto error;

				state->m_4[0] = group_code / 11;
				state->m_4[1] = group_code % 11;
				state->m_4_pointer = 0;
			}
			mantissa = state->m_4[state->m_4_pointer++];
			mantissa = q_4[mantissa];
			break;

		case 5:
			mantissa = (uint_16)bitstream_get(4,state);

			if(mantissa > 14)
				goto error;

			mantissa = q_5[mantissa];
			break;

		default:
			mantissa = (uint_16)bitstream_get(qnttztab[bap],state);
			mantissa <<= 16 - qnttztab[bap];
	}

	return mantissa;



error:
	if(!(*error_flag))
	{
		// debug(DAC3,"** Invalid mantissa - skipping frame **\n");
	}
	*error_flag = 1;

	return 0;
}

//
// Reset the mantissa state
//
static void 
coeff_reset(ac3_state *state)
{
	state->m_1[2] = state->m_1[1] = state->m_1[0] = 0;
	state->m_2[2] = state->m_2[1] = state->m_2[0] = 0;
	state->m_4[1] = state->m_4[0] = 0;
	state->m_1_pointer = state->m_2_pointer = state->m_4_pointer = 3;
}

//
// Uncouple the coupling channel into a fbw channel
//
static void coeff_uncouple_ch(float samples[],bsi_t *bsi,audblk_t *audblk,uint_32 ch,ac3_state *state)
{
	uint_32 bnd = 0;
	uint_32 sub_bnd = 0;
	uint_32 i,j;
	float cpl_coord = 1.0;
	uint_32 cpl_exp_tmp;
	uint_32 cpl_mant_tmp;
	sint_32 mantissa; 

	for(i=audblk->cplstrtmant;i<audblk->cplendmant;)
	{
		if(!audblk->cplbndstrc[sub_bnd++])
		{
			cpl_exp_tmp = audblk->cplcoexp[ch][bnd] + 3 * audblk->mstrcplco[ch];
			if(audblk->cplcoexp[ch][bnd] == 15)
				cpl_mant_tmp = (audblk->cplcomant[ch][bnd]) << 11;
			else
				cpl_mant_tmp = ((0x10) | audblk->cplcomant[ch][bnd]) << 10;
			
			cpl_coord = convert_to_float(cpl_exp_tmp,cpl_mant_tmp) * 8.0f;

			//Invert the phase for the right channel if necessary
			if(bsi->acmod == 0x2 && audblk->phsflginu && ch == 1 && audblk->phsflg[bnd])
				cpl_coord *= -1;

			bnd++;
		}

		for(j=0;j < 12; j++)
		{
			//Get new dither values for each channel if necessary, so
			//the channels are uncorrelated
			if(audblk->dithflag[ch] && audblk->cpl_bap[i] == 0)
				mantissa = dither_gen(state);
			else
				mantissa = audblk->cplmant[i];

			samples[i]  = cpl_coord * convert_to_float(audblk->cpl_exp[i],mantissa);

			i++;
		}
	}
}
