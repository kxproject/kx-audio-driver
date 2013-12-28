// kX Resource Control
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

#define MIN_REQUIRED	6
#define MAX_REQUIRED	6

#pragma warning(disable:4702)
#pragma warning(disable:4100)
#pragma warning(disable:4244)
#pragma warning(disable:4242)

extern "C"
{
#include "unzip.h"
};

#define UNRAR
#define SILENT
#undef STRICT
#undef WINVER
#undef _WIN32_WINNT
#undef WIN32_LEAN_AND_MEAN
#pragma warning(disable:4244)
#include "rar/rar.hpp"
#include "../kxrar/smallfn.cpp"

ErrorHandler ErrHandler;

extern "C" uLong ZEXPORT __declspec(dllexport) _adler32 OF((uLong adler, const Bytef *buf, uInt len))
{
 return adler32(adler,buf,len);
}

extern "C" int ZEXPORT __declspec(dllexport) _uncompress OF((Bytef *dest,   uLongf *destLen,
                                   const Bytef *source, uLong sourceLen))
{
 return uncompress(dest,destLen,source,sourceLen);
}

struct _image_cache_t
{
 kString *name;
 HBITMAP bm;
 struct _image_cache_t *next;
};

class CArchFile
{
public:
	CArchFile(const TCHAR *fname):Arc(&Cmd) 
	  { 
	    uf=NULL; 
	    error=0;
	    tmp_f=NULL;
	    HeaderSize=-1;

//	    USES_CONVERSION;

            // try zip first
#ifdef UNICODE
            char *fname_=(char *)W2A(fname);
#else
            char *fname_=(char *)fname;
#endif

	    uf=unzOpen(fname_);
	    if(uf==NULL)
	    {
	      // then - rar
              Cmd.FileArgs->AddString("*");

              Cmd.AddArcName(fname_,NULL);
              Cmd.Overwrite=OVERWRITE_ALL;
              Cmd.Test=false;
              strcpy(Cmd.Command,"E");
              if(Arc.Open(fname_))
              {
              	if(Arc.IsArchive(false))
              	{
              	 // ok
              	 Extract.ExtractArchiveInit(&Cmd,Arc);
              	 error=0;
              	 return;
              	}
              } 
	      error=GetLastError();
	      if(error==0)
	        error=-100;
	    }
	  };
	~CArchFile() 
	  { 
	     if(uf) 
	      { unzClose(uf); uf=NULL; }
             // rar is freed automatically
             HeaderSize=-1;
             error=0;
             if(tmp_f)
              { fclose(tmp_f); tmp_f=NULL; }
	  };

	int error;

	int LocateFile(const TCHAR *fname);
	int GetUncompressedSize(size_t *uncompressed_size);
	int OpenCurrentFile();
	int CloseCurrentFile();
	int ReadCurrentFile(void *buff,size_t size);

private:
	// unzip
	void *uf; 

        // unrar
        CommandData Cmd;
        CmdExtract Extract;
        Archive Arc;
        FILE *tmp_f;
        int HeaderSize;
};

int CArchFile::LocateFile(const TCHAR *fname)
{
 if(uf)
 {
//  USES_CONVERSION;
#ifdef UNICODE
  return unzLocateFile(uf,W2A(fname),0);
#else
  return unzLocateFile(uf,(char *)fname,0);
#endif
 }
 else
 {
     Arc.Seek(Arc.SFXSize,SEEK_SET);
     HeaderSize=Arc.SearchBlock(FILE_HEAD);

     TCHAR tmp_dir[MAX_PATH];
     GetTempPath(MAX_PATH,tmp_dir);

     TCHAR tmp_name[MAX_PATH];
     _tcsncpy(tmp_name,fname,MAX_PATH);
     TCHAR *p=tmp_name;
     while(*p)
      { if(*p=='/') *p='\\'; p++; }

     while(HeaderSize>0)
     {
//       OemToChar(NullToEmpty(tmp_dir),Cmd.ExtrPath);
//       USES_CONVERSION;

#ifdef UNICODE
       strcpy(Cmd.ExtrPath,W2A(tmp_dir));
#else
       strcpy(Cmd.ExtrPath,(char *)(tmp_dir));
#endif
       AddEndSlash(Cmd.ExtrPath);

#ifdef UNICODE
       if(_stricmp(Arc.NewLhd.FileName,W2A(tmp_name))==NULL)
#else
       if(_stricmp(Arc.NewLhd.FileName,(char *)(tmp_name))==NULL)
#endif
       {
        bool Repeat=false;
        Extract.ExtractCurrentFile(&Cmd,Arc,HeaderSize,Repeat);
        // found:
        return 0;
       }
       else
       {
        if(Arc.Solid)
        {
         Cmd.Test=true;
         bool Repeat=false;
         Extract.ExtractCurrentFile(&Cmd,Arc,HeaderSize,Repeat);
         Cmd.Test=false;
        }
       }
       Arc.SeekToNext();
       HeaderSize=Arc.SearchBlock(FILE_HEAD);
     }
  return -100;
 }
}

int CArchFile::GetUncompressedSize(size_t *uncompressed_size)
{
 if(uf)
 {
  char filename_inzip[512];
  unz_file_info file_info;
  int err=unzGetCurrentFileInfo(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);
  *uncompressed_size=file_info.uncompressed_size;
  return err;
 }
 else
 {
  if(HeaderSize>0)
  {
   *uncompressed_size=Arc.NewLhd.UnpSize;
   return 0;
  }
  else debug(_T("CArchFile::GetUncompressedSize(): Header Size = %d\n"));
 }
 *uncompressed_size=0;
 return -1;
}

int CArchFile::OpenCurrentFile()
{
 if(uf)
 {
  return unzOpenCurrentFile(uf);
 }
 else
 {
  if(HeaderSize>0)
  {
   TCHAR tmp_dir[MAX_PATH];
   TCHAR name[MAX_PATH];
   TCHAR ext[MAX_PATH];
   GetTempPath(sizeof(tmp_dir),tmp_dir);

   kString tt=Arc.NewLhd.FileName;
   _tsplitpath(tt,NULL,NULL,name,ext);
   _tcscat(name,ext);
   _tcscpy(ext,tmp_dir);
   _tcscat(ext,name);

   tmp_f=_tfopen(ext,_T("rb"));
   if(tmp_f)
   {
    return 0;
   }
   debug(_T("CArchFile::Rar::Open Error opening %s [%x] -- internal error\n"),ext,GetLastError());
   return GetLastError();
  }
 }
 return -100;
}

int CArchFile::CloseCurrentFile()
{
 if(uf)
 {
  return unzCloseCurrentFile(uf);
 }
 else
 {
  if(tmp_f)
  {  
    fclose(tmp_f); 
    tmp_f=NULL; 
    TCHAR tmp_dir[MAX_PATH];
    TCHAR name[MAX_PATH];
    TCHAR ext[MAX_PATH];
    GetTempPath(sizeof(tmp_dir),tmp_dir);

    kString tt=Arc.NewLhd.FileName;
    _tsplitpath(tt,NULL,NULL,name,ext);
    _tcscat(name,ext);
    _tcscpy(ext,tmp_dir);
    _tcscat(ext,name);

    _tchmod(ext,_S_IREAD|_S_IWRITE);
    _tunlink(ext);

    return 0; 
  }
 }
 return -100;
}

int CArchFile::ReadCurrentFile(void *buff,size_t size)
{
 if(uf)
 {
  return unzReadCurrentFile(uf,buff,(unsigned)size);
 }
 else
 {
  if(tmp_f)
  {
   fread(buff,size,1,tmp_f);
   return 1;
  }
 }
 return -100;
}

#define MAX_SECT_NAME	64

struct _kxgui_ini_sections
{
 TCHAR name[MAX_SECT_NAME];
 TCHAR *memory;

 struct _kxgui_ini_sections *next;
};


kFile::kFile()
{
  af=NULL;
  priority=0;
  InitCRC();
  cur_image_cache=NULL;
  current_language=GetUserDefaultLangID();
  next=NULL;

  sections=0;
}

kFile::~kFile()
{
  close();
  flush_cache();
  current_language=0;
  next=NULL;
}

int kFile::init()
{
   TCHAR tmp_str[MAX_PATH];
   get_full_skin_path(_T("kxskin.kxs"),tmp_str);
   debug(_T("trying: %s\n"),tmp_str);

   return set_skin(tmp_str);
}

int kFile::set_skin(const TCHAR *fname)
{
    close();
    flush_cache();

    struct _stat st;
    if(_tstat(fname,&st)!=0) // not found
    {
     debug(_T("kFile: _stat failed [%s; %x]\n"),fname,GetLastError());
     return -2;
    }

    if(!(st.st_mode&_S_IFDIR)) // not a directory
    {
      af = new CArchFile(fname);
      if((af==NULL) || (af->error))
      {
//        debug(_T("kFile: afOpen failed [%s; %x]\n"),fname,GetLastError());
      	return -1;
      }
    }

    skin_file=fname;

    int ret=load_inis();
    if(ret)
     return ret;

    return 0;
}

static void compact(kString &str,TCHAR *p, int current_language)
{
 int ln=0;
  if(p)
      while(*p)
      {
       TCHAR line[4096];
       TCHAR *d=strchr(p,'\r');// _tcschr(p,'\r');
       if(d)
       {
        *d=0;
        if(/*_tcslen(p)*/ strlen(p)>2)
        {
         // _tcsncpy(line,p,sizeof(line)-2);
         strncpy(line,p,sizeof(line)-2);
         // _tcscat(line,_T("\r\n"));
         strcat(line,_T("\r\n"));

         if(line[0]=='#')
         {
          goto next;
         }
         if(line[0]=='[')
         {
          str+=line;
          goto next;
         }
         TCHAR *t=strchr(line,'=');// _tcschr(line,'=');
         if(t)
         {
          *t=0;
          TCHAR *dot=strrchr(line,'.');// _tcsrchr(line,'.');
          if(dot)
          {
           int lang=-1;
           // _stscanf(dot+1,_T("%x"),&lang);
           sscanf(dot+1,_T("%x"),&lang);
           if((lang==-1)||(!isdigit(*(dot+1)))) // w/o .<lang> or .e
           {
            *t='=';
            str+=line;
           }
           else
           {
            if((lang==current_language)||(lang<0x400))
            {
             *t='=';
             str+=line;
            }
           }
          }
          else
          {
           *t='=';
           str+=line;
          }
         }
        }
        ln++;
next:
        p=d+1;
        while((*p=='\r')||(*p=='\n'))
         p++;
       }
        else 
         break;
      }
}

int kFile::load_inis()
{
 kFile *o_next=next;
 next=NULL;

    // assume it can be a directory...
    size_t size;
    int error;
    TCHAR *tmp_ini_file=(TCHAR *)load_data(_T("kxskin.ini"),&size,&error);

    if((tmp_ini_file==0)||(error!=0)||(size==0))
    {
     debug(_T("kFile: no kxskin.ini\n"));
     next=o_next;
     return -5;
    }

    kString ini_file;
    ini_file="\r\n\r\n";

    compact(ini_file,tmp_ini_file,current_language);

    free(tmp_ini_file);

/*    __asm
    {
     int 3
    }
*/
    TCHAR *tmp_lang_file=(TCHAR *)load_data(_T("lang.ini"),&size,&error);

    if((tmp_lang_file!=0)&&(error==0)&&(size!=0))
    {
      ini_file+="\r\n\r\n";
      compact(ini_file,tmp_lang_file,current_language);
      free(tmp_lang_file);
    }
   
    TCHAR *tmp_categ_file=(TCHAR *)load_data(_T("categories.ini"),&size,&error);

    if((tmp_categ_file!=0)&&(error==0)&&(size!=0))
    {
      ini_file+="\r\n\r\n";
      compact(ini_file,tmp_categ_file,current_language);
      free(tmp_categ_file);
    }
   
    TCHAR *tmp_sliders_file=(TCHAR *)load_data(_T("sliders.ini"),&size,&error);

    if((tmp_sliders_file!=0)&&(error==0)&&(size!=0))
    {
      ini_file+="\r\n\r\n";
      compact(ini_file,tmp_sliders_file,current_language);
      free(tmp_sliders_file);
    }
   
    TCHAR *tmp_buttons_file=(TCHAR *)load_data(_T("buttons.ini"),&size,&error);

    if((tmp_buttons_file!=0)&&(error==0)&&(size!=0))
    {
      ini_file+="\r\n\r\n";
      compact(ini_file,tmp_buttons_file,current_language);
      free(tmp_buttons_file);
    }
    next=o_next;

    // destroy ini sections (if any)
    if(sections)
    {
     if(sections->memory)
      free(sections->memory);
     sections->memory=0;
    }

    while(sections)
    {
     _kxgui_ini_sections *next=sections->next;
     free(sections);
     sections=next;
    }

    // process ini sections here...
    TCHAR *mem=NULL;
    size_t sz=(_tcslen((LPCTSTR)ini_file)+1)*sizeof(TCHAR);
    mem=(TCHAR *)malloc(sz);
    if(mem)
    {
       _kxgui_ini_sections *first=(_kxgui_ini_sections *)malloc(sizeof(struct _kxgui_ini_sections));
       if(first)
       {
        sections=first;
        sections->memory=mem;
        sections->name[0]=0;
        sections->next=0;

        memcpy(mem,(LPCTSTR)ini_file,sz);

        while(1)
        {
         mem=_tcsstr(mem,"\r\n[");
         if(mem)
         {
          mem[1]=0;
          mem+=2;
          TCHAR *p=_tcsstr(mem,"]\r\n");
          if(p)
          {
           _kxgui_ini_sections *next=(_kxgui_ini_sections *)malloc(sizeof(struct _kxgui_ini_sections));
           if(next)
           {
            *p=0;
            _tcsncpy(next->name,mem+1,MAX_SECT_NAME);
            *p=__T(']');

            next->memory=mem;
            next->next=0;
            first->next=next;

            first=next;
           }
          }
         }
         else
          break;
        }
       }
    }
    else
    {
     debug("kxgui: critical: no more memory for sections! [%d]\n",sz);
     return -1;
    }

    TCHAR tmp[12];
    if(!get_profile(_T("skin"),_T("required"),tmp,sizeof(tmp)))
    {
     int r=0;
     _stscanf(tmp,_T("%d"),&r);
     if(r<MIN_REQUIRED || r>MAX_REQUIRED)
     {
     	MessageBox(NULL,_T("Skin version is invalid"),_T("Severe Error"),MB_OK|MB_ICONSTOP);
     	debug(_T("required=%d\n"),r);
     	return -6;
     }
    }
    else
    {
    	MessageBox(NULL,_T("Skin version is invalid"),_T("Severe Error"),MB_OK|MB_ICONSTOP);
    	debug(_T("required field not found\n"));
        return -8;
    }
    skin_guid=get_profile(_T("skin"),_T("guid"));
    skin_name=get_profile(_T("skin"),_T("name"));

    return 0;
}

int kFile::close()
{
    if(af)
    {
     delete af;
     af=NULL;
    }
    skin_file="";
    skin_guid="";
    skin_name="";

    // destroy ini sections
    if(sections)
    {
     if(sections->memory)
      free(sections->memory);
     sections->memory=0;
    }

    while(sections)
    {
     _kxgui_ini_sections *next=sections->next;
     free(sections);
     sections=next;
    }

    return 0;
}

void *kFile::_load_from_arch(const TCHAR *fname,size_t *size,int *error)
{
    int err=UNZ_OK;
    void* buf;
    TCHAR *p=(TCHAR *)fname;

    if(!af)
    {
     *error=-100;
     return NULL;
    }

    while(*p)
     { if(*p=='\\') *p='/'; p++; }

    *error=0;
    *size=0;

    if(af->LocateFile(fname)!=UNZ_OK)
    {
//        debug(_T("kxfile::load locatefile failed [%s]\n"),fname);
    	*error=-2;
    	return NULL;
    }

    size_t uncompressed_size;
    err = af->GetUncompressedSize(&uncompressed_size);
    if (err!=UNZ_OK)
    {
        debug(_T("kxfile::load get uncompressed size failed\n"));
    	*error=-3;
    	return NULL;
    }

    buf = (void*)malloc(uncompressed_size+1);
    if (buf==NULL)
    {
    	*error=-4;
    	debug(_T("kxfile::load malloc failed [%s]\n"),fname);
    	return NULL;
    }
    ((char *)buf)[uncompressed_size]=0;

    err = af->OpenCurrentFile();
    if(err!=UNZ_OK)
    {
        debug(_T("kxfile::load openfile failed [%s]\n"),fname);
        free(buf);
    	*error=-5;
    	return NULL;
    }

    err = af->ReadCurrentFile(buf,uncompressed_size);
    if(err<=0)
    {
      debug(_T("kxfile::load read failed [%s:%d]\n"),fname,uncompressed_size);
      *error=-6;
      free(buf);
      buf=NULL;
    } 
     else 
    {
     *size=uncompressed_size;
    }
    af->CloseCurrentFile();

    return buf;
}

void *kFile::_load_from_file(const TCHAR *fname,size_t *size,int *error)
{
 *error=0;
 *size=0;

 FILE *f;
 TCHAR tmp_path[MAX_PATH];
 sprintf(tmp_path,"%s\\%s",(LPCTSTR)skin_file,fname);

 f=_tfopen(tmp_path,_T("rb"));
 if(f)
 {
  void *buf;
  int sz=0;
  fseek(f,0L,SEEK_END);
  sz=ftell(f);
  fseek(f,0L,SEEK_SET);
  buf=malloc(sz);
  if(buf==0)
  {
  	*error=-2;
  	fclose(f);
  	return NULL;
  }
  fread(buf,1,sz,f);
  *size=sz;

  fclose(f);
  return buf;
 }
 *error=-1;
 return NULL;
}

HBITMAP kFile::load_image(const TCHAR *fname)
{
 HBITMAP bm;
 size_t size=0;
 int error=0;
 void *buf=0;

 if(next && (priority==0))
 {
  bm=next->load_image(fname);
  if(bm!=0)
   return bm;
 }

 // try cache
 if(!find_image(fname,&bm))
  return bm;

 buf=load_data(fname,&size,&error);

 if((buf!=0) && (error==0) && (size!=0))
 {
   HGLOBAL hGlobal=GlobalAlloc(GMEM_MOVEABLE, size);
   _ASSERTE(NULL != hGlobal);
   
   memcpy(GlobalLock(hGlobal),buf,size);

   // create IStream* from global memory
   LPSTREAM pstm = NULL;
   HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pstm);
   _ASSERTE(SUCCEEDED(hr) && pstm);

   // Create IPicture from image file
   LPPICTURE gpPicture;

   hr = ::OleLoadPicture(pstm, (LONG)size, FALSE, IID_IPicture, (LPVOID *)&gpPicture);
   _ASSERTE(SUCCEEDED(hr) && gpPicture);	
   pstm->Release();

   OLE_HANDLE m_picHandle;
   gpPicture->get_Handle(&m_picHandle);

   HBITMAP src=(HBITMAP)(uintptr_t)m_picHandle;

   // now, copy bitmap
   HDC dc=GetDC(NULL); // get screen DC

   BITMAP bmInfo;
   VERIFY(GetObject(src,sizeof(bmInfo), &bmInfo) == sizeof(bmInfo));

   HDC memdc, hDC;
   bm=::CreateCompatibleBitmap(dc, bmInfo.bmWidth, bmInfo.bmHeight);
   hDC=CreateCompatibleDC(dc);
   SelectObject(hDC, (HBITMAP) bm);
   memdc=CreateCompatibleDC(dc);
   SelectObject(memdc, (HBITMAP) src);
   BitBlt(hDC, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, memdc, 0, 0, SRCCOPY);
   DeleteDC(hDC);
   DeleteDC(memdc);

   ReleaseDC(NULL,dc);

   gpPicture->Release();
  /* -- already freed?..
   GlobalUnlock(hGlobal);
   GlobalFree(hGlobal);
   */
   free(buf);
   // add image to the cache
   add_image(fname,bm);
   return bm;
  } // else failed

  if(next && (priority==1))
  {
   bm=next->load_image(fname);
   if(bm!=0)
    return bm;
  }

  return NULL;
}

void *kFile::load_data(const TCHAR *fname,size_t *size,int *error) // caller should call free(m); returns 0 if failed
{ 
 if(next && (priority==0))
 {
  void *tmp;
  tmp=next->load_data(fname,size,error);
  if(tmp!=0)
   return tmp;
 }
 void *ret=0;
 if(af)
   ret=_load_from_arch(fname,size,error); 
 else 
   ret=_load_from_file(fname,size,error); 

 if(ret)
  return ret;

 if(next && (priority==1))
 {
  void *tmp;
  tmp=next->load_data(fname,size,error);
  if(tmp!=0)
   return tmp;
 }
 return NULL;
}

LANGID kFile::get_language()
{
 return current_language;
}

void kFile::set_language(LANGID lang) 
{
 current_language=lang; 
 if(next)
  next->set_language(lang);

 load_inis();
/*
 // set CodePage, too
 switch(lang)
 {
  case 0x409: // eng
  	1252
  case 0x407: // deu
  	1252
  case 0x80a: // esp
        1252
  case 0x419: // rus
  	1251 // 866
  case 0x415: // pol
  	1250
 }
*/
}

void kFile::attach_skin(kFile *n) 
{ 
 next=n; 
}

void kFile::detach_skin() 
{ 
 next=NULL; 
}

int kFile::get_profile(const TCHAR *section,const TCHAR *key_name,TCHAR *buff,int bufsize)
{
 if(next && (priority==0))
 {
  if(next->get_profile(section,key_name,buff,bufsize)==0)
  {
   return 0;
  }
  else
  {
   // check if inheritance is disabled
   TCHAR tmp_sect[12];
   if(next->get_profile(section,_T("no_inherit"),tmp_sect,sizeof(tmp_sect))==0)
    return -1;
  }
 }
         if(next!=0) // do not use 'default' skin for these sections:
         {
           if(((_tcsncmp(key_name,_T("page"),4)==0) && (_tcscmp(section,_T("all"))==0)) ||
          	 ((_tcsncmp(key_name,_T("slider"),6)==0) && (_istdigit(key_name[6]))))
             return -1;
         }

         struct _kxgui_ini_sections *first=sections;
         TCHAR *section_n=NULL;

         while(first)
         {
          if(_tcscmp(first->name,section)==0)
          {
           section_n=first->memory;
           break;
          }
          first=first->next;
         }

         if(section_n)
         for(int lang_cnt=0;lang_cnt<2;lang_cnt++)
         {  
           // look for section name
           TCHAR *p=section_n; // p points to section

           kString key; 
           if(lang_cnt==0 && current_language!=0)
             key.Format(_T("\r\n%s.%x="),key_name,current_language);
           else
             key.Format(_T("\r\n%s="),key_name);

           size_t len=_tcslen((LPCTSTR)key);

            p=_tcsstr(p,(LPCTSTR)key);

            if(p) // found
            {
               p+=len;
               TCHAR *d=_tcschr(p,'\r');

               TCHAR old_ch;
               if(d!=NULL)
               {
                old_ch=*d;
                *d=0;
               }
               else
               {
                old_ch=0;
               }

               _tcsncpy(buff,p,bufsize);
               if(d)
                *d=old_ch;

               // parse buffer
               for(unsigned i=0;i<_tcslen(buff);i++)
               {
                if(buff[i]=='\\')
                {
                 _tcscpy(&buff[i],&buff[i+1]);
                 switch(buff[i])
                 {
                  case 'n': buff[i]='\n'; break;
                  case 'r': buff[i]='\r'; break;
                  case 't': buff[i]='\t'; break;
                 }
                }
               }
               get_profile(_T("lang"),buff,buff,bufsize); // ignore ret code
               return 0;
            } // found?
         }// language

 if(next && (priority==1))
 {
  if(next->get_profile(section,key_name,buff,bufsize)==0)
  {
   return 0;
  }
 }

 return -1;
}

// ----------------------------------------------------------------------

void kFile::add_image(const TCHAR *fname,HBITMAP bm)
{
 struct _image_cache_t *ic=NULL;
 ic=(struct _image_cache_t *)malloc(sizeof(_image_cache_t));
 if(ic)
 {
  ic->name=new kString;
  ic->name->Format(_T("%s"),fname);
  ic->bm=bm;
  ic->next=cur_image_cache;   
  cur_image_cache=ic;
 }
}

// returns 0 if found;
int kFile::find_image(const TCHAR *fname,HBITMAP *bm)
{
 struct _image_cache_t *ic=cur_image_cache;
 while(ic)
 {
  if(_tcscmp(fname,(LPCTSTR)(*ic->name))==0)
  { *bm=ic->bm; return 0; }
  ic=ic->next;
 }
 return -1;
}

void kFile::flush_cache()
{
 struct _image_cache_t *ic=cur_image_cache;
 while(ic)
 {
  delete ic->name;
  DeleteObject(ic->bm);
  _image_cache_t *tmp=ic;
  ic=ic->next;
  free(tmp);
 }
 cur_image_cache=0;
}

int kFile::get_profile(const TCHAR *section,const TCHAR *key,const TCHAR *default_,kString &ret_string)
{
 TCHAR tmp_buffer[2048];
 int ret=get_profile(section,key,tmp_buffer,sizeof(tmp_buffer));
 if(!ret)
 {
  ret_string=tmp_buffer;
 }
 else
  ret_string=default_;

 return ret;
}

kString kFile::get_profile(const TCHAR *section,const TCHAR *key)
{
 TCHAR tmp_buffer[2048];
 TCHAR *ret_string=NULL;

 int ret=get_profile(section,key,tmp_buffer,sizeof(tmp_buffer));
 
 if(ret) // error
 {
  _stprintf(tmp_buffer,_T("%s %s ??"),section,key);
  debug(_T("[kxskin error]: [%s] '%s' - not found\n"),section,key);
 }

 ret_string=tmp_buffer;

 return kString(ret_string);
}

kString kFile::get_skin_name() 
{ 
 if(next && (priority==0)) 
   return next->get_skin_name(); 
 else return skin_name; 
};

kString kFile::get_skin_guid() 
{ 
 if(next && (priority==0)) 
    return next->get_skin_guid(); 
 else 
    return skin_guid; 
};

kString kFile::get_skin_file() 
{ 
  if(next && (priority==0))
    return next->get_skin_file(); 
  else 
    return skin_file; 
};

int kFile::get_full_skin_path(const TCHAR *file,TCHAR *out)
{
 GetModuleFileName(NULL,out,MAX_PATH);

 FILE *f;
 f=_tfopen(file,_T("rb"));
 if(f!=NULL)
 {
    fclose(f);
    _tcscpy(out,file);
 }
 else
 {
    TCHAR *p=0;
    p = _tcsrchr(out,'\\');
    if(p)
    {
        p++;
        if(_tcschr(p,'"'))
        {
         *p=0;
         _tcscat(out,file);
         _tcscat(out,_T("\""));
        }
        else
        {
         *p=0;
         _tcscat(out,file);
        }
    }
 }

 return 0;
}
