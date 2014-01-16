
#include "ufw.h"
#include "udrawlib.h"
#include <math.h>

//..........................................................................
#define CheckGetDibBits(r) \
	if (!r) \
	{ \
		int rv2 = GetLastError(); \
		trace("<kxu> uDibBitsRGB::uDibBitsRGB(), GetDIBits failed\n" \
			"hbitmap: %x,\n" \
			"bm.bmHeight: %i,\n" \
			"pBits: %x,\n" \
			"lpbi.biBitCount: %x,\n" \
			"lpbi.biCompression: %x,\n" \
			"lpbi.biSize: %x,\n" \
			"LastError: %x.\n" \
			, hbitmap, bm.bmHeight, pBits, \
			lpbi->bmiHeader.biBitCount, \
			lpbi->bmiHeader.biCompression, \
			lpbi->bmiHeader.biSize, \
			rv2); \
		VERIFY(r); \
	} \
// endof CheckGetDibBits

/***************************************************************************

  CLEAN UP THAT FILE FROM MFC CODE (where possible ;)!!!

***************************************************************************/

void kxuBitBlt(HDC hDstDC, int x, int y, int nWidth, int nHeight, HDC hSrcDC,
		int xSrc, int ySrc, int nWidthSrc, int nHeightSrc, DWORD dwRop)
{
	int i, j, w, h = nHeight;
	int xend = x + nWidth; 
	int yend = y + nHeight;
	for (i = y; i < yend; i += nHeightSrc){
		if ((h + i) > yend) h = yend - i;
		w = nWidthSrc;
		for (j = x; j < xend; j += nWidthSrc){
			if ((w + j) > xend) w = xend - j;
			BitBlt(hDstDC, j, i, w, h, hSrcDC,
							xSrc, ySrc, dwRop);
		}
	}
	return;
}

/***************************************************************************/

#define _rb(cl) ((int) (((cl) >> 16) & 0xffL))
#define _rg(cl) ((int) (((cl) >> 8) & 0xffL))
#define _rr(cl) ((int) ((cl) & 0xffL))

// 0.8 fixed-point math (0xFF = 1.0) 
#define _mul_f_8(a, b) (((((int) a) * ((int) b)) + 0xff) >> 0x8)

// 16.16 fixed-point math (0x10000 = 1.0)
#define _mul_from_f16_(a, b)	((((a) * (b)) + 0x8000) >> 0x10)
#define _div_to_f16_(a, b)	(((a) << 0x10) / (b))
#define _interp_from_f16_(a, b, f) ((a) + ((((f) * ((b) - (a))) + 0x8000) >> 0x10)) 
#define _mul_f16_(a, b) (((((a) >> 0x2) * ((b) >> 0x2)) + 0x800) >> 0xc)

_inline int _pscalefact_f16_(int h, int v) 
{
	h = h >> 0x2; v = v >> 0x2;
	h = ((h*h + v*v) + 0x800) >> 0xc;
	return (int) (sqrt(h / 65536.) * 65536.);
}

_inline void rgb2rgb128(rgb128* _rgb128, COLORREF cl)
{
	_rgb128->r = (cl & 0xff);
	cl >>= 8;
	_rgb128->g = (cl & 0xff);
	cl >>= 8;
	_rgb128->b = (cl & 0xff);
	_rgb128->a = 0;
}

_inline COLORREF _PixelBlend(COLORREF dc, COLORREF sc, DWORD dwMode, DWORD Depth)
{
	int r, g, b;
	int idepth;
	switch (dwMode)
	{
	case BLEND_COPY: // Depth 0..ff !
		idepth = 0xff - Depth;
		r = _mul_f_8(_rr(dc), idepth) + _mul_f_8(_rr(sc), Depth);
		g = _mul_f_8(_rg(dc), idepth) + _mul_f_8(_rg(sc), Depth);
		b = _mul_f_8(_rb(dc), idepth) + _mul_f_8(_rb(sc), Depth);
		return RGB(r, g, b);

	case BLEND_SCREEN: // no Depth yet
		dc = ~dc;
		sc = ~sc;
		r = _mul_f_8(_rr(dc), _rr(sc));
		g = _mul_f_8(_rg(dc), _rg(sc));
		b = _mul_f_8(_rb(dc), _rb(sc));
		dc = RGB(r, g, b);
		return ((~dc) & 0xffffff);

	case BLEND_MULTIPLY: // no Depth yet
		r = _mul_f_8(_rr(dc), _rr(sc));
		g = _mul_f_8(_rg(dc), _rg(sc));
		b = _mul_f_8(_rb(dc), _rb(sc));
		return RGB(r, g, b);

	case BLEND_OVERLAY:
		r = _rr(dc);
		r = r * (0xFFL - r) * (_rr(sc) - 0x80L) * Depth;
		r = ((r + 0x40000L) >> 0x17) + _rr(dc);
		g = _rg(dc);
		g = g * (0xFFL - g) * (_rg(sc) - 0x80L) * Depth;
		g = ((g + 0x40000L) >> 0x17) + _rg(dc);
		b = _rb(dc);
		b = b * (0xFFL - b) * (_rb(sc) - 0x80L) * Depth;
		b = ((b + 0x40000L) >> 0x17) + _rb(dc);
		return RGB(r, g, b);
	}
	return dc;
}

int _kxuGetRGBBits(COLORREF** ppBits, HBITMAP hBitmap, UINT uStartScan, UINT cScanLines)
{
	HDC ScreenDC = GetWindowDC(NULL); // screen
	#define BITCOUNT32	32L
	BITMAPV4HEADER bhdr =  {sizeof BITMAPINFOHEADER};
	LPBITMAPINFO lpbi = (LPBITMAPINFO) &bhdr;

	VERIFY(GetDIBits(ScreenDC, hBitmap, uStartScan, cScanLines,     
		NULL, lpbi, DIB_RGB_COLORS));
	bhdr.bV4SizeImage = ((bhdr.bV4SizeImage << 5) / (int) bhdr.bV4BitCount);
	bhdr.bV4BitCount = BITCOUNT32;
	*ppBits = (COLORREF*) malloc(bhdr.bV4SizeImage);
	VERIFY(GetDIBits(ScreenDC, hBitmap, uStartScan, cScanLines,     
		(LPVOID) *ppBits, lpbi, DIB_RGB_COLORS));

	ReleaseDC(NULL, ScreenDC);
	return bhdr.bV4SizeImage;
}

int _kxuSetRGBBits(COLORREF* pBits, HBITMAP hBitmap, UINT uStartScan, UINT cScanLines)
{
	HDC ScreenDC = GetWindowDC(NULL); // screen
	#define BITCOUNT32	32L
	BITMAPV4HEADER bhdr =  {sizeof BITMAPINFOHEADER};
	LPBITMAPINFO lpbi = (LPBITMAPINFO) &bhdr;

	int rv = GetDIBits(ScreenDC, hBitmap, uStartScan, cScanLines,     
		NULL, lpbi, DIB_RGB_COLORS);
	ASSERT(rv);
	bhdr.bV4SizeImage = ((bhdr.bV4SizeImage << 5) / (int) bhdr.bV4BitCount);
	bhdr.bV4BitCount = BITCOUNT32;
	// fixme, without that masks SetDIBits does not work :(
	bhdr.bV4RedMask =	0xff0000;
	bhdr.bV4GreenMask = 0xff00;
	bhdr.bV4BlueMask =	0xff;
	rv = SetDIBits(ScreenDC, hBitmap, uStartScan, cScanLines,     
		pBits, lpbi, DIB_RGB_COLORS);
	ASSERT(rv);

	ReleaseDC(NULL, ScreenDC);
	return bhdr.bV4SizeImage;
}

void kxuApplyTexture(HBITMAP hDstBitmap, LPRECT pDstRect, HBITMAP hTexture,
		int xTxOrigin, int yTxOrigin, DWORD /* Reserved */, DWORD Depth)
{
	if (Depth == 0) return;
	// trace("<kxu> kxuApplyTexture()\n");
	uDibBitsRGB SrcBits(hTexture);
	uDibBitsRGB DstBits(hDstBitmap);

	COLORREF c;

	RECT xrect = {0, 0, DstBits.Width(), DstBits.Height()};
	IntersectRect(pDstRect, pDstRect, &xrect);

	int w = RECTW(*pDstRect), h = RECTH(*pDstRect);
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			c = _PixelBlend(DstBits.Get(x + pDstRect->left, y + pDstRect->top), 
				SrcBits.Get((x + xTxOrigin) % SrcBits.Width(), (y + yTxOrigin) % SrcBits.Height()), 
				BLEND_OVERLAY, Depth);
			DstBits.Set(x + pDstRect->left, y + pDstRect->top, c);
		}
	}
	SrcBits.CancelSave();
}

void kxuBlend(HBITMAP hDstBitmap, LPRECT pDstRect, HBITMAP hSrcBitmap,
		int xSrc, int ySrc, DWORD dwMode, DWORD Depth)
{
	COLORREF* pDstBuffer;
	COLORREF* pSrcBuffer;

	// calculate drawing area
	BITMAP sbm, dbm;
	GetObject(hSrcBitmap, sizeof BITMAP, &sbm);
	GetObject(hDstBitmap, sizeof BITMAP, &dbm);

	int height = RECTH(*pDstRect);
	if (height > (dbm.bmHeight - pDstRect->top)) height = (dbm.bmHeight - pDstRect->top);
	if (height > (sbm.bmHeight - ySrc)) height = (sbm.bmHeight - ySrc);
	if (height == 0) return;

	int width = RECTW(*pDstRect);
	if (width > (dbm.bmWidth - pDstRect->left)) width = (dbm.bmWidth - pDstRect->left);
	if (width > (sbm.bmWidth - xSrc)) width = (sbm.bmWidth - xSrc);
	if (width == 0) return;

	// retrieve source bitmap buffer
	_kxuGetRGBBits(&pSrcBuffer, hSrcBitmap, ySrc, height);

	// retrieve destination bitmap buffer
	_kxuGetRGBBits(&pDstBuffer, hDstBitmap, pDstRect->top, height);

	// blending
	int srcxskip = sbm.bmWidth - width;
	int dstyskip = dbm.bmWidth - width;

	int x, y, sp = xSrc, dp = pDstRect->left;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pDstBuffer[dp] = _PixelBlend(pDstBuffer[dp], pSrcBuffer[sp], dwMode, Depth);
			dp++; sp++;
		}
		sp += srcxskip;
		dp += dstyskip;
	}

	_kxuSetRGBBits(pDstBuffer, hDstBitmap, pDstRect->top, height);

	free(pSrcBuffer);
	free(pDstBuffer);
}

// BUG!
void kxuBlendDC(HBITMAP hDstBitmap, LPRECT pDstRect, HDC hSrcDC,
		int xSrc, int ySrc, DWORD dwMode, DWORD Depth)
{
	HBITMAP hSrcBitmap = (HBITMAP) GetCurrentObject(hSrcDC, OBJ_BITMAP);
	kxuBlend(hDstBitmap, pDstRect, hSrcBitmap, xSrc, ySrc, dwMode, Depth);
}

/*
_inline void interpcolor(rgb128 *dc, rgb128 sc, rgb128 ec, int f)
{
	dc->r = _interp_from_f16_(sc.r, ec.r, f); 
	dc->g = _interp_from_f16_(sc.g, ec.g, f); 
	dc->b = _interp_from_f16_(sc.b, ec.b, f);
}

_inline int fcurve_f16_(int f, int c)
{
	int x;
	switch (c)
	{
	case CURVE_LINEAR:
		return f;
	case CURVE_MOREIN:
		return _mul_f16_(f, f);
	case CURVE_MOREOUT:
		return _mul_f16_(f - (2 << 0x10), -f); //(f - 2) * -f
	case CURVE_LESSEDGE:
		f = f - 0x8000;
		x = _mul_f16_(f, f); if (f < 0) x = -x;
		return (x + 0x4000) << 0x1;
	case CURVE_MOREEDGE:
		f = 0x8000 - f;
		x = _mul_f16_(f, -f); if (f > 0) x = -x;
		return -((x + 0x4000) << 0x1);
	default:
		return f;
	}
}

void kxuGradientFill(CDC* pDstDC, LPRECT pDstRect, DWORD dwMode, COLORREF* pColor, LONG* pPoint)
{
	POINT pPoints[2]; memcpy(pPoints, pPoint, sizeof (pPoints));

	switch (dwMode & GRD_FILL_FIGURE_MASK)
	{
	case GRD_FILL_RECT_H:
	case GRD_FILL_RECT_V:
		_kxuGrdFill_Rect(pDstDC, pDstRect, dwMode, pColor, (LONG*) pPoints);
		break;
	case GRD_FILL_CIRCLE:
		pPoints[1].y = pPoints[1].x;
		// no break here
	case GRD_FILL_ELLIPSE:
		_kxuGrdFill_Ellipse(pDstDC, pDstRect, dwMode, pColor, pPoints);
		break;
	}
}

void _kxuGrdFill_Rect(CDC* pDstDC, LPRECT pDstRect, DWORD dwMode, COLORREF* pColor, LONG* pPoint)
{
	// fixme, very unoptimized

	rgb128 strtcl; rgb2rgb128(&strtcl, pColor[0]);
	rgb128 endcl; rgb2rgb128(&endcl, pColor[1]);
	rgb128 _dstcl;
	COLORREF dstcl;

	int s = pPoint[0];
	int e = pPoint[1];

	int x = pDstRect->left, y = pDstRect->top, f;
	int *h, *v;
	int *he, *ve; 

	switch (dwMode & GRD_FILL_FIGURE_MASK)
	{
	case GRD_FILL_RECT_H:
		h = &x;
		he = &pDstRect->right;
		v = &y;
		ve = &pDstRect->bottom;
		break;
	case GRD_FILL_RECT_V:
		h = &y;
		he = &pDstRect->bottom;
		v = &x;
		ve = &pDstRect->right;
	default:
		return;
	}

	for (*h; *h < *he; *h = *h + 1)
	{
		
		if (*h >= e) {dstcl = pColor[1];}
		else if (*h <= s) {dstcl = pColor[0];}
		else
		{
			f = _div_to_f16_((*h - s), (e - s));
			f = fcurve_f16_(f, dwMode & CURVE_MASK);
			interpcolor(&_dstcl, strtcl, endcl, f); 
			dstcl = RGB(_dstcl.r, _dstcl.g, _dstcl.b);
		}
		*v = ((dwMode & GRD_FILL_FIGURE_MASK) == GRD_FILL_RECT_V) ? pDstRect->top : pDstRect->left;
		for (*v; *v < *ve; *v = *v + 1)
		{
			pDstDC->SetPixel(x, y, dstcl);
		}
	}
}

void _kxuGrdFill_Ellipse(CDC* pDstDC, LPRECT pDstRect, DWORD dwMode, COLORREF* pColor, LPPOINT pPoint)
{
	rgb128 strtcl; rgb2rgb128(&strtcl, pColor[0]);
	rgb128 endcl; rgb2rgb128(&endcl, pColor[1]);
	rgb128 dstcl;

	POINT* strtpoint = &pPoint[0];
	int e = labs(pPoint[1].x - strtpoint->x);
	int vscale = _div_to_f16_(e, labs(pPoint[1].y - strtpoint->y));

	int x, y, f;

	int h; int v = pDstRect->top;

	for (v; v < pDstRect->bottom; v++)
	{
		h = pDstRect->left;
		y = _div_to_f16_((v - strtpoint->y), e);
		y = _mul_f16_(y, vscale);
		for (h; h < pDstRect->right; h++)
		{
			x = _div_to_f16_((h - strtpoint->x), e);
			f = _pscalefact_f16_(x, y);
			dstcl = endcl; // point is out of ellipse
			if (f < 0x10000){
				// point is inside
				f = fcurve_f16_(f, dwMode & CURVE_MASK);
				interpcolor(&dstcl, strtcl, endcl, f); 
			}
			pDstDC->SetPixel(h, v, RGB(dstcl.r, dstcl.g, dstcl.b));
		}
	}
}
*/

/***************************************************************************/

void kxuCorrectMinRect(LPRECT r, LPRECT minr, int options)
{
	LONG minw = minr->right - minr->left; 
	LONG minh = minr->bottom - minr->top; 
	if (options & BET_FIXEDWIDTH) r->right = minr->right;
	else if ((r->right - r->left) < minw) r->right = r->left + minw;
	if (options & BET_FIXEDHEIGHT) r->bottom = minr->bottom;
	else if ((r->bottom - r->top) < minh) r->bottom = r->top + minh;
}

void kxuCenterRectToAnotherOne(LPRECT r, LPRECT cont)
{
	LONG w = RECTW(*r);
	LONG h = RECTH(*r);
	LONG dl = (RECTW(*cont) - w) / 2;
	LONG dt = (RECTH(*cont) - h) / 2;
	r->left = cont->left + dl;
	r->bottom = cont->bottom - dt;
	r->right = r->left + w;
	r->top = r->bottom - h;
}

//.............................................................................

int kxuGetBitmapSize(HBITMAP bitmap, LPRECT rect)
{
	BITMAP bm;
	GetObject(bitmap, sizeof BITMAP, &bm);
	SETRECT(*rect, 0, 0, bm.bmWidth, bm.bmHeight);
	return 0;
}

//.............................................................................

#define BITCOUNT32	32L

uDibBitsRGB::uDibBitsRGB(HBITMAP hBitmap)
{
	ASSERT(hBitmap);
	ScreenDC = GetWindowDC(NULL); // screen
	hbitmap = hBitmap;
	
	GetObject(hbitmap, sizeof BITMAP, &bm);
	ZeroMemory(&bhdr.bV4Size, sizeof BITMAPINFOHEADER);
	bhdr.bV4Size = sizeof BITMAPINFOHEADER;
	LPBITMAPINFO lpbi = (LPBITMAPINFO) &bhdr;
	VERIFY(GetDIBits(ScreenDC, hbitmap, 0, bm.bmHeight,     
		NULL, lpbi, DIB_RGB_COLORS));
	bhdr.bV4SizeImage = ((bhdr.bV4SizeImage << 5) / (int) bhdr.bV4BitCount);
	bhdr.bV4BitCount = BITCOUNT32;
	pBits = (COLORREF*) malloc(bhdr.bV4SizeImage);
	ASSERT(pBits);
	bhdr.bV4Height = bhdr.bV4Height;
	//int rv =
	GetDIBits(ScreenDC, hbitmap, 0, bm.bmHeight,    
		(LPVOID) pBits, lpbi, DIB_RGB_COLORS);
	// CheckGetDibBits(rv);
	changed = 1;
}

uDibBitsRGB::~uDibBitsRGB()
{
	LPBITMAPINFO lpbi = (LPBITMAPINFO) &bhdr;

	if (changed)
	{
		bhdr.bV4RedMask =	0xff0000;
		bhdr.bV4GreenMask = 0xff00;
		bhdr.bV4BlueMask =	0xff;
		VERIFY(SetDIBits(ScreenDC, hbitmap, 0, bm.bmHeight,     
			pBits, lpbi, DIB_RGB_COLORS));
	}

	ReleaseDC(NULL, ScreenDC);
	free(pBits);
}

//.............................................................................
/*
const COLORREF redalarm = 0x000000ff;

#define BITCOUNT32	32L

uRGBBits::uRGBBits(HBITMAP _hbitmap, LPRECT _rect, int _flags, LPPOINT _origin)
{
	ASSERT(hbitmap);
	ScreenDC = GetWindowDC(NULL); // screen

	hbitmap = _hbitmap;
	flags = _flags;
	wrap = flags & UBF_WRAP;

	ZM(rect);
	ZM(origin);
	if (_origin) memcpy(&origin, _origin, sizeof origin);
	
	GetObject(hbitmap, sizeof BITMAP, &bm);
	if (_rect) memcpy(&rect, _rect, sizeof rect);
	else rect.right = bm.bmWidth; rect.bottom = bm.bmHeight;

	ZeroMemory(&bhdr.bV4Size, sizeof BITMAPINFOHEADER);
	bhdr.bV4Size = sizeof BITMAPINFOHEADER;
	LPBITMAPINFO lpbi = (LPBITMAPINFO) &bhdr;

	VERIFY(GetDIBits(ScreenDC, hbitmap, 0, bm.bmHeight,     
		NULL, lpbi, DIB_RGB_COLORS));
	bhdr.bV4SizeImage = ((bhdr.bV4SizeImage << 5) / (int) bhdr.bV4BitCount);
	bhdr.bV4BitCount = BITCOUNT32;
	pBits = (COLORREF*) malloc(bhdr.bV4SizeImage);
	ASSERT(pBits);
	// int rv =
	GetDIBits(ScreenDC, hbitmap, 0, bm.bmHeight,    
		(LPVOID) pBits, lpbi, DIB_RGB_COLORS);
	// CheckGetDibBits(rv);

	reset();
}

uRGBBits::~uRGBBits()
{
	if (!(flags & UBF_CANCEL))
	{
		bhdr.bV4RedMask =	0xff0000;
		bhdr.bV4GreenMask = 0xff00;
		bhdr.bV4BlueMask =	0xff;
		VERIFY(SetDIBits(ScreenDC, hbitmap, 0, bm.bmHeight,     
			pBits, (LPBITMAPINFO) &bhdr, DIB_RGB_COLORS));
	}

	ReleaseDC(NULL, ScreenDC);
	free(pBits);
}

void uRGBBits::reset()
{
	x = 0;
	y = 0;
	p = 0;
}
*/

