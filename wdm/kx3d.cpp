// kX 3-D Audio Technology and Environmental Audio
// Copyright (c) Eugene Gavrilov, 2002-2014.
// All rights reserved.

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


// some more math...

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline float vector_angle(kVector &v1,kVector &v2)
{
	float m1, m2, mt, dp;

	m1 = magnitude(v1);
	m2 = magnitude(v2);
	dp = dot_product(v1, v2);
	mt = m1 * m2;

	if (mt == 0.f) 
	 return 0.0f;

	return (dp / (float)kx_sqrt(mt));
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline void normalyze(kVector &position)
{
 float magn=magnitude(position);
 if(magn!=0.0f)
 {
  position.x/=magn;
  position.y/=magn;
  position.z/=magn;
 }
};

// kListener stuff

kListener::kListener()
{
	// instance=instance_; // already done in CMiniportWave::Init
	reset();
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
int kListener::reset()
{
		batch=0;
		allocated=0;

		zero_vector(position);
		zero_vector(velocity);

		init_vector(top,0.0f,1.0f,0.0f);
		init_vector(front,0.0f,0.0f,1.0f);
        	init_vector(right,1.0f,0.0f,0.0f); // really?..

		distance_factor=1.0f;
		doppler_factor=1.0f;
		rolloff_factor=1.0f;

		doppler_velocity=AIR_SPEED; // /distance_factor

		s_list=0;

		reset_eax(EAX_ENVIRONMENT_GENERIC);

		return 0;
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline void kListener::set_all(KSDS3D_LISTENER_ALL &all)
{
		doppler_factor=all.DopplerFactor;
		rolloff_factor=all.RolloffFactor;
		distance_factor=all.DistanceFactor;

		if(distance_factor!=0.0f)
		{
		 doppler_velocity=AIR_SPEED/distance_factor;
		}
		else
		{
		 doppler_velocity=AIR_SPEED;
		}

		copy_vector(top,all.OrientTop);
		copy_vector(front,all.OrientFront);
        	init_vector(right,top.y * front.z - top.z * front.y,
        		          top.z * front.x - top.x * front.z,
        		          top.x * front.y - top.y * front.x);

		set_position(all.Position); // sets cone and spatial, too
		set_velocity(all.Velocity);
};

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline void kListener::set_distance_factor(float f)
{
		distance_factor=f;
		if(distance_factor!=0.0f)
		{
		 doppler_velocity=AIR_SPEED/distance_factor;
		}
		else
		 doppler_velocity=AIR_SPEED;

                // @for each voice
                // recalc_doppler(); // don't recalc velocity vectors

                for_each(-1,RECALC_DOPPLER);
                for_each(-1,RECALC_HF);
};

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline void kListener::set_velocity(DS3DVECTOR &vel)
{
		 copy_vector(velocity,vel);

                // @for each voice
                // if(mode==head_related)
                //   nop=
                // else
                //   set_velocity(ori_velocity);

                for_each(DS3DMODE_NORMAL,SET_VELOCITY);
};

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline void kListener::set_position(DS3DVECTOR &pos)
{
		copy_vector(position,pos);

                // @for each voice
                // if(mode==head_related)
                //   nop;
                // else
                //   set_position(ori_position);

                for_each(DS3DMODE_NORMAL,SET_POSITION);
};

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline void kListener::set_doppler_factor(float f)
{
		doppler_factor=f;

                // @for each voice
                // recalc_doppler(); // don't recalc velocity vectors
                for_each(-1,RECALC_DOPPLER);
};

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline void kListener::set_rolloff_factor(float f)
{
		rolloff_factor=f;

                // @for each voice
                // recalc_distance_attn(0,1); // don't recalc magnitude; recalc attn
                for_each(-1,RECALC_DISTANCE_ATTN_01);
};

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline void kListener::set_batch(int b) 
{
	 //if(b==batch) 
	 // return;
	 batch=b;

         // changes will be applied in Listener's property handler
};

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline void kListener::set_orientation(KSDS3D_LISTENER_ORIENTATION &ori)
{
		copy_vector(top,ori.Top);
		copy_vector(front,ori.Front);

        	// calc 'right' vector:
        	init_vector(right,top.y * front.z - top.z * front.y,
        		          top.z * front.x - top.x * front.z,
        		          top.x * front.y - top.y * front.x);

                // ...
                // @for each voice
                // if(mode==head_relative) set_cone_orientation(,0) // depends on 'front' vector
                //						    // don't recalc if angle is the same
                // if(mode!=head_relative) recalc_spatial();

                for_each(DS3DMODE_HEADRELATIVE,SET_CONE_ORIENTATION0);
                for_each(DS3DMODE_NORMAL,RECALC_SPATIAL);
};

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
void kListener::add(kSource *s,void *instance_)
{
 if(allocated && instance)
 {
  s->reset(this,instance_);

  unsigned long flags;
  kx_lock_acquire(((CMiniportWave *)instance)->hw,
   &(((CMiniportWave *)instance)->listener_lock), &flags);
  // add to chain
  s->s_next=s_list;
  s_list=s;

  s->in_chain=1;

  kx_lock_release(((CMiniportWave *)instance)->hw,&((CMiniportWave*)instance)->listener_lock,&flags);
 }
 else
  debug(DERR,"adding kSource: all:%x i:%x\n",allocated,instance);
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
void kListener::remove(kSource *src)
{
 if(allocated && instance)
 {
  unsigned long flags;
  kx_lock_acquire(((CMiniportWave *)instance)->hw,
   &(((CMiniportWave *)instance)->listener_lock), &flags);

  kSource *prev=0;
  kSource *s=s_list;

  while(s)
  {
   if(s==src)
   {
    if(prev)
     prev->s_next=src->s_next;
    else // remove first one
     s_list=src->s_next;

    kx_lock_release(((CMiniportWave *)instance)->hw,&((CMiniportWave*)instance)->listener_lock,&flags);

    src->reset(NULL,NULL);
    return;
   }
   prev=s;
   s=s->s_next;
  }

  kx_lock_release(((CMiniportWave *)instance)->hw,&((CMiniportWave*)instance)->listener_lock,&flags);

  debug(DWDM,"!! invalid source to remove [%x]\n",src);
 }
 else
  debug(DERR,"removing kSource: all:%x i:%x\n",allocated,instance);
}

kSource::kSource()
{
	reset(NULL,0);
};

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
int kSource::reset(kListener *l,void *instance_)
{
		// should be no listener-dependent calls here!
		listener=l;
		s_next=0;
		in_chain=0;
		update=0;
		instance=instance_;

		inside_angle=outside_angle=DS3D_DEFAULTCONEANGLE; 	// 360 no sound cone
		outside_volume_ori=DS3D_DEFAULTCONEOUTSIDEVOLUME;	// 0 no attenuation
		outside_volume=0;					// in KX_VOL units
		max_distance=DS3D_DEFAULTMAXDISTANCE; 			// 1000000000.0f
		min_distance=DS3D_DEFAULTMINDISTANCE;			// 1.0f

		distance=0.0f;
		distance_attn=0;
		cone_attn=0;
		cone_attn_hf=0;

		// orientation_magnitude=0; // not needed - assume 1.0f
		position_magnitude=0;

		angle=0;

		bad_angle=1;
		bad_orientation=1;
		bad_cone=1;

		mode=DS3DMODE_NORMAL;

		doppler=1.0f;

		d_vl=0.0f;
		d_vs=0.0f;

                // orientation, position & velocity are set to 0.0 by the constructors
                 zero_vector(r_orientation);
                 zero_vector(r_position);
                 zero_vector(n_position);
                 zero_vector(r_velocity);

                 zero_vector(ori_orientation);
                 zero_vector(ori_position);
                 zero_vector(ori_velocity);

                // spatial
                 azimuth=0;
                 elevation=0;
                 zero_vector(p_position);

                 send[0]=FRONT_LEFT;
                 send[1]=FRONT_RIGHT;
                 send_amount[0]=0x80;
                 send_amount[1]=0x80;

                 // eax stuff
                 reset_eax();

                 update=UPDATE_ALL;

                 return 0;
};

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline void kSource::set_all(KSDS3D_BUFFER_ALL &all)
{
		mode=all.Mode;
		distance=-1.0f; // forces recalc_dist_attn to recalc (min/max dist changed)

		min_distance=all.MinDistance;
		limit(min_distance,FLT_MIN,FLT_MAX);

		max_distance=all.MaxDistance;
		limit(max_distance,FLT_MIN,FLT_MAX);

		set_position(all.Position,1); // don't recalc cone
		set_velocity(all.Velocity);

		set_cone(all.ConeOrientation,all.InsideConeAngle,all.OutsideConeAngle,all.ConeOutsideVolume);
};

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline void kSource::set_cone(DS3DVECTOR &orient,int inside,int outside,int outside_vol)
{
		inside_angle=inside; limit(inside_angle,0,360);
		outside_angle=outside; limit(outside_angle,0,360);
		if(inside_angle>outside_angle)
		 outside_angle=inside_angle;
		if(inside_angle==360)
		{
		 bad_cone=1;

		 if(cone_attn!=0)
		 {
		  cone_attn=0;
		  update|=UPDATE_VOL;
		 }

		 if(cone_attn_hf!=0)
		 {
		  cone_attn_hf=0;
                  update|=(UPDATE_DIRECT_HF|UPDATE_ROOM_HF);
		 }

		 copy_vector(ori_orientation,orient);
		 return;
		}

		bad_cone=0;

		outside_volume_ori=outside_vol; limit(outside_volume_ori,DSBVOLUME_MIN,DSBVOLUME_MAX);
		outside_volume=outside_vol*655; // result: *65536 (is already *100)

		set_cone_orientation(orient,1); // force attn, because inside/outside changed
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline void kSource::set_cone_orientation(DS3DVECTOR &orient,int force_attn)
{
		copy_vector(ori_orientation,orient);

		bad_orientation=0;

		if(listener)
		{
        		if(mode==DS3DMODE_NORMAL)
        		{
                         // normalize orientation here...
                         float mgn=magnitude(orient);
                         if(mgn!=0.0f)
                         {
                          r_orientation.x=orient.x/mgn;
                          r_orientation.y=orient.y/mgn;
                          r_orientation.z=orient.z/mgn;
                         }
                          else 
                         {
                          zero_vector(r_orientation);
                          bad_orientation=1;
                          if(cone_attn!=0)
                          {
                           cone_attn=0;
                           update|=UPDATE_VOL;
                          }
                          if(cone_attn_hf!=0)
                          {
                           cone_attn_hf=0;
                           update|=(UPDATE_DIRECT_HF|UPDATE_ROOM_HF);
                          }
                          return;
                         }
        		}
        		else if(mode==DS3DMODE_HEADRELATIVE)
        		{
        		 add_vector(r_orientation,orient,listener->front); 

                         float mgn=magnitude(r_orientation);
                         if(mgn!=0.0f)
                         {
                          r_orientation.x/=mgn;
                          r_orientation.y/=mgn;
                          r_orientation.z/=mgn;
                         }
                          else 
                         {
                          zero_vector(r_orientation);
                          bad_orientation=1;
                          if(cone_attn!=0)
                          {
                           cone_attn=0;
                           update|=UPDATE_VOL;
                          }
                          if(cone_attn_hf!=0)
                          {
                           cone_attn_hf=0;
                           update|=(UPDATE_DIRECT_HF|UPDATE_ROOM_HF);
                          }
                          return;
                         }
        		}
        		else // incorrect mode...
        		{
                          zero_vector(r_orientation);
                          bad_orientation=1;
                          if(cone_attn!=0)
                          {
                           cone_attn=0;
                           update|=UPDATE_VOL;
                          }
                          if(cone_attn_hf!=0)
                          {
                           cone_attn_hf=0;
                           update|=(UPDATE_DIRECT_HF|UPDATE_ROOM_HF);
                          }
                          return;
        		}
        	}
		else
		{
		 zero_vector(r_orientation);
		 bad_orientation=1;
		 if(cone_attn!=0)
		 {
		  cone_attn=0;
		  update|=UPDATE_VOL;
		 }
		 if(cone_attn_hf!=0)
		 {
		  cone_attn_hf=0;
		  update|=(UPDATE_DIRECT_HF|UPDATE_ROOM_HF);
		 }
		 return;
		}
		// calc orientation magnitude

		if(bad_orientation)
		{
		 if(cone_attn!=0)
		 {
		  cone_attn=0;
                  update|=UPDATE_VOL;
                 }
                 if(cone_attn_hf!=0)
                 {
                  cone_attn_hf=0;
                  update|=(UPDATE_DIRECT_HF|UPDATE_ROOM_HF);
                 }
		}
		else
		 recalc_cone_attn(1,force_attn);  // force recalc: angle, because orientation changed
						 // force attn: (if needed) if inside/outside changed
};

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline void kSource::set_cone_angles(int inside,int outside)
{
		inside_angle=inside; limit(inside_angle,0,360);
		outside_angle=outside; limit(outside_angle,0,360);

		if(inside_angle>outside_angle)
		 outside_angle=inside_angle;

		if(inside_angle==360)
		{
		 bad_cone=1;

		 if(cone_attn!=0)
		 {
		  cone_attn=0;
		  update|=UPDATE_VOL;
		 }
		 if(cone_attn_hf!=0)
		 {
		  cone_attn_hf=0;
		  update|=(UPDATE_DIRECT_HF|UPDATE_ROOM_HF);
		 }
		 return;
		}
		else
		 bad_cone=0;

		recalc_cone_attn(0,1); // force to recalc attn only
};

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline void kSource::set_mode(int mode_)
{ 
	 if(mode!=mode_)
	 {
	  mode=mode_;
       	  set_position(ori_position,1); // don't recalc cone
       	  set_velocity(ori_velocity);

       	  set_cone_orientation(ori_orientation); // don't recalc cone attn, if angle didn't change
       	 }
};

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline void kSource::set_cone_outside_volume(int vol)
{ 
	   outside_volume_ori=vol; 
	   limit(outside_volume_ori,DSBVOLUME_MIN,DSBVOLUME_MAX);
	   outside_volume=outside_volume_ori*655; 

           // recalc
           if(!bad_angle && !bad_cone)
           {
            if(angle>=outside_angle)
            {
              cone_attn=outside_volume; // KX_VOL units
              update|=UPDATE_VOL;
            }
            else
              recalc_cone_attn(0,1); // don't recalc angle; recalc attn
           }
};

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline void kSource::set_min_distance(float d) 
{ 
	   min_distance=d; 
	   limit(min_distance,FLT_MIN,FLT_MAX); 
	   float dst2=distance;
	   limit(dst2,min_distance,max_distance); 
	   if(dst2!=distance)
	   { distance=dst2; recalc_distance_attn(0,1); }
           // don't calc the distance; calc the attn
           // doesn't affect the cone, because the position didn't change
};

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline void kSource::set_max_distance(float d) 
{
	   max_distance=d; 
	   limit(max_distance,FLT_MIN,FLT_MAX); 
	   float dst2=distance;
	   limit(dst2,min_distance,max_distance); 
	   if(dst2!=distance)
	   { distance=dst2; recalc_distance_attn(0,1); }
	   // don't calc the distance; calc the attn
	   // doesn't affect the cone, because the position didn't change
};

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline void kSource::set_position(DS3DVECTOR &pos,int no_cone_recalc)
{
		copy_vector(ori_position,pos);

		if(listener)
		{
		 if(mode==DS3DMODE_NORMAL)
		 {
		   substract_vector(r_position,pos,listener->position);
		 }
		 else 
		  if(mode==DS3DMODE_HEADRELATIVE)
		  {
		    copy_vector(r_position,pos);
		  }
		  else
		  {
		    zero_vector(r_position);
		  }
		}
		else
		{
		 zero_vector(r_position);
		}

                recalc_distance_attn(1,0); // calc distance [position changed]
                			   // recalc attn if necessary
                if(no_cone_recalc==0)
                 recalc_cone_attn(1,0); // force angle recalc [position changed]
                 		      // don't recalc attn if angle is the same

                // spatialization stuff here
                recalc_spatial();

                if(no_cone_recalc==0) // change magnitude signs if necessary
                  recalc_doppler(1); // recalc magnitudes
};

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline void kSource::set_velocity(DS3DVECTOR &vel)
{
		copy_vector(ori_velocity,vel);

		if(listener)
		{
		 if(mode==DS3DMODE_NORMAL)
		 {
		  copy_vector(r_velocity,vel);
		 }
		 else 
		  if(mode==DS3DMODE_HEADRELATIVE)
		  {
		   add_vector(r_velocity,vel,listener->velocity);
		  }
		  else
		  {
		   zero_vector(r_velocity);
		  }
		}
		else
		{
		 zero_vector(r_velocity);
		}

		recalc_doppler(1); // force magnitude_recalc
};

// calculations
#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline void kSource::recalc_distance_attn(int recalc_distn,int force_attn)
{
        	float distance2=distance;
        	if(recalc_distn)
        	{
        		distance=magnitude(r_position);
        		position_magnitude=distance; // doesn't depend on min_dist -- vector only

        		if(distance!=0.0f)
        		{
        		 n_position.x=r_position.x/distance;
        		 n_position.y=r_position.y/distance;
        		 n_position.z=r_position.z/distance;
        		}
        		else
        		 zero_vector(n_position);

        		limit(distance,min_distance,max_distance);
        	}

		if(force_attn || (distance2!=distance))
		{
        	 if(min_distance!=0.0f)
        	 {
        	  if(listener->rolloff_factor!=0.0f || eax_rolloff_factor!=0.0f)
        	  {
//        	   if(distance==max_distance)
//        	   {
//        	           distance_attn=-100*65536;
                           // FIXME NOW: 3536c 2/11/2003
                           // should check for DSBCAPS_MUTE3DATMAXDISTANCE
                           // but it is uncertain...
//        	   }
//        	   else
        	   {
                	   // -1310720.0 = 65536.0f*(-20.0f)

                	   // kX: pre-3536c: distance_attn=(int)(-1310720.0f*kx_log10((listener->rolloff_factor+eax_rolloff_factor)*distance/min_distance));
                           // creative: -20*log10( (min_dist+rolloff*(distance-min_dist)) / min_dist)
                           // AL: G_dB = GAIN - 20*log10(1 + ROLLOFF_FACTOR*(dist-REFERENCE_DISTANCE)/REFERENCE_DISTANCE );

                	   distance_attn=(int)(-1310720.0f*kx_log10(1.0f+(listener->rolloff_factor+eax_rolloff_factor)*(distance-min_distance)/min_distance));
                	   limit(distance_attn,-100*65536,0);
        	   }
        	  }
        	  else
        	  {
        	   distance_attn=0; // no attenuation
        	  }
        	 }
        	 else
        	 {
        	  if(distance==0.0f)
        	  {
        	   distance_attn=0; // the attenuation is 0dB
        	  }
        	  else
        	  {
        	   distance_attn=-100*65536; // the sound is muted
        	  }
        	 }

                 // recalc distance-related EAX stuff here: [air absorption + room rolloff]
                 recalc_room();
                 recalc_room_hf();
                 recalc_direct_hf();
                 recalc_room_lf();
                 recalc_direct_lf();

        	 update|=UPDATE_VOL;
        	}
};

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline void kSource::recalc_cone_attn(int recalc_angle,int force_attn)
{
        	if(bad_cone)
        	{
        	 if(cone_attn!=0)
        	 {
        	  cone_attn=0;
        	  update|=UPDATE_VOL;
        	 }
        	 if(cone_attn_hf!=0)
        	 {
        	  cone_attn_hf=0;
        	  update|=(UPDATE_DIRECT_HF|UPDATE_ROOM_HF);
        	 }
        	 return;
        	}

        	int angle2=angle;
        	if(recalc_angle)
        	{
                	// calc the cosinus
                        // -r_position is vector 'BL' (r_position is vector 'LB')
                        if((bad_orientation!=1) && (position_magnitude!=0.0f))
                        {
                	 float cosinus=-dot_product(r_position,r_orientation);
                	 angle=abs((int)kx_arccos(cosinus/(position_magnitude))*2);
                         // orientation_magnitude is always '1.0f'
                	 limit(angle,0,180);
                	 bad_angle=0;
                        }
                        // else angle is useless
                        else
                        {
                         bad_angle=1;
                         angle=0;
                         if(cone_attn!=0)
                         {
                          cone_attn=0;
                          update|=UPDATE_VOL;
                         }
                         return;
                        }
                }
                if((!bad_angle) && (force_attn || (angle!=angle2)))
                {
                	if((angle<=inside_angle) || (outside_volume==0))
                	{
                	 cone_attn=0;
                	 update|=UPDATE_VOL;
                	}
                	else
                	{
                	 if(angle>=outside_angle)
                	 {
                	  cone_attn=outside_volume; // in KX_VOL units
                	  update|=UPDATE_VOL;
                	 }
                	 else
                	 {
                	  // recalc transitional attn
                          // linear:
                          //  DGAIN = 1 - (1-CONE_OUTER_GAIN)*((theta-INNER)/(OUTER-INNER))
                          // ...
                          // FIXME
                          // Creative:
                          // cone_attn=-10log10((1-cos(mang)-opow*(1+cos(mang)))/2)
                          // mang=(inside+opow*outside)/(1+outside)
                          // opow=10^((outside_volume[+eax_outside_volume_hf])/10)
                          cone_attn=outside_volume*(angle-inside_angle)/(outside_angle-inside_angle); // in KX_VOL units

                          update|=UPDATE_VOL;

                          limit(cone_attn,-100*65536,0);
                	 }
                	}
                         // EAX angle-related stuff (outside volume hf)
                         recalc_cone_hf();
                }
};

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline void kSource::recalc_doppler(int recalc_magnitude)
{
        	if(recalc_magnitude)
        	{
        	 // 'between' is actually 'relative source position'
                 // 3536c fix: -dot_product (since source-listener, not listener-source vectors!)
        	 d_vl=-dot_product(n_position,listener->velocity);
        	 d_vs=-dot_product(n_position,r_velocity);
        	}

        	if(listener->doppler_factor!=0.0f || eax_doppler_factor!=0.0f)
        	{
        	 float tmp=(listener->doppler_velocity-(listener->doppler_factor+eax_doppler_factor)*d_vl);

        	 if(tmp<0)
        	  doppler=0.01f;
        	 else
        	  doppler=tmp/
        	         (listener->doppler_velocity+(listener->doppler_factor+eax_doppler_factor)*d_vs);
        	 update|=UPDATE_PITCH;
        	}
        	else
        	{
        	 if(doppler!=1.0f)
        	 {
        	  doppler=1.0f;
        	  update|=UPDATE_PITCH;
        	 }
        	}
};


typedef struct
{
 int min_angle;
 int max_angle;
 int send0;
 int send1;
}decision_table_t;

decision_table_t table51_kx[]=
{
 { 330, 360, FRONT_LEFT,   FRONT_CENTER },	// 30
 { 0,    30, FRONT_CENTER, FRONT_RIGHT  },	// 30
 { 30,  110, FRONT_RIGHT,  REAR_RIGHT   },	// 80
 { 110, 250, REAR_RIGHT,   REAR_LEFT    },      // 140
 { 250, 330, REAR_LEFT,    FRONT_LEFT   }	// 80
};

/*
decision_table_t table61_kx[]=
{
 { 330, 360, FRONT_LEFT,   FRONT_CENTER  },	// 30
 { 0,    30, FRONT_CENTER, FRONT_RIGHT   },	// 30
 { 30,  110, FRONT_RIGHT,  REAR_RIGHT    },	// 80
 { 110, 180, REAR_RIGHT,   REAR_CENTER   },	// 70
 { 180, 250, REAR_CENTER,  REAR_LEFT     },	// 70
 { 250, 330, REAR_LEFT,    FRONT_LEFT    }	// 80
};

decision_table_t table51_cl[]=
{
 { 337, 360, FRONT_LEFT,   FRONT_CENTER },	// 23
 { 0,    23, FRONT_CENTER, FRONT_RIGHT  },	// 23
 { 23,  120, FRONT_RIGHT,  REAR_RIGHT   },	// 97
 { 120, 240, REAR_RIGHT,   REAR_LEFT    },      // 120
 { 240, 337, REAR_LEFT,    FRONT_LEFT   }	// 97
};

decision_table_t table61_cl[]=
{
 { 337, 360, FRONT_LEFT,   FRONT_CENTER  },	// 23
 { 0,    23, FRONT_CENTER, FRONT_RIGHT   },	// 23
 { 23,  120, FRONT_RIGHT,  REAR_RIGHT    },	// 97
 { 120, 180, REAR_RIGHT,   REAR_CENTER   },	// 60
 { 180, 240, REAR_CENTER,  REAR_LEFT     },	// 60
 { 240, 337, REAR_LEFT,    FRONT_LEFT    }	// 97
};

*/
decision_table_t table71_cl[]=
{
 { 337, 360, FRONT_LEFT,   FRONT_CENTER  },	// 23
 { 0,    23, FRONT_CENTER, FRONT_RIGHT   },	// 23
 { 23,   90, FRONT_RIGHT,  TARGET_RIGHT  },     // 67
 { 90,  120, TARGET_RIGHT, REAR_RIGHT    },	// 30
 { 120, 180, REAR_RIGHT,   REAR_CENTER   },	// 60
 { 180, 240, REAR_CENTER,  REAR_LEFT     },	// 60
 { 240, 270, REAR_LEFT,    TARGET_LEFT   },     // 30
 { 270, 337, TARGET_LEFT,  FRONT_LEFT    }	// 67
};

decision_table_t table_user[]=
{
 { 337, 360, FRONT_LEFT,   FRONT_CENTER  },	// 23
 { 0,    23, FRONT_CENTER, FRONT_RIGHT   },	// 23
 { 23,   90, FRONT_RIGHT,  TARGET_RIGHT  },     // 67
 { 90,  120, TARGET_RIGHT, REAR_RIGHT    },	// 30
 { 120, 180, REAR_RIGHT,   REAR_CENTER   },	// 60
 { 180, 240, REAR_CENTER,  REAR_LEFT     },	// 60
 { 240, 270, REAR_LEFT,    TARGET_LEFT   },     // 30
 { 270, 337, TARGET_LEFT,  FRONT_LEFT    }	// 67
};

// FIXME: SPEAKER_BOTTOM / SPEAKER_TOP

decision_table_t *table_list[]=
{
 table51_kx,
// table61_kx,
// table51_cl,
// table61_cl,
 table71_cl,
 table_user,
 NULL
};

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
inline void kSource::recalc_spatial()
{
 p_position.x=dot_product(r_position,listener->right);
 p_position.y=dot_product(r_position,listener->top);
 p_position.z=dot_product(r_position,listener->front);

 float magn_=dot_product(p_position,p_position);
 float magn=(float)kx_sqrt(magn_);

 if(magn!=0.0f)
 {
    // elevation: arcsin(p_position.y / magn);
    elevation = 90L - kx_arccos(p_position.y / magn);

    float projection=(float)kx_sqrt(magn_-p_position.y*p_position.y); // (float)kx_sqrt(p_position.x*p_position.x+p_position.z*p_position.z);

    // azimuth
    azimuth = kx_arccos(p_position.z / projection);

    if(p_position.x<0)
     azimuth=360L-azimuth;
 }
 else
 {
    elevation=0;
    azimuth=0;
 }

 int target0=-1,target1=-1;

 decision_table_t *table;

 CMiniportWaveOutStream *that=(CMiniportWaveOutStream *)instance;
 int pnt=0;

 if(that && (that->magic==WAVEOUTSTREAM_MAGIC))
 {
  if(that->Miniport->kx3d_sp8ps&KX_HW_8PS_ON)
   pnt=1;
  // ele pnt=0;
 }
 else
  debug(DERR,"internal error: invalid sp8ps in recalc_spatial()\n");

 table=table_list[pnt];
 if(table==NULL)
  table=table_list[0];

 limit(azimuth,0,360);

 int i;
 for(i=0;i<8;i++) // max_speaker
 {
  if(azimuth>=table[i].min_angle && azimuth<=table[i].max_angle)
  {
    // re-assign fxbuses

    if(send[0]==table[i].send0)
    {
     target0=0;
     target1=1;
     if(send[1]==table[i].send1) // everything's ok
     {
      // nop
     }
     else
     {
      // send[1]!=send1
      send[1]=table[i].send1;
      update|=UPDATE_FXBUS;
     }
    }
    else
    {
     if(send[0]==table[i].send1)
     {
      target0=1;
      target1=0;
      if(send[1]==table[i].send0) // reverse ok
      {
       // nop
      }
      else
      {
       // send[1]!=send0
       send[1]=table[i].send0;
       update|=UPDATE_FXBUS;
      }
     }
     else
     {
      // send[0]!=send0 && send[0]!=send1
      // check send[1]
           if(send[1]==table[i].send1)
           {
            target0=0;
            target1=1;
            // send[0]!=send0
            send[0]=table[i].send0;
            update|=UPDATE_FXBUS;
           }
           else
           {
            if(send[1]==table[i].send0)
            {
             target0=1;
             target1=0;
             // send[0]!=send1
             send[0]=table[i].send1;
             update|=UPDATE_FXBUS;
            }
            else
            {
             // everything's different!
             target0=0;
             target1=1;
             send[0]=table[i].send0;
             send[1]=table[i].send1;
             update|=UPDATE_FXBUS;
            }
           }
     }
    }
    break;
  }
 }

 if(i==8)
 {
  debug(DERR,"Invalid speaker configuration in kx3D [%d]\n",(int)azimuth);
  i=0;
 }

 if(target0!=-1 && target1!=-1)
 {
    int send_level = ((azimuth - table[i].min_angle) * 0xFFU) / (table[i].max_angle-table[i].min_angle);

    if(send_amount[target1]!=send_level || send_amount[target0]!=(0xff-send_level))
    {
     send_amount[target1] = send_level;
     send_amount[target0] = 0xff - send_level;
     update|=UPDATE_AMOUNT;
    }
 }
 else
 {
  debug(DWDM,"!! invalid target!\n");
 }
};

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
void kListener::for_each(int mode,int what)
{
 if(!instance)
 {
  if(allocated)
   debug(DERR,"!! invalid instance in listener::for_each\n");
  return;
 }

 unsigned long flags;
 kx_lock_acquire(((CMiniportWave *)instance)->hw,
   &(((CMiniportWave *)instance)->listener_lock), &flags);

 kSource *s=s_list;
 
 while(s)
 {
  if(mode==-1 || s->mode==mode)
  {
   switch(what)
   {
     case RECALC_DOPPLER: // recalc_doppler(); 
     		s->recalc_doppler(); // don't recalc S.velocity_magnitude
     		break;
     case SET_VELOCITY: // set_velocity(ori_velocity);
     		s->set_velocity(s->ori_velocity);
     		break;
     case SET_POSITION: // set_position(ori_position); recalc cone and doppler
     		s->set_position(s->ori_position);
     		break;
     case RECALC_DISTANCE_ATTN_01: // recalc_distance_attn(0,1); // don't recalc magnitude; recalc attn
     		s->recalc_distance_attn(0,1);
     		break;
     case RECALC_SPATIAL:
                // if(mode!=head_relative) recalc_spatial();
                s->recalc_spatial();
                break;
     case SET_CONE_ORIENTATION0:
                // if(mode==head_relative) set_cone_orientation(,0) // depends on 'front' vector
                //						    don't recalc if angle is the same
                s->set_cone_orientation(s->ori_orientation,0);
                break;
     case RECALC_HF:
     		// air parameters changed (related to distance_factor)
     		s->recalc_direct_hf();
     		s->recalc_room_hf();
     		break;
     case RECALC_LF:
     		// air parameters changed (related to distance_factor)
     		s->recalc_direct_lf();
     		s->recalc_room_lf();
     		break;
     case RECALC_DIRECT:
     		// air parameters changed (related to distance_factor)
     		s->recalc_direct_hf();
     		s->recalc_direct_lf();
     		break;
     case RECALC_ROOM_ONLY:
     		// air parameters changed (related to distance_factor)
     		s->recalc_room();
     		break;
     case RECALC_REFLECTED:
     		// air parameters changed (related to distance_factor)
     		s->recalc_room();
     		s->recalc_room_lf();
     		s->recalc_room_hf();
     		break;
     case RECALC_ROOMHFLF:
     		// air parameters changed (related to distance_factor)
     		s->recalc_room_hf();
     		s->recalc_room_lf();
     		break;
     case RECALC_EAX:
     		// air parameters changed (related to distance_factor)
     		s->recalc_cone_hf();
     		s->recalc_direct_hf();
     		s->recalc_direct_lf();
     		s->recalc_room();
     		s->recalc_room_hf();
     		s->recalc_room_lf();
     		break;
   }
  }
  s=s->s_next;
 }

 kx_lock_release(((CMiniportWave *)instance)->hw,&((CMiniportWave*)instance)->listener_lock,&flags);
}

#include "eax/eax_presets.h"

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
int kListener::reset_eax(int env)
{
  if(env>=0 && env<=EAX_ENVIRONMENT_UNDEFINED)
  {
                // eax 1
		eax_environment=env;

                eax1_volume=eax10_list[env].fVolume;
                // decay_time is set below:
                // decay_time=eax10_list[env].fDecayTime_sec;
                eax1_damping=eax10_list[env].fDamping;

                // eax 2/3

                env_size=eax30_list[env].flEnvironmentSize;       // environment size in meters
                env_diffusion=eax30_list[env].flEnvironmentDiffusion;  // environment diffusion
                room=eax30_list[env].lRoom;                    // room effect level (at mid frequencies)
                room_hf=eax30_list[env].lRoomHF;                  // relative room effect level at high frequencies
                room_lf=eax30_list[env].lRoomLF;                  // relative room effect level at low frequencies  
                decay_time=eax30_list[env].flDecayTime;             // reverberation decay time at mid frequencies
                decay_hf_ratio=eax30_list[env].flDecayHFRatio;          // high-frequency to mid-frequency decay time ratio
                decay_lf_ratio=eax30_list[env].flDecayLFRatio;          // low-frequency to mid-frequency decay time ratio   
                reflections=eax30_list[env].lReflections;             // early reflections level relative to room effect
                reflections_delay=eax30_list[env].flReflectionsDelay;      // initial reflection delay time
                reverb=eax30_list[env].lReverb;                  // late reverberation level relative to room effect
                reverb_delay=eax30_list[env].flReverbDelay;           // late reverberation delay time relative to initial reflection
                echo_time=eax30_list[env].flEchoTime;              // echo time
                echo_depth=eax30_list[env].flEchoDepth;             // echo depth
                modulation_time=eax30_list[env].flModulationTime;        // modulation time
                modulation_depth=eax30_list[env].flModulationDepth;       // modulation depth
                air_absorption_hf=eax30_list[env].flAirAbsorptionHF;       // change in level per meter at high frequencies
                hf_reference=eax30_list[env].flHFReference;           // reference high frequency
                lf_reference=eax30_list[env].flLFReference;           // reference low frequency 
                room_rolloff_factor=eax30_list[env].flRoomRolloffFactor;     // like DS3D flRolloffFactor but for room effect

                eax_flags=eax30_list[env].ulFlags;         // modifies the behavior of properties

                init_vector(reverb_pan,eax30_list[env].vReverbPan.x,eax30_list[env].vReverbPan.y,eax30_list[env].vReverbPan.z);          // late reverberation panning vector
                init_vector(reflections_pan,eax30_list[env].vReflectionsPan.x,eax30_list[env].vReflectionsPan.y,eax30_list[env].vReflectionsPan.z);     // early reflections panning vector

                // recalculate decay times
                recalc_decay();
                update_reverb(UPDATE_REVERB_ALL);

                for_each(-1,RECALC_EAX);

                return 0;
  }
  else
  {
  		return -1;
  }
}

int kListener::update_reverb(int fl)
{
 // FIXME

 if(fl&UPDATE_REVERB_DIFFUSION)
 {
  // FIXME: diffusion might affect other reverb params (such as echo/density/...)
 }
 return 0;
}


#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
int kSource::reset_eax()
{
        eax1_mix=0.0f;

        // eax2/3
        direct=0;
        direct_hf=0;
        room=0;
        room_hf=0;
        obstruction=0;
        occlusion=0;
        exclusion=0;
        outside_volume_hf=0;

        eax_flags=0x7;

        room_rolloff_factor=0;
        obstruction_lf_ratio=0;
        occlusion_lf_ratio=0.25f;
        air_absorption_factor=1.0f;

        occlusion_room_ratio=1.5f;
        occlusion_direct_ratio=1.0f;
        exclusion_lf_ratio=1.0f;

        eax_doppler_factor=0;
        eax_rolloff_factor=0;

        total_direct_hf=0;
        total_direct_lf=0;
        total_room=0;
        total_room_hf=0;
        total_room_lf=0;

        cone_attn_hf=0;

        return 0;
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
void kSource::recalc_cone_hf()
{
 int old_c=cone_attn_hf;

 if(eax_flags&EAXBUFFERFLAGS_DIRECTHFAUTO)
 {
  if(!bad_angle)
  {
                	if((angle<=inside_angle) || (outside_volume_hf==0))
                	{
                	 cone_attn_hf=0;
                	}
                	else
                	{
                	 if(angle>=outside_angle)
                	 {
                	  cone_attn_hf=outside_volume_hf; // in KX_VOL units
                	 }
                	 else
                	 {
                	  // recalc transitional attn
                          // linear:
                          //  DGAIN = 1 - (1-CONE_OUTER_GAIN)*((theta-INNER)/(OUTER-INNER))
                          // ...
                          // FIXME
                          // Creative:
                          // cone_attn=-10log10((1-cos(mang)-opow*(1+cos(mang)))/2)
                          // mang=(inside+opow*outside)/(1+outside)
                          // opow=10^((outside_volume[+eax_outside_volume_hf])/10)
                          cone_attn_hf=outside_volume_hf*(angle-inside_angle)/(outside_angle-inside_angle); // in KX_VOL units

                          limit(cone_attn_hf,-100*65536,0);
                	 }
                	}
  }
 }

 if(cone_attn_hf!=old_c)
  update|=(UPDATE_DIRECT_HF|UPDATE_ROOM_HF);
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
int kSource::recalc_direct_hf()
{
 int old_t_d_hf=total_direct_hf;

 total_direct_hf=direct_hf+
 		  obstruction+
 		  (int)((float)occlusion*occlusion_direct_ratio)+
                  (int)(listener->air_absorption_hf*air_absorption_factor*(distance-min_distance)*listener->distance_factor*(listener->rolloff_factor+eax_rolloff_factor));
                  // FIXME: my version: air abs. was w/o (distance-min_distance)
                  // FIXME: both factors?.. why?

 if(old_t_d_hf!=total_direct_hf)
  update|=UPDATE_DIRECT_HF;
 return 0;
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
int kSource::recalc_direct_lf()
{
 int old_t_d_lf=total_direct_lf;

 float occl_factor=occlusion_direct_ratio;
 if(occl_factor>1.0f)
  occl_factor=1.0f;

 total_direct_lf=
 		(int)((float)obstruction*obstruction_lf_ratio)+
 		(int)((float)occlusion*occlusion_lf_ratio*occl_factor);
                // FIXME: per patent: occl_factor might be +=, not *=

 if(old_t_d_lf!=total_direct_lf)
  update|=UPDATE_DIRECT_LF;

 return 0;
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
int kSource::recalc_room()
{
 int old_r=total_room;

 total_room=listener->room+room;

 if(eax_flags&EAXBUFFERFLAGS_ROOMAUTO)
 {
  // FIXME: some code to change total_room depending on distance
  // ??
  // total_room+=
 }

 float rolloff=room_rolloff_factor+listener->room_rolloff_factor;
 if(rolloff!=0.0f)
 {
  // recalc reverb rolloff based on distance
  // FIXME: room rolloff attenuation [distance-based]
 }

 // FIXME: earlier_0dB=...

 // FIXME: eax1_mix

 if(old_r!=total_room)
  update|=UPDATE_ROOM;

 return 0;
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
int kSource::recalc_room_hf()
{
 int old_t_r_hf=total_room_hf;

 int occl=
     (int)((float)occlusion*occlusion_room_ratio)+exclusion;
     // FIXME: per patent: occl_room_ratio might be +=(1+F), not *=

 if(occl<(cone_attn+distance_attn+cone_attn_hf))
  occl-=(cone_attn+distance_attn+cone_attn_hf); // will be added again, thus, giving 'occl' only
 else
  occl=0;

 total_room_hf=listener->room_hf+room_hf
 	+occl
 	-(int)(60.0f*(listener->rolloff_factor+eax_rolloff_factor)*(distance-min_distance)/(AIR_SPEED*listener->decay_time))
        +(int)(listener->air_absorption_hf*air_absorption_factor*(distance-min_distance)*listener->distance_factor*(listener->rolloff_factor+eax_rolloff_factor));
        // FIXME: my version: air abs. was w/o (distance-min_distance)
        // FIXME: both factors?.. why?

 if(old_t_r_hf!=total_room_hf)
  update|=UPDATE_ROOM_HF;

 return 0;
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
int kSource::recalc_room_lf()
{
 int old_t_r_lf=total_room_lf;

 float occl_factor=occlusion_room_ratio;
 if(occl_factor>1.0f)
  occl_factor=1.0f;

 int occl=(int)((float)exclusion*exclusion_lf_ratio)+
     (int)((float)occlusion*occlusion_lf_ratio*occl_factor);
     // FIXME: per patent: occl_f might be +=(f+F), not *=
 if(occl<(cone_attn+distance_attn))
  occl-=(cone_attn+distance_attn); // will be added again, thus, giving 'occl' only
 else
  occl=0;

 total_room_lf=listener->room_lf
 	+occl
 	-(int)(60.0f*(listener->rolloff_factor+eax_rolloff_factor)*(distance-min_distance)/(AIR_SPEED*listener->decay_time));

 if(old_t_r_lf!=total_room_lf)
  update|=UPDATE_ROOM_LF;

 return 0;
}

int kListener::recalc_decay()
{
          // recalculate decay times
          decay_hf_time=decay_time*decay_hf_ratio;
          decay_lf_time=decay_time*decay_lf_ratio;

          if(eax_flags&EAXLISTENERFLAGS_DECAYHFLIMIT)
          {
           // limit high-frequency decay time according to air absorption.
           // FIXME
          }
          
          return 0;
}

#ifdef CE_OPTIMIZE
#pragma optimize("gty", on)
#pragma inline_depth(16)
#endif
int kListener::set_env_size(float sz)
{
  	  float ratio=sz/env_size;

  	  env_size=sz;

          // recalculate 7 parameters (if flags are set)

          int db=0;
          if(eax_flags&(EAXLISTENERFLAGS_REFLECTIONSSCALE|EAXLISTENERFLAGS_REVERBSCALE))
          {
           db=(int)(-20.0f*kx_log10(ratio));
          }

          if(eax_flags&EAXLISTENERFLAGS_REFLECTIONSSCALE)
           reflections+=db;
          if(eax_flags&EAXLISTENERFLAGS_REVERBSCALE)
           reflections+=db;

          if(eax_flags&EAXLISTENERFLAGS_DECAYTIMESCALE)
           decay_time*=ratio;
          if(eax_flags&EAXLISTENERFLAGS_REFLECTIONSDELAYSCALE)
           reflections_delay*=ratio;                      
          if(eax_flags&EAXLISTENERFLAGS_REVERBDELAYSCALE)
           reverb_delay*=ratio;
          if(eax_flags&EAXLISTENERFLAGS_ECHOTIMESCALE)
           echo_time*=ratio;
          if(eax_flags&EAXLISTENERFLAGS_MODULATIONTIMESCALE)
           modulation_time*=ratio;

          // no per-source recalcs (imho)
          // update reverb engine
          update_reverb(UPDATE_REVERB_ALL);

          return 0;
}
