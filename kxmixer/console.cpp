// kX Mixer
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


#include "stdinc.h"

extern int get_mixer_folder(TCHAR *folder);

void consolidate()
{
         TCHAR mixer_folder[MAX_PATH]; mixer_folder[0]=0;
         get_mixer_folder(mixer_folder);
         _tcscat(mixer_folder,_T("kxctrl.exe"));

         char tmp_str[MAX_PATH];
         sprintf(tmp_str,"$%d --gui",manager->get_current_device()>=0?manager->get_current_device():0);
         spawnlp(_P_NOWAIT,mixer_folder,"kxctrl.exe",tmp_str,NULL);

         // debug("===%s=== ===%s===\n",mixer_folder,tmp_str);
         
/*         if(AllocConsole())
         {
            FILE *fin,*fout;

            fin=fopen("CONIN$","rb");
            fout=fopen("CONOUT$","wb");

            fprintf(fout,"Hello!\n");
            fflush(fout);

            int i;
            fscanf(fin,"%d",&i);

            FreeConsole();
         }
*/
}
