// kX kxctrl utility
// Copyright (c) Eugene Gavrilov, 2001-2008.
//  https://github.com/kxproject/ (previously www.kxproject.com)
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

#if defined(WIN32)	   
	#define mkdir(a,b)			CreateDirectory(a,NULL)
	#define chdir(a)			_chdir(a)
	#define ctrl_free(a)		LocalFree(a)
#elif defined(__APPLE__)
	#define ctrl_free(a)		free(a)
#else
	#error Unknown architecture
#endif

#ifdef _MSC_VER
	#pragma warning(disable:4100)
#endif

iKX *ikx=NULL;
int parse_file(char *file_name,int ( *command)(int argc,char **argv));
int parse_text(char *text,int size,int ( *command)(int argc,char **argv));
int batch_mode=0;

static int find_reg(word reg,dsp_register_info *info,int info_size)
{
	for(dword i=0;i<info_size/sizeof(dsp_register_info);i++)
	{
		if(info[i].num==reg)
			return i;
	}
	return -1;
}

void disassemble(dword *d,int sz);
void disassemble(dword *d,int sz)
{
	dword d1,d2;
	while(sz>0)
	{
		d1=*d++;
		d2=*d++;
		sz-=8;
		
		word op,res,a,x,y;
		
		if(!ikx->get_dsp())
		{
			op=(word)((d2&E10K1_OP_MASK_HI)>>E10K1_OP_SHIFT_LOW);
			res=(word)((d2&E10K1_R_MASK_HI)>>E10K1_OP_SHIFT_HI);
			a=(word)((d2&E10K1_A_MASK_HI));
			x=(word)((d1&E10K1_X_MASK_LOW)>>E10K1_OP_SHIFT_HI);
			y=(word)((d1&E10K1_Y_MASK_LOW));
		}
		else
		{
			op=(word)((d2&E10K2_OP_MASK_HI)>>E10K2_OP_SHIFT_LOW);
			res=(word)((d2&E10K2_R_MASK_HI)>>E10K2_OP_SHIFT_HI);
			a=(word)((d2&E10K2_A_MASK_HI));
			x=(word)((d1&E10K2_X_MASK_LOW)>>E10K2_OP_SHIFT_HI);
			y=(word)((d1&E10K2_Y_MASK_LOW));
		}
		
		kString s;
		s="\t ";
		ikx->format_opcode(&s,op);
		s+=" \t";
		ikx->format_reg(&s,res,NULL,0);
		s+=", ";
		ikx->format_reg(&s,a,NULL,0);
		s+=", ";
		ikx->format_reg(&s,x,NULL,0);
		s+=", ";
		ikx->format_reg(&s,y,NULL,0);
		s+="; ";
		//  ikx->format_opcode(&s,op,1);
		s+="\n";
		
		printf("%s",(const char *)s);
	}
}

void combine(char **argv);
void combine(char **argv)
{
	kString tmp;
	
	char name[KX_MAX_STRING];
	char copyright[KX_MAX_STRING];
	char engine[KX_MAX_STRING];
	char comment[KX_MAX_STRING];
	char created[KX_MAX_STRING];
	char guid[KX_MAX_STRING];
	
	int i;
	dsp_code *code=NULL;
	dsp_register_info *info=NULL;
	
	dsp_code *code2=NULL;
	
	dword m_dump[1024*2]; // code; 1024 or 512 x 2 dwords each instruction
	dword m_gprs[512];    // gpr: 256 or 512
	dword m_tram[256*2];  // tram: addr and data
	dword m_flg[256];     // 10k2 flg
	unsigned int n_instr=0,n_gprs=0;
	int is_10k2=0;
	
	int code_size=0,info_size=0,itramsize=0,xtramsize=0;
	int ret;
	FILE *f;
	f=fopen(argv[1],"rb");
	if(f)
	{
		fseek(f,0L,SEEK_END);
		size_t fsize=ftell(f);
		fseek(f,0L,SEEK_SET);
		char *mem=(char *)malloc(fsize);
		if(mem==NULL)
		{
			fclose(f);
			fprintf(stderr,"Not enough memory to load RIFX\n");
			return;
		}
		fsize=fread(mem,1,fsize,f);
		fclose(f);
		f=NULL;
		code=NULL;
		info=NULL;
		ret=-1;
		ret=ikx->parse_rifx(mem,(int)fsize,name,&code,&code_size,
							&info,&info_size,&itramsize,&xtramsize,
							copyright,engine,created,comment,guid);
		free(mem);
		if(ret<0)
		{
			fprintf(stderr,"Error parsing RIFX (%d)\n",ret);
			goto END;
		}
		else
		{
			fprintf(stderr,"RIFX successfuly parsed [%s]\n",name);
			if(ret!=0)
				fprintf(stderr,"Warnings: %d\n",ret);
		}
	} // f!=NULL
	else
	{
		fprintf(stderr,"Cannot open '%s'\n",argv[1]);
		goto END;
	}
	
	f=fopen(argv[2],"rb"); // dump
	if(!f)
	{
		fprintf(stderr,"Error opening dump\n");
		goto END;
	}
	char buff[17];
	fread(buff,1,16,f);
	if(strncmp(buff,"10k1 microcode $",16)==0)
	{
		ikx->set_dsp(0);
		n_gprs=256;
		n_instr=512;
		is_10k2=0;
	}
	else
		if(strncmp(buff,"10k2 microcode $",16)==0)
		{
			ikx->set_dsp(1);
			n_gprs=512;
			n_instr=1024;
			is_10k2=1;
		}
		else
		{
			fprintf(stderr,"Incorrect dump\n");
			goto END;
		}
	
	code2=(dsp_code *)malloc(n_instr*sizeof(dsp_code));
	if(code2==NULL)
	{
		fprintf(stderr,"No mem\n");
		goto END;
	}
	
	dword magic;
	magic=0xdeadbeaf;
	fread(&magic,1,4,f);
	if(magic!='$rcm')
	{
		fprintf(stderr,"Possibly incorrect dump file! [no microcode signature; '%x']\n",(unsigned)magic);
		goto END;
	}
	
	if(fread(&m_dump[0],1*4*2,n_instr,f)!=n_instr)
	{
		fprintf(stderr,"Bad dump file [microcode]\n");
		goto END;
	}
	
	magic=0xdeadbeaf; 
	fread(&magic,1,4,f);
	if(magic!='$rpg')
	{
		fprintf(stderr,"Possibly incorrect dump file! [no gpr signature; '%x']\n",(unsigned)magic);
		goto END;
	}
	
	if(fread(&m_gprs[0],4,n_gprs,f)!=n_gprs)
	{
		fprintf(stderr,"Bad dump file - $gpr section\n");
		goto END;
	}
	
	magic=0xdeadbeaf;
	fread(&magic,1,4,f);
	if(magic!='$mrt')
	{
		fprintf(stderr,"Possibly incorrect dump file! [no tram signature; '%x']\n",(unsigned)magic);
		goto END;
	}
	
	if(fread(&m_tram[0],2*4,256,f)!=256) // tram data & addr
	{
		fprintf(stderr,"Bad dump file - $trm section\n");
		goto END;
	}
	
	if(is_10k2)
	{
		magic=0xdeadbeaf;
		fread(&magic,1,4,f);
		if(magic!='$glf')
		{
			fprintf(stderr,"Possibly incorrect dump file! [no flag signature; '%x']\n",(unsigned)magic);
			goto END;
		}
		
		if(fread(&m_flg[0],1,256,f)!=256)
		{
			fprintf(stderr,"Bad dump file - $flg section\n");
			goto END;
		}
	}
	fclose(f);
	f=NULL;
	
	fprintf(stderr,"Dump file read\n");
	
	dword d1,d2;
	for(i=0;i<(int)n_instr;i++)
	{
		d1=m_dump[i*2];
		d2=m_dump[i*2+1];
		
		int op,res,a,x,y;
		
		if(is_10k2)
		{
			op=(d2&E10K2_OP_MASK_HI)>>E10K2_OP_SHIFT_LOW;
			res=(d2&E10K2_R_MASK_HI)>>E10K2_OP_SHIFT_HI;
			a=(d2&E10K2_A_MASK_HI);
			x=(d1&E10K2_X_MASK_LOW)>>E10K2_OP_SHIFT_HI;
			y=(d1&E10K2_Y_MASK_LOW);
		}
		else
		{
			op=(d2&E10K1_OP_MASK_HI)>>E10K1_OP_SHIFT_LOW;
			res=(d2&E10K1_R_MASK_HI)>>E10K1_OP_SHIFT_HI;
			a=(d2&E10K1_A_MASK_HI);
			x=(d1&E10K1_X_MASK_LOW)>>E10K1_OP_SHIFT_HI;
			y=(d1&E10K1_Y_MASK_LOW);
		}
		code2[i].op=(byte)op;
		code2[i].r=(word)res;
		code2[i].a=(word)a;
		code2[i].x=(word)x;
		code2[i].y=(word)y;
	}
	
	fprintf(stderr,"code uploaded\n");
	
	// search for rifx 'code' in 'code2'
	dword j;
	int offset;
	offset=-1;
	
	for(i=0;i<(int)n_instr;i++)
	{
		if(code2[i].op==code[0].op) // test
		{
			for(j=0;j<code_size/sizeof(dsp_code);j++)
				if(code2[i+j].op!=code[j].op)
					break;
			if(j==code_size/sizeof(dsp_code))
			{
				offset=i;
				fprintf(stderr,"RIFX Found in dump: @ offset= %d\n",i);
				printf("; RIFX is located @ offset %d\n",i);
			}
		}
	}
	
	fprintf(stderr,"dump search completed [%d]\n",offset);
	
	if(offset==-1)
	{
		fprintf(stderr,"RIFX not found in the dump\n");
		goto END;
	}
	
	// now we have: code[0..] - rifx; code2[offset..] - dump in memory corresponding rifx
	// re-initialize gprs
#define is_gpr(a) ((!is_10k2)?((a)>=0x100 && (a)<=0x1ff):((a)>=0x400 && (a)<=0x5ff))
	
	for(i=0;i<(int)(code_size/sizeof(dsp_code));i++)
	{
		int pos;
		
#define analyze_gpr(gpr,gpr2)	\
				if( ((!is_10k2) && (gpr>=0x100 && gpr<=0x3ff)) ||     \
				(is_10k2 && (gpr>=0x200 && gpr<=0x5ff))           \
				)			\
				{							\
					pos=find_reg((word)gpr2,info,info_size);		\
					if(pos==-1)					\
					{							\
						fprintf(stderr,"Error in RIFX: pointer (%xh) to nowhere [@%d]\n",gpr2,i);	\
						goto END;						\
					}							\
					/* found: translate it */				\
					if(is_gpr(gpr))					\
					{							\
						/*printf("kxctrl s 4 %x %x %x\n",gpr2,gprs[(gpr-0x100)*2],gprs[gpr]);*/	\
						info[pos].p=m_gprs[(gpr-(is_10k2?0x400:0x100))];			\
						info[pos].translated=gpr;			\
					}							\
				}
		
		analyze_gpr(code2[i+offset].r,code[i].r);
		analyze_gpr(code2[i+offset].a,code[i].a);
		analyze_gpr(code2[i+offset].x,code[i].x);
		analyze_gpr(code2[i+offset].y,code[i].y);
	}
	
	fprintf(stderr,"analysis done\n");
	
	ikx->disassemble_microcode(&tmp,KX_DISASM_DANE,-1,
							   code,code_size,
							   info,info_size,
							   itramsize,xtramsize,
							   name,copyright[0]?copyright:NULL,engine[0]?engine:NULL,created[0]?created:0,comment[0]?comment:0,
							   guid[0]?guid:0);
	
	fprintf(stderr,"writing results\n");
	
	fwrite((const char *)tmp,1,strlen((const char *)tmp),stdout);
	
END:
	if(code)
	{
		ctrl_free(code);
		code=NULL;
	}
	if(info)
	{
		ctrl_free(info);
		info=NULL;
	}
	if(code2)
	{
		free(code2);
		code2=NULL;
	}
	if(f)
	{
		fclose(f);
	}
}

void help(void);
void help(void)
{
	fprintf(stderr,
            " -cg \t\t\t\t - get EDSP/FPGA master clock frequency\n"
            " -cs <sample rate>\t\t - set EDSP/FPGA master clock frequency\n"
			" -ml <rifx file>\t\t - load RIFX/bin microcode\n"
			" -me <id>\t\t\t - enable microcode\n"
			" -mb <id> <mode>\t\t - toggle microcode bypass mode\n"
			" -gmf <id> \t\t\t - get microcode flag\n"
			" -smf <id> <flag>\t\t - set microcode flag\n"
			" -md <id>\t\t\t - disable microcode\n"
			" -mu <id>\t\t\t - unload microcode\n"
			" -mp [id]\t\t\t - list all uploaded effects / print fx regs\n"
			" -mo id\t\t\t\t - dump microcode (with opcodes)\n"
			" -mc <srcid> <reg> <dstid> <reg> - connect (reg is hex w/o '0x' or reg name)\n"
			" -mdc <srcid> <reg> \t\t - disconnect (reg is hex w/o '0x' or reg name)\n"
			"\t\t\t\t   if src | dst == -1, then reg is physical\n"
			" -ma <id> <pgm_name> <reg_name>\t - assign microcode slider to dsp pgm\n"
			"\t\t\t\t   [id: 0..5 - master/rec/kx0/kx1/synth/wave]\n"
			"\t\t\t\t   [set pgm_name to 'undefined'  to unmap]\n"
			" -mr\t\t\t\t - reset microcode to default\n"
			" -mrc\t\t\t\t - clear all microcode and stop the DSP\n"
			" -reset\t\t\t\t - reset settings to defaults\n"
			" -rv\t\t\t\t - reset hardware voices\n"
			" -dbr\t\t\t\t - re-initialize daughter board card(s)\n"
			" -mx <dll_name>\t\t\t - extract RIFX effects from DLL to .\\rifx\n"
			" -da [<file.bin>]\t\t - disassemble current 10kx/file's microcode\n"
			" -co <rifx.rifx> <memdump.bin> \t - combine rifx & dump to stdout [10k1 only]\n"
			"\n");
#if defined(WIN32)	
	fprintf(stderr,"-- pause --\n");
	_getch();
#endif
	fprintf(stderr,
			"\n"
			" -sg <pgm> <id> <val1> <val2>\t - set GPR\n"
			" -gg <pgm> <id>\t\t\t - get GPR\n"
			" -sr <id> <value>\t\t - set routing (id, value: hex - aaBBccDD)\n"
			" -gr <id>\t\t\t - get routing (id, value: hex)\n"
			" -sf <id> <value>\t\t - set FX amount (hex:0..ff)\n"
			" -gf <id> <value>\t\t - get FX amount (hex:0..ff)\n"
			" -shw <id> <value>\t\t - set HW parameter\n"
			" -ghw <id>\t\t\t - get HW parameter\n"
			" -istat\t\t\t\t - get spdif / i2s status\n"
			"\n"
			" -dd <num>\t\t\t - get driver's dword value\n"
			" -ds <num>\t\t\t - get driver's string value\n"
			" -sdb <id> <val>\t\t - set driver's buffer value (hex)\n"
			" -gdb <id>\t\t\t - get driver's buffer value (hex)\n"
			" -{s|g}{ac97|ptr|fn0|p|fpga}\t - set/get HAL register [reg [ch] val]\n"
			"\t\t\t\t (-sac97 0 0 - reset AC97)\n"
			" -ufpga <filename> \t\t - upload firmware into FPGA\n"
			" -link <src> <dst>\t\t - connect FPGA src to dst\n"
			" -sf2 {l|c|p|i|u} {dir|file} [{file|dir}]\n"
			"\t\t\t\t - Load/Compile/Parse/Info/Unload SoundFonts\n"
			" --nokx\t\t\t\t - do not init the driver (should be the last)\n"
			" --gui\t\t\t\t - perform interactive commands\n"
			" $<x>\t\t\t\t - use card number <x> (should be the first)\n"
			);
	if(!batch_mode)
	{
		if(ikx)
		{
			delete ikx;
			ikx=NULL;
		}	
		exit(1);
	}
}

const char *assignment_to_text(int i);
const char *assignment_to_text(int i)
{
	switch(i)
	{
		case MIXER_MASTER: return "Master";
		case MIXER_WAVE: return "Wave";
		case MIXER_SYNTH: return "Synth";
		case MIXER_KX0: return "kX0";
		case MIXER_KX1: return "kX1";
		case MIXER_REC: return "Rec";
	}
	return "(undef)";
}

int print_info(void);
int print_info(void)
{
	int sz=-3;
	sz=ikx->enum_soundfonts(0,0);
	if(sz>0)
	{
		printf("%ld soundfonts [%d bytes]\n",sz/sizeof(sfHeader),sz);
		sfHeader *hdr;
		hdr=(sfHeader *)malloc(sz);
		if(hdr)
		{
			if(!ikx->enum_soundfonts(hdr,sz))
			{
				for(dword i=0;i<sz/sizeof(sfHeader);i++)
				{
					printf("[%d] '%s'\n",hdr[i].rom_ver.minor,&hdr[i].name[0]);
				}
			} else { printf("Error enumerating sf\n"); sz=-2; }
			free(hdr);
		} else { printf("Not enough memory (%d)\n",sz); sz=-1; }
	} else
	{
		if(sz==0)
			printf("No loaded soundfonts\n");
		else
			printf("Error occured while acquiring soundfonts\n");
	}
	return sz;
}


int process(int argc, char **argv);
int process(int argc, char **argv)
{
	dword v1;
	int pgm;
	word id;
	char name[512];
	dsp_code *code;
	dsp_register_info *info;
	int code_size,info_size,itramsize,xtramsize;
	int ret;
	FILE *f=0;
	
	if(strcmp(argv[0],"--gui")==0)
	{
		fprintf(stderr,"Using device '%s'\n",ikx->get_device_name());
		printf("Entering interactive mode. Type '?' for help. Type 'quit' to exit\n\n");
		batch_mode=1;
		while(1)
		{
			char cmd[128]; cmd[0]=0;
			printf(">");
			fgets(cmd,sizeof(cmd)-1,stdin);
			
			while(cmd[strlen(cmd)-1]=='\r' || cmd[strlen(cmd)-1]=='\n')
				cmd[strlen(cmd)-1]=0;
			
			if(cmd[0]=='?' || strcmp(cmd,"help")==0)
			{
				help();
				printf("Type 'quit' to quit\n");
			}
			else
				if(strcmp(cmd,"quit")==0 || strcmp(cmd,"exit")==0)
					break;
				else
				{
					char to_do[128];
					sprintf(to_do,"-%s",cmd);
					parse_text(to_do,(int)strlen(to_do),process);
				}
		}
	}
	else
		if(strcmp(argv[0],"-gg")==0)
		{
			if(argc<3)
				help();
			else
				if(sscanf(argv[1],"%d",&pgm)==1) // id is #
				{
					unsigned tmp_id;
					if(sscanf(argv[2],"%x",&tmp_id)==1)
					{
						id=(word)tmp_id;
						if(ikx->get_dsp_register(pgm,id,&v1))
							printf("Get GPR failed\n");
						else
							printf("got GPR = %lx\n",(unsigned long)v1);
					} else printf("Bad id\n");
				} else printf("Bad pgm\n");
        }
    else
		if(strcmp(argv[0],"-cs")==0)
		{
            int sr=48000;

			if(argc<2)
				help();
			else
				if(sscanf(argv[1],"%d",&sr)==1) // id is #
				{
    				if(ikx->set_clock(sr))
						printf("Failed to set master clcok\n");
						else
							printf("Master clock set to %d\n",sr);
				} else printf("Bad sample rate\n");
        }
        else
    	if(strcmp(argv[0],"-cg")==0)
		{
            int sr=48000;

            if(ikx->get_clock(&sr))
              printf("Failed to get master clcok\n");
            else
			  printf("Master clock set to %d\n",sr);
        }
        else
			if(strcmp(argv[0],"-sg")==0)
			{
				if(argc<4)
					help();
				else
					if(sscanf(argv[1],"%d",&pgm)==1) // id is #
					{
						unsigned tmp_id;
						if(sscanf(argv[2],"%x",&tmp_id)==1)
						{
							id=(word)tmp_id;
							if(sscanf(argv[3],"%x",&v1)==1)
							{
								if(ikx->set_dsp_register(pgm,id,v1))
									printf("Get GPR failed\n");
								else
									printf("set GPR to = %lx\n",(unsigned long)v1);
							} else printf("Bad values\n");
						} else printf("Bad id\n");
					} else printf("Bad pgm\n");
			}
			else
				if(strcmp(argv[0],"-ml")==0)
				{
					if(argc<2)
						help();
					else
					{
						f=fopen(argv[1],"rb");
						if(f)
						{
							fseek(f,0L,SEEK_END);
							size_t fsize=ftell(f);
							fseek(f,0L,SEEK_SET);
							char *mem=(char *)malloc(fsize);
							if(mem==NULL)
							{
								fclose(f);
								printf("Not enough memory to load RIFX\n");
								return -9;
							}
							fsize=fread(mem,1,fsize,f);
							fclose(f);
							code=NULL;
							info=NULL;
							char m_copyright[KX_MAX_STRING];
							char m_engine[KX_MAX_STRING];
							char m_created[KX_MAX_STRING];
							char m_comment[KX_MAX_STRING];
							char m_guid[KX_MAX_STRING];
							
							ret=-1;
							ret=ikx->parse_rifx(mem,(int)fsize,name,&code,&code_size,
												&info,&info_size,&itramsize,&xtramsize,m_copyright,m_engine,m_created,m_comment,m_guid);
							if(ret<0)
							{
								fprintf(stderr,"Error parsing RIFX (%d)\n",ret);
							}
							else
							{
								printf("RIFX successfuly parsed [%s] Warnings: %d\n",name,ret);
							}
							free(mem);
							
							// now, translate & load
							pgm=ikx->load_microcode(name,code,code_size,info,info_size,itramsize,xtramsize,
														m_copyright,m_engine,m_created,m_comment,m_guid);
							if(pgm==0)
							{
								fprintf(stderr,"Load microcode failed\n");
							}
							else
							{
								printf("Microcode loaded; pgm=%d\n",pgm);
								if(!ikx->translate_microcode(pgm))
								{
									printf("Microcode translated\n");
								}
								else
								{
									printf("Microcode translation failed\n");
									ikx->unload_microcode(pgm);
								}
							}
							
							if(code)
							{
								ctrl_free(code);
								code=NULL;
							}
							if(info)
							{
								ctrl_free(info);
								info=NULL;
							}
						} // f!=NULL
						else perror("Error opening RIFX file");
						
					}
				}
				else
					if(strcmp(argv[0],"-mu")==0)
					{
						if(argc<2)
							help();
						else
						{
							if(sscanf(argv[1],"%d",&pgm)==1) // id is #
							{
								if(ikx->unload_microcode(pgm))
									printf("Unload microcode failed\n");
								else
									printf("Microcode unloaded\n");
							}
							else
							{
								printf("Bad ID specified [%s]\n",argv[1]);
							}
						}
					}
					else
						if(strcmp(argv[0],"-mo")==0)
						{
							if(argc<2)
								help();
							else
							{
								if(sscanf(argv[1],"%d",&pgm)==1) // id is #
								{
									dsp_microcode mc;
									if(ikx->enum_microcode(pgm,&mc)==0)
									{
										fprintf(stderr,"[%d] %s (%s%s%s); code_size=%d info_size=%d\n",
												pgm,
												mc.name,
												mc.flag&MICROCODE_TRANSLATED?"Uploaded ":"",
												mc.flag&MICROCODE_ENABLED?"Enabled ":"",
												mc.flag&MICROCODE_BYPASS?"ByPass":"",
												mc.code_size,mc.info_size);
										info=(dsp_register_info *)malloc(mc.info_size); memset(info,0,mc.info_size);
										code=(dsp_code *)malloc(mc.code_size); memset(code,0,mc.code_size);
										if(!ikx->get_microcode(pgm,code,mc.code_size,info,mc.info_size))
										{
											kString tmp;
											ikx->disassemble_microcode(&tmp,KX_DISASM_DANE,pgm,code,mc.code_size,info,mc.info_size,mc.itramsize,mc.xtramsize,mc.name,mc.copyright,mc.engine,mc.created,mc.comment,mc.guid);
											fwrite((const char *)tmp,1,strlen((const char *)tmp),stdout);
										} else printf("error getting microcode\n");
										free(code);
										free(info);
									} else printf("no such microcode\n");
								}
								else
								{
									printf("Bad ID specified [%s]\n",argv[1]);
								}
							}
						}
						else
							if(strcmp(argv[0],"-md")==0)
							{
								if(argc<2)
									help();
								else
								{
									if(sscanf(argv[1],"%d",&pgm)==1) // id is #
									{
										if(ikx->disable_microcode(pgm))
											printf("Disable microcode failed\n");
										else
											printf("Microcode disabled\n");
									}
									else
									{
										printf("Bad ID specified [%s]\n",argv[1]);
									}
								}
							}
							else
								if(strcmp(argv[0],"-me")==0)
								{
									if(argc<2)
										help();
									else
									{
										
										if(sscanf(argv[1],"%d",&pgm)==1) // id is #
										{
											if(ikx->enable_microcode(pgm))
												printf("Enable microcode failed\n");
											else
												printf("Microcode enabled\n");
										}
										else
										{
											printf("Bad ID specified [%s]\n",argv[1]);
										}
									}
								}
								else
									if(strcmp(argv[0],"-smf")==0)
									{
										if(argc<3)
											help();
										else
										{
											if(sscanf(argv[1],"%d",&pgm)==1) // id is #
											{
												unsigned state=0;
												sscanf(argv[2],"%x",&state);
												if(ikx->set_microcode_flag(pgm,(dword)state))
													printf("Microcode flag change failed\n");
												else
													printf("Microcode flag changed to '%x'\n",state);
											}
											else
											{
												printf("Bad ID specified [%s]\n",argv[1]);
											}
										}
									}
									else
										if(strcmp(argv[0],"-gmf")==0)
										{
											if(argc<2)
												help();
											else
											{
												if(sscanf(argv[1],"%d",&pgm)==1) // id is #
												{
													dword state=0;
													if(ikx->get_microcode_flag(pgm,&state))
														printf("Error getting microcode flag\n");
													else
														printf("Microcode [%d] flag: '%x'\n",pgm,(unsigned)state);
												}
												else
												{
													printf("Bad ID specified [%s]\n",argv[1]);
												}
											}
										}
										else
											if(strcmp(argv[0],"-mb")==0)
											{
												if(argc<3)
													help();
												else
												{
													if(sscanf(argv[1],"%d",&pgm)==1) // id is #
													{
														int state=0;
														sscanf(argv[2],"%d",&state);
														if(ikx->set_microcode_bypass(pgm,state))
															printf("Microcode bypass mode switch failed\n");
														else
															printf("Microcode bypass mode set to '%s'\n",state?"On":"Off");
													}
													else
													{
														printf("Bad ID specified [%s]\n",argv[1]);
													}
												}
											}
											else
												if(strcmp(argv[0],"-reset")==0) // reset settings
												{
													if(ikx->reset_settings())
														printf("Error resetting settings\n");
													else
														printf("Reset settings succeeded\n");
												}
												else
													if(strcmp(argv[0],"-rv")==0) // reset settings
													{
														if(ikx->reset_voices())
															printf("Error resetting hw voices\n");
														else
															printf("Reset hw voices succeeded\n");
													}
													else
														if(strcmp(argv[0],"-dbr")==0) // reset dB
														{
															if(ikx->reset_db())
																printf("Error resetting db\n");
															else
																printf("DB successfully reset\n");
														}
														else
															if(strcmp(argv[0],"-mr")==0)
															{
																if(ikx->reset_microcode())
																{
																	printf("Reset failed\n");
																}
																else
																{
																	printf("Reset succeeded\n");
																}
															}
															else
																if(strcmp(argv[0],"-mrc")==0)
																{
																	if(ikx->dsp_clear())
																	{
																		printf("DSP Clear failed\n");
																	}
																	else
																	{
																		printf("DSP Clear succeeded\n");
																	}
																}
																else
																	if(strcmp(argv[0],"-mx")==0)
																	{
																		if(argc<2)
																			help();
																		else
																		{
																			
																			f=fopen(argv[1],"rb");
																			if(f)
																			{
																				fseek(f,0L,SEEK_END);
																				size_t fsize=ftell(f);
																				fseek(f,0L,SEEK_SET);
																				char *mem=(char *)malloc(fsize);
																				if(mem==NULL)
																				{
																					fclose(f);
																					printf("Not enough memory to load RIFX\n");
																					return -9;
																				}
																				fsize=fread(mem,1,fsize,f);
																				fclose(f);
																				
																				size_t pos=0;
																				
																				mkdir("rifx",0777);
																				chdir("rifx");
#ifdef _MSC_VER																				
#pragma warning(disable:4127)
#endif
																				while(1)
																				{
																					if(strncmp(&mem[pos],"RIFX",4)==0)
																					{
																						char copyright[KX_MAX_STRING];
																						char engine[KX_MAX_STRING];
																						char created[KX_MAX_STRING];
																						char comment[KX_MAX_STRING];
																						char guid[KX_MAX_STRING];
																						
																						code=NULL;
																						info=NULL;
																						ret=-1;

																						ret=ikx->parse_rifx(&mem[pos],(int)(fsize-pos),name,&code,&code_size,
																											&info,&info_size,&itramsize,&xtramsize,
																											&copyright[0],
																											&engine[0],
																											&created[0],
																											&comment[0],
																											guid);

																						if(ret<0)
																						{
																							printf("Error parsing RIFX (%d)\n",ret);
																						}
																						else
																						{
																							printf("RIFX successfuly parsed [%s] warnings: %d\n",name,ret);
																							
																							ikx->generate_guid(guid);
																							
																							char tmp_name[KX_MAX_STRING+4];
																							strncpy(tmp_name,name,sizeof(tmp_name));
																							strncat(tmp_name,".da",sizeof(tmp_name)-strlen(tmp_name));
																							
																							f=fopen(tmp_name,"wt");
																							if(f)
																							{
																								kString tmp;
																								ikx->disassemble_microcode(&tmp,KX_DISASM_REGS|KX_DISASM_CODE,-1,code,code_size,info,info_size,itramsize,xtramsize,
																														   name,
																														   copyright[0]?copyright:0,
																														   engine[0]?engine:0,
																														   created[0]?created:0,
																														   comment[0]?comment:0,
																														   guid[0]?guid:0);
																								fwrite((const char *)tmp,1,strlen((const char *)tmp),f);
																								fclose(f);
																							}
																							strcat(name,".rifx");
																							f=fopen(name,"wb");
																							if(f)
																							{
																								dword szz=(byte)mem[pos+7]+(((byte)mem[pos+6])<<8)+8;
																								fwrite(&mem[pos],szz,1,f);
																								// write GUID here
																								fprintf(f,"$kX$\nguid=%s\ngenerated by kX Ctrl Utility version %x\n",guid,KX_VERSION_DWORD);
																								fclose(f);
																							}
																						}
																						if(code)
																						{
																							ctrl_free(code);
																							code=NULL;
																						}
																						if(info)
																						{
																							ctrl_free(info);
																							info=NULL;
																						}
																					}
																					
																					pos+=4;
																					if(pos>=fsize)
																						break;
																				}
																				
																				chdir("..");
																				free(mem);
																			} else perror("Error loading file");
																		}
																	}
																	else
																		if(strcmp(argv[0],"-da")==0)
																		{
																			if(argc==1) // current
																			{
																				if(!ikx->get_dsp()) 
																				{
																					dword microcode[1024];
																					for(dword i=0;i<1024;i++)
																						ikx->ptr_read(E10K1_MICROCODE_BASE+i,0,&microcode[i]);
																					disassemble(microcode,1024*4);
																				}
																				else // 10k2
																				{
																					dword microcode[2048];
																					for(dword i=0;i<2048;i++)
																						ikx->ptr_read(E10K2_MICROCODE_BASE+i,0,&microcode[i]);
																					disassemble(microcode,2048*4);
																				}
																			}
																			else // file
																			{
																				if(argc<2)
																					help();
																				else
																				{
																					
																					FILE *ff;
																					ff=fopen(argv[1],"rb");
																					if(ff)
																					{
																						fseek(ff,0L,SEEK_END);
																						size_t fsize=ftell(ff);
																						fseek(ff,0L,SEEK_SET);
																						char *mem=(char *)malloc(fsize);
																						if(mem==NULL)
																						{
																							fclose(ff);
																							printf("Not enough memory to load file\n");
																							return -9;
																						}
																						fsize=fread(mem,1,fsize,ff);
																						
																						fclose(ff);
																						if(memcmp(mem,"10k1 microcode $",16)==0)
																						{
																							ikx->set_dsp(0);
																							disassemble((dword *)(mem+16+4),1024*4);
																						}
																						else
																							if(memcmp(mem,"10k2 microcode $",16)==0)
																							{
																								ikx->set_dsp(1);
																								disassemble((dword *)(mem+16+4),2048*4);
																							}
																							else
																								printf("Incorrect microcode dump file\n");
																						free(mem);
																					} else perror("Error opening file");
																				}
																			}
																		}
																		else
																			if(strcmp(argv[0],"-mp")==0)
																			{
																				printf("// Microcode state:\n");
																				if(argc==1)
																				{
																					int i;
																					for(i=0;i<MAX_PGM_NUMBER;i++)
																					{
																						dsp_microcode mc;
																						if(ikx->enum_microcode(i,&mc)==0)
																						{
																							printf("[%d] %s (%s%s%s) - %s\n",i,mc.name,
																								   mc.flag&MICROCODE_TRANSLATED?"Uploaded ":"",
																								   mc.flag&MICROCODE_ENABLED?"Enabled ":"",
																								   mc.flag&MICROCODE_BYPASS?"Bypass":"",
																								   mc.guid);
																						}
																					}
																				}
																				else
																				{
																					if(argc<2)
																						help();
																					else
																					{	
																						
																						if(sscanf(argv[1],"%d",&pgm)==1)
																						{
																							dsp_microcode mc;
																							if(ikx->enum_microcode(pgm,&mc)==0)
																							{
																								printf("[%d] %s (%s%s%s); code_size=%d info_size=%d\n",
																									   pgm,
																									   mc.name,
																									   mc.flag&MICROCODE_TRANSLATED?"Uploaded ":"",
																									   mc.flag&MICROCODE_ENABLED?"Enabled ":"",
																									   mc.flag&MICROCODE_BYPASS?"Bypass":"",
																									   mc.code_size,mc.info_size);
																								info=(dsp_register_info *)malloc(mc.info_size); memset(info,0,mc.info_size);
																								code=(dsp_code *)malloc(mc.code_size); memset(code,0,mc.code_size);
																								if(!ikx->get_microcode(pgm,code,mc.code_size,info,mc.info_size))
																								{
																									kString tmp;
																									ikx->disassemble_microcode(&tmp,KX_DISASM_VALUES,pgm,code,mc.code_size,info,mc.info_size,mc.itramsize,mc.xtramsize,mc.name,mc.copyright,mc.engine,mc.created,mc.comment,mc.guid);
																									fwrite((const char *)tmp,1,strlen((const char *)tmp),stdout);
																								} else printf("error getting microcode\n");
																								free(code);
																								free(info);
																							} else printf("no such microcode\n");
																						}
																					}
																				}
																			}
																			else
																				if(strcmp(argv[0],"-mc")==0)
																				{
																					if(argc<5)
																						help();
																					else
																					{
																						
																						int pg1,pg2;
																						dword id1,id2;
																						pg1=0,pg2=0,id1=(dword)-1,id2=(dword)-1;
																						sscanf(argv[1],"%d",&pg1);
																						sscanf(argv[3],"%d",&pg2);
																						if(sscanf(argv[2],"%x",&id1)==1 &&
																						   sscanf(argv[4],"%x",&id2)==1)
																						{
																							if(ikx->connect_microcode(pg1,(word)id1,pg2,(word)id2))
																								printf("Connect failed\n");
																							else
																								printf("Connect ok\n");
																						}
																						else
																							printf("Only digits for now supported\n");
																					}
																				}
																				else
																					if(strcmp(argv[0],"-ma")==0)
																					{
																						if(argc<6)
																						{
																							if(argc!=2)
																								help();
																							else
																							{
																								kx_assignment_info ai;
																								sscanf(argv[1],"%d",&ai.level);
																								if(ikx->get_dsp_assignments(&ai)==0)
																								{
																									printf("Level %d (%s) assigned to microcode pgm: '%s', registers: '%s'/'%s'; max: %x\n",
																										   ai.level,assignment_to_text(ai.level),
																										   ai.pgm, ai.reg_left,ai.reg_right,ai.max_vol);
																								} else fprintf(stderr,"Error getting DSP assignments\n");
																							}
																						}
																						else
																						{
																							kx_assignment_info ai;
																							
																							char pgm_name[KX_MAX_STRING],reg_left[KX_MAX_STRING],reg_right[KX_MAX_STRING];
																							dword max_vol=0x7fffffff;
																							
																							int id_=0;
																							sscanf(argv[1],"%d",&id_); id=(word)id_;
																							sscanf(argv[2],"%s",pgm_name);
																							sscanf(argv[3],"%s",reg_left);
																							sscanf(argv[4],"%s",reg_right);
																							sscanf(argv[5],"%x",&max_vol);
																							
																							if(strcmp(pgm_name,"undefined")==0)
																							{
																								pgm_name[0]=0; reg_left[0]=0; reg_right[0]=0;
																							}
																							
																							ai.level=id;
																							strcpy(ai.pgm,pgm_name);
																							strcpy(ai.reg_left,reg_left);
																							strcpy(ai.reg_right,reg_right);
																							ai.max_vol=max_vol;
																							
																							if(ikx->set_dsp_assignments(&ai)==0)
																							{
																								printf("Level %d (%s) assigned to microcode id: pgm: '%s', registers: '%s'/'%s'; max: %x\n",
																									   id,assignment_to_text(id),
																									   ai.pgm, ai.reg_left,ai.reg_right,ai.max_vol);
																							} else fprintf(stderr,"Error settings DSP assignments\n");
																						}
																					}
																					else
																						if(strcmp(argv[0],"-mdc")==0)
																						{
																							if(argc<3)
																								help();
																							else
																							{
																								
																								int pg1; dword id1;
																								pg1=0,id1=(dword)-1;
																								sscanf(argv[1],"%d",&pg1);
																								if(sscanf(argv[2],"%x",&id1)==1)
																								{
																									if(ikx->disconnect_microcode(pg1,(word)id1))
																										printf("Disconnect failed\n");
																									else
																										printf("Disconnect ok\n");
																								}
																								else
																									printf("Only digits for now supported\n");
																							}
																						}
																						else
																							if(strcmp(argv[0],"-co")==0)
																							{
																								if(argc<3)
																									help();
																								else
																									combine(argv);
																							}
																							else
																								if(strcmp(argv[0],"-sf")==0)
																								{
																									if(argc<3)
																										help();
																									else
																									{
																										int n;
																										dword val;
																										
																										if(sscanf(argv[1],"%d",&n)==1 && sscanf(argv[2],"%x",&val)==1)
																										{
																											if(!ikx->set_send_amount(n,(byte)val))
																												printf("FX Amount for %d set to: %lx\n",n,(unsigned long)val);
																											else
																												printf("Error setting FX amount\n");
																										}
																										else help();
																									}
																								}
																								else
																									if(strcmp(argv[0],"-gf")==0)
																									{
																										if(argc<2)
																											help();
																										else
																										{
																											int n;
																											byte amount;
																											
																											if(sscanf(argv[1],"%d",&n)==1)
																											{
																												if(!ikx->get_send_amount(n,&amount))
																													printf("FX Amount for %d is: %x\n",n,amount);
																												else
																													printf("Error getting FX amount\n");
																											}
																											else help();
																										}
																									}
																									else
																										if(strcmp(argv[0],"-shw")==0)
																										{
																											if(argc<3)
																												help();
																											else
																											{
																												int n;
																												dword val;
																												
																												if(sscanf(argv[1],"%d",&n)==1 && sscanf(argv[2],"%x",&val)==1)
																												{
																													if(!ikx->set_hw_parameter(n,val))
																														printf("HW Parameter %d set to: %lx\n",n,(unsigned long)val);
																													else
																														printf("Error setting HW parameter\n");
																												}
																												else help();
																											}
																										}
																										else
																											if(strcmp(argv[0],"-ghw")==0)
																											{
																												if(argc<2)
																													help();
																												else
																												{
																													int n;
																													dword amount;
																													
																													if(sscanf(argv[1],"%d",&n)==1)
																													{
																														if(!ikx->get_hw_parameter(n,&amount))
																															printf("HW Parameter %d is: %lx\n",n,(unsigned long)amount);
																														else
																															printf("Error getting HW parameter\n");
																													}
																													else help();
																												}
																											}
																											else
																												if(strcmp(argv[0],"-sr")==0)
																												{
																													if(argc<3)
																														help();
																													else
																													{
																														int n;
																														dword val;
																														
																														if(sscanf(argv[1],"%d",&n)==1 && sscanf(argv[2],"%x",&val)==1)
																														{
																															if(!ikx->set_routing(n,val)) // FIXME: xrouting
																																printf("Routing for %d set to: %lx\n",n,(unsigned long)val);
																															else
																																printf("Error setting routing\n");
																														}
																														else help();
																													}
																												}
																												else
																													if(strcmp(argv[0],"-gr")==0) // FIXME: xrouting
																													{
																														if(argc<2)
																															help();
																														else
																														{
																															int n;
																															dword routing;
																															
																															if(sscanf(argv[1],"%d",&n)==1)
																															{
																																if(!ikx->get_routing(n,&routing))
																																	printf("FX Amount for %d is: %lx\n",n,(unsigned long)routing);
																																else
																																	printf("Error getting routing\n");
																															}
																															else help();
																														}
																													}
																													else
																														if(strcmp(argv[0],"-sdb")==0)
																														{
																															if(argc<3)
																																help();
																															{
																																int n;
																																int val;
																																
																																if(sscanf(argv[1],"%d",&n)==1 && sscanf(argv[2],"%x",&val)==1)
																																{
																																	if(!ikx->set_buffers(n,val))
																																		printf("Buffers for %d set to: %x\n",n,val);
																																	else
																																		printf("Error setting buffers\n");
																																}
																																else help();
																															}	
																														}
																														else
																															if(strcmp(argv[0],"-gdb")==0)
																															{
																																if(argc<2)
																																	help();
																																else
																																{
																																	int n;
																																	int val;
																																	
																																	if(sscanf(argv[1],"%d",&n)==1)
																																	{
																																		if(!ikx->get_buffers(n,&val))
																																			printf("Buffers for %d are: %x\n",n,val);
																																		else
																																			printf("Error getting buffers\n");
																																	}
																																	else help();
																																}
																															}
																															else
																																if(strcmp(argv[0],"-sac97")==0)
																																{
																																	if(argc<3)
																																		help();
																																	else
																																	{
																																		dword reg;
																																		dword val;
																																		if((sscanf(argv[1],"%x",&reg)==1) && (sscanf(argv[2],"%x",&val)==1))
																																		{
																																			if(!ikx->ac97_write((byte)reg,(word)val))
																																				printf("wrote AC97: reg=%lx val=%lx\n",(unsigned long)reg,(unsigned long)val);
																																			else
																																				printf("error writing AC97: reg=%lx val=%lx\n",(unsigned long)reg,(unsigned long)val);
																																		}
																																		else help();
																																	}	
																																}
																																else
																																	if(strcmp(argv[0],"-gac97")==0)
																																	{
																																		if(argc<2)
																																			help();
																																		else
																																		{
																																			dword reg;
																																			word val;
																																			if(sscanf(argv[1],"%x",&reg)==1)
																																			{
																																				if(!ikx->ac97_read((byte)reg,&val))
																																					printf("AC97[reg=%lx]=%x\n",(unsigned long)reg,val);
																																				else
																																					printf("error reading AC97 reg=%lx\n",(unsigned long)reg);
																																			}
																																			else help();
																																		}
																																	}
																																	else
																																		if(strcmp(argv[0],"-sfpga")==0)
																																		{
																																			if(argc<3)
																																				help();
																																			else
																																			{
																																				dword reg;
																																				dword val;
																																				if((sscanf(argv[1],"%x",&reg)==1) && (sscanf(argv[2],"%x",&val)==1))
																																				{
																																					if(!ikx->fpga_write((byte)reg,(byte)val))
																																						printf("wrote FPGA: reg=%lx val=%lx\n",(unsigned long)reg,(unsigned long)val);
																																					else
																																						printf("error writing FPGA: reg=%lx val=%lx\n",(unsigned long)reg,(unsigned long)val);
																																				}
																																				else help();
																																			}	
																																		}
																																		else
																																			if(strcmp(argv[0],"-gfpga")==0)
																																			{
																																				if(argc<2)
																																					help();
																																				else
																																				{
																																					dword reg;
																																					byte val;
																																					if(sscanf(argv[1],"%x",&reg)==1)
																																					{
																																						if(!ikx->fpga_read((byte)reg,&val))
																																							printf("FPGA[reg=%lx]=%x\n",(unsigned long)reg,val);
																																						else
																																							printf("error reading FPGA reg=%lx\n",(unsigned long)reg);
																																					}
																																					else help();
																																				}
																																			}
																																			else
																																				if(strcmp(argv[0],"-link")==0)
																																				{
																																					if(argc<3)
																																						help();
																																					else
																																					{
																																						dword src;
																																						dword dst;
																																						if((sscanf(argv[1],"%x",&src)==1) && (sscanf(argv[2],"%x",&dst)==1))
																																						{
																																							if(!ikx->fpga_link_src2dst(src,dst))
																																								printf("FPGA connected: src=%lx dst=%lx\n",(unsigned long)src,(unsigned long)dst);
																																							else
																																								printf("error connecting FPGA: src=%lx dst=%lx\n",(unsigned long)src,(unsigned long)dst);
																																						}
																																						else help();
																																					}	
																																				}
																																				else
																																					if(strcmp(argv[0],"-ufpga")==0)
																																					{
																																						if(argc<2)
																																							help();
																																						else
																																						{
																																							FILE *ff;
																																							ff=fopen(argv[1],"rb");
																																							if(ff)
																																							{
																																								fseek(ff,0L,SEEK_END);
																																								size_t fsize=ftell(ff);
																																								fseek(ff,0L,SEEK_SET);
																																								byte *mem=(byte *)malloc(fsize);
																																								if(mem==NULL)
																																								{
																																									fclose(ff);
																																									fprintf(stderr,"Not enough memory to load file (size is %ld)\n",fsize);
																																								}
																																								else
																																								{
																																									fsize=fread(mem,1,fsize,ff);
																																									fclose(ff);
																																									
																																									if(!ikx->upload_fpga_firmware(mem,(int)fsize))
																																										printf("Firmware uploaded\n");
																																									else
																																										printf("Failed to upload FPGA firmware\n");
																																									
																																									free(mem);
																																								}
																																							}
																																							else
																																								printf("Error opening file '%s'\n",argv[1]);
																																						}
																																					}
																																					else
																																						if(strcmp(argv[0],"-sptr")==0)
																																						{
																																							if(argc<4)
																																								help();
																																							else
																																							{
																																								dword reg,ch,val;
																																								if((sscanf(argv[1],"%x",&reg)==1) && (sscanf(argv[2],"%x",&ch)==1) &&
																																								   (sscanf(argv[3],"%x",&val)==1))
																																								{
																																									if(!ikx->ptr_write(reg,ch,val))
																																										printf("wrote PTR: reg=%lx ch=%lx val=%lx\n",(unsigned long)reg,(unsigned long)ch,(unsigned long)val);
																																									else
																																										printf("error writing PTR: reg=%lx ch=%lx val=%lx\n",(unsigned long)reg,(unsigned long)ch,(unsigned long)val);
																																								}
																																								else help();
																																							}
																																						}
																																						else
																																							if(strcmp(argv[0],"-gptr")==0)
																																							{
																																								if(argc<3)
																																									help();
																																								else
																																								{
																																									dword reg,ch,val;
																																									if((sscanf(argv[1],"%x",&reg)==1) && (sscanf(argv[2],"%x",&ch)==1))
																																									{
																																										if(!ikx->ptr_read(reg,ch,&val))
																																											printf("PTR[reg=%lx; ch=%lx]=%lx\n",(unsigned long)reg,(unsigned long)ch,(unsigned long)val);
																																										else
																																											printf("error reading PTR reg=%lx\n",(unsigned long)reg);
																																									}
																																									else help();
																																								}
																																							}
																																							else
																																								if(strcmp(argv[0],"-sfn0")==0)
																																								{
																																									if(argc<3)
																																										help();
																																									else
																																									{
																																										dword reg,val;
																																										if((sscanf(argv[1],"%x",&reg)==1) && (sscanf(argv[2],"%x",&val)==1))
																																										{
																																											if(!ikx->fn0_write(reg,val))
																																												printf("wrote fn0: reg=%lx val=%lx\n",(unsigned long)reg,(unsigned long)val);
																																											else
																																												printf("error writing fn0: reg=%lx val=%lx\n",(unsigned long)reg,(unsigned long)val);
																																										}
																																										else help();
																																									}
																																								}
																																								else
																																									if(strcmp(argv[0],"-gfn0")==0)
																																									{
																																										if(argc<2)
																																											help();
																																										else
																																										{
																																											dword reg,val;
																																											if(sscanf(argv[1],"%x",&reg)==1)
																																											{
																																												if(!ikx->fn0_read(reg,&val))
																																													printf("Fn0[reg=%lx]=%lx\n",(unsigned long)reg,(unsigned long)val);
																																												else
																																													printf("error reading FN0 reg=%lx\n",(unsigned long)reg);
																																											}
																																											else help();
																																										}
																																									}
																																									else
																																										if(strcmp(argv[0],"-sp")==0)
																																										{
																																											if(argc<3)
																																												help();
																																											else
																																											{
																																												int reg,val;
																																												if((sscanf(argv[1],"%x",&reg)==1) && (sscanf(argv[2],"%x",&val)==1))
																																												{
																																													if(!ikx->p16v_write(reg,val))
																																														printf("wrote p16v: reg=%x val=%x\n",reg,val);
																																													else
																																														printf("error writing p16v: reg=%x val=%x\n",reg,val);
																																												}
																																												else help();
																																											}
																																										}
																																										else
																																											if(strcmp(argv[0],"-gp")==0)
																																											{
																																												if(argc<2)
																																													help();
																																												else
																																												{
																																													dword reg,val;
																																													if(sscanf(argv[1],"%x",&reg)==1)
																																													{
																																														if(!ikx->p16v_read(reg,&val))
																																															printf("p16V[reg=%lx]=%lx\n",(unsigned long)reg,(unsigned long)val);
																																														else
																																															printf("error reading P16V reg=%lx\n",(unsigned long)reg);
																																													}
																																													else help();
																																												}
																																											}
																																											else
																																												if(strcmp(argv[0],"-sf2")==0)
																																												{
																																													if(argc<2)
																																														help();
																																													else
																																													{
																																														
																																														switch(argv[1][0])
																																														{
																																															case 'l':
																																															case 'L':
																																																if(argc<3)
																																																	help();
																																																FILE *ff;
																																																ff=fopen(argv[2],"rb");
																																																if(ff)
																																																{
																																																	fclose(ff);
																																																	ret=-1;
#if !defined(__APPLE__)
																																																	ret=ikx->parse_soundfont(argv[2],NULL);
#else
																																																	printf("SoundFont API not implemented for OSX\n");
#endif
																																																}
																																																else
																																																{
																																																	ret=-1;
#if !defined(__APPLE__)
																																																	ret=ikx->compile_soundfont(argv[2],NULL);
#else
																																																	printf("SoundFont API not implemented for OSX\n");
#endif
																																																}
																																																if(ret<=0)
																																																	printf("Error: %d\n",ret);
																																																else
																																																	printf("SoundFont uploaded successfully. Id=%d\n",ret);
																																																break;
																																															case 'c':
																																															case 'C':
																																																if(argc<4)
																																																	help();
																																																ret=-1;
#if !defined(__APPLE__)
																																																ret=ikx->compile_soundfont(argv[2],argv[3]);
#else
																																																printf("SoundFont API not implemented for OSX\n");
#endif
																																																if(ret)
																																																	printf("Error: %d\n",ret);
																																																else
																																																	printf("Ok\n");
																																																break;
																																															case 'p':
																																															case 'P':
																																																if(argc<4)
																																																	help();
																																																ret=-1;
#if !defined(__APPLE__)
																																																ret=ikx->parse_soundfont(argv[2],argv[3]);
#else
																																																printf("SoundFont API not implemented for OSX\n");
#endif
																																																if(ret)
																																																	printf("Error: %d\n",ret);
																																																else
																																																	printf("Ok\n");
																																																break;
																																															case 'i':
																																															case 'I':
																																																print_info();
																																																break;
																																															case 'u':
																																															case 'U':
																																																if(argc<3)
																																																	help();
																																																int id_;
																																																if(sscanf(argv[2],"%d",&id_)==1)
																																																{
																																																	if(ikx->unload_soundfont(id_))
																																																		printf("Error unloading\n");
																																																	else
																																																		printf("SoundFont unloaded\n");
																																																} else help();
																																																break;
																																															default:
																																																help();
																																														}
																																													}
																																												}
																																												else
																																													if(strcmp(argv[0],"-dd")==0) // driver's dword
																																													{
																																														if(argc<2)
																																															help();
																																														else
																																														{
																																															int n;
																																															dword val;
																																															
																																															if(sscanf(argv[1],"%d",&n)==1)
																																															{
																																																if(!ikx->get_dword(n,&val))
																																																	printf("dword value %d is: %lx\n",n,(unsigned long)val);
																																																else
																																																	printf("Error getting dword value\n");
																																															}
																																															else help();
																																														}
																																													}
																																													else
																																														if(strcmp(argv[0],"-istat")==0)
																																														{
																																															kx_spdif_i2s_status st;
																																															if(!ikx->get_spdif_i2s_status(&st))
																																															{
																																																printf("Status:\n"
																																																	   "spdif A: %lx %lx (%lx)\n",(unsigned long)st.spdif.channel_status_a,(unsigned long)st.spdif.srt_status_a,(unsigned long)st.spdif.channel_status_a_x);
																																																printf("spdif B: %lx %lx (%lx)\n",(unsigned long)st.spdif.channel_status_b,(unsigned long)st.spdif.srt_status_b,(unsigned long)st.spdif.channel_status_b_x);
																																																printf("spdif C: %lx %lx (%lx)\n",(unsigned long)st.spdif.channel_status_c,(unsigned long)st.spdif.srt_status_c,(unsigned long)st.spdif.channel_status_c_x);
																																																printf("i2s 0: %lx\n",(unsigned long)st.i2s.srt_status_0);
																																																printf("i2s 1: %lx\n",(unsigned long)st.i2s.srt_status_1);
																																																printf("i2s 2: %lx\n",(unsigned long)st.i2s.srt_status_2);
																																																printf("spdif 0: %lx / %lx\n",(unsigned long)st.spdif.scs0,(unsigned long)st.spdif.scs0x);
																																																printf("spdif 1: %lx / %lx\n",(unsigned long)st.spdif.scs1,(unsigned long)st.spdif.scs1x);
																																																printf("spdif 2: %lx / %lx\n",(unsigned long)st.spdif.scs2,(unsigned long)st.spdif.scs2x);
																																																printf("spdif freq: %d\n",st.spdif.spo_sr==0?44100:st.spdif.spo_sr==1?48000:96000);
																																																printf("p16v rec: %lx\n",(unsigned long)st.p16v);
																																															} else printf("Error getting spdif / i2s status\n");
																																														}
																																														else
																																															if(strcmp(argv[0],"-ds")==0)
																																															{
																																																if(argc<2)
																																																	help();
																																																else
																																																{
																																																	int n;
																																																	char drvname[KX_MAX_STRING];
																																																	
																																																	if(sscanf(argv[1],"%d",&n)==1)
																																																	{
																																																		if(!ikx->get_string(n,drvname))
																																																			printf("Driver string for %d is: %s\n",n,drvname);
																																																		else
																																																			printf("Error getting driver string\n");
																																																	}
																																																	else help();
																																																}
																																															}
																																															else
																																															{
																																																if(!batch_mode)
																																																	help();
																																																else
																																																	fprintf(stderr,"Invalid command\n");
																																															}
	return 0;
}

int main(int argc, char* argv[])
{
	// initialize MFC and print and error on failure
#if defined(WIN32)
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		fprintf(stderr,"Fatal Error: MFC initialization failed\n");
		return 1;
	}
#endif
	
	fprintf(stderr,"kX Driver Control Program\n"KX_COPYRIGHT_STR"\n");
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
	
	ikx=iKX::create(id);
	

    if(ikx==NULL)
	{
		#if defined(WIN32)
		if(strstr(GetCommandLine(),"--nokx")==0)
		#else
		int cnt=argc-1;
		while(cnt)
		{
			if(strstr(argv[cnt],"--nokx")!=0)
				break;
			cnt--;
		}
		if(cnt==0)
		#endif
		{
			fprintf(stderr,"Error initializing iKX interface\n");
			delete ikx;
			return -2;
		}
		
        ikx=new iKX(); // create 'dummy' instance
	}
	
	if(argc<2)
		help();
	
	if(argv[1][0]=='@') // list of commands
	{
		batch_mode=1;
		parse_file(&argv[1][1],process);
	}
	else
	{
		process(argc-1,&argv[1]);
	}
	
	ikx->destroy();
	
	return 0;
}
