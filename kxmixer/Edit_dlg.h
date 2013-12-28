// kX Mixer
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


#if !defined(AFX_EDITDIALOG_H__B294EF63_CE1F_4020_B1F8_573626C3E572__INCLUDED_)
#define AFX_EDITDIALOG_H__B294EF63_CE1F_4020_B1F8_573626C3E572__INCLUDED_

#pragma once

class CDSPEditor
{
public:
 CCodeMaxCtrl *cm;
 CEdit *ke;

 ~CDSPEditor() { if(cm) { delete cm; cm=NULL; } if(ke) { delete ke; ke=NULL; };  };

 int show() { if(cm) cm->ShowWindow(SW_SHOW); else ke->ShowWindow(SW_SHOW); return 1; };
 int hide() { if(cm) cm->ShowWindow(SW_HIDE); else ke->ShowWindow(SW_HIDE); return 1; };

 BOOL Create(DWORD dwStyle, const CRect& rect, CWnd *pParentWnd, UINT nID )
  { 
   if(cm) 
    return cm->Create( dwStyle, rect, pParentWnd, nID); 
   else
    return ke->Create( dwStyle|ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_MULTILINE|ES_WANTRETURN, rect, pParentWnd, nID);
  };
 int SetBorderStyle(DWORD dwStyle)
  {
   if(cm)
    return (int)cm->SetBorderStyle(dwStyle);
   else
    return 1;
  };
 int SetLanguage(LPCTSTR pszName)
  {
   if(cm)
    return (int)cm->SetLanguage(pszName);
   else
    return 1;
  };
 int SetReadOnly(BOOL bReadOnly = TRUE )
  {
   if(cm)
    return (int)cm->SetReadOnly(bReadOnly);
   else
    return ke->SetReadOnly(bReadOnly);
  };
 int SetCaretPos( int nLine, int nCol )
  {
   if(cm)
    return (int)cm->SetCaretPos(nLine,nCol);
   else
   {
    POINT pt; pt.y=nLine; pt.x=nCol;
    ke->SetCaretPos(pt);
    return 1;
   }
  };
 int SetColors(const CM_COLORS *pColors)
  {
   if(cm)
    return (int)cm->SetColors(pColors);
   else
    return 1;
  };
 int EnableTabExpand( BOOL bEnable = TRUE )
  {
   if(cm)
    return (int)cm->EnableTabExpand(bEnable);
   else
    return 1;
  };
 int SetUndoLimit(int nLimit )
  {
   if(cm)
    return (int)cm->SetUndoLimit(nLimit);
   else
    return 1;
  };
 int EnableColorSyntax(BOOL bEnable = TRUE )
  {
   if(cm)
    return (int)cm->EnableColorSyntax(bEnable);
   else
    return 1;
  };
 int EnableDragDrop(BOOL bEnable = TRUE )
  {
   if(cm)
    return (int)cm->EnableDragDrop(bEnable);
   else
    return 1;
  };
 int SetAutoIndentMode(int nMode )
  {
   if(cm)
    return (int)cm->SetAutoIndentMode(nMode);
   else
    return 1;
  };
 int SetTabSize( int nTabSize )
  {
   if(cm)
    return (int)cm->SetTabSize(nTabSize);
   else
    return 1;
  };
 int EnableSplitter(BOOL bHorz, BOOL bEnable = TRUE )
  {
   if(cm)
    return (int)cm->EnableSplitter(bHorz,bEnable);
   else
    return 1;
  };
 int EnableLeftMargin(BOOL bEnable = TRUE )
  {
   if(cm)
    return (int)cm->EnableLeftMargin(bEnable);
   else
    return 1;
  };
 int EnableSmoothScrolling( BOOL bEnable = TRUE )
  {
   if(cm)
    return (int)cm->EnableSmoothScrolling(bEnable);
   else
    return 1;
  };
 int EnableColumnSel(BOOL bEnable = TRUE )
  {
   if(cm)
    return (int)cm->EnableColumnSel(bEnable);
   else
    return 1;
  };
 int EnableNormalizeCase( BOOL bEnable = TRUE )
  {
   if(cm)
    return (int)cm->EnableNormalizeCase(bEnable);
   else
    return 1;
  };
 int EnableCaseSensitive( BOOL bEnable = TRUE )
  {
   if(cm)
    return (int)cm->EnableCaseSensitive(bEnable);
   else
    return 1;
  };
 int SetFocus()
  {
   if(cm)
    cm->SetFocus();
   else
    ke->SetFocus();

   return 1;
  };

 int GetText( kString &strText, const CM_RANGE *pRange = NULL );
 int AddText( LPCTSTR pszText );
 int SetText( LPCTSTR pszText );
 int ExecuteCmd( WORD wCmd, DWORD dwCmdData = 0 );
 CDSPEditor() ;
 int OpenFile( LPCTSTR pszFileName );
 int SaveFile(LPCTSTR pszFileName, BOOL bClearUndo = TRUE);
};

class CEditDialog: public CKXDialog
{
public:
        CEditDialog();
        ~CEditDialog();

        const char *get_class_name() { return "kXEditor"; };

	int pgm_id;

	char file_name[512];

	virtual void init();

        // CodeMax editor
        CDSPEditor *codemax;
        CDSPEditor *message;

        kButton b_return;
        kButton b_load;
        kButton b_save;
        kButton b_update;
        kButton b_export;
        kButton b_mini;
        kButton b_new;

        kStatic info_str;
        kStatic label;

        int select_pgm(int pgm,const char *da_source);
        int cleanup();
        int load_file();
        int check();

	dsp_code *code;
	int code_size;
	dsp_register_info *info;
	int info_size;
	int itramsize,xtramsize;
	char name[KX_MAX_STRING];
	char m_copyright[KX_MAX_STRING];
	char m_comment[KX_MAX_STRING];
	char m_engine[KX_MAX_STRING];
	char m_created[KX_MAX_STRING];
	char m_guid[KX_MAX_STRING];

        int on_command(int wParam, int lParam);

        #define IDM_LOAD	(WM_USER+0x20)
        #define IDM_SAVE	(WM_USER+0x21)
        #define IDM_UPDATE	(WM_USER+0x22)
        #define IDM_EXPORT	(WM_USER+0x23)
        #define IDM_NEW		(WM_USER+0x24)
};


#endif
