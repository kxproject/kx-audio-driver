// kX Mixer
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


#include "stdinc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DEFINE_GUID(KSDATAFORMAT_SUBTYPE_DOLBY_AC3_SPDIF,0x00000092, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);

typedef struct ac3_decode_t
{
   HWAVEIN wave_in;
   HWAVEOUT wave_out;

   #define TOTAL_BUFFERS	16	// at least 4!

   WAVEHDR in_buffer[TOTAL_BUFFERS];
   WAVEHDR out_buffer[TOTAL_BUFFERS];

   #define BUFFER_LENGTH	(1536*4)
   byte in_data[TOTAL_BUFFERS][BUFFER_LENGTH];
   byte out_data[TOTAL_BUFFERS][BUFFER_LENGTH];

   iKX *ikx_t;
   int device_num;

   uintptr_t thread;

   HANDLE done,out_done,in_done;

   int last_in_buffer;
   int last_out_buffer;
}ac3_decode;

static int first_time=1;
static ac3_decode *decoders[MAX_KX_DEVICES];

static void CALLBACK wave_out_proc(HWAVEOUT hwo,UINT uMsg,DWORD_PTR dwInstance,DWORD_PTR dwParam1,DWORD dwParam2);
static void CALLBACK wave_in_proc(HWAVEIN hwo,UINT uMsg,DWORD_PTR dwInstance,DWORD_PTR dwParam1,DWORD dwParam2);
static void __cdecl ac3_thread(void *);

int create_decoder(int dev)
{
 if(dev<0 || dev>=MAX_KX_DEVICES)
 {
  debug("kxmixer: invalid call [ac3 decode] - %d\n",dev);
  return -4;
 }

 if(first_time)
 {
  first_time=0;
  memset(&decoders,0,sizeof(decoders));
 }

 if(decoders[dev]==0)
 {
    UINT wavein_num=manager->get_ikx(dev)->get_winmm_device(KX_WINMM_WAVE,0);
    UINT waveout_num=manager->get_ikx(dev)->get_winmm_device(KX_WINMM_WAVE,1);
    if(wavein_num==0xffffffff)
    {
     debug("!!! kxmixer: error enumerating wave-in [%x %d]\n",GetLastError(),wavein_num);
     return -1;
    }
    if(waveout_num==0xffff)
    {
     debug("!!! kxmixer: error enumerating wave-out [%x %d]\n",GetLastError(),waveout_num);
     return -1;
    }

    ac3_decode *ac3;
    ac3=(ac3_decode *)malloc(sizeof(ac3_decode));
    if(ac3)
    {
	memset(ac3,0,sizeof(ac3_decode));

	decoders[dev]=ac3;

	ac3->ikx_t=manager->get_ikx(dev);
	ac3->device_num=dev;

	ac3->last_in_buffer=-1;
	ac3->last_out_buffer=-1;

        MMRESULT res;

        WAVEFORMATEXTENSIBLE format;
        format.Format.nChannels=2;
        format.Format.nSamplesPerSec=48000;
        format.Format.nAvgBytesPerSec=48000*4;
        format.Format.nBlockAlign=2*16/8;
        format.Format.wBitsPerSample=16;

        format.Samples.wValidBitsPerSample=16;
        format.dwChannelMask=3;
        memcpy(&format.SubFormat,&KSDATAFORMAT_SUBTYPE_DOLBY_AC3_SPDIF,sizeof(format.SubFormat));

        format.Format.cbSize=0;
        format.Format.wFormatTag=WAVE_FORMAT_PCM;
        //format.Format.cbSize=22;
        //format.Format.wFormatTag=WAVE_FORMAT_EXTENSIBLE;

        res=waveInOpen(&ac3->wave_in,wavein_num,(WAVEFORMATEX *)&format,(DWORD_PTR)wave_in_proc,(DWORD_PTR)ac3,CALLBACK_FUNCTION);
        if(res==MMSYSERR_NOERROR)
        {
          format.Format.cbSize=22;
          format.Format.wFormatTag=WAVE_FORMAT_EXTENSIBLE;

          res=waveOutOpen(&ac3->wave_out,waveout_num,(WAVEFORMATEX *)&format,(DWORD_PTR)wave_out_proc,(DWORD_PTR)ac3,CALLBACK_FUNCTION);
          if(res==MMSYSERR_NOERROR)
          {
           for(int i=0;i<TOTAL_BUFFERS;i++)
           {
            memset(&ac3->in_buffer[i],0,sizeof(ac3->in_buffer[i]));
            memset(&ac3->out_buffer[i],0,sizeof(ac3->out_buffer[i]));

            ac3->in_buffer[i].lpData=(LPSTR)ac3->in_data[i];
            ac3->in_buffer[i].dwBufferLength=BUFFER_LENGTH;
            ac3->in_buffer[i].dwUser=i+1;

            if(waveInPrepareHeader(ac3->wave_in,&ac3->in_buffer[i],sizeof(ac3->in_buffer[i]))!=MMSYSERR_NOERROR)
            {
             debug("kxmixer: error preparing wavein header [%x]\n",GetLastError());
             res=-1;
             break;
            }

            ac3->out_buffer[i].lpData=(LPSTR)ac3->out_data[i];
            ac3->out_buffer[i].dwBufferLength=BUFFER_LENGTH;
            ac3->out_buffer[i].dwUser=i+1;

            if(waveOutPrepareHeader(ac3->wave_out,&ac3->out_buffer[i],sizeof(ac3->out_buffer[i]))!=MMSYSERR_NOERROR)
            {
             debug("kxmixer: error preparing waveout header [%x]\n",GetLastError());
             res=-1;
             break;
            }
           }
           if(res==0)
           {
            res=waveInAddBuffer(ac3->wave_in,&ac3->in_buffer[0],sizeof(WAVEHDR));
            if(res==0)
            {
             res=waveInAddBuffer(ac3->wave_in,&ac3->in_buffer[1],sizeof(WAVEHDR));
             if(res==0)
             {
               res=waveInAddBuffer(ac3->wave_in,&ac3->in_buffer[2],sizeof(WAVEHDR));
               if(res==0)
               {
                debug("kxmixer: ac-3 decoding engine instantiated\n");
                //  create thread here:
                ac3->thread=_beginthread(ac3_thread,0,ac3);
                return 0;
               }
               else
                debug("kxmixer: error adding third wave-in buffer [%x]\n",GetLastError());
             }
             else
              debug("kxmixer: error adding second wave-in buffer [%x]\n",GetLastError());
            }
            else
             debug("kxmixer: error adding wave-in buffer [%x]\n",GetLastError());
           }
          }
          else debug("!!! kxmixer: error opening wave-out [%x]\n",GetLastError());
        } else debug("!!! kxmixer: error opening wave-in [%x]\n",GetLastError());

        destroy_decoder(dev);

    } else debug("kxmixer: ac3 decoder: not enought memory!\n");
  }
  else
   debug("!! kxmixer: ac3 decoder already started\n");

  return 0;
}

int destroy_decoder(int dev)
{
 if(dev<0 || dev>=MAX_KX_DEVICES)
  return -4;

 if(decoders[dev])
 {
 	   ac3_decode *ac3=decoders[dev];

           if(ac3->done)
            SetEvent(ac3->done);

           if(ac3->thread)
           {
            debug("kxmixer: waiting for ac-3 decoding thread to finish...\n");
            WaitForSingleObject((HANDLE)ac3->thread,1000);
           }

           for(int i=0;i<TOTAL_BUFFERS;i++)
           {
            if(ac3->in_buffer[i].dwUser==i+1)
             waveInUnprepareHeader(ac3->wave_in,&ac3->in_buffer[i],sizeof(ac3->in_buffer[i]));
            if(ac3->out_buffer[i].dwUser==i+1)
             waveOutUnprepareHeader(ac3->wave_out,&ac3->out_buffer[i],sizeof(ac3->out_buffer[i]));
           }

           if(ac3->wave_out)
            waveOutClose(ac3->wave_out);
           if(ac3->wave_in)
            waveInClose(ac3->wave_in);

           ac3->ikx_t=NULL;
           ac3->device_num=0;

           CloseHandle(ac3->done);
           CloseHandle(ac3->out_done);
           CloseHandle(ac3->in_done);

           memset(ac3,0,sizeof(ac3_decode));

           decoders[dev]=NULL;
           free(ac3);
  }

  return 0;
}

void CALLBACK wave_out_proc(HWAVEOUT hwo,UINT uMsg,DWORD_PTR dwInstance,DWORD_PTR dwParam1,DWORD dwParam2)
{
 if(uMsg==WOM_DONE)
 {
    ac3_decode *ac3=(ac3_decode *)dwInstance;
    if(ac3)
    {
      WAVEHDR *h=(WAVEHDR *)dwParam1;
      if(h)
      {
//        SetEvent(ac3->out_done);
      }
    }
 }
}

void CALLBACK wave_in_proc(HWAVEIN hwo,UINT uMsg,DWORD_PTR dwInstance,DWORD_PTR dwParam1,DWORD dwParam2)
{
 if(uMsg==WIM_DATA)
 {
    ac3_decode *ac3=(ac3_decode *)dwInstance;
    if(ac3)
    {
      WAVEHDR *h=(WAVEHDR *)dwParam1;
      if(h)
      {
        intptr_t tmp=h->dwUser;
        intptr_t next=tmp+3;
        if(next>TOTAL_BUFFERS)
         next-=TOTAL_BUFFERS;

        waveInAddBuffer(ac3->wave_in,&ac3->in_buffer[next-1],sizeof(WAVEHDR));

	memcpy(ac3->out_buffer[ac3->last_out_buffer-1].lpData,ac3->in_buffer[tmp-1].lpData,BUFFER_LENGTH);

        SetEvent(ac3->in_done);
      }
    }
 }
}

static void __cdecl ac3_thread(void *p)
{
 ac3_decode *ac3=(ac3_decode *)p;

 ac3->done=CreateEvent(NULL,0,0,0);
 ac3->out_done=CreateEvent(NULL,0,0,0);
 ac3->in_done=CreateEvent(NULL,0,0,0);

 if(SetThreadPriority((HANDLE)ac3->thread,THREAD_PRIORITY_HIGHEST)==0)
 {
  debug("kxmixer: error setting thread priority [%x]\n",GetLastError());
 }
 // else debug("kxmixer: set thread priority to highest [%x]\n",ac3->thread);

 if(SetThreadExecutionState(ES_SYSTEM_REQUIRED)==NULL)
 {
  debug("kxmixer: cannot set execution thread state -- might cause problems with APM...\n");
 }

 if(ac3->done && ac3->in_done && ac3->out_done)
 {
  // debug("kxmixer: thread created and active...starting rec...\n");

  waveInStart(ac3->wave_in);

  ac3->last_out_buffer=1;
  ac3->last_in_buffer=1;
  
  while(1)
  {
   // WaitForMultipleObject including 'done' event...
   HANDLE list[3];
   list[0]=ac3->done; list[1]=ac3->out_done; list[2]=ac3->in_done;
   int ret=WaitForMultipleObjects(3,list,0,INFINITE);
   if(ret==WAIT_OBJECT_0) // done
   {
    break;
   }
   if(ret==WAIT_OBJECT_0+1) // out
   {
   }
   if(ret==WAIT_OBJECT_0+2) // in
   {
     	waveOutWrite(ac3->wave_out,&ac3->out_buffer[ac3->last_out_buffer-1],sizeof(WAVEHDR));

     	ac3->last_out_buffer++;
     	if(ac3->last_out_buffer>TOTAL_BUFFERS)
         ac3->last_out_buffer=1;
   }
  }

  waveInStop(ac3->wave_in);

  debug("kxmixer: thread exited...\n");
 } else debug("kxmixer: error creating thread events\n");
}
