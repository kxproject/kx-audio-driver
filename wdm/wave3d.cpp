// kX WDM Audio Driver
// Copyright (c) Eugene Gavrilov, 2001-2014.
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


// kX 3-D Engine
// -------------
// wrapper between kx3d and directsound / WDM

#pragma code_seg()
// apply is called within a spinlock

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
void apply(register CMiniportWaveOutStream *that,register kSource *source)
{
    source->update&=(~that->Miniport->kx3d_compat);

//  debug(DWDM,"(3d): update: %x\n",source->update);

/*        debug(DWDM,"(3d): min: %d max: %d distance: %d bads: %d%d%d\n",
         (int)source->min_distance,
         (int)source->max_distance,
         (int)source->distance,
         source->bad_angle,
         source->bad_orientation,
         source->bad_cone);
*/
        if(source->update&UPDATE_VOL)
        {
/*         debug(DWDM,"(3D): azimuth / elev: %d/%d [pos: %d %d %d / ori: %d %d %d]\n",
           source->azimuth,
           source->elevation,
           (int)(source->p_position.x*100.0f),
           (int)(source->p_position.y*100.0f),
           (int)(source->p_position.z*100.0f),
           (int)(source->r_position.x*100.0f),
           (int)(source->r_position.y*100.0f),
           (int)(source->r_position.z*100.0f)
           );
*/
         that->kx3d_attn=source->distance_attn+source->cone_attn+source->direct*0x10000/100;

/*         debug(DWDM,"attns: distance: %d cone: %d - total: %d; vol: %d direct: %d -> final: %d\n",
          source->distance_attn/0x10000,
          source->cone_attn/0x10000,
          that->kx3d_attn/0x10000,
          that->vol1[0]/0x10000,
          source->direct,
          (dword)((int)that->vol1[0]+that->kx3d_attn));
*/

         kx_wave_set_parameter(that->hw,that->channels[0],0,KX_VOICE_VOLUME,(dword)((int)that->vol1[0]+that->kx3d_attn));
         source->update|=UPDATE_EAX;
         source->update&=~UPDATE_VOL;
        }
        if(source->update&UPDATE_PITCH)
        {
         that->current_freq_multiplier=(int)(source->doppler*(float)DOPPLER_MULT);

/*         debug(DWDM,"doppler: %d (vl: %d, vs: %d); dv: %d; vel1: %d %d %d; vel2: %d %d %d; distF:%d doppF:%d / %d\n",
          (int)(source->doppler*100.0f),
          (int)(source->d_vl*100.0f),
          (int)(source->d_vs*100.0f),
          (int)(source->listener->doppler_velocity*100.0f),
          (int)(source->r_velocity.x*100.0f),
          (int)(source->r_velocity.y*100.0f),
          (int)(source->r_velocity.z*100.0f),
          (int)(source->listener->velocity.x*100.0f),
          (int)(source->listener->velocity.y*100.0f),
          (int)(source->listener->velocity.z*100.0f),
          (int)(source->listener->distance_factor*100.0f),
          (int)(source->listener->doppler_factor*100.0f),
          (int)(source->eax_doppler_factor*100.0f));
*/
         if(that->current_freq*that->current_freq_multiplier/DOPPLER_MULT>120000)
          that->current_freq_multiplier=(120000*DOPPLER_MULT/that->current_freq);
          
         kx_wave_set_parameter(that->hw,
          that->channels[0],
          0,KX_VOICE_PITCH,
          that->current_freq*that->current_freq_multiplier/DOPPLER_MULT);

         source->update&=~UPDATE_PITCH;
        }

//        int deb=(source->update&(UPDATE_FXBUS|UPDATE_AMOUNT));

        if(source->update&UPDATE_FXBUS)
        {
         dword rt;

         rt=KX_MAKE_ROUTING(
               source->listener->routes[source->send[0]],
               source->listener->routes[source->send[1]],
               source->listener->routes[FX1],
               source->listener->routes[FX2]);

         kx_wave_set_parameter(that->hw,that->channels[0],0,
           KX_VOICE_ROUTINGS,
           rt);

         source->update&=~UPDATE_FXBUS;
        }
        if(source->update&UPDATE_AMOUNT)
        {
         // set pans
         that->hw->voicetable[that->channels[0]].param.send_a=source->send_amount[0];
         that->hw->voicetable[that->channels[0]].param.send_b=source->send_amount[1];

         kx_wave_set_parameter(that->hw,that->channels[0],0,
           KX_VOICE_PAN+KX_VOICE_UPDATE,
           0);

         // debug(DWDM,"(3D): amounts: %x / %x\n",source->send_amount[0],source->send_amount[1]);
         source->update&=~UPDATE_AMOUNT;
         // source->update|=UPDATE_EAX;
        }

        int attn=source->cone_attn+source->distance_attn;
        int p=0;

        if(source->update&UPDATE_ROOM)
        {
         // ??
         source->update&=(~UPDATE_ROOM);
        }
        if(source->update&UPDATE_ROOM_HF)
        {
         p=source->total_room_hf+attn+source->cone_attn_hf;
         source->update&=(~UPDATE_ROOM_HF);
        }
        if(source->update&UPDATE_ROOM_LF)
        {
         p=source->total_room_lf+attn;
         source->update&=(~UPDATE_ROOM_LF);
        }
        if(source->update&UPDATE_DIRECT_HF)
        {
         p=source->total_direct_hf+attn+source->cone_attn_hf;
         source->update&=(~UPDATE_DIRECT_HF);
        }
        if(source->update&UPDATE_DIRECT_LF)
        {
         p=source->total_direct_lf+attn;
         source->update&=(~UPDATE_DIRECT_LF);
        }

//        if(deb)
/*        {
         dword d1, d2;
         d1=kx_readptr(that->hw, PTAB,that->channels[0]);
         d2=kx_readptr(that->hw, that->hw->is_10k2?FXRT1_K2:FXRT_K1, that->channels[0]);

         debug(DWDM,"(3D): azimuth: %d fxbus: %x (%x / %x); final: [hw: %x %x]\n",
           source->azimuth,
           that->hw->voicetable[that->channels[0]].param.send_routings,
           source->send_amount[0],
           source->send_amount[1],d1,d2);
        }
*/
}

#pragma code_seg("PAGE")

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif

NTSTATUS CMiniportWaveOutStream::Property3DB(IN      PPCPROPERTY_REQUEST req)
{
 PAGED_CODE();

 int get=-1;
// debug(DKX3D,"Wave::Property3D [buffer] for node: %d; prop: %d verb: %x\n",req->Node,
//   req->PropertyItem->Id,req->Verb);
// debug(DKX3D," InstanceSize: %d ValueSize: %d\n",
//   req->InstanceSize,req->ValueSize);

 if(req->Verb&PCPROPERTY_ITEM_FLAG_SET)
  get=0;
 if(req->Verb&PCPROPERTY_ITEM_FLAG_GET)
  get=1;

 if(get==-1)
 {
  debug(DWDM,"!! invalid 'verb' in property 3-D\n");
  return STATUS_INVALID_PARAMETER;
 }


 CMiniportWaveOutStream *that=NULL;
 {
      if(req->MinorTarget==NULL)
      {
       debug(DWDM,"!!! No minor target in ::property3db\n");
       return STATUS_INVALID_PARAMETER;
      }

      PUNKNOWN pu=req->MinorTarget;
      that=(CMiniportWaveOutStream *)PMINIPORTWAVECYCLICSTREAM(pu);
      if(that)
       if(that->magic!=WAVEOUTSTREAM_MAGIC)
          {
           debug(DWDM,"!!! Bad wavestream magic: %x [in 3d]\n",that->magic);
           return STATUS_INVALID_PARAMETER;
          }
 }

 // check if source is initialized
 if(that->Miniport->listener.allocated==0)
 {
  debug(DWDM,"!! bad listener allocation [not created yet...] -- while accessing source3D\n");
  return STATUS_INVALID_PARAMETER;
 }

 kx_fpu_state state;
 that->hw->cb.save_fpu_state(&state);

 if(that->source.in_chain==0) // not initialized...
 {
  debug(DKX3D,"-- creating 3-D source\n");

  that->Miniport->listener.add(&that->source,that);
 }

 switch(req->PropertyItem->Id)
 {
  case KSPROPERTY_DIRECTSOUND3DBUFFER_ALL:
    if(req->ValueSize==sizeof(KSDS3D_BUFFER_ALL))
    {
     if(get)
      that->source.get_all((KSDS3D_BUFFER_ALL *)req->Value);
     else
      that->source.set_all(*(KSDS3D_BUFFER_ALL *)req->Value);
    } else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get all[b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
    break;
  case KSPROPERTY_DIRECTSOUND3DBUFFER_CONEANGLES:
    if(req->ValueSize==sizeof(KSDS3D_BUFFER_CONE_ANGLES))
    {
     KSDS3D_BUFFER_CONE_ANGLES *ca=(KSDS3D_BUFFER_CONE_ANGLES*)req->Value;

     if(get) 
      that->source.get_cone_angles((int&)ca->InsideConeAngle,(int&)ca->OutsideConeAngle);
     else
      that->source.set_cone_angles(ca->InsideConeAngle,ca->OutsideConeAngle);
    } else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get cone_angles[b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
    break;
  case KSPROPERTY_DIRECTSOUND3DBUFFER_CONEORIENTATION:
    if(req->ValueSize==sizeof(DS3DVECTOR))
    {
     if(get) 
      that->source.get_cone_orientation(*(DS3DVECTOR*)req->Value);
     else
      that->source.set_cone_orientation(*(DS3DVECTOR*)req->Value);
    } else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get cone_orient[b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
    break;
  case KSPROPERTY_DIRECTSOUND3DBUFFER_CONEOUTSIDEVOLUME:
    if(req->ValueSize==sizeof(LONG))
    {
     if(get) 
      *(LONG *)req->Value=that->source.get_cone_outside_volume();
     else
      that->source.set_cone_outside_volume(*(LONG *)req->Value);
    } else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get cone_outside[b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
    break;
  case KSPROPERTY_DIRECTSOUND3DBUFFER_MAXDISTANCE:
    if(req->ValueSize==sizeof(FLOAT))
    {
     if(get) 
      *(FLOAT *)req->Value=that->source.get_max_distance();
     else
      that->source.set_max_distance(*(FLOAT *)req->Value);
    } else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get max_dist[b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
    break;
  case KSPROPERTY_DIRECTSOUND3DBUFFER_MINDISTANCE:
    if(req->ValueSize==sizeof(FLOAT))
    {
     if(get) 
      *(FLOAT *)req->Value=that->source.get_min_distance();
     else
      that->source.set_min_distance(*(FLOAT *)req->Value);
    } else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get min_dist[b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
    break;
  case KSPROPERTY_DIRECTSOUND3DBUFFER_MODE:
    if(req->ValueSize==sizeof(ULONG))
    {
     if(get) 
      *(ULONG *)req->Value=that->source.get_mode();
     else
      that->source.set_mode(*(ULONG *)req->Value);
    } else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get mode[b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
    break;
  case KSPROPERTY_DIRECTSOUND3DBUFFER_POSITION:
    if(req->ValueSize==sizeof(DS3DVECTOR))
    {
     if(get) 
      that->source.get_position(*(DS3DVECTOR *)req->Value);
     else
      that->source.set_position(*(DS3DVECTOR *)req->Value);
    } else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get pos[b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
    break;
  case KSPROPERTY_DIRECTSOUND3DBUFFER_VELOCITY:
    if(req->ValueSize==sizeof(DS3DVECTOR))
    {
     if(get) 
      that->source.get_velocity(*(DS3DVECTOR *)req->Value);
     else
      that->source.set_velocity(*(DS3DVECTOR *)req->Value);
    } else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get velocity[b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
    break;
  default:
    debug(DWDM,"!! invalid 3-D property [buffer] [%d] [%d]\n",req->PropertyItem->Id,get);
    that->hw->cb.rest_fpu_state(&state);
    return STATUS_INVALID_PARAMETER;
 }

 if(!get && (that->Miniport->listener.batch==0)) // apply changes
 {
    apply((CMiniportWaveOutStream *)that,&that->source);
 }

 that->hw->cb.rest_fpu_state(&state);

 return STATUS_SUCCESS;
}


#if defined(_MSC_VER)
#pragma code_seg()
// this is required, since we call send_message() here
#endif

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif


NTSTATUS CMiniportWaveOutStream::Property3DL(IN      PPCPROPERTY_REQUEST req)
{
 // this needs to be in non-paged memory, since we call send_message() from here
 // it requires buffer to be in non-paged memory

 int get=-1;
// debug(DKX3D,"Wave::Property3D [listener] for node: %d; prop: %d verb: %x\n",req->Node,
//   req->PropertyItem->Id,req->Verb);
// debug(DKX3D," InstanceSize: %d ValueSize: %d\n",
//   req->InstanceSize,req->ValueSize);

 if(req->Verb&PCPROPERTY_ITEM_FLAG_SET)
  get=0;
 if(req->Verb&PCPROPERTY_ITEM_FLAG_GET)
  get=1;

 if(get==-1)
 {
  debug(DWDM,"!! invalid 'verb' in property 3-D\n");
  return STATUS_INVALID_PARAMETER;
 }

 CMiniportWave *that=NULL;
 PUNKNOWN pu=req->MajorTarget;
 that=(CMiniportWave *)PMINIPORTWAVECYCLIC(pu);
 if(that==0 || that->magic!=WAVE_MAGIC)
 {
  debug(DWDM,"!! invalid major target in 3d listener\n");
  return STATUS_INVALID_PARAMETER;
 }

 kx_fpu_state state;
 that->hw->cb.save_fpu_state(&state);

 switch(req->PropertyItem->Id)
 {
  case KSPROPERTY_DIRECTSOUND3DLISTENER_ALLOCATION:
    if(req->ValueSize==sizeof(BOOL))
    {
     if(get)
     {
      *(BOOL *)req->Value=that->listener.allocated;
     }
     else
     {
again:
      BOOL s=*(BOOL *)req->Value;
      if(s==TRUE && that->listener.allocated==0)
      {
       debug(DKX3D,"allocating 3-D listener...\n");
       that->hw->cb.send_message(that->hw->cb.call_with,KX_SYSEX_SIZE,KX_SYSEX_START_3D);
       that->listener.reset();
       that->listener.allocated=1;

           // depends on virtual targets
           that->listener.routes[TARGET_LEFT]=KX_GET_SEND_A(that->hw->cb.def_routings[DEF_3D_LEFT_ROUTING]);
           that->listener.routes[FRONT_LEFT]=KX_GET_SEND_A(that->hw->cb.def_routings[DEF_AC3_LEFT_ROUTING]);
           that->listener.routes[FRONT_CENTER]=KX_GET_SEND_A(that->hw->cb.def_routings[DEF_AC3_CENTER_ROUTING]);
           that->listener.routes[FRONT_RIGHT]=KX_GET_SEND_A(that->hw->cb.def_routings[DEF_AC3_RIGHT_ROUTING]);
           that->listener.routes[TARGET_RIGHT]=KX_GET_SEND_A(that->hw->cb.def_routings[DEF_3D_RIGHT_ROUTING]);
           that->listener.routes[REAR_RIGHT]=KX_GET_SEND_A(that->hw->cb.def_routings[DEF_AC3_SRIGHT_ROUTING]);
           that->listener.routes[REAR_CENTER]=KX_GET_SEND_A(that->hw->cb.def_routings[DEF_AC3_SCENTER_ROUTING]);
           that->listener.routes[REAR_LEFT]=KX_GET_SEND_A(that->hw->cb.def_routings[DEF_AC3_SLEFT_ROUTING]);
           that->listener.routes[SPEAKER_TOP]=KX_GET_SEND_A(that->hw->cb.def_routings[DEF_3D_TOP_ROUTING]);

           // that->listener.routes[SPEAKER_BOTTOM]=KX_GET_SEND_A(that->hw->cb.def_routings[DEF_3D_BOTTOM_ROUTING]);

           that->listener.routes[FX1]=KX_GET_SEND_C(that->hw->cb.def_routings[DEF_AC3_LEFT_ROUTING]);
           that->listener.routes[FX2]=KX_GET_SEND_D(that->hw->cb.def_routings[DEF_AC3_LEFT_ROUTING]);
      }
      else
       if(s==FALSE && that->listener.allocated==1)
       {
        debug(DKX3D,"de-allocating 3-D listener...\n");
        that->hw->cb.send_message(that->hw->cb.call_with,KX_SYSEX_SIZE,KX_SYSEX_STOP_3D);

            // remove all the sources here...
            // NOTE: unefficient, but lock-safe (since .remove uses listener_lock, too)
            while(that->listener.s_list)
            {
//             debug(DWDM,"!! removing incorrect source due to listener destroy...\n");
             that->listener.remove(that->listener.s_list);
            }

        that->listener.allocated=0;
        that->listener.reset();
       }
       else
       {
        debug(DWDM,"!!! incorrect call to 'listener_allocation' [%d][%d] -- deallocating (just in case...)\n",
         that->listener.allocated,s);

            // free sources
            while(that->listener.s_list)
            {
//             debug(DWDM,"!! removing incorrect source due to listener destroy...\n");
             that->listener.remove(that->listener.s_list);
            }

        that->listener.allocated=0;
        that->listener.reset();
        // that->hw->cb.rest_fpu_state(&state); 
        // return STATUS_INVALID_PARAMETER;

        if(s)
         goto again;
       }
     }
    } else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get all[l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
    break;
  case KSPROPERTY_DIRECTSOUND3DLISTENER_ALL:
    if(req->ValueSize==sizeof(KSDS3D_LISTENER_ALL))
    {
     if(get) 
      that->listener.get_all(*(KSDS3D_LISTENER_ALL*)req->Value);
     else
      that->listener.set_all(*(KSDS3D_LISTENER_ALL*)req->Value);
    } else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
    break;
  case KSPROPERTY_DIRECTSOUND3DLISTENER_BATCH:
    if(req->ValueSize==sizeof(BOOL))
    {
     if(get) 
      *(BOOL*)req->Value=that->listener.get_batch();
     else
      that->listener.set_batch(*(BOOL*)req->Value);
    } else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
    break;
  case KSPROPERTY_DIRECTSOUND3DLISTENER_DISTANCEFACTOR:
    if(req->ValueSize==sizeof(FLOAT))
    {
     if(get) 
      *(FLOAT*)req->Value=that->listener.get_distance_factor();
     else
      that->listener.set_distance_factor(*(FLOAT*)req->Value);
    } else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
    break;
  case KSPROPERTY_DIRECTSOUND3DLISTENER_DOPPLERFACTOR:
    if(req->ValueSize==sizeof(FLOAT))
    {
     if(get) 
      *(FLOAT*)req->Value=that->listener.get_doppler_factor();
     else
      that->listener.set_doppler_factor(*(FLOAT*)req->Value);
    } else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
    break;
  case KSPROPERTY_DIRECTSOUND3DLISTENER_ORIENTATION:
    if(req->ValueSize==sizeof(KSDS3D_LISTENER_ORIENTATION))
    {
     if(get) 
      that->listener.get_orientation(*(KSDS3D_LISTENER_ORIENTATION*)req->Value);
     else
      that->listener.set_orientation(*(KSDS3D_LISTENER_ORIENTATION*)req->Value);
    } else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
    break;
  case KSPROPERTY_DIRECTSOUND3DLISTENER_POSITION:
    if(req->ValueSize==sizeof(DS3DVECTOR))
    {
     if(get) 
      that->listener.get_position(*(DS3DVECTOR*)req->Value);
     else
      that->listener.set_position(*(DS3DVECTOR*)req->Value);
    } else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
    break;
  case KSPROPERTY_DIRECTSOUND3DLISTENER_ROLLOFFFACTOR:
    if(req->ValueSize==sizeof(FLOAT))
    {
     if(get) 
      *(FLOAT*)req->Value=that->listener.get_rolloff_factor();
     else
      that->listener.set_rolloff_factor(*(FLOAT*)req->Value);
    } else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
    break;
  case KSPROPERTY_DIRECTSOUND3DLISTENER_VELOCITY:
    if(req->ValueSize==sizeof(DS3DVECTOR))
    {
     if(get) 
      that->listener.get_velocity(*(DS3DVECTOR*)req->Value);
     else
      that->listener.set_velocity(*(DS3DVECTOR*)req->Value);
    } else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
    break;
  default:
    debug(DWDM,"!! invalid 3-D property [listener] [%d] [%d]\n",req->PropertyItem->Id,get);
    that->hw->cb.rest_fpu_state(&state);
    return STATUS_INVALID_PARAMETER;
 }

 if(!get && (that->listener.batch==0)) // apply changes
 {
   unsigned long flags;
   kx_lock_acquire(that->hw,
    &that->listener_lock, &flags);

   kSource *src=that->listener.s_list;
   while(src)
   {
    apply((CMiniportWaveOutStream *)src->instance,src);
    src=src->s_next;
   }    

   kx_lock_release(that->hw,&that->listener_lock,&flags);
 }

 that->hw->cb.rest_fpu_state(&state);

 return STATUS_SUCCESS; 
}

