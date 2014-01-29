
// kX DSP Resource Meter demo library
// Copyright (c) Max Mikhailov, 2009

// Permission to use, copy, modify, distribute, and sell this software
// for any purpose is hereby granted without fee, provided that the above
// copyright notice appears in all copies and that both that copyright
// notice and this permission notice appear in supporting documentation.
// The author makes no representations about the suitability of this
// software for any purpose. It is provided "as is" without express or
// implied warranty.

// rm.counter.h
// DSP Resources Counter

#ifndef RM_COUNTER_INCLUDED
#define RM_COUNTER_INCLUDED

#include "includes.h"
#include "helpers.h"

// ............................................................................

namespace rm {

// ............................................................................

struct Counter
{
    enum Index
    {
        Code,
        Regs,
        iTramRegs,
        xTramRegs,
        iTramSize,
        xTramSize,

        IndexCount
    };

    void update()
    {
        k2 = !!api.get_dsp();

        memset(used, 0, sizeof(used));
        dsp_microcode     m;
        dsp_code          code[2048];
        dsp_register_info regs[2048];

        for (int i = 0; i < MAX_PGM_NUMBER; i++)
        {
            if (!api.enum_microcode(i, &m)
                && (m.flag & MICROCODE_TRANSLATED)
                && !api.get_microcode(i, code,
                    m.code_size, regs, m.info_size))
            {
                m.code = code;
                m.info = regs;
                add(m);
            }
        }
    }

    void update(Tooltips& tt)
    {
        static const char* const string[2][IndexCount] =
        {{
            "Program Memory: %.01f%%\r\n(%i / %i instructions)",
            "Data Memory: %.01f%%\r\n(%i / %i registers)",
            "Internal Tank Memory Registers: %.01f%%\r\n(%i / %i registers)",
            "External Tank Memory Registers: %.01f%%\r\n(%i / %i registers)",
            "Internal Tank Memory Size: %.01f%%\r\n(%i / %i samples)",
            "External Tank Memory Size: %.01f%%\r\n(%i / %i samples)",
        },
        {
            "Instr: %.01f%%\r\n(%i / %i instructions)",
            "GPRs: %.01f%%\r\n(%i / %i registers)",
            "iTram: %.01f%%\r\n(%i / %i registers)",
            "xTram: %.01f%%\r\n(%i / %i registers)",
            "iTramSize: %.01f%%\r\n(%i / %i samples)",
            "xTramSize: %.01f%%\r\n(%i / %i samples)",
        }};

        update();

        const int n = 512;
        char text[n];

        for (int i = 0; i < Counter::IndexCount; i++)
        {
            int v = used[i];
            int m = max(i);
            _snprintf(text, n, string[legacyNames][i],
                100. * v / m, v, m);
            text[n - 1] = 0;
            tt.update(i, text);
        }
    }

    double value(int index) const
    {
        return double(used[index]) / max(index);
    }

    int max(int index) const
    {
        static const int max_[2][IndexCount] =
        {
            { 512, 256, 128, 32, 8192, ~0}, // k1
            {1024, 512, 192, 64, 8192, ~0}  // k2
        };

        int value = max_[k2][index];
        if (index == xTramSize)
        {
            api.get_buffers(KX_TANKMEM_BUFFER, &value);
            value >>= 1; // bytes to samples
        }

        return value;
    }

    void useLegacyNames(bool value)
    {
        legacyNames = value;
    }

    void print()
    {
        update();

        #if 0
            trace("Code:      %i\n", used[Code]);
            trace("Regs:      %i\n", used[Regs]);
            trace("iTramSize: %i\n", used[iTramSize]);
            trace("iTramRegs: %i\n", used[iTramRegs]);
            trace("xTramSize: %i\n", used[xTramSize]);
            trace("xTramRegs: %i\n", used[xTramRegs]);
        #else
            trace("Code:      %.02f\n", value(Code));
            trace("Regs:      %.02f\n", value(Regs));
            trace("iTramSize: %.02f\n", value(iTramSize));
            trace("iTramRegs: %.02f\n", value(iTramRegs));
            trace("xTramSize: %.02f\n", value(xTramSize));
            trace("xTramRegs: %.02f\n", value(xTramRegs));
        #endif
    }

    Counter(iKX* api) :
        api(*api),
        legacyNames(0),
        k2()
    {memset(used, 0, sizeof(used));}

private:
    iKX& api;
    int  used[IndexCount];
    bool legacyNames;
    bool k2;

    Counter(const Counter&);
    Counter& operator = (const Counter&);

    void add(const dsp_microcode& m)
    {
        used[Code]      += m.code_size / sizeof(dsp_code);
        used[iTramSize] += m.itramsize;
        used[xTramSize] += m.xtramsize;

        int n = m.info_size / sizeof(dsp_register_info);
        for (int i = 0; i < n; i++)
        {
            switch (m.info[i].type & GPR_MASK)
            {
                case GPR_TEMP:
                case GPR_CONST:
                case GPR_STATIC:
                case GPR_OUTPUT:
                case GPR_CONTROL: ++used[Regs];      break;
                case GPR_ITRAM:   ++used[iTramRegs]; break;
                case GPR_XTRAM:   ++used[xTramRegs]; break;
            }
        }
    }
};

// ............................................................................

} // ~ namespace rm

// ............................................................................

#endif // ~ RM_COUNTER_INCLUDED
