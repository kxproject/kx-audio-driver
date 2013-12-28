// kX GUI
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


#include "stdafx.h"

#include "vers.h"

#ifdef _DEBUG
 #define new DEBUG_NEW
 #undef THIS_FILE
 static char THIS_FILE[] = __FILE__;
#endif

static HICON default_icon=0;
static int _gui_has_sfx=0;
BOOL (WINAPI *_set_layered_window_attributes)(HWND hwnd,COLORREF crKey,BYTE bAlpha,DWORD dwFlags)=0;
static HMODULE user32=NULL;

HRGN gui_bitmap_region(HBITMAP hBitmap,COLORREF cTransparentColor,BOOL bIsTransparent)
{
	// We create an empty region
	HRGN		hRegion=NULL;

	// If the passed bitmap is NULL, go away!
	if(!hBitmap)
		return hRegion;

	// We create a memory context for working with the bitmap
	// The memory context is compatible with the display context (screen)
	HDC			hMemDC=CreateCompatibleDC(NULL);

	// If no context is created, go away, too!
	if(!hMemDC)
		return hRegion;

	// Computation of the bitmap size
	BITMAP		bmBitmap;

	GetObject(hBitmap, sizeof(bmBitmap), &bmBitmap);

	// In order to make the space for the region, we
	// create a bitmap with 32bit depth color and with the
	// size of the loaded bitmap!
	BITMAPINFOHEADER RGB32BITSBITMAPINFO=
	{
		sizeof(BITMAPINFOHEADER),
		bmBitmap.bmWidth,
		bmBitmap.bmHeight,
		1,32,BI_RGB,0,0,0,0,0
	};

	// Here is the pointer to the bitmap data
	VOID		*pBits;

	// With the previous information, we create the new bitmap!
	HBITMAP		hNewBitmap;
	hNewBitmap=CreateDIBSection(hMemDC,
								(BITMAPINFO *)&RGB32BITSBITMAPINFO,
								DIB_RGB_COLORS,&pBits,NULL,0);

	// If the creation process succeded...
	if(hNewBitmap)
	{
		// We select the bitmap onto the created memory context
		// and then we store the previosly selected bitmap on this context!
		HBITMAP		hPrevBmp=(HBITMAP) SelectObject(hMemDC,hNewBitmap);

		// We create another device context compatible with the first!
		HDC			hDC=CreateCompatibleDC(hMemDC);

		// If success...
		if(hDC)
		{
			// We compute the number of bytes per row that the bitmap contains, rounding to 32 bit-multiples
			BITMAP		bmNewBitmap;

			GetObject(hNewBitmap,sizeof(bmNewBitmap),&bmNewBitmap);

			while(bmNewBitmap.bmWidthBytes % 4)
				bmNewBitmap.bmWidthBytes++;

			// Copy of the original bitmap on the memory context!
			HBITMAP		hPrevBmpOrg=(HBITMAP) SelectObject(hDC,hBitmap);
			BitBlt(hMemDC,0,0,bmBitmap.bmWidth,bmBitmap.bmHeight,hDC,0,0,SRCCOPY);

			// In order to optimize the code, we don't call the GDI each time we
			// find a transparent pixel. We use a RGN_DATA structure were we store
			// consecutive rectangles, until we have a large amount of them and then we crete
			// the composed region with ExtCreateRgn(), combining it with the main region.
			// Then we begin again initializing the RGN_DATA structure and doing another
			// iteration, until the entire bitmap is analyzed.

			// Also, in order to not saturate the Windows API with calls for reserving
			// memory, we wait until NUMRECT rectangles are stores in order to claim
			// for another NUMRECT memory space!
            #define NUMRECT	100
			DWORD maxRect = NUMRECT;

			// We create the memory data
			HANDLE hData=GlobalAlloc(GMEM_MOVEABLE,sizeof(RGNDATAHEADER)+(sizeof(RECT)*maxRect));
			RGNDATA *pData=(RGNDATA*) GlobalLock(hData);
			pData->rdh.dwSize=sizeof(RGNDATAHEADER);
			pData->rdh.iType=RDH_RECTANGLES;
			pData->rdh.nCount=pData->rdh.nRgnSize=0;
			SetRect(&pData->rdh.rcBound,MAXLONG,MAXLONG,0,0);

			// We study each pixel on the bitmap...
			BYTE *Pixeles=(BYTE*) bmNewBitmap.bmBits+(bmNewBitmap.bmHeight-1)*bmNewBitmap.bmWidthBytes;

			// Main loop
			for(int Row=0;Row<bmBitmap.bmHeight;Row++)
			{
				// Horizontal loop
				for(int Column=0;Column<bmBitmap.bmWidth;Column++)
				{
					// We optimized searching for adyacent transparent pixels!
					int Xo=Column;
					LONG *Pixel=(LONG*) Pixeles+Column;

					while(Column<bmBitmap.bmWidth)
					{
						BOOL bInRange=FALSE;

						// If the color is that indicated as transparent...
						if(	GetRValue(*Pixel)==GetRValue(cTransparentColor) &&
							GetGValue(*Pixel)==GetGValue(cTransparentColor) &&
							GetBValue(*Pixel)==GetBValue(cTransparentColor) )
							bInRange=TRUE;

						if((bIsTransparent) && (bInRange))
							break;

						if((!bIsTransparent) && (!bInRange))
							break;

						Pixel++;
						Column++;
					} // while (Column < bm.bmWidth)

					if(Column>Xo)
					{
						// We add the rectangle (Xo,Row),(Column,Row+1) to the region

						// If the number of rectangles is greater then NUMRECT, we claim
						// another pack of NUMRECT memory places!
						if (pData->rdh.nCount>=maxRect)
						{
							GlobalUnlock(hData);
							maxRect+=NUMRECT;
							hData=GlobalReAlloc(hData,sizeof(RGNDATAHEADER)+(sizeof(RECT)*maxRect),GMEM_MOVEABLE);
							pData=(RGNDATA *)GlobalLock(hData);
						}

						RECT *pRect=(RECT*) &pData->Buffer;
						SetRect(&pRect[pData->rdh.nCount],Xo,Row,Column,Row+1);

						if(Xo<pData->rdh.rcBound.left)
							pData->rdh.rcBound.left=Xo;

						if(Row<pData->rdh.rcBound.top)
							pData->rdh.rcBound.top=Row;

						if(Column>pData->rdh.rcBound.right)
							pData->rdh.rcBound.right=Column;

						if(Row+1>pData->rdh.rcBound.bottom)
							pData->rdh.rcBound.bottom=Row+1;

						pData->rdh.nCount++;

						// In Win95/08 there is a limitation on the maximum number of
						// rectangles a RGN_DATA can store (aprox. 4500), so we call
						// the API for a creation and combination with the main region
						// each 2000 rectangles. This is a good optimization, because
						// instead of calling the routines for combining for each new
						// rectangle found, we call them every 2000 rectangles!!!
						if(pData->rdh.nCount==2000)
						{
							HRGN hNewRegion=ExtCreateRegion(NULL,sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRect),pData);
							if (hNewRegion) {
								// Si ya existe la región principal,sumamos la nueva,
								// si no,entonces de momento la principal coincide con
								// la nueva región.
								if (hRegion) {
									CombineRgn(hRegion,hRegion,hNewRegion,RGN_OR);
									DeleteObject(hNewRegion);
								} else
									hRegion=hNewRegion;


							}
							// Volvemos a comenzar la suma de rectángulos
							pData->rdh.nCount=0;
							SetRect(&pData->rdh.rcBound,MAXLONG,MAXLONG,0,0);
						}

					} // if (Column > Xo)
				} // for (int  Column ...)

				// Nueva Row. Lo del negativo se debe a que el bitmap está invertido
				// verticalmente.
				Pixeles -= bmNewBitmap.bmWidthBytes;

			} // for (int Row...)

			// Una vez finalizado el proceso,procedemos a la fusión de la
			// región remanente desde la última fusión hasta el final
			HRGN hNewRegion=ExtCreateRegion(NULL,sizeof(RGNDATAHEADER)+(sizeof(RECT)*maxRect),pData);

			if(hNewRegion)
			{
				// If the main region does already exist, we add the new one,
				if(hRegion)
				{
					CombineRgn(hRegion,hRegion,hNewRegion,RGN_OR);
					DeleteObject(hNewRegion);
				}
				else
					// if not, we consider the new one to be the main region at first!
					hRegion=hNewRegion;
			}

			// We free the allocated memory and the rest of used ressources
			GlobalUnlock(hData);
			SelectObject(hDC,hPrevBmpOrg);
			DeleteDC(hDC);
                        GlobalFree(hData);
		}// if (hDC)

		SelectObject(hMemDC,hPrevBmp);
		DeleteDC(hMemDC);

		DeleteObject(hNewBitmap);
	} //if (hNewBitmap)

	return hRegion;
}


int gui_create_button(kDialog *parent,kButton *b,UINT id,TCHAR *param,kFile &mf)
{
 TCHAR ref_button[128]; ref_button[0]=0;
 TCHAR tmp_str[512];
 int x=-1,y=-1;

 _stscanf(param,_T("%s %d %d"),&ref_button[0],&x,&y);

 kRect r_w;
 parent->get_rect(r_w);

 if(x<0)
  x=r_w.width()+x;

 if(y<0)
  y=r_w.height()+y; 

 int checkbox=0;

 if(_tcscmp(b->get_class_name(),_T("kGuiCheckButton"))==0)
  checkbox=1;
 
 int type=1;
 if(!mf.get_profile(ref_button,_T("type"),tmp_str,sizeof(tmp_str)))
 {
  if(_tcsstr(tmp_str,_T("skin"))==0) type=0;
 }
 else
  if(!mf.get_profile(_T("buttons.default"),_T("type"),tmp_str,sizeof(tmp_str)))
  {
   if(_tcsstr(tmp_str,_T("skin"))==0) type=0;
  }

 kRect r;
 r.left=x; r.top=y; r.right=r.left+32; r.bottom=r.top+32;

 if(type==1)
 {
  TCHAR tooltip[512]; _tcscpy(tooltip,_T(""));
  // 'tooltip' is not located in 'buttons.default'
  if(!mf.get_profile(ref_button,_T("tooltip"),tmp_str,sizeof(tmp_str)))
  {
   _tcscpy(tooltip,tmp_str);
  }

  TCHAR bm1[64],bm2[64],bm3[64],bm4[64],bm5[64];
  bm1[0]=0; bm2[0]=0; bm3[0]=0; bm4[0]=0; bm5[0]=0;
  // 'image' is not located in 'buttons.default'
  if(!mf.get_profile(ref_button,_T("image"),tmp_str,sizeof(tmp_str)))
  {
   _stscanf(tmp_str,_T("%s %s %s %s %s"),&bm1[0],&bm2[0],&bm3[0],&bm4[0],&bm5[0]);
   HBITMAP bm_n,bm_o,bm_s,bm_def,bm_dis;
   bm_n=bm1[0]?mf.load_image(bm1):0;
   bm_o=bm2[0]?mf.load_image(bm2):0;
   bm_s=bm3[0]?mf.load_image(bm3):0;
   bm_def=bm4[0]?mf.load_image(bm4):0;
   bm_dis=bm5[0]?mf.load_image(bm5):0;
 
   if(bm_n==0)
   {
        debug(_T("kxgui: gui_create_button failed\n"));
        kString e1,e2;
        mf.get_profile(_T("error"),_T("image"),_T("Incorrect image specified for button"),e1);
        mf.get_profile(_T("error"),_T("severe"),_T("Severe error"),e2);
        ::MessageBox(parent->get_wnd(),(LPCTSTR)e1,(LPCTSTR)e2,MB_OK|MB_ICONSTOP);
        exit(32);
   }

   // check if it is a checkbox

   if(checkbox)
   {
      ((kCheckButton *)b)->set_bitmaps(bm_n,bm_o,bm_s,bm_def,bm_dis,bm_dis);
   }
   else
   {
      if(bm_o==0) bm_o=bm_n;
      if(bm_s==0) bm_s=bm_n;
      b->set_bitmaps(bm_n,bm_o,bm_s,bm_def,bm_dis);
   }
  }
  int tmp_method=0;
  if(!mf.get_profile(ref_button,_T("method"),tmp_str,sizeof(tmp_str)))
  {
   if(_tcsstr(tmp_str,_T("blend"))) { tmp_method|=kMETHOD_BLEND; }
   if(_tcsstr(tmp_str,_T("trans"))) { tmp_method|=kMETHOD_TRANS; }
  }
  else
    if(!mf.get_profile(_T("buttons.default"),_T("method"),tmp_str,sizeof(tmp_str)))
    {
     if(_tcsstr(tmp_str,_T("blend"))) { tmp_method|=kMETHOD_BLEND; }
     if(_tcsstr(tmp_str,_T("trans"))) { tmp_method|=kMETHOD_TRANS; }
    }

  if(tmp_method==0) tmp_method=kMETHOD_COPY;

  if(checkbox)
   ((kCheckButton *)b)->create(tooltip[0]?tooltip:NULL,r,parent,id,tmp_method,0);
  else
   b->create(tooltip[0]?tooltip:NULL,type?BS_OWNERDRAW:0,r,parent,id,tmp_method);

  // after create:
  if(!mf.get_profile(ref_button,_T("alpha"),tmp_str,sizeof(tmp_str)))
  {
   int alpha=90;
   _stscanf(tmp_str,_T("%d"),&alpha);
   b->set_alpha(alpha);
  }
  else
    if(!mf.get_profile(_T("buttons.default"),_T("alpha"),tmp_str,sizeof(tmp_str)))
    {
     int alpha=90;
     _stscanf(tmp_str,_T("%d"),&alpha);
     b->set_alpha(alpha);
    }

  if(!mf.get_profile(ref_button,_T("transp_color"),tmp_str,sizeof(tmp_str)))
  {
   unsigned int t_c=RGB(0xff,0xff,0xff);
   _stscanf(tmp_str,_T("%x"),&t_c);
   b->set_transpcolor(t_c);
  }
  else
    if(!mf.get_profile(_T("buttons.default"),_T("transp_color"),tmp_str,sizeof(tmp_str)))
    {
     unsigned int t_c=RGB(0xff,0xff,0xff);
     _stscanf(tmp_str,_T("%x"),&t_c);
     b->set_transpcolor(t_c);
    }

  if(bm1[0])
   b->size_to_content();
 }
 else // normal button
 {
  TCHAR caption[64]; _tcscpy(caption,_T("<none>"));
  int wd=30;
  int ht=10;
  if(!mf.get_profile(ref_button,_T("caption"),tmp_str,sizeof(tmp_str)))
  {
   _tcsncpy(caption,tmp_str,sizeof(caption));
  }
  if(!mf.get_profile(ref_button,_T("wd"),tmp_str,sizeof(tmp_str)))
  {
    _stscanf(tmp_str,_T("%d"),&wd); r.right=r.left+wd;
  }
  else
    if(!mf.get_profile(_T("buttons.default"),_T("wd"),tmp_str,sizeof(tmp_str)))
    {
      _stscanf(tmp_str,_T("%d"),&wd); r.right=r.left+wd;
    }
   
  if(!mf.get_profile(ref_button,_T("ht"),tmp_str,sizeof(tmp_str)))
  {
    _stscanf(tmp_str,_T("%d"),&ht); r.bottom=r.top+ht;
  }
  else
    if(!mf.get_profile(_T("buttons.default"),_T("ht"),tmp_str,sizeof(tmp_str)))
    {
      _stscanf(tmp_str,_T("%d"),&ht); r.bottom=r.top+ht;
    }

  if(checkbox)
   ((kCheckButton *)b)->create((LPCTSTR)caption,r,parent,(UINT)id,kMETHOD_DEFAULT,0);
  else
   b->create((LPCTSTR)caption,(DWORD)BS_PUSHBUTTON,r,parent,(UINT)id,kMETHOD_DEFAULT);

  if(!mf.get_profile(ref_button,_T("tooltip"),tmp_str,sizeof(tmp_str)))
  {
   parent->add_tool(tmp_str,b,id);
  }
 }

 return 0;
}

int gui_set_background(kDialog *d,kFile *mf,const TCHAR *param,unsigned int &transp_color,
     unsigned int &alpha,unsigned int &cc_method,const TCHAR *def_bk)
{
   TCHAR tmp_image[128];
   _tcsncpy(tmp_image,def_bk,sizeof(tmp_image)/sizeof(tmp_image[0]));

   _stscanf(param,_T("%s %x %d"),tmp_image,&transp_color,&alpha);
   if(_tcsstr(param,_T(" blend"))) cc_method|=kMETHOD_BLEND;
   if(_tcsstr(param,_T(" trans"))) cc_method|=kMETHOD_TRANS;

   if(cc_method==0) cc_method=kMETHOD_COPY;

   int need_resize=1;

   if(_tcsstr(param,_T(" tiled"))) 
   {
    cc_method|=kMETHOD_TILED;
    int x=100,y=50;
    char *pos=_tcsstr(param,_T("size="));
    if(pos)
    {
     _stscanf(pos+5,_T("%d %d"),&x,&y);
     d->resize(kPoint(x,y));
     need_resize=0;
    }
   }

  if(mf)
   d->set_background(mf->load_image(tmp_image),transp_color,alpha,cc_method);

  if(!(cc_method&kMETHOD_TILED) || need_resize)
    d->size_to_content();

  return 0;
}


int gui_register_window_class(TCHAR *lpszClassName)
{
	WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();

    if (!(::GetClassInfo(hInst, lpszClassName, &wndcls)))
    {
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNCLIENT;
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

void gui_transparent_blit(HDC hdc, HBITMAP hBitmap, short xStart,
                           short yStart, COLORREF cTransparentColor)
{
   BITMAP     bm;
   COLORREF   cColor;
   HBITMAP    bmAndBack, bmAndObject, bmAndMem, bmSave;
   HBITMAP    bmBackOld, bmObjectOld, bmMemOld, bmSaveOld;
   HDC        hdcMem, hdcBack, hdcObject, hdcTemp, hdcSave;
   POINT      ptSize;

   hdcTemp = CreateCompatibleDC(hdc);
   SelectObject(hdcTemp, hBitmap);   // Select the bitmap

   GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);
   ptSize.x = bm.bmWidth;            // Get width of bitmap
   ptSize.y = bm.bmHeight;           // Get height of bitmap
   DPtoLP(hdcTemp, &ptSize, 1);      // Convert from device

                                     // to logical points

   // Create some DCs to hold temporary data.
   hdcBack   = CreateCompatibleDC(hdc);
   hdcObject = CreateCompatibleDC(hdc);
   hdcMem    = CreateCompatibleDC(hdc);
   hdcSave   = CreateCompatibleDC(hdc);

   // Create a bitmap for each DC. DCs are required for a number of
   // GDI functions.

   // Monochrome DC
   bmAndBack   = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);

   // Monochrome DC
   bmAndObject = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);

   bmAndMem    = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);
   bmSave      = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);

   // Each DC must select a bitmap object to store pixel data.
   bmBackOld   = (HBITMAP)SelectObject(hdcBack, bmAndBack);
   bmObjectOld = (HBITMAP)SelectObject(hdcObject, bmAndObject);
   bmMemOld    = (HBITMAP)SelectObject(hdcMem, bmAndMem);
   bmSaveOld   = (HBITMAP)SelectObject(hdcSave, bmSave);

   // Set proper mapping mode.
   SetMapMode(hdcTemp, GetMapMode(hdc));

   // Save the bitmap sent here, because it will be overwritten.
   BitBlt(hdcSave, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);

   // Set the background color of the source DC to the color.
   // contained in the parts of the bitmap that should be transparent
   cColor = SetBkColor(hdcTemp, cTransparentColor);

   // Create the object mask for the bitmap by performing a BitBlt
   // from the source bitmap to a monochrome bitmap.
   BitBlt(hdcObject, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0,
          SRCCOPY);

   // Set the background color of the source DC back to the original
   // color.
   SetBkColor(hdcTemp, cColor);

   // Create the inverse of the object mask.
   BitBlt(hdcBack, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0,
          NOTSRCCOPY);

   // Copy the background of the main DC to the destination.
   BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdc, xStart, yStart,
          SRCCOPY);

   // Mask out the places where the bitmap will be placed.
   BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, SRCAND);

   // Mask out the transparent colored pixels on the bitmap.
   BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcBack, 0, 0, SRCAND);

   // XOR the bitmap with the background on the destination DC.
   BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCPAINT);

   // Copy the destination to the screen.
   BitBlt(hdc, xStart, yStart, ptSize.x, ptSize.y, hdcMem, 0, 0,
          SRCCOPY);

   // Place the original bitmap back into the bitmap sent here.
   BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcSave, 0, 0, SRCCOPY);

   // Delete the memory bitmaps.
   DeleteObject(SelectObject(hdcBack, bmBackOld));
   DeleteObject(SelectObject(hdcObject, bmObjectOld));
   DeleteObject(SelectObject(hdcMem, bmMemOld));
   DeleteObject(SelectObject(hdcSave, bmSaveOld));

   // Delete the memory DCs.
   DeleteDC(hdcMem);
   DeleteDC(hdcBack);
   DeleteDC(hdcObject);
   DeleteDC(hdcSave);
   DeleteDC(hdcTemp);
}

int kCLASS_TYPE gui_get_version()
{
 gui_has_sfx(); // generic init here

 return KX_VERSION_DWORD;
}

static int gui_inited=0;
static int force_gui_balloon_tooltip=0;

int gui_dll_versions[3]={0,0,0};
int gui_balloon_tooltip=0;
int gui_show_content=1;

int kCLASS_TYPE gui_set_show_content(int set)
{
 gui_show_content=set;
 return 0;
}

int kCLASS_TYPE gui_get_show_content(void)
{
 return gui_show_content;
}

typedef HRESULT (CALLBACK *_DllGetVersion)(DLLVERSIONINFO* pdvi);

int kCLASS_TYPE gui_set_tooltip_type(int t)
{
 if(t)
  gui_balloon_tooltip=1;
 else
  gui_balloon_tooltip=0;

 if(force_gui_balloon_tooltip)
  gui_balloon_tooltip=0;

 return 0;
}

int gui_init_environtment()
{
 if(gui_inited==0)
 {
  gui_inited=1;

  {
   HKEY hk;
   if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"),NULL,KEY_ALL_ACCESS,&hk)==ERROR_SUCCESS)
   {
    DWORD v=1;
    DWORD type=REG_DWORD;
    DWORD data_buffer=sizeof(DWORD);

    if(RegQueryValueEx(hk,"EnableBalloonTips",NULL,&type,(LPBYTE)&v,&data_buffer)==ERROR_SUCCESS)
    {
     if(v==0)
      force_gui_balloon_tooltip=1;
    }
    
    RegCloseKey(hk);
   }
   if(RegOpenKeyEx(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"),NULL,KEY_ALL_ACCESS,&hk)==ERROR_SUCCESS)
   {
    DWORD v=1;
    DWORD type=REG_DWORD;
    DWORD data_buffer=sizeof(DWORD);

    if(RegQueryValueEx(hk,"EnableBalloonTips",NULL,&type,(LPBYTE)&v,&data_buffer)==ERROR_SUCCESS)
    {
     if(v==0)
      force_gui_balloon_tooltip=1;
    }
    
    RegCloseKey(hk);
   }
  }

  HMODULE shlwapi,shell32,comctl32;
  shlwapi=LoadLibrary("shlwapi.dll");
  shell32=LoadLibrary("shell32.dll");
  comctl32=LoadLibrary("comctl32.dll");

  _DllGetVersion gv;
  DLLVERSIONINFO vi;

  if(shlwapi)
  {
   gv=(_DllGetVersion)GetProcAddress(shlwapi,"DllGetVersion");
   if(gv)
   {
    try
    {
     vi.cbSize=sizeof(vi);
     if(gv(&vi)==NOERROR)
     {
      gui_dll_versions[0]=((vi.dwMajorVersion&0xffff)<<16)|(vi.dwMinorVersion&0xffff);
     }
    }
    catch(...)
    {
     debug("kxgui: error calling DLLGetVersion for shlwapi.dll\n");
    }
   }   
   FreeLibrary(shlwapi);
  }
  if(shell32)
  {
   gv=(_DllGetVersion)GetProcAddress(shell32,"DllGetVersion");
   if(gv)
   {
    try
    {
     vi.cbSize=sizeof(vi);
     if(gv(&vi)==NOERROR)
     {
      gui_dll_versions[1]=((vi.dwMajorVersion&0xffff)<<16)|(vi.dwMinorVersion&0xffff);
     }
    }
    catch(...)
    {
     debug("kxgui: error calling DLLGetVersion for shlwapi.dll\n");
    }
   }   
   FreeLibrary(shell32);
  }
  if(comctl32)
  {
   gv=(_DllGetVersion)GetProcAddress(comctl32,"DllGetVersion");
   if(gv)
   {
    try
    {
     vi.cbSize=sizeof(vi);
     if(gv(&vi)==NOERROR)
     {
      gui_dll_versions[2]=((vi.dwMajorVersion&0xffff)<<16)|(vi.dwMinorVersion&0xffff);
     }
    }
    catch(...)
    {
     debug("kxgui: error calling DLLGetVersion for shlwapi.dll\n");
    }
   }   
   FreeLibrary(comctl32);
  }

  // debug("kxgui: DLL versions: %x %x %x\n",gui_dll_versions[0],gui_dll_versions[1],gui_dll_versions[2]);
 }
 return 0; 
}

int gui_has_sfx()
{
 // first of all, perform some additional tests here...
 gui_init_environtment();

 if(user32==0)
 {
    user32=GetModuleHandle(TEXT("user32.dll"));
    _set_layered_window_attributes=(int (__stdcall *)(HWND,ULONG,UCHAR,ULONG))GetProcAddress(user32,"SetLayeredWindowAttributes");
    if(_set_layered_window_attributes)
    {
     TCHAR *cmd=GetCommandLine();
     if(_tcsstr(cmd,_T("--no2k"))==NULL)
      _gui_has_sfx=1;
     else
      _set_layered_window_attributes=NULL;
    }
    else
     _gui_has_sfx=0;
 }

 return _gui_has_sfx;
}


void gui_set_default_icon(HICON ic)
{ 
 default_icon=ic; 
}

HICON gui_get_default_icon()
{
 return default_icon;
}

void kFont::set_font(kWindow *w,HFONT fnt,int flag_)
 { font=fnt; if(w) ::SendMessage(w->get_wnd(),WM_SETFONT, (WPARAM)font, TRUE); flag=flag_; }

void kFont::set_font(kWindow *w,kFont &f,int flag_)
  { font=(HFONT)f; set_font(w,font,flag_); }

// FIXME: resource leakage

int gui_create_point_font(kFont &f,kFile &mf,const TCHAR *section,const TCHAR *key,const TCHAR *default_params,HDC dc)
{
	 TCHAR tmp_font_str[128]; _tcscpy(tmp_font_str,default_params);
	 int font_size=90;
	 _stscanf(tmp_font_str,_T("%d"),&font_size);

	 TCHAR font_name[64]; _tcscpy(font_name,_T("Tahoma"));
         TCHAR *p=_tcschr(tmp_font_str,' ');
         if(p)
          _tcsncpy(font_name,p+1,sizeof(font_name));

	 if(!mf.get_profile(_T("all"),_T("font"),tmp_font_str,sizeof(tmp_font_str)))
	 {
	  _stscanf(tmp_font_str,_T("%d"),&font_size);
          TCHAR *p=_tcschr(tmp_font_str,' ');
          if(p)
           _tcsncpy(font_name,p+1,sizeof(font_name));
	 }

	 if(!mf.get_profile(section,key,tmp_font_str,sizeof(tmp_font_str)))
	 {
	  _stscanf(tmp_font_str,_T("%d"),&font_size);
          TCHAR *p=_tcschr(tmp_font_str,' ');
          if(p)
           _tcsncpy(font_name,p+1,sizeof(font_name));
	 }

  	LOGFONT logFont;
	memset(&logFont, 0, sizeof(LOGFONT));
	logFont.lfCharSet = DEFAULT_CHARSET;
	// logFont.lfCharSet = DEFAULT_CHARSET;
        // logFont.lfCharSet=EASTEUROPE_CHARSET;
        // logFont.lfCharSet=GREEK_CHARSET;
        // logFont.lfCharSet=RUSSIAN_CHARSET;
	// logFont.lfCharSet=CHINESEBIG5_CHARSET;

	_tcsncpy(logFont.lfFaceName, font_name, sizeof(logFont.lfFaceName));

	POINT pt;
	pt.y = ::GetDeviceCaps(dc, LOGPIXELSY) * font_size;
	pt.y /= 720;    // 72 points/inch, 10 decipoints/point
	::DPtoLP(dc, &pt, 1);
	POINT ptOrg = { 0, 0 };
	::DPtoLP(dc, &ptOrg, 1);
	logFont.lfHeight = -abs(pt.y - ptOrg.y);

	f.set_font(NULL,CreateFontIndirect(&logFont),1);
	return f.get_font()?0:-1;
}

int kRect::point_in_rect(const kPoint &pt)
{
 POINT ptt; ptt.x=pt.x; ptt.y=pt.y;
 return PtInRect((RECT *)&x1,ptt);
}
