
#include "ufw.h"
#include "ucore.h"
#include "uskin.h"

/***************************** Global Functions ****************************/

#define UFONT2LOGFONT(lf, uf)				\
	lf.lfHeight = uf.height;				\
	lf.lfWeight = uf.weight;				\
	strcpy(lf.lfFaceName, uf.facename);	\
	lf.lfCharSet = DEFAULT_CHARSET;
// end of UFONT2LOGFONT

_inline int isXP()
{
	OSVERSIONINFO os = {sizeof OSVERSIONINFO};
	GetVersionEx(&os);
	return ((os.dwMajorVersion > 4) && (os.dwMinorVersion > 0)) ? 1 : 0;
}

int kxuRegisterWindowClass(LPCSTR lpszClassName)
{
	WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();

    if (!(::GetClassInfo(hInst, lpszClassName, &wndcls)))
    {
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNCLIENT;
        // FIXME - kludge
        if (isXP() && !strcmp("UFX-PluginContainer", lpszClassName))
            wndcls.style |= CS_DROPSHADOW;

        wndcls.lpfnWndProc      = DefWindowProc;
        wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor((LPCTSTR) IDC_ARROW);
        wndcls.hbrBackground    = NULL;
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = lpszClassName;
        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return false;
        }
    }
    return true;
}
		

/******************************* kxuCore *******************************/

/////////////////////////////////////////////////////////////////////////////
// kxuCore

extern HMODULE hmodule;

kxuCore::kxuCore()
{
	// Resource DLL
	ResourceLib = GetModuleHandle(RESOURCELIB);
	if (!ResourceLib) 
    {
        char path[MAX_PATH];
        GetModuleFileName(hmodule, path, sizeof(path));
        strcpy(strrchr(path, '\\') + 1, RESOURCELIB);
        ResourceLib = AfxLoadLibrary(path);
    }
	
	CDC MemDC;
    HGDIOBJ old = 0;
	MemDC.CreateCompatibleDC(NULL);

	// Fonts
	LOGFONT lf; 
	TEXTMETRIC tm;
	ZeroMemory(&lf, sizeof LOGFONT);
	for (int i = 0; i < _ufonts_count; i++)
	{
		UFONT2LOGFONT(lf, uFonts[i]);
		Font[i].CreateFontIndirect(&lf);
        HGDIOBJ o = MemDC.SelectObject(Font[i]);
        if (old == 0)
            old = o;
		MemDC.GetTextMetrics(&tm);
		AveCharWidth[i] = tm.tmAveCharWidth;
	}
    MemDC.SelectObject(old);
}

kxuCore::~kxuCore()
{
	// AfxFreeLibrary(ResourceLib);
}

// Local Functions

int	kxuCore::IsOK() {return (1 && ResourceLib);}

int kxuCore::GetIcon(HICON* icon)
{
	icon[0] = (HICON) LoadImage(ResourceLib, (LPCTSTR) IDI_DEFAULT, IMAGE_ICON, 16, 16, 0);
	icon[1] = (HICON) LoadImage(ResourceLib, (LPCTSTR) IDI_DEFAULT, IMAGE_ICON, 32, 32, 0);
	return 0;
}

void kxuCore::RenderPlainBitmap(CDC* pDstDC, LPRECT pDstRect, CBitmap* pSrcBitmap, ElementData* pRenderData)
{
	CDC TmpDC;

	// Create memory DCs
    if (!TmpDC.CreateCompatibleDC(pDstDC)) return;

	CBitmap* pTmpOld = TmpDC.SelectObject(pSrcBitmap);

	// Draw
	pDstDC->BitBlt(EXPANDRECTWH(*pDstRect),
					&TmpDC,
					EXPANDRECTLT(pRenderData->SrcRect),
					SRCCOPY);

	// Restore TmpDC
	TmpDC.SelectObject(pTmpOld);
}

void kxuCore::RenderMarginBitmap(CDC* pDstDC, LPRECT pDstRect, CBitmap* pSrcBitmap, ElementData* pRenderData)
{
	CDC TmpDC;
	RECT DstMargins;

	// Calc DstMargins
	DstMargins.left = pDstRect->left + (pRenderData->Margins.left - pRenderData->SrcRect.left);
	DstMargins.top = pDstRect->top + (pRenderData->Margins.top - pRenderData->SrcRect.top);
	DstMargins.right = pDstRect->right - (pRenderData->SrcRect.right - pRenderData->Margins.right);
	DstMargins.bottom = pDstRect->bottom - (pRenderData->SrcRect.bottom - pRenderData->Margins.bottom);

    // Create memory DC
    if (!TmpDC.CreateCompatibleDC(pDstDC)) return;

	CBitmap* pTmpOld = TmpDC.SelectObject(pSrcBitmap);

	// Draw Corners
	// LeftTop
	pDstDC->BitBlt(pDstRect->left,
					pDstRect->top,
					pRenderData->Margins.left - pRenderData->SrcRect.left,
					pRenderData->Margins.top - pRenderData->SrcRect.top,
					&TmpDC,
					pRenderData->SrcRect.left,
					pRenderData->SrcRect.top,
					SRCCOPY);

	// RightTop
	pDstDC->BitBlt(DstMargins.right,
					pDstRect->top,
					pRenderData->SrcRect.right - pRenderData->Margins.right,
					pRenderData->Margins.top - pRenderData->SrcRect.top,
					&TmpDC,
					pRenderData->Margins.right,
					pRenderData->SrcRect.top,
					SRCCOPY);

	// LeftBottom
	pDstDC->BitBlt(pDstRect->left,
					DstMargins.bottom,
					pRenderData->Margins.left - pRenderData->SrcRect.left,
					pRenderData->SrcRect.bottom - pRenderData->Margins.bottom,
					&TmpDC,
					pRenderData->SrcRect.left,
					pRenderData->Margins.bottom,
					SRCCOPY);

	// RightBottom
	pDstDC->BitBlt(DstMargins.right,
					DstMargins.bottom,
					pRenderData->SrcRect.right - pRenderData->Margins.right,
					pRenderData->SrcRect.bottom - pRenderData->Margins.bottom,
					&TmpDC,
					pRenderData->Margins.right,
					pRenderData->Margins.bottom,
					SRCCOPY);

	// Draw Space

	// Top
	kxuBitBlt((HDC) *pDstDC,
					DstMargins.left,
					pDstRect->top,
					DstMargins.right - DstMargins.left,
					pRenderData->Margins.top - pRenderData->SrcRect.top,
					(HDC) TmpDC,
					pRenderData->Margins.left,
					pRenderData->SrcRect.top,
					pRenderData->Margins.right - pRenderData->Margins.left,
					pRenderData->Margins.top - pRenderData->SrcRect.top,
					SRCCOPY); 
				
	// Bottom
	kxuBitBlt((HDC) *pDstDC,
					DstMargins.left,
					DstMargins.bottom,
					DstMargins.right - DstMargins.left,
					pRenderData->SrcRect.bottom - pRenderData->Margins.bottom,
					(HDC) TmpDC,
					pRenderData->Margins.left,
					pRenderData->Margins.bottom,
					pRenderData->Margins.right - pRenderData->Margins.left,
					pRenderData->SrcRect.bottom - pRenderData->Margins.bottom,
					SRCCOPY); 

	// Center
	kxuBitBlt((HDC) *pDstDC,
					DstMargins.left,
					DstMargins.top,
					DstMargins.right - DstMargins.left,
					DstMargins.bottom - DstMargins.top,
					(HDC) TmpDC,
					pRenderData->Margins.left,
					pRenderData->Margins.top,
					pRenderData->Margins.right - pRenderData->Margins.left,
					pRenderData->Margins.bottom - pRenderData->Margins.top,
					SRCCOPY);

	// Left
	kxuBitBlt((HDC) *pDstDC,
					pDstRect->left,
					DstMargins.top,
					DstMargins.left - pDstRect->left,
					DstMargins.bottom - DstMargins.top,
					(HDC) TmpDC,
					pRenderData->SrcRect.left,
					pRenderData->Margins.top,
					pRenderData->Margins.left - pRenderData->SrcRect.left,
					pRenderData->Margins.bottom - pRenderData->Margins.top,
					SRCCOPY); 

	// Right
	kxuBitBlt((HDC) *pDstDC,
					DstMargins.right,
					DstMargins.top,
					pRenderData->SrcRect.right - pRenderData->Margins.right,
					DstMargins.bottom - DstMargins.top,
					(HDC) TmpDC,
					pRenderData->Margins.right,
					pRenderData->Margins.top,
					pRenderData->SrcRect.right - pRenderData->Margins.right,
					pRenderData->Margins.bottom - pRenderData->Margins.top,
					SRCCOPY);

	// Restore TmpDC
	//trace("<kxu> RenderMarginBitmap - pTmpOld: %x\n", pTmpOld->m_hObject);
	TmpDC.SelectObject(pTmpOld);
}

/////////////////////////////////////////////////////////////////////////////
// kxuCore member functions

int kxuCore::CopyCBitmap(CBitmap* pDstBitmap, CBitmap* pSrcBitmap)
// fixme, this is very stupid way
{
	CWindowDC ScreenDC(NULL); // screen
	CDC DstDC, SrcDC;
	BITMAP bm;

	pSrcBitmap->GetBitmap(&bm);

	DstDC.CreateCompatibleDC(NULL);
	SrcDC.CreateCompatibleDC(NULL);

	if (pDstBitmap->m_hObject != NULL) pDstBitmap->DeleteObject();
	pDstBitmap->CreateCompatibleBitmap(&ScreenDC, bm.bmWidth, bm.bmHeight);
	DstDC.SelectObject(pDstBitmap);
	SrcDC.SelectObject(pSrcBitmap);

	DstDC.BitBlt(0, 0, bm.bmWidth, bm.bmHeight,
					&SrcDC,
					0, 0,
					SRCCOPY);
	return 0;
}

int kxuCore::GetControlBitmap(CBitmap* pDstBitmap, LONG /*Flags*/, int nItems, int* pId, RECT** ppDstRect)
{
	CWindowDC ScreenDC(NULL); // screen
	CDC MemDC;
	MemDC.CreateCompatibleDC(NULL);
	const BitmapElement* SrcElement;
	CBitmap SrcBitmap, *pDstDCOldBitmap;
	int i;
	
	HINSTANCE PrevResHandle = AfxGetResourceHandle();
	AfxSetResourceHandle(ResourceLib);

	RECT* pDstRect = (LPRECT) ppDstRect[0]; // first element defines bitmap size
	if (uElements[pId[0]].Type != BET_SOLID) kxuCorrectMinRect(pDstRect, (LPRECT) &uElements[pId[0]].Data.SrcRect);

	if (pDstBitmap->m_hObject != NULL) pDstBitmap->DeleteObject();
	pDstBitmap->CreateCompatibleBitmap(&ScreenDC, pDstRect->right, pDstRect->bottom);
	pDstDCOldBitmap = MemDC.SelectObject(pDstBitmap);

	/* Draw Redness for testing */
	#ifdef _DEBUG 
		MemDC.FillSolidRect(pDstRect, 0xff);
	#endif
	
	// Render
	for (i = 0; i < nItems; i++)
	{
		SrcElement = &uElements[pId[i]];
		pDstRect = ppDstRect[i];

		switch (SrcElement->Type & BET_TYPE_MASK)
		{
		case BET_SOLID:
			MemDC.FillSolidRect(pDstRect, SrcElement->Resource);
			break;

		case BET_PLAINBITMAP:
		case BET_FILMBITMAP:
			SrcBitmap.LoadBitmap(SrcElement->Resource);
			RenderPlainBitmap(&MemDC, pDstRect, &SrcBitmap, (ElementData*) &SrcElement->Data);
			SrcBitmap.DeleteObject();
			break;

		case BET_MARGINBITMAP:
			kxuCorrectMinRect(pDstRect, (LPRECT) &SrcElement->Data.SrcRect);
			SrcBitmap.LoadBitmap(SrcElement->Resource);
			RenderMarginBitmap(&MemDC, pDstRect, &SrcBitmap, (ElementData*) &SrcElement->Data);
			SrcBitmap.DeleteObject();
			break;
		}
	}
	
	//trace("<kxu> GetControlBitmap - pDstDCOldBitmap: %x\n", pDstDCOldBitmap->m_hObject);
	MemDC.SelectObject(pDstDCOldBitmap);

	AfxSetResourceHandle(PrevResHandle);
	return 0;
}

COLORREF kxuCore::GetColor(int Id) 
{
	return uColors[Id];
}

int kxuCore::GetElementData(int Id, int Index) 
{
	switch (Index)
	{
	case UED_SIZEWIDTH:
	case UED_SIZEHEIGHT:
		return ((int*) &uElements[Id])[Index] - ((int*) &uElements[Id])[Index - 2];
	default:
		return ((int*) &uElements[Id])[Index];
	}
}

int kxuCore::GetElementData(int Id, int Index, int Size, LPVOID pData)
{
	RECT srcrect;
	LPVOID src;
	if (pData == 0) return -1;

	switch (Index)
	{
	case UED_SIZERECT:
		// convert source rect to size rect  
		RECTTOSIZE(srcrect, uElements[Id].Data.SrcRect);
		src = (LPVOID) &srcrect;
		break;
	default:
		src = (LPVOID) &(((int*) &uElements[Id])[Index]);
	}
	memcpy(pData, src, Size);
	return 0;
}




