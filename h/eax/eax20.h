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


/******************************************************************
*
*  EAX.H - DirectSound3D Environmental Audio Extensions version 2.0
*  Updated July 8, 1999
*
*******************************************************************
*/

#ifndef EAX20_H_INCLUDED
#define EAX20_H_INCLUDED

#pragma pack(push, 4)

// For compatibility with future EAX versions:
// #define DSPROPSETID_EAX_ListenerProperties DSPROPSETID_EAX20_ListenerProperties
// #define DSPROPSETID_EAX_SourceProperties DSPROPSETID_EAX20_BufferProperties

typedef enum
{
    DSPROPERTY_EAX20LISTENER_NONE,
    DSPROPERTY_EAX20LISTENER_ALLPARAMETERS,
    DSPROPERTY_EAX20LISTENER_ROOM,
    DSPROPERTY_EAX20LISTENER_ROOMHF,
    DSPROPERTY_EAX20LISTENER_ROOMROLLOFFFACTOR,
    DSPROPERTY_EAX20LISTENER_DECAYTIME,
    DSPROPERTY_EAX20LISTENER_DECAYHFRATIO,
    DSPROPERTY_EAX20LISTENER_REFLECTIONS,
    DSPROPERTY_EAX20LISTENER_REFLECTIONSDELAY,
    DSPROPERTY_EAX20LISTENER_REVERB,
    DSPROPERTY_EAX20LISTENER_REVERBDELAY,
    DSPROPERTY_EAX20LISTENER_ENVIRONMENT,
    DSPROPERTY_EAX20LISTENER_ENVIRONMENTSIZE,
    DSPROPERTY_EAX20LISTENER_ENVIRONMENTDIFFUSION,
    DSPROPERTY_EAX20LISTENER_AIRABSORPTIONHF,
    DSPROPERTY_EAX20LISTENER_FLAGS
} DSPROPERTY_EAX20_LISTENERPROPERTY;
	
// OR these flags with property id
#define DSPROPERTY_EAX20LISTENER_IMMEDIATE 0x00000000 // changes take effect immediately
#define DSPROPERTY_EAX20LISTENER_DEFERRED  0x80000000 // changes take effect later
#define DSPROPERTY_EAX20LISTENER_COMMITDEFERREDSETTINGS (DSPROPERTY_EAX20LISTENER_NONE | \
                                                       DSPROPERTY_EAX20LISTENER_IMMEDIATE)

// Use this structure for DSPROPERTY_EAXLISTENER_ALLPARAMETERS
// - all levels are hundredths of decibels
// - all times are in seconds
// - the reference for high frequency controls is 5 kHz
//
// NOTE: This structure may change in future EAX versions.
//       It is recommended to initialize fields by name:
//              myListener.lRoom = -1000;
//              myListener.lRoomHF = -100;
//              ...
//              myListener.dwFlags = myFlags /* see EAXLISTENERFLAGS below */ ;
//       instead of:
//              myListener = { -1000, -100, ... , 0x00000009 };
//       If you want to save and load presets in binary form, you 
//       should define your own structure to insure future compatibility.
//
typedef struct _EAX20LISTENERPROPERTIES
{
    long lRoom;                    // room effect level at low frequencies
    long lRoomHF;                  // room effect high-frequency level re. low frequency level
    float flRoomRolloffFactor;     // like DS3D flRolloffFactor but for room effect
    float flDecayTime;             // reverberation decay time at low frequencies
    float flDecayHFRatio;          // high-frequency to low-frequency decay time ratio
    long lReflections;             // early reflections level relative to room effect
    float flReflectionsDelay;      // initial reflection delay time
    long lReverb;                  // late reverberation level relative to room effect
    float flReverbDelay;           // late reverberation delay time relative to initial reflection
    unsigned long dwEnvironment;   // sets all listener properties
    float flEnvironmentSize;       // environment size in meters
    float flEnvironmentDiffusion;  // environment diffusion
    float flAirAbsorptionHF;       // change in level per meter at 5 kHz
    unsigned long dwFlags;         // modifies the behavior of properties
} EAX20LISTENERPROPERTIES, *LPEAX20LISTENERPROPERTIES;

// Used by DSPROPERTY_EAXLISTENER_FLAGS
//
// Note: The number and order of flags may change in future EAX versions.
//       It is recommended to use the flag defines as follows:
//              myFlags = EAXLISTENERFLAGS_DECAYTIMESCALE | EAXLISTENERFLAGS_REVERBSCALE;
//       instead of:
//              myFlags = 0x00000009;
//
// These flags determine what properties are affected by environment size.
#define EAXLISTENERFLAGS_DECAYTIMESCALE        0x00000001 // reverberation decay time
#define EAXLISTENERFLAGS_REFLECTIONSSCALE      0x00000002 // reflection level
#define EAXLISTENERFLAGS_REFLECTIONSDELAYSCALE 0x00000004 // initial reflection delay time
#define EAXLISTENERFLAGS_REVERBSCALE           0x00000008 // reflections level
#define EAXLISTENERFLAGS_REVERBDELAYSCALE      0x00000010 // late reverberation delay time

// This flag limits high-frequency decay time according to air absorption.
#define EAXLISTENERFLAGS_DECAYHFLIMIT          0x00000020
 
#define EAX20LISTENERFLAGS_RESERVED              0xFFFFFFC0 // reserved future use

// property ranges and defaults:

#define EAXLISTENER_MINROOM                       (-10000)
#define EAXLISTENER_MAXROOM                       0
#define EAXLISTENER_DEFAULTROOM                   (-1000)

#define EAXLISTENER_MINROOMHF                     (-10000)
#define EAXLISTENER_MAXROOMHF                     0
#define EAXLISTENER_DEFAULTROOMHF                 (-100)

#define EAXLISTENER_MINROOMROLLOFFFACTOR          0.0f
#define EAXLISTENER_MAXROOMROLLOFFFACTOR          10.0f
#define EAXLISTENER_DEFAULTROOMROLLOFFFACTOR      0.0f

#define EAXLISTENER_MINDECAYTIME                  0.1f
#define EAXLISTENER_MAXDECAYTIME                  20.0f
#define EAXLISTENER_DEFAULTDECAYTIME              1.49f

#define EAXLISTENER_MINDECAYHFRATIO               0.1f
#define EAXLISTENER_MAXDECAYHFRATIO               2.0f
#define EAXLISTENER_DEFAULTDECAYHFRATIO           0.83f

#define EAXLISTENER_MINREFLECTIONS                (-10000)
#define EAXLISTENER_MAXREFLECTIONS                1000
#define EAXLISTENER_DEFAULTREFLECTIONS            (-2602)

#define EAXLISTENER_MINREFLECTIONSDELAY           0.0f
#define EAXLISTENER_MAXREFLECTIONSDELAY           0.3f
#define EAXLISTENER_DEFAULTREFLECTIONSDELAY       0.007f

#define EAXLISTENER_MINREVERB                     (-10000)
#define EAXLISTENER_MAXREVERB                     2000
#define EAXLISTENER_DEFAULTREVERB                 200

#define EAXLISTENER_MINREVERBDELAY                0.0f
#define EAXLISTENER_MAXREVERBDELAY                0.1f
#define EAXLISTENER_DEFAULTREVERBDELAY            0.011f

#define EAXLISTENER_MINENVIRONMENT                0
#define EAXLISTENER_MAXENVIRONMENT                (EAX_ENVIRONMENT_COUNT-1)
#define EAXLISTENER_DEFAULTENVIRONMENT            EAX_ENVIRONMENT_GENERIC

#define EAXLISTENER_MINENVIRONMENTSIZE            1.0f
#define EAXLISTENER_MAXENVIRONMENTSIZE            100.0f
#define EAXLISTENER_DEFAULTENVIRONMENTSIZE        7.5f

#define EAXLISTENER_MINENVIRONMENTDIFFUSION       0.0f
#define EAXLISTENER_MAXENVIRONMENTDIFFUSION       1.0f
#define EAXLISTENER_DEFAULTENVIRONMENTDIFFUSION   1.0f

#define EAXLISTENER_MINAIRABSORPTIONHF            (-100.0f)
#define EAXLISTENER_MAXAIRABSORPTIONHF            0.0f
#define EAXLISTENER_DEFAULTAIRABSORPTIONHF        (-5.0f)

#define EAXLISTENER_DEFAULTFLAGS                  (EAXLISTENERFLAGS_DECAYTIMESCALE |        \
                                                   EAXLISTENERFLAGS_REFLECTIONSSCALE |      \
                                                   EAXLISTENERFLAGS_REFLECTIONSDELAYSCALE | \
                                                   EAXLISTENERFLAGS_REVERBSCALE |           \
                                                   EAXLISTENERFLAGS_REVERBDELAYSCALE |      \
                                                   EAXLISTENERFLAGS_DECAYHFLIMIT)



// For compatibility with future EAX versions:
// #define DSPROPSETID_EAX_BufferProperties DSPROPSETID_EAX20_BufferProperties

typedef enum
{
    DSPROPERTY_EAX20BUFFER_NONE,
    DSPROPERTY_EAX20BUFFER_ALLPARAMETERS,
    DSPROPERTY_EAX20BUFFER_DIRECT,
    DSPROPERTY_EAX20BUFFER_DIRECTHF,
    DSPROPERTY_EAX20BUFFER_ROOM,
    DSPROPERTY_EAX20BUFFER_ROOMHF, 
    DSPROPERTY_EAX20BUFFER_ROOMROLLOFFFACTOR,
    DSPROPERTY_EAX20BUFFER_OBSTRUCTION,
    DSPROPERTY_EAX20BUFFER_OBSTRUCTIONLFRATIO,
    DSPROPERTY_EAX20BUFFER_OCCLUSION, 
    DSPROPERTY_EAX20BUFFER_OCCLUSIONLFRATIO,
    DSPROPERTY_EAX20BUFFER_OCCLUSIONROOMRATIO,
    DSPROPERTY_EAX20BUFFER_OUTSIDEVOLUMEHF,
    DSPROPERTY_EAX20BUFFER_AIRABSORPTIONFACTOR,
    DSPROPERTY_EAX20BUFFER_FLAGS
} DSPROPERTY_EAX20_BUFFERPROPERTY;    

// OR these flags with property id
#define DSPROPERTY_EAXBUFFER_IMMEDIATE 0x00000000 // changes take effect immediately
#define DSPROPERTY_EAXBUFFER_DEFERRED  0x80000000 // changes take effect later
#define DSPROPERTY_EAXBUFFER_COMMITDEFERREDSETTINGS (DSPROPERTY_EAXBUFFER_NONE | \
                                                     DSPROPERTY_EAXBUFFER_IMMEDIATE)

// Use this structure for DSPROPERTY_EAXBUFFER_ALLPARAMETERS
// - all levels are hundredths of decibels
//
// NOTE: This structure may change in future EAX versions.
//       It is recommended to initialize fields by name:
//              myBuffer.lDirect = 0;
//              myBuffer.lDirectHF = -200;
//              ...
//              myBuffer.dwFlags = myFlags /* see EAXBUFFERFLAGS below */ ;
//       instead of:
//              myBuffer = { 0, -200, ... , 0x00000003 };
//
typedef struct _EAX20BUFFERPROPERTIES
{
    long lDirect;                // direct path level
    long lDirectHF;              // direct path level at high frequencies
    long lRoom;                  // room effect level
    long lRoomHF;                // room effect level at high frequencies
    float flRoomRolloffFactor;   // like DS3D flRolloffFactor but for room effect
    long lObstruction;           // main obstruction control (attenuation at high frequencies) 
    float flObstructionLFRatio;  // obstruction low-frequency level re. main control
    long lOcclusion;             // main occlusion control (attenuation at high frequencies)
    float flOcclusionLFRatio;    // occlusion low-frequency level re. main control
    float flOcclusionRoomRatio;  // occlusion room effect level re. main control
    long lOutsideVolumeHF;       // outside sound cone level at high frequencies
    float flAirAbsorptionFactor; // multiplies DSPROPERTY_EAXLISTENER_AIRABSORPTIONHF
    unsigned long dwFlags;       // modifies the behavior of properties
} EAX20BUFFERPROPERTIES, *LPEAX20BUFFERPROPERTIES;

// Used by DSPROPERTY_EAXBUFFER_FLAGS
//    TRUE:    value is computed automatically - property is an offset
//    FALSE:   value is used directly
//
// Note: The number and order of flags may change in future EAX versions.
//       To insure future compatibility, use flag defines as follows:
//              myFlags = EAXBUFFERFLAGS_DIRECTHFAUTO | EAXBUFFERFLAGS_ROOMAUTO;
//       instead of:
//              myFlags = 0x00000003;
//
#define EAXBUFFERFLAGS_DIRECTHFAUTO 0x00000001 // affects DSPROPERTY_EAXBUFFER_DIRECTHF
#define EAXBUFFERFLAGS_ROOMAUTO     0x00000002 // affects DSPROPERTY_EAXBUFFER_ROOM
#define EAXBUFFERFLAGS_ROOMHFAUTO   0x00000004 // affects DSPROPERTY_EAXBUFFER_ROOMHF

#define EAXBUFFERFLAGS_RESERVED     0xFFFFFFF8 // reserved future use

// property ranges and defaults:

#define EAXBUFFER_MINDIRECT                  (-10000)
#define EAXBUFFER_MAXDIRECT                  1000
#define EAXBUFFER_DEFAULTDIRECT              0

#define EAXBUFFER_MINDIRECTHF                (-10000)
#define EAXBUFFER_MAXDIRECTHF                0
#define EAXBUFFER_DEFAULTDIRECTHF            0

#define EAXBUFFER_MINROOM                    (-10000)
#define EAXBUFFER_MAXROOM                    1000
#define EAXBUFFER_DEFAULTROOM                0

#define EAXBUFFER_MINROOMHF                  (-10000)
#define EAXBUFFER_MAXROOMHF                  0
#define EAXBUFFER_DEFAULTROOMHF              0

#define EAXBUFFER_MINROOMROLLOFFFACTOR       0.0f
#define EAXBUFFER_MAXROOMROLLOFFFACTOR       10.f
#define EAXBUFFER_DEFAULTROOMROLLOFFFACTOR   0.0f

#define EAXBUFFER_MINOBSTRUCTION             (-10000)
#define EAXBUFFER_MAXOBSTRUCTION             0
#define EAXBUFFER_DEFAULTOBSTRUCTION         0

#define EAXBUFFER_MINOBSTRUCTIONLFRATIO      0.0f
#define EAXBUFFER_MAXOBSTRUCTIONLFRATIO      1.0f
#define EAXBUFFER_DEFAULTOBSTRUCTIONLFRATIO  0.0f

#define EAXBUFFER_MINOCCLUSION               (-10000)
#define EAXBUFFER_MAXOCCLUSION               0
#define EAXBUFFER_DEFAULTOCCLUSION           0

#define EAXBUFFER_MINOCCLUSIONLFRATIO        0.0f
#define EAXBUFFER_MAXOCCLUSIONLFRATIO        1.0f
#define EAXBUFFER_DEFAULTOCCLUSIONLFRATIO    0.25f

#define EAXBUFFER_MINOCCLUSIONROOMRATIO      0.0f
#define EAXBUFFER_MAXOCCLUSIONROOMRATIO      10.0f
#define EAX20BUFFER_DEFAULTOCCLUSIONROOMRATIO  0.5f

#define EAXBUFFER_MINOUTSIDEVOLUMEHF         (-10000)
#define EAXBUFFER_MAXOUTSIDEVOLUMEHF         0
#define EAXBUFFER_DEFAULTOUTSIDEVOLUMEHF     0

#define EAXBUFFER_MINAIRABSORPTIONFACTOR     0.0f
#define EAXBUFFER_MAXAIRABSORPTIONFACTOR     10.0f
#define EAXBUFFER_DEFAULTAIRABSORPTIONFACTOR 1.0f

#define EAX20BUFFER_DEFAULTFLAGS               (EAXBUFFERFLAGS_DIRECTHFAUTO | \
                                              EAXBUFFERFLAGS_ROOMAUTO |     \
                                              EAXBUFFERFLAGS_ROOMHFAUTO)

// Material transmission presets
// 3 values in this order:
//     1: occlusion (or obstruction)
//     2: occlusion LF Ratio (or obstruction LF Ratio)
//     3: occlusion Room Ratio

// Single window material preset
#define EAX_MATERIAL_SINGLEWINDOW          (-2800)
#define EAX_MATERIAL_SINGLEWINDOWLF        0.71f
#define EAX_MATERIAL_SINGLEWINDOWROOMRATIO 0.43f

// Double window material preset
#define EAX_MATERIAL_DOUBLEWINDOW          (-5000)
#define EAX_MATERIAL_DOUBLEWINDOWHF        0.40f
#define EAX_MATERIAL_DOUBLEWINDOWROOMRATIO 0.24f

// Thin door material preset
#define EAX_MATERIAL_THINDOOR              (-1800)
#define EAX_MATERIAL_THINDOORLF            0.66f
#define EAX_MATERIAL_THINDOORROOMRATIO     0.66f

// Thick door material preset
#define EAX_MATERIAL_THICKDOOR             (-4400)
#define EAX_MATERIAL_THICKDOORLF           0.64f
#define EAX_MATERIAL_THICKDOORROOMRTATION  0.27f

// Wood wall material preset
#define EAX_MATERIAL_WOODWALL              (-4000)
#define EAX_MATERIAL_WOODWALLLF            0.50f
#define EAX_MATERIAL_WOODWALLROOMRATIO     0.30f

// Brick wall material preset
#define EAX_MATERIAL_BRICKWALL             (-5000)
#define EAX_MATERIAL_BRICKWALLLF           0.60f
#define EAX_MATERIAL_BRICKWALLROOMRATIO    0.24f

// Stone wall material preset
#define EAX_MATERIAL_STONEWALL             (-6000)
#define EAX_MATERIAL_STONEWALLLF           0.68f
#define EAX_MATERIAL_STONEWALLROOMRATIO    0.20f

// Curtain material preset
#define EAX_MATERIAL_CURTAIN               (-1200)
#define EAX_MATERIAL_CURTAINLF             0.15f
#define EAX_MATERIAL_CURTAINROOMRATIO      1.00f


#pragma pack(pop)

#endif
