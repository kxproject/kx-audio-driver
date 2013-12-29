
#ifndef _NO_STATICS

// ............................................................................

const char* const eqp1_name		= "EQ P1";
const char* const eqp1_copyright	= UCOPYRIGHT;
const char* const eqp1_engine	= "";
const char* const eqp1_created	= "May 9 2003";
const char* const eqp1_comment	= "1-Band Parametric Equalizer; $nobypass";
const char* const eqp1_guid		= "8010000D-0ADF-11D6-BFBC-D4F706E10C52";

const int eqp1_itramsize	= 0x00000; // 0.00 ms (0 samples)
const int eqp1_xtramsize	= 0x00000; // 0 ms (0 samples)

#define R(i)
#define _	0xFFFF,
const dsp_register_info eqp1_info[] =
{
R(0)	{"L", 			0x4000u, 0x07, _ 0x00000000},	// input
R(1)	{"R", 			0x4001u, 0x07, _ 0x00000000},	// input
R(2)	{"L", 			0x8000u, 0x08, _ 0x00000000},	// output
R(3)	{"R", 			0x8001u, 0x08, _ 0x00000000},	// output
R(4)	{"r004", 		0x8002u, 0x01, _ 0x7FFFFFFF},	// static, 1.0
R(5)	{"r005", 		0x8003u, 0x01, _ 0x00000000},	// static
R(6)	{"r006", 		0x8004u, 0x01, _ 0x00000000},	// static
R(7)	{"r007", 		0x8005u, 0x01, _ 0x00000000},	// static
R(8)	{"r008", 		0x8006u, 0x01, _ 0x00000000},	// static
R(9)	{"r009", 		0x8007u, 0x01, _ 0x00000000},	// static
R(10)	{"r010", 		0x8008u, 0x01, _ 0x00000000},	// static
R(11)	{"r011", 		0x8009u, 0x01, _ 0x00000000},	// static
R(12)	{"r012", 		0x800Au, 0x01, _ 0x00000000},	// static
R(13)	{"r013", 		0x800Bu, 0x01, _ 0x00000001},	// static
R(14)	{"r014", 		0x800Cu, 0x03, _ 0x00000000},	// temp  
};
#undef _
#undef R

#define I(i)
const dsp_code eqp1_code[] =
{
I(0)	{0x0, 0x800Cu, 0x2040u, 0x4000u, 0x8002u},	// {macs,	R_T, 0x2040u, R_IN0, R_B0	},
I(1)	{0x6, 0x8000u, 0x2056u, 0x8007u, 0x8007u},	// {acc3,	R_OUT0, 0x2056u, R_Z1, R_Z1	},
I(2)	{0x0, 0x8007u, 0x8008u, 0x8000u, 0x8005u},	// {macs,	R_Z1, R_Z2, R_OUT0, R_A1	},
I(3)	{0x0, 0x8007u, 0x2056u, 0x800Cu, 0x8003u},	// {macs,	R_Z1, 0x2056u, R_T, R_B1	},
I(4)	{0x0, 0x8008u, 0x2040u, 0x8000u, 0x8006u},	// {macs,	R_Z2, 0x2040u, R_OUT0, R_A2	},
I(5)	{0x0, 0x8008u, 0x2056u, 0x800Cu, 0x8004u},	// {macs,	R_Z2, 0x2056u, R_T, R_B2	},
I(6)	{0x4, 0x8000u, 0x2040u, 0x8000u, 0x800Bu},	// {macints,	R_OUT0, 0x2040u, R_OUT0, R_G	},
I(7)	{0x0, 0x800Cu, 0x2040u, 0x4001u, 0x8002u},	// {macs,	R_T, 0x2040u, R_IN1, R_B0	},
I(8)	{0x6, 0x8001u, 0x2056u, 0x8009u, 0x8009u},	// {acc3,	R_OUT1, 0x2056u, R_Z3, R_Z3	},
I(9)	{0x0, 0x8009u, 0x800Au, 0x8001u, 0x8005u},	// {macs,	R_Z3, R_Z4, R_OUT1, R_A1	},
I(10)	{0x0, 0x8009u, 0x2056u, 0x800Cu, 0x8003u},	// {macs,	R_Z3, 0x2056u, R_T, R_B1	},
I(11)	{0x0, 0x800Au, 0x2040u, 0x8001u, 0x8006u},	// {macs,	R_Z4, 0x2040u, R_OUT1, R_A2	},
I(12)	{0x0, 0x800Au, 0x2056u, 0x800Cu, 0x8004u},	// {macs,	R_Z4, 0x2056u, R_T, R_B2	},
I(13)	{0x4, 0x8001u, 0x2040u, 0x8001u, 0x800Bu},	// {macints,	R_OUT1, 0x2040u, R_OUT1, R_G	},
};
#undef I

// ............................................................................

const char* const eqp1m_name	= eqp1_name;
const char* const eqp1m_copyright	= eqp1_copyright;
const char* const eqp1m_engine	= eqp1_engine;
const char* const eqp1m_created	= eqp1_created;
const char* const eqp1m_comment	= "1-Band Parametric Equalizer, Mono; $nobypass";
const char* const eqp1m_guid	= "8010000D-0ADF-11D7-BFBC-D4F706E10C52";

const int eqp1m_itramsize	= eqp1_itramsize;
const int eqp1m_xtramsize	= eqp1_xtramsize;

#define R(i)
#define _	0xFFFF,
const dsp_register_info eqp1m_info[] =
{
R(0)	{"I", 			0x4000u, 0x07, _ 0x00000000},	// input
//R(1)	{"R", 			0x4001u, 0x07, _ 0x00000000},	// input
R(2)	{"O", 			0x8000u, 0x08, _ 0x00000000},	// output
//R(3)	{"R", 			0x8001u, 0x08, _ 0x00000000},	// output
R(4)	{"r004", 		0x8002u, 0x01, _ 0x7FFFFFFF},	// static, 1.0
R(5)	{"r005", 		0x8003u, 0x01, _ 0x00000000},	// static
R(6)	{"r006", 		0x8004u, 0x01, _ 0x00000000},	// static
R(7)	{"r007", 		0x8005u, 0x01, _ 0x00000000},	// static
R(8)	{"r008", 		0x8006u, 0x01, _ 0x00000000},	// static
R(9)	{"r009", 		0x8007u, 0x01, _ 0x00000000},	// static
R(10)	{"r010", 		0x8008u, 0x01, _ 0x00000000},	// static
//R(11)	{"r011", 		0x8009u, 0x01, _ 0x00000000},	// static
//R(12)	{"r012", 		0x800Au, 0x01, _ 0x00000000},	// static
R(13)	{"r013", 		0x800Bu, 0x01, _ 0x00000001},	// static
R(14)	{"r014", 		0x800Cu, 0x03, _ 0x00000000},	// temp  
};
#undef _
#undef R

#define I(i)
const dsp_code eqp1m_code[] =
{
I(0)	{0x0, 0x800Cu, 0x2040u, 0x4000u, 0x8002u},	// {macs,	R_T, 0x2040u, R_IN0, R_B0	},
I(1)	{0x6, 0x8000u, 0x2056u, 0x8007u, 0x8007u},	// {acc3,	R_OUT0, 0x2056u, R_Z1, R_Z1	},
I(2)	{0x0, 0x8007u, 0x8008u, 0x8000u, 0x8005u},	// {macs,	R_Z1, R_Z2, R_OUT0, R_A1	},
I(3)	{0x0, 0x8007u, 0x2056u, 0x800Cu, 0x8003u},	// {macs,	R_Z1, 0x2056u, R_T, R_B1	},
I(4)	{0x0, 0x8008u, 0x2040u, 0x8000u, 0x8006u},	// {macs,	R_Z2, 0x2040u, R_OUT0, R_A2	},
I(5)	{0x0, 0x8008u, 0x2056u, 0x800Cu, 0x8004u},	// {macs,	R_Z2, 0x2056u, R_T, R_B2	},
I(6)	{0x4, 0x8000u, 0x2040u, 0x8000u, 0x800Bu},	// {macints,	R_OUT0, 0x2040u, R_OUT0, R_G	},
/*I(7)	{0x0, 0x800Cu, 0x2040u, 0x4001u, 0x8002u},	// {macs,	R_T, 0x2040u, R_IN1, R_B0	},
I(8)	{0x6, 0x8001u, 0x2056u, 0x8009u, 0x8009u},	// {acc3,	R_OUT1, 0x2056u, R_Z3, R_Z3	},
I(9)	{0x0, 0x8009u, 0x800Au, 0x8001u, 0x8005u},	// {macs,	R_Z3, R_Z4, R_OUT1, R_A1	},
I(10)	{0x0, 0x8009u, 0x2056u, 0x800Cu, 0x8003u},	// {macs,	R_Z3, 0x2056u, R_T, R_B1	},
I(11)	{0x0, 0x800Au, 0x2040u, 0x8001u, 0x8006u},	// {macs,	R_Z4, 0x2040u, R_OUT1, R_A2	},
I(12)	{0x0, 0x800Au, 0x2056u, 0x800Cu, 0x8004u},	// {macs,	R_Z4, 0x2056u, R_T, R_B2	},
I(13)	{0x4, 0x8001u, 0x2040u, 0x8001u, 0x800Bu},	// {macints,	R_OUT1, 0x2040u, R_OUT1, R_G	},*/
};
#undef I

// ............................................................................

#endif

// register ids
#define R_IN0		0x4000u
#define R_IN1		0x4001u
#define R_OUT0		0x8000u
#define R_OUT1		0x8001u
#define R_B0		0x8002u
#define R_B1		0x8003u
#define R_B2		0x8004u
#define R_A1		0x8005u
#define R_A2		0x8006u
#define R_Z1		0x8007u
#define R_Z2		0x8008u
#define R_Z3		0x8009u
#define R_Z4		0x800Au
#define R_G			0x800Bu   

