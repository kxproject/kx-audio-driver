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

void __stdcall ac3_init(ac3_state *state)
{
	state->frame_count=0;
	bitstream_init(state);
	imdct_init();
	sanity_check_init(&state->syncinfo,&state->bsi,&state->audblk);
}

int __stdcall ac3_decode_frame(ac3_state *state)
{
	uint_32 i;
	int j;
	int error_flag=0;

	// debug(DAC3,"(decode_frame) begin frame %d\n",state->frame_count);

	//find a syncframe and parse
	parse_syncinfo(&state->syncinfo,&error_flag,state);
	if(error_flag)
	{
		// debug(DAC3," -- fail after parse_syncinfo\n");
		goto error;
	}

        state->frame_count++;

	state->sampling_rate = state->syncinfo.sampling_rate;

	parse_bsi(&state->bsi,state);

	stats_print_banner(&state->syncinfo,&state->bsi,state);

	for(i=0; i < 6; i++)
	{
		//Initialize freq/time sample storage
		memset(state->samples,0,sizeof(float) * 256 * (state->bsi.nfchans + state->bsi.lfeon));

		// Extract most of the audblk info from the bitstream
		// (minus the mantissas 
		parse_audblk(&state->bsi,&state->audblk,state);

		// Take the differential exponent data and turn it into
		// absolute exponents 
		exponent_unpack(&state->bsi,&state->audblk,&error_flag,state); 
		if(error_flag)
		{
			// debug(DAC3," -- fail after exponent_unpack\n");
			goto error;
		}

		// Figure out how many bits per mantissa 
		bit_allocate(state->syncinfo.fscod,&state->bsi,&state->audblk,state);

		// Extract the mantissas from the stream and
		// generate floating point frequency coefficients
		coeff_unpack(&state->bsi,&state->audblk,state->samples,&error_flag,state);
		if(error_flag)
		{
			// debug(DAC3," -- fail after coeff_unpack\n");
			goto error;
		}

		if(state->bsi.acmod == 0x2)
		{
			rematrix(&state->audblk,state->samples);
		}	

		// Convert the frequency samples into time samples
		imdct(&state->bsi,&state->audblk,state->samples);

		// Downmix into the requested number of channels
		// and convert floating point to sint_16

		for(j=0;j<256;j++)
		{
			state->left[i*256+j]=(sint_16)(state->samples[0][j]*32767.0f);
                        state->center[i*256+j]=(sint_16)(state->samples[1][j]*32767.0f);
                	state->right[i*256+j]=(sint_16)(state->samples[2][j]*32767.0f);
                        state->sleft[i*256+j]=(sint_16)(state->samples[3][j]*32767.0f);
                        state->sright[i*256+j]=(sint_16)(state->samples[4][j]*32767.0f);
                        state->subwoofer[i*256+j]=(sint_16)(state->samples[5][j]*32767.0f);
                }

		sanity_check(&state->syncinfo,&state->bsi,&state->audblk,&error_flag,state);
		if(error_flag)
		{
			// debug(DAC3," -- sanity check fail\n");
			goto error;
		}
	}
	parse_auxdata(&state->syncinfo,state);

	return 0;	

error:
	// debug(DAC3," -- FRAME ZEROED\n");

	return -1;
}
