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


// kX 3-D Engine (EAX)
// -------------
// wrapper between kx3d and directsound / WDM / EAX

#pragma code_seg("PAGE")

NTSTATUS CMiniportWaveOutStream::PropertyEAB(int id, IN      PPCPROPERTY_REQUEST req)
{
 PAGED_CODE();

 int get=-1;

 if(req->Verb&PCPROPERTY_ITEM_FLAG_SET)
  get=0;
 if(req->Verb&PCPROPERTY_ITEM_FLAG_GET)
  get=1;

 if(get==-1)
 {
  debug(DWDM,"!! invalid 'verb' in property 3-D/EAX [buffer]\n");
  return STATUS_INVALID_PARAMETER;
 }


 CMiniportWaveOutStream *that=NULL;
 {
      if(req->MinorTarget==NULL)
      {
       debug(DWDM,"!!! No minor target in ::property3db/eax [buffer]\n");
       return STATUS_INVALID_PARAMETER;
      }

      PUNKNOWN pu=req->MinorTarget;
      that=(CMiniportWaveOutStream *)PMINIPORTWAVECYCLICSTREAM(pu);
      if(that)
       if(that->magic!=WAVEOUTSTREAM_MAGIC)
          {
           debug(DWDM,"!!! Bad wavestream magic: %x [in 3d/eax] buffer\n",that->magic);
           return STATUS_INVALID_PARAMETER;
          }
 }
 if(that->hw->ext_flags&KX_HW_WAVE_EAX_ALL)
  return STATUS_INVALID_PARAMETER;

 // check if source is initialized
 if(that->Miniport->listener.allocated==0)
 {
  debug(DWDM,"!! bad listener/eax allocation [not created yet...] -- while accessing EAXSource3D\n");
  return STATUS_INVALID_PARAMETER;
 }

 kx_fpu_state state;
 that->hw->cb.save_fpu_state(&state);

 if(that->source.in_chain==0) // not initialized...
 {
  debug(DKX3D,"-- first call to 3-D source - re-init [eax]\n");

  that->Miniport->listener.add(&that->source,that);
 }

 int deferred=0;

 if(id&DSPROPERTY_EAXBUFFER_DEFERRED)
 {
  deferred=1;
  id&=(~DSPROPERTY_EAXBUFFER_DEFERRED);
 }

 switch(id)
 {
    case DSPROPERTY_EAX30BUFFER_NONE: break;
    case DSPROPERTY_EAX30BUFFER_ALLPARAMETERS:
	if(req->ValueSize==sizeof(EAX30BUFFERPROPERTIES))
  	{
  	 if(get) 
  	 {
  	  EAX30BUFFERPROPERTIES *out=(EAX30BUFFERPROPERTIES *)req->Value;

  	  out->lDirect=that->source.direct;
  	  out->lDirectHF=that->source.direct_hf;
  	  out->lRoom=that->source.room;
  	  out->lRoomHF=that->source.room_hf;
  	  out->lObstruction=that->source.obstruction;
  	  out->flObstructionLFRatio=that->source.obstruction_lf_ratio;
  	  out->lOcclusion=that->source.occlusion;
  	  out->flOcclusionLFRatio=that->source.occlusion_lf_ratio;
  	  out->flOcclusionRoomRatio=that->source.occlusion_room_ratio;
  	  out->flOcclusionDirectRatio=that->source.occlusion_direct_ratio;
  	  out->lExclusion=that->source.exclusion;
  	  out->flExclusionLFRatio=that->source.exclusion_lf_ratio;
  	  out->lOutsideVolumeHF=that->source.outside_volume_hf;
  	  out->flDopplerFactor=that->source.eax_doppler_factor;
  	  out->flRolloffFactor=that->source.eax_rolloff_factor;
  	  out->flRoomRolloffFactor=that->source.room_rolloff_factor;
  	  out->flAirAbsorptionFactor=that->source.air_absorption_factor;
  	  out->ulFlags=that->source.eax_flags;
  	 }
  	 else
  	 {
  	  EAX30BUFFERPROPERTIES *in=(EAX30BUFFERPROPERTIES *)req->Value;
  	  if(that->source.direct!=in->lDirect)
  	  {
  	   that->source.direct=in->lDirect;
           // update volume
           that->source.update|=UPDATE_VOL;
  	  }

  	  that->source.direct_hf=in->lDirectHF;
  	  that->source.room=in->lRoom;
  	  that->source.room_hf=in->lRoomHF;
  	  that->source.obstruction=in->lObstruction;
  	  that->source.occlusion=in->lOcclusion;
  	  that->source.exclusion=in->lExclusion;
  	  that->source.obstruction_lf_ratio=in->flObstructionLFRatio;
  	  that->source.occlusion_lf_ratio=in->flOcclusionLFRatio;
  	  that->source.exclusion_lf_ratio=in->flExclusionLFRatio;
  	  that->source.occlusion_room_ratio=in->flOcclusionRoomRatio;
  	  that->source.occlusion_direct_ratio=in->flOcclusionDirectRatio;
  	  if(that->source.eax_doppler_factor!=in->flDopplerFactor)
  	  {
  	   that->source.eax_doppler_factor=in->flDopplerFactor;
  	   // update doppler if necessary
  	   that->source.recalc_doppler(0); // don't recalc magnitudes...
  	  }
  	  if(that->source.eax_rolloff_factor!=in->flRolloffFactor)
  	  {
  	   that->source.eax_rolloff_factor=in->flRolloffFactor;
  	   // update rolloff if necessary
  	   that->source.recalc_distance_attn(0,1); // don't recalc magnitude; force recalc attn
  	  }
  	  that->source.air_absorption_factor=in->flAirAbsorptionFactor;
  	  that->source.room_rolloff_factor=in->flRoomRolloffFactor;
  	  that->source.eax_flags=in->ulFlags;
  	  that->source.outside_volume_hf=in->lOutsideVolumeHF;

          // recalcs...
          that->source.recalc_cone_hf();
          that->source.recalc_direct_hf();
          that->source.recalc_direct_lf();
          that->source.recalc_room();
          that->source.recalc_room_hf();
          that->source.recalc_room_lf();
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get all [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
  // i3dl compatibility
  case DSPROPERTY_I3DL2BUFFER_ALL+I3D_OFFSET:
	if(req->ValueSize==sizeof(I3DL2_BUFFERPROPERTIES))
  	{
  	 if(get) 
  	 {
  	  I3DL2_BUFFERPROPERTIES *out=(I3DL2_BUFFERPROPERTIES *)req->Value;

  	  out->lDirect=that->source.direct;
  	  out->lDirectHF=that->source.direct_hf;
  	  out->lRoom=that->source.room;
  	  out->lRoomHF=that->source.room_hf;
  	  out->Obstruction.lHFLevel=that->source.obstruction;
  	  out->Obstruction.flLFRatio=that->source.obstruction_lf_ratio;
  	  out->Occlusion.lHFLevel=that->source.occlusion;
  	  out->Occlusion.flLFRatio=that->source.occlusion_lf_ratio;
  	  out->flRoomRolloffFactor=that->source.room_rolloff_factor;
  	 }
  	 else
  	 {
  	  I3DL2_BUFFERPROPERTIES *in=(I3DL2_BUFFERPROPERTIES *)req->Value;
  	  if(that->source.direct!=in->lDirect)
  	  {
  	   that->source.direct=in->lDirect;
           // update volume
  	   that->source.update|=UPDATE_VOL;
  	  }
  	  that->source.direct_hf=in->lDirectHF;
  	  that->source.room=in->lRoom;
  	  that->source.room_hf=in->lRoomHF;
  	  that->source.obstruction=in->Obstruction.lHFLevel;
  	  that->source.occlusion=in->Occlusion.lHFLevel;
  	  that->source.obstruction_lf_ratio=in->Obstruction.flLFRatio;
  	  that->source.occlusion_lf_ratio=in->Occlusion.flLFRatio;
  	  that->source.room_rolloff_factor=in->flRoomRolloffFactor;

          // recalcs...
          that->source.recalc_cone_hf();
          that->source.recalc_direct_hf();
          that->source.recalc_direct_lf();
          that->source.recalc_room();
          that->source.recalc_room_hf();
          that->source.recalc_room_lf();
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get all_i3d [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }

  	break;
  // eax 2.0 compatibility
  case DSPROPERTY_EAX20BUFFER_ALLPARAMETERS+EAX20_OFFSET:
	if(req->ValueSize==sizeof(EAX20BUFFERPROPERTIES))
  	{
  	 if(get) 
  	 {
  	  EAX20BUFFERPROPERTIES *out=(EAX20BUFFERPROPERTIES *)req->Value;

  	  out->lDirect=that->source.direct;
  	  out->lDirectHF=that->source.direct_hf;
  	  out->lRoom=that->source.room;
  	  out->lRoomHF=that->source.room_hf;
  	  out->lObstruction=that->source.obstruction;
  	  out->flObstructionLFRatio=that->source.obstruction_lf_ratio;
  	  out->lOcclusion=that->source.occlusion;
  	  out->flOcclusionRoomRatio=that->source.occlusion_room_ratio;
  	  out->flOcclusionLFRatio=that->source.occlusion_lf_ratio;
  	  out->lOutsideVolumeHF=that->source.outside_volume_hf;
  	  out->flRoomRolloffFactor=that->source.room_rolloff_factor;
  	  out->flAirAbsorptionFactor=that->source.air_absorption_factor;
  	  out->dwFlags=that->source.eax_flags;
  	 }
  	 else
  	 {
  	  EAX20BUFFERPROPERTIES *in=(EAX20BUFFERPROPERTIES *)req->Value;
  	  if(that->source.direct!=in->lDirect)
  	  {
  	   that->source.direct=in->lDirect;
  	   that->source.update|=UPDATE_VOL;
  	  }
  	  that->source.direct_hf=in->lDirectHF;
  	  that->source.room=in->lRoom;
  	  that->source.room_hf=in->lRoomHF;
  	  that->source.obstruction=in->lObstruction;
  	  that->source.occlusion=in->lOcclusion;
  	  that->source.obstruction_lf_ratio=in->flObstructionLFRatio;
  	  that->source.occlusion_lf_ratio=in->flOcclusionLFRatio;
  	  that->source.occlusion_room_ratio=in->flOcclusionRoomRatio;
  	  that->source.air_absorption_factor=in->flAirAbsorptionFactor;
  	  that->source.room_rolloff_factor=in->flRoomRolloffFactor;
  	  that->source.eax_flags=in->dwFlags;
  	  that->source.outside_volume_hf=in->lOutsideVolumeHF;

          // recalcs...
          that->source.recalc_cone_hf();
          that->source.recalc_direct_hf();
          that->source.recalc_direct_lf();
          that->source.recalc_room();
          that->source.recalc_room_hf();
          that->source.recalc_room_lf();
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get * [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30BUFFER_OBSTRUCTIONPARAMETERS:
	if(req->ValueSize==sizeof(EAXOBSTRUCTIONPROPERTIES))
  	{
  	 if(get) 
  	 {
  	  EAXOBSTRUCTIONPROPERTIES *out=(EAXOBSTRUCTIONPROPERTIES *)req->Value;
  	  out->lObstruction=that->source.obstruction;
  	  out->flObstructionLFRatio=that->source.obstruction_lf_ratio;
  	 }
  	 else
  	 {
  	  EAXOBSTRUCTIONPROPERTIES *in=(EAXOBSTRUCTIONPROPERTIES *)req->Value;
  	  that->source.obstruction=in->lObstruction;
  	  that->source.obstruction_lf_ratio=in->flObstructionLFRatio;

          // recalcs...
          that->source.recalc_direct_hf();
          that->source.recalc_direct_lf();
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get obstr_all [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30BUFFER_OCCLUSIONPARAMETERS:
	if(req->ValueSize==sizeof(EAXOCCLUSIONPROPERTIES))
  	{
  	 if(get) 
  	 {
  	  EAXOCCLUSIONPROPERTIES *out=(EAXOCCLUSIONPROPERTIES *)req->Value;
  	  out->lOcclusion=that->source.occlusion;
  	  out->flOcclusionLFRatio=that->source.occlusion_lf_ratio;
  	  out->flOcclusionRoomRatio=that->source.occlusion_room_ratio;
  	  out->flOcclusionDirectRatio=that->source.occlusion_direct_ratio;
  	 }
  	 else
  	 {
  	  EAXOCCLUSIONPROPERTIES *in=(EAXOCCLUSIONPROPERTIES *)req->Value;
  	  that->source.occlusion=in->lOcclusion;
  	  that->source.occlusion_lf_ratio=in->flOcclusionLFRatio;
  	  that->source.occlusion_room_ratio=in->flOcclusionRoomRatio;
  	  that->source.occlusion_direct_ratio=in->flOcclusionDirectRatio;

          // recalcs...
          that->source.recalc_direct_hf();
          that->source.recalc_direct_lf();
          that->source.recalc_room_hf();
          that->source.recalc_room_lf();
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get occl_all [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30BUFFER_EXCLUSIONPARAMETERS:
	if(req->ValueSize==sizeof(EAXEXCLUSIONPROPERTIES))
  	{
  	 if(get) 
  	 {
  	  EAXEXCLUSIONPROPERTIES *out=(EAXEXCLUSIONPROPERTIES *)req->Value;
  	  out->lExclusion=that->source.exclusion;
  	  out->flExclusionLFRatio=that->source.exclusion_lf_ratio;
  	 }
  	 else
  	 {
  	  EAXEXCLUSIONPROPERTIES *in=(EAXEXCLUSIONPROPERTIES *)req->Value;
  	  that->source.exclusion=in->lExclusion;
  	  that->source.exclusion_lf_ratio=in->flExclusionLFRatio;

          // recalcs...
          that->source.recalc_room_hf();
          that->source.recalc_room_lf();
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get excl_all [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30BUFFER_DIRECT: 
	if(req->ValueSize==sizeof(LONG))
  	{
  	 if(get) 
  	  *(LONG *)req->Value=that->source.direct;
  	 else
  	 {
  	  that->source.direct=*(LONG*)req->Value;

  	  that->source.update|=UPDATE_VOL;
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get direct [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
    case DSPROPERTY_EAX30BUFFER_DIRECTHF:
	if(req->ValueSize==sizeof(LONG))
  	{
  	 if(get) 
  	  *(LONG *)req->Value=that->source.direct_hf;
  	 else
  	 {
  	  that->source.direct_hf=*(LONG*)req->Value;

          // recalcs...
          that->source.recalc_direct_hf();
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get direct_hf [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
    case DSPROPERTY_EAX30BUFFER_ROOM:
	if(req->ValueSize==sizeof(LONG))
  	{
  	 if(get) 
  	  *(LONG *)req->Value=that->source.room;
  	 else
  	 {
  	  that->source.room=*(LONG*)req->Value;

          // recalcs...
          that->source.recalc_room();
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get room [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
    case DSPROPERTY_EAX30BUFFER_ROOMHF: 
	if(req->ValueSize==sizeof(LONG))
  	{
  	 if(get) 
  	  *(LONG *)req->Value=that->source.room_hf;
  	 else
  	 {
  	  that->source.room_hf=*(LONG*)req->Value;

          // recalcs...
          that->source.recalc_room_hf();
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get room_hf [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
    case DSPROPERTY_EAX30BUFFER_OBSTRUCTION:
	if(req->ValueSize==sizeof(LONG))
  	{
  	 if(get) 
  	  *(LONG *)req->Value=that->source.obstruction;
  	 else
  	 {
  	  that->source.obstruction=*(LONG*)req->Value;

          // recalcs...
          that->source.recalc_direct_hf();
          that->source.recalc_direct_lf();
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get obstr [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
    case DSPROPERTY_EAX30BUFFER_OBSTRUCTIONLFRATIO:
	if(req->ValueSize==sizeof(FLOAT))
  	{
  	 if(get) 
  	  *(FLOAT *)req->Value=that->source.obstruction_lf_ratio;
  	 else
  	 {
  	  that->source.obstruction_lf_ratio=*(FLOAT*)req->Value;

          // recalcs...
          that->source.recalc_direct_lf();
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get obstr_lf [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
    case DSPROPERTY_EAX30BUFFER_OCCLUSION:
	if(req->ValueSize==sizeof(LONG))
  	{
  	 if(get) 
  	  *(LONG *)req->Value=that->source.occlusion;
  	 else
  	 {
  	  that->source.occlusion=*(LONG*)req->Value;

          // recalcs...
          that->source.recalc_direct_hf();
          that->source.recalc_direct_lf();
          that->source.recalc_room_hf();
          that->source.recalc_room_lf();
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get occlusion [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
    case DSPROPERTY_EAX30BUFFER_OCCLUSIONLFRATIO:
	if(req->ValueSize==sizeof(FLOAT))
  	{
  	 if(get) 
  	  *(FLOAT *)req->Value=that->source.occlusion_lf_ratio;
  	 else
  	 {
  	  that->source.occlusion_lf_ratio=*(FLOAT*)req->Value;

          // recalcs...
          that->source.recalc_direct_lf();
          that->source.recalc_room_lf();
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get occl_lf [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
    case DSPROPERTY_EAX30BUFFER_OCCLUSIONROOMRATIO:
	if(req->ValueSize==sizeof(FLOAT))
  	{
  	 if(get) 
  	  *(FLOAT *)req->Value=that->source.occlusion_room_ratio;
  	 else
  	 {
  	  that->source.occlusion_room_ratio=*(FLOAT*)req->Value;

          // recalcs...
          that->source.recalc_room_hf();
          that->source.recalc_room_lf();
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get occl_room [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
    case DSPROPERTY_EAX30BUFFER_OCCLUSIONDIRECTRATIO:
	if(req->ValueSize==sizeof(FLOAT))
  	{
  	 if(get) 
  	  *(FLOAT *)req->Value=that->source.occlusion_direct_ratio;
  	 else
  	 {
  	  that->source.occlusion_direct_ratio=*(FLOAT*)req->Value;

          // recalcs...
          that->source.recalc_direct_hf();
          that->source.recalc_direct_lf();
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get occl_direct [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
    case DSPROPERTY_EAX30BUFFER_EXCLUSION:
	if(req->ValueSize==sizeof(LONG))
  	{
  	 if(get) 
  	  *(LONG *)req->Value=that->source.exclusion;
  	 else
  	 {
  	  that->source.exclusion=*(LONG*)req->Value;

          // recalcs...
          that->source.recalc_room_hf();
          that->source.recalc_room_lf();
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get excl [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
    case DSPROPERTY_EAX30BUFFER_EXCLUSIONLFRATIO:
	if(req->ValueSize==sizeof(FLOAT))
  	{
  	 if(get) 
  	  *(FLOAT *)req->Value=that->source.exclusion_lf_ratio;
  	 else
  	 {
  	  that->source.exclusion_lf_ratio=*(FLOAT*)req->Value;

          // recalcs...
          that->source.recalc_room_lf();
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get excl_rat [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
    case DSPROPERTY_EAX30BUFFER_OUTSIDEVOLUMEHF:
	if(req->ValueSize==sizeof(LONG))
  	{
  	 if(get) 
  	  *(LONG *)req->Value=that->source.outside_volume_hf;
  	 else
  	 {
  	  that->source.outside_volume_hf=*(LONG*)req->Value;

          // recalcs...
          that->source.recalc_cone_hf();
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get outside_v [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
    case DSPROPERTY_EAX30BUFFER_DOPPLERFACTOR:
	if(req->ValueSize==sizeof(FLOAT))
  	{
  	 if(get) 
  	  *(FLOAT *)req->Value=that->source.eax_doppler_factor;
  	 else
  	 {
  	  that->source.eax_doppler_factor=*(FLOAT*)req->Value;
  	  that->source.recalc_doppler(0); // don't recalc magnitudes...
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get eax_doppler [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
    case DSPROPERTY_EAX30BUFFER_ROLLOFFFACTOR:
	if(req->ValueSize==sizeof(FLOAT))
  	{
  	 if(get) 
  	  *(FLOAT *)req->Value=that->source.eax_rolloff_factor;
  	 else
  	 {
  	  that->source.eax_rolloff_factor=*(FLOAT*)req->Value;

  	  that->source.recalc_distance_attn(0,1); // don't recalc magnitude; force recalc attn
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get eax_rolloff [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
    case DSPROPERTY_EAX30BUFFER_ROOMROLLOFFFACTOR:
	if(req->ValueSize==sizeof(FLOAT))
  	{
  	 if(get) 
  	  *(FLOAT *)req->Value=that->source.room_rolloff_factor;
  	 else
  	 {
  	  that->source.room_rolloff_factor=*(FLOAT*)req->Value;

          // recalcs...
          that->source.recalc_room();
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get room_rolloff [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
    case DSPROPERTY_EAX30BUFFER_AIRABSORPTIONFACTOR:
	if(req->ValueSize==sizeof(FLOAT))
  	{
  	 if(get) 
  	  *(FLOAT *)req->Value=that->source.air_absorption_factor;
  	 else
  	 {
  	  that->source.air_absorption_factor=*(FLOAT*)req->Value;

          // recalcs...
          that->source.recalc_direct_hf();
          that->source.recalc_room_hf();
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get air_f [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
    case DSPROPERTY_EAX30BUFFER_FLAGS:
	if(req->ValueSize==sizeof(INT))
  	{
  	 if(get) 
  	  *(INT *)req->Value=that->source.eax_flags;
  	 else
  	 {
  	  that->source.eax_flags=*(INT *)req->Value;
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get eax_flags [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

  case DSPROPERTY_I3DL2BUFFER_OCCLUSIONALL+I3D_OFFSET:
	if(req->ValueSize==sizeof(I3DL2_OCCLUSIONPROPERTIES))
  	{
  	 if(get) 
  	 {
  	  I3DL2_OCCLUSIONPROPERTIES *out=(I3DL2_OCCLUSIONPROPERTIES *)req->Value;
  	  out->lHFLevel=that->source.occlusion;
  	  out->flLFRatio=that->source.occlusion_lf_ratio;
  	 }
  	 else
  	 {
  	  I3DL2_OCCLUSIONPROPERTIES *in=(I3DL2_OCCLUSIONPROPERTIES *)req->Value;
  	  that->source.occlusion=in->lHFLevel;
  	  that->source.occlusion_lf_ratio=in->flLFRatio;

          // recalcs...
          that->source.recalc_direct_hf();
          that->source.recalc_direct_lf();
          that->source.recalc_room_hf();
          that->source.recalc_room_lf();
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get occl_all_i3d [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

  // eax 1.0 compatibility
  case DSPROPERTY_EAX10BUFFER_ALL+EAX10_OFFSET:
	if(req->ValueSize==sizeof(EAX10BUFFER_REVERBPROPERTIES))
  	{
  	 if(get) 
  	 {
  	  EAX10BUFFER_REVERBPROPERTIES *out=(EAX10BUFFER_REVERBPROPERTIES *)req->Value;
  	  out->fMix=that->source.eax1_mix;
  	 }
  	 else
  	 {
  	  EAX10BUFFER_REVERBPROPERTIES *in=(EAX10BUFFER_REVERBPROPERTIES *)req->Value;
  	  that->source.eax1_mix=in->fMix;

          // update
          that->source.recalc_room();
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get eax1all [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

  case DSPROPERTY_EAX10BUFFER_REVERBMIX+EAX10_OFFSET:
	if(req->ValueSize==sizeof(FLOAT))
  	{
  	 if(get) 
  	  *(FLOAT *)req->Value=that->source.eax1_mix;
  	 else
  	 {
  	  that->source.eax1_mix=*(FLOAT*)req->Value;

          // update
          that->source.recalc_room();
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get eax1_mix [b]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

  default:
  	debug(DWDM,"!! invalid 3-D / EAX property [buffer] [%d/%d] [%d]\n",id,req->PropertyItem->Id,get);
  	that->hw->cb.rest_fpu_state(&state);
  	return STATUS_INVALID_PARAMETER;
 }

 if(!get && (that->Miniport->listener.batch==0) && (!deferred)) // apply changes
 {
 	apply((CMiniportWaveOutStream *)that,&that->source);
 }

 that->hw->cb.rest_fpu_state(&state);

 return STATUS_SUCCESS;
}


#pragma code_seg("PAGE")

NTSTATUS CMiniportWaveOutStream::PropertyEAL(int id, IN      PPCPROPERTY_REQUEST req)
{
 PAGED_CODE();

 int get=-1;

 if(req->Verb&PCPROPERTY_ITEM_FLAG_SET)
  get=0;
 if(req->Verb&PCPROPERTY_ITEM_FLAG_GET)
  get=1;

 if(get==-1)
 {
  debug(DWDM,"!! invalid 'verb' in property 3-D/EAX [listener]\n");
  return STATUS_INVALID_PARAMETER;
 }

 CMiniportWave *that=NULL;
 PUNKNOWN pu=req->MajorTarget;
 that=(CMiniportWave *)PMINIPORTWAVECYCLIC(pu);
 if(that==0 || that->magic!=WAVE_MAGIC)
 {
  debug(DWDM,"!! invalid major target in 3d/eax listener [listener]\n");
  return STATUS_INVALID_PARAMETER;
 }
 if(that->hw->ext_flags&KX_HW_WAVE_EAX_ALL)
  return STATUS_INVALID_PARAMETER;

 kx_fpu_state state;
 that->hw->cb.save_fpu_state(&state);

 int deferred=0;
 if(id&DSPROPERTY_EAXBUFFER_DEFERRED)
 {
  deferred=1;
  id&=(~DSPROPERTY_EAXBUFFER_DEFERRED);
 }

 int invalidate_env=0;
 if(!get)
  invalidate_env=1;

 switch(id)
 {
    case DSPROPERTY_EAX30LISTENER_NONE: 
    	invalidate_env=0;
    	break;

    case DSPROPERTY_EAX30LISTENER_ALLPARAMETERS:
	if(req->ValueSize==sizeof(EAX30LISTENERPROPERTIES))
  	{
  	 if(get) 
  	 {
  	  EAX30LISTENERPROPERTIES *out=(EAX30LISTENERPROPERTIES *)req->Value;
  	  out->ulEnvironment=that->listener.eax_environment;
  	  out->flEnvironmentSize=that->listener.env_size;
  	  out->flEnvironmentDiffusion=that->listener.env_diffusion;
  	  out->lRoom=that->listener.room;
  	  out->lRoomHF=that->listener.room_hf;
  	  out->lRoomLF=that->listener.room_lf;
  	  out->flDecayTime=that->listener.decay_time;
  	  out->flDecayHFRatio=that->listener.decay_hf_ratio;
  	  out->flDecayLFRatio=that->listener.decay_lf_ratio;
  	  out->lReflections=that->listener.reflections;
  	  out->flReflectionsDelay=that->listener.reflections_delay;
  	  out->lReverb=that->listener.reverb;
  	  out->flReverbDelay=that->listener.reverb_delay;
  	  out->flEchoTime=that->listener.echo_time;
  	  out->flEchoDepth=that->listener.echo_depth;
  	  out->flModulationTime=that->listener.modulation_time;
  	  out->flModulationDepth=that->listener.modulation_depth;
  	  out->flAirAbsorptionHF=that->listener.air_absorption_hf;
  	  out->flHFReference=that->listener.hf_reference;
  	  out->flLFReference=that->listener.lf_reference;
  	  out->flRoomRolloffFactor=that->listener.room_rolloff_factor;
  	  out->ulFlags=that->listener.eax_flags;

  	  out->vReflectionsPan.x=that->listener.reflections_pan.x;
  	  out->vReflectionsPan.y=that->listener.reflections_pan.y;
  	  out->vReflectionsPan.z=that->listener.reflections_pan.z;

  	  out->vReverbPan.x=that->listener.reverb_pan.x;
  	  out->vReverbPan.y=that->listener.reverb_pan.y;
  	  out->vReverbPan.z=that->listener.reverb_pan.z;
  	 }
  	 else
  	 {
  	  EAX30LISTENERPROPERTIES *in=(EAX30LISTENERPROPERTIES *)req->Value;
  	  that->listener.room=in->lRoom;
  	  that->listener.room_hf=in->lRoomHF;
  	  that->listener.room_lf=in->lRoomLF;
  	  that->listener.decay_time=in->flDecayTime;
  	  that->listener.eax_environment=in->ulEnvironment;
  	  that->listener.env_size=in->flEnvironmentSize;
          // FIXME: should we update all size-dependent stuff?..
          // how should we manage 'eax_env' in this case?..
          // overwrite settings?..
  	  that->listener.env_diffusion=in->flEnvironmentDiffusion;
  	  that->listener.decay_hf_ratio=in->flDecayHFRatio;
  	  that->listener.decay_lf_ratio=in->flDecayLFRatio;
  	  that->listener.reflections=in->lReflections;
  	  that->listener.reflections_delay=in->flReflectionsDelay;
  	  that->listener.reverb=in->lReverb;
  	  that->listener.reverb_delay=in->flReverbDelay;
  	  that->listener.echo_time=in->flEchoTime;
  	  that->listener.echo_depth=in->flEchoDepth;
  	  that->listener.modulation_time=in->flModulationTime;
  	  that->listener.modulation_depth=in->flModulationDepth;
  	  that->listener.air_absorption_hf=in->flAirAbsorptionHF;
  	  that->listener.hf_reference=in->flHFReference;
  	  that->listener.lf_reference=in->flLFReference;
  	  that->listener.room_rolloff_factor=in->flRoomRolloffFactor;
  	  that->listener.eax_flags=in->ulFlags;

  	  that->listener.reflections_pan.x=in->vReflectionsPan.x;
  	  that->listener.reflections_pan.y=in->vReflectionsPan.y;
  	  that->listener.reflections_pan.z=in->vReflectionsPan.z;

  	  that->listener.reverb_pan.x=in->vReverbPan.x;
  	  that->listener.reverb_pan.y=in->vReverbPan.y;
  	  that->listener.reverb_pan.z=in->vReverbPan.z;

  	  that->listener.recalc_decay();
  	  that->listener.update_reverb(UPDATE_REVERB_ALL);

  	  that->listener.for_each(-1,RECALC_EAX);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get all [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX20LISTENER_ALLPARAMETERS+EAX20_OFFSET:
	if(req->ValueSize==sizeof(EAX20LISTENERPROPERTIES))
  	{
  	 if(get) 
  	 {
  	  EAX20LISTENERPROPERTIES *out=(EAX20LISTENERPROPERTIES *)req->Value;
  	  out->dwEnvironment=that->listener.eax_environment;
  	  out->flEnvironmentSize=that->listener.env_size;
  	  out->flEnvironmentDiffusion=that->listener.env_diffusion;
  	  out->lRoom=that->listener.room;
  	  out->lRoomHF=that->listener.room_hf;
  	  out->flDecayTime=that->listener.decay_time;
  	  out->flDecayHFRatio=that->listener.decay_hf_ratio;
  	  out->lReflections=that->listener.reflections;
  	  out->flReflectionsDelay=that->listener.reflections_delay;
  	  out->lReverb=that->listener.reverb;
  	  out->flReverbDelay=that->listener.reverb_delay;
  	  out->flAirAbsorptionHF=that->listener.air_absorption_hf;
  	  out->flRoomRolloffFactor=that->listener.room_rolloff_factor;
  	  out->dwFlags=(that->listener.eax_flags)&(~EAX20LISTENERFLAGS_RESERVED);
  	 }
  	 else
  	 {
  	  EAX20LISTENERPROPERTIES *in=(EAX20LISTENERPROPERTIES *)req->Value;
  	  that->listener.room=in->lRoom;
  	  that->listener.room_hf=in->lRoomHF;
  	  that->listener.decay_time=in->flDecayTime;
  	  that->listener.eax_environment=in->dwEnvironment;
  	  that->listener.env_size=in->flEnvironmentSize;
          // FIXME: should we update all size-dependent stuff?..
          // how should we manage 'eax_env' in this case?..
          // overwrite settings?..
  	  that->listener.env_diffusion=in->flEnvironmentDiffusion;
  	  that->listener.decay_hf_ratio=in->flDecayHFRatio;
  	  that->listener.reflections=in->lReflections;
  	  that->listener.reflections_delay=in->flReflectionsDelay;
  	  that->listener.reverb=in->lReverb;
  	  that->listener.reverb_delay=in->flReverbDelay;
  	  that->listener.air_absorption_hf=in->flAirAbsorptionHF;
  	  that->listener.room_rolloff_factor=in->flRoomRolloffFactor;
  	  that->listener.eax_flags=(in->dwFlags&(~EAX20LISTENERFLAGS_RESERVED))|(that->listener.eax_flags&EAX20LISTENERFLAGS_RESERVED);

          // recalculate decay times
          that->listener.recalc_decay();
          that->listener.update_reverb(UPDATE_REVERB_ALL);

          that->listener.for_each(-1,RECALC_EAX);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get all_eax2 [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_I3DL2LISTENER_ALL+I3D_OFFSET:
	if(req->ValueSize==sizeof(I3DL2_LISTENERPROPERTIES))
  	{
  	 if(get) 
  	 {
  	  I3DL2_LISTENERPROPERTIES *out=(I3DL2_LISTENERPROPERTIES *)req->Value;
  	  
  	  out->lRoom=that->listener.room;
  	  out->lRoomHF=that->listener.room_hf;
  	  out->flRoomRolloffFactor=that->listener.room_rolloff_factor;
  	  out->flDecayTime=that->listener.decay_time;
  	  out->flDecayHFRatio=that->listener.decay_hf_ratio;
  	  out->lReflections=that->listener.reflections;
  	  out->flReflectionsDelay=that->listener.reflections_delay;
  	  out->lReverb=that->listener.reverb;
  	  out->flReverbDelay=that->listener.reverb_delay;
  	  out->flDiffusion=that->listener.env_diffusion*100.0f;
  	  out->flHFReference=that->listener.hf_reference;

          // fixme: density*100.0f
  	 }
  	 else
  	 {
  	  I3DL2_LISTENERPROPERTIES *in=(I3DL2_LISTENERPROPERTIES *)req->Value;
  	  that->listener.room=in->lRoom;
  	  that->listener.room_hf=in->lRoomHF;
  	  that->listener.room_rolloff_factor=in->flRoomRolloffFactor;
  	  that->listener.decay_time=in->flDecayTime;
  	  that->listener.decay_hf_ratio=in->flDecayHFRatio;
  	  that->listener.reflections=in->lReflections;
  	  that->listener.reflections_delay=in->flReflectionsDelay;
  	  that->listener.reverb=in->lReverb;
  	  that->listener.reverb_delay=in->flReverbDelay;
  	  that->listener.env_diffusion=(in->flDiffusion)/100.0f;
  	  that->listener.hf_reference=in->flHFReference;

          // fixme: density/100.0f

          // recalculate decay times
          that->listener.recalc_decay();
          that->listener.update_reverb(UPDATE_REVERB_ALL);

          that->listener.for_each(-1,RECALC_EAX);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get all_i3d [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_ENVIRONMENT:
	if(req->ValueSize==sizeof(int))
  	{
  	 if(get) 
  	  *(int *)req->Value=that->listener.eax_environment;
  	 else
  	 {
  	  that->listener.eax_environment=*(int *)req->Value;
  	  invalidate_env=0;

  	  that->listener.reset_eax(that->listener.eax_environment);

  	  that->listener.for_each(-1,RECALC_EAX);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get env [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_ENVIRONMENTSIZE:
	if(req->ValueSize==sizeof(float))
  	{
  	 if(get)
  	  *(float *)req->Value=that->listener.env_size;
  	 else
  	 {
  	  that->listener.set_env_size(*(float *)req->Value);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get env_size [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_ENVIRONMENTDIFFUSION:
	if(req->ValueSize==sizeof(float))
  	{
  	 if(get) 
  	  *(float *)req->Value=that->listener.env_diffusion;
  	 else
  	 {
  	  that->listener.env_diffusion=*(float*)req->Value;
          that->listener.update_reverb(UPDATE_REVERB_DIFFUSION);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get env_diff [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_ROOM:
	if(req->ValueSize==sizeof(int))
  	{
  	 if(get) 
  	  *(int *)req->Value=that->listener.room;
  	 else
  	 {
  	  that->listener.room=*(int *)req->Value;

  	  that->listener.for_each(-1,RECALC_ROOM_ONLY);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get room [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_ROOMHF:
	if(req->ValueSize==sizeof(int))
  	{
  	 if(get) 
  	  *(int *)req->Value=that->listener.room_hf;
  	 else
  	 {
  	  that->listener.room_hf=*(int *)req->Value;

  	  that->listener.for_each(-1,RECALC_ROOM_HF);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get room_hf [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_ROOMLF:
	if(req->ValueSize==sizeof(int))
  	{
  	 if(get) 
  	  *(int *)req->Value=that->listener.room_lf;
  	 else
  	 {
  	  that->listener.room_lf=*(int *)req->Value;

  	  that->listener.for_each(-1,RECALC_ROOM_LF);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get room_Lf [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_DECAYTIME:
	if(req->ValueSize==sizeof(float))
  	{
  	 if(get) 
  	  *(float *)req->Value=that->listener.decay_time;
  	 else
  	 {
  	  that->listener.decay_time=*(float *)req->Value;

          // recalculate decay times
          that->listener.recalc_decay();
          that->listener.update_reverb(UPDATE_REVERB_DECAY);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get decay_time [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_DECAYHFRATIO: 
	if(req->ValueSize==sizeof(float))
  	{
  	 if(get) 
  	  *(float *)req->Value=that->listener.decay_hf_ratio;
  	 else
  	 {
  	  that->listener.decay_hf_ratio=*(float *)req->Value;

  	  that->listener.recalc_decay();
  	  that->listener.update_reverb(UPDATE_REVERB_DECAY);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get decay_hf_ratio [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_DECAYLFRATIO: 
	if(req->ValueSize==sizeof(float))
  	{
  	 if(get) 
  	  *(float *)req->Value=that->listener.decay_lf_ratio;
  	 else
  	 {
  	  that->listener.decay_lf_ratio=*(float *)req->Value;

  	  that->listener.recalc_decay();
  	  that->listener.update_reverb(UPDATE_REVERB_DECAY);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get decay_lf_ratio [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_REFLECTIONS: 
	if(req->ValueSize==sizeof(int))
  	{
  	 if(get) 
  	  *(int *)req->Value=that->listener.reflections;
  	 else
  	 {
  	  that->listener.reflections=*(int *)req->Value;

          // update reverb engine
          that->listener.update_reverb(UPDATE_REVERB_REFL);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get reflections [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_REFLECTIONSDELAY: 
	if(req->ValueSize==sizeof(float))
  	{
  	 if(get) 
  	  *(float *)req->Value=that->listener.reflections_delay;
  	 else
  	 {
  	  that->listener.reflections_delay=*(float *)req->Value;

          // update reverb engine
          that->listener.update_reverb(UPDATE_REVERB_REFL);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get reflections_delay [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_REFLECTIONSPAN: 
	if(req->ValueSize==sizeof(EAXVECTOR))
  	{
  	 if(get) 
  	 {
  	  EAXVECTOR *out=(EAXVECTOR *)req->Value;
  	  out->x=that->listener.reflections_pan.x;
  	  out->y=that->listener.reflections_pan.y;
  	  out->z=that->listener.reflections_pan.z;
  	 }
  	 else
  	 {
  	  EAXVECTOR *in=(EAXVECTOR *)req->Value;
  	  that->listener.reflections_pan.x=in->x;
  	  that->listener.reflections_pan.y=in->y;
  	  that->listener.reflections_pan.z=in->z;

          // update reverb engine
          that->listener.update_reverb(UPDATE_REVERB_REFL);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get refl_pan [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_REVERB: 
	if(req->ValueSize==sizeof(int))
  	{
  	 if(get) 
  	  *(int *)req->Value=that->listener.reverb;
  	 else
  	 {
  	  that->listener.reverb=*(int *)req->Value;

          // update reverb engine
          that->listener.update_reverb(UPDATE_REVERB_REV);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get reverb [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_REVERBDELAY: 
	if(req->ValueSize==sizeof(float))
  	{
  	 if(get) 
  	  *(float *)req->Value=that->listener.reverb_delay;
  	 else
  	 {
  	  that->listener.reverb_delay=*(float *)req->Value;

          // update reverb engine
          that->listener.update_reverb(UPDATE_REVERB_REV);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get reverb_delay [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_REVERBPAN: 
	if(req->ValueSize==sizeof(EAXVECTOR))
  	{
  	 if(get) 
  	 {
  	  EAXVECTOR *out=(EAXVECTOR *)req->Value;
  	  out->x=that->listener.reverb_pan.x;
  	  out->y=that->listener.reverb_pan.y;
  	  out->z=that->listener.reverb_pan.z;
  	 }
  	 else
  	 {
  	  EAXVECTOR *in=(EAXVECTOR *)req->Value;
  	  that->listener.reverb_pan.x=in->x;
  	  that->listener.reverb_pan.y=in->y;
  	  that->listener.reverb_pan.z=in->z;

          // update reverb engine
          that->listener.update_reverb(UPDATE_REVERB_REV);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get reverb_pan [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_ECHOTIME: 
	if(req->ValueSize==sizeof(float))
  	{
  	 if(get) 
  	  *(float *)req->Value=that->listener.echo_time;
  	 else
  	 {
  	  that->listener.echo_time=*(float *)req->Value;

          // update reverb engine
          that->listener.update_reverb(UPDATE_REVERB_ECHO);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get echo_time [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_ECHODEPTH: 
	if(req->ValueSize==sizeof(float))
  	{
  	 if(get) 
  	  *(float *)req->Value=that->listener.echo_depth;
  	 else
  	 {
  	  that->listener.echo_depth=*(float *)req->Value;

          // update reverb engine
          that->listener.update_reverb(UPDATE_REVERB_ECHO);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get echo_depth [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_MODULATIONTIME: 
	if(req->ValueSize==sizeof(float))
  	{
  	 if(get) 
  	  *(float *)req->Value=that->listener.modulation_time;
  	 else
  	 {
  	  that->listener.modulation_time=*(float *)req->Value;

          // update reverb engine
          that->listener.update_reverb(UPDATE_REVERB_MOD);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get modulation_time [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_MODULATIONDEPTH: 
	if(req->ValueSize==sizeof(float))
  	{
  	 if(get) 
  	  *(float *)req->Value=that->listener.modulation_depth;
  	 else
  	 {
  	  that->listener.modulation_depth=*(float *)req->Value;

          // update reverb engine
          that->listener.update_reverb(UPDATE_REVERB_MOD);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get modulation_depth [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_AIRABSORPTIONHF: 
	if(req->ValueSize==sizeof(float))
  	{
  	 if(get) 
  	  *(float *)req->Value=that->listener.air_absorption_hf;
  	 else
  	 {
  	  that->listener.air_absorption_hf=*(float *)req->Value;

  	  that->listener.recalc_decay();
  	  that->listener.for_each(-1,RECALC_HF);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get air [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_HFREFERENCE: 
	if(req->ValueSize==sizeof(float))
  	{
  	 if(get) 
  	  *(float *)req->Value=that->listener.hf_reference;
  	 else
  	 {
  	  that->listener.hf_reference=*(float *)req->Value;

          // update reverb engine
          that->listener.update_reverb(UPDATE_REVERB_REFERENCE);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get hfreference [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_LFREFERENCE: 
	if(req->ValueSize==sizeof(float))
  	{
  	 if(get) 
  	  *(float *)req->Value=that->listener.lf_reference;
  	 else
  	 {
  	  that->listener.lf_reference=*(float*)req->Value;

          // update reverb engine
          that->listener.update_reverb(UPDATE_REVERB_REFERENCE);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get lfreference [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_ROOMROLLOFFFACTOR: 
	if(req->ValueSize==sizeof(float))
  	{
  	 if(get) 
  	  *(float *)req->Value=that->listener.room_rolloff_factor;
  	 else
  	 {
  	  that->listener.room_rolloff_factor=*(float *)req->Value;

          that->listener.for_each(-1,RECALC_ROOM_ONLY);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get room_rolloff [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

    case DSPROPERTY_EAX30LISTENER_FLAGS: 
	if(req->ValueSize==sizeof(int))
  	{
  	 if(get) 
  	  *(int *)req->Value=that->listener.eax_flags;
  	 else
  	 {
  	  that->listener.eax_flags=*(int *)req->Value;
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get flags [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;

  // i3dl compatibility
  case DSPROPERTY_I3DL2LISTENER_DIFFUSION+I3D_OFFSET:
        // note: 0..100%
	if(req->ValueSize==sizeof(float))
  	{
  	 if(get) 
  	  *(float *)req->Value=that->listener.env_diffusion*100.0f;
  	 else
  	 {
  	  that->listener.env_diffusion=(*(float *)req->Value)/100.0f;
          // FIXME: don't have any formulas
          // FIXME: could be different parameter!
          // affects echo density and (in) reverberation decay

          that->listener.update_reverb(UPDATE_REVERB_DIFFUSION);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get i3d_diffusion [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
  case DSPROPERTY_I3DL2LISTENER_DENSITY+I3D_OFFSET:
        // note: 0..100%
	if(req->ValueSize==sizeof(float))
  	{
  	 if(get) 
  	  *(float *)req->Value=0.0f; // FIXME: don't have formulas
  	 else
  	 {
  	  // that->listener.env_diffusion=(*(float *)req->Value)/100.0f;
          // FIXME: don't have any formulas
          // FIXME: could be different parameter!

          that->listener.update_reverb(UPDATE_REVERB_DENSITY);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get i3d_diffusion [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
  	break;
  // EAX 2.0 compatibility
  case DSPROPERTY_EAX20LISTENER_FLAGS+EAX20_OFFSET:
	if(req->ValueSize==sizeof(INT))
  	{
  	 if(get) 
  	  *(INT *)req->Value=that->listener.eax_flags&(~EAX20LISTENERFLAGS_RESERVED);
  	 else
  	 {
  	  that->listener.eax_flags=((*(INT *)req->Value)&(~EAX20LISTENERFLAGS_RESERVED))|(that->listener.eax_flags&EAX20LISTENERFLAGS_RESERVED);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get eax2_flags [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
  // EAX 1.0 compatibility
  case DSPROPERTY_EAX10_ALL+EAX10_OFFSET:
	if(req->ValueSize==sizeof(EAX10_REVERBPROPERTIES))
  	{
  	 if(get) 
  	 {
  	  EAX10_REVERBPROPERTIES *out=(EAX10_REVERBPROPERTIES *)req->Value;
  	  out->environment=that->listener.eax_environment;
  	  out->fVolume=that->listener.eax1_volume;
  	  out->fDecayTime_sec=that->listener.decay_time;
  	  out->fDamping=that->listener.eax1_damping;
  	 }
  	 else
  	 {
  	  EAX10_REVERBPROPERTIES *in=(EAX10_REVERBPROPERTIES *)req->Value;
  	  that->listener.eax_environment=in->environment;
  	  that->listener.eax1_volume=in->fVolume;
  	  that->listener.decay_time=in->fDecayTime_sec;
  	  that->listener.eax1_damping=in->fDamping;

          // recalculate decay times
          that->listener.recalc_decay();

  	  that->listener.reset_eax(that->listener.eax_environment);
  	  that->listener.for_each(-1,RECALC_EAX);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get eax1_all [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
  case DSPROPERTY_EAX10_VOLUME+EAX10_OFFSET:
	if(req->ValueSize==sizeof(FLOAT))
  	{
  	 if(get) 
  	  *(FLOAT *)req->Value=that->listener.eax1_volume;
  	 else
  	 {
  	  that->listener.eax1_volume=*(FLOAT*)req->Value;

          // update reverb engine
          that->listener.update_reverb(UPDATE_REVERB_VOL);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get eax1_vol [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
  case DSPROPERTY_EAX10_DAMPING+EAX10_OFFSET:
	if(req->ValueSize==sizeof(FLOAT))
  	{
  	 if(get) 
  	  *(FLOAT *)req->Value=that->listener.eax1_damping;
  	 else
  	 {
  	  that->listener.eax1_damping=*(FLOAT*)req->Value;

          // update reverb engine
          that->listener.update_reverb(UPDATE_REVERB_DAMPING);
  	 }
  	} else { debug(DWDM,"!! invalid Value size [%d] -- [%d] set/get eax1_damp [l]\n",req->ValueSize,get); that->hw->cb.rest_fpu_state(&state); return STATUS_INVALID_PARAMETER; }
  	break;
  default:
  	debug(DWDM,"!! invalid 3-D/EAX property [listener] [%d/%d] [%d]\n",id,req->PropertyItem->Id,get);
  	that->hw->cb.rest_fpu_state(&state);
  	return STATUS_INVALID_PARAMETER;
 }

 if(invalidate_env)
  that->listener.eax_environment=EAX_ENVIRONMENT_UNDEFINED;

 if(!get && (that->listener.batch==0) && (!deferred)) // apply changes
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
