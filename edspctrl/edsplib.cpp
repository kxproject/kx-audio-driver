// kX E-DSP Control utility
// Copyright (c) Eugene Gavrilov, 2008-2014.
// www.kxproject.com
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

#include "stdafx.h"
#include "interface/kxapi.h"
#include "vers.h"
#include "emu.h"

#ifdef _MSC_VER
#pragma warning(disable:4100)
#endif

#include "hana_netlist.h"               // this is v1 card - hana_netlist
#include "emu1010b_netlist.h"           // this is v2 card - emu1010b_netlist
#include "emu0404_netlist.h"            // this is v1 0404 - emu0404_netlist
#include "emu1010_notebook_netlist.h"   // this is v2 cardbus - emu1010_notebook_netlist

#include "audio_dock_netlist.h" // this is original card-powered dock - audio_dock_netlist
#include "micro_dock_netlist.h" // this is v2 self-powered microdock  - micro_dock_netlist

// fpga functions:
int is_fpga_programmed(iKX *ikx);
int is_dock_connected(iKX *ikx);
int is_dock_online(iKX *ikx);
int upload_card_firmware(iKX *ikx);
int upload_dock_firmware(iKX *ikx);
void load_defaults(iKX *ikx);
void print_status(iKX *ikx);

int is_fpga_programmed(iKX *ikx)
{
	byte reg=0;
	ikx->fpga_read(EMU_HANA_ID, &reg);
	return ((reg & 0x3f) == 0x15);
}

int is_dock_connected(iKX *ikx)
{
	byte reg=0;
	ikx->fpga_read(EMU_HANA_OPTION_CARDS, &reg ); // OPTIONS: Which cards are attached to the EMU
	return ((reg & EMU_HANA_OPTION_DOCK_OFFLINE) || (reg & EMU_HANA_OPTION_DOCK_ONLINE));
}

int is_dock_online(iKX *ikx)
{
	byte reg=0;
	ikx->fpga_read(EMU_HANA_OPTION_CARDS, &reg ); // OPTIONS: Which cards are attached to the EMU
	return (reg & EMU_HANA_OPTION_DOCK_ONLINE);
}

int upload_card_firmware(iKX *ikx)
{
	dword is_k8=0; ikx->get_dword(KX_DWORD_IS_K8,&is_k8);
	dword is_cardbus=0; ikx->get_dword(KX_DWORD_IS_CARDBUS,&is_cardbus);
	dword subsys=0; ikx->get_dword(KX_DWORD_SUBSYS,&subsys);
	
	// power-down audiodock first [won't work for microdock/v2-cards]
	if(!is_k8)
		ikx->fpga_write(EMU_HANA_DOCK_PWR,0);
	
	if(is_fpga_programmed(ikx))
	{
		// FPGA netlist already present so clear it
		// Return to programming mode
		ikx->fpga_write(EMU_HANA_FPGA_CONFIG, EMU_HANA_FPGA_CONFIG_HANA);
	}
	if(is_fpga_programmed(ikx))
	{
		printf("Error: Failed to set FPGA to programming mode\n");
		return -5;
	}
	
	int ret;
	
	if(is_cardbus) // 1616 cardbus
	{
		if(is_k8)
		{
			if(subsys==0x42011102) // EM8950, 1616 cardbus
				ret=ikx->upload_fpga_firmware(emu1010_notebook_netlist,sizeof(emu1010_notebook_netlist));
			else
			{
				printf("Error: your E-DSP card is cardbus, but not EM8950\n");
				// ret=ikx->upload_fpga_firmware(emu1010_notebook_netlist,sizeof(emu1010_notebook_netlist));
				ret=-11;
			}
		}
		else
		{
			printf("Error: your E-DSP card is cardbus/PCMCIA, but not p17v-based\n");
			ret=-11;
		}
	}
	
	if(is_k8) // v2 EM8960 or v2 0404 or PCIe 0404 EM8982
	{
		if(subsys==0x40021102 || subsys==0x40051102) // 0404 or EM8982 PCIe 0404
		{
			ret=ikx->upload_fpga_firmware(emu0404_netlist,sizeof(emu0404_netlist));  // 0404 v2
		}
		else
			if(subsys==0x40041102) // v2 EM8960 ('PCI' series)
				ret=ikx->upload_fpga_firmware(emu1010b_netlist,sizeof(emu1010b_netlist));
		else
			if(subsys==0x40071102) // EM8982 - 1010 ('PCIe' series)
			{
				ret=ikx->upload_fpga_firmware(emu1010b_netlist,sizeof(emu1010b_netlist));
			}
			else
			{
				printf("Error: your E-DSP card is not recognized, assume it is v2 EM8960-like\n");
				ret=ikx->upload_fpga_firmware(emu1010b_netlist,sizeof(emu1010b_netlist));
			}
	}
	else // v1 1010 or v1 0404
	{
		if(subsys==0x40021102)
			ret=ikx->upload_fpga_firmware(emu0404_netlist,sizeof(emu0404_netlist)); // 0404 v1
		else
			if(subsys==0x40011102)
				ret=ikx->upload_fpga_firmware(hana_netlist,sizeof(hana_netlist)); // original v1 1010
			else
			{
				printf("Warning: your E-DSP card is not recognized\n");
				// ret=ikx->upload_fpga_firmware(hana_netlist,sizeof(hana_netlist));
				ret=-11;
			}
	}
	
	if(ret)
		printf("Error: FPGA could not be programmed [%x]\n",ret);
	
	// check if it was OK
	if(ret==0)
	{
		if(!is_fpga_programmed(ikx))
		{
			printf("Error: FPGA programming failed!\n");
			ret=-5;
		}
	}
	
	if(ret==0)
	{
		printf("FPGA firmware uploaded successfully\n");
		
		byte v1,v2,v3,v4,v5;
		ikx->fpga_read(EMU_HANA_MAJOR_REV,&v1);
		ikx->fpga_read(EMU_HANA_MINOR_REV,&v2);
		ikx->fpga_read(EMU_DOCK_MAJOR_REV,&v3);
		ikx->fpga_read(EMU_DOCK_MINOR_REV,&v4);
		ikx->fpga_read(EMU_DOCK_BOARD_ID,&v5);
		
		printf("FPGA Version: Hana: %d.%d, Dock: %d.%d, Board: %d\n",v1,v2,v3,v4,v5);
		
		// Power-On AudioDock
		if(!is_k8)
			ikx->fpga_write(EMU_HANA_DOCK_PWR,EMU_HANA_DOCK_PWR_ON);
		
#if defined(WIN32)
		Sleep(100);
#elif defined(__APPLE__)
		sleep(1);
#else
#error unknown architecture
#endif
		
		// Unmute all. Default is muted after a firmware load
		ikx->fpga_write(EMU_HANA_UNMUTE,EMU_UNMUTE);
	}
	
	return ret;
}

int upload_dock_firmware(iKX *ikx)
{
	dword is_k8=0; ikx->get_dword(KX_DWORD_IS_K8,&is_k8);
	dword is_cardbus=0; ikx->get_dword(KX_DWORD_IS_CARDBUS,&is_cardbus);
	dword subsys=0; ikx->get_dword(KX_DWORD_SUBSYS,&subsys);
	
	int ret=-1;
	
	if (is_dock_connected(ikx))
	{
		// Audio Dock attached
		// Return to Audio Dock programming mode */
		printf("Found AudioDock in OFFLINE state, upload FPGA firmware now...\n");
		
		ikx->fpga_write(EMU_HANA_UNMUTE,EMU_MUTE);
		ikx->fpga_write(EMU_HANA_FPGA_CONFIG, EMU_HANA_FPGA_CONFIG_AUDIODOCK);
		
		if(is_k8)
		{
			if(subsys==0x42011102 ||  // this is microdock
			   subsys==0x40041102 ||  // this is v2 EM8960
			   subsys==0x40071102)    // this is PCIe 1010
			{
                ret=ikx->upload_fpga_firmware(micro_dock_netlist,sizeof(micro_dock_netlist));
			}
			else
			{
                printf("Error: your Dock seems to be MicroDock, but it is not EM8960, 1010-PCIe or EM8950\n");
                // ret=ikx->upload_fpga_firmware(micro_dock_netlist,sizeof(micro_dock_netlist));
                ret=-11;
			}
		}
		else
		{
			if(subsys==0x40011102) // original v1
			{
				ret=ikx->upload_fpga_firmware(audio_dock_netlist,sizeof(audio_dock_netlist));
			}
			else
			{
				printf("Warning: your Dock seems to be AudioDock, but it is not recognized\n");
				// ret=ikx->upload_fpga_firmware(audio_dock_netlist,sizeof(audio_dock_netlist));
				ret=-11;
			}
		}
		
		if(ret)
			printf("Error: FPGA could not be programmed [%x]\n",ret);
		
		ikx->fpga_write(EMU_HANA_FPGA_CONFIG, 0);
		
		// FIXME: do we need this?..
		byte tmp=0;
		ikx->fpga_read(EMU_HANA_IRQ_STATUS, &tmp);
		
		if(ret==0)
			if(!is_fpga_programmed(ikx))
			{
				// FPGA failed to be programmed
				printf("Failed to program Dock FPGA!\n");
				ret=-12;
			}
		
		byte v1,v2,v3,v4,v5;
		ikx->fpga_read(EMU_HANA_MAJOR_REV,&v1);
		ikx->fpga_read(EMU_HANA_MINOR_REV,&v2);
		ikx->fpga_read(EMU_DOCK_MAJOR_REV,&v3);
		ikx->fpga_read(EMU_DOCK_MINOR_REV,&v4);
		ikx->fpga_read(EMU_DOCK_BOARD_ID,&v5);
		
		printf("FPGA Version: Hana: %d.%d, Dock: %d.%d, Board: %d\n",v1,v2,v3,v4,v5);
		
		// Sync clocking between 1010 and the Dock
		// Allow DLL to settle
#if defined(WIN32)
		Sleep(100);
#elif defined(__APPLE__)
		sleep(1);
#else
#error unknown architecture
#endif
		// Unmute all. Default is muted after a firmware load
		ikx->fpga_write(EMU_HANA_UNMUTE,EMU_UNMUTE);
	}
	else
		printf("Dock is not connected\n");
	
	return ret;
}

void load_defaults(iKX *ikx)
{
	dword is_k8=0; ikx->get_dword(KX_DWORD_IS_K8,&is_k8);
	// usually 1 for new p17v-based E-DSP: 1616m cardbus, 1616m PCI, 1212m PCI, ...
	
	// mute all first
	ikx->fpga_write(EMU_HANA_UNMUTE,EMU_MUTE);
	
	byte val=0;
	
	byte cards=0;
	ikx->fpga_read(EMU_HANA_OPTION_CARDS,&cards);
	
	ikx->fpga_read(EMU_HANA_DOCK_PWR,&val);
	// MicroDock has no power control, while AudioDock has one; for MicroDock FPGA returns 0x1f in this register
	if(val!=0x1f)
		ikx->fpga_write(EMU_HANA_DOCK_PWR,EMU_HANA_DOCK_PWR_ON);
	
	ikx->fpga_write(EMU_HANA_WCLOCK,EMU_HANA_WCLOCK_INT_48K);
	
	ikx->fpga_write(EMU_HANA_DEFCLOCK,EMU_HANA_DEFCLOCK_48K);
	
	ikx->fpga_write(EMU_HANA_IRQ_ENABLE,EMU_HANA_IRQ_WCLK_CHANGED|EMU_HANA_IRQ_ADAT|EMU_HANA_IRQ_DOCK|EMU_HANA_IRQ_DOCK_LOST);
	
	ikx->fpga_write(EMU_HANA_SPDIF_MODE,EMU_HANA_SPDIF_MODE_TX_COMSUMER|EMU_HANA_SPDIF_MODE_RX_COMSUMER);
	
	ikx->fpga_write(EMU_HANA_OPTICAL_TYPE,EMU_HANA_OPTICAL_IN_SPDIF|EMU_HANA_OPTICAL_OUT_SPDIF);
	
	byte midi_in=0;
	
	if(cards&EMU_HANA_OPTION_HAMOA)
	{
		midi_in|=EMU_HANA_MIDI_INA_FROM_HAMOA;
		
		// hamoa + dock?
		if(cards&EMU_HANA_OPTION_DOCK_ONLINE)
			midi_in|=EMU_HANA_MIDI_INA_FROM_DOCK1;
	}
	else
		if(cards&EMU_HANA_OPTION_DOCK_ONLINE)
			midi_in=EMU_HANA_MIDI_INA_FROM_DOCK1|EMU_HANA_MIDI_INA_FROM_DOCK2;
	
	ikx->fpga_write(EMU_HANA_MIDI_IN,midi_in);
	
	// MIDI Out:
	//   HAMOA: MIDI A
	//   Dock1: MIDI A
	//   Dock2: MIDI B
	//   Sync:  MIDI B
	ikx->fpga_write(EMU_HANA_MIDI_OUT,EMU_HANA_MIDI_OUT_DOCK2|EMU_HANA_MIDI_OUT_SYNC2);
	
	ikx->fpga_write(EMU_HANA_DOCK_LEDS_1,0x0);
	ikx->fpga_write(EMU_HANA_DOCK_LEDS_2,0x12);
	ikx->fpga_write(EMU_HANA_DOCK_LEDS_3,0x0);
	
	ikx->fpga_write(EMU_HANA_ADC_PADS,0x0 /* EMU_HANA_DOCK_ADC_PAD1|EMU_HANA_DOCK_ADC_PAD2|EMU_HANA_DOCK_ADC_PAD3|EMU_HANA_0202_ADC_PAD1*/ ); // +4dB PRO
	ikx->fpga_write(EMU_HANA_DAC_PADS,0x0 /* EMU_HANA_DOCK_DAC_PAD1|EMU_HANA_DOCK_DAC_PAD2|EMU_HANA_DOCK_DAC_PAD3|EMU_HANA_DOCK_DAC_PAD4|EMU_HANA_0202_DAC_PAD1*/); // +4dB PRO
	
	// FIXME !!
	// microdock:
	// ikx->fpga_write(EMU_HANA_DOCK_MISC,0x1f);
	// audiodock:
	// ikx->fpga_write(EMU_HANA_DOCK_MISC,EMU_HANA_DOCK_PHONES_192_DAC1);
	
	// default connections:
	// ********************
	
	// out10,out11,out12,out13 [epilog's out17,out18,out30,out31] -> unused
	
	// out0,out1:
    // for 1212m, 0404 -> output [300,301]; 1616m -> headphones [300,301]
    ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+0,EMU_DST_HAMOA_DAC_LEFT1);
    ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+1,EMU_DST_HAMOA_DAC_RIGHT1);
    // for audiodock: -> DAC1 [100,104]
    ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+0,EMU_DST_DOCK_DAC1_LEFT1);
    ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+1,EMU_DST_DOCK_DAC1_RIGHT1);
    // and -> headphones [112,116]
    if(!is_k8)
    {
		ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+0,EMU_DST_DOCK_PHONES_LEFT1);
		ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+1,EMU_DST_DOCK_PHONES_RIGHT1);
    }
	
	// out2,out3:
    // on-board SPDIF [200, 201]
    ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+2,EMU_DST_HANA_SPDIF_LEFT1);
    ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+3,EMU_DST_HANA_SPDIF_RIGHT1);
    // and dock SPDIF [11a,11e] (AudioDock only, not MicroDock)
    if(!is_k8)
    {
		ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+2,EMU_DST_DOCK_SPDIF_LEFT1);
		ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+3,EMU_DST_DOCK_SPDIF_RIGHT1);
    }
    else // and dock SPDIF [112,116] (MicroDock, not AudioDock)
    {
		ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+2,EMU_DST_MDOCK_SPDIF_LEFT1);
		ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+3,EMU_DST_MDOCK_SPDIF_RIGHT1);
    }
	
	// out4,out5:
    // for audiodock: -> DAC2 [108,10c]
    ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+4,EMU_DST_DOCK_DAC2_LEFT1);
    ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+5,EMU_DST_DOCK_DAC2_RIGHT1);
	
	// out6,out7:
    // for audiodock: -> DAC3 [110,114]
    ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+6,EMU_DST_DOCK_DAC3_LEFT1);
    ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+7,EMU_DST_DOCK_DAC3_RIGHT1);
	
	// out8,out9:
	if(!is_k8)
	{
		// for audiodock: -> DAC4 [118,11c]
		ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+8,EMU_DST_DOCK_DAC4_LEFT1);
		ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+9,EMU_DST_DOCK_DAC4_RIGHT1);
	}
	// else: for MicroDock these outputs are already used by ADAT
	
	if(is_k8)
	{
		// MicroDock ADAT [118..11f]
		ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+2,EMU_DST_MDOCK_ADAT+0);
		ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+3,EMU_DST_MDOCK_ADAT+1);
		ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+4,EMU_DST_MDOCK_ADAT+2);
		ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+5,EMU_DST_MDOCK_ADAT+3);
		ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+6,EMU_DST_MDOCK_ADAT+4);
		ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+7,EMU_DST_MDOCK_ADAT+5);
		ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+8,EMU_DST_MDOCK_ADAT+6);
		ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+9,EMU_DST_MDOCK_ADAT+7);
	}
	
	// out2,3 4,5 6,7 8,9:
	// on-board ADAT (400..407)
	ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+2,EMU_DST_HANA_ADAT+0);
	ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+3,EMU_DST_HANA_ADAT+1);
	ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+4,EMU_DST_HANA_ADAT+2);
	ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+5,EMU_DST_HANA_ADAT+3);
	ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+6,EMU_DST_HANA_ADAT+4);
	ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+7,EMU_DST_HANA_ADAT+5);
	ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+8,EMU_DST_HANA_ADAT+6);
	ikx->fpga_link_src2dst(EMU_SRC_ALICE_EMU32A+9,EMU_DST_HANA_ADAT+7);
	
	// inputs
	// ******
	int input=0;
	int table[16] = { 0xf,0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe };
	
	// in 0,1 <- A,B
	// in 2,3 <- ADC1
	// in 4,5 <- ADC2
	// in 6,7 <- ADC3
	ikx->fpga_link_src2dst(EMU_SRC_DOCK_MIC_A1,table[input++]);
	ikx->fpga_link_src2dst(EMU_SRC_DOCK_MIC_B1,table[input++]);
	
	ikx->fpga_link_src2dst(EMU_SRC_DOCK_ADC1_LEFT1,table[input++]);
	ikx->fpga_link_src2dst(EMU_SRC_DOCK_ADC1_RIGHT1,table[input++]);
	
	ikx->fpga_link_src2dst(EMU_SRC_DOCK_ADC2_LEFT1,table[input++]);
	ikx->fpga_link_src2dst(EMU_SRC_DOCK_ADC2_RIGHT1,table[input++]);
	
	ikx->fpga_link_src2dst(EMU_SRC_DOCK_ADC3_LEFT1,table[input++]);
	ikx->fpga_link_src2dst(EMU_SRC_DOCK_ADC3_RIGHT1,table[input++]);
	
	// in 8,9 SPDIF-In
	ikx->fpga_link_src2dst(EMU_SRC_HANA_SPDIF_LEFT1,table[input++]);
	ikx->fpga_link_src2dst(EMU_SRC_HANA_SPDIF_RIGHT1,table[input++]);
	
	// in 10,11:
	// for 0404 and 1212, use their inputs first
	ikx->fpga_link_src2dst(EMU_SRC_HAMOA_ADC_LEFT1,table[input++]);
	ikx->fpga_link_src2dst(EMU_SRC_HAMOA_ADC_RIGHT1,table[input++]);
	
	// in 12,13, 14,15 ADAT-In [Channel 0,1 only]
	//    (default 'prolog' will not display 14,15)
	ikx->fpga_link_src2dst(EMU_SRC_HANA_ADAT+0,table[input++]);
	ikx->fpga_link_src2dst(EMU_SRC_HANA_ADAT+1,table[input++]);
	ikx->fpga_link_src2dst(EMU_SRC_HANA_ADAT+2,table[input++]);
	ikx->fpga_link_src2dst(EMU_SRC_HANA_ADAT+3,table[input++]);
	
	// NOTE:
	// for MicroDock, we can also use:
	// EMU_SRC_MDOCK_SPDIF_xxx
	// and EMU_SRC_MDOCK_ADAT
	
	// unmute now
	ikx->fpga_write(EMU_HANA_UNMUTE,EMU_UNMUTE);
	
	printf("==== E-DSP Parameters were reset to defaults\n");
}

void print_status(iKX *ikx)
{
	printf("==== Current E-DSP status\n");
	
	printf("Card: '%s'\n",ikx->get_device_name());
	
	byte val;
	
	ikx->fpga_read(EMU_HANA_OPTION_CARDS,&val);
	
	printf("Options: %x - %s %s %s %s\n",
		   val,
		   val&EMU_HANA_OPTION_HAMOA?"Hamoa":"",
		   val&EMU_HANA_OPTION_SYNC?"Sync":"",
		   val&EMU_HANA_OPTION_DOCK_ONLINE?"Dock_online":"",
		   val&EMU_HANA_OPTION_DOCK_OFFLINE?"Dock_offline":"");
	
	ikx->fpga_read(EMU_HANA_ID,&val);
	printf("Hana ID: %x\n",val);
	
	byte v1,v2,v3,v4,v5;
	ikx->fpga_read(EMU_HANA_MAJOR_REV,&v1);
	ikx->fpga_read(EMU_HANA_MINOR_REV,&v2);
	ikx->fpga_read(EMU_DOCK_MAJOR_REV,&v3);
	ikx->fpga_read(EMU_DOCK_MINOR_REV,&v4);
	ikx->fpga_read(EMU_DOCK_BOARD_ID,&v5);
	
	printf("Versions: %d.%d, %d.%d, %d\n",v1,v2,v3,v4,v5);
	
	ikx->fpga_read(EMU_HANA_DOCK_PWR,&val);
	if(val!=0x1f)
		printf("Dock is %s. [0x%x]\n",val&EMU_HANA_DOCK_PWR_ON?"Powered-On":"Powered-off",val);
	else
		printf("Dock power state is not known, probably, self-powered MicroDock [%x]\n",val);
	
	const char *wclock_list[]=
	{
		"Int 48",
		"Int 44.1",
		"HANA SPDIF-In",
		"HANA ADAT-In",
		"Sync BNC",
		"2nd Hana",
		"Reserved",
	"OFF" };
	
	ikx->fpga_read(EMU_HANA_WCLOCK,&val);
	printf("WClock: %x [%s]\n",val&EMU_HANA_WCLOCK_SRC_MASK,wclock_list[val&EMU_HANA_WCLOCK_SRC_MASK]);
	
	ikx->fpga_read(EMU_HANA_DEFCLOCK,&val);
	printf("Def clock: %d\n",(val==EMU_HANA_DEFCLOCK_48K)?48000:(val==EMU_HANA_DEFCLOCK_44_1K)?44100:-1);
	
	ikx->fpga_read(EMU_HANA_UNMUTE,&val);
	printf("HANA is %s\n",(val==EMU_MUTE)?"Muted":(val==EMU_UNMUTE)?"UnMuted":"Unknown!");
	
	ikx->fpga_read(EMU_HANA_IRQ_ENABLE,&val);
	printf("HANA IRQs: %x\n",val);
	
	ikx->fpga_read(EMU_HANA_SPDIF_MODE,&val);
	printf("SPDIF mode [%x]:\n\tOut: %s%s\n\tIn: %s%s%s\n",val,
		   (val&EMU_HANA_SPDIF_MODE_TX_PRO)?"Professional":"Consumer",
		   (val&EMU_HANA_SPDIF_MODE_TX_NOCOPY)?"; Copy-protected":"",
		   (val&EMU_HANA_SPDIF_MODE_RX_PRO)?"; Professional":"Consumer",
		   (val&EMU_HANA_SPDIF_MODE_RX_NOCOPY)?"; Copy-protected":"",
		   (val&EMU_HANA_SPDIF_MODE_RX_INVALID)?"; Not Valid":"");
	
	ikx->fpga_read(EMU_HANA_OPTICAL_TYPE,&val);
	printf("Optical Type: [%x] -- Input: %s, Output: %s\n",val,
		   (val&EMU_HANA_OPTICAL_IN_ADAT)?"ADAT":"SPDIF",
		   (val&EMU_HANA_OPTICAL_OUT_ADAT)?"ADAT":"SPDIF");
	
	const char *midi_in[4]={ "none", "0202", "dock1", "dock2 " };
	
	ikx->fpga_read(EMU_HANA_MIDI_IN,&val);
	printf("MIDI In: %s, %s [%x]\n",
		   midi_in[(val&3)],midi_in[(val>>3)&3],val);
	
	ikx->fpga_read(EMU_HANA_MIDI_OUT,&val);
	printf("MIDI Out: %x\n",val);
	
	ikx->fpga_read(EMU_HANA_DOCK_LEDS_1,&v1);
	ikx->fpga_read(EMU_HANA_DOCK_LEDS_2,&v2);
	ikx->fpga_read(EMU_HANA_DOCK_LEDS_3,&v3);
	printf("LEDs: %x %x %x\n",v1,v2,v3);
	
	ikx->fpga_read(EMU_HANA_ADC_PADS,&val);
	printf("ADC pads: %x - %s%s%s%s\n",
		   val,
		   (val&EMU_HANA_DOCK_ADC_PAD1)?"Dock_1 ":"",
		   (val&EMU_HANA_DOCK_ADC_PAD2)?"Dock_2 ":"",
		   (val&EMU_HANA_DOCK_ADC_PAD3)?"Dock_3 ":"",
		   (val&EMU_HANA_0202_ADC_PAD1)?"0202_1 ":"");
	
	ikx->fpga_read(EMU_HANA_DAC_PADS,&val);
	printf("DAC pads: %x - %s%s%s%s%s\n",
		   val,
		   (val&EMU_HANA_DOCK_DAC_PAD1)?"Dock_1 ":"",
		   (val&EMU_HANA_DOCK_DAC_PAD2)?"Dock_2 ":"",
		   (val&EMU_HANA_DOCK_DAC_PAD3)?"Dock_3 ":"",
		   (val&EMU_HANA_DOCK_DAC_PAD4)?"Dock_4 ":"",
		   (val&EMU_HANA_0202_DAC_PAD1)?"0202_1 ":"");
	
	// FIXME !! this register is not valid for MicroDock ??
	ikx->fpga_read(EMU_HANA_DOCK_MISC,&val);
	if(val!=0x1f)
		printf("Dock misc: [%x] DAC mute: %d%d%d%d headphones: %d\n",val,
			   (val&EMU_HANA_DOCK_DAC1_MUTE)?1:0,
			   (val&EMU_HANA_DOCK_DAC2_MUTE)?1:0,
			   (val&EMU_HANA_DOCK_DAC3_MUTE)?1:0,
			   (val&EMU_HANA_DOCK_DAC4_MUTE)?1:0,
			   val>>4);
	else
		printf("Dock misc flags are set to %x, probably, MicroDock\n",val);
	
	ikx->fpga_read(0x38,&val); // FIXME !! register not documented
	
	if ((val & 0x1) == 0) 
	{
		byte val1, val2;
		ikx->fpga_read(EMU_HANA_WC_ADAT_HI,&val1);
		ikx->fpga_read(EMU_HANA_WC_ADAT_LO,&val2);
		printf("ADAT Locked : %d\n", 0x1770000 / (((val1 << 5) | val2)+1));
	} else
		printf("ADAT Unlocked\n");
	
	ikx->fpga_read(EMU_HANA_IRQ_STATUS,&val);
	
	if ((val & EMU_HANA_IRQ_DOCK) == 0)  // ??? FIXME !! this is not correct
	{
		byte val1,val2;
		ikx->fpga_read(EMU_HANA_WC_SPDIF_HI,&val1);
		ikx->fpga_read(EMU_HANA_WC_SPDIF_LO,&val2);
		
		printf("SPDIF Locked : %d\n", 0x1770000 / (((val1 << 5) | val2)+1));
	} else
		printf("SPDIF Unlocked\n");
	
	if( 1 ) // FIXME don't know BNC lock bit location
	{  
		byte val1,val2;
		ikx->fpga_read(EMU_HANA_WC_BNC_HI,&val1);
		ikx->fpga_read(EMU_HANA_WC_BNC_LO,&val2);
		
		printf("BNC Locked : %d\n", 0x1770000 / (((val1 << 5) | val2)+1));
	} else
		printf("Sync card unlocked\n");
	
	if ( 1 ) // FIXME don't know SPDIF lock bit location
	{
		byte val1,val2;
		ikx->fpga_read(EMU_HANA2_WC_SPDIF_HI,&val1);
		ikx->fpga_read(EMU_HANA2_WC_SPDIF_LO,&val2);
		
		printf("SPDIF Locked : %d\n", 0x1770000 / (((val1 << 5) | val2)+1));
	} else
		printf("SPDIF Unlocked\n");
	
	printf("reg 14-1f:");  
	for(int i=0x14;i<0x20;i++)
	{
		ikx->fpga_read((byte)i,&val);
		printf("[%x] ",val);
	}
	printf("\n");
	
	printf("reg 30-3f:");
	for(int i=0x30;i<0x3f;i++)
	{
		ikx->fpga_read((byte)i,&val);
		printf("[%x] ",val);
	}
	printf("\n");
}
