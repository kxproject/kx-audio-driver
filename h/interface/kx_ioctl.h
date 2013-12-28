// kX Audio Driver Internal Interface
// Copyright (c) Eugene Gavrilov, 2001-2008.
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


#ifndef _KX_IOCTL_H_
#define _KX_IOCTL_H_

#if defined(__APPLE__)
enum
{
    kOpenUserClient,
    kCloseUserClient,
    kTransaction,
    kNumMethods
};
#endif

// prop_op
#define KX_PROP_GET 0x0100000
#define KX_PROP_SET 0x0200000

#define PROPERTY_MAGIC      (0x99308277&KX_VERSION_DWORD)

// Structures to get/set properties:

#pragma pack(1)

typedef struct // if modified -> check driver_property()
{
 dword magic;
 dword prop;
}kx_property_instance;

// prop items
#define KX_PROP_AC97        0x0
typedef struct
{
 byte reg;  // =reg(out) for enum; if =0 & method=SET -> Reset AC97
 word val;  // =ndx(in) for enum
}ac97_property;

// driver internal variables
#define KX_PROP_GET_STRING  0x10
#define KX_PROP_GET_DWORD   0x11
#define KX_PROP_GET_BUFFERS 0x12
#define KX_PROP_SET_BUFFERS 0x13
typedef struct
{
 int what;
 union
 {
  dword val;
  char str[KX_MAX_STRING];
 };
}get_property;

#define KX_PROP_MUTE            0x20
#define KX_PROP_UNMUTE          0x21

// low-level hardware
#define KX_PROP_FN0             0x30
#define KX_PROP_P16V            0x31
#define KX_PROP_GPIO_GET        0x32
#define KX_PROP_GPIO_SET        0x33
#define KX_PROP_FPGA            0x34
#define KX_PROP_FPGALINK        0x35
#define KX_PROP_PTR             0x40
#define KX_PROP_HW_PARAM        0x50
#define KX_PROP_INITPASSTHRU    0x55
#define KX_PROP_CLOSEPASSTHRU   0x56
#define KX_PROP_SETPASSTHRU     0x57
#define KX_PROP_CLOCK           0x58
typedef struct
{
 dword reg;
 dword val;
 dword chn;
 union
 {
    void *pntr;
    __int64 pntr_padding;
    // note: pntr is used for KX_PROP_INITPASSTHRU / kx_init_passthru, but actually only 32 bits are necessary)
 };
}dword_property;

#define KX_PROP_SPDIF_I2S_STATE 0x60

#define KX_PROP_ROUTING 0x80
#define KX_PROP_AMOUNT  0x81
#define KX_PROP_P16V_VOLUME 0x84
typedef struct
{
 int ndx;
 dword routing; // all send_a,send_b,send_c,send_d routing || send amount (0..ff)
        // routing: aaBBccDD
 dword xrouting; // xrouting: eeFFggHH (10k2 only)
}routing_property;

#define KX_PROP_DSP_REGISTER_NAME   0x200
#define KX_PROP_DSP_REGISTER_ID     0x201
#define KX_PROP_TRAM_ADDR_NAME      0x202
#define KX_PROP_TRAM_ADDR_ID        0x203
#define KX_PROP_TRAM_FLAG_NAME      0x204
#define KX_PROP_TRAM_FLAG_ID        0x205
typedef struct
{
 union
 {
  word id;
  char name[MAX_GPR_NAME];
 };
 dword val;
 int pgm;
}dsp_register_property;

#define KX_PROP_INSTRUCTION_READ    0x207
#define KX_PROP_INSTRUCTION_WRITE   0x208
typedef struct
{
 word z,w,x,y,op;
 int valid;
 int offset;
 int pgm;
}dsp_instruction_property;

#define KX_PROP_MICROCODE       0x210 // should use 'GET' op; result is in 'cmd' field
#define KX_PROP_RESET_MICROCODE 0x211
#define KX_PROP_RESET_SETTINGS  0x213
#define KX_PROP_RESET_DB    0x214
#define KX_PROP_RESET_VOICES    0x215
typedef struct
{
 int cmd;
 #define KX_PROP_MICROCODE_ENABLE   0
 #define KX_PROP_MICROCODE_DISABLE  1
 #define KX_PROP_MICROCODE_TRANSLATE    2
 #define KX_PROP_MICROCODE_UNLOAD   3
 #define KX_PROP_MICROCODE_UNTRANSLATE  4
 #define KX_PROP_MICROCODE_DSP_GO   5
 #define KX_PROP_MICROCODE_DSP_STOP 6
 #define KX_PROP_MICROCODE_DSP_CLEAR    7
 #define KX_PROP_MICROCODE_BYPASS_ON    8
 #define KX_PROP_MICROCODE_BYPASS_OFF   9
 #define KX_PROP_MICROCODE_SET_FLAG 10
 #define KX_PROP_MICROCODE_GET_FLAG 11

 int pgm;
 int p1,p2;
}microcode_property;

#define KX_PROP_MICROCODE_CONNECT_NAME  0x220 // should use 'GET' op;
#define KX_PROP_MICROCODE_CONNECT_ID        0x221 // should use 'GET' op;
#define KX_PROP_MICROCODE_DISCONNECT_NAME   0x222 // should use 'GET' op;
#define KX_PROP_MICROCODE_DISCONNECT_ID     0x223 // should use 'GET' op;
#define KX_PROP_GET_CONNECTIONS 0x224 // 'GET'
#define KX_PROP_GET_CONNECTIONS_QUERY 0x225 // with size=0; param is ignored
typedef struct
{
 int op;    // =0 on input; =result on output
 int pgm1;
 int pgm2;
 union
 {
    word src_w;
    char src_c[MAX_GPR_NAME];
 };
 union
 {
    word dst_w;
    char dst_c[MAX_GPR_NAME];
 };
}microcode_connect_property;

#define KX_PROP_MICROCODE_ENUM_NAME     0x230 // should use 'GET' op:
#define KX_PROP_MICROCODE_ENUM_ID       0x231 // should use 'GET' op;
#define KX_PROP_MICROCODE_ENUM_ALL      0x232 // should use 'GET' op; size!=0; param is 'dsp_microcode'; first dword - size of
#define KX_PROP_MICROCODE_QUERY         0x233 // = with size=0; param is dword
#define KX_PROP_MICROCODE_SET_NAME      0x234 // 'GET'
typedef struct
{
 int pgm;
 char name[KX_MAX_STRING];
 dsp_microcode m;   // .flag contains 'what' for 'set_name'
}microcode_enum_property;

#define KX_PROP_LOAD_MICROCODE      0x300 // should use 'GET' op;
#define KX_PROP_GET_MICROCODE       0x301 // should use 'GET' op;
#define KX_PROP_UPDATE_MICROCODE    0x302 // should use 'GET' op;
#define LOAD_MICROCODE_MAGIC        (0x19283721^PROPERTY_MAGIC)
#define GET_MICROCODE_MAGIC         (0x54758265^PROPERTY_MAGIC)
#define UPDATE_MICROCODE_MAGIC      (0x69752134^PROPERTY_MAGIC)

#define KX_PROP_SOUNDFONT_LOAD      0x500
#define KX_PROP_SOUNDFONT_UNLOAD    0x501
#define KX_PROP_SOUNDFONT_QUERY     0x502
#define KX_PROP_SOUNDFONT_ENUM      0x503
#define KX_PROP_SOUNDFONT_LOAD_SMPL 0x504
#define KX_PROP_SOUNDFONT_PRESET_DESCR  0x505
typedef struct
{
 int size;
}sf_query_property;
typedef struct
{
 int bank,preset;
 char name[20];
}sfpreset_query_property;

#define KX_PROP_SOUNDFONT_SYNTH     0x510
typedef struct
{
 int synth_id;
 dword data;
}kx_synth_data;

#define KX_PROP_VOICE_INFO  0x600
#define KX_PROP_SPECTRAL_INFO   0x601

#define KX_PROP_TEXT2ID     0x620

#define KX_PROP_DSP_ASSIGN  0x630

#if defined(WIN32)

           // KX_WAVE property: DirectSound's DirectSoundBuffer
           // ----------------------------------------------------
           #define KX_PROP_VOICE_ROUTING    0x700
           typedef struct
           {
            dword routing; // complete 'send_a+send_b+send_c+send_d' rout
            dword xrouting;
                    // NB! for mono voices automatically sets send_b amount to 'none'; use DX pan to change
           }voice_routing_req;

           #define KX_PROP_ASIO     0x701
           
           typedef struct
           {
            int cmd;
            #define KX_ASIO_ALLOC_INPUT 1
            #define KX_ASIO_START       2
            #define KX_ASIO_STOP        3
            #define KX_ASIO_FREE_OUTPUT     4
            #define KX_ASIO_ALLOC_OUTPUT    5
            #define KX_ASIO_FREE_INPUT  6
            #define KX_ASIO_GET_POSITION    7
            #define KX_ASIO_INIT        8   // 'chn' = 'method': 1 - old; 2 - new
            #define KX_ASIO_CLOSE       9
            #define KX_ASIO_WAIT        10
            #define KX_ASIO_TIMER       11

            dword position; // KX_ASIO_GET_POSITION

            union
            {
                size_t size; // alloc: buffer size
                __int64 size_padding;
            };

            union
            {
                void *addr; // alloc: buffer addr
                __int64 addr_padding;
            };

            int chn;  // alloc/free: channel #
            int rate; // sampling rate: for playback voices only
            int bps; // 16 or 32

            union
            {
                volatile bool *ptr; // for KX_ASIO_WAIT
                __int64 ptr_padding;
            };

            // for init:
            union
            {
                HANDLE user_event;
                __int64 user_event_padding;
            };

            union
            {
                asio_notification_t *notification;
                __int64 notification_padding;
            };

            #define KX_ASIO_MAGIC   (0x3314354^KX_VERSION_DWORD)
            dword magic; // magic
           }asio_property;
#endif

#define KX_PROP_ASIO_CAPS   0x702

#define KX_PROP_FPGA_FIRMWARE   0x800
#define KX_PROP_FPGA_FW_START   0x801
#define KX_PROP_FPGA_FW_CONT    0x802
#define KX_PROP_FPGA_FW_END 0x803
typedef struct
{
 int size;
}fpga_firmware_property;

#pragma pack()

#endif // _KX_IOCTL_H_
