// kX Mixer / FX Control - MIDI Router
// kX VSTi Interface
// Copyright (c) Eugene Gavrilov and Max Mikhailov, 2002-2005.
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

#include "midiparser.h"
#include "midirouter.h"
#include "notify.h"
#include "mstuff.h"

iKXMidiParser::iKXMidiParser(iKXNotifier *n_)
{
	notifier=n_;
	ParserMap=(struct pluginparam *)malloc(MAX_PARAMS_COUNT*sizeof(struct pluginparam));
	reset();
}

iKXMidiParser::~iKXMidiParser()
{
	if(ParserMap)
	{
	 free(ParserMap);
	 ParserMap=0;
	}
}

void iKXMidiParser::reset()
{
	ActiveParam = NULL;
	EmptyParam = &ParserMap[0];
	for (int i = 0; i < (MAX_PARAMS_COUNT - 1); i++)
	{
	  ParserMap[i].next = &ParserMap[i + 1];
	}
	ParserMap[MAX_PARAMS_COUNT - 1].next = NULL;

        if(midi_window && notifier)
        {
         midi_window->select_device();
        }
}

int iKXMidiParser::event(int pgm_id, iKXPlugin *plg, int event)
{ 
        if((event==IKX_UNLOAD)&&(ActiveParam))
        {
        	struct pluginparam *param=ActiveParam;
        	struct pluginparam *prev=NULL;

        	for(int i=0;i<(MAX_PARAMS_COUNT);i++) // fixme, change this to 'while (parentparam != NULL)' when everything is ok
        	{
        		if(param==NULL)
        		 break;
        		if(param->plugin==plg) // remove this param
        		{
        			struct pluginparam *tmp=param;
        			struct pluginparam *tmp_next=param->next;

                                // remove from active list
        			if(prev)
        			{
        			 prev->next=tmp_next;
        			}
        			else
        			{
        			 ActiveParam=tmp_next;
        			}

                                // add to empty list 
        			tmp->next=EmptyParam;
        			EmptyParam=tmp;

        			param=tmp_next;
        		}
        		else
        		{
        		 prev=param;
        		 param = param->next;
        		}
        	}
	}
	if((event==IKX_LOAD)||(event==IKX_UNLOAD))
	{
	  // debug("-- loading/unloading plugin: [%d] %s\n",plg->pgm_id,plg->name);
          if(midi_window && notifier)
          {
            midi_window->select_device();
          }
        }	
	return 0;
}

struct pluginparam* iKXMidiParser::add_param()
{
	if(EmptyParam == NULL) { debug("!!!iKXMidiParser::add_param(), no empty parameter slots..."); return NULL; }
	struct pluginparam* newparam = EmptyParam;
	EmptyParam = newparam->next;
	newparam->next = ActiveParam;
	ActiveParam = newparam;
	return newparam;
}

void iKXMidiParser::remove_param(struct pluginparam *param)
{
	if (param == NULL) return;
	struct pluginparam* parentparam = ActiveParam;
	if (parentparam == param) 
	{
			ActiveParam = param->next;
			param->next = EmptyParam;
			EmptyParam = param;
			return;
	}
	for (int i = 0; i < (MAX_PARAMS_COUNT); i++) // fixme, change this to 'while (parentparam != NULL)' when everything is ok
	{
		if (parentparam == NULL) return;
		if (parentparam->next == param) 
		{
			parentparam->next = param->next;
			param->next = EmptyParam;
			EmptyParam = param;
			return;
		}
		parentparam = parentparam->next;
	}
	debug("!!!iKXMidiParser::remove_param(): no end of the list...");
}

struct pluginparam* iKXMidiParser::get_param(iKXPlugin *plugin, int ndx)
{
	struct pluginparam* param = ActiveParam;
	for (int i = 0; i < (MAX_PARAMS_COUNT); i++) // fixme, change this to 'while (param != NULL)' when everything is ok
	{
		if (param == NULL) return NULL;
		if ((param->plugin == plugin) & (param->ndx == ndx)) return param;
		param = param->next;
	}
	return NULL; // not found
}

void iKXMidiParser::parse_cc(dword data)
{
	struct pluginparam* param = ActiveParam;
	int is_note_off = 0;
	kxparam_t h, l, v;

	// convert note_off to note_on for better search
	if (getm_event(data) == NOTE_OFF) 
	{
		data += 0x10;
		is_note_off = 1;
	}

	while (param != NULL)
	{
		// if ((data) & ((1 << ((flags) & FLAG_MSG_MASK)) - 1))
		if ((getm_msg_to_compare(data, param->flags)) == param->msgmask)
		{
			switch ((param->flags & FLAG_FUNCTION_MASK) + is_note_off)
			{
			case FLAG_FUNCTION_DEFAULT: // control, pitchbend, velocity etc.
				h = getm_msb_value(data, param->flags);
				l = getm_lsb_value(data, param->flags);
				v = _interp_by14bv(param->min, param->max, h, l);
				break;
			case FLAG_FUNCTION_BUTTON:
			case FLAG_FUNCTION_BUTTON + 1: // if note_off
				v = (is_note_off) ? param->min : param->max;
				break;
			case FLAG_FUNCTION_SWITCH:
				v = (param->flags & FLAG_STATE_MASK) ? param->min : param->max;
				param->flags ^= FLAG_STATE_MASK; // save new state
				break;
			case FLAG_FUNCTION_RAW_DATA:
				v = data;
				break;
			case FLAG_FUNCTION_RAW_DATA+1: // is note_off
				v = data-0x10;
				break;
			default:
				param = param->next;
				continue;
			}
/*#ifdef SUPER_DEBUG
	if (param->plugin == NULL) {OutputDebugString("!!!iKXMidiParser::parse_cc, error - null plugin pointer..."); return;}
	wsprintf(str, "set_param - %s, ndx: %i, value: %i\n",
		param->plugin->name, param->ndx, v);
	OutputDebugString(str);
#endif*/
			param->plugin->set_param(param->ndx, (dword) v);
		}
		param = param->next;
	}
	return;
}

int iKXMidiParser::save_settings(kSettings &cfg)
{
 struct pluginparam *list=ActiveParam;
 int i=1;

 while(list)
 {
 	cfg.write("automation","mask",list->msgmask,i);
 	cfg.write("automation","flag",list->flags,i);
 	cfg.write("automation","ndx",list->ndx,i);
 	cfg.write("automation","min",(unsigned int)list->min,i);
 	cfg.write("automation","max",(unsigned int)list->max,i);
 	cfg.write("automation","plg_id",list->plugin->pgm_id,i);
 	dword guid=0;
 	const char *p=list->plugin->get_plugin_description(IKX_PLUGIN_GUID);

    if(p)
    {
            for(int j=0;j<36;j++)
                guid^=(p[j]<<(j));
    }
 	cfg.write("automation","guid",guid,i);

	list=list->next;
	i++;
 }
 cfg.write("automation","flag",0xffffffff,i);
 return 0;
}

int iKXMidiParser::restore_settings(kSettings &cfg)
{
 reset();

 int i=1;
 while(1)
 {
       dword data;
       if(cfg.read("automation","flag",&data,i)==0)
       {
         if(data==0xffffffff) 
          break; // last one

         dword flg=data;
         iKXPlugin *plg=NULL;
         dword guid=0;

         if(cfg.read("automation","plg_id",&data,i)==0)
         {
          plg=notifier->pm_t->find_plugin_by_id(data);
         }
         else
          { debug("automation: no plg_id\n"); break; }

         if(plg==NULL)
          { debug("automation: invalid plg_id [%d] \n",data); break; }


         const char *pp=plg->get_plugin_description(IKX_PLUGIN_GUID);
         if(pp)
         {
          for(int j=0;j<36;j++)
           guid^=(pp[j]<<(j));
         }
          else { debug("automation: no guid\n"); break; }

         if(cfg.read("automation","guid",&data,i)==0)
         {
          if(data!=guid)
           { debug("automation: guid invalid [%d; %x vs %x]\n",plg->pgm_id,guid,data); break; }
         }
          else { debug("automation: guid not saved\n"); break; }

         struct pluginparam *p=add_param();
         if(p)
         {
           p->flags=flg;
           p->plugin=plg;

           if(cfg.read("automation","min",&data,i)==0)
            p->min=data;
           else
            p->min=0;

           if(cfg.read("automation","max",&data,i)==0)
            p->max=data;
           else
            p->max=0x7fffffff;

           if(cfg.read("automation","mask",&data,i)==0)
            p->msgmask=data;
           else
            p->msgmask=0;

           if(cfg.read("automation","ndx",&data,i)==0)
            p->ndx=data;
           else
            p->ndx=0;

           i++;

         } else { debug("automation: error adding params...\n"); break; }
       } else { break; }
 }

 if(midi_window && notifier)
 {
   midi_window->select_device();
 }

 return 0;
}
