// KS library
// Copyright (c) Eugene Gavrilov, 2007-2014

#pragma once
#ifndef __KSCOMMON_H
#define __KSCOMMON_H

// filter types
typedef enum {eUnknown, eAudRen, eAudCap} ETechnology;

typedef struct
{
    KSP_PIN KsPProp;
    KSMULTIPLE_ITEM KsMultiple;
} INTERSECTION;

#include "tlist.h"
#include "util.h"
#include "irptgt.h"
#include "filter.h"
#include "pin.h"
#include "node.h"
#include "enum.h"
#include "audfilter.h"
#include "audpin.h"

#endif
