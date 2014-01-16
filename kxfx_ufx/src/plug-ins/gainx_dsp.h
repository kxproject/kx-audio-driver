
const char* const gainx_dsp_copyright= UCOPYRIGHT;
const char* const gainx_dsp_engine="";
const char* const gainx_dsp_comment="Inplace Gain Control; $nobypass";
const char* const gainx_dsp_created="Mar 19 2003";
const char* const gainx_dsp_guid="E063BF42-054C-4485-B11D-68BCA792AEA8";

const char* const gainx_dsp_name="GainX";
const int gainx_dsp_itramsize=0,gainx_dsp_xtramsize=0;

#define R_K	0x8002u

const dsp_register_info gainx_dsp_info[]={
	{ "L",0x4000,0x7,0xffff,0x0 },
	{ "R",0x4001,0x7,0xffff,0x0 },
	{ "L",0x8000,0x8,0xffff,0x0 },
	{ "R",0x8001,0x8,0xffff,0x0 },
	{ "k",0x8002,0x1,0xffff,0x80000000},
};

const dsp_code gainx_dsp_code[]={
	{ 0x1,0x8000,0x2040,0x4000,0x8002 },
	{ 0x4,0x8000,0x2040,0x8000,0x2041 },
	{ 0x1,0x8001,0x2040,0x4001,0x8002 },
	{ 0x4,0x8001,0x2040,0x8001,0x2041 },
};

