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

static const char *service_ids[8] = 
{
	"CM","ME","VI","HI",
	 "D", "C","E", "VO"
};

struct mixlev_s
{
	float clev;
	char *desc;
};

static const struct mixlev_s cmixlev_tbl[4] =  
{
	{0.707f, "(-3.0 dB)"}, {0.595f, "(-4.5 dB)"},
	{0.500f, "(-6.0 dB)"}, {1.0f,  "Invalid"}
};

static const struct mixlev_s smixlev_tbl[4] =  
{
	{0.707f, "(-3.0 dB)"}, {0.500f, "(-6.0 dB)"},
	{  0.0f,   "off    "}, {  1.0f, "Invalid"}
};

static const char *language[128] = 
{
	"unknown", "Albanian", "Breton", "Catalan", "Croatian", "Welsh", "Czech", "Danish", 
	"German", "English", "Spanish", "Esperanto", "Estonian", "Basque", "Faroese", "French", 
	"Frisian", "Irish", "Gaelic", "Galician", "Icelandic", "Italian", "Lappish", "Latin", 
	"Latvian", "Luxembourgian", "Lithuanian", "Hungarian", "Maltese", "Dutch", "Norwegian", "Occitan", 
	"Polish", "Portugese", "Romanian", "Romansh", "Serbian", "Slovak", "Slovene", "Finnish", 
	"Swedish", "Turkish", "Flemish", "Walloon", "0x2c", "0x2d", "0x2e", "0x2f", 
	"0x30", "0x31", "0x32", "0x33", "0x34", "0x35", "0x36", "0x37", 
	"0x38", "0x39", "0x3a", "0x3b", "0x3c", "0x3d", "0x3e", "0x3f", 
	"background", "0x41", "0x42", "0x43", "0x44", "Zulu", "Vietnamese", "Uzbek", 
	"Urdu", "Ukrainian", "Thai", "Telugu", "Tatar", "Tamil", "Tadzhik", "Swahili", 
	"Sranan Tongo", "Somali", "Sinhalese", "Shona", "Serbo-Croat", "Ruthenian", "Russian", "Quechua", 
	"Pustu", "Punjabi", "Persian", "Papamiento", "Oriya", "Nepali", "Ndebele", "Marathi", 
	"Moldavian", "Malaysian", "Malagasay", "Macedonian", "Laotian", "Korean", "Khmer", "Kazakh",
	"Kannada", "Japanese", "Indonesian", "Hindi", "Hebrew", "Hausa", "Gurani", "Gujurati", 
	"Greek", "Georgian", "Fulani", "Dari", "Churash", "Chinese", "Burmese", "Bulgarian", 
	"Bengali", "Belorussian", "Bambora", "Azerbijani", "Assamese", "Armenian", "Arabic", "Amharic"
};

void stats_print_banner(syncinfo_t *syncinfo,bsi_t *bsi,ac3_state *state)
{
	debug(DAC3,"%d.%d Mode ",bsi->nfchans,bsi->lfeon);
	debug(DAC3,"%d KHz",syncinfo->sampling_rate);
	debug(DAC3,"%4d kbps ",syncinfo->bit_rate);
	if (bsi->langcode && (bsi->langcod < 128))
	{
		if(bsi->langcod<128 && language[bsi->langcod])
		  debug(DAC3,"%s ", language[bsi->langcod]);
	}

	switch(bsi->bsmod)
	{
		case 0:
			debug(DAC3,"Complete Main Audio Service");
			break;
		case 1:
			debug(DAC3,"Music and Effects Audio Service");
		case 2:
			debug(DAC3,"Visually Impaired Audio Service");
			break;
		case 3:
			debug(DAC3,"Hearing Impaired Audio Service");
			break;
		case 4:
			debug(DAC3,"Dialogue Audio Service");
			break;
		case 5:
			debug(DAC3,"Commentary Audio Service");
			break;
		case 6:
			debug(DAC3,"Emergency Audio Service");
			break;
		case 7:
			debug(DAC3,"Voice Over Audio Service");
			break;
	}
	debug(DAC3,"\n");
}

void stats_print_syncinfo(syncinfo_t *syncinfo,ac3_state *state)
{
	debug(DAC3,"(syncinfo) ");
	
	switch (syncinfo->fscod)
	{
		case 2:
			debug(DAC3,"32 KHz   ");
			break;
		case 1:
			debug(DAC3,"44.1 KHz ");
			break;
		case 0:
			debug(DAC3,"48 KHz   ");
			break;
		default:
			debug(DAC3,"Invalid sampling rate ");
			break;
	}

	debug(DAC3,"%4d kbps %4d words per frame\n",syncinfo->bit_rate,syncinfo->frame_size);
}
	
void stats_print_bsi(bsi_t *bsi,ac3_state *state)
{
	debug(DAC3,"(bsi) ");
	if(bsi->bsmod<=8 && service_ids[bsi->bsmod])
	 debug(DAC3,"%s",service_ids[bsi->bsmod]);
	debug(DAC3," %d.%d Mode ",bsi->nfchans,bsi->lfeon);
	if ((bsi->acmod & 0x1) && (bsi->acmod != 0x1))
	{
		debug(DAC3," Centre Mix Level %s ",cmixlev_tbl[bsi->cmixlev].desc);
	}	
	if (bsi->acmod & 0x4)
	{
		debug(DAC3," Sur Mix Level %s ",smixlev_tbl[bsi->cmixlev].desc);
	}
	debug(DAC3,"\n");
}

char *exp_strat_tbl[4] = {"R   ","D15 ","D25 ","D45 "};

void stats_print_audblk(bsi_t *bsi,audblk_t *audblk,ac3_state *state)
{
/*	uint_32 i;

	debug(DAC3,"(audblk) ");
	debug(DAC3,"%s ",audblk->cplinu ? "cpl on " : "cpl off");
	debug(DAC3,"%s ",audblk->baie? "bai " : "    ");
	debug(DAC3,"%s ",audblk->snroffste? "snroffst " : "         ");
	debug(DAC3,"%s ",audblk->deltbaie? "deltba " : "       ");
	debug(DAC3,"%s ",audblk->phsflginu? "phsflg " : "       ");
	debug(DAC3,"(%s %s %s %s %s) ",exp_strat_tbl[audblk->chexpstr[0]],
	exp_strat_tbl[audblk->chexpstr[1]],exp_strat_tbl[audblk->chexpstr[2]],
	exp_strat_tbl[audblk->chexpstr[3]],exp_strat_tbl[audblk->chexpstr[4]]);
	debug(DAC3,"[");
	for(i=0;i<bsi->nfchans;i++)
		debug(DAC3,"%1d",audblk->blksw[i]);
	debug(DAC3,"]");

	debug(DAC3,"\n");
*/
}

