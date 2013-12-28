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

// ----------------------------------------------------------
//  global defines shared between kernel and user-level APIs
//  no OS-dependent part here
// ----------------------------------------------------------
//  do not include this file directly, use 'kxapi.h' instead


#ifndef _IKX_H_
#define _IKX_H_

    #define MAX_ASIO_OUTPUTS    16
    #define MAX_ASIO_INPUTS 16

    // default routings (ndx for get/set_routing())
    #define DEF_WAVE01_ROUTING      0x0
    #define DEF_AC3_LEFT_ROUTING    0x1
    #define DEF_AC3_CENTER_ROUTING  0x2
    #define DEF_AC3_RIGHT_ROUTING   0x3
    #define DEF_AC3_SLEFT_ROUTING   0x4
    #define DEF_AC3_SRIGHT_ROUTING  0x5
    #define DEF_AC3_SUBWOOFER_ROUTING   0x6
    #define DEF_WAVE23_ROUTING      0x7
    #define DEF_WAVE45_ROUTING      0x8
    #define DEF_WAVE67_ROUTING      0x9
    #define DEF_AC3PASSTHROUGH_ROUTING  0xa
    #define DEF_ASIO_ROUTING        0xb
    #define DEF_SYNTH1_ROUTING      (DEF_ASIO_ROUTING+MAX_ASIO_OUTPUTS)
    #define DEF_SYNTH2_ROUTING      (DEF_SYNTH1_ROUTING+16)
    #define DEF_AC3_SCENTER_ROUTING (DEF_SYNTH2_ROUTING+16)
    #define DEF_3D_LEFT_ROUTING     (DEF_AC3_SCENTER_ROUTING+1)
    #define DEF_3D_RIGHT_ROUTING    (DEF_3D_LEFT_ROUTING+1)
    #define DEF_3D_TOP_ROUTING      (DEF_3D_RIGHT_ROUTING+1)
    // #define DEF_3D_BOTTOM_ROUTING    (DEF_3D_TOP_ROUTING+1) // FIXME

    #define ROUTING_LAST (DEF_3D_TOP_ROUTING)

    // default amounts (ndx for get/set_amount())
    #define DEF_SYNTH_AMOUNT_C      0x0
    #define DEF_SYNTH_AMOUNT_D      0x1
    #define DEF_SYNTH_AMOUNT_E      0x2
    #define DEF_SYNTH_AMOUNT_F      0x3
    #define DEF_SYNTH_AMOUNT_G      0x4
    #define DEF_SYNTH_AMOUNT_H      0x5
    #define MAX_DEF_AMOUNT      0x5

    #define AMOUNT_LAST (MAX_DEF_AMOUNT)

    #define KX_MAKE_ROUTING(a,b,c,d) (dword)((dword)((a)&0xff)+((dword)((b)&0xff)<<8)+((dword)((c)&0xff)<<16)+((dword)((d)&0xff)<<24))
    #define KX_MAKE_XROUTING(a,b,c,d) KX_MAKE_ROUTING(a,b,c,d)
    #define KX_GET_SEND_A(a) (dword)((a)&0xff)
    #define KX_GET_SEND_B(a) (dword)(((a)&0xff00)>>8)
    #define KX_GET_SEND_C(a) (dword)(((a)&0xff0000)>>16)
    #define KX_GET_SEND_D(a) (dword)(((a)&0xff000000)>>24)
    #define KX_GET_SEND_E(a) KX_GET_SEND_A(a)
    #define KX_GET_SEND_F(a) KX_GET_SEND_B(a)
    #define KX_GET_SEND_G(a) KX_GET_SEND_C(a)
    #define KX_GET_SEND_H(a) KX_GET_SEND_D(a)

    #define AC3_LEFT        0
    #define AC3_CENTER      1
    #define AC3_RIGHT       2
    #define AC3_SLEFT       3
    #define AC3_SRIGHT      4
    #define AC3_SUBWOOFER   5

    // params for set/get_buffers
    #define KX_TANKMEM_BUFFER   0
    #define KX_PLAYBACK_BUFFER  1
    #define KX_RECORD_BUFFER    2
    #define KX_AC3_BUFFERS  3   // number of, not size
    #define KX_GSIF_BUFFER  4   // in samples 32..2048

    // params for get_string
    #define KX_STR_DRIVER_NAME      0
    #define KX_STR_DRIVER_VERSION   1
    #define KX_STR_CARD_NAME        2
    #define KX_STR_AC97CODEC_NAME   3
    #define KX_STR_AC973D_NAME      4
    #define KX_STR_DRIVER_DATE      5
    #define KX_STR_DB_NAME      6

    // params for get_dword
    #define KX_DWORD_CARD_PORT      0
    #define KX_DWORD_CARD_IRQ       1
    #define KX_DWORD_AC97_CAPS      2
    #define KX_DWORD_AC97_ID1       3
    #define KX_DWORD_AC97_ID2       4
    #define KX_DWORD_AC97_IS20      5
    #define KX_DWORD_AC97_EXTID     6
    #define KX_DWORD_AC97_PRESENT   7
    #define KX_DWORD_IS_51      8   // any '5.1' model
    #define KX_DWORD_IS_APS     9
    #define KX_DWORD_IS_10K2        10  // audigy & audigy2
    #define KX_DWORD_HAVE_MPU       11
    #define KX_DWORD_SUBSYS     12
    #define KX_DWORD_CHIPREV        13
    #define KX_DWORD_DEVICE     14
    #define KX_DWORD_IS_A2      15  // audigy2
    #define KX_DWORD_IS_A2EX        16  // audigy2 platinum ex
    #define KX_DWORD_CAN_PASSTHRU   17  // supports ac-3 passthru / direct playback
    #define KX_DWORD_IS_K8      18  // a2 value; a4
    #define KX_DWORD_IS_EDSP        19  // edsp
    #define KX_DWORD_IS_BAD_SB22X   20
    #define KX_DWORD_IS_A2ZSNB      21
    #define KX_DWORD_IS_A4          22  // a4 value
    #define KX_DWORD_IS_CARDBUS     23  // a4 value
    #define KX_DWORD_CAN_K8_PASSTHRU 24 // does not have 0x80008000 DSP issue [usually 10k8]

    // ids for get_hw_parameter
    #define KX_HW_DOO           0
    #define KX_HW_SPDIF_FREQ        2 // parameter: 0-44100 1-48000 2-96000
    #define KX_HW_SPDIF_BYPASS      3 
    #define KX_HW_SWAP_FRONT_REAR   4
    #define KX_HW_ROUTE_PH_TO_CSW   5
    #define KX_HW_AC3_PASSTHROUGH   6
    #define KX_HW_ECARD_ROUTING     7 // low 16 bit: spdif0, high 16 bit: spdif1
    #define KX_HW_ECARD_ADC_GAIN    8 // low 8 bit: left; high 8 bit: right
//    #define KX_HW_SLIDER_PGM      9 // outdated; don't use; use 'get_dsp_assignments()' instead
//    #define KX_HW_SLIDER_REGISTER 10 // outdated; don't use
    #define KX_HW_SPDIF_DECODE      11  // decode incoming ac-3 stream (spdif-in)
    #define KX_HW_SPDIF_RECORDING   12  // 1: SPDIFA 2: SPDIFB 3: SPDIFC
                            // 4: i2sa 5: i2sb 6: i2sc
    #define KX_HW_SYNTH_COMPATIBILITY   13  // see KX_SYNTH_COMPAT_...
    #define KX_HW_KX3D          14  // kX 3-D compatibility flag [internal/debug]
                            // any bit set to '1' disables corresponding 3D feature
    #define KX_HW_COMPAT        15  // driver compatibility flag
        #define KX_HW_WAVE_PAN          0x1
        #define KX_HW_WAVE_INTERPROM        0x2
        #define KX_HW_WAVE_MULTICHANNEL_COMPAT  0x4
        #define KX_HW_WAVE_DONT_SMOOTH      0x8 // smooth/interpolate fxbus amounts
                                // note: '0' means 'yes' (compatibility issue)
        #define KX_HW_WAVE_EAX_ALL      0x10    // filters all EAX requests
        #define KX_HW_FORCE_XTRAM_PT        0x20    // force XTRAM-based ac-3 passthru method
        // #define KX_HW_FORCE_PT_SWAP      0x40    // swap L&R due to spdif issue [a2] - outdated since 3543
        #define KX_HW_NO_SPDIF_NOTIFICATION 0x80    // disable S/PDIF notification
        // #define KX_HW_P16V_FOLLOW_EPILOG    0x100   // p16v volumes follow epilog settings
        // #define KX_HW_ASIO_NOWAIT        0x200   // disables kernel-level KeDelayExecutionThread [for SMP only!]
        #define KX_HW_SB22x         0x400   // enable sb22x hack
        #define KX_HW_NO_UART_ROUTING       0x800   // disable UART routing
        #define KX_HW_P16V_FOLLOW       0x1000  // p16v i2s follows wave hq [3538j, 0x400 bit EHC]
        #define KX_HW_FORCE_SPDIF_SWAP  0x2000  // similar to KX_HW_FORCE_PT_SWAP, but for epilog
        #define KX_HW_ALT_CHN_ALLOCATION    0x4000  // disables circular channel allocation
        #define KX_HW_DISABLE_PCI_NOTIFY    0x8000  // disables notifications about PCI bus error
    #define KX_HW_P16V_PB_ROUTING   16  // see P16V_ROUTE_...
    #define KX_HW_P16V_REC_ROUTING  17  // see P16V_REC_...
//    #define KX_HW_KX3D_SPSET      18  // OUTDATED
    #define KX_HW_K2_AC97       19  // GPIO 0x80 flag: ac97->front + ac97 master volume
                            // (used for ac97.2 recording...)
    #define KX_HW_8PS           20
      // flags:
      // 0: default [w/o W/E/S] 
      // 1: 8ps on (full 8-ps) -- turned on by Surrounder+ for 6.1, 7.1 and headphones preset
      #define KX_HW_8PS_ON  1
    #define KX_HW_DRUM_CHANNEL      21
    #define KX_HW_A2ZSNB_SOURCE     22
      #define KX_ZSNB_LINEIN    0
      #define KX_ZSNB_MICIN 1
      // #define KX_ZSNB_SPDIFIN    1
      // #define KX_ZSNB_WUH    3
    #define KX_HW_LAST          22

    // synth compatibility flags
    #define KX_SYNTH_COMPAT_HOLD        1       // per specs
    #define KX_SYNTH_COMPAT_ATTN        2       // per CL
    #define KX_SYNTH_COMPAT_842         4       // per specs (sf2.01: article 8.4.2)
    #define KX_SYNTH_COMPAT_LEG_MOD     8       // (not default; a-la <=3531)
    #define KX_SYNTH_COMPAT_FREQ        0x10        // per 8010
    #define KX_SYNTH_COMPAT_LFOVOL      0x20        // per 8010
    #define KX_SYNTH_COMPAT_LEG_ATT     0x40        // (not default; a-la <=3531)
    #define KX_SYNTH_COMPAT_LEG_REL     0x80        // (not default; a-la <=3531)
    #define KX_SYNTH_COMPAT_REAL_RESONANCE  0x100       // realtime resonance (filterQ)
    #define KX_SYNTH_COMPAT_REAL_FC     0x200       // realtime filter cutoff
    #define KX_SYNTH_COMPAT_BANK        0xc00       // mask:
        #define KX_SYNTH_COMPAT_BANK_NORMAL 0
        #define KX_SYNTH_COMPAT_BANK_LSB    0x400
        #define KX_SYNTH_COMPAT_BANK_MSB    0x800
        #define KX_SYNTH_COMPAT_BANK_LSB_MSB    0xc00

    #define KX_SYNTH_DEFAULTS (KX_SYNTH_COMPAT_HOLD|KX_SYNTH_COMPAT_ATTN|KX_SYNTH_COMPAT_842|KX_SYNTH_COMPAT_LFOVOL|KX_SYNTH_COMPAT_FREQ|KX_SYNTH_COMPAT_REAL_FC)


 // 'usage' flags (mainly for get_voice_info(...))
 #define VOICE_FLAGS_MONO       0x00
 #define VOICE_FLAGS_8BIT       0x00

 #define VOICE_FLAGS_STEREO     0x100
 #define VOICE_FLAGS_16BIT      0x200

 // voice usage flags
 #define VOICE_USAGE_FREE       0x1
 #define VOICE_USAGE_MIDI       0x2
 #define VOICE_USAGE_PLAYBACK   0x4
 #define VOICE_USAGE_RELEASING  0x8
 #define VOICE_USAGE_ASIO       0x10
 #define VOICE_USAGE_TEMP       0x20

// legacy:
// #define KX_MAX_NAME          KX_MAX_STRING
// it is no longer used, use KX_MAX_STRING instead
#if !defined(KX_MAX_STRING)
    #define KX_MAX_STRING 128
#endif

 #define voice_usage(a) ((a)&0xff)

 // internal flags; also for Direct kX interface
 #define VOICE_OPEN_NOTIMER     0x1000
 #define VOICE_OPEN_NOTIFY      0x10000

 // midi/synth
 #define VOICE_SYNTH_SUSTAINED      0x2000
 #define VOICE_SYNTH_SOSTENUTO      0x4000
 #define VOICE_SYNTH_SOSTENUTO_FINISHED 0x8000

 // for user-level only
 #define VOICE_STATUS_AC3       0x20000
 #define VOICE_STATUS_BUFFERED  0x40000
 #define VOICE_STATUS_24BIT     0x80000
 #define VOICE_STATUS_AC3PT     0x100000

 // parameter for translate_microcode()
 #define KX_MICROCODE_ANY   0
 #define KX_MICROCODE_AFTER 1   // after pgm_pos or the last, if pgm_pos==0;
 #define KX_MICROCODE_BEFORE    2   // before pgm_pos or the first, if pgm_pos==0;
 #define KX_MICROCODE_ABSOLUTE  3   // for save/restore purposes only

 // valid flags: for write_instr
 #define VALID_OP   1
 #define VALID_R    2
 #define VALID_A    4
 #define VALID_X    8
 #define VALID_Y    0x10
 #define VALID_OPERANDS (VALID_R|VALID_A|VALID_Y|VALID_W)

 // 'what' for set_microcode_name()
 #define KX_MICROCODE_NAME  0
 #define KX_MICROCODE_COPYRIGHT 1
 #define KX_MICROCODE_ENGINE    2
 #define KX_MICROCODE_CREATED   3
 #define KX_MICROCODE_COMMENT   4
 #define KX_MICROCODE_GUID  5

 // 'flags' for update_microcode
 #define IKX_UPDATE_NAME        1
 #define IKX_UPDATE_COPYRIGHT       2
 #define IKX_UPDATE_ENGINE      4
 #define IKX_UPDATE_CREATED     8
 #define IKX_UPDATE_GUID        0x10
 #define IKX_UPDATE_COMMENT     0x20

 #define IKX_UPDATE_STRINGS     (0xff)

 #define IKX_UPDATE_CODE        0x100
 #define IKX_UPDATE_REGS        0x200
 #define IKX_UPDATE_RESOURCES       0x400

 #define IKX_UPDATE_DSP         (0xff00)

 #define IKX_UPDATE_ALL         (0xffffffff)


 // parameters for audio_set_parameter / audio_get_parameter
#define KX_VOICE_VOLUME     0x1
#define KX_VOICE_PITCH      0x2
#define KX_VOICE_PAN        0x3
#define KX_VOICE_ROUTINGS   0x4
#define KX_VOICE_XROUTINGS  0x5
#define KX_VOICE_SEND_A     0x6
#define KX_VOICE_SEND_B     0x7
#define KX_VOICE_SEND_C     0x8
#define KX_VOICE_SEND_D     0x9
#define KX_VOICE_SEND_E     0xa
#define KX_VOICE_SEND_F     0xb
#define KX_VOICE_SEND_G     0xc
#define KX_VOICE_SEND_H     0xd
#define KX_VOICE_UPDATE     0x10000

#define KX_MIN_RATE 100
#define KX_MAX_RATE 191999

// All volumes are: max: 0          0   dB
//          min: FF9C0000h  -100 dB * 0x10000
//          step: 28fh      0.01 dB * 0x10000
#define KX_MAX_VOLUME   0x0U
#define KX_MIN_VOLUME   0xFF9C0000U
#define KX_VOLUME_STEP  0x28fU

typedef struct
{
 dword usage;
 dword cur_vol;
}kx_voice_info;

typedef struct
{
    dword control_bits;
    dword adc_gain;
    dword spdif1;
    dword spdif0;
}kx_ecard_state;

typedef struct
{
  struct spdif_t
  {
   // spdif inputs / sampling rates
   dword channel_status_a; // CDCS
   dword srt_status_a;     // CDSRCS
   dword channel_status_b; // GPSCS
   dword srt_status_b;     // GPSRCS
   dword channel_status_c; // GP2SCS [10k2 only]
   dword srt_status_c;     // ZVSRCS

   // 10k2.2 only: [ext. spdif status]
   dword channel_status_a_x; // CDCSx
   dword channel_status_b_x; // GPSCSx
   dword channel_status_c_x; // GP2SCSx

   // spdif outputs
   dword scs0,scs1,scs2;
   dword scs0x,scs1x,scs2x; // [10k2.2 only] -- [ext. spdif status]

   // sampling rate (as for KX_HW_SPDIF_FREQ call)
   int spo_sr;
  }spdif;
  struct i2s_t
  {
   // i2s inputs
   dword srt_status_0;
   dword srt_status_1;
   dword srt_status_2;
  }i2s;

  dword p16v;   // internal: p16v recording status
}kx_spdif_i2s_status;

typedef struct
{
 int level; // currently supported:
 #define MAX_MIXER_CONTROLS  6 // if particular pgm[0] is '\0', mixer control is not used
 #define MIXER_MASTER    0
 #define MIXER_REC       1
 #define MIXER_KX0       2
 #define MIXER_KX1       3
 #define MIXER_SYNTH     4 // if pgm[0] is '\0', use synth-based volume control
 #define MIXER_WAVE      5

 char pgm[KX_MAX_STRING];
 char reg_left[KX_MAX_STRING];
 char reg_right[KX_MAX_STRING];
 int max_vol; // 0x7fffffff or 0x20000000
}kx_assignment_info;

typedef struct
{
 int what;

 #define KX_TEXT2ID 0
 #define KX_ID2TEXT 1

 int pgm;
 char pgm_name[KX_MAX_STRING];
 word reg;
 char reg_name[KX_MAX_STRING];
}kx_text2id;

typedef struct
{
    int core;
    int bps;
    int sample_rate;

    int n_outs,n_ins;

    char friendly_name[KX_MAX_STRING];

    int min_bufsize,max_bufsize,def_bufsize;    // in samples
}kx_caps;

#define KX_CAPS_10K1        1
#define KX_CAPS_10K2        2
#define KX_CAPS_P16V        3
#define KX_CAPS_P17V        4
#define KX_CAPS_EDSP        5

#define KX_EC_SPDIF1_SHIFT  15  // 0x18000  d1 - GPINPUT1 - dsp In 0x12/13
#define KX_EC_SPDIF0_SHIFT  17  // 0xe0000  d2 - GPINPUT0 - dsp In 0x16/17

#define KX_EC_SOURCE_ECARD      0
#define KX_EC_SOURCE_EDRIVE     1
// and what is 2? FIXME
#define KX_EC_SOURCE_CDROM      4

// It is impossible to assign KX_EC_SPDIF1_IN1 to KX_EC_SOURCE_CDROM

// p16v routings:
 // [low bits]
 // enable 10k2 routings:
 #define P16V_ROUTE_SPDIF   1
 #define P16V_ROUTE_I2S     2
 // enable p16v routings:
 #define P16V_ROUTE_P_SPDIF 4
 #define P16V_ROUTE_P_I2S   8
 // [high bits]
 #define P16V_PB_DEF_ROUTE_MASK 0xf00
 // 0x0 - 0/1
 // 0x100 - 2/3
 // etc..

// rec source: low bits:
 #define P16V_REC_SPDIF 0
 #define P16V_REC_I2S   1
 #define P16V_REC_WUH   2   // 'what you hear'
 // high bits: (&0xf0) -- particular i2s/spdif source (0..2)
 #define P16V_REC_0 0x00
 #define P16V_REC_1 0x10
 #define P16V_REC_2 0x20
 // #define P16V_REC_3  0x30

// parameters for ikx->get_description() -- what
                // p:
#define KX_CARD_NAME    0   // subsys_id
#define KX_EPILOG_NAME  1   // epilog reg id (0x4000...)
#define KX_PROLOG_NAME  2   // prolog reg id (0x8000...)
#define KX_INPUT_NAME   3   // KX_IN(0x0..0xf) -- per I/O assignment
#define KX_OUTPUT_NAME  4   // KX_OUT(0x0..0x31) -- per I/O assignment

#define KX_HW_NAME  5   
 // 'p' for 'KX_HW_NAME':
 // the function will return non-zero if the input/output isn't present
 // these values might become localized in the future 
 // -- do not assume presence of any English tokens (such as 'In' or 'Left')
 #define KX_IN_SPDIFA   0x00    // KX_IN(2,3)
 #define KX_IN_SPDIFB   0x01    // KX_IN(6,7)
 #define KX_IN_SPDIFC   0x02    // KX_IN(4,5)
 #define KX_IN_I2S0 0x10    // KX_IN(8,9)
 #define KX_IN_I2S1 0x11    // KX_IN(a,b)
 #define KX_IN_I2S2 0x12    // KX_IN(c,d)
 #define KX_IN_AC97 0x20    // KX_IN(0,1) note: this is a1/a2 for the APS board!

 #define KX_OUT_SPDIF0  0x1000  // KX_OUT(2,3)
 #define KX_OUT_SPDIF1  0x1001  // KX_OUT(4,5)
 #define KX_OUT_SPDIF2  0x1002  // KX_OUT(6,7)
 #define KX_OUT_SPDIF3  0x1003  // KX_OUT(28,29) - 10k2 only
 #define KX_OUT_I2S0    0x1010  // KX_OUT(0,1)
                // shared with AC97_0 on 10k1; so, always use 'i2s0' for 'analog front'
 #define KX_OUT_I2S1    0x1011  // KX_OUT(11,12)
                // shared with AC97_1 on 10k1; so, always use 'i2s1' for 'analog center/lfe'
 #define KX_OUT_I2S2    0x1012  // KX_OUT(30,31) -- 10k2 only
 #define KX_OUT_I2S3    0x1013  // KX_OUT(8,9)
                // note: 10k1 uses one output for both analog & digital rear
                // 10k2 uses i2s3 & spdif3 (see above)
 #define KX_OUT_REC 0x2000

// kX kernel <-> user notification SysEx

// Creative-like messages:
// -----------------------

// generic remote message:
//   \xf0\x00\x20\x21\x60\x00\x01\x0a\x41\x44\x??\x??\xf7
//   \xf0\x00\x20\x21\x60\x00\x01\x09\x41\x44\x??\x??\xf7
// (10k2ex):
//   \xf0\x00\x20\x21\x61\x00\x01\x??\x??\x??\xf7

#define KX_SYSEX_REMOTE_INITIAL     "\xf0\x00\x20\x21\x60\x00\x01"
#define KX_SYSEX_REMOTE_INITIAL_SIZE    7
#define KX_SYSEX_REMOTE_SIZE        13
#define KX_SYSEX_REMOTE2_INITIAL    "\xf0\x00\x20\x21\x61\x00\x01"
#define KX_SYSEX_REMOTE2_INITIAL_SIZE   7
#define KX_SYSEX_REMOTE2_SIZE       11

// \xf0\x00\x20\x21\x60\x00\x01\x09\x41\x44\x76\x09\xf7
// - vol mute from the remote / onboard connector
// \xf0\x00\x20\x21\x60\x00\x01\x09\x41\x44\x46\x39\xf7
// - vol incr from the remote / onboard connector
// \xf0\x00\x20\x21\x60\x00\x01\x0a\x41\x44\x46\x39\xf7
// - vol decr from the remote / onboard connector
#define KX_SYSEX_VOLMUTE "\xf0\x00\x20\x21\x60\x00\x01\x09\x41\x44\x76\x09\xf7"
#define KX_SYSEX_VOLINCR "\xf0\x00\x20\x21\x60\x00\x01\x09\x41\x44\x46\x39\xf7"
#define KX_SYSEX_VOLDECR "\xf0\x00\x20\x21\x60\x00\x01\x0a\x41\x44\x46\x39\xf7"

// GM/GS notification (master volume)
//   F0 7F 7F 04 01 LL MM F7

#define KX_SYSEX_GMGS_VOLUME_INITIAL    "\xf0\x7f\x7f\x04\x01"
#define KX_SYSEX_GMGS_VOLUME_INITIAL_SIZE 5

// kX-specific SysEx messages:
#define KX_SYSEX_SIZE       11
#define KX_SYSEX_INITIAL     "\xf0\x00\x20\x21\x62\x00\x01"
#define KX_SYSEX_INITIAL_SIZE   7

// function/subfunction index in the SysEx message
#define KX_SYSEX_FUNCTION   7
#define KX_SYSEX_SUBFUNCTION    8

#define KX_SYSEX_NOTIFY_F   0xe

#define KX_SYSEX_PCIBUSERROR     "\xf0\x00\x20\x21\x62\x00\x01\x0e\x01\x00\xf7"
#define KX_SYSEX_SPDIF           "\xf0\x00\x20\x21\x62\x00\x01\x0e\x02\x00\xf7"
#define KX_SYSEX_AC3PASSTHRU_ON  "\xf0\x00\x20\x21\x62\x00\x01\x0e\x03\x00\xf7"
#define KX_SYSEX_AC3PASSTHRU_OFF "\xf0\x00\x20\x21\x62\x00\x01\x0e\x04\x00\xf7"
#define KX_SYSEX_AC3DECODE_ON    "\xf0\x00\x20\x21\x62\x00\x01\x0e\x05\x00\xf7"
#define KX_SYSEX_AC3DECODE_OFF   "\xf0\x00\x20\x21\x62\x00\x01\x0e\x06\x00\xf7"
#define KX_SYSEX_START_3D        "\xf0\x00\x20\x21\x62\x00\x01\x0e\x07\x00\xf7"
#define KX_SYSEX_STOP_3D         "\xf0\x00\x20\x21\x62\x00\x01\x0e\x08\x00\xf7"
#define KX_SYSEX_EDSP_CFG        "\xf0\x00\x20\x21\x62\x00\x01\x0e\x09\x00\xf7"

#define KX_SYSEX_GPIO_F     0xf

#define KX_SYSEX_GPIO        "\xf0\x00\x20\x21\x62\x00\x01\x0f\x00\x00\xf7"
#define KX_SYSEX_GPIO_POS1  8
#define KX_SYSEX_GPIO_POS2  9

// ASIO methods (user-level options):
#define KXASIO_METHOD_NOWAIT    0x0    // user/kernel
#define KXASIO_METHOD_SLEEP     0x1    // user/kernel
#define KXASIO_METHOD_THREAD    0x2    // user only
#define KXASIO_METHOD_TIMER     0x4    // user only
#define KXASIO_METHOD_SEND_EVENT    0x8  // send user_event when ready

#define KXASIO_METHOD_WAIT_IN_KERNEL     0x100

#define KXASIO_METHOD_MAP_TOGGLE    0x1000  // map asio_notifications_t instead of asio_get_position();
#define KXASIO_METHOD_USE_HWIRQ     0x2000  // generate hw irq (buffer full, half-buffer full)
#define KXASIO_METHOD_USE_HWTIMER   0x4000  // generate hw timer irq

#define KXASIO_METHOD_OLD   0x10000    // caller provides buffers
#define KXASIO_METHOD_NEW   0x20000    // kernel provides buffers

#define KXASIO_METHOD_DEFAULT   (KXASIO_METHOD_SLEEP|KXASIO_METHOD_NEW)

typedef struct
{
 int asio_method;
 int toggle; // current hardware position
 int pb_buf; // current hardware position
 int rec_buf; // current hardware position
 int n_voice;
 dword semi_buff;
 int active;
 dword cur_pos; // in samples

 void *kevent;  // (PRKEVENT)
}asio_notification_t;

#endif
