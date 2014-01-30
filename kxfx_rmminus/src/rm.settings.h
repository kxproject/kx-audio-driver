
// kX DSP Resource Meter demo library
// Copyright (c) Max Mikhailov, 2009

// Permission to use, copy, modify, distribute, and sell this software
// for any purpose is hereby granted without fee, provided that the above
// copyright notice appears in all copies and that both that copyright
// notice and this permission notice appear in supporting documentation.
// The author makes no representations about the suitability of this
// software for any purpose. It is provided "as is" without express or
// implied warranty.

// rm.settings.cpp
// RM settings

#ifndef RM_SETTINGS_INCLUDED
#define RM_SETTINGS_INCLUDED

// ............................................................................

namespace rm       {
namespace settings {

// ............................................................................

enum Index
{
    codeColorHi,
    codeColorMid,
    codeColorLow,

    regsColorHi,
    regsColorMid,
    regsColorLow,

    iTramRegsColorHi,
    iTramRegsColorMid,
    iTramRegsColorLow,

    xTramRegsColorHi,
    xTramRegsColorMid,
    xTramRegsColorLow,

    iTramSizeColorHi,
    iTramSizeColorMid,
    iTramSizeColorLow,

    xTramSizeColorHi,
    xTramSizeColorMid,
    xTramSizeColorLow,

    boxWidth,
    boxHeight,

    bkgColor,
    borderColor,
    borderRound,
    bevelDepth,
    emptyOpacity,
    labelColor,
    labelSize,

    ancientNames,

    Count
};

// ............................................................................

struct Descriptor
{
    Index       index;
    int         min;
    int         max;
    int         default_;
};

const Descriptor descriptor[] =
{
    #define _           0x80000000, 0x7FFFFFFF,
    #define bool(v)     0, 1, v

    {codeColorHi,       _ 0xCCEE5533},
    {codeColorMid,      _ 0xCCEEEE33},
    {codeColorLow,      _ 0xCC33EE33},

    {regsColorHi,       _ 0xCCEE5533},
    {regsColorMid,      _ 0xCCEEEE33},
    {regsColorLow,      _ 0xCC33EE33},

    {iTramRegsColorHi,  _ 0xCCEE5533},
    {iTramRegsColorMid, _ 0xCCEEEE33},
    {iTramRegsColorLow, _ 0xCC33EE33},

    {xTramRegsColorHi,  _ 0xCCEE5533},
    {xTramRegsColorMid, _ 0xCCEEEE33},
    {xTramRegsColorLow, _ 0xCC33EE33},

    {iTramSizeColorHi,  _ 0xCCEE5533},
    {iTramSizeColorMid, _ 0xCCEEEE33},
    {iTramSizeColorLow, _ 0xCC33EE33},

    {xTramSizeColorHi,  _ 0xCCEE5533},
    {xTramSizeColorMid, _ 0xCCEEEE33},
    {xTramSizeColorLow, _ 0xCC33EE33},

    {boxWidth,            64, 2400, 118},
    {boxHeight,           24, 2400, 88},

    {bkgColor,          _ 0x72000000},
    {borderColor,       _ 0x99ffffff},
    {borderRound,         0, 48, 8},
    {bevelDepth,          0, 255, 102},
    {emptyOpacity,        0, 255, 20},
    {labelColor,        _ 0xffffffff},
    {labelSize,           7, 48, 11},
    {ancientNames,        bool(0)}

    #undef bool
    #undef _
};

// ............................................................................

struct Type
{
    explicit Type(const char* settingsKey)
    {
        Settings key(settingsKey);
        const EnumNames <Index, Count> name;
        for (int i = 0; i < Count; i++)
        {
            const Descriptor& d = descriptor[i];
            int& v = value[i];
            v = key.get(name[i], d.default_);
            v = max(v, d.min);
            v = min(v, d.max);

            // save value back for easier customization
            key.set(name[i], v);
        }
    }

    int operator () (int i) const {return value[i];}

private:
    int value[Count];
};

// ............................................................................

} // ~ namespace settings

// ............................................................................

namespace config {

// ............................................................................

using namespace settings;

const char    settingsKey[] = "Software\\kX\\Plugins\\RM.minus";
const wchar_t labelFont[]   = L"Tahoma";

const int meterPadMin       = 3;
const int meterPadRatio     = 3; // meter width/padding ratio

// ............................................................................

} // ~ namespace config
} // ~ namespace rm

// ............................................................................

#endif // ~ RM_SETTINGS_INCLUDED
