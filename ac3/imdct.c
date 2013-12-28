// KX Driver
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


// source code license / origin is unknown; probably, public domain

#pragma function(cos)
#pragma function(sin)

#include "stdafx.h"

#ifndef M_PI
 #define M_PI 3.14159265359
#endif

void imdct_do_256(float data[],float delay[]);
void imdct_do_512(float data[],float delay[]);

typedef struct complex_s
{
	float real;
	float imag;
} complex_t;


#define N 512


/* 128 point bit-reverse LUT */
static uint_8 bit_reverse_512[] = {
	0x00, 0x40, 0x20, 0x60, 0x10, 0x50, 0x30, 0x70, 
	0x08, 0x48, 0x28, 0x68, 0x18, 0x58, 0x38, 0x78, 
	0x04, 0x44, 0x24, 0x64, 0x14, 0x54, 0x34, 0x74, 
	0x0c, 0x4c, 0x2c, 0x6c, 0x1c, 0x5c, 0x3c, 0x7c, 
	0x02, 0x42, 0x22, 0x62, 0x12, 0x52, 0x32, 0x72, 
	0x0a, 0x4a, 0x2a, 0x6a, 0x1a, 0x5a, 0x3a, 0x7a, 
	0x06, 0x46, 0x26, 0x66, 0x16, 0x56, 0x36, 0x76, 
	0x0e, 0x4e, 0x2e, 0x6e, 0x1e, 0x5e, 0x3e, 0x7e, 
	0x01, 0x41, 0x21, 0x61, 0x11, 0x51, 0x31, 0x71, 
	0x09, 0x49, 0x29, 0x69, 0x19, 0x59, 0x39, 0x79, 
	0x05, 0x45, 0x25, 0x65, 0x15, 0x55, 0x35, 0x75, 
	0x0d, 0x4d, 0x2d, 0x6d, 0x1d, 0x5d, 0x3d, 0x7d, 
	0x03, 0x43, 0x23, 0x63, 0x13, 0x53, 0x33, 0x73, 
	0x0b, 0x4b, 0x2b, 0x6b, 0x1b, 0x5b, 0x3b, 0x7b, 
	0x07, 0x47, 0x27, 0x67, 0x17, 0x57, 0x37, 0x77, 
	0x0f, 0x4f, 0x2f, 0x6f, 0x1f, 0x5f, 0x3f, 0x7f};

static uint_8 bit_reverse_256[] = {
	0x00, 0x20, 0x10, 0x30, 0x08, 0x28, 0x18, 0x38, 
	0x04, 0x24, 0x14, 0x34, 0x0c, 0x2c, 0x1c, 0x3c, 
	0x02, 0x22, 0x12, 0x32, 0x0a, 0x2a, 0x1a, 0x3a, 
	0x06, 0x26, 0x16, 0x36, 0x0e, 0x2e, 0x1e, 0x3e, 
	0x01, 0x21, 0x11, 0x31, 0x09, 0x29, 0x19, 0x39, 
	0x05, 0x25, 0x15, 0x35, 0x0d, 0x2d, 0x1d, 0x3d, 
	0x03, 0x23, 0x13, 0x33, 0x0b, 0x2b, 0x1b, 0x3b, 
	0x07, 0x27, 0x17, 0x37, 0x0f, 0x2f, 0x1f, 0x3f};

static complex_t buf[128];

/* Twiddle factor LUT */
static complex_t *w[7];
static complex_t w_1[1];
static complex_t w_2[2];
static complex_t w_4[4];
static complex_t w_8[8];
static complex_t w_16[16];
static complex_t w_32[32];
static complex_t w_64[64];

/* Twiddle factors for IMDCT */
static float xcos1[128];
static float xsin1[128];
static float xcos2[64];
static float xsin2[64];

/* Delay buffer for time domain interleaving */
static float delay[6][256];

/* Windowing function for Modified DCT - Thank you acroread */
static float window[] = {
	0.00014f, 0.00024f, 0.00037f, 0.00051f, 0.00067f, 0.00086f, 0.00107f, 0.00130f,
	0.00157f, 0.00187f, 0.00220f, 0.00256f, 0.00297f, 0.00341f, 0.00390f, 0.00443f,
	0.00501f, 0.00564f, 0.00632f, 0.00706f, 0.00785f, 0.00871f, 0.00962f, 0.01061f,
	0.01166f, 0.01279f, 0.01399f, 0.01526f, 0.01662f, 0.01806f, 0.01959f, 0.02121f,
	0.02292f, 0.02472f, 0.02662f, 0.02863f, 0.03073f, 0.03294f, 0.03527f, 0.03770f,
	0.04025f, 0.04292f, 0.04571f, 0.04862f, 0.05165f, 0.05481f, 0.05810f, 0.06153f,
	0.06508f, 0.06878f, 0.07261f, 0.07658f, 0.08069f, 0.08495f, 0.08935f, 0.09389f,
	0.09859f, 0.10343f, 0.10842f, 0.11356f, 0.11885f, 0.12429f, 0.12988f, 0.13563f,
	0.14152f, 0.14757f, 0.15376f, 0.16011f, 0.16661f, 0.17325f, 0.18005f, 0.18699f,
	0.19407f, 0.20130f, 0.20867f, 0.21618f, 0.22382f, 0.23161f, 0.23952f, 0.24757f,
	0.25574f, 0.26404f, 0.27246f, 0.28100f, 0.28965f, 0.29841f, 0.30729f, 0.31626f,
	0.32533f, 0.33450f, 0.34376f, 0.35311f, 0.36253f, 0.37204f, 0.38161f, 0.39126f,
	0.40096f, 0.41072f, 0.42054f, 0.43040f, 0.44030f, 0.45023f, 0.46020f, 0.47019f,
	0.48020f, 0.49022f, 0.50025f, 0.51028f, 0.52031f, 0.53033f, 0.54033f, 0.55031f,
	0.56026f, 0.57019f, 0.58007f, 0.58991f, 0.59970f, 0.60944f, 0.61912f, 0.62873f,
	0.63827f, 0.64774f, 0.65713f, 0.66643f, 0.67564f, 0.68476f, 0.69377f, 0.70269f,
	0.71150f, 0.72019f, 0.72877f, 0.73723f, 0.74557f, 0.75378f, 0.76186f, 0.76981f,
	0.77762f, 0.78530f, 0.79283f, 0.80022f, 0.80747f, 0.81457f, 0.82151f, 0.82831f,
	0.83496f, 0.84145f, 0.84779f, 0.85398f, 0.86001f, 0.86588f, 0.87160f, 0.87716f,
	0.88257f, 0.88782f, 0.89291f, 0.89785f, 0.90264f, 0.90728f, 0.91176f, 0.91610f,
	0.92028f, 0.92432f, 0.92822f, 0.93197f, 0.93558f, 0.93906f, 0.94240f, 0.94560f,
	0.94867f, 0.95162f, 0.95444f, 0.95713f, 0.95971f, 0.96217f, 0.96451f, 0.96674f,
	0.96887f, 0.97089f, 0.97281f, 0.97463f, 0.97635f, 0.97799f, 0.97953f, 0.98099f,
	0.98236f, 0.98366f, 0.98488f, 0.98602f, 0.98710f, 0.98811f, 0.98905f, 0.98994f,
	0.99076f, 0.99153f, 0.99225f, 0.99291f, 0.99353f, 0.99411f, 0.99464f, 0.99513f,
	0.99558f, 0.99600f, 0.99639f, 0.99674f, 0.99706f, 0.99736f, 0.99763f, 0.99788f,
	0.99811f, 0.99831f, 0.99850f, 0.99867f, 0.99882f, 0.99895f, 0.99908f, 0.99919f,
	0.99929f, 0.99938f, 0.99946f, 0.99953f, 0.99959f, 0.99965f, 0.99969f, 0.99974f,
	0.99978f, 0.99981f, 0.99984f, 0.99986f, 0.99988f, 0.99990f, 0.99992f, 0.99993f,
	0.99994f, 0.99995f, 0.99996f, 0.99997f, 0.99998f, 0.99998f, 0.99998f, 0.99999f,
	0.99999f, 0.99999f, 0.99999f, 1.00000f, 1.00000f, 1.00000f, 1.00000f, 1.00000f,
	1.00000f, 1.00000f, 1.00000f, 1.00000f, 1.00000f, 1.00000f, 1.00000f, 1.00000f };


static inline void swap_cmplx(complex_t *a, complex_t *b)
{
	complex_t tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}



static inline complex_t cmplx_mult(complex_t a, complex_t b)
{
	complex_t ret;

	ret.real = a.real * b.real - a.imag * b.imag;
	ret.imag = a.real * b.imag + a.imag * b.real;

	return ret;
}

void imdct_init(void)
{
	int i,k;
	complex_t angle_step;
	complex_t current_angle;

	/* Twiddle factors to turn IFFT into IMDCT */
	for( i=0; i < 128; i++)
	{
		xcos1[i] = -cosf(2.0f * (float)M_PI * ((float)(8*i+1)/(float)(8*N))) ; 
		xsin1[i] = -sinf(2.0f * (float)M_PI * ((float)(8*i+1)/(float)(8*N))) ;
	}
	
	/* More twiddle factors to turn IFFT into IMDCT */
	for( i=0; i < 64; i++)
	{
		xcos2[i] = -cosf(2.0f * (float)M_PI * (float)(8*i+1)/(float)(4*N)) ; 
		xsin2[i] = -sinf(2.0f * (float)M_PI * (float)(8*i+1)/(float)(4*N)) ;
	}

	/* Canonical twiddle factors for FFT */
	w[0] = w_1;
	w[1] = w_2;
	w[2] = w_4;
	w[3] = w_8;
	w[4] = w_16;
	w[5] = w_32;
	w[6] = w_64;

	for( i = 0; i < 7; i++)
	{
		angle_step.real = cosf(-2.0f * (float)M_PI / (float)(1 << (i+1)));
		angle_step.imag = sinf(-2.0f * (float)M_PI / (float)(1 << (i+1)));

		current_angle.real = 1.0f;
		current_angle.imag = 0.0f;

		for (k = 0; k < 1 << i; k++)
		{
			w[i][k] = current_angle;
			current_angle = cmplx_mult(current_angle,angle_step);
		}
	}
}

void
imdct_do_512(float data[],float delay[])
{
	int i,k;
	int p,q;
	int m;
	int two_m;
	int two_m_plus_one;

	float tmp_a_i;
	float tmp_a_r;
	float tmp_b_i;
	float tmp_b_r;

	float *data_ptr;
	float *delay_ptr;
	float *window_ptr;
	
	//
	// 512 IMDCT with source and dest data in 'data'
	//
	
	// Pre IFFT complex multiply plus IFFT cmplx conjugate 
	for( i=0; i < 128; i++)
	{
		/* z[i] = (X[256-2*i-1] + j * X[2*i]) * (xcos1[i] + j * xsin1[i]) ; */ 
		buf[i].real =         (data[256-2*i-1] * xcos1[i])  -  (data[2*i]       * xsin1[i]);
	  buf[i].imag = -1.0f * ((data[2*i]       * xcos1[i])  +  (data[256-2*i-1] * xsin1[i]));
	}

	//Bit reversed shuffling
	for(i=0; i<128; i++) 
	{ 
		k = bit_reverse_512[i];
		if (k < i)
			swap_cmplx(&buf[i],&buf[k]);
	}

	/* FFT Merge */
	for (m=0; m < 7; m++)
	{
		if(m)
			two_m = (1 << m);
		else
			two_m = 1;

		two_m_plus_one = (1 << (m+1));

		for(k = 0; k < two_m; k++)
		{
			for(i = 0; i < 128; i += two_m_plus_one)
			{
				p = k + i;
				q = p + two_m;
				tmp_a_r = buf[p].real;
				tmp_a_i = buf[p].imag;
				tmp_b_r = buf[q].real * w[m][k].real - buf[q].imag * w[m][k].imag;
				tmp_b_i = buf[q].imag * w[m][k].real + buf[q].real * w[m][k].imag;
				buf[p].real = tmp_a_r + tmp_b_r;
				buf[p].imag =  tmp_a_i + tmp_b_i;
				buf[q].real = tmp_a_r - tmp_b_r;
				buf[q].imag =  tmp_a_i - tmp_b_i;

			}
		}
	}

	/* Post IFFT complex multiply  plus IFFT complex conjugate*/
	for( i=0; i < 128; i++)
	{
		/* y[n] = z[n] * (xcos1[n] + j * xsin1[n]) ; */
		tmp_a_r =        buf[i].real;
		tmp_a_i = -1.0f * buf[i].imag;
		buf[i].real =(tmp_a_r * xcos1[i])  -  (tmp_a_i  * xsin1[i]);
	  buf[i].imag =(tmp_a_r * xsin1[i])  +  (tmp_a_i  * xcos1[i]);
	}
	
	data_ptr = data;
	delay_ptr = delay;
	window_ptr = window;

	/* Window and convert to real valued signal */
	for(i=0; i< 64; i++) 
	{ 
		*data_ptr++   = 2.0f * (-buf[64+i].imag   * *window_ptr++ + *delay_ptr++); 
		*data_ptr++   = 2.0f * ( buf[64-i-1].real * *window_ptr++ + *delay_ptr++); 
	}

	for(i=0; i< 64; i++) 
	{ 
		*data_ptr++  = 2.0f * (-buf[i].real       * *window_ptr++ + *delay_ptr++); 
		*data_ptr++  = 2.0f * ( buf[128-i-1].imag * *window_ptr++ + *delay_ptr++); 
	}
	
	/* The trailing edge of the window goes into the delay line */
	delay_ptr = delay;

	for(i=0; i< 64; i++) 
	{ 
		*delay_ptr++  = -buf[64+i].real   * *--window_ptr; 
		*delay_ptr++  =  buf[64-i-1].imag * *--window_ptr; 
	}

	for(i=0; i<64; i++) 
	{
		*delay_ptr++  =  buf[i].imag       * *--window_ptr; 
		*delay_ptr++  = -buf[128-i-1].real * *--window_ptr; 
	}
}

void
imdct_do_256(float data[],float delay[])
{
	int i,k;
	int p,q;
	int m;
	int two_m;
	int two_m_plus_one;

	float tmp_a_i;
	float tmp_a_r;
	float tmp_b_i;
	float tmp_b_r;

	float *data_ptr;
	float *delay_ptr;
	float *window_ptr;

	complex_t *buf_1, *buf_2;

	buf_1 = &buf[0];
	buf_2 = &buf[64];

	/* Pre IFFT complex multiply plus IFFT cmplx conjugate */
	for(k=0; k<64; k++) 
	{ 
		/* X1[k] = X[2*k]  */
		/* X2[k] = X[2*k+1]     */

		p = 2 * (128-2*k-1);
		q = 2 * (2 * k);

		/* Z1[k] = (X1[128-2*k-1] + j * X1[2*k]) * (xcos2[k] + j * xsin2[k]); */ 
		buf_1[k].real =         data[p] * xcos2[k] - data[q] * xsin2[k];
	  buf_1[k].imag = -1.0f * (data[q] * xcos2[k] + data[p] * xsin2[k]); 
		/* Z2[k] = (X2[128-2*k-1] + j * X2[2*k]) * (xcos2[k] + j * xsin2[k]); */ 
		buf_2[k].real =          data[p + 1] * xcos2[k] - data[q + 1] * xsin2[k];
	  buf_2[k].imag = -1.0f * ( data[q + 1] * xcos2[k] + data[p + 1] * xsin2[k]); 
	}

	//IFFT Bit reversed shuffling
	for(i=0; i<64; i++) 
	{ 
		k = bit_reverse_256[i];
		if (k < i)
		{
			swap_cmplx(&buf_1[i],&buf_1[k]);
			swap_cmplx(&buf_2[i],&buf_2[k]);
		}
	}

	/* FFT Merge */
	for (m=0; m < 6; m++)
	{
		two_m = (1 << m);
		two_m_plus_one = (1 << (m+1));

		//original FIXME
		if(m)
			two_m = (1 << m);
		else
			two_m = 1;

		for(k = 0; k < two_m; k++)
		{
			for(i = 0; i < 64; i += two_m_plus_one)
			{
				p = k + i;
				q = p + two_m;
				//Do block 1
				tmp_a_r = buf_1[p].real;
				tmp_a_i = buf_1[p].imag;
				tmp_b_r = buf_1[q].real * w[m][k].real - buf_1[q].imag * w[m][k].imag;
				tmp_b_i = buf_1[q].imag * w[m][k].real + buf_1[q].real * w[m][k].imag;
				buf_1[p].real = tmp_a_r + tmp_b_r;
				buf_1[p].imag =  tmp_a_i + tmp_b_i;
				buf_1[q].real = tmp_a_r - tmp_b_r;
				buf_1[q].imag =  tmp_a_i - tmp_b_i;

				//Do block 2
				tmp_a_r = buf_2[p].real;
				tmp_a_i = buf_2[p].imag;
				tmp_b_r = buf_2[q].real * w[m][k].real - buf_2[q].imag * w[m][k].imag;
				tmp_b_i = buf_2[q].imag * w[m][k].real + buf_2[q].real * w[m][k].imag;
				buf_2[p].real = tmp_a_r + tmp_b_r;
				buf_2[p].imag =  tmp_a_i + tmp_b_i;
				buf_2[q].real = tmp_a_r - tmp_b_r;
				buf_2[q].imag =  tmp_a_i - tmp_b_i;

			}
		}
	}

	/* Post IFFT complex multiply */
	for( i=0; i < 64; i++)
	{
		/* y1[n] = z1[n] * (xcos2[n] + j * xs in2[n]) ; */ 
		tmp_a_r =  buf_1[i].real;
		tmp_a_i = -buf_1[i].imag;
		buf_1[i].real =(tmp_a_r * xcos2[i])  -  (tmp_a_i  * xsin2[i]);
	  buf_1[i].imag =(tmp_a_r * xsin2[i])  +  (tmp_a_i  * xcos2[i]);
		/* y2[n] = z2[n] * (xcos2[n] + j * xsin2[n]) ; */ 
		tmp_a_r =  buf_2[i].real;
		tmp_a_i = -buf_2[i].imag;
		buf_2[i].real =(tmp_a_r * xcos2[i])  -  (tmp_a_i  * xsin2[i]);
	  buf_2[i].imag =(tmp_a_r * xsin2[i])  +  (tmp_a_i  * xcos2[i]);
	}
	
	data_ptr = data;
	delay_ptr = delay;
	window_ptr = window;

	/* Window and convert to real valued signal */
	for(i=0; i< 64; i++) 
	{ 
		*data_ptr++  = 2.0f * (-buf_1[i].imag      * *window_ptr++ + *delay_ptr++);
		*data_ptr++  = 2.0f * ( buf_1[64-i-1].real * *window_ptr++ + *delay_ptr++);
	}

	for(i=0; i< 64; i++) 
	{
		*data_ptr++  = 2.0f * (-buf_1[i].real      * *window_ptr++ + *delay_ptr++);
		*data_ptr++  = 2.0f * ( buf_1[64-i-1].imag * *window_ptr++ + *delay_ptr++);
	}
	
	delay_ptr = delay;

	for(i=0; i< 64; i++) 
	{
		*delay_ptr++ = -buf_2[i].real      * *--window_ptr;
		*delay_ptr++ =  buf_2[64-i-1].imag * *--window_ptr;
	}

	for(i=0; i< 64; i++) 
	{
		*delay_ptr++ =  buf_2[i].imag      * *--window_ptr;
		*delay_ptr++ = -buf_2[64-i-1].real * *--window_ptr;
	}
}

void 
imdct(bsi_t *bsi,audblk_t *audblk, stream_samples_t samples) {
	int i;

	for(i=0; i<bsi->nfchans;i++)
	{
		if(audblk->blksw[i])
			imdct_do_256(samples[i],delay[i]);
		else
			imdct_do_512(samples[i],delay[i]);
	}
	if (bsi->lfeon)
		imdct_do_512(samples[5],delay[5]);
}
