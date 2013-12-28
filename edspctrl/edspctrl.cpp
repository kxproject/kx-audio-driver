// kX E-DSP Library
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

/* 
 * Added command line option for selecting which card to program
 * Russell Garber 7/21/08
 */

#include "stdafx.h"
#include "interface/kxapi.h"
#include "vers.h"

#include "edsplib.h"

int main(int argc, char* argv[])
{
#if defined(WIN32)
	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		fprintf(stderr,"Fatal Error: MFC initialization failed\n");
		return 1;
	}
#endif
	
	fprintf(stderr,"kX E-DSP Control Program\n"KX_COPYRIGHT_STR"\n");
	fprintf(stderr,"This program is free software; you can redistribute it and/or modify\n"
			"it under the terms of the GNU General Public License as published by\n"
			"the Free Software Foundation; either version 2 of the License, or\n"
			"(at your option) any later version.\n\n");
	
	int id=0;
	
	if(argc>1 && argv[1][0]=='$')
	{
		id=argv[1][1]-'0';
		argc--; 
		argv++; 
	}
	
	iKX *ikx=iKX::create(id);
	
	if(ikx) // success?
	{
        dword v=0;
        ikx->get_dword(KX_DWORD_IS_EDSP,&v);
        if(!v)
        {
         printf("Your card is not E-DSP\n");
         ikx->destroy();
         return 1;
        }

		printf("==== E-mu E-DSP initialization\n");
		
		int reprogram_fpga=0;
		int reprogram_dock=0;
		int reload_defaults=0;
		
		while(argc>1)
		{
			argv++;
			argc--;
			if(strcmp(argv[0],"-fpga")==0) reprogram_fpga=1;
			else if(strcmp(argv[0],"-dock")==0) reprogram_dock=1;
			else if(strcmp(argv[0],"-defaults")==0) reload_defaults=1;
			else if(strcmp(argv[0],"-reset")==0) { reprogram_fpga=1; reprogram_dock=1; reload_defaults=1; }
		}
		
		if(!is_fpga_programmed(ikx) || reprogram_fpga)
		{
			printf("==== FPGA is not programmed, upload FPGA firmware...\n");
			upload_card_firmware(ikx);
			
			load_defaults(ikx);
			reload_defaults=0;
		}
		else
			printf("==== FPGA appears to be already programmed\n");
		
		if(is_dock_connected(ikx))
		{
			if(!is_dock_online(ikx) || reprogram_dock)
			{
				printf("==== Dock is connected, but appears offline, upload dock firmware...\n");
				
				upload_dock_firmware(ikx);
				load_defaults(ikx);
				reload_defaults=0;
				
			} else printf("==== Dock is connected and is functioning properly\n");
		} else printf("==== Dock is not connected\n");
		
		if(reload_defaults)
			load_defaults(ikx);
		
		print_status(ikx);
		
		ikx->destroy();
	}
	else
		fprintf(stderr,"Error initializing kX interface\n");
    
    return 0;
}
