
#ifndef _NO_STATICS

char* u128_name		= "u128";
char* u128_copyright	= "© Max Mikhailov, 2003-2004";
char* u128_engine	= "Emu10kX";
char* u128_created	= "November 7 2003";
char* u128_comment	= "panner128, experimental, don't use";
char* u128_guid		= "87c09a40-23bc-11d8-b0d0-bad008ae0513";

int u128_itramsize	= 0x00000; // 0.00 msec (0 samples)
int u128_xtramsize	= 0x00000; // 0 msec (0 samples)

#define R(i)
#define _	0xFFFF,
efx_register_info u128_info[] =
{
R(0)	{"in", 			0x4000u, 0x07, _ 0x00000000},	// input
R(1)	{"nw", 			0x8000u, 0x08, _ 0x00000000},	// output
R(2)	{"ne", 			0x8001u, 0x08, _ 0x00000000},	// output
R(3)	{"sw", 			0x8002u, 0x08, _ 0x00000000},	// output
R(4)	{"se", 			0x8003u, 0x08, _ 0x00000000},	// output
R(5)	{"n", 			0x8004u, 0x08, _ 0x00000000},	// output
R(6)	{"s", 			0x8005u, 0x08, _ 0x00000000},	// output
R(7)	{"w", 			0x8006u, 0x08, _ 0x00000000},	// output
R(8)	{"e", 			0x8007u, 0x08, _ 0x00000000},	// output
R(9)	{"angle", 		0x8008u, 0x01, _ 0x00000000},	// static
R(10)	{"i", 			0x8009u, 0x03, _ 0x00000000},	// temp
};
#undef _
#undef R

#define I(i)
efx_code u128_code[] =
{
I(0)	{0x0, 0x8009u, 0x4000u, 0x2040u, 0x2040u},	// {macs,	R_I, R_IN, 0x2040u, 0x2040u	},
I(1)	{0xE, 0x8004u, 0x8009u, 0x8008u, 0x2040u},	// {interp,	R_N, R_I, R_ANGLE, 0x2040u	},
I(2)	{0xE, 0x8001u, 0x2040u, 0x8008u, 0x8009u},	// {interp,	R_NE, 0x2040u, R_ANGLE, R_I	},
};
#undef I

#endif

// register ids
#define R_IN		0x4000u
#define R_NW		0x8000u
#define R_NE		0x8001u
#define R_SW		0x8002u
#define R_SE		0x8003u
#define R_N			0x8004u
#define R_S			0x8005u
#define R_W			0x8006u
#define R_E			0x8007u
#define R_ANGLE		0x8008u
#define R_I			0x8009u
