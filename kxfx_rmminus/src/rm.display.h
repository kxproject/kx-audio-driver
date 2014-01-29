
// kX DSP Resource Meter demo library
// Copyright (c) Max Mikhailov, 2009

// Permission to use, copy, modify, distribute, and sell this software
// for any purpose is hereby granted without fee, provided that the above
// copyright notice appears in all copies and that both that copyright
// notice and this permission notice appear in supporting documentation.
// The author makes no representations about the suitability of this
// software for any purpose. It is provided "as is" without express or
// implied warranty.

// rm.display.cpp
// RM visualization

#include "includes.h"
#include "helpers.h"
#include "rm.counter.h"
#include "rm.settings.h"

// ............................................................................

namespace rm {

using namespace Gdiplus;

// ............................................................................

struct Display
{
    int width()  const {return settings(config::boxWidth);}
    int height() const {return settings(config::boxHeight);}

    void draw(HDC dc, int x, int y, bool dragging)
    {
        Graphics graphics(dc);
        draw(graphics, x, y, dragging);
    }

    int draw(Graphics& graphics, int x, int y, bool dragging) const
    {
        graphics.TranslateTransform(float(x), float(y));
        graphics.SetCompositingQuality(CompositingQualityHighQuality);

        graphics.FillPath(&bkgBrush,  &borderPath); // box background
        graphics.DrawPath(&borderPen, &borderPath); // box border

        if (dragging)
            return graphics.ResetTransform();

        graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
        graphics.TranslateTransform(float(meterRect.X), float(meterRect.Y));
        const int pad = meterRect.Width + meterPad(meterRect.Width);

        const int ntext = 8;
        wchar_t   text[ntext];

        for (int i = 0; i < Counter::IndexCount; i++)
        {
            double v = counter.value(i);
            y = int(v * meterRect.Height + .5);

            Rect rhi(0, 0, meterRect.Width, meterRect.Height - y);
            Rect rlo(0, meterRect.Height - y, meterRect.Width, y);

            graphics.FillRectangle(meterBrush[i][0], rhi); // off meter
            graphics.FillRectangle(bevelBrush[0],    rhi); // off bevel
            graphics.FillRectangle(meterBrush[i][1], rlo); // on  meter
            graphics.FillRectangle(bevelBrush[1],    rlo); // on  bevel
            --rlo.Width, --rlo.Height;
            graphics.DrawRectangle(&darkPen, rlo);         // meter border

            _snwprintf(text, ntext, L"%i", int(100. * v + .5));
            graphics.DrawString(text, -1, &font,
                PointF(.5f * (meterRect.Width + 1),
                float(meterRect.Height + labelPad)),
                &stringFormat, &labelBrush);

            graphics.TranslateTransform(float(pad), 0);
        }

        return graphics.ResetTransform();
    }

    Display(Counter& counter) :
        counter(counter),
        settings(config::settingsKey),
        font(config::labelFont, float(settings(config::labelSize)),
            FontStyleRegular, UnitPixel),
        darkPen(meterBorderColor()),
        bkgBrush(sqrAlpha(settings(config::bkgColor))),
        borderPen(sqrAlpha(settings(config::borderColor))),
        labelBrush(settings(config::labelColor)),
        stringFormat(StringFormat::GenericDefault())
    {
        using namespace config;

        counter.useLegacyNames(!!settings(ancientNames));

        // cook layout:

        const int ww = settings(boxWidth);
        const int hh = settings(boxHeight);

        int n = Counter::IndexCount;
        int w = ((ww - (n + 1) * meterPadMin)
            * meterPadRatio) / (n * meterPadRatio + n + 1);
        int y = meterPad(w);
        int x = (ww - n * w - (n - 1) * y) / 2 + 1;
        int h = hh - 2 * y - settings(labelSize);

        labelPad  = (y - 1 - settings(labelSize) / 4) / 2;
        meterRect = Rect(0, 0, w, h);

        // cook drawing objects:

        for (int j = 0; j < 2; j++)
            createBevelBrush(j);

        for (int i = 0; i < Counter::IndexCount; i++)
            for (int j = 0; j < 2; j++)
                createMeterBrush(i, j);

        meterRect = Rect(x, y, w, h);
        roundedRectPath(borderPath, ww, hh, settings(borderRound));
        stringFormat.SetAlignment(StringAlignmentCenter);
    }

    void createTooltips(Tooltips& tt, HMODULE module, HWND parent) const
    {
        tt.create(module, parent);
        Rect r(meterRect);
        const int pad = r.Width + meterPad(r.Width);
        for (int i = 0; i < Counter::IndexCount; i++)
        {
            tt.attach(i, r.X, r.Y, r.Width, r.Height);
            r.X += pad;
        }
    }

    ~Display()
    {
        for (int j = 0; j < 2; j++)
            delete bevelBrush[j];

        for (int i = 0; i < Counter::IndexCount; i++)
            for (int j = 0; j < 2; j++)
                delete meterBrush[i][j];
    }

private:
    UsingGdiPlus         ugp_;
    Counter&             counter;
    settings::Type       settings;
    Font                 font;
    Pen                  darkPen;
    SolidBrush           bkgBrush;
    int                  labelPad;
    Rect                 meterRect;
    Pen                  borderPen;
    GraphicsPath         borderPath;
    SolidBrush           labelBrush;
    StringFormat         stringFormat;
    LinearGradientBrush* bevelBrush[2];
    LinearGradientBrush* meterBrush[Counter::IndexCount][2];

    Display(const Display&);
    Display& operator = (const Display&);

    static int meterPad(int w)
    {
        using namespace config;
        return meterPadMin + (w + 1) / meterPadRatio;
    }

    void createMeterBrush(int i, int j)
    {
        static const float p[3] = {.0f, .5f, 1.f};

        static const int colorIndex[Counter::IndexCount] =
        {
            config::codeColorHi,
            config::regsColorHi,
            config::iTramRegsColorHi,
            config::xTramRegsColorHi,
            config::iTramSizeColorHi,
            config::xTramSizeColorHi,
        };

        const Color c[3] =
        {
            meterColor(settings(colorIndex[i] + 0), !j),
            meterColor(settings(colorIndex[i] + 1), !j),
            meterColor(settings(colorIndex[i] + 2), !j)
        };

        LinearGradientBrush* b = new LinearGradientBrush
            (meterRect, 0, 0, LinearGradientModeVertical);
        b->SetInterpolationColors(c, p, 3);
        b->SetWrapMode(WrapModeTileFlipX);
        meterBrush[i][j] = b;
    }

    void createBevelBrush(int j)
    {
        using namespace config;

        static const float p[]  = {.0f, .3f, 1.f};
        unsigned a = mulAlpha(0x80000000, settings(bevelDepth));
        if (!j)
            a = mulAlpha(a, (settings(emptyOpacity) + 1) >> 1);
        Color c[]  = {0x000000 | a, 0xffffff | a, 0x000000 | a};

        LinearGradientBrush* b = new LinearGradientBrush
            (meterRect, 0, 0, LinearGradientModeHorizontal);
        b->SetInterpolationColors(c, p, 3);
        b->SetWrapMode(WrapModeTileFlipX);
        bevelBrush[j] = b;
    }

    static void roundedRectPath(GraphicsPath& p, int w, int h, int r)
    {
        p.AddLine(r, 0, w - r * 2, 0);
        p.AddArc(w - r * 2, 0, r * 2, r * 2, 270, 90);
        p.AddLine(w, r, w, h - r * 2);
        p.AddArc(w - r * 2, h - r * 2, r * 2, r * 2, 0, 90);
        p.AddLine(w - r * 2, h, r, h);
        p.AddArc(0, h - r * 2, r * 2, r * 2, 90, 90);
        p.AddLine(0, h - r * 2, 0, r);
        p.AddArc(0, 0, r * 2, r * 2, 180, 90);
        p.CloseFigure();
    }

    ARGB meterBorderColor() const
    {
        return min<ARGB>(mulAlpha(0xCC000000,
            settings(config::bevelDepth)), 0x66000000);
    }

    ARGB meterColor(ARGB color, bool applyOpacity) const
    {
        if (applyOpacity)
            color = mulAlpha(color,
                (settings(config::emptyOpacity) + 1) >> 1);
        return color;
    }

    static ARGB mulAlpha(ARGB color, unsigned x)
    {
        unsigned a = (color >> 24) & 0xFF;
        a = ((a * x + 0xFF) << 16) & 0xFF000000;
        return a | (color & 0x00FFFFFF);
    }

    static ARGB sqrAlpha(ARGB color)
    {
        unsigned a = (color >> 24) & 0xFF;
        a = ((a * a + 0xFF) << 16) & 0xFF000000;
        return a | (color & 0x00FFFFFF);
    }
};

// ............................................................................

} // ~ namespace rm

// ............................................................................
