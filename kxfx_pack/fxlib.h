// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2001-2005.
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

#ifndef _KX_FXLIB_H
#define _KX_FXLIB_H

// declare 'da_*.cpp' variables
#define declare_effect_source(effect_name)			\
	extern char * effect_name##_name;			\
	extern char * effect_name##_copyright;			\
	extern char * effect_name##_created;			\
	extern char * effect_name##_engine;			\
	extern char * effect_name##_guid;			\
	extern char * effect_name##_comment;			\
        extern int effect_name##_itramsize,effect_name##_xtramsize; \
        extern dsp_register_info effect_name##_info[];		\
        extern dsp_code effect_name##_code[]

// this is used for 'general' plugin declaration
#define declare_plugin_members(effect_name,class_name)		\
int class_name##Plugin::request_microcode()			\
{								\
	info = effect_name##_info;				\
	code = effect_name##_code;				\
	info_size = sizeof(effect_name##_info);			\
        if(effect_name##_code[0].op!=0xff)	/* avoids C++ declaration bug */			\
           code_size=sizeof(effect_name##_code);                \
        else							\
           code_size=0;						\
	itramsize = effect_name##_itramsize;			\
	xtramsize = effect_name##_xtramsize;			\
	strncpy(name, effect_name##_name, sizeof(name));	\
	return 0;						\
}								\
const char *class_name##Plugin::get_plugin_description(int id)	\
{								\
	switch (id)						\
	{                                                       \
	case IKX_PLUGIN_NAME:                                   \
		return effect_name##_name;                      \
	case IKX_PLUGIN_COPYRIGHT:                              \
		return effect_name##_copyright;                 \
	case IKX_PLUGIN_ENGINE:                                 \
		return effect_name##_engine;                    \
	case IKX_PLUGIN_CREATED:                                \
		return effect_name##_created;                   \
	case IKX_PLUGIN_COMMENT:                                \
		return effect_name##_comment;                   \
	case IKX_PLUGIN_GUID:                                   \
		return effect_name##_guid;                      \
	default:                                                \
		return NULL;                                    \
	}                                                       \
}

#define declare_plugin_class(effect_name,class_name)		\
class class_name##Plugin : public iKXPlugin 			\
{								\
public:								\
 virtual const char *get_plugin_description(int id);			\
 virtual int request_microcode();				\
};								\
declare_plugin_members(effect_name,class_name)

#define publish_microcode(plgname)			\
 info=plgname##_info;                              	\
 code=plgname##_code;                              	\
 info_size=sizeof(plgname##_info);                 	\
 if(plgname##_code[0].op!=0xff)	/* avoids C++ declaration bug */			\
    code_size=sizeof(plgname##_code);                 	\
 else							\
    code_size=0;					\
 itramsize=plgname##_itramsize;                    	\
 xtramsize=plgname##_xtramsize;                    	\
 strncpy(name,plgname##_name,sizeof(name))

#define plugin_description(name)		\
	switch (id)				\
	{                                       \
	case IKX_PLUGIN_NAME:                   \
		return name##_name;             \
	case IKX_PLUGIN_COPYRIGHT:              \
		return name##_copyright;        \
	case IKX_PLUGIN_ENGINE:                 \
		return name##_engine;           \
	case IKX_PLUGIN_CREATED:                \
		return name##_created;          \
	case IKX_PLUGIN_COMMENT:                \
		return name##_comment;          \
	case IKX_PLUGIN_GUID:                   \
		return name##_guid;             \
	default:                                \
		return NULL;                    \
	}

#define instantiate_plugin(NAME,GUI) \
	if(strcmp(plugin_list[param].name,NAME##_name)==0) \
	{                                                 \
	        GUI *p;                                   \
	        p=new GUI();                              \
                *ret=(uintptr_t)p;                            \
                return 0;                                 \
        }

#define plugin_name(a) a##_name

// Plugin list
typedef struct
{
 char *name;
 char *guid;
}kxplugin_list_t;

#endif
