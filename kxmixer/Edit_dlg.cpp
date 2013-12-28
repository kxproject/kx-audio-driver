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


#include "stdinc.h"

#include "cmaxafx.h"
#include "edit_dlg.h"
#include "kxdspdlg.h"

#include <sys/stat.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CM_LANGUAGE LangKXDSP = {CMLS_PROCEDURAL, FALSE,
                            // keywords
                            _T("input\noutput\nstatic\ncontrol\nconst\ntemp\nidelay\nxdelay\nread")
                            _T("\nwrite\nwscl\niscl\noscl\nintr\ncoarse\nname\ncomments\nitramsize\nxtramsize")
                            _T("\nccr\nirq\naccum\nnoise1\nnoise2\ndbac\nend\nguid\ncomment\ncopyright\ncreated\nengine")
                            _T("\nKX_IN\nKX_OUT")
                            _T("\nC_0\nC_1\nC_2\nC_3\nC_4\nC_8\nC_10\nC_20\nC_100\nC_10000\nC_80000\nC_10000000\nC_20000000\nC_40000000\nC_80000000\nC_7fffffff\nC_ffffffff\nC_fffffffe\nC_c0000000\nC_4f1bbcdc\nC_5a7ef9db\nC_00100000"),
                            // operators
                            _T("macs\nmacsn\nmacw\nmacwn\nmacints\nmacintw\nacc3\nmacmv\nandxor\ntstneg\nlimit\nlimitn\nlog\nexp\ninterp\nskip\n")
                            _T("=\n&\nat"),
                            // Single line comments
                            _T(";"),
                            // Multi-line comment set
                            NULL,
                            NULL,
                            // Scope words
                            NULL,
                            NULL,
                            // String literal char
                            _T("\""),
                            // Escape Char
                            NULL,
                            // Statement Terminator Char
                            NULL,
                            // Tag element names
                            NULL,
                            // Tag attribute names
                            NULL,
                            // Tag entities
                            NULL};

void _GetDefaultColors(CM_COLORS* pColors)
{
    pColors->crWindow = 0xffffffff;               // window background color
    pColors->crLeftMargin = 0x00ffffff;           // left margin background color
    pColors->crBookmark = 0xffffffff;             // bookmark foreground color
    pColors->crBookmarkBk = 0xffffffff;           // bookmark background color
    pColors->crText = 0x00000000;                 // plain text foreground color
    pColors->crTextBk = 0xffffffff;               // plain text background color
    pColors->crNumber = 0x00800080;               // numeric literal foreground color
    pColors->crNumberBk = 0xffffffff;             // numeric literal background color
    pColors->crKeyword = 0x00ff0000;              // keyword foreground color
    pColors->crKeywordBk = 0xffffffff;            // keyword background color
    pColors->crOperator = 0x00800000;             // operator foreground color
    pColors->crOperatorBk = 0xffffffff;           // operator background color
    pColors->crScopeKeyword = 0x00ff0000;         // scope keyword foreground color
    pColors->crScopeKeywordBk = 0xffffffff;       // scope keyword background color
    pColors->crComment = 0x00008000;              // comment foreground color
    pColors->crCommentBk = 0xffffffff;            // comment background color
    pColors->crString = 0x00800080;               // string foreground color
    pColors->crStringBk = 0xffffffff;             // string background color
    pColors->crTagText = 0x00000000;              // plain tag text foreground color
    pColors->crTagTextBk = 0xffffffff;            // plain tag text background color
    pColors->crTagEntity = 0x000000ff;            // tag entity foreground color
    pColors->crTagEntityBk = 0xffffffff;          // tag entity background color
    pColors->crTagElementName = 0x00000080;       // tag element name foreground color
    pColors->crTagElementNameBk = 0xffffffff;     // tag element name background color
    pColors->crTagAttributeName = 0x00ff0000;     // tag attribute name foreground color
    pColors->crTagAttributeNameBk = 0xffffffff;   // tag attribute name background color
    pColors->crLineNumber = 0x00000000;           // line number foreground color
    pColors->crLineNumberBk = 0x00ffffff;         // line number background color
    pColors->crHDividerLines = 0xffffffff;        // line number separate line color
    pColors->crVDividerLines = 0xffffffff;        // left margin separate line color
    pColors->crHighlightedLine = 0x0000ffff;      // highlighted line color
}

int find_reg(word reg,dsp_register_info *info,int info_size)
{
 for(dword i=0;i<info_size/sizeof(dsp_register_info);i++)
 {
    if(info[i].num==reg)
     return i;
 }
 return -1;
}

CEditDialog::CEditDialog() : CKXDialog()
{ 
         codemax=0; 
         message=0;
         info=NULL;
         code=NULL;
         name[0]=0;

         file_name[0]=0;

         pgm_id=-1;
}

int CEditDialog::cleanup()
{
 // cleanup current values
 if(codemax)
   codemax->SetText("");
 if(message)
   message->SetText("");

 info_size=0;
 code_size=0;
 itramsize=0;
 xtramsize=0;
 if(info)
 {
         free(info);
         info=NULL;
 }
 if(code)
 {
         free(code);
         code=NULL;
 }
 b_update.destroy();
 name[0]=0;
 m_copyright[0]=0;
 m_comment[0]=0;
 m_engine[0]=0;
 m_created[0]=0;
 m_guid[0]=0;

 file_name[0]=0;

 pgm_id=-1;

 return 0;
}

int CEditDialog::load_file()
{
  if(codemax->OpenFile(file_name)==CME_SUCCESS)
  {
      kString s;
      s.Format(mf.get_profile("lang","editor.st_loaded"),file_name);
      message->AddText(s);

      check();
  }
  else
  {
   MessageBox(mf.get_profile("lang","editor.error_open"),"kX Mixer",MB_OK|MB_ICONSTOP);
   return -1;
  }
  return 0;
}

int CEditDialog::check()
{
      int ret=0;

      message->AddText(mf.get_profile("lang","editor.compiling"));
      kString err;
      kString s;
      dsp_code *new_code=NULL;
      dsp_register_info *new_info=NULL;
      codemax->GetText(s);
      if(ikx_t->assemble_microcode(s.GetBuffer(1),&err,name,&new_code,&code_size,
       &new_info,&info_size,&itramsize,&xtramsize,
       m_copyright,m_engine,m_created,m_comment,m_guid)==0)
      {
        message->AddText(mf.get_profile("lang","editor.st_parsed"));
        ret=0;
      }
      else
      {
        message->AddText(mf.get_profile("lang","editor.st_compile_error"));
        message->AddText(err);
        ret=-1;
      }
      // FREE here
      if(new_info) LocalFree(new_info);
      if(new_code) LocalFree(new_code);

      message->ExecuteCmd(CMD_DOCUMENTEND);
      codemax->SetFocus();

      return ret;
}

int CEditDialog::select_pgm(int pgm,const char *da_source)
{
 cleanup();

 pgm_id=pgm;

 if(da_source)
  strncpy(file_name,da_source,sizeof(file_name));

 dsp_microcode mc;

 if(pgm==-1) // open new window / load microcode
 {
   if(file_name[0]==0)
   {
     strcpy(name,"new");
     strcpy(m_copyright,"Copyright (c) 2004.");
     strcpy(m_engine,"kX");

       int ver=810;
       int month=10;
       int day=31;
       int year=2001;

       tm *tim;
       time_t current;
       time(&current);
       tim=localtime(&current);

       month=tim->tm_mon+1;
       day=tim->tm_mday;
       year=tim->tm_year+1900;

     sprintf(m_created,"%02d/%02d/%04d",month,day,year);
     ikx_t->generate_guid(m_guid);

     kString tmp1;
     tmp1.Format("; New microcode\n"
       "\tname \"%s\";\n"
       "\tcopyright \"%s\";\n"
       "\tcreated \"%s\";\n"
       "\tengine \"%s\";\n"
       "\t; comment \"\";\n"
       "\tguid \"%s\";\n\t; -- generated GUID\n\n\n"
       "; itramsize 0\n"
       "; xtramsize 0\n\n"
       "; code\n\nend\n",
       name,m_copyright,m_created,m_engine,m_guid);

     codemax->SetText(tmp1);
     kString sn;
     sn.Format("%s - %s",name,(LPCSTR)mf.get_profile("lang","editor.name"));

     set_text((LPCTSTR)sn);

     message->AddText(mf.get_profile("lang","editor.st_new"));
   }
   else
   {
    // load file here
    if(load_file())
     return -1;
   }
 }
 else
 {
  if(file_name[0]==0) // dump from the driver
  {
    if(ikx_t->enum_microcode(pgm_id,&mc)==0)
    {
      strncpy(name,mc.name,sizeof(name));
      strncpy(m_copyright,mc.copyright,sizeof(m_copyright));
      strncpy(m_comment,mc.comment,sizeof(m_comment));
      strncpy(m_engine,mc.engine,sizeof(m_engine));
      strncpy(m_created,mc.created,sizeof(m_created));
      strncpy(m_guid,mc.guid,sizeof(m_guid));

      info_size=mc.info_size;
      code_size=mc.code_size;
      itramsize=mc.itramsize;
      xtramsize=mc.xtramsize;

      info=(dsp_register_info *)malloc(info_size); memset(info,0,info_size);
      code=(dsp_code *)malloc(code_size); memset(code,0,code_size);
      ikx_t->get_microcode(pgm_id,code,code_size,info,info_size);

      kString s;

      ikx_t->disassemble_microcode
       (&s,KX_DISASM_DANE,pgm_id,code,code_size,info,info_size,itramsize,xtramsize,name,m_copyright,m_engine,m_created,m_comment,m_guid);

      codemax->AddText("; Generated by kX DSP Editor - microcode dump\n");
      codemax->AddText((LPCTSTR)s);

      message->AddText(mf.get_profile("lang","editor.st_dump"));
    }
    else
    {
     MessageBox((LPCTSTR)mf.get_profile("lang","editor.inv_microcode"),"kX Editor",MB_ICONSTOP);
     return -1;
    }
  }
  else // load from .da file
  {
    if(load_file())
     return -1;
  }

    kString sn;
    sn.Format("%s [%d] - %s",name,pgm_id,(LPCSTR)mf.get_profile("lang","editor.name"));
    set_text((LPCTSTR)sn);
 } // if pgm!=-1

 {
   codemax->EnableLeftMargin();
   codemax->EnableNormalizeCase();
   codemax->EnableSmoothScrolling();
   codemax->EnableTabExpand();

   CM_COLORS colors;
   _GetDefaultColors(&colors);
   codemax->SetColors(&colors);

   codemax->EnableSplitter(TRUE,TRUE);
   codemax->EnableSplitter(FALSE,TRUE);
   codemax->SetTabSize(4);
   codemax->SetUndoLimit(-1);
   codemax->EnableCaseSensitive(FALSE);
   codemax->EnableColorSyntax();
   codemax->EnableColumnSel();
   codemax->EnableDragDrop();
   codemax->SetAutoIndentMode(CM_INDENT_SCOPE);
   codemax->SetBorderStyle(CM_BORDER_THIN);
   codemax->SetCaretPos(1,1);

   codemax->show();
 }
 {
   message->EnableLeftMargin();
   message->EnableNormalizeCase();
   message->EnableSmoothScrolling();
   message->EnableTabExpand();

   CM_COLORS colors;
   _GetDefaultColors(&colors);
   message->SetColors(&colors);

   message->EnableSplitter(TRUE,FALSE);
   message->EnableSplitter(FALSE,FALSE);
   message->SetTabSize(4);
   message->SetUndoLimit(-1);
   message->EnableCaseSensitive(FALSE);
   message->EnableColorSyntax();
   message->EnableColumnSel();
   message->EnableDragDrop();
   message->SetAutoIndentMode(CM_INDENT_SCOPE);
   message->SetBorderStyle(CM_BORDER_THIN);
   message->SetCaretPos(1,1);

   message->show();
 }
 
 kString st;
 if(pgm==-1)
 {
   if(file_name[0])
     st.Format(mf.get_profile("lang","editor.loaded"));
   else
     st.Format(mf.get_profile("lang","editor.new"));
 }
 else
 {
   st.Format(mf.get_profile("lang","editor.dump"));
 }

// if(pgm_id!=-1)
 {
  kString tmp;
  tmp.Format((LPCTSTR)mf.get_profile("lang","editor.status"),
    name,pgm_id,code_size/sizeof(dsp_code),info_size/sizeof(dsp_register_info),itramsize,xtramsize);
  st+=tmp;
 }

 info_str.set_text(st);
 info_str.set_font(font);
 info_str.show();
 info_str.redraw();

 char tmp_str[128];
 if(pgm_id!=-1)
 {
     if(!mf.get_profile("editor","update_button",tmp_str,sizeof(tmp_str)))
     {
      gui_create_button(this,&b_update,IDM_UPDATE,tmp_str,mf);
      b_update.show();
     }
 }
 else
 {
     if(!mf.get_profile("editor","check_button",tmp_str,sizeof(tmp_str)))
     {
      gui_create_button(this,&b_update,IDM_UPDATE,tmp_str,mf);
      b_update.show();
     }
 }

 return 0;
}

CEditDialog::~CEditDialog() 
{ 
 if(code) { free(code); code=0; }
 if(info) { free(info); info=0; }
 if(codemax) { delete codemax; codemax=0; }
 if(message) { delete message; message=0; }
}


void CEditDialog::init()
{
  CKXDialog::init("editor");

  char tmp_str[256];

  unsigned int transp_color=0x0;
  unsigned int alpha=90;
  unsigned int cc_method=0;

  if(mf.get_profile("editor","background",tmp_str,sizeof(tmp_str)))
  {
   tmp_str[0]=0;
  }

  gui_set_background(this,&mf,tmp_str,transp_color,alpha,cc_method,_T("dsp/editor.jpg"));

  set_dragging(gui_get_show_content());

  SetIcon(AfxGetApp()->LoadIcon(IDR_ICON), TRUE);           // Set big icon
//  SetIcon(AfxGetApp()->LoadIcon(IDR_ICON), FALSE);        // Set small icon

  if(!mf.get_profile("editor","quit_button",tmp_str,sizeof(tmp_str)))
  {
   gui_create_button(this,&b_return,IDOK,tmp_str,mf);
   b_return.show();
  }
  if(!mf.get_profile("editor","mini_button",tmp_str,sizeof(tmp_str)))
  {
   gui_create_button(this,&b_mini,IDg_MINI,tmp_str,mf);
   b_mini.show();
  }
  if(!mf.get_profile("editor","load_button",tmp_str,sizeof(tmp_str)))
  {
   gui_create_button(this,&b_load,IDM_LOAD,tmp_str,mf);
   b_load.show();
  }
  if(!mf.get_profile("editor","save_button",tmp_str,sizeof(tmp_str)))
  {
   gui_create_button(this,&b_save,IDM_SAVE,tmp_str,mf);
   b_save.show();
  }
  if(!mf.get_profile("editor","export_button",tmp_str,sizeof(tmp_str)))
  {
   gui_create_button(this,&b_export,IDM_EXPORT,tmp_str,mf);
   b_export.show();
  }
  if(!mf.get_profile("editor","new_button",tmp_str,sizeof(tmp_str)))
  {
   gui_create_button(this,&b_new,IDM_NEW,tmp_str,mf);
   b_new.show();
  }

  RECT r;
  memset(&r,0,sizeof(r));

  if(!mf.get_profile("editor","edit",tmp_str,sizeof(tmp_str)))
  {
    sscanf(tmp_str,"%d %d %d %d",&r.left,&r.top,&r.right,&r.bottom);
  }
   else
  {
   debug(_T("kxmixer: skin invalid (editor)\n"));
   MessageBox((LPCTSTR)mf.get_profile("lang","editor.incorrect_skin"),
    (LPCTSTR)mf.get_profile("lang","editor.severe"),MB_OK|MB_ICONSTOP);
   exit(32);
  }

 codemax=new CDSPEditor();
 codemax->Create(WS_BORDER | WS_CHILD | WS_VSCROLL | WS_HSCROLL, r, this, 0x1000);
 
 VERIFY( codemax->SetLanguage( _T("10kX DSP") ) == CME_SUCCESS);

 // label
 if(!mf.get_profile("editor","label",tmp_str,sizeof(tmp_str)))
 {
  dword fg=0,bk=0;
  sscanf(tmp_str,"%d %d %d %d %x %x",&r.left,&r.top,&r.right,&r.bottom,&fg,&bk);
  label.create((char *)(LPCTSTR)mf.get_profile("editor","label.text"),r.left,r.top,r.right,r.bottom,this,fg,bk);
  label.show();
  label.set_font(font);
 }

 COLORREF fg=0xffffff;
 COLORREF bk=0xb0b000;
 if(!mf.get_profile("editor","status_line",tmp_str,sizeof(tmp_str)))
 {
    // x,y,wd,ht
    sscanf(tmp_str,"%d %d %d %d %x %x",&r.left,&r.top,&r.right,&r.bottom,&fg,&bk);
 }
 info_str.create((char *)(LPCTSTR)mf.get_profile("lang","editor.info"),r.left,r.top,r.right,r.bottom,this,fg,bk);
 info_str.show();

 // messages list creation
 if(!mf.get_profile("editor","message",tmp_str,sizeof(tmp_str)))
 {
    sscanf(tmp_str,"%d %d %d %d",&r.left,&r.top,&r.right,&r.bottom);
 }
 message=new CDSPEditor();
 message->Create(WS_BORDER | WS_CHILD | WS_VSCROLL | WS_HSCROLL, r, this, 0x1000);
 message->SetReadOnly();

 rest_position();
}

int CEditDialog::on_command(int wParam, int lParam)
{
    kString str;
    switch(wParam)
    {
                case IDM_NEW:
                    select_pgm(-1,NULL);
                    return 1;
        case IDM_LOAD:
            {
            restore_cwd("da");
                        CFileDialog *f_d = new CFileDialog(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_EXPLORER,
                          "kX Dane Source (*.da)|*.da||",this);
                        if(f_d)
                        {
                            char tmp_cwd[MAX_PATH];
                            GetCurrentDirectory(sizeof(tmp_cwd),tmp_cwd);
                            f_d->m_ofn.lpstrInitialDir=tmp_cwd;

                            if(f_d->DoModal()==IDCANCEL)
                            {
                             delete f_d;
                             return 1;
                            }

                            save_cwd("da");

                            kString fname=f_d->GetPathName();
                            delete f_d;

                            if(fname.GetLength()==0) // cancel or empty
                            {
                             return 1;
                            }
                            strncpy(file_name,(LPCTSTR)fname,sizeof(file_name));
                            load_file();
                            pgm_id=-1;
                                info_str.set_text((LPCTSTR)mf.get_profile("lang","editor.uploaded"));
                                info_str.redraw();
                        }
                        }
            return 1;
        case IDM_SAVE:
            {
            check();

            char tmp_name[MAX_PATH];
            if(file_name[0])
            {
             strncpy(tmp_name,file_name,sizeof(tmp_name));
            }
            else
            {
             strcpy(tmp_name,name);
             while(tmp_name[strlen(tmp_name)-1]==' ')
              tmp_name[strlen(tmp_name)-1]=0;
             strcat(tmp_name,".da");
            }
            restore_cwd("da");

                        CFileDialog *f_d = new CFileDialog(FALSE,NULL,tmp_name,OFN_HIDEREADONLY|OFN_EXPLORER|OFN_OVERWRITEPROMPT,
                          "kX Dane Source (*.da)|*.da||",this);
                        if(f_d)
                        {
                            char tmp_cwd[MAX_PATH];
                            GetCurrentDirectory(sizeof(tmp_cwd),tmp_cwd);
                            f_d->m_ofn.lpstrInitialDir=tmp_cwd;

                            if(f_d->DoModal()==IDCANCEL)
                            {
                             delete f_d;
                             return 1;
                            }
                            save_cwd("da");

                            kString fname=f_d->GetPathName();
                            delete f_d;

                            if(fname.GetLength()==0) // cancel or empty
                            {
                             return 1;
                            }
                            if(fname.Find(".da")==-1)
                             fname+=".da";

                            codemax->SaveFile(fname);
                        }
                        }
            return 1;
        case IDM_EXPORT:
            {
            if(check()==0)
            {
            char tmp_name[MAX_PATH];
            strcpy(tmp_name,name);
            while(tmp_name[strlen(tmp_name)-1]==' ')
             tmp_name[strlen(tmp_name)-1]=0;
            strcat(tmp_name,".cpp");

            restore_cwd("cpp");
                        CFileDialog *f_d = new CFileDialog(FALSE,NULL,tmp_name,OFN_HIDEREADONLY|OFN_EXPLORER|OFN_OVERWRITEPROMPT,
                          "kX C++ Source (*.cpp)|*.cpp||",this);
                        if(f_d)
                        {
                            char tmp_cwd[MAX_PATH];
                            GetCurrentDirectory(sizeof(tmp_cwd),tmp_cwd);
                            f_d->m_ofn.lpstrInitialDir=tmp_cwd;

                            if(f_d->DoModal()==IDCANCEL)
                            {
                             delete f_d;
                             return 1;
                            }
                            save_cwd("cpp");
                            kString fname=f_d->GetPathName();
                            delete f_d;

                            if(fname.GetLength()==0) // cancel or empty
                            {
                             return 1;
                            }
                            if(fname.Find(".cpp")==-1)
                             fname+=".cpp";

                  dsp_register_info *new_info;
                  dsp_code *new_code;
                  int new_itramsize,new_xtramsize;
                  int new_code_size,new_info_size;
                  char new_name[KX_MAX_STRING];
                  char new_copyright[KX_MAX_STRING];
                  char new_comment[KX_MAX_STRING];
                  char new_engine[KX_MAX_STRING];
                  char new_created[KX_MAX_STRING];
                  char new_guid[KX_MAX_STRING];

                  new_name[0]=0;
                  new_copyright[0]=0;
                  new_comment[0]=0;
                  new_engine[0]=0;
                  new_created[0]=0;
                  new_guid[0]=0;

                  kString s;
                  kString err;
                  codemax->GetText(s);
                  message->AddText(mf.get_profile("lang","editor.compiling"));

                                  if(ikx_t->assemble_microcode(s.GetBuffer(1),&err,new_name,&new_code,&new_code_size,
                                   &new_info,&new_info_size,&new_itramsize,&new_xtramsize,
                                   new_copyright,new_engine,new_created,new_comment,new_guid)==0)
                                  {
                                    if(err.GetLength()>0)
                                    {
                                     MessageBox((LPCTSTR)mf.get_profile("lang","editor.warning"),"kX Editor",MB_OK|MB_ICONEXCLAMATION);
                                     message->AddText(err);
                                    }
                                    // re-disassemble here
                                        kString tmp;
                                        if(ikx_t->disassemble_microcode(&tmp,KX_DISASM_CPP,-1,new_code,new_code_size,new_info,new_info_size,new_itramsize,new_xtramsize,
                                          new_name,new_copyright,new_engine,new_created,new_comment,new_guid)==0)
                                        {
                                            FILE *f;
                                            f=fopen((LPCTSTR)fname,"wt");
                                            if(f)
                                            {
                                             fwrite(tmp.GetBuffer(1),1,tmp.GetLength(),f);
                                             fclose(f);
                                            } 
                                             else 
                                            {
                                             MessageBox((LPCTSTR)mf.get_profile("errors","file_open"),"kX Mixer",MB_OK|MB_ICONSTOP);
                                            }
                                        } else 
                                        {
                                         MessageBox((LPCTSTR)mf.get_profile("lang","editor.redis"),"kX Mixer",MB_OK|MB_ICONSTOP);
                                        }
                                    // FREE here
                                    if(new_info) LocalFree(new_info);
                                    if(new_code) LocalFree(new_code);
                                  } else 
                                  {
                                   err+=(LPCTSTR)mf.get_profile("errors","translate");
                                   MessageBox((LPCTSTR)err,(LPCTSTR)mf.get_profile("locals","compile"),MB_OK|MB_ICONSTOP);
                                   message->AddText(err);
                                  }
                          } // f_d ok
                        } // if check() ok
                        }
            return 1;
        case IDM_UPDATE:
            if(pgm_id==-1)
             check();
            else
            {
                           kString err;
                           kString s;
                           dsp_code *new_code=NULL;
                           dsp_register_info *new_info=NULL;
                           codemax->GetText(s);
                           if(ikx_t->assemble_microcode(s.GetBuffer(1),&err,name,&new_code,&code_size,
                            &new_info,&info_size,&itramsize,&xtramsize,
                            m_copyright,m_engine,m_created,m_comment,m_guid)==0)
                           {
                               if(ikx_t->update_microcode(pgm_id,name,new_code,code_size,
                                 new_info,info_size,itramsize,xtramsize,
                                 m_copyright,m_engine,m_created,m_comment,m_guid,IKX_UPDATE_ALL)!=0)
                               {
                                message->AddText(mf.get_profile("lang","editor.update_failed"));
                                message->AddText("\n");
                               }
                               else
                               {
                                message->AddText(mf.get_profile("lang","editor.updated"));
                                message->AddText("\n");
                               }

                               // update kX DSP here
                               if(dsp_window)
                               {
                                dsp_window->redraw_window(); 
                                dsp_window->redraw();
                               }
                           }
                           else
                           {
                             message->AddText(mf.get_profile("lang","editor.st_compile_error"));
                             message->AddText(err);
                           }
                           // FREE here
                           if(new_info) LocalFree(new_info);
                           if(new_code) LocalFree(new_code);
            }
            return 1;
    }
    return CKXDialog::on_command(wParam,lParam);
}


int CDSPEditor::GetText( kString &strText, const CM_RANGE *pRange )
{
 if(cm)
  return (int)cm->GetText(strText,pRange);
 else
 {
  ke->GetWindowText(strText);
  return 0;
 }
}

int CDSPEditor::AddText( LPCTSTR pszText )
{
 if(cm)
  return (int)cm->AddText(pszText);
 else
  {
   kString tt;
   GetText(tt);
   tt+=pszText;
   SetText(tt);
   return 0;
  }
};

int CDSPEditor::SetText( LPCTSTR pszText )
{
 if(!pszText)
  return -1;

 if(cm)
  return (int)cm->SetText(pszText);
 else
  {
   // parse test ('\n' \ '\r')
   TCHAR *tmp=(TCHAR *)malloc(strlen(pszText)*2+1);
   memset(tmp,0,strlen(pszText)*2+1);

   if(tmp)
   {
    TCHAR *p=tmp;
    TCHAR *q=(TCHAR *)pszText;
    
    while(*q)
    {
     *p=*q;
     if(*q=='\n' && p[-1]!='\r')
     { *p='\r'; p++; *p='\n'; }
     p++;
     q++;
    }
    *p=0;

    if(ke)
     ::SetWindowText(ke->m_hWnd,tmp);

    free(tmp);
    return 0;
   }
   
   return -1;
  }
};

int CDSPEditor::ExecuteCmd( WORD wCmd, DWORD dwCmdData )
{
 if(cm)
  return (int)cm->ExecuteCmd(wCmd,dwCmdData);
 else
  {
   switch(wCmd)
   {
    case CMD_DOCUMENTEND:
        ke->LineScroll(ke->GetLineCount());
        return 0;
    default:
        debug("kxmixer: kx editor: invalid cmax20 cmd [%d]\n",wCmd);
        return -1;
   }
   return 0;
  }
};
int CDSPEditor::OpenFile( LPCTSTR pszFileName )
{
 if(cm)
  return (int)cm->OpenFile(pszFileName);
 else
  {
   int ret=0;

   FILE *f;
   f=fopen(pszFileName,"rt"); // 'wt' since opened file may be \r\n-based
   if(f)
   {
    struct _stat st;
    if(_stat(pszFileName,&st)==0)
    {
     char *mem=(char *)malloc(st.st_size);
     if(mem!=0)
     {
      memset(mem,0,st.st_size);

      fread(mem,1,st.st_size,f);

      SetText(mem);
      ret=1;

      free(mem);
     }
    }
    fclose(f);
   }
   return ret;
  }
};
int CDSPEditor::SaveFile(LPCTSTR pszFileName, BOOL bClearUndo )
{
 if(cm)
  return (int)cm->SaveFile(pszFileName,bClearUndo);
 else
  {
   int ret=0;
   kString tmp;
   ke->GetWindowText(tmp);
   FILE *f=fopen(pszFileName,"wb"); // since 'getwindowtext' contains \r\n
   if(f)
   {
    fwrite((LPCTSTR)tmp,1,strlen((LPCTSTR)tmp),f);
    fclose(f);
   }

   return ret;
  }
};


static HINSTANCE cmax_dll=NULL;
static int registered=0;

extern "C" typedef CME_CODE (__cdecl *CMRegisterControl_)( DWORD dwVersion );
extern "C" typedef CME_CODE (__cdecl *CMUnregisterControl_)();
extern "C" typedef CME_CODE (__cdecl *CMRegisterLanguage_)( LPCTSTR pszName, CM_LANGUAGE *pLang );

CDSPEditor::CDSPEditor() 
{
 if(cmax_dll && registered)
 {
  cm=new CCodeMaxCtrl; 
  ke=NULL;
 }
 else
 {
  cm=NULL;
  ke=new CEdit;
 }
};

void init_cmax()
{
 if(cmax_dll==NULL)
 {
  cmax_dll=LoadLibrary("cmax20.dll");
  if(cmax_dll)
  {
   CMRegisterControl_ reg=(CMRegisterControl_)GetProcAddress(cmax_dll,"CMRegisterControl");
   CMRegisterLanguage_ lan=(CMRegisterLanguage_)GetProcAddress(cmax_dll,"CMRegisterLanguage");

   if(reg && lan)
   {
    reg(CM_VERSION);
    lan( _T("10kX DSP"), &LangKXDSP );
    registered=1;
   }
   else
    debug("Error getting CMAX20.DLL entry points\n");
  }
 }
}

void close_cmax()
{
 if(cmax_dll)
 {
  CMUnregisterControl_ unr=(CMUnregisterControl_)GetProcAddress(cmax_dll,"CMUnregisterControl");
  if(unr)
  {
   unr();
  } else debug("Error getting CMAX20.DLL entry points\n");

  FreeLibrary(cmax_dll);
  cmax_dll=NULL;
  registered=0;
 }
}
