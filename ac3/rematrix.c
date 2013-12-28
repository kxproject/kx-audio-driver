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

struct rematrix_band_s
{
	uint_32 start;
	uint_32 end;
};

struct rematrix_band_s rematrix_band[] = { {13,24}, {25,36}, {37 ,60}, {61,252}};

static inline uint_32 min_(uint_32 a,uint_32 b);

static inline uint_32
min_(uint_32 a,uint_32 b)
{
	return (a < b ? a : b);
}

/* This routine simply does stereo rematixing for the 2 channel 
 * stereo mode */
void rematrix(audblk_t *audblk, stream_samples_t samples)
{
	uint_32 num_bands;
	uint_32 start;
	uint_32 end;
	uint_32 i,j;
	float left,right;

	if(!audblk->cplinu || audblk->cplbegf > 2)
		num_bands = 4;
	else if (audblk->cplbegf > 0)
		num_bands = 3;
	else
		num_bands = 2;

	for(i=0;i < num_bands; i++)
	{
		if(!audblk->rematflg[i])
			continue;

		start = rematrix_band[i].start;
		end = min_(rematrix_band[i].end ,12 * audblk->cplbegf + 36);
	
		for(j=start;j < end; j++)
		{
			left  = samples[0][j] + samples[1][j];
			right = samples[0][j] - samples[1][j];
			samples[0][j] = left;
			samples[1][j] = right;
		}
	}
}
