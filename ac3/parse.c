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

// #include <stdio.h>

/* Misc LUT */
static const uint_16 nfchans[8] = {2,1,2,3,3,4,4,5};

struct frmsize_s
{
	uint_16 bit_rate;
	uint_16 frm_size[3];
};

static const struct frmsize_s frmsizecod_tbl[64] = 
{
	{ 32  ,{64   ,69   ,96   } },
	{ 32  ,{64   ,70   ,96   } },
	{ 40  ,{80   ,87   ,120  } },
	{ 40  ,{80   ,88   ,120  } },
	{ 48  ,{96   ,104  ,144  } },
	{ 48  ,{96   ,105  ,144  } },
	{ 56  ,{112  ,121  ,168  } },
	{ 56  ,{112  ,122  ,168  } },
	{ 64  ,{128  ,139  ,192  } },
	{ 64  ,{128  ,140  ,192  } },
	{ 80  ,{160  ,174  ,240  } },
	{ 80  ,{160  ,175  ,240  } },
	{ 96  ,{192  ,208  ,288  } },
	{ 96  ,{192  ,209  ,288  } },
	{ 112 ,{224  ,243  ,336  } },
	{ 112 ,{224  ,244  ,336  } },
	{ 128 ,{256  ,278  ,384  } },
	{ 128 ,{256  ,279  ,384  } },
	{ 160 ,{320  ,348  ,480  } },
	{ 160 ,{320  ,349  ,480  } },
	{ 192 ,{384  ,417  ,576  } },
	{ 192 ,{384  ,418  ,576  } },
	{ 224 ,{448  ,487  ,672  } },
	{ 224 ,{448  ,488  ,672  } },
	{ 256 ,{512  ,557  ,768  } },
	{ 256 ,{512  ,558  ,768  } },
	{ 320 ,{640  ,696  ,960  } },
	{ 320 ,{640  ,697  ,960  } },
	{ 384 ,{768  ,835  ,1152 } },
	{ 384 ,{768  ,836  ,1152 } },
	{ 448 ,{896  ,975  ,1344 } },
	{ 448 ,{896  ,976  ,1344 } },
	{ 512 ,{1024 ,1114 ,1536 } },
	{ 512 ,{1024 ,1115 ,1536 } },
	{ 576 ,{1152 ,1253 ,1728 } },
	{ 576 ,{1152 ,1254 ,1728 } },
	{ 640 ,{1280 ,1393 ,1920 } },
	{ 640 ,{1280 ,1394 ,1920 } }
};

int AC3_CALLTYPE __stdcall get_frame_size(unsigned int tmp,int *frame_size,int *bit_rate,int *sampling_rate)
{
	syncinfo_t syncinfo;

	// Get the sampling rate 
	syncinfo.fscod  = (tmp >> 6) & 0x3;

	if(syncinfo.fscod == 3)
	{
		//invalid sampling rate code
		return -1;
	}
	else if(syncinfo.fscod == 2)
		*sampling_rate = 32000;
	else if(syncinfo.fscod == 1)
		*sampling_rate = 44100;
	else
		*sampling_rate = 48000;

	// Get the frame size code 
	syncinfo.frmsizecod = tmp & 0x3f;

	if(syncinfo.frmsizecod>37)
	{
		return -2;
	}

	// Calculate the frame size and bitrate
	*frame_size = 
		frmsizecod_tbl[syncinfo.frmsizecod].frm_size[syncinfo.fscod];
	*bit_rate = frmsizecod_tbl[syncinfo.frmsizecod].bit_rate;

	return 0;
}

/* Parse a syncinfo structure, minus the sync word */
void parse_syncinfo(syncinfo_t *syncinfo,int *error_flag,ac3_state *state)
{
	uint_32 tmp = 0;
	uint_16 sync_word = 0;
	uint_32 time_out = 32*4;

	// 
	// Find a ac3 sync frame.
	// 
	while(time_out--)
	{
		sync_word = (sync_word << 8) + bitstream_get_byte(state);

		if(sync_word == 0x0b77)
			break;
	}

	if(time_out==0)
	{
	 *error_flag=1;
	 return;
	}

	//
	// We need to read in the entire syncinfo struct (0x0b77 + 24 bits)
	// in order to determine how big the frame is
	//
	tmp = (tmp << 8) + bitstream_get_byte(state);
	tmp = (tmp << 8) + bitstream_get_byte(state);
	tmp = (tmp << 8) + bitstream_get_byte(state);

	// Get the sampling rate 
	syncinfo->fscod  = (tmp >> 6) & 0x3;

	if(syncinfo->fscod == 3)
	{
		//invalid sampling rate code
		*error_flag = 1;	
		return;
	}
	else if(syncinfo->fscod == 2)
		syncinfo->sampling_rate = 32000;
	else if(syncinfo->fscod == 1)
		syncinfo->sampling_rate = 44100;
	else
		syncinfo->sampling_rate = 48000;

	// Get the frame size code 
	syncinfo->frmsizecod = tmp & 0x3f;

	if(syncinfo->frmsizecod>37)
	{
		*error_flag=1;
		return;
	}

	// Calculate the frame size and bitrate
	syncinfo->frame_size = 
		frmsizecod_tbl[syncinfo->frmsizecod].frm_size[syncinfo->fscod];
	syncinfo->bit_rate = frmsizecod_tbl[syncinfo->frmsizecod].bit_rate;

	if(syncinfo->frame_size==0 || syncinfo->frame_size>sizeof(state->buffer))
	{
		*error_flag=1;
		return;
	}

	// Buffer the entire syncframe 
	bitstream_buffer_frame(syncinfo->frame_size * 2 - 5,state);
	// Check the crc over the entire frame 
	crc_init(state);
	crc_process_byte((uint_8) (tmp>>16) ,state);
	crc_process_byte((uint_8) ((tmp>>8) & 0xff) ,state);
	crc_process_byte((uint_8) (tmp & 0xff) ,state);
	crc_process_frame(bitstream_get_buffer_start(state),syncinfo->frame_size * 2 - 5,state);

	if(!crc_validate(state))
	{
		*error_flag = 1;
		// debug(DAC3,"** AC3 CRC failed - skipping frame **\n");
		return;
	}

	// stats_print_syncinfo(syncinfo,state);
}

/*
 * This routine fills a bsi struct from the AC3 stream
 */

void parse_bsi(bsi_t *bsi,ac3_state *state)
{
	int i;

	/* Check the AC-3 version number */
	bsi->bsid = (uint_16)bitstream_get(5,state);

	/* Get the audio service provided by the steram */
	bsi->bsmod = (uint_16)bitstream_get(3,state);

	/* Get the audio coding mode (ie how many channels)*/
	bsi->acmod = (uint_16)bitstream_get(3,state);
	/* Predecode the number of full bandwidth channels as we use this
	 * number a lot */
	bsi->nfchans = nfchans[bsi->acmod];

	/* If it is in use, get the centre channel mix level */
	if ((bsi->acmod & 0x1) && (bsi->acmod != 0x1))
		bsi->cmixlev = (uint_16)bitstream_get(2,state);

	/* If it is in use, get the surround channel mix level */
	if (bsi->acmod & 0x4)
		bsi->surmixlev = (uint_16)bitstream_get(2,state);

	/* Get the dolby surround mode if in 2/0 mode */
	if(bsi->acmod == 0x2)
		bsi->dsurmod= (uint_16)bitstream_get(2,state);

	/* Is the low frequency effects channel on? */
	bsi->lfeon = (uint_16)bitstream_get(1,state);

	/* Get the dialogue normalization level */
	bsi->dialnorm = (uint_16)bitstream_get(5,state);

	/* Does compression gain exist? */
	bsi->compre = (uint_16)bitstream_get(1,state);
	if (bsi->compre)
	{
		/* Get compression gain */
		bsi->compr = (uint_16)bitstream_get(8,state);
	}

	/* Does language code exist? */
	bsi->langcode = (uint_16)bitstream_get(1,state);
	if (bsi->langcode)
	{
		/* Get langauge code */
		bsi->langcod = (uint_16)bitstream_get(8,state);
	}

	/* Does audio production info exist? */
	bsi->audprodie = (uint_16)bitstream_get(1,state);
	if (bsi->audprodie)
	{
		/* Get mix level */
		bsi->mixlevel = (uint_16)bitstream_get(5,state);

		/* Get room type */
		bsi->roomtyp = (uint_16)bitstream_get(2,state);
	}

	/* If we're in dual mono mode then get some extra info */
	if (bsi->acmod ==0)
	{
		/* Get the dialogue normalization level two */
		bsi->dialnorm2 = (uint_16)bitstream_get(5,state);

		/* Does compression gain two exist? */
		bsi->compr2e = (uint_16)bitstream_get(1,state);
		if (bsi->compr2e)
		{
			/* Get compression gain two */
			bsi->compr2 = (uint_16)bitstream_get(8,state);
		}

		/* Does language code two exist? */
		bsi->langcod2e = (uint_16)bitstream_get(1,state);
		if (bsi->langcod2e)
		{
			/* Get langauge code two */
			bsi->langcod2 = (uint_16)bitstream_get(8,state);
		}

		/* Does audio production info two exist? */
		bsi->audprodi2e = (uint_16)bitstream_get(1,state);
		if (bsi->audprodi2e)
		{
			/* Get mix level two */
			bsi->mixlevel2 = (uint_16)bitstream_get(5,state);

			/* Get room type two */
			bsi->roomtyp2 = (uint_16)bitstream_get(2,state);
		}
	}

	/* Get the copyright bit */
	bsi->copyrightb = (uint_16)bitstream_get(1,state);

	/* Get the original bit */
	bsi->origbs = (uint_16)bitstream_get(1,state);
	
	/* Does timecode one exist? */
	bsi->timecod1e = (uint_16)bitstream_get(1,state);

	if(bsi->timecod1e)
		bsi->timecod1 = (uint_16)bitstream_get(14,state);

	/* Does timecode two exist? */
	bsi->timecod2e = (uint_16)bitstream_get(1,state);

	if(bsi->timecod2e)
		bsi->timecod2 = (uint_16)bitstream_get(14,state);

	/* Does addition info exist? */
	bsi->addbsie = (uint_16)bitstream_get(1,state);

	if(bsi->addbsie)
	{
		/* Get how much info is there */
		bsi->addbsil = (uint_16)bitstream_get(6,state);

		/* Get the additional info */
		for(i=0;i<(int)(bsi->addbsil + 1);i++)
			bsi->addbsi[i] = (uint_8)bitstream_get(8,state);
	}

	stats_print_bsi(bsi,state);
}

/* More pain inducing parsing */
void parse_audblk(bsi_t *bsi,audblk_t *audblk,ac3_state *state)
{
	int i,j;

	for (i=0;i < bsi->nfchans; i++)
	{
		/* Is this channel an interleaved 256 + 256 block ? */
		audblk->blksw[i] = (uint_16)bitstream_get(1,state);
	}

	for (i=0;i < bsi->nfchans; i++)
	{
		/* Should we dither this channel? */
		audblk->dithflag[i] = (uint_16)bitstream_get(1,state);
	}

	/* Does dynamic range control exist? */
	audblk->dynrnge = (uint_16)bitstream_get(1,state);
	if (audblk->dynrnge)
	{
		/* Get dynamic range info */
		audblk->dynrng = (uint_16)bitstream_get(8,state);
	}

	/* If we're in dual mono mode then get the second channel DR info */
	if (bsi->acmod == 0)
	{
		/* Does dynamic range control two exist? */
		audblk->dynrng2e = (uint_16)bitstream_get(1,state);
		if (audblk->dynrng2e)
		{
			/* Get dynamic range info */
			audblk->dynrng2 = (uint_16)bitstream_get(8,state);
		}
	}

	/* Does coupling strategy exist? */
	audblk->cplstre = (uint_16)bitstream_get(1,state);
	if (audblk->cplstre)
	{
		/* Is coupling turned on? */
		audblk->cplinu = (uint_16)bitstream_get(1,state);
		if(audblk->cplinu)
		{
			for(i=0;i < bsi->nfchans; i++)
				audblk->chincpl[i] = (uint_16)bitstream_get(1,state);
			if(bsi->acmod == 0x2)
				audblk->phsflginu = (uint_16)bitstream_get(1,state);
			audblk->cplbegf = (uint_16)bitstream_get(4,state);
			audblk->cplendf = (uint_16)bitstream_get(4,state);
			audblk->ncplsubnd = (audblk->cplendf + 2) - audblk->cplbegf + 1;

			/* Calculate the start and end bins of the coupling channel */
			audblk->cplstrtmant = (audblk->cplbegf * 12) + 37 ; 
			audblk->cplendmant =  ((audblk->cplendf + 3) * 12) + 37;

			/* The number of combined subbands is ncplsubnd minus each combined
			 * band */
			audblk->ncplbnd = audblk->ncplsubnd; 

			for(i=1; i< audblk->ncplsubnd; i++)
			{
				audblk->cplbndstrc[i] = (uint_16)bitstream_get(1,state);
				audblk->ncplbnd -= audblk->cplbndstrc[i];
			}
		}
	}

	if(audblk->cplinu)
	{
		/* Loop through all the channels and get their coupling co-ords */	
		for(i=0;i < bsi->nfchans;i++)
		{
			if(!audblk->chincpl[i])
				continue;

			/* Is there new coupling co-ordinate info? */
			audblk->cplcoe[i] = (uint_16)bitstream_get(1,state);

			if(audblk->cplcoe[i])
			{
				audblk->mstrcplco[i] = (uint_16)bitstream_get(2,state); 
				for(j=0;j < audblk->ncplbnd; j++)
				{
					audblk->cplcoexp[i][j] = (uint_16)bitstream_get(4,state); 
					audblk->cplcomant[i][j] = (uint_16)bitstream_get(4,state); 
				}
			}
		}

		/* If we're in dual mono mode, there's going to be some phase info */
		if( (bsi->acmod == 0x2) && audblk->phsflginu && 
				(audblk->cplcoe[0] || audblk->cplcoe[1]))
		{
			for(j=0;j < audblk->ncplbnd; j++)
				audblk->phsflg[j] = (uint_16)bitstream_get(1,state); 

		}
	}

	/* If we're in dual mono mode, there may be a rematrix strategy */
	if(bsi->acmod == 0x2)
	{
		audblk->rematstr = (uint_16)bitstream_get(1,state);
		if(audblk->rematstr)
		{
			if (audblk->cplinu == 0) 
			{ 
				for(i = 0; i < 4; i++) 
					audblk->rematflg[i] = (uint_16)bitstream_get(1,state);
			}
			if((audblk->cplbegf > 2) && audblk->cplinu) 
			{
				for(i = 0; i < 4; i++) 
					audblk->rematflg[i] = (uint_16)bitstream_get(1,state);
			}
			if((audblk->cplbegf <= 2) && audblk->cplinu) 
			{ 
				for(i = 0; i < 3; i++) 
					audblk->rematflg[i] = (uint_16)bitstream_get(1,state);
			} 
			if((audblk->cplbegf == 0) && audblk->cplinu) 
				for(i = 0; i < 2; i++) 
					audblk->rematflg[i] = (uint_16)bitstream_get(1,state);

		}
	}

	if (audblk->cplinu)
	{
		/* Get the coupling channel exponent strategy */
		audblk->cplexpstr = (uint_16)bitstream_get(2,state);
		audblk->ncplgrps = (audblk->cplendmant - audblk->cplstrtmant) / 
				(3 << (audblk->cplexpstr-1));
	}

	for(i = 0; i < bsi->nfchans; i++)
		audblk->chexpstr[i] = (uint_16)bitstream_get(2,state);

	/* Get the exponent strategy for lfe channel */
	if(bsi->lfeon) 
		audblk->lfeexpstr = (uint_16)bitstream_get(1,state);

	/* Determine the bandwidths of all the fbw channels */
	for(i = 0; i < bsi->nfchans; i++) 
	{ 
		uint_16 grp_size;

		if(audblk->chexpstr[i] != EXP_REUSE) 
		{ 
			if (audblk->cplinu && audblk->chincpl[i]) 
			{
				audblk->endmant[i] = audblk->cplstrtmant;
			}
			else
			{
				audblk->chbwcod[i] = (uint_16)bitstream_get(6,state); 
				audblk->endmant[i] = ((audblk->chbwcod[i] + 12) * 3) + 37;
			}

			/* Calculate the number of exponent groups to fetch */
			grp_size =  3 * (1 << (audblk->chexpstr[i] - 1));
			audblk->nchgrps[i] = (audblk->endmant[i] - 1 + (grp_size - 3)) / grp_size;
		}
	}

	/* Get the coupling exponents if they exist */
	if(audblk->cplinu && (audblk->cplexpstr != EXP_REUSE))
	{
		audblk->cplabsexp = (uint_16)bitstream_get(4,state);
		for(i=0;i< audblk->ncplgrps;i++)
			audblk->cplexps[i] = (uint_16)bitstream_get(7,state);
	}

	/* Get the fwb channel exponents */
	for(i=0;i < bsi->nfchans; i++)
	{
		if(audblk->chexpstr[i] != EXP_REUSE)
		{
			audblk->exps[i][0] = (uint_16)bitstream_get(4,state);			
			for(j=1;j<=audblk->nchgrps[i];j++)
				audblk->exps[i][j] = (uint_16)bitstream_get(7,state);
			audblk->gainrng[i] = (uint_16)bitstream_get(2,state);
		}
	}

	/* Get the lfe channel exponents */
	if(bsi->lfeon && (audblk->lfeexpstr != EXP_REUSE))
	{
		audblk->lfeexps[0] = (uint_16)bitstream_get(4,state);
		audblk->lfeexps[1] = (uint_16)bitstream_get(7,state);
		audblk->lfeexps[2] = (uint_16)bitstream_get(7,state);
	}

	/* Get the parametric bit allocation parameters */
	audblk->baie = (uint_16)bitstream_get(1,state);

	if(audblk->baie)
	{
		audblk->sdcycod = (uint_16)bitstream_get(2,state);
		audblk->fdcycod = (uint_16)bitstream_get(2,state);
		audblk->sgaincod = (uint_16)bitstream_get(2,state);
		audblk->dbpbcod = (uint_16)bitstream_get(2,state);
		audblk->floorcod = (uint_16)bitstream_get(3,state);
	}

	/* Get the SNR off set info if it exists */
	audblk->snroffste = (uint_16)bitstream_get(1,state);

	if(audblk->snroffste)
	{
		audblk->csnroffst = (uint_16)bitstream_get(6,state);

		if(audblk->cplinu)
		{
			audblk->cplfsnroffst = (uint_16)bitstream_get(4,state);
			audblk->cplfgaincod = (uint_16)bitstream_get(3,state);
		}

		for(i = 0;i < bsi->nfchans; i++)
		{
			audblk->fsnroffst[i] = (uint_16)bitstream_get(4,state);
			audblk->fgaincod[i] = (uint_16)bitstream_get(3,state);
		}
		if(bsi->lfeon)
		{

			audblk->lfefsnroffst = (uint_16)bitstream_get(4,state);
			audblk->lfefgaincod = (uint_16)bitstream_get(3,state);
		}
	}

	/* Get coupling leakage info if it exists */
	if(audblk->cplinu)
	{
		audblk->cplleake = (uint_16)bitstream_get(1,state);	
		
		if(audblk->cplleake)
		{
			audblk->cplfleak = (uint_16)bitstream_get(3,state);
			audblk->cplsleak = (uint_16)bitstream_get(3,state);
		}
	}
	
	/* Get the delta bit alloaction info */
	audblk->deltbaie = (uint_16)bitstream_get(1,state);	
	
	if(audblk->deltbaie)
	{
		if(audblk->cplinu)
			audblk->cpldeltbae = (uint_16)bitstream_get(2,state);

		for(i = 0;i < bsi->nfchans; i++)
			audblk->deltbae[i] = (uint_16)bitstream_get(2,state);

		if (audblk->cplinu && (audblk->cpldeltbae == DELTA_BIT_NEW))
		{
			audblk->cpldeltnseg = (uint_16)bitstream_get(3,state);
			for(i = 0;i < audblk->cpldeltnseg + 1; i++)
			{
				audblk->cpldeltoffst[i] = (uint_16)bitstream_get(5,state);
				audblk->cpldeltlen[i] = (uint_16)bitstream_get(4,state);
				audblk->cpldeltba[i] = (uint_16)bitstream_get(3,state);
			}
		}

		for(i = 0;i < bsi->nfchans; i++)
		{
			if (audblk->deltbae[i] == DELTA_BIT_NEW)
			{
				audblk->deltnseg[i] = (uint_16)bitstream_get(3,state);
				for(j = 0; j < audblk->deltnseg[i] + 1; j++)
				{
					audblk->deltoffst[i][j] = (uint_16)bitstream_get(5,state);
					audblk->deltlen[i][j] = (uint_16)bitstream_get(4,state);
					audblk->deltba[i][j] = (uint_16)bitstream_get(3,state);
				}
			}
		}
	}

	/* Check to see if there's any dummy info to get */
	if((audblk->skiple =  (uint_16)bitstream_get(1,state)))
	{
		uint_16 skip_data;

		audblk->skipl = (uint_16)bitstream_get(9,state);

		for(i = 0; i < audblk->skipl ; i++)
		{
			skip_data = (uint_16)bitstream_get(8,state);
		}
	}

	stats_print_audblk(bsi,audblk,state);
}

void parse_auxdata(syncinfo_t *syncinfo,ac3_state *state)
{
/*
	FIXME

	int i;
	int skip_length;
	uint_16 crc;
	uint_16 auxdatae;

	skip_length = (syncinfo->frame_size * 16)  - bitstream_get_total_bits() - 17 - 1;

	
	for(i=0; i <  skip_length; i++)
		//printf("Skipped bit %i\n",(uint_16)bitstream_get(1));
		bitstream_get(1);

	//get the auxdata exists bit
	auxdatae = bitstream_get(1);	


	//Skip the CRC reserved bit
	bitstream_get(1);

	//Get the crc
	crc = bitstream_get(16);
*/
}


