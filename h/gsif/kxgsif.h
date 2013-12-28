// kX Driver  / kX Driver Interface
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


#ifndef KX_GSIF_H
#define KX_GSIF_H

class CGSIFInterface : public IGSIFInterface, public CASIO, public CUnknown
{
public:
    IMP_IGSIFInterface;

    // IUnknown
    //
    DECLARE_STD_UNKNOWN();
    DEFINE_STD_CONSTRUCTOR(CGSIFInterface);
    ~CGSIFInterface();

public:
    void init(CAdapterCommon *adapter_);
    CAdapterCommon *adapter;               // driver's extension space
    PFNDRIVERREQUEST fnGSIFRequest;
    PFNDRIVERISR     fnGSIFIsr;
    PVOID            pContext;

    int buffer_size;
    int buffer_position;
    int gsif_latency;
};

#endif
