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
#include "interface/dspnames.h"

static int find_reg(word reg,const dsp_register_info *info,int info_size)
{
	for(dword i=0;i<info_size/sizeof(dsp_register_info);i++)
	{
		if(info[i].num==reg)
			return i;
	}
	return -1;
}

int iKX::format_reg(kString *out,word a,const dsp_register_info *info,int info_size)
{
	kString tmp;
	
	if(a<0x100)
	{
        if(get_dsp())
        {
			if(a<sizeof(operand_names_k2)/sizeof(operand_names_k2[0]))
				tmp.Format(" %s",operand_names_k2[a]); 	
			else
				tmp.Format(" ?incorrect(%x)",a);
        }
        else
        {
			if(a<sizeof(operand_names_k1)/sizeof(operand_names_k1[0]))
				tmp.Format(" %s ",operand_names_k1[a]); 	
			else
				tmp.Format(" ?incorrect(%x)",a);
        }
	}
	else 						
		if(a<0x200)
		{
			if(get_dsp())
			{
				if(a>=0x160 && a<=0x19f)
					tmp.Format("KX_E32IN(0x%x)",a-0x160);
				else
					if(a>=0x1e0 && a<=0x21f)
						tmp.Format("KX_E32OUT(0x%x)",a-0x1e0);
			}
			else
				tmp.Format("GPR_k1[%d]",a-0x100);
		}
		else
			if(a<0x300)
				tmp.Format("TankData[%d]",a-0x200);
			else
				if(a<0x400)
					tmp.Format("TankAddr[%d]",a-0x300);
				else
					if(a<0x600)
					{
						if(get_dsp())
							tmp.Format("GPR_k2[%d]",a-0x400);
						else
							tmp.Format("unknown[%x]",a);
					}
					else
						
						if(((a)>=KX_CONST) && ((a)<=(KX_CONST+0xff)))
						{						
							if((unsigned)(a-KX_CONST+0x80) < sizeof(operand_names_k2)/sizeof(operand_names_k2[0]))
							{
								if(get_dsp() || (a!=C_80000))
									tmp.Format(" %s" ,operand_names_k2[a-KX_CONST+0x80]); // consider 10k1 consts=10k2 consts
								else
									tmp.Format(" %s" ,operand_names_k1[a-KX_CONST]);
							}
							else
								tmp.Format(" ??incorrect(%x)",a-KX_CONST);
						} else 						
							if(((a)>=KX_IN(0)) && ((a)<=KX_IN(0xff))) /*inputs*/	
							{						
								tmp.Format(" KX_IN(0x%x)" ,a-KX_IN(0)); 		
							} else 						
								if(((a)>=KX_OUT(0)) && ((a)<=KX_OUT(0xff))) /*outputs*/  
								{						
									tmp.Format(" KX_OUT(0x%x)" ,a-KX_OUT(0x0));
								} else 						
									if(((a)>=KX_FX(0)) && ((a)<=KX_FX(0xff))) /*outputs*/  
									{						
										tmp.Format(" KX_FX(0x%x)" ,a-KX_FX(0x0)); 		
									} else 						
										if(((a)>=KX_FX2(0)) && ((a)<=KX_FX2(0xff))) /*outputs*/  
										{						
											tmp.Format(" KX_FX2(0x%x)" ,a-KX_FX2(0x0)); 		
										} else 						
											if(((a)>=KX_E32IN(0)) && ((a)<=KX_E32IN(0xff))) /*outputs*/  
											{						
												tmp.Format(" KX_E32IN(0x%x)" ,a-KX_E32IN(0x0)); 		
											} else 						
												if(((a)>=KX_E32OUT(0)) && ((a)<=KX_E32OUT(0xff))) /*inputs*/  
												{						
													tmp.Format(" KX_E32OUT(0x%x)" ,a-KX_E32OUT(0x0)); 		
												} else 						
												{
													int t_reg=find_reg(a,info,info_size);
													if(info && (t_reg!=-1))
													{
														if(info[t_reg].type!=GPR_CONST)
															tmp.Format(" %s",info[t_reg].name); 
														else
															tmp.Format(" 0x%x",info[t_reg].p);
													}
													else						
														tmp.Format(" !!!%x!!!",a);			
												}						
	
	(*out)+=tmp;
	return 0;
}

int iKX::format_opcode(kString *s,word op,int fl) // if fl=1, print 'help', not the opcode
{
	if(op<TOTAL_OPCODES)
	{
		if(fl)
			*s+=op_helps[op];
		else
			*s+=op_codes[op];
		
		return 0;
	}
	else return -1;
}


int iKX::disassemble_microcode(kString *out,int flag,int pgm,const dsp_code *code,int code_size,
							   const dsp_register_info *info,int info_size,
							   int itramsize,int xtramsize,
							   const char *name,const char *copyright,const char *engine,
							   const char *created,const char *comment,const char *guid)
{
	kString tmp;
	
	if(flag&KX_DISASM_CPP)
	{
		char tmp_name[KX_MAX_STRING];
		strcpy(tmp_name,name);
		while(tmp_name[strlen(tmp_name)-1]==' ')
			tmp_name[strlen(tmp_name)-1]=0;
		
		tmp.Format("// kX DSP Generated file\n\n// 10kX microcode\n// Patch name: '%s'\n\n",tmp_name);
		(*out)+=tmp;
		if(copyright && copyright[0])
		{
			tmp.Format("char *%s_copyright=\"%s\";\n",tmp_name,copyright);
			(*out)+=tmp;
		} else 
		{
			tmp.Format("char *%s_copyright=\"\";\n",tmp_name);
			(*out)+=tmp;
		}
		
		(*out)+="// NOTE: The present DSP microcode dump is protected by the \n"
		"// license agreement bundled with the appropriate software \n"
		"// package containing this microcode,\n"
		"// regardless the particular copyright notice is present in the dump.\n\n";
		
		if(engine && engine[0])
		{
			tmp.Format("const char *%s_engine=\"%s\";\n",tmp_name,engine);
			(*out)+=tmp;
		} else 
		{
			tmp.Format("const char *%s_engine=\"\";\n",tmp_name);
			(*out)+=tmp;
		}
		if(comment && comment[0])
		{
			tmp.Format("const char *%s_comment=\"%s\";\n",tmp_name,comment);
			(*out)+=tmp;
		} else 
		{
			tmp.Format("const char *%s_comment=\"\";\n",tmp_name);
			(*out)+=tmp;
		}
		if(created && created[0])
		{
			tmp.Format("const char *%s_created=\"%s\";\n",tmp_name,created);
			(*out)+=tmp;
		} else 
		{
			tmp.Format("const char *%s_created=\"\";\n",tmp_name);
			(*out)+=tmp;
		}
		if(guid && guid[0])
		{
			tmp.Format("const char *%s_guid=\"%s\";\n",tmp_name,guid);
			(*out)+=tmp;
		} else 
		{
			tmp.Format("const char *%s_guid=\"\";\n",tmp_name);
			(*out)+=tmp;
		}
		(*out)+="\n";
		tmp.Format("const char *%s_name=\"%s\";\n",tmp_name,tmp_name);
		(*out)+=tmp;
		tmp.Format("int %s_itramsize=%d,%s_xtramsize=%d;\n\n",tmp_name,itramsize,tmp_name,xtramsize);
		(*out)+=tmp;
		
		tmp.Format("dsp_register_info %s_info[]={\n",tmp_name);
		(*out)+=tmp;
		for(dword i=0;i<info_size/sizeof(dsp_register_info);i++)
		{
			tmp.Format("\t{ \"%s\",0x%x,0x%x,0x%x,0x%x },\n",
					   info[i].name,info[i].num,info[i].type,DSP_REG_NOT_TRANSLATED,info[i].p);
			(*out)+=tmp;
		}
		tmp.Format("};\n\n");
		(*out)+=tmp;
		
		tmp.Format("dsp_code %s_code[]={\n",tmp_name);
		(*out)+=tmp;
		for(dword i=0;i<code_size/sizeof(dsp_code);i++)
		{
			tmp.Format("\t{ 0x%x,0x%x,0x%x,0x%x,0x%x },\n",
					   code[i].op,code[i].r,code[i].a,code[i].x,code[i].y);
			(*out)+=tmp;
		}
		tmp.Format("};\n\n");
		(*out)+=tmp;
		return 0;
	}
	
	if(name && name[0])
	{
		tmp.Format("name \"%s\";\n",name);
		(*out)+=tmp;
	}
	if(copyright && copyright[0])
	{
		tmp.Format("copyright \"%s\";\n",copyright);
		(*out)+=tmp;
	}
	(*out)+=
	"; NOTE The present DSP microcode dump is protected by the \n"
	"; license agreement bundled with the appropriate software \n"
	"; package containing this microcode,\n"
	"; regardless the particular copyright notice is present in the dump.\n\n";
	
	if(engine && engine[0])
	{
		tmp.Format("engine \"%s\";\n",engine);
		(*out)+=tmp;
	}
	if(created && created[0])
	{
		tmp.Format("created \"%s\";\n",created);
		(*out)+=tmp;
	}
	if(comment && comment[0])
	{
		tmp.Format("comment \"%s\";\n",comment);
		(*out)+=tmp;
	}
	if(guid && guid[0])
	{
		tmp.Format("guid \"%s\";\n",guid);
		(*out)+=tmp;
	}
	
	if(itramsize)
	{	tmp.Format("\titramsize %d\n",itramsize); (*out)+=tmp; }
	if(xtramsize)
	{	tmp.Format("\txtramsize %d\n",xtramsize); (*out)+=tmp; }	
	
	if(flag&(KX_DISASM_REGS|KX_DISASM_VALUES))
	{
		tmp.Format("; Registers\n");
		(*out)+=tmp;
		
		int prev_type=-1;
		int same_type=0;
		int had_itram=0;
		int had_xtram=0;
		for(dword i=0;i<info_size/sizeof(dsp_register_info);i++)
		{
			if((info[i].type&GPR_MASK)==GPR_TRAMA) continue;
			
			if((flag&KX_DISASM_VALUES) && (pgm!=-1))
			{
				dword v1;
				if(get_dsp_register(pgm,info[i].num,&v1)==0)
				{
					tmp.Format("[%x]: '%s' p=%x; translated: %x; current=%x\n",info[i].num,info[i].name,info[i].p,info[i].translated,v1);
					(*out)+=tmp;
					if(info[i].translated!=DSP_REG_NOT_TRANSLATED)
					{
						if(((info[i].type&GPR_MASK)==GPR_ITRAM) || 
						   ((info[i].type&GPR_MASK)==GPR_XTRAM))
						{
							dword p1,p2,p3=0;
							ptr_read(info[i].translated,0,&p1);
							ptr_read(info[i].translated+0x100,0,&p2);
							if(is_10k2)
								ptr_read(info[i].translated-0x100,0,&p3);
							tmp.Format("[ -- TRAM: p1=%x p2=%x p3=%x]\n",p1,p2,p3);
							(*out)+=tmp;
						}
					}
				}
				continue;
			}
			
			same_type++;
			if((prev_type!=(info[i].type&GPR_MASK)) || (same_type>=3)) 
			{ if(prev_type!=-1) (*out)+=";\n"; prev_type=info[i].type&GPR_MASK; same_type=0; }
			
			if(((info[i].type&GPR_MASK)==GPR_ITRAM)&&(had_itram==0))
			{
				had_itram=1;
				if(itramsize>0)
				{
					tmp.Format("; Internal TRAM delay line (%d samples; ~%f sec)\n",
							   itramsize,(float)itramsize/48000.0);
					(*out)+=tmp;
					
				} else { tmp.Format("; ERROR: itramsize=0\n"); (*out)+=tmp; }
			}
			if(((info[i].type&GPR_MASK)==GPR_XTRAM)&&(had_xtram==0))
			{
				had_xtram=1;
				if(xtramsize>0)
				{
					tmp.Format("; External TRAM delay line (%d samples; ~%f sec)\n",
							   xtramsize,(float)xtramsize/48000.0);
					(*out)+=tmp; 
					
				} else { tmp.Format("; ERROR: xtramsize=0\n"); (*out)+=tmp;  }
			}
			
			switch(info[i].type&GPR_MASK)
			{
				case GPR_STATIC:
					if(!same_type)
						(*out)+="\tstatic ";
					else
						(*out)+=", ";
					tmp.Format("%s=0x%x",info[i].name,info[i].p);
					break;
				case GPR_CONST:
					if(!same_type)
						(*out)+=";\tconst ";
					else
						(*out)+=", ";
					tmp.Format("%s=0x%x",info[i].name,info[i].p);
					break;
				case GPR_TEMP:
					if(!same_type)
						(*out)+="\ttemp ";
					else
						(*out)+=", ";
					tmp.Format("%s",info[i].name);
					break;
				case GPR_CONTROL:
					if(!same_type)
						(*out)+="\tcontrol ";
					else
						(*out)+=", ";
					tmp.Format("%s=0x%x",info[i].name,info[i].p);
					break;
				case GPR_INPUT:
					if(!same_type)
						(*out)+="\tinput ";
					else
						(*out)+=", ";
					tmp.Format("%s",info[i].name);
					break;
				case GPR_OUTPUT:
					if(!same_type)
						(*out)+="\toutput ";
					else
						(*out)+=", ";
					tmp.Format("%s",info[i].name);
					break;
				case GPR_ITRAM:
					// test bits: read/write
					switch(info[i].type&(TRAM_WRITE|TRAM_READ))
				{
					case 0: tmp.Format("\tstatic coarse %s=0x%x;\n", info[i].name,info[i].p); same_type=0; break;
					case TRAM_READ: tmp.Format("\tidelay read %s at 0x%x;\n",info[i].name,info[i].p); same_type=0; break;
					case TRAM_WRITE: tmp.Format("\tidelay write %s at 0x%x;\n",info[i].name,info[i].p); same_type=0; break;
					case TRAM_READ|TRAM_WRITE: tmp.Format("\tidelay add %s at 0x%x;\n",info[i].name,info[i].p); same_type=0; break;
				}
					break;
				case GPR_XTRAM:
					// test bits: read/write
					switch(info[i].type&(TRAM_WRITE|TRAM_READ))
				{
					case 0: tmp.Format("\tstatic xcoarse %s=0x%x;\n",info[i].name,info[i].p); same_type=0; break;
					case TRAM_READ: tmp.Format("\txdelay read %s at 0x%x;\n", info[i].name,info[i].p); same_type=0; break;
					case TRAM_WRITE: tmp.Format("\txdelay write %s at 0x%x;\n", info[i].name,info[i].p); same_type=0; break;
					case TRAM_READ|TRAM_WRITE: tmp.Format("\txdelay add %s at 0x%x;\n", info[i].name,info[i].p); same_type=0; break;
				}
					break;
				case GPR_INTR:
					if(!same_type)
						(*out)+="\tintr ";
					else
						(*out)+=", ";
					tmp.Format("%s=%x",info[i].name,info[i].p);
					break;
				default:
					tmp.Format("; ??? unknown register type\n");
			} // switch
			(*out)+=tmp;
		} // for each reg
	} // disasm regs?
	
	if(flag&KX_DISASM_CODE)
	{
		tmp.Format("\n\n; Code\n");
		(*out)+=tmp;
		
		for(dword i=0;i<code_size/sizeof(dsp_code);i++)
		{
			(*out)+="\t ";
			format_opcode(out,code[i].op);
			(*out)+=" \t";
			format_reg(out,code[i].r,info,info_size);
			(*out)+=", ";
			format_reg(out,code[i].a,info,info_size);
			(*out)+=", ";
			format_reg(out,code[i].x,info,info_size);
			(*out)+=", ";
			format_reg(out,code[i].y,info,info_size);
			(*out)+=";\n";
		}
	}
	(*out)+="\nend\n";
	
	return 0;
}

int iKX::assemble_microcode(char *buf,kString *err,char *name,dsp_code **code,int *code_size,
							dsp_register_info **info,int *info_size,int *itramsize,int *xtramsize,
							char *copyright,
							char *engine,
							char *created,
							char *comment,
							char *guid)
{
	int assemble_dane(char *buf,kString *err,char *name,dsp_code **code,int *code_size,
					  dsp_register_info **info,int *info_size,int *itramsize,int *xtramsize,
					  char *copyright,char *engine,char *created,char *comment,char *guid);
	
	return assemble_dane(buf,err,name,code,code_size,
						 info,info_size,itramsize,xtramsize,copyright,engine,created,comment,guid);
}
