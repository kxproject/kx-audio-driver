// kX kxctrl: parser
// Copyright (c) Eugene Gavrilov, 2001-2008.
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

// main function to parse any kind of text
int parse_text(char *text,int size,int (*command)(int argc,char **argv));
int parse_text(char *text,int size,int (*command)(int argc,char **argv))
{
	while(size>0)
	{
		char line[1024];
		
		char *crlf=strstr(text,"\n");
		if(!crlf)
			crlf=strstr(text,"\r");
		if(!crlf)
		{
			strncpy(line,text,sizeof(line));
			size=0;
		}
		else
		{
			*crlf=0; strncpy(line,text,sizeof(line)); text=crlf+1;
		}
		
		char *p=line;
		int argc_=0;
		char **argv_;
		
		if(strlen(line)<=0)
			continue;
		
		// strip 'cr' 'lf' ' '
		while(isspace(line[strlen(line)-1]))
       		line[strlen(line)-1]=0;
		// strip 'cr' 'lf' ' '
		while(isspace(line[0]))
			strcpy(line,line+1);
		
		// comment out
		char *cmnt=strstr(line,"//");
		if(cmnt)
			*cmnt=0;
		
		if(strlen(line)==0)
			continue;
		
		// printf("analyzing line: '%s'\n",line);
		
		if(*p)
			argc_=1;
		while(*p)
		{
			if(isspace(*p))
			{
				argc_++;
				while(*p && isspace(*p))
					p++;
			}
			else
			{
				if(*p=='"')
				{
					p++;
					while(*p && *p!='"')
						p++;
				}
				else
					if(*p=='\'')
					{
						p++;
						while(*p && *p!='\'')
							p++;
					}
				if(*p)
					p++;   	   		 
			}
		}
		// printf("Pass1 done: %d\n",argc_);
		argv_=(char **)malloc(argc_*sizeof(char *));
		if(argv_)
		{
			p=line;
			int cnt=0;
			argv_[cnt]=p;
			
			while(*p)
			{
				if(isspace(*p))
				{
					char *sp=p;
					while(*p && isspace(*p))
						p++;
					if(*p)
					{
						argv_[++cnt]=p;
						*sp=0;
					}
				}
				else
				{
					if(*p=='"')
					{
						p++;
						while(*p && *p!='"')
							p++;
					}
					else
						if(*p=='\'')
						{
							p++;
							while(*p && *p!='\'')
								p++;
						}
					if(*p)
						p++;   	   		 
				}
				
			}
		} else { fprintf(stderr,"Not enough memory\n"); break; }
		
		for(int i=0;i<argc_;i++)
		{
       		p=argv_[i];
       		while(p && *p)
       		{
       			char oldp=*p;
       			if(*p=='\'' || *p=='"')
       			{
       				strcpy(p,p+1);
       				p=strchr(p,oldp);
					if(p!=NULL)
						strcpy(p,p+1);
       			}
       			else
					p++;
       		}
			//         	printf("[%d] '%s'\n",i,argv_[i]);
		}
		command(argc_,argv_);
		
		free(argv_);
	}
	return 0;
}

int parse_file(char *file_name,int (*command)(int argc,char **argv));
int parse_file(char *file_name,int (*command)(int argc,char **argv))
{
	FILE *f;
	f=fopen(file_name,"rt");
	if(f!=NULL)
	{
		fseek(f,0L,SEEK_END);
		size_t fsize=ftell(f);
		fseek(f,0L,SEEK_SET);
		char *mem=(char *)malloc(fsize);
		if(mem==NULL)
		{
			fclose(f);
			fprintf(stderr,"Not enough memory to load file (size is %ld)\n",fsize);
			return -2;
		}
		fsize=fread(mem,1,fsize,f);
		fclose(f);
		parse_text(mem,(int)fsize,command);
		free(mem);
	} else { perror("Error opening directive file"); }
	return 0;
}
