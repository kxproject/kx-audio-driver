// kX SDK:
// kX API, kX Audio Driver Interface, kX Plugin Manager API
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

//   ------------------------------
//   kX Dynamic Dane Implementation
//   ------------------------------
//   do not include this file directly, use 'kxapi.h' instead

#ifndef KX_DBL_TO_DSPWORD
#define KX_DBL_TO_DSPWORD
    inline dword _dbl_to_dspword(double x)
    {
      double y = (x * 2147483647.5) - 0.25;
      y += 6755399441055744.0;
      return ((dword*) &y)[0];
    }
#endif

class KX_CLASS_TYPE iDane
{
 public:
 	iDane(char *name_=NULL,char *guid_=NULL);
 	~iDane();

 	void reset(int what=(IKX_UPDATE_ALL&(~(IKX_UPDATE_NAME|IKX_UPDATE_GUID)))); 
 	// the same flags IKX_UPDATE_...
 	// except 'name' and GUID

        // main 'declarator'
        // returns register ID
 	word declare(char *name_,word id,byte type,dword p);

        // inlines...
 	word declare_temp(char *name_,word id=0) { return declare(name_,id,GPR_TEMP,0); };
 	word declare_static(char *name_,dword value,word id=0) { return declare(name_,id,GPR_STATIC,value); };
 	word declare_static(char *name_,float value,word id=0) { return declare_static(name_,(dword)_dbl_to_dspword((double)value),id); };
 	word declare_static(char *name_,double value,word id=0) { return declare_static(name_,(dword)_dbl_to_dspword(value),id); };
 	word declare_const(char *name_,dword value,word id=0) { return declare(name_,id,GPR_CONST,value); };
 	word declare_const(char *name_,float value,word id=0) { return declare_static(name_,(dword)_dbl_to_dspword((double)value),id); };
 	word declare_const(char *name_,double value,word id=0) { return declare_static(name_,(dword)_dbl_to_dspword(value),id); };
 	word declare_output(char *name_,word id=0) { return declare(name_,id,GPR_OUTPUT,0); };
 	word declare_input(char *name_,word id=0) { return declare(name_,id,GPR_INPUT,0); };
 	word declare_control(char *name_,dword value,word id=0) { return declare(name_,id,GPR_CONTROL,value); };
 	word declare_control(char *name_,float value,word id=0) { return declare_control(name_,(dword)_dbl_to_dspword((double)value),id); };
 	word declare_control(char *name_,double value,word id=0) { return declare_control(name_,(dword)_dbl_to_dspword(value),id); };

        // main 'opcode' generator
        // returns opcode number
        int op(byte mne,word r,word a,word x,word y);

        // inlines...
 	int macs(word r,word a,word x,word y) { return op(MACS,r,a,x,y); };
 	int macsn(word r,word a,word x,word y) { return op(MACS1,r,a,x,y); };
 	int macs1(word r,word a,word x,word y) { return op(MACS1,r,a,x,y); };
 	int macw(word r,word a,word x,word y) { return op(MACW,r,a,x,y); };
 	int macw1(word r,word a,word x,word y) { return op(MACW1,r,a,x,y); };
 	int macwn(word r,word a,word x,word y) { return op(MACW1,r,a,x,y); };
 	int macints(word r,word a,word x,word y) { return op(MACINTS,r,a,x,y); };
 	int macintw(word r,word a,word x,word y) { return op(MACINTW,r,a,x,y); };
 	int acc3(word r,word a,word x,word y) { return op(ACC3,r,a,x,y); };
 	int macmv(word r,word a,word x,word y) { return op(MACMV,r,a,x,y); };
 	int andxor(word r,word a,word x,word y) { return op(ANDXOR,r,a,x,y); };
 	int tstneg(word r,word a,word x,word y) { return op(TSTNEG,r,a,x,y); };
 	int limit(word r,word a,word x,word y) { return op(LIMIT,r,a,x,y); };
 	int limit1(word r,word a,word x,word y) { return op(LIMIT1,r,a,x,y); };
 	int log(word r,word a,word x,word y) { return op(LOG,r,a,x,y); };
 	int exp(word r,word a,word x,word y) { return op(EXP,r,a,x,y); };
 	int interp(word r,word a,word x,word y) { return op(INTERP,r,a,x,y); };
 	int skip(word r,word a,word x,word y) { return op(SKIP,r,a,x,y); };

        // this function populates -references- for code&info massives,
        //  but duplicates any text strings used (KX_MAX_STRING)
 	int populate(char *name_,dsp_code* &code_,int &code_size_,
   		dsp_register_info* &info_,int &info_size_,int &itramsize_,int &xtramsize_,
   		char *copyright_,
   		char *engine_,
   		char *created_,
   		char *comment_,
   		char *guid_);

	// -- don't access these directly... (consider 'private'):
        // -------------------------------------------------------
 	dsp_code *code;
 	dsp_register_info *info;
 	int itramsize,xtramsize;
 	int last_info,last_code;

 	char copyright[KX_MAX_STRING];
 	char name[KX_MAX_STRING];
 	char engine[KX_MAX_STRING];
 	char comment[KX_MAX_STRING];
 	char created[KX_MAX_STRING];
 	char guid[KX_MAX_STRING];
};
