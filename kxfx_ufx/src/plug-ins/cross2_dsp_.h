
#ifndef _NO_STATICS

const char* const cross2_name		= "Crossover-2";
const char* const cross2_copyright	= UCOPYRIGHT;
const char* const cross2_engine		= "";
const char* const cross2_created	= "Mar 18 2003";
const char* const cross2_comment	= "2nd-Order Crossover, 12dB/octave; $nobypass";
const char* const cross2_guid		= "993A73A0-7893-11D8-B0D1-8B8FB634CD13";

const int cross2_itramsize	= 0x00000; // 0.00 ms (0 samples)
const int cross2_xtramsize	= 0x00000; // 0 ms (0 samples)

#define R(i)
#define _	0xFFFF,
const dsp_register_info cross2_info[] =
{
R(0)	{"L", 		0x4000u, 0x07, _ 0x00000000},	// input
R(1)	{"R", 		0x4001u, 0x07, _ 0x00000000},	// input
R(2)	{"lo L", 			0x8000u, 0x08, _ 0x00000000},	// output
R(3)	{"hi L", 			0x8001u, 0x08, _ 0x00000000},	// output
R(4)	{"lo R", 			0x8002u, 0x08, _ 0x00000000},	// output
R(5)	{"hi R", 			0x8003u, 0x08, _ 0x00000000},	// output
R(6)	{"k", 			0x8004u, 0x01, _ 0x79DE5848},	// static, 0.952
R(7)	{"z1", 			0x8005u, 0x01, _ 0x00000000},	// static
R(8)	{"z2", 			0x8006u, 0x01, _ 0x00000000},	// static
R(9)	{"z3", 			0x8007u, 0x01, _ 0x00000000},	// static
R(10)	{"z4", 			0x8008u, 0x01, _ 0x00000000},	// static
R(11)	{"i", 			0x8009u, 0x03, _ 0x00000000},	// temp
R(12)	{"x", 			0x800Au, 0x03, _ 0x00000000},	// temp
R(13)	{"y", 			0x800Bu, 0x03, _ 0x00000000},	// temp
};
#undef _
#undef R

#define I(i)
const dsp_code cross2_code[] =
{
I(0)	{0x0, 0x8009u, 0x4000u, 0x2040u, 0x2040u},	// {macs,	R_I, R_IN1, 0x2040u, 0x2040u	},
I(1)	{0x1, 0x800Au, 0x8005u, 0x8009u, 0x8004u},	// {macsn,	R_X, R_Z1, R_I, R_K	},
I(2)	{0x0, 0x8005u, 0x8009u, 0x800Au, 0x8004u},	// {macs,	R_Z1, R_I, R_X, R_K	},
I(3)	{0xE, 0x8009u, 0x800Au, 0x204Du, 0x8009u},	// {interp,	R_I, R_X, 0x204Du, R_I	},
I(4)	{0x1, 0x800Bu, 0x8006u, 0x8009u, 0x8004u},	// {macsn,	R_Y, R_Z2, R_I, R_K	},
I(5)	{0x0, 0x8006u, 0x8009u, 0x800Bu, 0x8004u},	// {macs,	R_Z2, R_I, R_Y, R_K	},
I(6)	{0xE, 0x8000u, 0x800Bu, 0x204Du, 0x8009u},	// {interp,	R_L1, R_Y, 0x204Du, R_I	},
I(7)	{0x0, 0x8001u, 0x800Au, 0x8000u, 0x204Eu},	// {macs,	R_H1, R_X, R_L1, 0x204Eu	},
I(8)	{0x0, 0x8009u, 0x4001u, 0x2040u, 0x2040u},	// {macs,	R_I, R_IN2, 0x2040u, 0x2040u	},
I(9)	{0x1, 0x800Au, 0x8007u, 0x8009u, 0x8004u},	// {macsn,	R_X, R_Z3, R_I, R_K	},
I(10)	{0x0, 0x8007u, 0x8009u, 0x800Au, 0x8004u},	// {macs,	R_Z3, R_I, R_X, R_K	},
I(11)	{0xE, 0x8009u, 0x800Au, 0x204Du, 0x8009u},	// {interp,	R_I, R_X, 0x204Du, R_I	},
I(12)	{0x1, 0x800Bu, 0x8008u, 0x8009u, 0x8004u},	// {macsn,	R_Y, R_Z4, R_I, R_K	},
I(13)	{0x0, 0x8008u, 0x8009u, 0x800Bu, 0x8004u},	// {macs,	R_Z4, R_I, R_Y, R_K	},
I(14)	{0xE, 0x8002u, 0x800Bu, 0x204Du, 0x8009u},	// {interp,	R_L2, R_Y, 0x204Du, R_I	},
I(15)	{0x0, 0x8003u, 0x800Au, 0x8002u, 0x204Eu},	// {macs,	R_H2, R_X, R_L2, 0x204Eu	},
};
#undef I

#endif

// register ids
#define R_IN1		0x4000u
#define R_IN2		0x4001u
#define R_L1		0x8000u
#define R_H1		0x8001u
#define R_L2		0x8002u
#define R_H2		0x8003u
#define R_K			0x8004u
#define R_Z1		0x8005u
#define R_Z2		0x8006u
#define R_Z3		0x8007u
#define R_Z4		0x8008u
#define R_I			0x8009u
#define R_X			0x800Au
#define R_Y			0x800Bu
