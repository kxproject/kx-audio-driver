// kX 3-D Audio Technology and Environmental Audio
// Copyright (c) kX Project, 2001-2005.
// written by Eugene Gavrilov, 2002-2005.
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


#ifndef KX3D_H_
#define KX3D_H_

// note: C++ operator 'new' should be driver-safe

#include "kx3d/kdsound.h"

#ifndef NATIVE_MATH
 #define sqrt(a) kx_sqrt(a)
 #define log10(a) kx_log10(a)
 #define pow2(a) kx_pow2(a)
 #define pow10(a) kx_pow10(a)
#endif

#define kVector DS3DVECTOR
#define copy_vector(a,b) { memcpy(&a,&b,sizeof(a)); }
#define substract_vector(r,a,b) { r.x=a.x-b.x; r.y=a.y-b.y; r.z=a.z-b.z; }
#define add_vector(r,a,b) { r.x=a.x+b.x; r.y=a.y+b.y; r.z=a.z+b.z; }
#define zero_vector(a) { memset(&a,0,sizeof(a)); }

#define init_vector(v,x_,y_,z_) { v.x=x_; v.y=y_; v.z=z_; }

#define limit(a,minn,maxx) { if((a)<(minn)) a=(minn); else if((a)>(maxx)) a=(maxx); }
#define magnitude(a) ((float)kx_sqrt((a).x*(a).x+(a).y*(a).y+(a).z*(a).z))
#define dot_product(a,b) (((a).x)*((b).x)+((a).y)*((b).y)+((a).z)*((b).z))

#define AIR_SPEED	343
// Sound travels at 331 m/s in air at 0 degrees Celsius. 
// For temperatures "T" above 0 degrees, the equation v = 331 + 0.6 T describes the speed of sound.

class kSource;

// listener
class kListener
{
public:
	int allocated;
	void *instance; // CMiniportWave pointer

	kVector position;
	kVector velocity;

	kVector top,front,right;

	float distance_factor;
	float doppler_factor;
	float rolloff_factor;

	float doppler_velocity; 	// = 383/distance_factor (it is in 'units')

	int batch;

	kSource *s_list;

// virtual targets
#define TARGET_LEFT             0
#define FRONT_LEFT		1
#define FRONT_CENTER		2
#define FRONT_RIGHT		3
#define TARGET_RIGHT		4
#define REAR_RIGHT		5
#define REAR_CENTER		6
#define REAR_LEFT		7
#define SPEAKER_TOP		8
#define SPEAKER_BOTTOM		9
#define FX1			10
#define FX2			11

#define MAX_3D_TARGETS	(FX2+1)

	int routes[MAX_3D_TARGETS];		// index depends on virtual targets

	#define RECALC_DOPPLER		0
	#define SET_VELOCITY		1
	#define SET_POSITION		2
	#define RECALC_DISTANCE_ATTN_01	3
	#define RECALC_SPATIAL		4
	#define SET_CONE_ORIENTATION0	5

	#define RECALC_HF		6
	#define RECALC_LF		7
	#define RECALC_ROOM_ONLY	8	// room
	#define RECALC_DIRECT		9
	#define RECALC_REFLECTED	10	// room + room hf + room lf
	#define RECALC_ROOMHFLF		11	// room hf + room lf
	#define RECALC_EAX		12	// all
	#define RECALC_ROOM_HF		13
	#define RECALC_ROOM_LF		14

        // EAX/EAX2/EAX3/I3DL1,2 stuff...
        // eax1
        int eax_environment; // id # (0..26)

        float eax1_volume,   // 0..1
              eax1_damping;  // 0..1
        
        // eax2/3
        int room,room_hf,reflections,reverb; // mB, -10000..0/1000/2000
        int eax_flags;
        float room_rolloff_factor; // 0..10.0
        float decay_hf_ratio, // 0.1 .. 2.0
              reflections_delay, // 0.0 .. 0.3 (sec)
              reverb_delay, // 0 .. 0.1 (sec)
              decay_time, // 0.1 .. 20.0 (sec)
              decay_hf_time,
              decay_lf_time; 
        float env_size, // 1.0 .. 100.0 meters
              env_diffusion, // 0 .. 1.0
              air_absorption_hf; // -100 .. 0.0 mB/m (def: -5.0)

        // eax3
        int room_lf; // mB, -10000..0
        float decay_lf_ratio, // 0.1 .. 2.0
              echo_time,	// 0.075 .. 0.25 (sec)
              echo_depth,	// 0... 1.0
              modulation_time,	// 0.04 .. 4.0 (sec)
              modulation_depth, // 0... 1.0
              hf_reference,	// 1000 .. 20000
              lf_reference;	// 20...1000

        kVector reflections_pan,reverb_pan;
        
	kListener();
	int reset();
	inline void set_all(KSDS3D_LISTENER_ALL &all);
	inline void set_distance_factor(float f);
	inline void set_velocity(DS3DVECTOR &vel);
	inline void set_position(DS3DVECTOR &pos);
	inline void set_doppler_factor(float f);
	inline void set_rolloff_factor(float f);
	inline void set_batch(int b);
	inline void set_orientation(KSDS3D_LISTENER_ORIENTATION &ori);

        inline int get_batch() { return batch; };
        inline void get_all(KSDS3D_LISTENER_ALL &all)
        {
        	copy_vector(all.Position,position);
        	copy_vector(all.Velocity,velocity);
        	copy_vector(all.OrientTop,top);
        	copy_vector(all.OrientFront,front);

        	all.DopplerFactor=doppler_factor;
        	all.DistanceFactor=distance_factor;
        	all.RolloffFactor=rolloff_factor;
        };
        inline float get_distance_factor() { return distance_factor; };
        inline float get_doppler_factor() { return doppler_factor; };
        inline float get_rolloff_factor() { return rolloff_factor; };
	inline void get_velocity(DS3DVECTOR &pos) { copy_vector(pos,velocity); };
	inline void get_position(DS3DVECTOR &pos) { copy_vector(pos,position); };
	inline void get_orientation(KSDS3D_LISTENER_ORIENTATION &ori)
	{
		copy_vector(ori.Top,top);
		copy_vector(ori.Front,front);
	};

	void for_each(int mode,int what);

	void add(kSource *,void *instance_);
	void remove(kSource *);

	int reset_eax(int environment);
	int set_env_size(float sz);
	int recalc_decay();

	int update_reverb(int what);
	#define UPDATE_REVERB_DIFFUSION		0x1
	#define UPDATE_REVERB_DECAY		0x2
	#define UPDATE_REVERB_REFL		0x4
	#define UPDATE_REVERB_REV		0x8
	#define UPDATE_REVERB_ECHO		0x10
	#define UPDATE_REVERB_MOD		0x20
	#define UPDATE_REVERB_REFERENCE		0x40
	#define UPDATE_REVERB_VOL		0x80	// EAX 1.0 'Volume'
	#define UPDATE_REVERB_DAMPING		0x100	// EAX 1.0 'Damping'
	#define UPDATE_REVERB_DENSITY		0x200	// i3dlv2

	#define UPDATE_REVERB_ALL		0x3ff
};

// sound source
class kSource
{
public:
	kListener *listener;
	kSource *s_next;
	int in_chain;
	int update;
	void *instance;

	#define UPDATE_VOL		1
	#define UPDATE_PITCH		2
	#define UPDATE_FXBUS		4
	#define UPDATE_AMOUNT		8
	#define UPDATE_DIRECT_HF	0x10
	#define UPDATE_DIRECT_LF	0x20
	#define UPDATE_ROOM_HF		0x40
	#define UPDATE_ROOM_LF		0x80
	#define UPDATE_ROOM		0x100

	#define UPDATE_EAX		(UPDATE_DIRECT_HF|UPDATE_DIRECT_LF|UPDATE_ROOM_HF| \
				         UPDATE_ROOM_LF|UPDATE_ROOM)

	#define UPDATE_ALL	(UPDATE_VOL|UPDATE_PITCH|UPDATE_FXBUS|UPDATE_AMOUNT|UPDATE_EAX)

        float max_distance, min_distance; // in distance units

        // FIXME: change to radians!
        // degree * (M_PI / 180.0f);
	int inside_angle;		// in degrees, 0..360
	int outside_angle;		// in degrees, 0..360
        int angle;			// degrees

	int outside_volume_ori;		// in 1/100 dB
	int outside_volume;		// in KX_VOL units
	// KX_VOL units:    max: 0          0   dB
	//		    min: FF9C0000h  -100 dB	* 0x10000
	//		    step: 28fh	    0.01 dB	* 0x10000

	// r_ is listener-related; ori_ is nonparsed (for mode changes)
	kVector r_orientation;		// magnitude should be 1.0 (by DSound)
        kVector r_position;		// in distance units
        kVector n_position;		// normalyzed position (for Doppler)
        kVector r_velocity;		// in (ditance unit/sec)
        kVector ori_orientation,ori_position,ori_velocity;

        int mode;			// normal/head-related/none

        float distance;			// in distance units
        int distance_attn;		// in KX_VOL units

        int cone_attn;			// in KX_VOL units
        int cone_attn_hf;		// in KX_VOL units

// not needed (assume always 1.0f)
//        float orientation_magnitude;	// the length of the 'orientation' vector -- [precalc]
//
	int bad_orientation;

        float position_magnitude;	// the same as 'distance', except, is 'min_dist'-unrelated
        int bad_angle;			// a flag set when angle is incorrect
        int bad_cone;			// =1 if inside_angle>=360

        float doppler;

        float d_vl,d_vs;		// listener/source velocities (projected)

        // spatialization stuff
        int azimuth,elevation;
        kVector p_position;		// related to {front,top,right}

        int send[2];
        int send_amount[2];

        // EAX/EAX2/EAX3/I3DL1,2 stuff...
        float eax1_mix;

        int direct,direct_hf,room,room_hf,obstruction,occlusion,exclusion,outside_volume_hf;
        	// mB, -10000..0/1000

        int eax_flags;

        float room_rolloff_factor,
        	obstruction_lf_ratio,
        	occlusion_lf_ratio,
        	occlusion_room_ratio,
        	occlusion_direct_ratio,
        	exclusion_lf_ratio,
                eax_doppler_factor,        	
                eax_rolloff_factor,
        	air_absorption_factor;

        int total_direct_hf,total_direct_lf,total_room,total_room_hf,total_room_lf;

        kSource();
	int reset(kListener *l,void *instance_);
	inline void set_all(KSDS3D_BUFFER_ALL &all);
	inline void set_cone(DS3DVECTOR &orient,int inside,int outside,int outside_vol);
	inline void set_cone_orientation(DS3DVECTOR &orient,int force_attn=0);
	inline void set_cone_angles(int inside,int outside);
	inline void set_mode(int mode_);
	inline void set_cone_outside_volume(int vol);
	inline void set_min_distance(float d);
	inline void set_max_distance(float d);
	inline void set_position(DS3DVECTOR &pos,int no_cone_recalc=0);
	inline void set_velocity(DS3DVECTOR &vel);
        inline void recalc_distance_attn(int recalc_distn,int force_attn);
        inline void recalc_cone_attn(int recalc_angle,int force_attn);
        inline void recalc_doppler(int recalc_magnitude=0);
        inline void recalc_spatial();
        inline void recalc_cone_hf();

        // get_...() functions
        inline int get_mode() { return mode; };
	inline void get_cone_orientation(DS3DVECTOR &orient) { copy_vector(orient,ori_orientation); };
	inline void get_velocity(DS3DVECTOR &pos) { copy_vector(pos,ori_velocity); };
	inline void get_position(DS3DVECTOR &pos) { copy_vector(pos,ori_position); };
        inline void get_cone_angles(int &inside,int &outside) { inside=inside_angle; outside=outside_angle; };
        inline int get_cone_outside_volume() { return outside_volume_ori; };
        inline float get_min_distance() { return min_distance; };
        inline float get_max_distance() { return max_distance; };
	inline void get_all(KSDS3D_BUFFER_ALL *all)
	{
		all->Mode=mode;
		all->MinDistance=min_distance;
		all->MaxDistance=max_distance;
		all->InsideConeAngle=inside_angle;
		all->OutsideConeAngle=outside_angle;
		all->ConeOutsideVolume=outside_volume_ori;

		copy_vector(all->Position,ori_position);
		copy_vector(all->Velocity,ori_velocity);
		copy_vector(all->ConeOrientation,ori_orientation);
	};

	int reset_eax();

        // eax recalcs:
        int recalc_direct_hf();
        int recalc_direct_lf();
        int recalc_room();
        int recalc_room_hf();
        int recalc_room_lf();
};
#endif	// KX3D_H_
