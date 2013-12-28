// kX Driver
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


#ifndef KX_EAX_PRESETS_H
#define KX_EAX_PRESETS_H

EAX10_REVERBPROPERTIES eax10_list[]=
{
  {  EAX10_PRESET_GENERIC         },
  {  EAX10_PRESET_PADDEDCELL      },
  {  EAX10_PRESET_ROOM            },
  {  EAX10_PRESET_BATHROOM        },
  {  EAX10_PRESET_LIVINGROOM      },
  {  EAX10_PRESET_STONEROOM       },
  {  EAX10_PRESET_AUDITORIUM      },
  {  EAX10_PRESET_CONCERTHALL     },
  {  EAX10_PRESET_CAVE            },
  {  EAX10_PRESET_ARENA           },
  {  EAX10_PRESET_HANGAR          },
  {  EAX10_PRESET_CARPETEDHALLWAY },
  {  EAX10_PRESET_HALLWAY         },
  {  EAX10_PRESET_STONECORRIDOR   },
  {  EAX10_PRESET_ALLEY           },
  {  EAX10_PRESET_FOREST          },
  {  EAX10_PRESET_CITY            },
  {  EAX10_PRESET_MOUNTAINS       },
  {  EAX10_PRESET_QUARRY          },
  {  EAX10_PRESET_PLAIN           },
  {  EAX10_PRESET_PARKINGLOT      },
  {  EAX10_PRESET_SEWERPIPE       },
  {  EAX10_PRESET_UNDERWATER      },
  {  EAX10_PRESET_DRUGGED         },
  {  EAX10_PRESET_DIZZY           },
  {  EAX10_PRESET_PSYCHOTIC       },
  {  EAX_ENVIRONMENT_UNDEFINED, 0.5F,1.493F,0.5F }
};

#define EAX30_PRESET_GENERIC \
	{0,		7.5f,	1.000f,	-1000,	-100,	0,		1.49f,	0.83f,	1.00f,	-2602,	0.007f,	0.00f,0.00f,0.00f,	200,	0.011f,		0.00f,0.00f,0.00f,	0.250f,	0.000f,	0.250f,	0.000f,	-5.0f,	5000.0f,	250.0f,	0.00f,	0x3f }
#define EAX30_PRESET_PADDEDCELL \
	{1,		1.4f,	1.000f,	-1000,	-6000,	0,		0.17f,	0.10f,	1.00f,	-1204,	0.001f, 0.00f,0.00f,0.00f,  207,	0.002f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f,	-5.0f,	5000.0f,	250.0f,	0.00f,	0x3f }
#define EAX30_PRESET_ROOM \
	{2,		1.9f,	1.000f,	-1000,	-454,	0,		0.40f,	0.83f,	1.00f,  -1646,	0.002f, 0.00f,0.00f,0.00f,	53,		0.003f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f,	-5.0f,  5000.0f,	250.0f,	0.00f,	0x3f }
#define EAX30_PRESET_BATHROOM \
	{3,		1.4f,	1.000f,	-1000,  -1200,	0,		1.49f,	0.54f,	1.00f,  -370,	0.007f, 0.00f,0.00f,0.00f,	1030,	0.011f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f,	-5.0f,  5000.0f,	250.0f,	0.00f,	0x3f }
#define EAX30_PRESET_LIVINGROOM \
	{4,		2.5f,	1.000f,	-1000,  -6000,	0,		0.50f,	0.10f,	1.00f,  -1376,	0.003f, 0.00f,0.00f,0.00f,	-1104,	0.004f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f,	-5.0f,  5000.0f,	250.0f,	0.00f,	0x3f }
#define EAX30_PRESET_STONEROOM \
	{5,		11.6f,	1.000f,  -1000, -300,	0,		2.31f,	0.64f,	1.00f,	-711,	0.012f, 0.00f,0.00f,0.00f,	83,		0.017f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f,	-5.0f,  5000.0f,	250.0f,	0.00f,	0x3f }
#define EAX30_PRESET_AUDITORIUM \
	{6,		21.6f,	1.000f,  -1000,	-476,	0,		4.32f,	0.59f,	1.00f,	-789,	0.020f, 0.00f,0.00f,0.00f,	-289,	0.030f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f,	-5.0f,  5000.0f,	250.0f,	0.00f,	0x3f }
#define EAX30_PRESET_CONCERTHALL \
	{7,		19.6f,	1.000f,  -1000,	-500,	0,		3.92f,	0.70f,	1.00f,  -1230,	0.020f, 0.00f,0.00f,0.00f,  -02,	0.029f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_CAVE \
	{8,		14.6f,	1.000f,  -1000,	0,		0,		2.91f,	1.30f,	1.00f,  -602,	0.015f, 0.00f,0.00f,0.00f,	-302,	0.022f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f,	0.00f,	0x1f }
#define EAX30_PRESET_ARENA \
	{9,		36.2f,	1.000f,  -1000,	-698,	0,		7.24f,	0.33f,	1.00f,  -1166,	0.020f, 0.00f,0.00f,0.00f,  16,		0.030f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f,	0.00f,	0x3f }
#define EAX30_PRESET_HANGAR \
	{10,	50.3f,	1.000f,  -1000,	-1000,	0,		10.05f, 0.23f,	1.00f,  -602,	0.020f, 0.00f,0.00f,0.00f,  198,	0.030f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_CARPETTEDHALLWAY \
	{11,	1.9f,	1.000f,	-1000,	-4000,	0,		0.30f,	0.10f,	1.00f,  -1831,	0.002f, 0.00f,0.00f,0.00f,	-1630,	0.030f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_HALLWAY \
	{12,	1.8f,	1.000f,	-1000,	-300,	0,		1.49f,	0.59f,	1.00f,  -1219,	0.007f, 0.00f,0.00f,0.00f,  441,	0.011f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_STONECORRIDOR \
	{13,	13.5f,	1.000f,	-1000,	-237,	0,		2.70f,	0.79f,	1.00f,  -1214,	0.013f, 0.00f,0.00f,0.00f,  395,	0.020f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_ALLEY \
	{14,	7.5f,	0.300f,	-1000,	-270,	0,		1.49f,	0.86f,	1.00f,  -1204,	0.007f, 0.00f,0.00f,0.00f,  -4,		0.011f,		0.00f,0.00f,0.00f,	0.125f, 0.950f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_FOREST \
	{15,	38.0f,	0.300f,	-1000,	-3300,	0,		1.49f,	0.54f,	1.00f,  -2560,	0.162f, 0.00f,0.00f,0.00f,	-229,	0.088f,		0.00f,0.00f,0.00f,	0.125f, 1.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_CITY \
	{16,	7.5f,	0.500f,	-1000,	-800,	0,		1.49f,	0.67f,	1.00f,  -2273,	0.007f, 0.00f,0.00f,0.00f,	-1691,	0.011f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_MOUNTAINS \
	{17,	100.0f, 0.270f,	-1000,	-2500,	0,		1.49f,	0.21f,	1.00f,  -2780,	0.300f, 0.00f,0.00f,0.00f,	-1434,	0.100f,		0.00f,0.00f,0.00f,	0.250f, 1.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x1f }
#define EAX30_PRESET_QUARRY \
	{18,	17.5f,	1.000f,	-1000,	-1000,	0,		1.49f,	0.83f,	1.00f,	-10000, 0.061f, 0.00f,0.00f,0.00f,  500,	0.025f,		0.00f,0.00f,0.00f,	0.125f, 0.700f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_PLAIN \
	{19,	42.5f,	0.210f,	-1000,	-2000,	0,		1.49f,	0.50f,	1.00f,  -2466,	0.179f, 0.00f,0.00f,0.00f,	-1926,	0.100f,		0.00f,0.00f,0.00f,	0.250f, 1.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_PARKINGLOT \
	{20,	8.3f,	1.000f,	-1000,	0,		0,		1.65f,	1.50f,	1.00f,  -1363,	0.008f, 0.00f,0.00f,0.00f,	-1153,	0.012f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x1f }
#define EAX30_PRESET_SEWERPIPE \
	{21,	1.7f,	0.800f,	-1000,	-1000,	0,		2.81f,	0.14f,	1.00f,	429,	0.014f, 0.00f,0.00f,0.00f,	1023,	0.021f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 0.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_UNDERWATER \
	{22,	1.8f,	1.000f,	-1000,  -4000,	0,		1.49f,	0.10f,	1.00f,  -449,	0.007f, 0.00f,0.00f,0.00f,	1700,	0.011f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 1.180f, 0.348f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x3f }
#define EAX30_PRESET_DRUGGED \
	{23,	1.9f,	0.500f,	-1000,	0,		0,		8.39f,	1.39f,	1.00f,  -115,	0.002f, 0.00f,0.00f,0.00f,  985,	0.030f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 0.250f, 1.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x1f }
#define EAX30_PRESET_DIZZY \
	{24,	1.8f,	0.600f,	-1000,	-400,	0,		17.23f, 0.56f,	1.00f,  -1713,	0.020f, 0.00f,0.00f,0.00f,	-613,	0.030f,		0.00f,0.00f,0.00f,	0.250f, 1.000f, 0.810f, 0.310f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x1f }
#define EAX30_PRESET_PSYCHOTIC \
	{25,	1.0f,	0.500f,	-1000,	-151,	0,		7.56f,	0.91f,	1.00f,  -626,	0.020f, 0.00f,0.00f,0.00f,  774,	0.030f,		0.00f,0.00f,0.00f,	0.250f, 0.000f, 4.000f, 1.000f, -5.0f,  5000.0f,	250.0f, 0.00f,	0x1f }

EAX30LISTENERPROPERTIES eax30_list[]=
{
    EAX30_PRESET_GENERIC         ,
    EAX30_PRESET_PADDEDCELL      ,
    EAX30_PRESET_ROOM            ,
    EAX30_PRESET_BATHROOM        ,
    EAX30_PRESET_LIVINGROOM      ,
    EAX30_PRESET_STONEROOM       ,
    EAX30_PRESET_AUDITORIUM      ,
    EAX30_PRESET_CONCERTHALL     ,
    EAX30_PRESET_CAVE            ,
    EAX30_PRESET_ARENA           ,
    EAX30_PRESET_HANGAR          ,
    EAX30_PRESET_CARPETTEDHALLWAY ,
    EAX30_PRESET_HALLWAY         ,
    EAX30_PRESET_STONECORRIDOR   ,
    EAX30_PRESET_ALLEY           ,
    EAX30_PRESET_FOREST          ,
    EAX30_PRESET_CITY            ,
    EAX30_PRESET_MOUNTAINS       ,
    EAX30_PRESET_QUARRY          ,
    EAX30_PRESET_PLAIN           ,
    EAX30_PRESET_PARKINGLOT      ,
    EAX30_PRESET_SEWERPIPE       ,
    EAX30_PRESET_UNDERWATER      ,
    EAX30_PRESET_DRUGGED         ,
    EAX30_PRESET_DIZZY           ,
    EAX30_PRESET_PSYCHOTIC       ,
    EAX30_PRESET_GENERIC
};

#endif
