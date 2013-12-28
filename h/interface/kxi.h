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

// kxi.h
// -----
// the main iKX class used for controlling kX Audio driver
// -----
// do not include this file directly, use 'kxapi.h' instead


#ifndef _iKX_INTERFACE__H_
#define _iKX_INTERFACE__H_

#if defined(WIN32) 
    // these are for KS library
    class CKsEnumFilters;
    class CKsAudRenFilter;
    class CKsAudRenPin;
#endif

#if defined(__APPLE__)
    #include <IOKit/IOKitLib.h>
#endif

class KX_CLASS_TYPE iKX
{
public:
    // creation and destruction:

    iKX();
    static iKX* create(int id=0);   // this is identical to new iKX; ikx->init(id);
    virtual void destroy(void);     // this is identical to ikx->close(); delete this;
    int close();

    #if defined(WIN32)
        int init(int device=0,int ignore_winmm=0); // 0-first; 1-second etc;
        ~iKX();
    #else
        int init(int device=0); // 0-first; 1-second etc;
        virtual ~iKX();
    #endif

    // kX Interface version
    const char *get_version();
    dword get_version_dword();

    // low-level access: in general, should not be used directly
    // prop_func
    #define KX_WAVE     0x10000000
    #define KX_TOPO     0x20000000
    #define KX_SYNTH    0x40000000
    #define KX_UART     0x80000000

    // prop_op
    #define KX_PROP_GET 0x0100000
    #define KX_PROP_SET 0x0200000

    // (only) these functions return: 0: ok; else - failed
    // prop is: prop_func | prop_op | prop_item
    int ctrl(dword prop,void *buff,int bsize,int *ret_bytes);

#if defined(WIN32)  
    // func is: prop_func
    int raw_ctrl(dword func,void *prop,int psize,void *buff,int bsize,int *ret_bytes);
    
    // op is prop_op
    int mixer(int op,int node,int channel,int *val);
        
    // nodes for mixer(); note: mute should be control+1
    #define KX_MIXER_MASTER     0x0
    #define KX_MIXER_MASTER_MUTE    0x1
    #define KX_MIXER_WAVE       0x2
    #define KX_MIXER_WAVE_MUTE  0x3
    #define KX_MIXER_SYNTH      0x4
    #define KX_MIXER_SYNTH_MUTE 0x5
    #define KX_MIXER_WAVE23     0x6
    #define KX_MIXER_WAVE23_MUTE    0x7
    #define KX_MIXER_WAVE45     0x8
    #define KX_MIXER_WAVE45_MUTE    0x9
    #define KX_MIXER_WAVE67     0xa
    #define KX_MIXER_WAVE67_MUTE    0xb
    #define KX_MIXER_LINEIN     0xc
    #define KX_MIXER_LINEIN_MUTE    0xd
    #define KX_MIXER_MICIN      0xe
    #define KX_MIXER_MICIN_MUTE 0xf
    #define KX_MIXER_REC        0x10
    #define KX_MIXER_REC_MUTE   0x11
    #define KX_MIXER_WAVEHQ     0x12
    #define KX_MIXER_WAVEHQ_MUTE    0x13
    #define KX_MIXER_LAST       0x14
        
#endif // WIN32
    
        // DSP emulation; this normally should not be used
        int set_dsp(int card); // 0: 10k1; 1: 10k2
        int get_dsp(void);  // 0: 10k1; 1: 10k2

        // user-friendly functions: HAL
        int ac97_read(byte reg,word *val);
        int ac97_write(byte reg,word val);
        int ac97_reset();
        int ptr_read(dword reg,dword chn,dword *val);
        int ptr_write(dword reg,dword chn,dword val);
        int fn0_read(dword reg,dword *val);
        int fn0_write(dword reg,dword val);
        int p16v_read(dword reg,dword *val);
        int p16v_write(dword reg,dword val);

        // set/get GPIO bits
        int gpio_read(dword *in);
        int gpio_write(dword out);

        // WDM driver dword and string parameters; see ikx.h
        int get_dword(int n,dword *val);
        int get_string(int n,char *str);

        // generic capabilities (mainly for ASIO)
        int get_device_caps(kx_caps *caps,int *sz); // sz is in bytes; on return, *sz contains actual buffer size

        // Effects API: routing: aaBBccDD; xrouting: eeFFggHH (10k2 only)
        int get_routing(int ndx, dword *routing, dword *xrouting=0);
        int set_routing(int ndx, dword routing, dword xrouting=0x3f3f3f3f);
        // Amount: 0x0-0xff linear
        int get_send_amount(int ndx,byte *amount);
        int set_send_amount(int ndx,byte amount);
        // p16v amounts
        int get_p16v_volume(int chn,dword *value);
        int set_p16v_volume(int chn,dword value);

        // id is logical; val is register type specific; values are _physical_
        // generally, should be used for GPRs only
        int set_dsp_register(int pgm,const char *name,dword val);
        int get_dsp_register(int pgm,const char *name,dword *val);
        int set_dsp_register(int pgm,word id,dword val);
        int get_dsp_register(int pgm,word id,dword *val);
        // the same, but register type specific
        int set_tram_addr(int pgm,const char *name,dword addr); // addr is relative to tram_start
        int get_tram_addr(int pgm,const char *name,dword *addr);
        int set_tram_flag(int pgm,const char *name,dword flag); // flag is TRAM_READ || TRAM_WRITE
        int get_tram_flag(int pgm,const char *name,dword *flag);
        int set_tram_addr(int pgm,word id,dword addr); // addr is relative to tram_start
        int get_tram_addr(int pgm,word id,dword *addr);
        int set_tram_flag(int pgm,word id,dword flag); // flag is TRAM_READ || TRAM_WRITE
        int get_tram_flag(int pgm,word id,dword *flag);

        int translate_text2id(kx_text2id *ti);

        int write_instruction(int pgm,int offset,word op,word z,word w,word x,word y,int valid=0xffffffff);
        int read_instruction(int pgm,int offset,word *op,word *z,word *w,word *x,word *y);

    int translate_microcode(int pgm,int place=KX_MICROCODE_ANY,int pos_pgm=0);
        int untranslate_microcode(int pgm);

    int enable_microcode(int pgm);
    int disable_microcode(int pgm);
    int set_microcode_bypass(int pgm,int state); // 0=ByPass=Off; 1=ByPass=On
    int set_microcode_flag(int pgm,dword state); // (dsp_microcode.flag)
            // set_microcode_flag should not be used for enabling/disabling/bypassing
                        // (although this is supported in kernel)
    int get_microcode_flag(int pgm,dword *state); // (dsp_microcode.flag)

    int dsp_go();
    int dsp_stop();
    int dsp_clear(); // unloads all uploaded microcode; frees all hw resources; assumes dsp_stop();

    int mute();
    int unmute();

        int get_spdif_i2s_status(kx_spdif_i2s_status *);

    // returns pgm id or <=0 if failed
    int load_microcode(const char *name,const dsp_code *code,int code_size,
                           const dsp_register_info *info,int info_size,int itramsize,int xtramsize,
                           const char *copyright,
                           const char *engine,
                           const char *created,
                           const char *comment,
                           const char *guid,
                           int force_pgm_id=0);
    int unload_microcode(int pgm);
    int update_microcode(int pgm_id,const char *name,dsp_code *code,int code_size,
                             dsp_register_info *info,int info_size,int itramsize,int xtramsize,
                             const char *copyright,
                             const char *engine,
                             const char *created,
                             const char *comment,
                             const char *guid,
                             unsigned int flag);
                             // IKX_UPDATE_... flags are declared in the ikx.h file

    int connect_microcode(int pgm1,word src,int pgm2,word dst);
    int connect_microcode(int pgm1,const char *src,int pgm2,const char *dst);
        // if pgm2==-1 -> dst='physical input/physical output'
    int disconnect_microcode(int pgm,word src);
    int disconnect_microcode(int pgm,const char *src);
        // if pgm==-1 -> src='physical output'
    int get_connections(int pgm,kxconnections *out,int size);
        // if size==0 -> returns needed buffer size

        // all names (name,copyright,engine,created,comment,guid) should be at least .. bytes
        // (KX_MAX_STRING)

    // returns >=0 if ok; 
    // applications _should_ set *info & *code = NULL before calling this function
    // and call LocalFree for *code & *info (if !=NULL) to free allocated memory
    // ret <0 - error
    // max_size: max size of void *rifx memory block
    int parse_rifx(void *rifx,int max_size,char *name,dsp_code **code,int *code_size,
                       dsp_register_info **info,int *info_size,int *itramsize,int *xtramsize,
                       char *copyright,
                       char *engine,
                       char *created,
                       char *comment,
                       char *guid);

        int format_reg(kString *s,word reg,const dsp_register_info *info,int info_size);
        int format_opcode(kString *s,word op,int fl=0); // if fl=1, print 'help', not the opcode

        int reset_microcode(void); // clears and re-loads pre-defined microcode -- 're-initialize'
        int reset_voices(void);
        int enum_microcode(int pgm,dsp_microcode *mc);
        int enum_microcode(const char *pgm_id,dsp_microcode *mc);
            // enums all microcode
        int enum_microcode(dsp_microcode *mc_ret,int size); // if size is 0 - returns needed array size
        int get_microcode(int pgm,dsp_code *code,int code_size,
                          dsp_register_info *info,int info_size);
        // info_size & code_size - sizes of code&info buffers (in bytes);
        // if they dont match ones got by enum_microcode -> fuction will fail
        int set_microcode_name(int pgm_id,const char *new_name,int what=KX_MICROCODE_NAME);

        // Dane-related API:
        #define KX_DISASM_REGS      0x1
        #define KX_DISASM_CODE      0x2
        #define KX_DISASM_VALUES    0x4

        #define KX_DISASM_DANE      (KX_DISASM_REGS|KX_DISASM_CODE)
        #define KX_DISASM_CPP       0x1000  /* C++ massives format; all other flags are ignored */
        // in general, use KX_DISASM_DANE or KX_DISASM_CPP

        // 'out' is constructed and initialized by the caller; output format: Dane compatible
        int disassemble_microcode(kString *out,
                                  int flag,int pgm,
                                  const dsp_code *code,int code_size,const dsp_register_info *info,int info_size,
                                  int itramsize,int xtramsize,
                                  const char *name,const char *copyright,const char *engine,
                                  const char *created,const char *comment,const char *guid);
                                // pgm - real PGM id (if you use 'KX_DUMP_VALUES'); else use '-1'

        // input: Dane compatible source
        // info & code should be freed by the caller with LocalFree
        // string sizes should be at least 128 bytes;
        // err should be provided by the caller
        int assemble_microcode(char *buf,kString *err,char *name,dsp_code **code,int *code_size,
                               dsp_register_info **info,int *info_size,int *itramsize,int *xtramsize,
                               char *copyright,
                               char *engine,
                               char *created,
                               char *comment,
                               char *guid);

        // iKXPlugin / iKXAddOn API-related functions
        // NOTE: iKX API assumes iKXAddOns and iKXPlugins are 'objects',
        //  although from kX Mixer point of view these are two different objects!
        void *instantiate_object(const char *library,int id); // returns 0 if failed; id is 0..(get_plugin_count()-1)
        void *instantiate_object(const char *library,const char *guid); // returns 0 if failed
        int delete_object(iKXPlugin *plugin); // call delete_plugin to delete iKXPlugin object;
                            // do _not_ call 'delete plugin';
        int delete_object(iKXAddOn *addon); // call delete_addon to delete iKXAddOn object;
                            // do _not_ call 'delete addon';
        int get_object_type(const char *fname); // types: see below; <0 - error
        
        // types:
        #define KX_OBJECT_KXL   0
        #define KX_OBJECT_DANE  1
        #define KX_OBJECT_RIFX  2
        #define KX_OBJECT_ADDON 3

        int get_object_count(const char *library); // returns <=0 if failed

        int get_object_name(const char *library,int id,char *name); // returns 0 if succeeded; name should be at least KX_MAX_STRING size
        int get_object_guid(const char *library,int id,char *ret_guid); // returns <0 if failed; 0 - success

        // id's: see kx.h
        int set_buffers(int id,int value);
        int get_buffers(int id,int *value);

        // SoundFont
        int load_soundfont(kx_sound_font *fnt,int partial=0); // returns id or <0 -error
        int load_soundfont_x(kx_sound_font *fnt,const char *fname,long f_pos); // returns id or <0 -error
        int unload_soundfont(int id);
        int enum_soundfonts(sfHeader *hdr,int size); // if size==0 - ret is # of bytes needed
        // on exist hdr[..].rom_ver = SF id  (.minor)
        int compile_soundfont(char *dir,char *fname,dword sfman_id=0,dword subsynth_=0); // (if fname==NULL - auto upload)
        int parse_soundfont(char *fname,char *dir,dword sfman_id=0,dword subsynth_=0);
            // if dir==NULL - auto upload; 
                // if sfman_id==VIENNA_DYNAMIC - use Vienna-styled structures
                //    Vienna-style structures may lack 'RIFF' header and [samples]
                //    refer to CViSmplObject objects
            // fname can also be "mem://<hex addr>";
                // subsynth_: 0-both; 1-Synth1 only; 2-Synth2 only
        int get_preset_description(int bank,int preset,char *name); 
           // max size is 20 (soundfont.h)
           // if preset==-1, return (dword) enum_soundfonts index# in 'name'

        // DirectSynth / Vienna architecture
        int send_synth(int synth_id,dword data);

        // low-level hardware parameter
        int get_hw_parameter(int id,dword *value);
        int set_hw_parameter(int id,dword value);

        // settings
        int reset_settings();   // resets FXAmounts, Routings and XRoutings (incl. ASIO)
                    // and buffer sizes and TRAM size
                                // and HW parameters to 'default' values (e.g. "swap front/rear"=true)
        int reset_db();     // re-initalizes daughterboard card

        // mixer voice info
        int get_voice_info(kx_voice_info *vi);
        int get_spectral_info(kx_voice_info *vi);

        // topology/dsp assignments
        int get_dsp_assignments(kx_assignment_info *ai);
        int set_dsp_assignments(kx_assignment_info *ai);
    
        // card-dependent text strings
        int get_description(int what,int p,char *name); // name should be -at-least- KX_MAX_STRING
                                // 'what' -- see ikx.h

        int init_passthru(void **instance,int *method); // in: none; out: method will contain ac-3 passthru method (ignored)
        int close_passthru(void *instance);
        int set_passthru(void *instance,int onoff); // 1/0

        // E-DSP FPGA
        int upload_fpga_firmware(byte *data,int size);
        int fpga_read(byte reg,byte *val);
        int fpga_write(byte reg,byte val);
        int fpga_link_src2dst(dword src, dword dst);

        int set_clock(int frequency); // FIXME: preliminary, will include spdif/adat/.. clock selection later
        int get_clock(int *frequency); // FIXME: preliminary, will include spdif/adat/.. clock selection later

#if defined(WIN32)  
        // ASIO
        // ----
        int asio_init(int method); // should be called _before_ any other asio call
        int asio_close(void);
        int asio_alloc(int fl,int chn,void **addr,int size,int rate,int bps); // =0 - ok; returns *addr; size is in bytes
        // rate: should be 48000 for rec;
        // fl:
        #define ASIO_INPUT  0x0
        #define ASIO_OUTPUT 0x1
        int asio_free(int fl,void *addr);
        int asio_get_position(void);
        int asio_wait(int dbuffer,volatile bool *done); // current Toggle value
        int asio_timer(int set_reset,int value,int &new_value); // set: 1, reset: 0 [value is ignored for 'reset']
                            // value: milliseconds*10000
                                                // that is, 1*10000=10000=1 millisecond

        int asio_start(void);
        int asio_stop(void);

        // send is 0..7 (0..3 for 10k1); -> send_a,b,c,d; _e,f,g,h
        // deprecated since 3544:
        // int asio_set_send_amount(int chn,int send,byte amount);
        // int asio_get_send_amount(int chn,int send,byte *amount);

        int driver_property(void *iks,dword prop,void *buff,int bsize,int *ret_bytes=NULL);
        // void *iks -> IKsPropertySet for USE_DSOUND_ASIO mode or pRenderPin->GetHandle() for KS mode
        // prop is: prop_op | prop_item; (prop_func is assumed to be KX_WAVE)
        // returns: 0 - ok

        // DirectSound-related:
        // --------------------
        LPGUID get_dsound_guid(int what); // pointer is stored inside iKX class instance
        // 'what' is preferred wave device: KX_WINMM_WAVE etc...

        int set_dsound_routing(void *sd, dword routing,dword xrouting=0x3f3f3f3f);
        int get_dsound_routing(void *sd, dword *routing,dword *xrouting=NULL);
        // dword sd -> LPDIRECTSOUNDBUFFER
        // returns: 0 - ok
        int dsound_property(void *iks,dword prop,void *buff,int bsize,int *ret_bytes=NULL);
        // void *iks -> IKsPropertySet
        // prop is: prop_op | prop_item; (prop_func is assumed to be KX_WAVE)
        // returns: 0 - ok
#endif
    
        int get_device_num(); // parameter to init(...);
        const char *get_device_name();
        const char *get_error_string();
    
        int generate_guid(char *guid); // should be at lease KX_MAX_STRING

private:    
        // recent error
        char error_name[128];
    
        dword is_10k2;  // emulation, if no hardware is present; is set by set/get_dsp()
public:
    
#if defined(WIN32)  
        const char *get_winmm_name(int what);
        #define KX_WINMM_MIXER  0
        #define KX_WINMM_WAVE   1
        #define KX_WINMM_SYNTH  2
        #define KX_WINMM_SYNTH2 3
        #define KX_WINMM_UART   4
        #define KX_WINMM_UART2  5
        #define KX_WINMM_CTRL   6
        #define KX_WINMM_WAVE23 7
        #define KX_WINMM_WAVE45 8
        #define KX_WINMM_WAVE67 9
        #define KX_WINMM_WAVEHQ 10

        #define KX_MAX_WAVE_DEVICES 5

        unsigned int get_winmm_device(int what,int subdevice); // returns 0xffffffff if failed
            // subdevice: 0 - In, 1 - Out; use '0' for KX_WINMM_MIXER

        // DirectKX API
        int audio_close(int num);
        int audio_stop(int num);
        int audio_start(int num);
        int audio_get_position(int num,int *pos);
        int audio_set_parameter(int num, int ch,int param,dword value);
        int audio_get_parameter(int num, int ch,int param,dword *value);
        int audio_open(void *buffer, size_t bufsize,int usage,int sampling_rate);
        // usage: stereo/mono 8/16bit playback; + flags; voice_open_notimer
        // voice_open_notimer is assumed!

        // ----------------------------------------------------------------
        // these should not be accessed directly...
        // because of compatibility issues between different kXAPI versions
        // ----------------------------------------------------------------

private:
        #ifndef MAXPNAMELEN // from Windows headers
         #define MAXPNAMELEN 32
        #endif

        // WinMM parameters (device names)
        char mixer_name[MAXPNAMELEN];
        char wave_name[MAXPNAMELEN];
        char synth_name[MAXPNAMELEN];
        char synth2_name[MAXPNAMELEN];
        char uart_name[MAXPNAMELEN];
        char uart2_name[MAXPNAMELEN];
        char ctrl_name[MAXPNAMELEN];
        char wave23_name[MAXPNAMELEN];
        char wave45_name[MAXPNAMELEN];
        char wave67_name[MAXPNAMELEN];
        char waveHQ_name[MAXPNAMELEN];

        int device_num;
        char device_name[KX_MAX_STRING];

        void *hWave,*hTopo,*hUart,*hUart2,*hSynth,*hSynth2; // CreateFile() Handlers
        char wave_path[256]; // SetupDi... device file paths
        char topo_path[256];

public: // internal use by kxsetup
        int init_winmm();
        int get_asio_method();
        HANDLE get_asio_event();
private:
// WIN32 is already defined above
#if defined(KX_INTERFACE) || defined(KX_SETUP)
    // this is made private to avoid directx stuff to be included

    uintptr_t mixer_handler[KX_MAX_WAVE_DEVICES];

    // ASIO:
    // ikX API specific
    void *asio_mem_table[MAX_ASIO_OUTPUTS+MAX_ASIO_INPUTS]; // buffers or NULL if none
    int last_asio_voice;
    int asio_method; // full bitfield
    HANDLE asio_user_event;

    // ASIO -> DirectSound specific
    LPDIRECTSOUND8 asio_ds;
    LPDIRECTSOUNDBUFFER asio_prim_buffer;
    LPDIRECTSOUNDBUFFER asio_secondary_buffer;
    IKsPropertySet *asio_iks_property; // this is actuall pRenderPin->GetHandle() for non-USE_DSOUND_ASIO builds [KS]
    
    CKsEnumFilters *pRenderEnumerator;
    CKsAudRenFilter *pRenderFilter;
    CKsAudRenPin *pRenderPin;

    GUID guid_;

    // ASIO kX specific
    dword asio_hw_start;
    int asio_inited;
    asio_notification_t *asio_notification;
#else
    uintptr_t tmp[KX_MAX_WAVE_DEVICES+MAX_ASIO_OUTPUTS+MAX_ASIO_INPUTS+1+1+1+1+1+1+1+1+1+1+sizeof(GUID)+1+1+1];
#endif

    // DirectX initialization
    int init_directx(int what);
    int init_kstream(void);
#endif // WIN32-specific

#if defined(__APPLE__)  
private:
    // these are private fields for accessing IOService
    io_service_t            service;
    io_iterator_t           iterator;
    io_connect_t            connect;
    
    int device_num;
    char device_name[KX_MAX_STRING];
#endif // OSX-specific
};

#endif
