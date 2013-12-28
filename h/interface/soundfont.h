// kX SDK:
// kX Audio Driver / kX Audio Driver API
// Copyright (c) Eugene Gavrilov, 2001-2005.
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

// soundfont.h
// -----------
// user and kernel-level structures for SoundFont subsystem
// -----------
// do not include this file directly, use 'kxapi.h' instead


#ifndef _SOUNDFONT_H_
#define _SOUNDFONT_H_

#pragma once

#pragma pack(1)
typedef union
{
	struct range_s
	{
		byte lo;
		byte high;
	}range;
	short amount;
	word amount_w;
}gen_amount_t;

typedef enum
{
monoSample = 1,
rightSample = 2,
leftSample = 4,
linkedSample = 8,
RomMonoSample = 0x8001,
RomRightSample = 0x8002,
RomLeftSample = 0x8004,
RomLinkedSample = 0x8008
}SFSampleLink;

typedef word SFGenerator;
typedef word SFModulator;
typedef word SFTransform;

typedef struct
{
 	char name[20];	// 'EOP' for last preset
 	word preset;
 	word bank;	// 128 for percussion
 	word preset_bag_ndx; // ndx in preset_bag
 	dword library;
 	dword genre;
 	dword morphology;
}sfPresetHeader;

typedef struct
{
	SFModulator mod_src_oper;
	SFGenerator mod_dest_oper;
	short amount;
	SFModulator mod_amt_src_oper;
	SFTransform mod_trans_oper;
}sfModList;

typedef struct
{
	SFGenerator gen_oper;
	gen_amount_t gen_amount;
}sfGenList;

typedef struct
{
	char name[20]; // 'EOI' in the last one
	word inst_bag_ndx;
}sfInst;

typedef struct
{
	word gen_ndx;
	word mod_ndx;	
}sfModGenBag;

typedef struct
{
	char name[20];
	dword start,end,start_loop,end_loop,sample_rate;
        // end: end (+46 zero bytes)
	byte original_key; // 60 = Middle C; 255-unpitched
	signed char correction; // in cents (?)
	word sample_link; // points to correspoding left-right pair
	word sample_type; 
}sfSample;

typedef struct
{
	word major,minor;
}sfVersionTag;

typedef struct
{
 sfVersionTag ver;
 sfVersionTag rom_ver;	// for enum_soundfonts() -> .minor contains soundfont id
 char engine[32];
 char name[128];
 char rom_name[32];
 char card[32];
 char engineer[64];
 char product[64];
 char copyright[256];
 char comments[256];
 char creator[64];

 int samples;
 int presets;
 int preset_bags;
 int pmodlists;
 int pgenlists;
 int insts;
 int inst_bags;
 int imodlists;
 int igenlists;
 int sample_len; // pre-MacOSX - was size_t

 // SoundFont management parameters
 dword sfman_id;
 char sfman_file_name[256];
 int subsynth; // 0-both; 1-Synth1, 2-Synth2
}sfHeader;
#pragma pack()

#pragma pack(1)
typedef struct
{
 struct list list;				// for internal use only
 int id;					// on input: unused; on output-> id;
 dword size; // size of stru+all data
 sfHeader header;

 // sequence:
 // presets, preset_bags, pmodlists, pgenlists, insts, inst_bags, imodlists, igenlists, sample_info
 // samples;
 // -- here are OFFSETS [i.e. presets is '0']
 sfPresetHeader *presets;
 sfModGenBag *preset_bags;
 sfModList *pmodlists;
 sfGenList *pgenlists;
 sfInst *insts;
 sfModGenBag *inst_bags;
 sfModList *imodlists;
 sfGenList *igenlists;

 // these are in separate calls
 sfSample *samples;
 short *sample_data;	// note: for kernel-level it is a lmem structure...

 union
 {
  byte data;
 };

 #define KX_SOUNDFONT_MAGIC	0x92873645

}kx_sound_font;
#pragma pack()

typedef struct	// upload sample data page by page
{
 int id;
 int size;
 int pos;
 char data[4096-12];
}sf_load_sample_property;

#define VIENNA_DYNAMIC 256

#endif
