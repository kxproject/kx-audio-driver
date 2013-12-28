// kX Project
// Copyright (c) Eugene Gavrilov, 2001-2014
// All rights reserved

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

int find(char *where,char *section,char *key,char *output)
{
 if(!where)
  return -1;

 char tmp_section[256];
 sprintf(tmp_section,"[%s]\n",section);

 char *start=strstr(where,tmp_section);
 if(start)
 {
   start+=strlen(tmp_section);
   char *end=strstr(start,"\n[");
   if(end)
    *end=0;

   char tmp_key[1024];
   sprintf(tmp_key,"\n%s=",key);

   char *val=strstr(start,tmp_key);
   
   if(val)
   {
     val+=strlen(tmp_key);
     char *p=strstr(val,"\n");
     if(p)
     {
      *p=0;
      strcpy(output,val);
      *p='\n';
      if(end)
       *end='\n';
      return 0;
     }    
   }

   if(end)
    *end='\n';
 }

 return -1;
}

char *mix=NULL;
char *inp=NULL;

int read_file(char *fname,char **addr)
{
 *addr=NULL;
 FILE *f;
 f=fopen(fname,"rt");
 if(f)
 {
   fseek(f,0,SEEK_END);
   size_t fsize=ftell(f);
   fseek(f,0,SEEK_SET);

   *addr=(char *)malloc(fsize);
   if(*addr)
   {
    fread(*addr,1,fsize,f);
    fclose(f);
    return 0;
   }
   fclose(f);
 }
 return -1;
}

void main(int argc, char **argv)
{
 if(argc<2) { printf("invalid argument\n"); exit(1); };

 read_file("mix",&mix);
 read_file("inp",&inp);

 if(inp)
  printf("[parser]: adding language\n");
 else
  printf("[parser]: compiling language\n");

 FILE *fin=fopen("english","rt");
 if(fin)
 {
   FILE *fout=fopen(argv[1],"wt");
   if(fout)
   {
      char section[128];
      section[0]=0;

      while(!feof(fin))
      {
      	char line[4096];
      	line[0]=0;

      	if(fgets(line,sizeof(line),fin)) // ok
      	{
          // parse
      	  if((line[0]=='\r') || (line[0]=='\n'))
      	  { fputs(line,fout); continue; }
      	  if(line[0]=='#')
      	  { fputs(line,fout); continue; }

      	  if(line[0]=='[')
      	  {
      	   strncpy(section,line+1,sizeof(section));
      	   char *d=strchr(section,']');
      	   if(d)
      	    *d=0;
      	   fputs(line,fout);
      	   continue;
      	  }

          // output english line
      	  fputs(line,fout);

          char *p;
          p=strchr(line,'=');
          if(p)
          {
            *p=0;

            p++;

            while((p[strlen(p)-1]=='\n')||(p[strlen(p)-1]=='\r'))
             p[strlen(p)-1]=0;

            int inps[]=  {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     };
            int ids []=  {0x402,0x404,0x406,0x407,0x408,0x40c,0x40e,0x410,0x411,0x412,0x413,0x414,0x415,0x416,0x418,0x419,0x41a,0x41d,0x41e,0x41f,0x422,0x804,0x80a,0x40c0 };
            int flags[]= {0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     };

            #pragma warning(disable:4127)
            if( (sizeof(flags)!=sizeof(ids)) || (sizeof(flags)!=sizeof(inps)) )
            {
             printf("recomiple!\n");
             fprintf(stderr,"recompile!\n");
             return;
            }

            char output[4096];
            int i;

            // for each

            for(i=0;i<sizeof(flags)/sizeof(flags[0]);i++)
            {
             char key[256];

             // try 'inp'
             if(inps[i]!=0 && flags[i]==0)
             {
                 sprintf(key,"%s.%x",line,ids[i]);
                 if(find(inp,section,key,output)==0)
                 {
                   fprintf(fout,"%s=%s\n",key,output);
                   flags[i]=1;
                 }
             }

             // now, mix
             if(ids[i]!=0 && flags[i]==0)
             {
                 sprintf(key,"%s.%x",line,ids[i]);
                 if(find(mix,section,key,output)==0)
                 {
                   fprintf(fout,"%s=%s\n",key,output);
                   flags[i]=1;
                 }
             }

            if(flags[i]==0)
             fprintf(fout,"# %s.%x=???\n",line,ids[i]);
            } // for
           } // second if p
      	} // if p
      	 else
      	  break;
      } // while
      fclose(fout);
  }
  fclose(fin);
 }
}
