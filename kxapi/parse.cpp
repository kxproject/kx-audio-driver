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

#if defined(WIN32)

#include "sfman/sfdevman.h"
#include "sfman/sfdevdta.h"
#include "sfman/sfman.h"

#if defined(WIN32)	   
	#define mkdir(a,b)        CreateDirectory(a,NULL);
	#define chdir(a)          _chdir(a)
#endif

static sfPresetHeader *preset=0;
static sfModGenBag *preset_bag=0;
static sfModList *pmodlist=0;
static sfGenList *pgenlist=0;

static sfInst *inst=0;
static sfModGenBag *inst_bag=0;
static sfModList *imodlist=0;
static sfGenList *igenlist=0;

static sfSample *sample=0;

static sfHeader header;
static signed short *sample_data_w=0;

static long sf_pos=0;

typedef struct sample_list_t_s
	{
		struct sample_list_t_s *next,*prev;
		CViSmplObject *sample;
	}sample_list_t;

static int do_upload(iKX *ikx,dword sfman_id,const char *sf_fname,const char *actual_fname)
{
	kx_sound_font sf;
	int ret=-1;
	
	memcpy(&sf.header,&header,sizeof(header));
	
	sf.header.sfman_id=sfman_id;
	strncpy(sf.header.sfman_file_name,sf_fname,sizeof(sf.header.sfman_file_name));
	
	size_t final_size=0;
	sf.presets=0;
 	final_size+=header.presets*sizeof(sfPresetHeader);
 	sf.preset_bags=(sfModGenBag *)final_size;
 	final_size+=header.preset_bags*sizeof(sfModGenBag);
 	sf.pmodlists=(sfModList *)final_size;
 	final_size+=header.pmodlists*sizeof(sfModList);
 	sf.pgenlists=(sfGenList *)final_size;
 	final_size+=header.pgenlists*sizeof(sfGenList);
 	sf.insts=(sfInst *)final_size;
 	final_size+=header.insts*sizeof(sfInst);
 	sf.inst_bags=(sfModGenBag *)final_size;
 	final_size+=header.inst_bags*sizeof(sfModGenBag);
 	sf.imodlists=(sfModList *)final_size;
 	final_size+=header.imodlists*sizeof(sfModList);
 	sf.igenlists=(sfGenList *)final_size;
 	final_size+=header.igenlists*sizeof(sfGenList);
 	sf.samples=(sfSample *)final_size;
 	final_size+=header.samples*sizeof(sfSample);
	sf.sample_data=(short *)final_size;
	
 	final_size+=sf.header.sample_len;
	
 	final_size+=4+sizeof(sf);
 	sf.size=(dword)final_size;
	sf.id=0;
	
 	char *mem=0;
 	int method;
 	
 	if(sample_data_w)
 	{
		mem=(char *)realloc(sample_data_w,final_size);
		sample_data_w=NULL;
		method=0; // legacy
 	}
 	else
 	{
		mem=(char *)malloc(final_size-sf.header.sample_len);
		method=1;
 	}
	
 	if(mem)
 	{
 		// reverse memcpy
 		if(method==0)
 		{
			char *p1,*p2;
			p1=(char *)((uintptr_t)mem+final_size-4U-sf.header.sample_len-1);
			p2=mem;
			
			for(int a=sf.header.sample_len-1;a>=0;a--)
			{
				p1[a]=p2[a];
				p2[a]=0;
			}
 		}
		
 		memcpy(mem,&sf,sizeof(sf));
 		char *ptr=(char *)(mem+sizeof(sf)-1);
		
#define add(a,b,c) memcpy(ptr,a,sizeof(b)*c); ptr+=sizeof(b)*c;
		add(preset,sfPresetHeader,header.presets);
		add(preset_bag,sfModGenBag,header.preset_bags);
		add(pmodlist,sfModList,header.pmodlists);
		add(pgenlist,sfGenList,header.pgenlists);
		add(inst,sfInst,header.insts);
		add(inst_bag,sfModGenBag,header.inst_bags);
		add(imodlist,sfModList,header.imodlists);
		add(igenlist,sfGenList,header.igenlists);
		add(sample,sfSample,header.samples);
		
		if(method==0)
		{
			ptr+=sf.header.sample_len;
		}
		else
		{
			// ptr?..
		}
		
		*(dword *)ptr=KX_SOUNDFONT_MAGIC;
		if(method==0)
			ret=ikx->load_soundfont((kx_sound_font *)mem);
		else
			ret=ikx->load_soundfont_x((kx_sound_font *)mem,actual_fname,sf_pos);
		free(mem);
	} else { debug("sf_parse: Not enough memory (%d required)\n",final_size); }
	return ret;
}


void parse_name(char *in,char *out)
{
	while(*in)
	{
		switch(*in)
		{
			case ':':
			case '/':
				*out='_';
				break;
			default:
				*out=*in;
		}
		in++;
		out++;
	}
}

static void print_modulator(FILE *,sfModList *)
{
}

static void print_generator(FILE *f,sfGenList *gen)
{
	if(gen->gen_oper!=53 && gen->gen_oper!=41) // inst/sample
	{
		fprintf(f,"%d=%d",gen->gen_oper,gen->gen_amount.amount_w);
	}
	else
	{
		if(gen->gen_oper==53) // sample
			fprintf(f,"53=%s",sample[gen->gen_amount.amount_w].name);
		else
			fprintf(f,"41=%s",inst[gen->gen_amount.amount_w].name);
	}
	fprintf(f," ;");
	switch(gen->gen_oper)
	{
		case 0: fprintf(f,"startOffsetAddr"); break;
		case 1: fprintf(f,"endOffsetAddr"); break;
		case 2: fprintf(f,"startLoopAddrOffset"); break;
		case 3: fprintf(f,"endLoopAddrOffset"); break;
		case 4: fprintf(f,"startAddrsCoarseOffset"); break;
		case 5: fprintf(f,"modLfoToPitch"); break;
		case 6: fprintf(f,"vibLfoToPitch"); break;
		case 7: fprintf(f,"modEnvToPitch"); break;
		case 8: fprintf(f,"initialFilterFc"); break;
		case 9: fprintf(f,"initialFilterQ"); break;
		case 10: fprintf(f,"modLfoToFilterFc"); break;
		case 11: fprintf(f,"modEnvToFilterFc"); break;
		case 12: fprintf(f,"endAddrsCoarseOffset"); break;
		case 13: fprintf(f,"modLfoToVolume"); break;
		case 15: fprintf(f,"chorusEffectsSend"); break;
		case 16: fprintf(f,"reverbEffectsSend"); break;
		case 17: fprintf(f,"pan"); break;
		case 21: fprintf(f,"delayModLfo"); break;
		case 22: fprintf(f,"freqModLfo"); break;
		case 23: fprintf(f,"delayVibLfo"); break;
		case 24: fprintf(f,"freqVibLfo"); break;
		case 25: fprintf(f,"delayModEnv"); break;
		case 26: fprintf(f,"attackModEnv"); break;
		case 27: fprintf(f,"holdModEnv"); break;
		case 28: fprintf(f,"decayModEnv"); break;
		case 29: fprintf(f,"sustainModEnv"); break;
		case 30: fprintf(f,"releaseModEnv"); break;
		case 31: fprintf(f,"keynumToModEnvHold"); break;
		case 32: fprintf(f,"keynumToModEnvDecay"); break;
		case 33: fprintf(f,"delayVolEnv"); break;
		case 34: fprintf(f,"attackVolEnv"); break;
		case 35: fprintf(f,"holdVolEnv"); break;
		case 36: fprintf(f,"decayVolEnv"); break;
		case 37: fprintf(f,"sustainVolEnv"); break;
		case 38: fprintf(f,"releaseVolEnv"); break;
		case 39: fprintf(f,"keynumToVolEnvHold"); break;
		case 40: fprintf(f,"keynumToVolEnvDecay"); break;
		case 41: fprintf(f,"inst name"); break;
		case 43: fprintf(f,"key range"); break;
		case 44: fprintf(f,"vel range"); break;
		case 45: fprintf(f,"starloopAddrsCoarseOffset"); break;
		case 46: fprintf(f,"keynum"); break;
		case 47: fprintf(f,"velocity"); break;
		case 48: fprintf(f,"initialAttennuation"); break;
		case 50: fprintf(f,"endloopAddrsCoarseOffset"); break;
		case 51: fprintf(f,"coarseTune"); break;
		case 52: fprintf(f,"fineTune"); break;
		case 53: fprintf(f,"sample name"); break;
		case 54: fprintf(f,"sampleModes"); break;
		case 56: fprintf(f,"scaleTuning"); break;
		case 57: fprintf(f,"exclusiveClass"); break;
		case 58: fprintf(f,"overridingRootKey"); break;
		case 60: fprintf(f,"endList"); break;
		default: fprintf(f,"???UNKNOWN???"); break;
	}
	fprintf(f,"\n");
}

static inline int my_fread(void *mem,int p,int s,FILE *f_i,char **m)
{
	if(*m)
	{
		memcpy(mem,*m,p*s); 
		*m+=(p*s);
		return p;
	}
	else
		return (int)fread(mem,p,s,f_i);
}

static inline void my_fseek(FILE *f_i,long offset,char **m)
{
	if(*m)
		*m+=(offset);
	else
		fseek(f_i,(long)offset,SEEK_CUR);
}

static inline long my_ftell(FILE *f_i,char **m)
{
	if(*m)
		return (long)(uintptr_t)(*m);
	else
		return ftell(f_i);
}

// if dir==NULL -> upload
int iKX::parse_soundfont(char *file_name_,char *dir,dword sfman_id_,dword subsynth_)
{
	char real_file_name[MAX_PATH];
	
	int vienna=(sfman_id_&VIENNA_DYNAMIC)?1:0;
	int ret=0;
	FILE *f_i=NULL;
	FILE *fo=NULL;
	FILE *fi=NULL;
	char *m=0;
	
	strncpy(real_file_name,file_name_,MAX_PATH);
	int unlink_file=0;
	
	if(strstr(file_name_,"mem://")!=0)
	{
		sscanf(file_name_+6,"%x",(unsigned int *)&m);
		f_i=0;
	}
	else
	{
		f_i=fopen(file_name_,"rb");
		if(f_i==NULL)
		{
			// perror("Error opening file");
			ret=-1;
			goto END;
		}
		m=0;
		
		// check for file type
		dword sign=0;
		if(fread(&sign,4,1,f_i))
		{
			if((sign&0xffff)==0x4c46 || // sfArk 1.0
			   (sign==0x0) ||  // sfArk 2.0
			   strstr(real_file_name,".sfArk")!=0)
			{
				fclose(f_i);
				f_i=0;
				
				// unpack on the fly...
				try
				{
					char sfark_cwd[MAX_PATH];
					char sfark_temp[MAX_PATH];
					
					extern int sfkl_Decode(const char *InFileName, const char *ReqOutFileName);
					
					GetCurrentDirectory(MAX_PATH, sfark_cwd);
					sprintf(sfark_temp, "%s", getenv("TEMP")?getenv("TEMP"):".");
					
					chdir(sfark_temp);
					
					sprintf(sfark_temp,"%s\\kx_sfark_tmp_%d.sf2",getenv("TEMP")?getenv("TEMP"):".",rand());
					debug("sf: parse: using '%s'\n",sfark_temp);
					
					if(sfkl_Decode(file_name_,sfark_temp)==0) // success
					{
						strncpy(real_file_name,sfark_temp,MAX_PATH);
						f_i=fopen(sfark_temp,"rb");
						unlink_file=1;
					}
					else
						debug("sf: parse: sfArk: decompression failed\n");
					_chdir(sfark_cwd);
				}
				catch(...)
				{
					debug("sf: parse: sfArk: exception!\n");
				}
				
				if(f_i==0)
				{
					ret=-100;
					goto END;
				}
			} // sign == sfArk?
			else
			{
				fseek(f_i,0,SEEK_SET); // rewind
			}
			
		} else debug("sf: parse: empty file?..\n");
		
	} // 'mem://'
	
	memset(&header,0,sizeof(header));
	
	// fill-in the header
	if(vienna)
	{
		debug("sf_parse: Opened mem [%x] for Vienna\n",m);
		// sfHeader header;
		strcpy(header.engine,"kX");
		strcpy(header.card,"kX");
		strcpy(header.engineer,"Eugene Gavrilov");
		strcpy(header.copyright,"Eugene Gavrilov");
		strcpy(header.comments,"dynamic SoundFont for Vienna / SoundFont editors");
		strcpy(header.product,"kX Project");
		strcpy(header.creator,"kX API - internal");
		strcpy(header.sfman_file_name,"(dynamic)");
		
		// useless:
		//  header.sfman_id=VIENNA_DYNAMIC;
		header.ver.major=2;
		header.ver.minor=1; // 2.01
	}
	else
	{
		debug("sf_parse: Opened '%s'\n",file_name_);
	}
	
	// 'RIFF' 'size'
	dword id;
	int size;
	
	if(vienna)
	{
		if(m)
			m+=4;
	}
	else
	{
		my_fread(&id,1,4,f_i,&m);
		if(id!='FFIR')
		{
			debug("sf_parse: File is not RIFF\n");
			ret=-2;
			goto END;
		}
		my_fread(&size,1,4,f_i,&m);
		my_fread(&id,1,4,f_i,&m); // should be 'sfbk'
		if(id!='kbfs')
		{
			debug("sf_parse: RIFF file w/o sfbk section\n");
			ret=-3;
			goto END;
		}
	}
	while(1)
	{
		my_fread(&id,1,4,f_i,&m); // 'info', 'sdta', or 'pdta' LISTS
		
		if(f_i)
			if(feof(f_i))
				break;
		
		if(id!='TSIL')
		{
			if(m)
				break;
			
			debug("sf_parse: Unknown non-LIST section [%x]\n",id);
			ret=-4;
			goto END;
		}
		my_fread(&size,1,4,f_i,&m);
		my_fread(&id,1,4,f_i,&m); size-=4;
		switch(id)
		{
			case 'OFNI': // INFO
				while(size>0)
				{
					dword subsize=0;
					dword real_size=0;
					char string[2048];
					
					my_fread(&id,1,4,f_i,&m); size-=4;
					my_fread(&subsize,1,4,f_i,&m); size-=4;
					
					switch(id)
					{
						case 'lifi': // 'ifil'
							if(subsize==4)
							{
								sfVersionTag ver;
								my_fread(&ver,1,sizeof(sfVersionTag),f_i,&m); // major, minor
								subsize=real_size=0;
								header.ver=ver;
								size-=4;
							}
							else
							{
								debug("sf_parse: ifil is not 4 bytes (%d)\n",subsize);
								subsize=0;
								ret=-5;
								goto END;
							}
							break;
						case 'revi': // 'iver'
							if(subsize==4)
							{
								sfVersionTag ver;
								my_fread(&ver,1,sizeof(sfVersionTag),f_i,&m); // major, minor
								subsize=real_size=0;
								header.rom_ver=ver;
								size-=4;
							}
							else
							{
								debug("sf_parse: iver is not 4 bytes (%d)\n",subsize);
								subsize=0;
							}
							break;
						case 'gnsi': // 'isng' : ascii
							my_fread(string,1,subsize,f_i,&m);
							size-=subsize;
							real_size=subsize=0;
							strncpy(header.engine,string,sizeof(header.engine));
							break;
						case 'MANI': // 'INAM'
							my_fread(string,1,subsize,f_i,&m);
							size-=subsize;
							real_size=subsize=0;
							strncpy(header.name,string,sizeof(header.name));
							break;
						case 'mori': // 'irom'
							my_fread(string,1,subsize,f_i,&m);
							size-=subsize;
							real_size=subsize=0;
							strncpy(header.rom_name,string,sizeof(header.rom_name));
							break;
						case 'DRCI': // 'ICRD'
							my_fread(string,1,subsize,f_i,&m);
							size-=subsize;
							real_size=subsize=0;
							strncpy(header.card,string,sizeof(header.card));
							break;
						case 'GNEI': // 'IENG'
							my_fread(string,1,subsize,f_i,&m);
							size-=subsize;
							real_size=subsize=0;
							strncpy(header.engineer,string,sizeof(header.engineer));
							break;
						case 'DRPI': // 'IPRD'
							my_fread(string,1,subsize,f_i,&m);
							size-=subsize;
							real_size=subsize=0;
							strncpy(header.product,string,sizeof(header.product));
							break;
						case 'POCI': // 'ICOP'
							my_fread(string,1,subsize,f_i,&m);
							size-=subsize;
							real_size=subsize=0;
							strncpy(header.copyright,string,sizeof(header.copyright));
							break;
						case 'TMCI': // 'ICMT'
							my_fread(string,1,subsize,f_i,&m);
							size-=subsize;
							real_size=subsize=0;
							strncpy(header.comments,string,sizeof(header.comments));
							break;
						case 'TFSI': // 'ISFT'
							my_fread(string,1,subsize,f_i,&m);
							size-=subsize;
							real_size=subsize=0;
							strncpy(header.creator,string,sizeof(header.creator));
							break;
						default:
							debug("sf_parse: ! skipping unknown INFO subsection (%c%c%c%c)\n",
								  id,id>>8,id>>16,id>>24);
							break;
					}
					if(subsize!=real_size)
					{
						debug("sf_parse: ! section size doesn't match (%d;%d)\n",subsize,real_size);
					}
					if(subsize)
					{
						size-=subsize;
						if(f_i)
							fseek(f_i,subsize,SEEK_CUR);
						else
							m+=subsize;
					}
				}
				if(size<0)
					debug("sf_parse: !NB! LIST size<0 = %d\n",size);
				break;
			case 'atds': // 'sdta'
				my_fread(&id,1,4,f_i,&m); size-=4;
				if(id!='lpms') // smpl
				{
					debug("sf_parse: ! sdta section w/o smpl first\n");
					ret=-6;
					goto END;
				}
				dword ssize;
				my_fread(&ssize,1,4,f_i,&m); size-=4;
				debug("sf_parse: Sample size: %d\n",ssize);
				header.sample_len=size;
				
				sample_data_w=0;
				
				if(dir!=NULL) // parse and save
				{
					sample_data_w=(signed short *)malloc(size);
					if(!sample_data_w)
					{
						debug("sf_parse: Not enough mem for sample data[%d]\n",size);
						ret=-7;
						goto END;
					}
					my_fread(sample_data_w,1,size,f_i,&m); 
				}
				else
				{
					// save the pointer FIXME NOW !!!
					sf_pos=my_ftell(f_i,&m);
					my_fseek(f_i,size,&m);
                }
				
				size-=ssize;
				break;
			case 'atdp': // 'pdta'
				while(size>0)
				{
					dword subsize=0;
					dword real_size=0;
					
					my_fread(&id,1,4,f_i,&m); size-=4;
					my_fread(&subsize,1,4,f_i,&m); size-=4;
					void **p=NULL;
					
					switch(id)
					{
						case 'rdhp': // phdr
							real_size=sizeof(sfPresetHeader);
							p=(void **)&preset;
							header.presets=subsize/real_size;
							break;
						case 'gabp': // pbag
							real_size=sizeof(sfModGenBag);
							p=(void **)&preset_bag;
							header.preset_bags=subsize/real_size;
							break;
						case 'domp': // pmod
							real_size=sizeof(sfModList);
							p=(void **)&pmodlist;
							header.pmodlists=subsize/real_size;
							break;
						case 'negp': // pgen
							real_size=sizeof(sfGenList);
							p=(void **)&pgenlist;
							header.pgenlists=subsize/real_size;
							break;
						case 'tsni': // inst
							real_size=sizeof(sfInst);
							p=(void **)&inst;
							header.insts=subsize/real_size;
							break;
						case 'gabi': // ibag
							real_size=sizeof(sfModGenBag);
							p=(void **)&inst_bag;
							header.inst_bags=subsize/real_size;
							break;
						case 'domi': // imod
							real_size=sizeof(sfModList);
							p=(void **)&imodlist;
							header.imodlists=subsize/real_size;
							break;
						case 'negi': // igen
							real_size=sizeof(sfGenList);
							p=(void **)&igenlist;
							header.igenlists=subsize/real_size;
							break;
						case 'rdhs': // shdr
							real_size=sizeof(sfSample);
							p=(void **)&sample;
							header.samples=subsize/real_size;
							break;
						default:
							p=NULL;
							debug("sf_parse: ! skipping unknown pdta subsection (%c%c%c%c)\n",
								  id,id>>8,id>>16,id>>24);
							break;
					}
					if(subsize%real_size)
					{
						debug("sf_parse: Size mismatch\n");
						ret=-8;
						goto END;
					}
					if(p && subsize)
					{
						if(*p!=0)
						{
							debug("sf_parse: Double section found!\n");
							ret=-9;
							goto END;
						}
						*p=malloc(subsize);
						if(*p==0)
						{
							debug("sf_parse: Not enough mem (%d)\n",subsize);
							ret=-10;
							goto END;
						}
						my_fread(*p,1,subsize,f_i,&m);
						size-=subsize;
						subsize=0;
					}
					if(subsize)
					{
						debug("sf_parse: NB! subsize !=0 (%d)\n",subsize);
						size-=subsize;
						if(f_i)
							fseek(f_i,subsize,SEEK_CUR);
						else
							m+=subsize;
					}
				}
				if(size<0)
					debug("sf_parse: !NB! pdta size<0 = %d\n",size);
				
				break;
			default:
				debug("sf_parse: ! Unknown RIFF sfbk section\n");
		}
		if(f_i)
			fseek(f_i,size,SEEK_CUR);
		else
			m+=size;
	}
	
	
	if(ret || ( /*(vienna?(0):!sample_data_w) ||*/ !preset ||  !preset_bag || !pgenlist || !inst ||
			   !inst_bag || !igenlist || (vienna?(0):!sample) ) )
	{
		debug("sf_parse: File error:\n");
		if(!sample) { debug("sf_parse: no sample data\n"); ret=-11; }
		if(!sample_data_w) { debug("sf_parse: no sample_data_w data\n"); ret=-11; }
		if(!preset) { debug("sf_parse: no presets\n"); ret=-12; }
		if(!preset_bag) { debug("sf_parse: no preset_bag\n"); ret=-13; }
		if(!pgenlist) { debug("sf_parse: no pgenlist\n"); ret=-14; }
		if(!inst) { debug("sf_parse: no inst\n"); ret=-15; }
		if(!inst_bag) { debug("sf_parse: no inst_bag\n"); ret=-16; }
		if(!igenlist) { debug("sf_parse: no igenlist\n"); ret=-17; }
		goto END;
	}
	
	//debug("File read OK. parsing...\n");
	if(vienna)
	{
		// process sample, sample_data_w
		header.sample_len=0;
		
		for(int i=0;i<header.samples-1;i++)
		{
			sample_list_t *vsample=(sample_list_t *)(uintptr_t)sample[i].start;
			header.sample_len+=vsample->sample->dwSampleSize;
			header.sample_len+=46*2;
		}
		if(sample_data_w)
		{
			debug("!!! sfont: warning: sample_data_w already allocated\n");
		}
		sample_data_w=(short *)malloc(header.sample_len);
		if(sample_data_w)
		{
			memset(sample_data_w,0,header.sample_len);
			
			header.sample_len=0;
			
			for(int i=0;i<header.samples-1;i++)
			{
				// restore StartLoop [start/stloop]: EndLoop
				sample_list_t *vsample=(sample_list_t *)(uintptr_t)sample[i].start;
				sample[i].start=header.sample_len/2;
				sample[i].start_loop+=sample[i].start;
				sample[i].end_loop+=sample[i].start;
				sample[i].end+=sample[i].start;
				
				memcpy(&sample_data_w[header.sample_len/2],&vsample->sample->iSample[0],
					   vsample->sample->dwSampleSize);
				
				header.sample_len+=vsample->sample->dwSampleSize;
				header.sample_len+=46*2;
			}
			
			// re-map the instrument to 0x77, VIENNA_DYNAMIC-2
			sprintf(header.name,"Dynamic SoundFont - %s",preset[0].name);
			
			preset[0].preset=0x77;
			preset[0].bank=0x0;
		}
		else
		{
			ret=-50;
			debug("kx: sf: memory allocation error\n");
			goto END;
		}
	}
	
	if(dir==NULL)
	{
		debug("sf_parse: Uploading...\n");
		
		header.subsynth=subsynth_;
		ret=do_upload(this,(vienna)?(VIENNA_DYNAMIC-2):sfman_id_,(vienna)?"(dynamic)":file_name_,real_file_name);
		goto END;
	}
	
	int i,j;
	CreateDirectory(dir,NULL);
	_chdir(dir);
	
	// ---------------------------
	CreateDirectory("samples",NULL);
	_chdir("samples");
	
	debug("sf_parse: Parsing samples...\n");
	
	// output all the samples
	
	for(i=0;i<header.samples;i++)
	{
		sample[i].name[19]=0;
		//debug("[%d] %s                  \r",i+1,sample[i].name);
		if(i==header.samples-1)
		{
			if(strcmp(sample[i].name,"EOS")!=NULL)
			{
				debug("sf_parse: Bad samples w/o EOS\n");
				ret=-18;
				goto END;
			}
			break;
		}
		// remove spaces
		while(sample[i].name[0]==' ')
			strcpy(sample[i].name,sample[i].name+1);
		while(sample[i].name[strlen(sample[i].name)-1]==' ')
			sample[i].name[strlen(sample[i].name)-1]=0;
		
		char fname[256];
		memset(fname,0,sizeof(fname));
		parse_name(sample[i].name,fname);
		fo=fopen(fname,"wb");
		if(fo==NULL)
		{
			debug("sf_parse: Error creating file %s!\n",fname);
			ret=-19;
			goto END;
		}
		strcat(fname,".i");
		fi=fopen(fname,"wt");
		
		if(sample_data_w)
			fwrite(&sample_data_w[sample[i].start],1,
				   sample[i].end*2-sample[i].start*2,fo);
		
		fprintf(fi,"Name=%s\nStartLoop=%d [start/stloop]: %x %x]\nEndLoop=%d [%x]\n"
				"SampleRate=%d\nOriginalKey=%d\nCorrection=%d\nSampleLink=%s [%x]\n"
				"SampleType=%d\n\n",
				sample[i].name,sample[i].start_loop-sample[i].start,
				sample[i].start,sample[i].start_loop,
				sample[i].end_loop-sample[i].start,
				sample[i].end_loop,
				sample[i].sample_rate,sample[i].original_key,
				sample[i].correction,
				(sample[i].sample_link==0xffff || sample[i].sample_link==0)?"NULL":
				sample[sample[i].sample_link].name,
				sample[i].sample_link,
				sample[i].sample_type);
		
		fclose(fo); fo=NULL;
		fclose(fi); fi=NULL;
	}
	
	_chdir("..");
	
	// ---------------------------
	debug("sf_parse: Parsing Instruments...\n");
	CreateDirectory("inst",NULL);
	_chdir("inst");
	for(i=0;i<header.insts;i++)
	{
		inst[i].name[19]=0;
		//debug("[%d] %s                  \r",i+1,inst[i].name);
		if(i==header.insts-1)
		{
			if(strcmp(inst[i].name,"EOI")!=NULL)
			{
				debug("sf_parse: Bad instruments w/o EOI\n");
				ret=-20;
				goto END;
			}
			break;
		}
		// remove spaces
		while(inst[i].name[0]==' ')
			strcpy(inst[i].name,inst[i].name+1);
		while(inst[i].name[strlen(inst[i].name)-1]==' ')
			inst[i].name[strlen(inst[i].name)-1]=0;
		
		char fname[256];
		memset(fname,0,sizeof(fname));
		parse_name(inst[i].name,fname);
		fo=fopen(fname,"wb");
		if(fo==NULL)
		{
			debug("sf_parse: Error creating file %s!\n",fname);
			ret=-21;
			goto END;
		}
		if(inst[i].inst_bag_ndx>inst[i+1].inst_bag_ndx)
		{
			debug("sf_parse: Inst bag isnt monotonic!\n");
			ret=-22;
			goto END;
		}
		fprintf(fo,"Name=%s\n",inst[i].name);
		for(j=inst[i].inst_bag_ndx;j<inst[i+1].inst_bag_ndx;j++)
		{
			int k;
			fprintf(fo,"2147483647=2147483647 ; gen_ndx: %d - %d\n",
					inst_bag[j].gen_ndx,inst_bag[j+1].gen_ndx);
			for(k=inst_bag[j].gen_ndx;k<inst_bag[j+1].gen_ndx;k++)
				print_generator(fo,&igenlist[k]);
			fprintf(fo,"2147483647=2147483646 ; mod_ndx: %d - %d\n",
					inst_bag[j].mod_ndx,inst_bag[j+1].mod_ndx);
			for(k=inst_bag[j].mod_ndx;k<inst_bag[j+1].mod_ndx;k++)
				print_modulator(fo,&imodlist[k]);
		}
		fclose(fo);
	}
	chdir("..");
	
	// ---------------------------
	debug("sf_parse: Parsing presets...\n");
	CreateDirectory("presets",NULL);
	_chdir("presets");
	for(i=0;i<header.presets;i++)
	{
		preset[i].name[19]=0;
		//debug("[%d] %s                  \r",i+1,preset[i].name);
		if(i==header.presets-1)
		{
			if(strcmp(preset[i].name,"EOP")!=NULL)
			{
				debug("sf_parse: Bad presets w/o EOP\n");
				ret=-20;
				goto END;
			}
			break;
		}
		// remove spaces
		while(preset[i].name[0]==' ')
			strcpy(preset[i].name,preset[i].name+1);
		while(preset[i].name[strlen(preset[i].name)-1]==' ')
			preset[i].name[strlen(preset[i].name)-1]=0;
		
		char fname[256];
		memset(fname,0,sizeof(fname));
		sprintf(fname,"%03d_%03d_%s",preset[i].bank,preset[i].preset,preset[i].name);
		parse_name(fname,fname);
		fo=fopen(fname,"wb");
		if(fo==NULL)
		{
			debug("sf_parse: Error creating file %s!\n",fname);
			ret=-21;
			goto END;
		}
		fprintf(fo,"Name=%s\n",preset[i].name);
		if(preset[i].preset_bag_ndx>preset[i+1].preset_bag_ndx)
		{
			debug("sf_parse: Preset bag isnt monotonic!\n");
			ret=-22;
			goto END;
		}
		fprintf(fo,"bank=%d\n"
				"preset=%d\n"
				"lib=%x\n"
				"genre=%x\n"
				"morphology=%x\n"
				,
				preset[i].bank,preset[i].preset,
				preset[i].library,
				preset[i].genre,
				preset[i].morphology);
		
		for(j=preset[i].preset_bag_ndx;j<preset[i+1].preset_bag_ndx;j++)
		{
			int k;
			fprintf(fo,"2147483647=2147483647; gen_ndx %d - %d\n",preset_bag[j].gen_ndx,
					preset_bag[j+1].gen_ndx);
			for(k=preset_bag[j].gen_ndx;k<preset_bag[j+1].gen_ndx;k++)
				print_generator(fo,&pgenlist[k]);
			fprintf(fo,"2147483647=2147483646; mod_ndx %d - %d\n",preset_bag[j].mod_ndx,
					preset_bag[j+1].mod_ndx);
			for(k=preset_bag[j].mod_ndx;k<preset_bag[j+1].mod_ndx;k++)
				print_modulator(fo,&pmodlist[k]);
		}
		fclose(fo);
		fo=NULL;
	}
	//debug("\n\n");
	chdir("..");
	fo=fopen("info","wt");
	if(fo)
	{
		if(header.name[0])
			fprintf(fo,"Name: %s\n",header.name);
		if(header.engineer[0])
			fprintf(fo,"Engineer: %s\n",header.engineer);
		if(header.product[0])
			fprintf(fo,"Product: %s\n",header.product);
		if(header.copyright[0])
			fprintf(fo,"Copyright: %s\n",header.copyright);
		if(header.card[0])
			fprintf(fo,"Card: %s\n",header.card);
		if(header.engine[0])
			fprintf(fo,"Engine: %s\n",header.engine);
		if(header.rom_name[0])
			fprintf(fo,"ROM Name: %s\n",header.rom_name);
		if(header.comments[0])
			fprintf(fo,"Comments: %s\n",header.comments);
		if(header.creator[0])
			fprintf(fo,"Creator: %s\n",header.creator);
		fprintf(fo,"Version: %d\nVersionLow: %d\n",header.ver.major,
				header.ver.minor);
		if(header.rom_ver.major && header.rom_ver.minor)
			fprintf(fo,"RomVersion: %d\nRomVersionLow:%d\n",
					header.rom_ver.major,header.rom_ver.minor);
		fclose(fo);
	}
	chdir("..");
	
END:
	if(f_i)
		fclose(f_i);
	if(fo)
		fclose(fo);
	if(fi)
		fclose(fi);
	
	if(sample_data_w)
		free(sample_data_w);
	if(preset)
		free(preset);
	if(preset_bag)
		free(preset_bag);
	if(pmodlist)
		free(pmodlist);
	if(pgenlist)
		free(pgenlist);
	if(inst)
		free(inst);
	if(inst_bag)
		free(inst_bag);
	if(imodlist)
		free(imodlist);
	if(igenlist)
		free(igenlist);
	if(sample)
		free(sample);
	
	debug("sf_parse: Parsing %s\n",ret<0?"FAILED":"done");
	
	preset=0;
	preset_bag=0;
	pmodlist=0;
	pgenlist=0;
	
	inst=0;
	inst_bag=0;
	imodlist=0;
	igenlist=0;
	
	sample=0;
	
	memset(&header,0,sizeof(header));
	sample_data_w=0;
	
	if(unlink_file)
		unlink(real_file_name);
	
	return ret;
}

#elif defined(__APPLE__)
#warning SoundFont parse functions not implemented
#endif
