
// Spectrum Analyzer demo library
// Copyright (c) Max Mikhailov, 2009 

// Permission to use, copy, modify, distribute, and sell this software
// for any purpose is hereby granted without fee, provided that the above
// copyright notice appears in all copies and that both that copyright
// notice and this permission notice appear in supporting documentation.
// The author makes no representations about the suitability of this
// software for any purpose. It is provided "as is" without express or 
// implied warranty.

// sa.options.h
// SA options

#ifndef SA_OPTIONS_INCLUDED
#define SA_OPTIONS_INCLUDED

#include "helpers.h"

// ............................................................................

namespace sa {

// ............................................................................

namespace config
{
    const char* const presetsKey = "SOFTWARE\\kX\\Plugins\\SA.minus\\Presets";
    const char* const colorsKey  = "SOFTWARE\\kX\\Plugins\\SA.minus\\Colours";
};

// ............................................................................
// High-level settings, e.g the settings to be exposed to a user.
// All are editable on the fly.

namespace options {

// ............................................................................

const int version = '1/2';

enum Index
{
    levelTop,        
    levelRange,      
    levelGrid,       
    peakFreeze,      
    peakDecay,       
    holdDecay,       
    holdHoldTime,    
    holdVisible,     
    holdInfinite,
    bandIntersect,
    bandLeakComp,   
    holdBarType,     
    holdBarSize,     
    peakAlphaColors, 
    peakBarColor,    
    holdBarColor,    
    bkgLightColor,   
    bkgDarkColor,    
    gridBorderColor, 
    gridLineColor,   
    labelColor,      

    OptionsCount,
};

enum {ColorsIndex = peakBarColor};

// ............................................................................

struct Descriptor
{
    Descriptor()
    {
        value[levelTop]        = Int(-80, 20, 1, -5,      "dB", "Level display top");
        value[levelRange]      = Int( 10, 80, 1, 40,      "dB", "Level display range");
        value[levelGrid]       = Int(  2, 10, 1,  5,      "dB", "Level grid lines");
        value[peakFreeze]      = Bool(0,             "off, on", "Freeze display");
        value[peakDecay]       = Int(1, 60, 1, 20,      "dB/s", "Peak decay");   
        value[holdDecay]       = Int(1, 30, 1,  3,      "dB/s", "Peak hold decay");         
        value[holdHoldTime]    = Int(0, 20000, 200, 2000, "ms", "Peak hold time");
        value[holdVisible]     = Bool(1,             "off, on", "Peak hold");
        value[holdInfinite]    = Bool(0,             "off, on", "Infinite peak hold");
        value[bandIntersect]   = Bool(0,        "-6 dB, -3 dB", "Band intersection");
        value[bandLeakComp]    = Bool(0,             "off, on", "Band leak compensation");
        value[holdBarType]     = Bool(0,           "bar, line", "Peak hold display type");
        value[holdBarSize]     = Int(1, 4, 1, 2,          "px", "Peak hold display size");
        value[peakAlphaColors] = Bool(1,             "no, yes", "Enable transparency");
                         
        value[peakBarColor]    = Color(0xD040B0FF, "argb");
        value[holdBarColor]    = Color(0xE0A0C0D0, "argb");
        value[bkgLightColor]   = Color(0x00081020,  "rgb");
        value[bkgDarkColor]    = Color(0x00000000,  "rgb");
        value[gridBorderColor] = Color(0xb06080e0, "argb");
        value[gridLineColor]   = Color(0x506080e0, "argb");
        value[labelColor]      = Color(0xFF6080e0, "argb");
    }

    struct Value
    {
        int         min; 
        int         max; 
        int         step;
        int         default_; 
        const char* unit;
        const char* info;
    };

    const Value& operator[] (int i) const {return value[i];}

private:
    Value value[OptionsCount];

private:

    static Value Int(int min, int max, int step, 
        int default_, const char* unit, const char* info)
    {
        Value  aux = {min, max, step, default_, unit, info};
        return aux;
    }

    static Value Bool(int default_, const char* unit, const char* info)
    {
        Value  aux = {0, 1, 1, default_, unit, info};
        return aux;
    }

    static Value Color(int default_, const char* unit)
    {
        Value  aux = {0x80000000, 0x7FFFFFFF, 0x202020, default_, unit, ""};
        return aux;
    }

} const descriptor;

// ............................................................................

struct Handler
{
    virtual void optionsChanged() = 0;

protected:
    virtual ~Handler() {}
};

// ............................................................................

using namespace config;

struct Type 
{
    explicit Type(int* valueArray) 
        : handler_(0), value(valueArray) {}

    void defaults(bool factory = false)
    {
        Settings settings(Settings(presetsKey), 
            "Default" + factory * 7);
        for (int i = 0; i < OptionsCount; i++)
            value[i] = settings.get(name(i), default_(i));
        if (handler_)
            handler_->optionsChanged();
    }

    int infoString(char* string, int size) const
    {
        int n = 0;
        for (int i = 0; i < ColorsIndex; i++)
            n += infoString(i, string + n, size - n);
        return n;
    }

    int valueString(char* string, int size) const
    {
        int n = 0;
        for (int i = 0; i < ColorsIndex; i++)
            n += valueString(i, string + n, size - n);
        return n;
    }

    void operator () (int i, int v, bool notify = true) 
    {
        v = min(descriptor[i].max, 
            max(descriptor[i].min, v));
        if (value[i] != v)
        {
            value[i] = v;
            if (notify && handler_)
                handler_->optionsChanged();
        }
    }

    int operator () (int i) const {return value[i];}
    const char* name(int i) const {return name_[i];}
    int default_(int i) const {return descriptor[i].default_;}
    int step(Index i)   const {return descriptor[i].step;}

    void cycle(Index i) 
    {
        int v = value[i] + step(i);
        operator()(i, v > descriptor[i].max
            ? descriptor[i].min : v);
    }

    void inc(Index i)        {operator()(i,  value[i] + step(i));}
    void dec(Index i)        {operator()(i,  value[i] - step(i));}
    void toggle(Index i)     {operator()(i, !value[i]);}
    void handler(Handler* h) {handler_ = h;}

private:
    Handler* handler_;
    int*     value;
    EnumNames <Index, OptionsCount> name_;

private:

    int infoString(int i, char* string, int size) const
    {
        return _snprintf(string, size, 
            "%s:\n", descriptor[i].info);
    }

    int valueString(int i, char* string, int size) const
    {
        if (!strstr(descriptor[i].unit, ", "))
            return _snprintf(string, size, "%i %s\n", 
                value[i], descriptor[i].unit);

        const char* u = descriptor[i].unit;
        int n = (value[i] - descriptor[i].min)
            / descriptor[i].step;
        while (--n >= 0)
            u = strstr(u, ", ") + 2;
        n = int(strstr(u, ", ") - u);
        return _snprintf(string, size, "%.*s\n", abs(n), u);
    } 
};

inline void defaults(int* valueArray)
{
    Type(valueArray).defaults();
}

// ............................................................................

} // ~ namespace options
} // ~ namespace sa

// ............................................................................

#endif // ~ SA_OPTIONS_INCLUDED
