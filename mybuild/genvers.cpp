// kX Driver
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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/utime.h>
#include <time.h>

char *str=
"// kX Driver\n"
"// Copyright (c) Eugene Gavrilov, 2001-2014.\n"
"// All rights reserved\n"
"\n"
"// --- automatically generated file - DO NOT EDIT\n"
"\n"
"// version: %4d\n\n"
"#ifndef VER_H_\n"
"#define VER_H_\n"
"\n"
"#define KX_DRIVER_DATE		%02d/%02d/%4d\n"
"\n"
"\n"
"#define KX_VERSION_DWORD		0x5100%04d\n"
"#ifdef KX_DEBUG\n"
" #define KX_DRIVER_VERSION_STR		\"5.10.00.%04d - debug\"\n"
"#else\n"
" #define KX_DRIVER_VERSION_STR		\"5.10.00.%04d\"\n"
"#endif\n"
"#define KX_DRIVER_VERSION		5,10,0,%04d\n"
"#define KX_DRIVER_VERSION_		5.10.0.%04d\n"
"#ifdef KX_DEBUG\n"
" #define KX_VERSION_STR			\"5, 10, 00, %04d - debug\"\n"
"#else\n"
" #define KX_VERSION_STR			\"5, 10, 00, %04d\"\n"
"#endif\n"
"#define KX_COPYRIGHT_STR		\"Copyright © Eugene Gavrilov, 2001-2014. All rights reserved.\"\n"
"#define KX_COPYRIGHT_STR_R		\"Copyright © Eugene Gavrilov, 2001-2014.\"\n"
"#define KX_COPYRIGHT_YEARS		\"2001-2014\"\n"
"#define KX_COMPANY_NAME			\"Eugene Gavrilov\"\n"
"\n"
"#define KX_AC3_VERSION \"8.1.0\"\n"
"\n"
"#define KX_ASIO_VERSION		%d\n"
"\n"
"#endif\n";

int __cdecl main(void)
{
 int ver=810;
 int month=10;
 int day=31;
 int year=2001;
 char *b_root=getenv("KX_BUILD_ROOT");
 char b_path[512];
 char *i_root=getenv("HTTPD_ROOT");

 if(b_root==0)
 {
  printf("KX_BUILD_ROOT environment not set\n");
  return -5;
 }
 if(i_root==0)
 {
  printf("HTTPD_ROOT environment not set\n");
  return -6;
 }

 tm *tim;
 time_t current;
 time(&current);
 tim=localtime(&current);

 month=tim->tm_mon+1;
 day=tim->tm_mday;
 year=tim->tm_year+1900;

 FILE *f;
 strcpy(b_path,b_root); strcat(b_path,"\\h\\vers.h");
 f=fopen(b_path,"rt");
 if(!f)
 {
  perror("Can't access vers.h");
  return -5;
 }
 char buff[1024];
 fread(buff,1,1024,f);
 fclose(f);
 char *p=strstr(buff,"// version:");
 if(p==0)
 {
  fprintf(stderr,"Incorrect vers.h file\n");
  return -6;
 }
 p+=11;
 sscanf(p,"%d",&ver);
 struct _stat st;
 if(_stat(b_path,&st)!=0)
 {
  fprintf(stderr,"Error 'stat' on vers.h\n");
  return -9;
 }
 f=fopen(b_path,"wt");
 if(!f)
 {
  perror("Error opening vers.h for writing");
  return -10;
 }
 ver++;
 fprintf(f,str,ver,month,day,year,ver,ver,ver,ver,ver,ver,ver,ver);
 fclose(f);

 f=fopen("..\\oem_env.mak","wt");
 fprintf(f,"# machine-generated file for 'makefile', do NOT edit\n\nKX_VERSION=%04d\n\n",ver);
 fclose(f);

 _utimbuf t;
 t.actime=st.st_atime;
 t.modtime=st.st_mtime;

 _utime(b_path,&t);

 printf("[Build Version: %d (%02d/%02d/%04d)\n",
  ver,month,day,year);

 strcpy(b_path,i_root); strcat(b_path,"\\help\\license.txt");
 f=fopen(b_path,"rt");
 strcpy(b_path,b_root); strcat(b_path,"\\h\\license.h");

 FILE *f2=fopen(b_path,"wt");
 char str[1024];
 while(!feof(f))
 {
  fgets(str,1024,f);
  if(feof(f))
   break;

  while(str[strlen(str)-1]=='\n' || str[strlen(str)-1]=='\r')
   str[strlen(str)-1]='\0';
  char *p=str;
  while(*p)
  {
   if(*p=='"') *p='\'';
   p++;
  }
  fprintf(f2,"\"%s\\n\"\n",str);
 }
 fclose(f2);
 fclose(f);

 return 0;
}
