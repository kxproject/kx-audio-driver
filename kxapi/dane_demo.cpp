// kX Driver Interface / Dane Assembler
// Copyright (c) Max Mikhailov, Eugene Gavrilov, 2001-2005.
// All rights reserved

// should be compiled: cl -nologo -MD -D_DEBUG -D_AFXDLL dane.cpp kxapi.lib
// -D_DEBUG is needed because iKX & mfc versions are debug ones

#include <afxwin.h>
#include "kxapi.h"
#include <stdio.h>
#include <sys/stat.h>

int main(int argc,char **argv)
{
 // initialize MFC and print and error on failure
 if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
 {
        fprintf(stderr,"Fatal Error: MFC initialization failed\n");
        return 1;
 }

 iKX *ikx;
 ikx=new iKX(); // we don't call init() --> enables to assemble with no kX drivers installed

 FILE *in,*out;
 char *buf; size_t fsize;
 struct _stat st;

 if(argc<2)
 {
   fprintf(stderr,"kX Dane v%s\nWritten by Max Mikhailov, Eugene Gavrilov. (c) 2001-2005\n\n",
         ikx->get_version()
       );
   fprintf(stderr,"Usage: dane.exe source_file [cpp_output_file]\n");
   delete ikx;
   return 1;
 }

 if(_stat(argv[1],&st)!=0) // error
 {
  perror("dane: input file");
  delete ikx;
  return 2;
 }

 buf=(char *)malloc(st.st_size);
 if(buf==0)
 {
  fprintf(stderr,"dane: not enough memory (%d bytes required)\n",st.st_size);
  delete ikx;
  return 3;
 }

 in=fopen(argv[1],"rt");
 if(in==0)
 {
  perror("dane: input file");
  free(buf);
  delete ikx;
  return 4;
 }

 if(fread(buf,st.st_size,1,in)<0)
 {
  perror("dane: error reading input file");
  fclose(in);
  free(buf);
  delete ikx;
  return 5;
 }
 fclose(in);

 // microcode-related variables
 dsp_register_info *info;
 dsp_code *code;
 int info_size,code_size,itramsize,xtramsize;

 char name[KX_MAX_STRING];
 char copyright[KX_MAX_STRING];
 char engine[KX_MAX_STRING];
 char created[KX_MAX_STRING];
 char comment[KX_MAX_STRING];
 char guid[KX_MAX_STRING];

 int ret=ikx->assemble_microcode(buf,NULL,name,&code,&code_size,&info,&info_size,&itramsize,&xtramsize,
 	copyright,engine,created,comment,guid);

 if(ret==0)
 {
   kString s;
   ret=ikx->disassemble_microcode(s,KX_DISASM_DANE,-1,code,code_size,info,info_size,
        itramsize,xtramsize,
 	name,copyright,engine,created,comment,guid);
   if(info) LocalFree(info);
   if(code) LocalFree(code);	
   if(ret==0)
   {
     if(argc<3)
      out=stdout;
     else
      out=fopen(argv[2],"wt");
     if(out!=0)
     {
       fwrite(s.GetBuffer(1),1,s.GetLength(),out);
       fclose(out);
     } else perror("dane: error opening output file");
   } else fprintf(stderr,"dane: pass2 failed (result code: %d)\n",ret);
 } else fprintf(stderr,"dane: compilation aborted (result code: %d)\n",ret);

 delete ikx;

 return 0;
}
