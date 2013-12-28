// kX Driver / kX Driver Interface / kX Driver Effects Library
// Copyright (c) Eugene Gavrilov, 2001-2008.
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


#ifndef _KX_EPILOG_H
#define _KX_EPILOG_H

// Plugin definition
class iEpilogPlugin : public iKXPlugin
{            
public:
        ~iEpilogPlugin() { close(); };
        virtual int request_microcode();
        virtual int close();
        virtual const char *get_plugin_description(int id);
        virtual iKXDSPWindow *create_dsp_wnd(kDialog *parent_,kWindow *that_,kFile *mf_);
};

declare_effect_source(epilog);

#endif
