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


#ifndef KX_DRIVER_H_
#define KX_DRIVER_H_

// #define KX_INTERNAL      // - include internal structures/defs
// #define KX_DEBUG     // - release / debug
// #define KX_DEBUG_FUNC    // - code to debug smthing

#include "vers.h"
#include "defs.h"

#if defined(_WIN32) || defined(_WINDOWS) || defined(WIN32)
 #include "driver/os_win.h"
#elif defined(__APPLE__) && defined(__MACH__) // MacOSX
 #include "driver/os_mac.h"
#else
 #error "Unknown OS"
#endif

#define KX_PAGE_SIZE    4096

#include "driver/list.h"
#include "hw/8010x.h"
#include "hw/p16v.h"
#include "hw/p17v.h"
#include "hw/tina.h"
#include "driver/pci.h"

#include "interface/ikx.h"
#include "interface/dsp.h"

#include "driver/math.h"

// debug flags
#define DERR    1
#define DLIB    2
#define DWDM    3
#define DGSIF   4
#define DKX3D   5
#define DEAX    6
#define DASIO   7

#define DNONE   0

#define DBUFF   -10
#define DAC3    -12
#define DPROP   -13
#define DSTATE  -14
#define DSFNT   -15
#define DMIDI   -16
#define DDB -17
#define DNRPN   -19
#define DPV -20

#if defined(KX_DEBUG_FUNC) && defined(KX_DEBUG)
 #define debug (((KX_DEBUG_FUNC)==0)?0:(KX_DEBUG_FUNC))
#else
 #define debug (void)
#endif

enum kx_cpu_cache_type_t
{
 KX_NONCACHED,
 KX_CACHED
};

struct kx_fpu_state
{
 dword fpu_state[8];
};

struct kx_mixer_control
{
 char pgm_name[KX_MAX_STRING];
 char reg_left[KX_MAX_STRING];
 char reg_right[KX_MAX_STRING];
 int max_vol;
};

#define KX_NUMBER_OF_VOICES     64
#define MAX_MPU_BUFFER          4096
#define MAX_MPU_DEVICES         2

struct kx_voice_buffer
{
 int pageindex; // do not fill it yourself

 dword size; // in bytes
 int notify; // in ms
 dword physical;
 void *addr;
 void *that;

 void *instance;

#if defined(__APPLE__) && defined(__MACH__) // MacOSX
 IOBufferMemoryDescriptor *desc;
#endif
};

struct kx_hw;

struct sync_data
{
 #define KX_SYNC_IPR_IRQ        1
 #define KX_SYNC_IRQ_ENABLE     2
 #define KX_SYNC_IRQ_DISABLE    3
 #define KX_SYNC_CLEAR_IPR      4
 #define KX_SYNC_MPUIN          5
 #define KX_SYNC_MPUOUT         6

 int what;
 kx_hw *hw;

 dword irq_mask;
 int ret;
 int turn_on;
};

struct kx_callbacks
{
    void *call_with;

    int (*debug_func)(int where,const char *__format, ... );

    void (*save_fpu_state)(kx_fpu_state *state);
    void (*rest_fpu_state)(kx_fpu_state *state);
    void (*sync)(void *call_with,sync_data*);
         // this should simply call kx_sync_interrupt() with appr. parameters
         // in Windows this should be synchronized with IRQ handler
    void (*usleep)(int microseconds);

    #define KX_NONPAGED 0
    // #define KX_PAGED     1      // never used
    void (*malloc_func)(void *call_with,int len,void **b,int where);
    void (*free_func)(void *call_with,void *buff);

    void (*send_message)(void *call_with,int len,const void *b);	// const: 3551
    void (*notify_func)(void *data,int what);
        // event:
        #define LLA_NOTIFY_FULL     0
        #define LLA_NOTIFY_HALF     1
        #define LLA_NOTIFY_TIMER    2
        #define LLA_NOTIFY_EVENT    4
        #define LLA_NOTIFY_SYSTEM   8

    // allocates contiguous memory
    int  (*pci_alloc)(void *call_with,struct memhandle *,kx_cpu_cache_type_t cache_type);
    void (*pci_free)(void *call_with,struct memhandle *);
    void (*get_physical)(void *call_with,kx_voice_buffer *buff,int offset,__int64 *physical_addr);

    // soundfont functions
    // allocates large memory blocks [nonpaged, not-contiguous]
    int (*lmem_alloc_func)(void *call_with,int len,void **lm,kx_cpu_cache_type_t cache_type);
    int (*lmem_free_func)(void *call_with,void **lm);
    void * (*lmem_get_addr_func)(void *call_with,void **lm,int offset,__int64 *phisical); // physical is optional

    word io_base;
    byte irql;

    dword device; // needed
    dword subsys;
    byte chip_rev;

    byte bus,dev,func; // for spy.exe

    // default parameters:
    int tram_size;  // in bytes; if == 0 - no TRAM
    
    // default routings: see ikx.h
    dword def_routings[ROUTING_LAST+1];
    dword def_xroutings[ROUTING_LAST+1]; // 10k2 only

    byte def_amount[AMOUNT_LAST+1];

    // rec
    dword rec_buffer,pb_buffer,ac3_buffers;

    // tweakable volumes
    kx_mixer_control mixer_controls[MAX_MIXER_CONTROLS];
};

struct kx_voice_param
{
 dword initial_pitch,   // apitch
     pitch_target,
     start,
     startloop,
     endloop,
     end,
     send_a, // for stereo voices send_amount is stored in channel's own send_xx...
     send_b,
     send_c,
     send_d,
     send_e,
     send_f,
     send_g,
     send_h,
     send_routings,
     send_xroutings,
     volume_target, // vtarget
     initial_fc,    // acutoff
     initial_attn,  // avol
     initial_cv,    // cvcf
     fc_target,
     current_fc,
     filter_amount,
     pitch_amount;
 int
     moddelay,
     modatk,
     modhld,
     moddecay,
     modsustain,
     modrelease;
 int
     voldelay,
     volatk,
     volhld,
     voldecay,
     volsustain,
     volrelease;
 dword
     lfo1delay,lfo2delay,
     modlfofreq, // tremfrq
     modlfo2vol,
     modlfo2fc, // fmmod
     modlfo2pitch,
     viblfofreq, // fm2frq2
     viblfo2pitch,
     filterQ;

 dword sf2_pitch;
 int sample_type;

 int loop_type;

 #define SF_PARAMETERS 60
 int sf2_params[SF_PARAMETERS];

 int sf2_usage;
 int interpolate;

 // temp values in order to set-up synth buffers
 dword tmp_buffer_size;
 dword tmp_buffer_physical;
 void *tmp_buffer_addr;
};

struct kx_timer 
{
    struct list list;
    byte status; 
    dword counter;
    dword target;           
    dword delay;    // should be unsigned

    void (*timer_func)(void *data,int what);
    void *data;
};

#include "interface/soundfont.h"

struct kx_voice
{
 dword usage;
 kx_voice_buffer buffer;
 kx_voice_param param;
 kx_timer timer;

 int sampling_rate;
 int bytespersample;
 int hw_pos;
 #define VOICE_STATE_STARTED    0x1
 #define VOICE_STATE_STOPPED    0x2
 int state;

 int total_played;

 // some data for midi/synth
 int channel,note;
 byte vel;
 uintptr_t synth_id; // shared with multichannel status: VOICE_STATUS_* flag
 dword unique;

 void *asio_id;
 dword asio_channel;
 void *asio_mdl,*asio_user_addr,*asio_kernel_addr;
};

struct kx_rec_voice
{
 dword usage;

 // physical registers:
 dword rec_set;
 dword rec_pos;
 dword rec_size;

 kx_voice_buffer buffer;
 kx_timer timer;

 int state;
};

#ifdef KX_INTERNAL

struct kx_hw_previous
{
   dword hcfg_k1,hcfg_k2;
   dword inte;
   dword timer;

   dword mbs,mba,fxbs,fxba,adcbs,adcba,adcsr;
   dword cliel,clieh,soll,solh;
   dword scs0,scs1,scs2;
   dword ptba,trba,trbs;
   dword dbg;
   dword ehc;
   dword pcb;
   dword soc;
   dword fxwc_k1,fxwcl,fxwch;

   dword hliel,hlieh;
   dword spra,sprc;

   dword ttda,ttdd;
   dword ttb,tdof;
   dword tblsz;

   dword per_voice[KX_NUMBER_OF_VOICES][0x30+3];

   dword dsp_data[0xf00];

   #define MAX_AC97_VALUES  32
   word ac97_values[MAX_AC97_VALUES];

   // p16v state
   dword recvh,recvl,recsel,sa,phcfg;
   dword p16v_values[0x40]; // 60..6f [more space for k4]
};

struct kx_mpu_buffer
{
    int where;
    struct list list;
    void *instance;
    int mpu_tail;
    int mpu_head;
    byte mpu_buffer[MAX_MPU_BUFFER];
};

struct kx_pagetable_t
{
    word sz;    // block size
    word usage; // number of block uses
    dword addr;
};

struct kx_midi_state_t;

// ac-3 passthru (xtram method) state
struct kx_ac3_passthru
{
 void *instance; // that
 int method;    
 #define KX_AC3_PASSTHRU_GENERIC    1
 #define KX_AC3_PASSTHRU_XTRAM      2

 int pgm_id;
 int buf_size;
 word dbac_reg;
 dsp_microcode *m;
 word counter_reg;

 dword prev_spdif_freq;

 void (*callback)(void *); // note: stack calling convention
};

struct asio_physical_descr_t
{
       dword physical; // physical address
       void *kernel_addr;
       void *user_addr;
       size_t size;
       void *magic;
       void *mdl;
};

struct kx_hw
{
    char kx_version[KX_MAX_STRING];
    char kx_driver[KX_MAX_STRING];
    char kx_date[KX_MAX_STRING];

    // PCI info
    byte pci_bus;   // used only by spy.exe:
    byte pci_dev;
    byte pci_func;

    dword pci_subsys;   // needed
    dword pci_device;
    byte  pci_chiprev;

    byte is_10k2;       // dev: 0004
    byte is_aps;        //    model match: e-mu APS
    byte is_a2ex;       // a2 platinum ex
    byte is_a2;         // dev: 0004 rev: 4
    byte is_51;
    byte is_k8;     // dev: 0008
    byte can_k8_passthru;
    byte is_edsp;       // subsys: 4000
    byte is_zsnb;       //    model match: a2zsnb
    byte is_cardbus;    // subsys: 2000 -> any cardbus
    byte is_bad_sb22x;
    byte is_a4;         // ??

    char card_name[KX_MAX_STRING];
    char db_name[KX_MAX_STRING];
    dword hcfg_k1,hcfg_k2;

    // extended flags [KX_HW_COMPAT value]:
    dword ext_flags;

    // bitfield
    dword initialized;
    #define KX_BUFFERS_INITED       1
    #define KX_SF_INITED        2
    #define KX_DSP_INITED       8

    // AC97
    char have_ac97;
    char lack_ac97;
    char ac97_codec_name[KX_MAX_STRING];
    char ac97_3d_name[KX_MAX_STRING];
    char ac97_is_20;
    word ac97_extid;
    word ac97_cap;
    word ac97_id1;
    word ac97_id2;
    byte has_surdac;
    byte can_passthru;

    // Resources
    word port;
    byte irq;
    byte standalone;

    // pagetable
    #define MAXPAGES        8192 
    kx_pagetable_t pagetable[MAXPAGES];

    // memory
    memhandle   virtualpagetable;
    memhandle   tankmem;
    memhandle   silentpage;
    memhandle   mtr_buffer;

    // voice info
    kx_voice voicetable[KX_NUMBER_OF_VOICES];
    int last_voice;

    #define KX_NUMBER_OF_REC_VOICES 2
    kx_rec_voice rec_voicetable[KX_NUMBER_OF_REC_VOICES];

    int synth_compat;

    // locks
    spinlock_t k_lock;
    spinlock_t hw_lock;
    spinlock_t timer_lock;
    spinlock_t mpu_lock[MAX_MPU_DEVICES];
    spinlock_t dsp_lock;
    spinlock_t sf_lock;
    spinlock_t uartout_lock;
    spinlock_t ac97_lock;
    spinlock_t pt_lock;
    dword irq_pending;

    // lists
    struct list timers;
    struct list microcodes;
    struct list sf;

    // timers
    dword timer_delay;

    kx_timer sys_timer;

    // MPU
    struct list mpu_buffers[MAX_MPU_DEVICES];
    int have_mpu;

    #define MAX_UART_BUFFER 8192
    byte uart_out_buffer[MAX_MPU_DEVICES][MAX_UART_BUFFER];
    int uart_out_tail[MAX_MPU_DEVICES],uart_out_head[MAX_MPU_DEVICES];

    // dsp / microcode
    dword dsp_flags;
    #define KX_DSP_SWAP_REAR        0x1
    #define KX_DSP_ROUTE_PH_TO_CSW  0x2
    #define KX_AC3_PASSTHROUGH      0x4

    #define FX_REGISTER_MASSIVE_SIZE (1024/32)  
     // 0x100..0x600 (-0x100 for Tram addr) as per 10k2; -0x100 for tram flag [10k2]
    dword fx_regs_usage[FX_REGISTER_MASSIVE_SIZE]; // GPR + TRAM (addr only)

    #define FX_MICROCODE_MASSIVE_SIZE (E10K2_MAX_INSTRUCTIONS/32) // as per 10k2
    dword fx_microcode_usage[FX_MICROCODE_MASSIVE_SIZE];

    // 10k2/10k1
    int opcode_shift;
    int high_operand_shift;
    int first_instruction;
    int microcode_size;

    dword synth_vol[2]; // in KX_VOLUME units (-dB/0x10000)

    // ASIO
    asio_physical_descr_t asio_inputs[MAX_ASIO_INPUTS]; // without p16v
    asio_notification_t asio_notification_krnl;
    int cur_asio_in_bps;
    int card_frequency;

    int drum_channel;

    dword mtr_buffer_size;

    dword spdif_states[3];
    dword pt_spdif;

    // prev state
    kx_hw_previous prev_state;
    int power_state;
    kx_callbacks cb;

    // e-card
    kx_ecard_state ecard;
    byte gp_ins;

    int spdif_decode;
    int spdif_recording;

    // vienna/DirectSynth device
    struct kx_midi_state_t *midi[2];

    // p16v stuff
    int p16v_pb_opened,p16v_rec_opened,p16v_pb_chn,p16v_rec_chn;
    kx_timer p16v_pb_timer,p16v_rec_timer;

    int p16v_volumes[8];

    int p16v_rec_select;
    int p16v_routing;

    kx_ac3_passthru ac3_pt_state;
};

int kx_state_save(kx_hw *hw);
int kx_state_restore(kx_hw *hw);

#define KX_MAX_SF_VOICES    16

#else // !KX_INTERNAL
 struct kx_hw
 {
  dword reserved;
 };
#endif

#ifdef KX_INTERNAL
 #include "hw/ecard.h"
 #include "driver/init.h"
 #include "driver/timer.h"
 #include "driver/calc.h"
 #include "driver/voice.h"
#endif

#include "hw/ac97.h"

struct kx_ac97_registers_t
{
 byte index;
 word def_value;
 const char *name;
};

extern const kx_ac97_registers_t kx_ac97_registers[];

#include "driver/irq.h"

// internal driver usage
#ifdef KX_INTERNAL
  #define hwOP(at, op, z, w, x, y) \
        do { kx_writeptr(hw, hw->first_instruction+(at)*2, 0, ((x) << hw->high_operand_shift) | (y)); \
        kx_writeptr(hw, hw->first_instruction+(at)*2+1, 0, ((op) << hw->opcode_shift ) | ((z) << hw->high_operand_shift) | (w)); \
        } while (0)
#endif

KX_API(int,kx_init(kx_hw **hw,kx_callbacks *cb,int standalone));
KX_API(int,kx_defaults(kx_hw *hw,kx_callbacks *cb));
KX_API(int,kx_close(kx_hw **));
KX_API(int,kx_getstring(kx_hw *,int what,char *buff));

KX_API(dword,kx_getdword(kx_hw *hw,int what,dword *ret)); // returns 0 on success

// buffer management; num is voice number
int kx_alloc_buffer(kx_hw *hw,int num);
void kx_free_buffer(kx_hw *hw,int num);

// UART
KX_API(int,kx_mpu_write_data(kx_hw *card, byte data,int where));
KX_API(int,kx_mpu_read_data(kx_hw *card, byte *data,int where));

KX_API(int,kx_mpu_reset(kx_hw *card,int where,int irqs));
KX_API(int,kx_mpu_read_buffer_close(kx_hw *hw,void *inst,int where));
KX_API(int,kx_mpu_read_buffer_open(kx_hw *hw,void **inst,int where));
KX_API(int,kx_mpu_read_buffer(kx_hw *card, void *instance,byte *data,int where));
KX_API(int,kx_mpu_write_buffer(kx_hw *hw,int offset,byte *data,int len,int *actual_len=NULL));

// AC97
KX_API(word,kx_ac97read(kx_hw *hw,byte index));
KX_API(void,kx_ac97write(kx_hw *hw,byte index, word data));
KX_API(void,kx_ac97reset(kx_hw *hw));

// Lowlevel hardware access
#define REGLIST_END (-1)
KX_API(int,kx_interrupt_deferred(kx_hw *hw)); // returns KX_IRQ_...
KX_API(int,kx_interrupt_critical(kx_hw *hw)); // returns 0 if IRQ processed, otherwise error code
KX_API(void,kx_sync(sync_data *s));
KX_API(dword,kx_get_irq_pending(kx_hw *hw));
KX_API(void,kx_clear_irq_pending(kx_hw *hw,dword pat));
KX_API(void, kx_wcwait(kx_hw *card, dword wait));
KX_API(void, kx_writefn0(kx_hw *card, dword reg, dword data));
KX_API(dword, kx_readfn0(kx_hw * card, dword reg));
KX_API(void, kx_writefn0w(kx_hw *card, dword reg, word data));
KX_API(word, kx_readfn0w(kx_hw * card, dword reg));
KX_API(void, kx_writefn0b(kx_hw *hw, dword reg, byte data));
KX_API(byte, kx_readfn0b(kx_hw * hw, dword reg));

KX_API(void, kx_writeptr_prof(kx_hw *hw, dword reg, dword channel, dword data));
KX_API(void, kx_writeptr(kx_hw *card, dword reg, dword channel, dword data));

KX_API(void, kx_writeptr_multiple(kx_hw *card, dword channel, ...));
KX_API(dword, kx_readptr(kx_hw * card, dword reg, dword channel));
KX_API(void, kx_writeptrb(kx_hw *card, dword reg, dword channel, byte data));
KX_API(byte, kx_readptrb(kx_hw * card, dword reg, dword channel));
KX_API(void, kx_writeptrw(kx_hw *card, dword reg, dword channel, word data));
KX_API(word, kx_readptrw(kx_hw * card, dword reg, dword channel));

KX_API(void, kx_writep16v(kx_hw *card, dword reg, dword chn, dword data));
KX_API(dword, kx_readp16v(kx_hw * card, dword reg, dword chn));

KX_API(int,kx_writefpga(kx_hw *hw, dword reg, dword value));
KX_API(dword,kx_readfpga(kx_hw *hw, dword reg));
KX_API(int,kx_fpga_link_src2dst(kx_hw *hw, dword src, dword dst));
KX_API(int,kx_upload_fpga_firmware(kx_hw *hw,byte *data,int size));

KX_API(int, kx_writespi(kx_hw *hw, dword data));
KX_API(int, kx_writei2c(kx_hw * hw,dword reg, dword value));

KX_API(void, kx_voice_stop_on_loop_disable(kx_hw *card, dword voicenum));
KX_API(void, kx_voice_stop_on_loop_enable(kx_hw *card, dword voicenum));
KX_API(void, kx_voice_irq_ack(kx_hw *hw, dword voicenum));
KX_API(void, kx_voice_irq_disable(kx_hw *card, dword voicenum));
KX_API(void, kx_voice_irq_enable(kx_hw *card, dword voicenum));

KX_API(int,kx_steal_voices(kx_hw *hw,dword usage,int flag=0));
// flags:
#define KX_STEAL_SIZE           1   // size priority
#define KX_STEAL_MAINTENANCE        2   // no real need to free the voice

KX_API(void, kx_irq_disable(kx_hw *card, dword irq_mask));
KX_API(void, kx_irq_enable(kx_hw *card, dword irq_mask));

// kX WaveIn device
// --------------------
KX_API(int,kx_rec_open(kx_hw *hw,kx_voice_buffer *buffer,int usage,int sampling_rate,int sub_device));
// supports MONO/Stereo; 16bit ONLY
KX_API(int,kx_rec_close(kx_hw *hw,int sub_device));
KX_API(int,kx_rec_start(kx_hw *hw,int sub_device));
KX_API(int,kx_rec_stop(kx_hw *hw,int sub_device));
KX_API(int,kx_rec_get_position(kx_hw *hw,int *pos,int sub_device));
KX_API(int,kx_rec_set_notify(kx_hw *hw,int num,int interval)); // in ms

// kX WaveOut device
// ---------------------
KX_API(int,kx_wave_open(kx_hw *hw,kx_voice_buffer *buffer,int usage,int sampling_rate,int device_=DEF_WAVE01_ROUTING));
// usage: stereo/mono 8/16bit playback; + flags; voice_open_notimer
// device_: in terms DEF_WAVExx_ROUTING
KX_API(int,kx_wave_close(kx_hw *hw,int num));
KX_API(int,kx_wave_stop(kx_hw *hw,int num));
KX_API(int,kx_wave_start(kx_hw *hw,int num));
KX_API(int,kx_wave_stop_multiple(kx_hw *hw,int first,dword low,dword high));
KX_API(int,kx_wave_start_multiple(kx_hw *hw,int first,dword low,dword high));
KX_API(int,kx_wave_set_notify(kx_hw *hw,int num,int interval)); // in ms
KX_API(int,kx_wave_get_position(kx_hw *hw,int num,int *pos)); // position is in bytes
KX_API(int,kx_wave_set_position(kx_hw *hw,int num,int pos)); // position is in bytes
KX_API(dword,kx_calc_position(kx_hw *hw,int num,dword qkbca)); // position is in samples
KX_API(int,kx_wave_set_parameter(kx_hw *hw,int num, int ch,int param,dword value));
KX_API(int,kx_wave_get_parameter(kx_hw *hw,int num, int ch,int param,dword *value));
KX_API(int,kx_wave_setup_b2b(kx_hw *hw,int num,dword new_routing,int high_low,int ab)); // high_low: 1 or 0; ab: 0 for A, 1 for B

KX_API(int,kx_set_fx_amount(kx_hw *hw,int where,int num,int i));

dword calc_volume(kx_hw *hw,dword value,dword max);

// kX MultiChannel device
// ----------------------
KX_API(int,kx_allocate_multichannel(kx_hw *hw,int bps,int rate,int flag,kx_voice_buffer *buffer,int routing));

// kX Multitrack device
// --------------------
KX_API(int,kx_mtrec_start(kx_hw *hw));
KX_API(int,kx_mtrec_stop(kx_hw *hw));
KX_API(int,kx_mtrec_select(kx_hw *hw,dword flag,dword flag2=0)); // flag2-10k2 only

// kX P16V/HQ device
// -----------------
// functions

KX_API(int,kx_p16v_set_rec_volume(kx_hw *hw,dword vol)); // vol is in vol units
KX_API(int,kx_p16v_rec_select(kx_hw *hw,int source));
 // see P16V_REC_...
KX_API(int,kx_p16v_set_pb_volume(kx_hw *hw,int cnh,dword vol)); // chn is absolute spdif/i2s channel [0..7] (w/o any routing stuff)
KX_API(int,kx_p16v_set_routing(kx_hw *hw,int routing)); // swap front & rear capable
 // see P16V_ROUTE_...

KX_API(int,kx_p16v_pb_start(kx_hw *hw));
KX_API(int,kx_p16v_pb_stop(kx_hw *hw));
KX_API(int,kx_p16v_pb_open(kx_hw *hw,kx_voice_buffer *buff,int frequency,int flags));
 #define P16V_STEREO        1
 #define P16V_MULTICHANNEL  0
KX_API(int,kx_p16v_pb_close(kx_hw *hw));
KX_API(int,kx_p16v_pb_get_pos(kx_hw *hw,dword &pos));
KX_API(int,kx_p16v_pb_set_notify(kx_hw *hw,int freq));
KX_API(int,kx_p16v_pb_set_freq(kx_hw *hw,int freq));

KX_API(int,kx_p16v_rec_start(kx_hw *hw));
KX_API(int,kx_p16v_rec_stop(kx_hw *hw));
KX_API(int,kx_p16v_rec_open(kx_hw *hw,kx_voice_buffer *buff,int frequency,int flags));
KX_API(int,kx_p16v_rec_close(kx_hw *hw));
KX_API(int,kx_p16v_rec_get_pos(kx_hw *hw,dword &pos));
KX_API(int,kx_p16v_rec_set_notify(kx_hw *hw,int freq));

KX_API(int,kx_p16v_init(kx_hw *hw));
KX_API(int,kx_get_p16v_volume(kx_hw *hw,int chn,dword *vol));
KX_API(int,kx_set_p16v_volume(kx_hw *hw,int chn,dword vol));

// kX Synth/Midi device
// --------------------
// synth channel info
typedef struct
{
 byte program;
 word pitch;
 byte chpressure;

 word bank_lsb; // 0, 32
 word bank_msb;
 byte modulation; // 1
 byte breath; // 2
 byte foot; // 4
 byte portamento_time; // 5
 byte vol; // 7
 byte balance; // 8
 byte pan; // 10
 byte expression; // 11
 byte sustain; // hold_pedal, 64
 byte portamento_onoff; // 65
 byte sostenuto; // 66
 byte soft_pedal; // 67
 byte legato; // 68
 byte hold_2; // 69

 byte gp1,gp2,gp3,gp4; // 16,17,18,19 MIDI CC

 // 70: variation
 // 75: // sound control 6 decay time             
 // 76: // sound control 7 vibrato rate (freq.)  
 // 77: // sound control 8 vibrato depth         
 // 78: // sound control 9 vibrato delay          

 byte portamento_ctrl; // 84

 int  resonance,     // 71: Harmonic Content (filter resonance)
      sound_release, // 72
      sound_attack,  // 73
      sound_brightness; // 74
 byte tremulo_level, // 92
      celeste_level, // 94
      phaser_level;  // 95
 byte reverb; // 91
 byte chorus; // 93

 byte data_entry_lsb,data_entry_msb;
 byte nrpn_lsb,nrpn_msb;
 byte rpn_lsb,rpn_msb;

 int multiplier;
 int last_rpn;

 // GM RPNs
 int pb_sensivity;
 int master_fine_tuning;
 int master_coarse_tuning;
 // if added -> modify reset_all_controllers()

 // NRPNs
 #define AWE_PARAMETERS 30
 #define AWE_UNUSED 0x10000
 int nrpn_sf_data[SF_PARAMETERS];
 int nrpn_awe_data[AWE_PARAMETERS];

}kx_midi_channel;

typedef struct
{
 word gen;
 word flag;
}sf_parameters_t;

extern sf_parameters_t sf_defaults[SF_PARAMETERS];

// synth state
typedef struct kx_midi_state_t
{
 kx_hw *hw;
 int inited;
 #define MAX_MIDI_CHANNELS 16
 kx_midi_channel channels[MAX_MIDI_CHANNELS];
 int drum_channel;

 int synth_num;
}kx_midi_state;

KX_API(int,kx_midi_init(kx_hw *hw,kx_midi_state *midi,int synth_));
KX_API(int,kx_midi_play_buffer(kx_midi_state *midi,byte *buff,int len));
KX_API(int,kx_midi_stop(kx_midi_state *midi));
KX_API(int,kx_midi_close(kx_midi_state *midi));
KX_API(int,kx_midi_set_volume(kx_hw *hw,int chn,dword vol)); // chn=0..1 L&R; vol: general kX vol
KX_API(int,kx_midi_get_volume(kx_hw *hw,int chn,dword *vol));
KX_API(int,kx_midinote_play(kx_midi_state *midi,int chn,int note,int vel));
#define CHANGE_KEYPRESSURE  100
#define CHANGE_CONTROL      102
#define CHANGE_CHANNELPRESSURE  103
#define CHANGE_PEFE     104
#define CHANGE_FILTERQ      105
#define CHANGE_GP1      106
#define CHANGE_GP2      107
#define CHANGE_GP3      108
#define CHANGE_GP4      109
#define CHANGE_PITCHBEND    4
KX_API(int,kx_midi_changes(kx_midi_state *midi,int ch,int what,int note,int how));

// Synth HAL
KX_API(int,kx_synth_changes(kx_midi_state *midi,int ch,int note,int what)); // if note==-1 -> all notes of this MIDI channel
#define CHANGE_VOLUME       7
// #define CHANGE_PITCHBEND 4 /* same */
#define CHANGE_MODULATION   1
#define CHANGE_PAN      10
#define CHANGE_REVERB       91
#define CHANGE_CHORUS       93
#define CHANGE_SOFT_PEDAL   67
#define CHANGE_HOLD2        69
KX_API(int,kx_synth_start(kx_midi_state *midi,int ch,int note,int vel));
KX_API(int,kx_synth_release(kx_midi_state *midi,int phys_chn));
KX_API(int,kx_synth_release(kx_hw *hw,int phys_chn));
KX_API(int,kx_synth_term(kx_midi_state *midi,int phys_num));
KX_API(int,kx_synth_term(kx_hw *hw,int phys_num));

// SoundFont
// returns number of filled in values
// 0,if no voice match
int sf_find_voice(kx_midi_state *midi,kx_voice_param *table,int chn,int note,int vel);

// Effects API
KX_API(int,kx_set_dsp_register(kx_hw *hw,int pgm,const char *name,dword val));
KX_API(int,kx_get_dsp_register(kx_hw *hw,int pgm,const char *name,dword *val));
KX_API(int,kx_set_dsp_register(kx_hw *hw,int pgm,word id,dword val));
KX_API(int,kx_get_dsp_register(kx_hw *hw,int pgm,word id,dword *val));

KX_API(int,kx_get_tram_addr(kx_hw *hw,int pgm,const char *name,dword *addr));
KX_API(int,kx_set_tram_addr(kx_hw *hw,int pgm,const char *name,dword addr));
KX_API(int,kx_get_tram_flag(kx_hw *hw,int pgm,const char *name,dword *flag));
KX_API(int,kx_set_tram_flag(kx_hw *hw,int pgm,const char *name,dword flag));

KX_API(int,kx_get_tram_addr(kx_hw *hw,int pgm,word id,dword *addr));
KX_API(int,kx_set_tram_addr(kx_hw *hw,int pgm,word id,dword addr));
KX_API(int,kx_get_tram_flag(kx_hw *hw,int pgm,word id,dword *flag));
KX_API(int,kx_set_tram_flag(kx_hw *hw,int pgm,word id,dword flag));

KX_API(int,kx_write_instruction(kx_hw *hw,int pgm,int offset,word op,word z,word w,word x,word y,int valid=0xffffffff));
KX_API(int,kx_read_instruction(kx_hw *hw,int pgm,int offset, word *op,word *z,word *w,word *x,word *y));

KX_API(int,kx_translate_microcode(kx_hw *hw,int pgm,int place=KX_MICROCODE_ANY,int pos_pgm=0));
KX_API(int,kx_untranslate_microcode(kx_hw *hw,int pgm));

KX_API(int,kx_enable_microcode(kx_hw *hw,int pgm));
KX_API(int,kx_disable_microcode(kx_hw *hw,int pgm));
KX_API(int,kx_set_microcode_bypass(kx_hw *hw,int pgm,int state));
KX_API(int,kx_set_microcode_flag(kx_hw *hw,int pgm,dword flag));
KX_API(int,kx_get_microcode_flag(kx_hw *hw,int pgm,dword *flag));

KX_API(int,kx_dsp_stop(kx_hw *hw));
KX_API(int,kx_dsp_go(kx_hw *hw));
KX_API(int,kx_dsp_clear(kx_hw *hw));
KX_API(int,kx_dsp_reset(kx_hw *hw));
KX_API(int,kx_dsp_reload_epilog(kx_hw *hw));

KX_API(int,kx_set_clock(kx_hw *hw,dword freq));

KX_API(int,kx_reset_voices(kx_hw *hw));

// returns pgm id or 0 if failed
KX_API(int,kx_load_microcode(kx_hw *hw,const char *name,dsp_code *code,int code_size,
   dsp_register_info *info,int info_size,int itramsize,int xtramsize,
   const char *copyright,const char *engine,const char *created,const char *comment,const char *guid,
   int force_pgm_id=0));
KX_API(int,kx_update_microcode(kx_hw *hw,int pgm_id,const char *name,dsp_code *code,int code_size,
        dsp_register_info *info,int info_size,int itramsize,int xtramsize,
        const char *copyright,
        const char *engine,
        const char *created,
        const char *comment,
        const char *guid,
        unsigned int flag));
KX_API(int,kx_unload_microcode(kx_hw *hw,int pgm));

KX_API(int,kx_connect_microcode(kx_hw *hw,int pgm1,word src,int pgm2,word dst));
KX_API(int,kx_connect_microcode(kx_hw *hw,int pgm1,const char *src,int pgm2,const char *dst));
KX_API(int,kx_connect_microcode(kx_hw *hw,int pgm1,const char *src,int pgm2,word dst_n));
// if pgm2==-1 -> dst='physical input/physical output';[untranslated]
KX_API(int,kx_disconnect_microcode(kx_hw *hw,int pgm,word src));
KX_API(int,kx_disconnect_microcode(kx_hw *hw,int pgm,const char *src));
// if pgm==-1 src='physical input'; [untranslated]

KX_API(int,kx_get_connections(kx_hw *hw,int pgm,kxconnections *out,int size));
// if size==0 -> returns needed buffer size

KX_API(int,kx_set_volume(kx_hw *hw,const char *pgm_id,const char *name,dword val,dword max=0x80000000));
KX_API(int,kx_set_volume(kx_hw *hw,int pgm_id,word reg,dword val,dword max=0x80000000));
// val=KX_MIN_VOLUME..KX_MAX_VOLUME

KX_API(int,kx_set_microcode_name(kx_hw *hw,int pgm_id,const char *str,int what=0));

KX_API(int,kx_enum_microcode(kx_hw *hw,int pgm,dsp_microcode *mc));
KX_API(int,kx_enum_microcode(kx_hw *hw,char *pgm_id,dsp_microcode *mc));
KX_API(int,kx_enum_microcode(kx_hw *hw,dsp_microcode *mc_ret,int size));
KX_API(int,kx_get_microcode(kx_hw *hw,int pgm,dsp_code *code,int code_size,
        dsp_register_info *info,int info_size));
  // info_size & code_size - sizes of code&info buffers (in bytes);
  // if they dont match ones got by kx_enum_microcode -> fuction will fail...
KX_API(int,kx_translate_text2id(kx_hw *hw,kx_text2id *));
KX_API(int,kx_get_routing(kx_hw *hw,int id,dword *routing,dword *xrouting));
KX_API(int,kx_set_routing(kx_hw *hw,int id,dword routing,dword xrouting));
KX_API(int,kx_get_fx_amount(kx_hw *hw,int id,byte *amount));
KX_API(int,kx_set_fx_amount(kx_hw *hw,int id,byte amount));
KX_API(int,kx_set_asio_fx_amount(kx_hw *hw,int chn,int id,byte amount));

KX_API(int,kx_get_asio_position(kx_hw *hw,int reget)); // returns current audio position or -1

KX_API(int,kx_load_soundfont(kx_hw *hw,kx_sound_font *sf));
KX_API(int,kx_unload_soundfont(kx_hw *hw,int id));
KX_API(int,kx_enum_soundfonts(kx_hw *hw,sfHeader *hdr,int size)); // if size==0: returns # of bytes required
// on exit: rom_ver contains 'id'
KX_API(int,kx_load_soundfont_samples(kx_hw *hw,sf_load_sample_property *sf_p)); // 0 - success
KX_API(int,kx_sf_get_preset_description(kx_hw *hw,int bank,int preset,char *name));
 // if preset==-1, return (dword) enum_soundfonts index# in 'name'

KX_API(int,kx_get_hw_parameter(kx_hw *hw,int id,dword *value));
KX_API(int,kx_set_hw_parameter(kx_hw *hw,int id,dword value));

KX_API(int,kx_set_tram_size(kx_hw *hw,int new_size));

KX_API(int,kx_mute(kx_hw *hw));
KX_API(int,kx_unmute(kx_hw *hw));

// power management
KX_API(int,kx_set_power_state(kx_hw *hw,int state));
#define KX_POWER_NORMAL     0
#define KX_POWER_SLEEP      1
#define KX_POWER_SUSPEND    2

// E-Card/E-Drive
KX_API(int,kx_ecard_spdif_select(kx_hw *hw, dword target, dword source)); // target is KX_EC_SPDIFxx_SHIFT; source is KX_EC_SOURCE_...
KX_API(int,kx_ecard_set_routing(kx_hw *hw, dword routing)); // for both [16+16 bit]; low=spdif0, high=spdif1
KX_API(void,kx_ecard_set_adc_gain(kx_hw *hw, dword gain)); // 8bit+8bit (low=left,high=right)

KX_API(int,kx_get_spdif_i2s_status(kx_hw *hw,kx_spdif_i2s_status *st));

// GP IO
KX_API(byte,kx_get_gp_inputs(kx_hw *hw));
KX_API(void,kx_set_gp_outputs(kx_hw *hw,byte output));

// spectral / mixer func
KX_API(int,kx_get_voice_info(kx_hw *hw,int what,kx_voice_info *vi));
#define KX_VOICE_INFO_USAGE 0
#define KX_VOICE_INFO_SPECTRAL  1

// AC-3 passthru
KX_API(int,kx_init_passthru(kx_hw *hw,void *instance,int *method)); // method is not user-controllable
KX_API(int,kx_close_passthru(kx_hw *hw,void *instance));
KX_API(int,kx_set_passthru(kx_hw *hw,void *instance,int onoff));

KX_API(int,kx_get_device_caps(kx_hw *hw,kx_caps *caps,int *sz));

#endif
