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

#include "stdafx.h"

uint_8 bitstream_get_byte(ac3_state *state)
{
	if(state->chunk_start == state->chunk_end)
		state->fill_buffer(&state->chunk_start,&state->chunk_end,state,0);

	return (*state->chunk_start++);
}

uint_8 *bitstream_get_buffer_start(ac3_state *state)
{
	return state->buffer_start;
}

void bitstream_buffer_frame(uint_32 frame_size,ac3_state *state)
{
  uintptr_t bytes_read=0;
  uintptr_t num_bytes=0;

  do
  {
    if(state->chunk_start > state->chunk_end)
    {
    	debug(1,"!!! chunk_start>end\n");
    }
    if(state->chunk_start >= state->chunk_end)
      state->fill_buffer(&state->chunk_start,&state->chunk_end,state,frame_size);

    num_bytes = (uintptr_t)state->chunk_end - (uintptr_t)state->chunk_start;

    if(bytes_read + num_bytes > frame_size)
      num_bytes = frame_size - bytes_read;

    memcpy(&state->buffer[bytes_read], state->chunk_start, num_bytes);

    bytes_read += num_bytes;
    state->chunk_start += num_bytes;
  }
  while (bytes_read < frame_size);

  state->buffer_start = state->buffer;
  state->buffer_end   = (uint_8 *)((uintptr_t)state->buffer + (uintptr_t)frame_size);

  state->bits_left = 0;
}

void bitstream_fill_current(ac3_state *state)
{
	state->current_word = *((uint_32*)state->buffer_start)++;
	state->current_word = swab32(state->current_word);
}

//
// The fast paths for _get is in the
// bitstream.h header file so it can be inlined.
//
// The "bottom half" of this routine is suffixed _bh
//
// -ah
//

uint_32 bitstream_get_bh(uint_32 num_bits,ac3_state *state)
{
	uint_32 result;

	num_bits -= state->bits_left;
	result = (state->current_word << (32 - state->bits_left)) >> (32 - state->bits_left);

	bitstream_fill_current(state);

	if(num_bits != 0)
		result = (result << num_bits) | (state->current_word >> (32 - num_bits));
	
	state->bits_left = 32 - num_bits;

	return result;
}

void bitstream_init(ac3_state *state)
{
	// Setup the buffer fill callback 
	state->buffer_start=state->buffer;
	state->buffer_end=state->buffer;
}
