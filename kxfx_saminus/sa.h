
// Spectrum Analyzer demo library
// Copyright (c) Max Mikhailov, 2009 

// Permission to use, copy, modify, distribute, and sell this software
// for any purpose is hereby granted without fee, provided that the above
// copyright notice appears in all copies and that both that copyright
// notice and this permission notice appear in supporting documentation.
// The author makes no representations about the suitability of this
// software for any purpose. It is provided "as is" without express or 
// implied warranty.

// sa.h 
// SA implementation

#ifndef SA_INCLUDED
#define SA_INCLUDED

#include "sa.options.h"
#include "sa.dsp.h"

// ............................................................................

namespace sa {

// ............................................................................
// Low-level configuration settings, they are not supposed 
// to be exposed to a user (hence all constant).

namespace config
{
    using Gdiplus::Rect;

    const int      nBands          = 40;
    const double   freqMin         = 26.278;
    const double   freqMax         = 22627.;

    const int      pollTime        = 48;         // ms
    const Rect     gridPadding(24, 16, 24, 16);  // px
    const int      peakBarPadding  = 2;          // px
    const wchar_t  labelFontFace[] = L"Verdana";
    const bool     labelFontBold   = true;
    const float    labelFontSize   = 8.f;

    const int      unityGainInt    = 0x40000000; // e.g.: 0.5 -> 0dB
    const double   peakInf         = -140;       // dB
    const double   peakInfEdge     = pow(10., .05 * -73 /* dB */);
    const double   peakInfEdgeEmph = pow(10., .05 * -3  /* dB */);

    const double   postProcK[2][5] = 
    {
        {0.8947, -.082, -.100, .022, .800}, // -6db band intersection
        {0.8767, -.110, -.100, .031, .780}, // -3dB band intersection
    };

    const Rect windowRect    (200, 200, 668, 364); // px
    const Rect infoLeftRect  (  7,   7, 126, 210); // px
    const Rect infoRightRect (132,   7,  96, 210); // px
    const Rect infoRect
    (
        windowRect.X + (windowRect.Width - 20), 
        windowRect.Y + (windowRect.Height + 248) / 2, 
        242, 
        248
    );
   
#if 1
    // grid frequencies, Hz, (second value specifies if the grid line is labeled)
    // "decimal" version:
    const double freqGrid[][2] = 
    {
                       {30, 1},    {40, 0},    {50, 1}, 
            {60, 0},   {70, 0},    {80, 0},    {90, 0},   
           {100, 1},  {200, 1},   {300, 0},   {400, 0},  
           {500, 1},  {600, 0},   {700, 0},   {800, 0},  
           {900, 0}, {1000, 1},  {2000, 1},  {3000, 0}, 
          {4000, 0}, {5000, 1},  {6000, 0},  {7000, 0}, 
          {8000, 0}, {9000, 0}, {10000, 1}, {20000, 1}
    };
#else 
    // simple "linear" alternative:
    const double freqGrid[][2] = 
    {
        {31.25, 1}, {62.5, 1},  {125, 1},  {250, 1},   {500, 1}, 
         {1000, 1}, {2000, 1}, {4000, 1}, {8000, 1}, {16000, 1}
    };
#endif
}

// ............................................................................
// MeterValue handles common peak "metering" procedure, e.g. it applies
// all those "hold/decay" options and provides the 'current' and 'hold'
// peak values to be visualized

struct MeterValue
{
    void tick(double value, const options::Type& option)
    {
        using namespace options;

        peak -= option(peakDecay) * (.001 * pollTime);
        if (peak < value)
            peak = value;

        if (holdTime > 0)
            holdTime -= pollTime;
        else
            hold -= !option(holdInfinite) 
                * option(holdDecay) * (.001 * pollTime);
  
        if (hold < value)
        {
            hold = value;
            holdTime = option(holdHoldTime);
        }
    }

    void resetHold() {hold = peak;}

    MeterValue() : 
        peak(config::peakInf), 
        hold(config::peakInf), 
        holdTime(0)
    {}

public:
    double peak;
    double hold;

private:
    int holdTime;
};

// ............................................................................
// Filter primitive for peak data post-processing

struct Filter
{
    double g, k, z;

    Filter(double g, double k) 
        : g(g), k(k), z(0) {}

    double operator () (double in)
    {
        double out;
        out = g * in + z;
        z   = g * in + k * out;
        return in - out;
    }
};

// ............................................................................
// Drawing - SA visualization 

struct Drawing
{
    void draw(MeterValue (&peak)[config::nBands]) 
    {
        graphics.DrawCachedBitmap(gridImage, 0, 0);
        drawPeaks(peak);
        dst.DrawImage(&buffer, 0, 0);
    }

    Drawing(HWND handle, const options::Type& option) :
        option(option),
        dst(::GetDC(handle)),
        viewRect(windowRect(handle)),
        buffer(viewRect.Width, viewRect.Height, &dst),
        graphics(&buffer),
        gridRect(viewRect),
        gridImage(0)
    {
        using namespace options;
        using namespace Gdiplus;

        // calculate 'the best fit' grid rectangle & peak bar width
        Rect& r   = gridRect;
        int width = r.Width - gridPadding.X - gridPadding.Width;
        barWidth  = (width - peakBarPadding - 1) / nBands - peakBarPadding;
        width     = 1 + peakBarPadding + nBands * (barWidth + peakBarPadding);
        r.X      += (r.Width - width) / 2;
        r.Width   = width;
        r.Y      += gridPadding.Y;
        r.Height -= gridPadding.Y + gridPadding.Height;

        // vertical scale, px/dB
        gridLevelScale = (r.Height - peakBarPadding
            * 2 - 2) / double(option(levelRange));

        // pre-render background & grid
        drawBackground();

        dst.SetCompositingMode(CompositingModeSourceCopy);
        if (!option(peakAlphaColors))
            graphics.SetCompositingMode(CompositingModeSourceCopy);   
    }

    ~Drawing()
    {
        delete gridImage;
    }

private:

    void drawPeaks(MeterValue (&peak)[config::nBands])
    {
        using namespace options;
        using namespace Gdiplus;

        int x = gridRect.X      + peakBarPadding + 1;
        int y = gridRect.Y      + peakBarPadding + 1;
        int h = gridRect.Height - peakBarPadding * 2 - 1;
        int w = barWidth;

        Rect  peakRect[nBands];
        Rect  holdRect[nBands];
        Point holdLine[nBands];

        const double scale = gridLevelScale;

        for (int i = 0; i < nBands; i++)
        {
            int py = y + int(scale * (option(levelTop) - peak[i].peak));
            int hy = y + int(scale * (option(levelTop) - peak[i].hold));
            peakRect[i] = Rect(x, py, w, h + y - py);
            holdRect[i] = Rect(x, hy, w, option(holdBarSize));
            holdLine[i] = Point(x + w / 2, hy);
            x += w + peakBarPadding;
        }

        graphics.SetClip(Rect(gridRect.X + 1, y, gridRect.Width - 1, h));

        SolidBrush peakBrush(option(peakBarColor));
        graphics.FillRectangles(&peakBrush, peakRect, nBands);

        if (option(holdVisible) && option(holdBarType))
        {
            Pen holdPen(option(holdBarColor), 
                1.f + .5f * option(holdBarSize));
            holdLine[0].X = gridRect.X;
            holdLine[nBands - 1].X = gridRect.X + gridRect.Width;
            graphics.SetSmoothingMode(SmoothingModeHighQuality);
            graphics.DrawCurve(&holdPen, holdLine, nBands, .5f); // tension: .25-.75
            graphics.SetSmoothingMode(SmoothingModeHighSpeed);
        }
        else if (option(holdVisible) && !option(holdBarType))
        {
            SolidBrush holdBrush(option(holdBarColor));
            graphics.FillRectangles(&holdBrush, holdRect, nBands);
        }    

        graphics.ResetClip();
    }

    void drawBackground()
    {
        using namespace options;
        using namespace Gdiplus;

        // background

        LinearGradientBrush bkgBrush(viewRect, 
            option(bkgLightColor) | Color::AlphaMask, 
            option(bkgDarkColor)  | Color::AlphaMask, 60.f, 1);
        graphics.FillRectangle(&bkgBrush, viewRect);

        // grid border

        const Rect rect = gridRect;
        Pen borderPen(option(gridBorderColor), 1);
        graphics.DrawRectangle(&borderPen, rect);
        
        // grid lines & labels

        Font font(labelFontFace, labelFontSize, 
            labelFontBold ? FontStyleBold : FontStyleRegular, UnitPixel);
        StringFormat format(StringFormat::GenericDefault());
        graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);

        Pen        gridPen(option(gridLineColor), 1);
        SolidBrush labelBrush(option(labelColor));

        // vertical grid lines & labels

        const int ntext = 16;
        wchar_t   text[ntext];

        format.SetAlignment(StringAlignmentCenter);
        format.SetLineAlignment(StringAlignmentNear);

        const double fRatio1 = 1. / freqMin;
        const double fRatio2 = 1. / log(fRatio1 * freqMax);
        const int x = peakBarPadding + barWidth / 2;
        const int w = rect.Width - x * 2 - 2;
        const int n = sizeof(freqGrid) / sizeof(*freqGrid);
        for (int i = 0; i < n; i++)
        {   
            int xx = rect.X + 1 + x + int(w * fRatio2 
                * log(freqGrid[i][0] * fRatio1));

            graphics.DrawLine(&gridPen, xx, rect.Y + 1,
                xx, rect.GetBottom() - 1);

            if (freqGrid[i][1] > 0)
                graphics.DrawString(freqString(text, ntext, freqGrid[i][0]), -1, 
                    &font, PointF(float(xx + 1), float(rect.GetBottom() + 2)), 
                    &format, &labelBrush);
        }

        // horizontal grid lines & labels

        format.SetAlignment(StringAlignmentFar);
        format.SetLineAlignment(StringAlignmentCenter);
        
        const double scale = gridLevelScale;
        const int    top   = option(levelTop);
        const int    grid  = option(levelGrid);
        const int    range = option(levelRange);

        int level = top - top % grid;
        if (level > top)
            level -= grid;
        while (level > (top - range))
        {
            int y = rect.Y + peakBarPadding + 1 
                + int(scale * (top - level));

            if (level < top)
                graphics.DrawLine(&gridPen, rect.X + 1, 
                    y, rect.GetRight() - 1, y);

            _snwprintf(text, ntext, L"%i", level);
            graphics.DrawString(text, -1, &font, 
                PointF(float(rect.X - 2), float(y)), 
                &format, &labelBrush);

            level -= grid;
        } 

        // ...

        gridImage = new CachedBitmap(&buffer, &graphics);
    }

    static const wchar_t* freqString(wchar_t* dst, int n, double value)
    {
        if (value < 1000)
            _snwprintf(dst, n, L"%.f", value);
        else
        {
            int rem = int(.01 * fmod(value, 1000));
            _snwprintf(dst, n, rem ? L"%ik%i" : L"%ik", 
                int(.001 * value), rem);
        }

        return dst;
    }

    static Gdiplus::Rect windowRect(HWND handle)
    {
        RECT r;
        ::GetClientRect(handle, &r);
        return Rect(0, 0, r.right - r.left, r.bottom - r.top);
    }

    typedef const options::Type&  Options;
    typedef Gdiplus::CachedBitmap CachedBitmap;
    typedef Gdiplus::Graphics     Graphics;
    typedef Gdiplus::Bitmap       Bitmap;
    typedef Gdiplus::Rect         Rect;
    
private:
    Options       option;
    Graphics      dst;
    const Rect    viewRect;
    Bitmap        buffer;
    Graphics      graphics;
    Rect          gridRect;
    CachedBitmap* gridImage;
    double        gridLevelScale;
    int           barWidth;

private:
    Drawing(const Drawing&);
    Drawing& operator = (const Drawing&);
};

// ............................................................................
// View binds drawing, options, settings, keyboard control etc., all together

template <typename Plugin, typename InfoTool>
struct View : options::Handler
{
    void poll(int) 
    {
        using namespace options;
        if (option(peakFreeze))
            return;

        double value[nBands];
        read(value);
        postprocess(value);
        double edge = peakInfEdge;
        for (int i = 0; i < nBands; i++)
            peak[i].tick((value[i] 
                > (edge *= peakInfEdgeEmph)) 
                ? 20. * log10(value[i])
                : peakInf, option);

        if (!::IsIconic(handle))
            draw();
    }

    void read(double (&value)[config::nBands])
    {
        dword v;
        for (int i = 0; i < nBands; i++)
        {
            plugin->get_dsp_register(word(dsp::pk00 + i), &v);
            plugin->set_dsp_register(word(dsp::pk00 + i), 0);
            value[i] = (1. / unityGainInt) * v;
        }
    }

    void applyBandWidth()
    {
        using namespace options;
        using namespace dsp;

        const unsigned* const value = 
            option(bandIntersect) ? bandWidthK3 : bandWidthK6;

        for (int i = 0; i < nBands; i++)
            plugin->set_dsp_register(word(kb00 + i * 4), value[i]);

        for (int i = 0; i < nBands; i++)
            plugin->set_dsp_register(word(pk00 + i), 0);
    }

    void postprocess(double (&value)[config::nBands])
    {
        using namespace options;
        if (!option(bandLeakComp))
            return;

        const double (&k)[5] = postProcK[option(bandIntersect)];
        Filter filter1(k[1], k[2]);
        Filter filter2(k[3], k[4]);

        filter2(filter1(value[nBands - 1] * .707));
        for (int i = nBands - 1; i >= 0; i--)
            value[i] = filter2(filter1(value[i])) * k[0];

        filter1.z  = filter2.z = 0;
        filter2(filter1(value[0] * .707));
        for (int i = 0; i < nBands; i++)
            value[i] = filter2(filter1(value[i]));
    };

    void draw() 
    {
        if (!drawing)
            drawing = new Drawing(handle, option);
        drawing->draw(peak);
    }

    void keyDown(int key, int flags)
    {
        enum
        {
            Ctrl   = 0x0100,
            Shift  = 0x0200,
            Repeat = 0x1000
        };

        int code = key
            + Ctrl   * (0 > ::GetKeyState(VK_CONTROL))
            + Shift  * (0 > ::GetKeyState(VK_SHIFT))
            + Repeat * !!(flags & KF_REPEAT);

        using namespace options;

        switch (code)
        {
        case VK_F1: 
            return help();

        case VK_F2: 
            info->toggle(plugin->instance, 
                handle, "Settings");
            return updateInfo();

        case 'R': 
            return resetHold();

        case 'F': 
            return option.toggle(peakFreeze);

        case 'H': 
            return option.toggle(holdVisible);

        case Ctrl + 'I':
            return option.toggle(holdInfinite);

        case Ctrl + 'B':
            option.toggle(bandLeakComp);
            if (!option(bandLeakComp))
                option.toggle(bandIntersect);
            return;

        case Ctrl + 'H':
            option(holdVisible, 1);
            return option.toggle(holdBarType);

        case Ctrl + 'J':
            return option.cycle(holdBarSize);

        case Ctrl + 'T': 
            return option.toggle(peakAlphaColors);

        case Ctrl + 'D':
            colorScheme = colorSchemeUnknown;
            return option.defaults();

        case Shift + 'D': 
            colorScheme = colorSchemeUnknown;
            return option.defaults(true);

        case Ctrl + Shift + 'D': 
            return savePreset("Default");

        case Shift + 'C': 
            return loadColorScheme(true);

        case VK_F5: 
            return loadColorScheme(false);

        case Ctrl + 'G':
            return option.cycle(levelGrid);

        case Shift + VK_OEM_PERIOD: // '>'
        case Shift + VK_OEM_PERIOD + Repeat:
            return option.inc(holdHoldTime);

        case Shift + VK_OEM_COMMA: // '<'
        case Shift + VK_OEM_COMMA  + Repeat:
            return option.dec(holdHoldTime);

        case Ctrl + VK_OEM_PERIOD: // '>'
        case Ctrl + VK_OEM_PERIOD + Repeat:
            return option.dec(holdDecay);

        case Ctrl + VK_OEM_COMMA: // '<'
        case Ctrl + VK_OEM_COMMA  + Repeat:
            return option.inc(holdDecay);

        case Ctrl + VK_OEM_6: // '}'
        case Ctrl + VK_OEM_6 + Repeat:
            return option.dec(peakDecay);

        case Ctrl + VK_OEM_4: // '{'
        case Ctrl + VK_OEM_4  + Repeat:
            return option.inc(peakDecay);

        case Shift + VK_UP:
        case Shift + VK_UP + Repeat:
            return option.dec(levelTop);

        case Shift + VK_DOWN:
        case Shift + VK_DOWN + Repeat:
            return option.inc(levelTop);

        case Ctrl + VK_UP:
        case Ctrl + VK_UP + Repeat:
            return option.dec(levelRange);

        case Ctrl + VK_DOWN:
        case Ctrl + VK_DOWN + Repeat:
            return option.inc(levelRange);
        }

        if ((code >= (Ctrl + '0'))
            && (code <= (Ctrl + '9')))
                return loadPreset((const char*) &key);

        if ((code >= (Ctrl + Shift + '0'))
            && (code <= (Ctrl + Shift + '9')))
                return savePreset((const char*) &key);
    }

    void mouseDoubleClick()
    {
        resetHold();        
    }

    void help() const
    {
        char file[MAX_PATH] = "";
        ::GetModuleFileName(plugin->instance, file, sizeof(file));
        if (char* ext = strrchr(file, '.'))
        {
            strcpy(ext + 1, "html");
            ::ShellExecute(handle, 0, file, 0, 0, SW_NORMAL);
        }
    }

    void loadPreset(const char* key)
    {
        using namespace options;
        Settings settings(Settings(presetsKey), key);
        for (int i = 0; i < OptionsCount; i++)
            option(i, settings.get(option.name(i), option(i)), 0);
        colorScheme = colorSchemeUnknown;
        optionsChanged();
    }

    void savePreset(const char* key) const
    {
        using namespace options;
        Settings settings(Settings(presetsKey), key);
        for (int i = 0; i < OptionsCount; i++)
            settings.set(option.name(i), option(i));
    }

    void loadColorScheme(bool cycle)
    {
        using namespace options;
        Settings settings(Settings(colorsKey), colorScheme += cycle);
        if (settings)
            for (int i = ColorsIndex; i < OptionsCount; i++)
                option(i, settings.get(option.name(i), option(i)), 0);
        else
        {
            colorScheme = colorSchemeDefault;
            for (int i = ColorsIndex; i < OptionsCount; i++)
                option(i, option.default_(i), 0);    
        }

        optionsChanged();
    }

    void resetHold()
    {
        for (int i = 0; i < nBands; i++)
            peak[i].resetHold();
    }

    void resized() 
    {
        optionsChanged();
        ::InvalidateRect(handle, 0, 0);
    }

    void optionsChanged()
    {
        updateInfo();
        applyBandWidth();

        // trigger background/grid re-rendering
        delete drawing; 
        drawing = 0;
        if (option(options::peakFreeze))
            ::InvalidateRect(handle, 0, 0);
    }

    void updateInfo()
    {
        char left[512];
        char right[512];
        int  nLeft  = option.infoString(left, sizeof(left));
        int  nRight = option.valueString(right, sizeof(right));
        
        if (colorScheme != colorSchemeUnknown)
        {
            char name[MAX_PATH] = "Default";
            _snprintf(left + nLeft, sizeof(left) - nLeft, 
                "\nColour scheme:");
            _snprintf(right + nRight, sizeof(right) - nRight, 
                "\n\"%s\"", Settings(colorsKey).subKey(name, colorScheme));
        }

        left[sizeof(left) - 1]   = 0;
        right[sizeof(right) - 1] = 0;
        info->update(left, right);
    }

    View(HWND handle, Plugin* plugin, InfoTool* info, int* optionsArray) : 
        plugin(plugin),
        info(info),
        option(optionsArray),
        handle(handle),
        drawing(0),
        colorScheme(colorSchemeUnknown)
    {
        using namespace options;
        option.handler(this);
        option(peakFreeze, 0);

        // 5 4 3 2 1 ... Go!
        timer.start<View, &View::poll>(this, pollTime);
    }

    ~View() 
    {
        delete drawing;
    }

private:
    MeterValue      peak[config::nBands];
    Drawing*        drawing;
    Plugin* const   plugin;
    InfoTool* const info;
    options::Type   option;
    const HWND      handle;
    Timer           timer;
    int             colorScheme;

    enum           
    {
        colorSchemeDefault = -1,
        colorSchemeUnknown = -2
    };

private:
    friend struct Timer;
    View(const View&);
    View& operator = (const View&);
};

// ............................................................................

} // ~ namespace sa

// ............................................................................

#endif // ~ SA_INCLUDED
