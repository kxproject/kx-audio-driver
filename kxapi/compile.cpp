// kX Driver Interface
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


#include "stdafx.h"

#ifdef _MSC_VER
#pragma warning(disable:4242)
#pragma warning(disable:4244)
#endif

#if defined(__APPLE__)
	#warning SoundFont compile functions not implemented
#elif defined(WIN32)

static CList<sfSample,sfSample> samples;
static CList<sfPresetHeader,sfPresetHeader> preset;
static CList<sfModGenBag,sfModGenBag> preset_bag;
static CList<sfGenList,sfGenList> pgenlist;
static CList<sfModList,sfModList> pmodlist;
static CList<sfInst,sfInst> inst;
static CList<sfModGenBag,sfModGenBag> inst_bag;
static CList<sfGenList,sfGenList> igenlist;
static CList<sfModList,sfModList> imodlist;

typedef struct
{
	int num;
	char name[256];
}unresolved;
static CList<unresolved,unresolved> unresolved_list;

static sfHeader header;
static signed short *sample_data=NULL;

#define get_int(param,var)	\
		if(strncmp(str,param,strlen(param))==NULL) \
		{ sscanf(str+strlen(param),"%d",&tmp); var=tmp; }

#define get_name(param,var) \
		if(strncmp(str,param,strlen(param))==NULL)	\
			strncpy(var,str+strlen(param),sizeof(var));

#define get_string()	\
		fgets(str,256,f);	\
		if(feof(f))	\
			break;	\
		while(str[strlen(str)-1]=='\n' ||	\
			str[strlen(str)-1]=='\r')	\
			str[strlen(str)-1]=0;	\
		if(strchr(str,';'))	\
			*(strchr(str,';'))=0;	\
		while(str[strlen(str)-1]==' ' ||	\
			str[strlen(str)-1]=='\x9')	\
			str[strlen(str)-1]=0;	\
		line++;

#define begin_search(str)	\
	memset(&fd,0,sizeof(fd));	\
	h=_findfirst(str,&fd);		\
	if(h)						\
	{							\
		do						\
		{						\
			if((strcmp(fd.name,".")!=NULL)&&(strcmp(fd.name,"..")!=NULL) \
				&&(!(fd.attrib&_A_SUBDIR)))	\
			{	

#define end_search()	\
				} \
		}while(!_findnext(h,&fd));\
		_findclose(h);\
	}


int find_sample_by_name(char *n)
{
	int i=0;
	POSITION p=samples.GetHeadPosition();
	while(p)
	{
		if(p==NULL)
			return -1;
		sfSample sf=samples.GetAt(p);
		if(strcmp(sf.name,n)==NULL)
			return i;
		samples.GetNext(p);
		i++;
	}
	return -1;
}

int find_inst_by_name(char *n)
{
	int i=0;
	POSITION p=inst.GetHeadPosition();
	while(p)
	{
		if(p==NULL)
			return -1;
		sfInst i_=inst.GetAt(p);
		if(strcmp(i_.name,n)==NULL)
			return i;
		inst.GetNext(p);
		i++;
	}
	return -1;
}

static int do_upload(iKX *ikx)
{
        kx_sound_font sf;
        int ret=-1;

        memcpy(&sf.header,&header,sizeof(header));

        memset(&sf.header.sfman_file_name[0],0,sizeof(sf.header.sfman_file_name));

        size_t final_size=0;
        sf.presets=0;
 	final_size+=preset.GetCount()*sizeof(sfPresetHeader);
 	sf.preset_bags=(sfModGenBag *)final_size;
 	final_size+=preset_bag.GetCount()*sizeof(sfModGenBag);
 	sf.pmodlists=(sfModList *)final_size;
 	final_size+=pmodlist.GetCount()*sizeof(sfModList);
 	sf.pgenlists=(sfGenList *)final_size;
 	final_size+=pgenlist.GetCount()*sizeof(sfGenList);
 	sf.insts=(sfInst *)final_size;
 	final_size+=inst.GetCount()*sizeof(sfInst);
 	sf.inst_bags=(sfModGenBag *)final_size;
 	final_size+=inst_bag.GetCount()*sizeof(sfModGenBag);
 	sf.imodlists=(sfModList *)final_size;
 	final_size+=imodlist.GetCount()*sizeof(sfModList);
 	sf.igenlists=(sfGenList *)final_size;
 	final_size+=igenlist.GetCount()*sizeof(sfGenList);
 	sf.samples=(sfSample *)final_size;
 	final_size+=samples.GetCount()*sizeof(sfSample);
        sf.sample_data=(short *)final_size;

 	final_size+=sf.header.sample_len;

 	final_size+=4+sizeof(sf);
 	sf.size=(word)final_size;
       	sf.id=0;

 	char *mem=(char *)realloc(sample_data,final_size);
 	if(mem)
 	{
 		// reverse memcpy
 		sample_data=NULL;
 		char *p1,*p2;
 		p1=mem+final_size-4;
 		p2=mem+sf.header.sample_len-1;

 		for(int a=0;a<sf.header.sample_len;a++)
 		{
 			*p1=*p2;
 			p1--;
 			p2--; 			
 		}

 		memcpy(mem,&sf,sizeof(sf));
 		char *ptr=(char *)(mem+sizeof(sf)-1);

        	POSITION pos;

        	#define add(a,b)			\
        	pos=a.GetHeadPosition();		\
        	while(pos)				\
        	{					\
        		b p;				\
        		p=a.GetAt(pos);			\
        		memcpy(ptr,&p,sizeof(p));	\
        		ptr+=sizeof(p);			\
        		a.GetNext(pos);			\
        	}

        	add(preset,sfPresetHeader);
                add(preset_bag,sfModGenBag);
                add(pmodlist,sfModList);
                add(pgenlist,sfGenList);
                add(inst,sfInst);
                add(inst_bag,sfModGenBag);
                add(imodlist,sfModList);
                add(igenlist,sfGenList);
                add(samples,sfSample);

                ptr+=sf.header.sample_len;

                *(dword *)ptr=KX_SOUNDFONT_MAGIC;
                ret=ikx->load_soundfont((kx_sound_font *)mem);
		free(mem);
      	} else debug("sf_compile: Not enough memory (%d required)\n",final_size);
      	return ret;
}

// if file_name is NULL - upload data to driver
int iKX::compile_soundfont(char *dir,char *file_name,dword sfman_id,dword subsynth_)
{
	int ret=0;
	FILE *f=NULL;
	FILE *fo=NULL;
	_finddata_t fd;	
	int h=0;
	char str[256];
	int line=0;

	if(file_name)
	{
	 fo=fopen(file_name,"wb");
	 if(!fo)
	 {
		debug("sf_compile: Error opening output file %s\n",file_name);
		ret=-4;
		goto ERROR_FAILED;
	 }
	}
	
	_chdir(dir);

	// read general info
	f=fopen("info","rt");
	if(f==NULL)
	{
		debug("sf_compile: No INFO file\n");
		ret=-10;
		goto ERROR_FAILED;
	}

	memset(&header,0,sizeof(header));
	line=0;
	while(1)
	{
		int tmp;

		get_string();
		
		get_int("Version: ",header.ver.major);
		get_int("VersionLow: ",header.ver.minor);
		get_int("RomVersion: ",header.rom_ver.major);
		get_int("RomVersionLow: ",header.rom_ver.minor);

		get_name("Name: ",header.name);
		get_name("Engineer: ",header.engineer);
		get_name("Engine: ",header.engine);
		get_name("Product: ",header.product);
		get_name("Copyright: ",header.copyright);
		get_name("Card: ",header.card);
		get_name("ROM Name: ",header.rom_name);
		get_name("Comments: ",header.comments);
		get_name("Creator: ",header.creator);
	}
	fclose(f);
	f=NULL;

	// read all sample data
	debug("sf_compile: Reading samples...\n");
	_chdir("samples");

	int cnt;
	cnt=0;
	int cumulative_len;
	cumulative_len=0;

	// Find out total length of samples (to avoid slow realloc.. :( )
	begin_search("*.*");
				if(fd.name[strlen(fd.name)-1]=='i' &&
					fd.name[strlen(fd.name)-2]=='.')
					goto NEXT_ITEM_1;
				cumulative_len+=(fd.size+46);
				
NEXT_ITEM_1:
				;
	end_search();

	sample_data=(signed short *)malloc(cumulative_len);
	if(sample_data==NULL)
	{
		debug("\nsf_compile: No more memory (now %d) [%d added]\n",header.sample_len,fd.size);
		ret=-1;
		goto ERROR_FAILED;
	}

	begin_search("*.*");

				if(fd.name[strlen(fd.name)-1]=='i' &&
					fd.name[strlen(fd.name)-2]=='.')
					goto NEXT_ITEM;

				sfSample sa; memset(&sa,0,sizeof(sa));

				if(!fd.size || fd.size%2)
				{
					debug("\nsf_compile: Sample is 0 bytes long or is uneven\n");
					ret=-3;
					goto ERROR_FAILED;
				}

				//debug("[%d] %s                      \r",++cnt,fd.name);
				f=fopen(fd.name,"rb");
				if(!f)
				{
					debug("\nsf_compile: Error accessing file %s\n",fd.name);
					ret=-2;
					goto ERROR_FAILED;
				}
				fread(&sample_data[header.sample_len/2],1,fd.size,f);
				fclose(f); f=NULL;

				sa.start=header.sample_len/2;
				sa.end=header.sample_len/2+fd.size/2;

				header.sample_len+=fd.size;
				memset(&sample_data[header.sample_len/2],0,46);
				header.sample_len+=46;

				strcat(fd.name,".i");
				f=fopen(fd.name,"rt");
				if(!f)
				{
					debug("\nsf_compile: Sample info file '%s' not found\n",fd.name);
					ret=-5;
					goto ERROR_FAILED;
				}

				// parse sample's ".i" file
				line=0;
				while(1)
				{
					int tmp;
					get_string();

					get_name("Name=",sa.name);

					get_int("StartLoop=",sa.start_loop);
					get_int("EndLoop=",sa.end_loop);
					get_int("SampleRate=",sa.sample_rate);
					get_int("OriginalKey=",sa.original_key);
					get_int("Correction=",sa.correction);
					get_int("SampleType=",sa.sample_type);

					if(strncmp(str,"SampleLink=",11)==NULL)
					{
						char tmp_s[256];
						get_name("SampleLink=",tmp_s);
						
						if(strcmp(tmp_s,"NULL")==NULL) // no link
							sa.sample_link=0;
						else
						{
							if(strcmp(tmp_s,sa.name)==NULL) 
								// link to same sample
							{
								sa.sample_link=samples.GetCount();
							}
							else
							{
								sa.sample_link=65535; // our flag
								int i=find_sample_by_name(tmp_s);
								if(i!=-1)
									sa.sample_link=i;
								else
								{
									unresolved u;
									u.num=samples.GetCount();
									strcpy(u.name,tmp_s);
									unresolved_list.AddTail(u);
								}
							}
						}
					}
				} // file parsed (while(1))

				sa.start_loop+=sa.start;
				sa.end_loop+=sa.start;
				fclose(f);

				samples.AddTail(sa);

NEXT_ITEM:
				;

	end_search();

	debug("\nsf_compile: Loading instruments....\n");
	cnt=0;

	// process instruments
	_chdir("..");
	_chdir("inst");
	begin_search("*.*");

		f=fopen(fd.name,"rt");
		if(f==NULL)
		{
			debug("\nsf_compile: Error accessing file %s\n",fd.name);
			ret=-11;
			goto ERROR_FAILED;
		}
		line=0;

		//debug("[%d] %s                      \r",++cnt,fd.name);

		while(1)
		{
			// create new item in sfInst
			if(line==0)
			{
				sfInst i; memset(&i,0,sizeof(i));

				get_string();
				get_name("Name=",i.name);
				if(i.name[0]==0)
				{
					debug("\nsf_compile: Bad instrument descr w/o 'Name='\n");
					ret=-16;
					goto ERROR_FAILED;
				}

				i.inst_bag_ndx=inst_bag.GetCount();
				inst.AddTail(i);
			}

			get_string();
			sfGenList gen_list;
			int m=0; dword n=0;

			char *p;
			p=strchr(str,'=');

			if(sscanf(str,"%d",&m)!=1 || p==NULL)
			{
				debug("\nsf_compile: Instrument file structure is invalid (line %d)\n",line);
				ret=-13;
				goto ERROR_FAILED;
			}
			gen_list.gen_oper=m;
			if(m!=41 && m!=53)
			{
				if(sscanf(p+1,"%d",&n)!=1)
				{
					debug("\nsf_compile: Parameter in instrument file is invalid (line %d)\n",line);
					ret=-14;
					goto ERROR_FAILED;
				}
				gen_list.gen_amount.amount_w=n;
			}
			else
			{
				m=find_sample_by_name(p+1);
				if(m==-1)
				{
					debug("\nsf_compile: Reference to unknown sample '%s' [n=%d]\n",p+1,n);
					ret=-15;
					goto ERROR_FAILED;
				}
				gen_list.gen_amount.amount=m;
			}
			if(n==2147483647 && m==2147483647) // new gen_ list
			{
				sfModGenBag mod_gen;
				mod_gen.gen_ndx=igenlist.GetCount();
				mod_gen.mod_ndx=imodlist.GetCount();
				inst_bag.AddTail(mod_gen);
				continue;
			}
			if(m==2147483647 && n==2147483646) // new mod_ list
			{
				// ignore
				continue;
			}
			igenlist.AddTail(gen_list);
		}
		fclose(f);
		f=NULL;

	end_search();
	
	debug("\nsf_compile: Loading Presets....\n");
	cnt=0;
	// process presets
	_chdir("..");
	_chdir("presets");
	begin_search("*.*");

		f=fopen(fd.name,"rt");
		if(f==NULL)
		{
			debug("\nsf_compile: Error accessing preset file %s\n",fd.name);
			ret=-20;
			goto ERROR_FAILED;
		}
		line=0;
		//debug("[%d] %s                      \r",++cnt,fd.name);

		// create new item in sfPreset
		sfPresetHeader p; memset(&p,0,sizeof(p));

		while(1) // for get_string() to work
		{
			int tmp;

			p.bank=0xffff;
			p.preset=0xffff;

			get_string();
			get_name("Name=",p.name);
			get_string();
			get_int("bank=",p.bank);
			get_string();
			get_int("preset=",p.preset);
			get_string();
			get_int("lib=",p.library);
			get_string();
			get_int("genre=",p.genre);
			get_string();
			get_int("morphology=",p.morphology);
			if(p.name[0]==0 || p.bank==0xffff || p.preset==0xffff)
			{
				debug("\nsf_compile: Bad preset descr\n");
				ret=-21;
				goto ERROR_FAILED;
			}

			p.preset_bag_ndx=preset_bag.GetCount();
			preset.AddTail(p);

			break;
		}
		if(feof(f))
		{
			debug("\nsf_compile: Empty preset '%s'[%s] @%d.%d\n",p.name,fd.name,p.bank,p.preset);
		}
		else
		while(1)
		{
			get_string();
			sfGenList gen_list;
			int m; dword n=0;

			char *p;
			p=strchr(str,'=');

			if(sscanf(str,"%d",&m)!=1 || p==NULL)
			{
				debug("\nsf_compile: Preset file structure is invalid (line %d)\n",line);
				ret=-23;
				goto ERROR_FAILED;
			}
			gen_list.gen_oper=m;
			if(m!=41 && m!=53)
			{
				if(sscanf(p+1,"%d",&n)!=1)
				{
					debug("\nsf_compile: Parameter in preset file is invalid (line %d)\n",line);
					ret=-24;
					goto ERROR_FAILED;
				}
				gen_list.gen_amount.amount_w=n;
			}
			else
			{
				m=find_inst_by_name(p+1);
				if(m==-1)
				{
					debug("\nsf_compile: Reference to unknown instrument '%s' [n=%d]\n",p+1,m);
					ret=-15;
					goto ERROR_FAILED;
				}
				gen_list.gen_amount.amount=m;
			}
			if(n==2147483647 && m==2147483647) // new gen_ list
			{
				sfModGenBag mod_gen;
				mod_gen.gen_ndx=pgenlist.GetCount();
				mod_gen.mod_ndx=pmodlist.GetCount();
				preset_bag.AddTail(mod_gen);
				continue;
			}
			if(m==2147483647 && n==2147483646) // new mod_ list
			{
				// ignore
				continue;
			}
			pgenlist.AddTail(gen_list);
		}
		fclose(f);
		f=NULL;

	end_search();
	_chdir("..");	

	debug("\nsf_compile: Processing loaded data...\nSearch for links...\n");
	// Now, process output
	if(ret)
		goto ERROR_FAILED;

	// Process sfSamples
	int i;
	for(i=0;i<samples.GetCount();i++)
	{
		POSITION pos;
		pos=samples.FindIndex(i);
		sfSample &r=samples.GetAt(pos);

		if(r.sample_link==65535)
		{
			// find, what string it was
			int j;
			for(j=0;j<unresolved_list.GetCount();j++)
			{
				POSITION pos2;
				pos2=unresolved_list.FindIndex(j);

				unresolved u=unresolved_list.GetAt(pos2);
				if(u.num==i)
				{
					// here was u.name
					int num2=find_sample_by_name(u.name);
					if(num2==-1)
					{
						debug("\nsf_compile: Reference to unknown sample_link [%s]\n",u.name);
						ret=-7;
						goto ERROR_FAILED;
					}
					r.sample_link=num2;
				}
			}
		}

		if(r.sample_link==65535)
		{
			debug("\nsf_compile: sample link was never resolved\n");
			ret=-8;
			goto ERROR_FAILED;
		}
	}

	debug("sf_compile: Creating final records...\n");

	// last sample
	sfSample sa_term;
	memset(&sa_term,0,sizeof(sa_term));
	strcpy(sa_term.name,"EOS");
	samples.AddTail(sa_term);

	// last instrument
	sfInst i_term;
	memset(&i_term,0,sizeof(i_term));
	strcpy(i_term.name,"EOI");
	i_term.inst_bag_ndx=inst_bag.GetCount();
	inst.AddTail(i_term);

	// last preset
	sfPresetHeader p_term;
	memset(&p_term,0,sizeof(p_term));
	strcpy(p_term.name,"EOP");
	p_term.preset_bag_ndx=preset_bag.GetCount();
	preset.AddTail(p_term);

	// final inst_bag
	sfModGenBag i_bag;
	i_bag.gen_ndx=igenlist.GetCount();
	i_bag.mod_ndx=imodlist.GetCount();
	inst_bag.AddTail(i_bag);

	// final preset_bag
	sfModGenBag p_bag;
	p_bag.gen_ndx=pgenlist.GetCount();
	p_bag.mod_ndx=(word)pmodlist.GetCount();
	preset_bag.AddTail(p_bag);

	// final mod lists (i&p)
	sfModList m_l;
	memset(&m_l,0,sizeof(m_l));
	imodlist.AddTail(m_l);
	pmodlist.AddTail(m_l);

	// final gen lists (i&p)
	sfGenList g_l;
	memset(&g_l,0,sizeof(g_l));
	igenlist.AddTail(g_l);
	pgenlist.AddTail(g_l);

	// write final file :) uphhhh
	header.igenlists=igenlist.GetCount();
	header.imodlists=imodlist.GetCount();
	header.inst_bags=inst_bag.GetCount();
	header.insts=inst.GetCount();
	header.pgenlists=pgenlist.GetCount();
	header.pmodlists=pmodlist.GetCount();
	header.preset_bags=preset_bag.GetCount();
	header.presets=preset.GetCount();
	header.samples=samples.GetCount();

	strcpy(header.creator,"SFont Parser/Compiler");

	size_t info_size;
	info_size=0;
	size_t final_size;
	final_size=0;

#define get_info_len(a) if(a[0]) { info_size+=(8+strlen(a)+1); }

	info_size=4; /* INFO */
	get_info_len(header.card)
	get_info_len(header.comments)
	get_info_len(header.copyright)
	get_info_len(header.creator)
	get_info_len(header.engine)
	get_info_len(header.engineer)
	get_info_len(header.name)
	get_info_len(header.rom_name)
	get_info_len(header.product)

	info_size+=12; // ver
	if(header.rom_ver.major)
		info_size+=12;

	final_size+=4 /* sfbk */ + 4 /* list */ + 4 /* size */;

	final_size+=4 /* info */ + 4 /* list */ + 4 /* info_size */;
	final_size+=info_size;

	final_size+=4 /* sdta */ + 4 /* list */ + 4 /* sample_size */;
	final_size+=header.sample_len;

	final_size+=4 /* pdta */ + 4 /* list */ + 4 /* pdta_size */;
	final_size+=9*8; // all the chunks

	final_size+=samples.GetCount()*sizeof(sfSample);
	final_size+=preset.GetCount()*sizeof(sfPresetHeader);
	final_size+=preset_bag.GetCount()*sizeof(sfModGenBag);
	final_size+=pgenlist.GetCount()*sizeof(sfGenList);
	final_size+=pmodlist.GetCount()*sizeof(sfModList);
	final_size+=inst.GetCount()*sizeof(sfInst);
	final_size+=inst_bag.GetCount()*sizeof(sfModGenBag);
	final_size+=igenlist.GetCount()*sizeof(sfGenList);
	final_size+=imodlist.GetCount()*sizeof(sfModList);

        // time to write data / upload soundfont...
        if(fo==NULL)
        {
         debug("sf_compile: Uploading...\n");
         header.sfman_id=sfman_id;
         header.subsynth=subsynth_;
         ret=do_upload(this);
         goto END;
        }

       	debug("sf_compile: Saving results...\n");
	f=fo;
	// write samples
	fprintf(f,"RIFF");
	fwrite(&final_size,1,4,f);

	fprintf(f,"sfbkLIST");
	dword tmp2;
	tmp2=(dword)info_size;
	fwrite(&tmp2,1,4,f);

	fprintf(f,"INFO");

#define write_info(a,b)	\
	if(a[0]) { \
	tmp2=b; fwrite(&tmp2,1,4,f); \
	tmp2=(dword)strlen(a)+1;	\
	fwrite(&tmp2,1,4,f); \
	fwrite(&a[0],1,strlen(a)+1,f); }

	fprintf(f,"ifil");
	tmp2=4;
	fwrite(&tmp2,1,4,f);
	fwrite(&header.ver,1,4,f);

	write_info(header.engine,'gnsi');
	write_info(header.name,'MANI');
	write_info(header.creator,'TFSI');
	write_info(header.rom_name,'mori');
	write_info(header.card,'DRCI');
	write_info(header.comments,'TMCI');
	write_info(header.copyright,'POCI');
	write_info(header.engineer,'GNEI');
	write_info(header.product,'DRPI');

	if(header.rom_ver.major)
	{
		fprintf(f,"iver");
		tmp2=4;
		fwrite(&tmp2,1,4,f);
		fwrite(&header.rom_ver,1,4,f);
	}

	fprintf(f,"LIST");
	// size
	tmp2=header.sample_len+4 /*sdta*/ +4 /*smpl*/ +4 /* size */;
	fwrite(&tmp2,1,4,f);

	fprintf(f,"sdtasmpl");
	fwrite(&header.sample_len,1,4,f);
	
	fwrite(sample_data,header.sample_len,1,f);

	fprintf(f,"LIST");
	tmp2=(dword)(samples.GetCount()*sizeof(sfSample)
		+preset.GetCount()*sizeof(sfPresetHeader)
		+preset_bag.GetCount()*sizeof(sfModGenBag)
		+pgenlist.GetCount()*sizeof(sfGenList)
		+pmodlist.GetCount()*sizeof(sfModList)
		+inst.GetCount()*sizeof(sfInst)
		+inst_bag.GetCount()*sizeof(sfModGenBag)
		+igenlist.GetCount()*sizeof(sfGenList)
		+imodlist.GetCount()*sizeof(sfModList)
		+8*9 /*'id+len'*/ + 4 /* len of 'size' field */);
	fwrite(&tmp2,1,4,f);
	fprintf(f,"pdta");
	
	tmp2='rdhp';
	fwrite(&tmp2,1,4,f);
	tmp2=(dword)(preset.GetCount()*sizeof(sfPresetHeader));
	fwrite(&tmp2,1,4,f);
	POSITION pos;
	pos=preset.GetHeadPosition();
	while(pos)
	{
		sfPresetHeader p;
		p=preset.GetAt(pos);
		fwrite(&p,1,sizeof(p),f);
		preset.GetNext(pos);
	}

	tmp2='gabp';
	fwrite(&tmp2,1,4,f);
	tmp2=(dword)(preset_bag.GetCount()*sizeof(sfModGenBag));
	fwrite(&tmp2,1,4,f);
	pos=preset_bag.GetHeadPosition();
	while(pos)
	{
		sfModGenBag p;
		p=preset_bag.GetAt(pos);
		fwrite(&p,1,sizeof(p),f);
		preset_bag.GetNext(pos);
	}

	tmp2='domp';
	fwrite(&tmp2,1,4,f);
	tmp2=(dword)(pmodlist.GetCount()*sizeof(sfModList));
	fwrite(&tmp2,1,4,f);
	pos=pmodlist.GetHeadPosition();
	while(pos)
	{
		sfModList p;
		p=pmodlist.GetAt(pos);
		fwrite(&p,1,sizeof(p),f);
		pmodlist.GetNext(pos);
	}

	tmp2='negp';
	fwrite(&tmp2,1,4,f);
	tmp2=(dword)(pgenlist.GetCount()*sizeof(sfGenList));
	fwrite(&tmp2,1,4,f);
	pos=pgenlist.GetHeadPosition();
	while(pos)
	{
		sfGenList p;
		p=pgenlist.GetAt(pos);
		fwrite(&p,1,sizeof(p),f);
		pgenlist.GetNext(pos);
	}

	tmp2='tsni';
	fwrite(&tmp2,1,4,f);
	tmp2=(dword)(inst.GetCount()*sizeof(sfInst));
	fwrite(&tmp2,1,4,f);
	pos=inst.GetHeadPosition();
	while(pos)		
	{
		sfInst p;
		p=inst.GetAt(pos);
		fwrite(&p,1,sizeof(p),f);
		inst.GetNext(pos);
	}

	tmp2='gabi';
	fwrite(&tmp2,1,4,f);
	tmp2=(dword)(inst_bag.GetCount()*sizeof(sfModGenBag));
	fwrite(&tmp2,1,4,f);
	pos=inst_bag.GetHeadPosition();
	while(pos)	
	{
		sfModGenBag p;
		p=inst_bag.GetAt(pos);
		fwrite(&p,1,sizeof(p),f);
		inst_bag.GetNext(pos);
	}

	tmp2='domi';
	fwrite(&tmp2,1,4,f);
	tmp2=(dword)(imodlist.GetCount()*sizeof(sfModList));
	fwrite(&tmp2,1,4,f);
	pos=imodlist.GetHeadPosition();
	while(pos)
	{
		sfModList p;
		p=imodlist.GetAt(pos);
		fwrite(&p,1,sizeof(p),f);
		imodlist.GetNext(pos);
	}

	tmp2='negi';
	fwrite(&tmp2,1,4,f);
	tmp2=(dword)(igenlist.GetCount()*sizeof(sfGenList));
	fwrite(&tmp2,1,4,f);
	pos=igenlist.GetHeadPosition();
	while(pos)
	{
		sfGenList p;
		p=igenlist.GetAt(pos);
		fwrite(&p,1,sizeof(p),f);
		igenlist.GetNext(pos);
	}

	tmp2='rdhs';
	fwrite(&tmp2,1,4,f);
	tmp2=(dword)(samples.GetCount()*sizeof(sfSample));
	fwrite(&tmp2,1,4,f);
	pos=samples.GetHeadPosition();
	while(pos)
	{
		sfSample p;
		p=samples.GetAt(pos);
		fwrite(&p,1,sizeof(p),f);
		samples.GetNext(pos);
	}


	fclose(f);
ERROR_FAILED:
END:

/*
FIXME: clear all lists
static CList<sfSample,sfSample> samples;
static CList<sfPresetHeader,sfPresetHeader> preset;
static CList<sfModGenBag,sfModGenBag> preset_bag;
static CList<sfGenList,sfGenList> pgenlist;
static CList<sfModList,sfModList> pmodlist;
static CList<sfInst,sfInst> inst;
static CList<sfModGenBag,sfModGenBag> inst_bag;
static CList<sfGenList,sfGenList> igenlist;
static CList<sfModList,sfModList> imodlist;
static CList<unresolved,unresolved> unresolved_list;
*/
	memset(&header,0,sizeof(header));
	if(sample_data)
	{
	 free(sample_data);
	 sample_data=NULL;
	}

	if(f)
		fclose(f);

	if(ret>=0)
	{
		debug("sf_compile: Compiling succeded\n");
	}
	return ret;
}

#endif // WIN32
