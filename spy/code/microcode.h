// kX Driver
// Copyright (c) Eugene Gavrilov, 2001-2014
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


fprintf(stderr,"kX Microcode Dumper\nCopyright kX Project, 2003-2005. All rights reserved. Written by Eugene Gavrilov\n\n");

if(argv[1])
{

 if(argv[1][0]=='s')
 {

argv++;
 if(argv[1] && argv[1][0])
 {
 FILE *f=fopen(argv[1],"wb");
 if(f)
 {

 dword data;
 char buff[17];
 sprintf(buff,"10k%c microcode $",hw->is_10k2?'2':'1');
 fwrite(buff,1,16,f);

 dword magic='$rcm';
 fwrite(&magic,1,4,f);
 for(int i=0;i<hw->microcode_size*2;i++)
 {
  data=kx_readptr(hw,hw->first_instruction+i,0);
  fwrite(&data,1,4,f);
  }
 magic='$rpg';
 fwrite(&magic,1,4,f);
 for (int i = 0; i < (hw->is_10k2?512:256); i++)
 {
 	data=kx_readptr(hw, (hw->is_10k2?E10K2_GPR_BASE:E10K1_GPR_BASE)+i,0);
        fwrite(&data,1,4,f);
 }
 magic='$mrt';
 fwrite(&magic,1,4,f);

 for (int i = 0; i < 256; i++)
 {
 	data=kx_readptr(hw,TANKMEMADDRREGBASE + i,0);
        fwrite(&data,1,4,f);
 	data=kx_readptr(hw,TANKMEMDATAREGBASE + i,0);
        fwrite(&data,1,4,f);
 }
 if(hw->is_10k2)
 {
   magic='$glf';
   fwrite(&magic,1,4,f);
   for (int i = 0; i < 256; i++)
   {
   	  // 10k2 tram flags...
   	  data=kx_readptr(hw, E10K1_GPR_BASE+i,0);
          fwrite(&data,1,4,f);
   }
 }
 fprintf(f,"\n10kX Microcode dump. Written by kX Tool version %x\n",KX_VERSION_DWORD);

 fclose(f);
 }
   else fprintf(stderr,"Invalid file\n");
 } else fprintf(stderr,"Invalid arguments\nUsage: {l|s} <filename>; l-load dsp microcode; s - save\n");

 }
 else
 if(argv[1][0]=='l')
 {
 argv++;

 if(argv[1] && argv[1][0])
 {
 FILE *f=fopen(argv[1],"rb");
 if(f)
 {
 dword data;
 char buff[17],buff2[17];
 fread(buff,1,16,f);
 sprintf(buff2,"10k%c microcode $",hw->is_10k2?'2':'1');
 if(strncmp(buff,buff2,16)!=NULL)
 {
  fprintf(stderr,"Incorrect microcode dump\n");
 }
 else
 {
  do
  {
  	 dword magic=0xdeadbeaf;
         fread(&magic,1,4,f);
         if(magic!='$rcm')
         {
          fprintf(stderr,"Possibly incorrect dump file! [no microcode signature; '%x']\n",magic);
          break;
         }

         for(int i=0;i<hw->microcode_size*2;i++)
         {
          fread(&data,1,4,f);
          kx_writeptr(hw,hw->first_instruction+i,0,data);
         }

         magic=0xdeadbeaf; 
         fread(&magic,1,4,f);
         if(magic!='$rpg')
         {
          fprintf(stderr,"Possibly incorrect dump file! [no gpr signature; '%x']\n",magic);
          break;
         }

         for (int i = 0; i < (hw->is_10k2?512:256); i++)
         {
          fread(&data,1,4,f);
          kx_writeptr(hw, (hw->is_10k2?E10K2_GPR_BASE:E10K1_GPR_BASE) + i,0, data);
         }
         magic=0xdeadbeaf;
         fread(&magic,1,4,f);
         if(magic!='$mrt')
         {
          fprintf(stderr,"Possibly incorrect dump file! [no tram signature; '%x']\n",magic);
          break;
         }

         for (int i = 0; i < 256; i++)
         {
          dword d1,d2;
          fread(&d1,1,4,f);
          fread(&d2,1,4,f);
          kx_writeptr(hw,TANKMEMADDRREGBASE  + i,0, d1);
          kx_writeptr(hw,TANKMEMDATAREGBASE  + i,0, d2);
         }

         if(hw->is_10k2)
         {
         magic=0xdeadbeaf;
         fread(&magic,1,4,f);
         if(magic!='$glf')
         {
          fprintf(stderr,"Possibly incorrect dump file! [no flag signature; '%x']\n",magic);
          break;
         }
         for (int i = 0; i < 256; i++)
         {
          fread(&data,1,4,f);
          kx_writeptr(hw, E10K1_GPR_BASE + i,0, data);
         }
         }
   } while(0);

 }
 fclose(f);
 } // if f
   else fprintf(stderr,"Invalid file\n");
 } // if argv
  else fprintf(stderr,"Invalid arguments\nUsage: {l|s} <filename>; l-load dsp microcode; s - save\n");

 }
 else
  fprintf(stderr,"Incorrect command\nUsage: {l|s} <filename>; l-load dsp microcode; s - save\n");

}
else
 fprintf(stderr,"Usage: [$<device_number>] {l|s} <filename>; l-load dsp microcode; s - save\n");
