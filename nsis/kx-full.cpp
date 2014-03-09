
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

#include <vers.h>

; kX Audio Driver package
; KX_COPYRIGHT_STR

  !define HTTPD_ROOT                                KX_HTTPD_ROOT
  !define SystemRoot                                KX_SYSTEM_ROOT
  !define DONE_FOLDER								CE_DONE
  !define SDK_ROOT									KXSDK_ROOT

  !define PRODUCT                                   "kX Project Audio Driver"
  !define MUI_FOLDER                                "kX Audio Driver"
  !define PRODUCT_VERSION                           KX_ASIO_VERSION
  !define MUI_DATE                                  __DATE__

  !define MUI_STARTMENUPAGE_REGISTRY_ROOT           "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY            "Software\kX\NSIS Setup" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME      "Start Menu Folder"
  !define MUI_STARTMENU_VARIABLE                    "kX Audio Driver"
  !define MUI_STARTMENUPAGE_DEFAULTFOLDER           "kX Audio Driver"

  !define MUI_DISTRIB                               "kxdrv${PRODUCT_VERSION}-full.exe"
  !define SETUP_NAME                                "kX Project Audio Driver"
  
  !define INSTALL_DIR                               "kX Project"

  !define MUI_WELCOMEPAGE
  !define MUI_LICENSEPAGE
    !define MUI_LICENSEPAGE_CHECKBOX
  !define MUI_FINISHPAGE

  !define MUI_ICON              "Installer.ico"
  !define MUI_UNICON            "UnInstaller.ico"
  !define MUI_FONT              "Tahoma"
  !define MUI_INSTALLCOLORS     "/windows"
  !define MUI_PROGRESSBAR       "smooth"
  !define MUI_BRANDINGTEXT      KX_COPYRIGHT_STR
  !define MUI_ABORTWARNING

  !define MUI_UNINSTALLER
  !define MUI_UNWELCOMEPAGE
  !define MUI_UNCONFIRMPAGE
  !define MUI_UNFINISHPAGE

  !include "MUI2.nsh"
  !include "x64.nsh"

  XPStyle                 on
  Name                    "${SETUP_NAME}"
  OutFile                 "${DONE_FOLDER}\${MUI_DISTRIB}"
  ViProductVersion        "${PRODUCT_VERSION}"
  LicenseData             "${HTTPD_ROOT}\help\license.txt"
  RequestExecutionLevel   admin

  !define MUI_WELCOMEFINISHPAGE_BITMAP "kxinst.bmp"
  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_BITMAP "header_image.bmp"

  ; Languages
  ; kX-supported:

  !insertmacro MUI_LANGUAGE "English"

  !insertmacro MUI_LANGUAGE "PortugueseBR"
  !insertmacro MUI_LANGUAGE "Bulgarian"
  !insertmacro MUI_LANGUAGE "Croatian"
  !insertmacro MUI_LANGUAGE "Danish"
  !insertmacro MUI_LANGUAGE "Dutch"
  !insertmacro MUI_LANGUAGE "Finnish"
  !insertmacro MUI_LANGUAGE "French"
  !insertmacro MUI_LANGUAGE "German"
  !insertmacro MUI_LANGUAGE "Greek"
  !insertmacro MUI_LANGUAGE "Hungarian"
  !insertmacro MUI_LANGUAGE "Italian"
  !insertmacro MUI_LANGUAGE "Japanese"
  !insertmacro MUI_LANGUAGE "Korean"
  !insertmacro MUI_LANGUAGE "Norwegian"
  !insertmacro MUI_LANGUAGE "Polish"
  !insertmacro MUI_LANGUAGE "Portuguese"
  !insertmacro MUI_LANGUAGE "Romanian"
  !insertmacro MUI_LANGUAGE "Russian"
  !insertmacro MUI_LANGUAGE "SimpChinese"
  !insertmacro MUI_LANGUAGE "TradChinese"
  !insertmacro MUI_LANGUAGE "Spanish"
  !insertmacro MUI_LANGUAGE "Swedish"
  !insertmacro MUI_LANGUAGE "Thai"
  !insertmacro MUI_LANGUAGE "Ukrainian"

  ; additional:
  !insertmacro MUI_LANGUAGE "Albanian"
  !insertmacro MUI_LANGUAGE "Arabic"
  !insertmacro MUI_LANGUAGE "Belarusian"
  !insertmacro MUI_LANGUAGE "Bosnian"
  !insertmacro MUI_LANGUAGE "Breton"
  !insertmacro MUI_LANGUAGE "Catalan"
  !insertmacro MUI_LANGUAGE "Czech"
  !insertmacro MUI_LANGUAGE "Estonian"
  !insertmacro MUI_LANGUAGE "Farsi"
  !insertmacro MUI_LANGUAGE "Hebrew"
  !insertmacro MUI_LANGUAGE "Icelandic"
  !insertmacro MUI_LANGUAGE "Indonesian"
  !insertmacro MUI_LANGUAGE "Kurdish"
  !insertmacro MUI_LANGUAGE "Latvian"
  !insertmacro MUI_LANGUAGE "Lithuanian"
  !insertmacro MUI_LANGUAGE "Luxembourgish"
  !insertmacro MUI_LANGUAGE "Macedonian"
  !insertmacro MUI_LANGUAGE "Malay"
  !insertmacro MUI_LANGUAGE "Mongolian"
  !insertmacro MUI_LANGUAGE "Serbian"
  !insertmacro MUI_LANGUAGE "Slovak"
  !insertmacro MUI_LANGUAGE "Turkish"

  !insertmacro MUI_RESERVEFILE_LANGDLL

Function .onInit

    DetailPrint "== Initializing setup...$\n"

    UserInfo::GetAccountType
    Pop $0
    StrCmp $0 'Admin' admin_ok
       DetailPrint "Setup needs Admin account, your account is $0$\n"
       MessageBox MB_ICONEXCLAMATION|MB_OK "In order to install the software under this operating system you must be logged-in with Administrator privileges.$\n$\nSetup cannot continue."
       SetErrorLevel 24 // CESETUP_ERROR_NEED_ADMIN
       Abort
    admin_ok:

    DetailPrint "== Start-up OK$\n"

    // redirection must be enabled here, otherwise license agreement will not be shown
    InitPluginsDir
    File /oname=$PLUGINSDIR\splash.bmp "Splash.bmp"

    splash::show 1000 $PLUGINSDIR\splash

    Pop $0 ; $0 has '1' if the user closed the splash screen early,
           ; '0' if everything closed normal, and '-1' if some error occured.

    !insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd
 
  ; BEGIN
  ; -----

  Var STARTMENU_FOLDER
  Var MUI_TEMP

  !define MUI_WELCOMEPAGE_TITLE_3LINES
  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "${HTTPD_ROOT}\help\license.txt"
  !insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER
  !insertmacro MUI_PAGE_INSTFILES
  !define MUI_FINISHPAGE_TITLE_3LINES
  !insertmacro MUI_PAGE_FINISH

  !define MUI_WELCOMEPAGE_TITLE_3LINES
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !define MUI_FINISHPAGE_TITLE_3LINES
  !insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Installer Sections

Section "Driver" Driver

    DetailPrint "== Driver install...$\n"

    // close x86 (redirected) kxmixer.exe, if necessary
    ExecWait '"$SYSDIR\kxmixer" --quit'
    ExecWait '"$INSTDIR\kxmixer" --quit'

    ${If} ${RunningX64}
       SetRegView 64
       ${DisableX64FSRedirection}
    ${EndIf}

    ${If} ${RunningX64}
       StrCpy $INSTDIR "$PROGRAMFILES64\${INSTALL_DIR}"
    ${Else}
       StrCpy $INSTDIR "$PROGRAMFILES\${INSTALL_DIR}"
    ${EndIf}

    // close x64 (native) kxmixer.exe, if necessary
    ExecWait '"$SYSDIR\kxmixer" --quit'
    ExecWait '"$INSTDIR\kxmixer" --quit'

    // Copy files:
    DetailPrint "== Copying files...$\n"

    SetOutPath "$INSTDIR"

    ${If} ${RunningX64}
            // main 64-bit components
            File /a "${DONE_FOLDER}\..\amd64\kx.inf"
            File /a "${DONE_FOLDER}\..\amd64\kx.sys"
            File /a "${DONE_FOLDER}\..\amd64\kx.cat"
            File /a "${DONE_FOLDER}\..\amd64\kxapi.dll"
            File /a "${DONE_FOLDER}\..\amd64\kxasio.dll"
            File /a "${DONE_FOLDER}\..\amd64\kxgui.dll"
            File /a "${DONE_FOLDER}\..\amd64\sfman32.dll"
            File /a "${DONE_FOLDER}\..\amd64\kxctrl.exe"
            File /a "${DONE_FOLDER}\..\amd64\edspctrl.exe"
            File /a "${DONE_FOLDER}\..\amd64\kxmixer.exe"
            File /a "${DONE_FOLDER}\..\amd64\kxsetup.exe"
            File /a "${DONE_FOLDER}\..\amd64\kXi.dll"
            File /a "${DONE_FOLDER}\..\amd64\kxsfi.dll"
            File /a "${DONE_FOLDER}\..\amd64\kxfxlib.kxl"
            File /a "${DONE_FOLDER}\..\amd64\kxaddons.kxa"
            File /a "${DONE_FOLDER}\..\amd64\kxhelp.chm"
            File /a "${DONE_FOLDER}\..\amd64\readme.html"
            File /a "${DONE_FOLDER}\..\amd64\kxskin.kxs"

            // fx pack
            File /a "${DONE_FOLDER}\..\amd64\kxfxpack.kxl"
            File /a "${DONE_FOLDER}\..\amd64\kxdynamica.kxl"
            File /a "${DONE_FOLDER}\..\amd64\kxfxmixy42.kxl"
            File /a "${DONE_FOLDER}\..\amd64\kxfxmixy82.kxl"
            File /a "${DONE_FOLDER}\..\amd64\kxfxloudness.kxl"
            File /a "${DONE_FOLDER}\..\amd64\kxfxrouter.kxl"
            File /a "${DONE_FOLDER}\..\amd64\kxfxadc.kxl"
            File /a "${DONE_FOLDER}\..\amd64\kxefxtube.kxl"
            File /a "${DONE_FOLDER}\..\amd64\kxefxreverb.kxl"
            File /a "${DONE_FOLDER}\..\amd64\kxefx.dll"
            File /a "${DONE_FOLDER}\..\amd64\kxefx.kxs"
            File /a "${DONE_FOLDER}\..\amd64\kxTimeBalanceV2A.kxs"
            File /a "${DONE_FOLDER}\..\amd64\kxTimeBalanceV2B.kxs"
            File /a "${DONE_FOLDER}\..\amd64\kxm120.kxl"
            // File /a "${DONE_FOLDER}\..\amd64\sa.minus.kxl"

            File "${DONE_FOLDER}\..\amd64\cmax20.dll"

            // also install x86 versions into program files 32 => "$PROGRAMFILES32\${COMPANY_NAME}\${FOLDER_NAME}"
            CreateDirectory "$PROGRAMFILES32\${INSTALL_DIR}"
            SetOutPath "$PROGRAMFILES32\${INSTALL_DIR}"
    ${EndIf}

     // main 32-bit components
     ${IfNot} ${RunningX64}
        // do not install drivers
        File /a "${DONE_FOLDER}\..\i386\kx.inf"
        File /a "${DONE_FOLDER}\..\i386\kx.sys"
        File /a "${DONE_FOLDER}\..\i386\kx.cat"
     ${EndIf}

     File /a "${DONE_FOLDER}\..\i386\kxapi.dll"
     File /a "${DONE_FOLDER}\..\i386\kxasio.dll"
     File /a "${DONE_FOLDER}\..\i386\kxgui.dll"
     File /a "${DONE_FOLDER}\..\i386\sfman32.dll"
     File /a "${DONE_FOLDER}\..\i386\kxctrl.exe"
     File /a "${DONE_FOLDER}\..\i386\edspctrl.exe"
     File /a "${DONE_FOLDER}\..\i386\kxmixer.exe"
     ${IfNot} ${RunningX64}
        // do not install setup
        File /a "${DONE_FOLDER}\..\i386\kxsetup.exe"
     ${EndIf}
     File /a "${DONE_FOLDER}\..\i386\kXi.dll"
     File /a "${DONE_FOLDER}\..\i386\kxsfi.dll"
     File /a "${DONE_FOLDER}\..\i386\kxfxlib.kxl"
     File /a "${DONE_FOLDER}\..\i386\kxaddons.kxa"
     File /a "${DONE_FOLDER}\..\i386\kxhelp.chm"
     File /a "${DONE_FOLDER}\..\i386\readme.html"
     File /a "${DONE_FOLDER}\..\i386\kxskin.kxs"

     // fx pack
     File /a "${DONE_FOLDER}\..\i386\kxfxpack.kxl"
     File /a "${DONE_FOLDER}\..\i386\kxdynamica.kxl"
     File /a "${DONE_FOLDER}\..\i386\kxfxmixy42.kxl"
     File /a "${DONE_FOLDER}\..\i386\kxfxmixy82.kxl"
     File /a "${DONE_FOLDER}\..\i386\kxfxloudness.kxl"
     File /a "${DONE_FOLDER}\..\i386\kxfxrouter.kxl"
     File /a "${DONE_FOLDER}\..\i386\kxfxadc.kxl"
     File /a "${DONE_FOLDER}\..\i386\kxefxtube.kxl"
     File /a "${DONE_FOLDER}\..\i386\kxefxreverb.kxl"
     File /a "${DONE_FOLDER}\..\i386\kxefx.dll"
     File /a "${DONE_FOLDER}\..\i386\kxefx.kxs"
     File /a "${DONE_FOLDER}\..\i386\kxTimeBalanceV2A.kxs"
     File /a "${DONE_FOLDER}\..\i386\kxTimeBalanceV2B.kxs"
     File /a "${DONE_FOLDER}\..\i386\kxm120.kxl"
     // File /a "${DONE_FOLDER}\..\i386\sa.minus.kxl"

     File /a "${DONE_FOLDER}\..\i386\cmax20.dll"

    // install 3-rd party 32-bit only components

    File /a "..\kxfx_profx\ProFx.kxl"
    File /A "..\kxfx_ufx\ufx.kxl"
    File /A "..\kxfx_ufx\ufxrc.dll"
    File /A "..\kxfx_synth\asynth.kxl"
    File /A "..\kxfx_synth\drumsynth.kxl"
    File /A "..\kxfx_synth\organ.kxl"
    // back to "Program Files" (x64, if necessary)
    SetOutPath "$INSTDIR"

    // SDK

    SetOutPath "$INSTDIR\SDK"
         File /r "${SDK_ROOT}\*.*"
    SetOutPath "$INSTDIR"

    // Uninstaller

    WriteUninstaller "$INSTDIR\Uninstall.exe"

    // Start Menu shortcuts:

    SetShellVarContext all        // instruct to use start menu items from 'all users' group ($SMPROGRAMS)

    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
        CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
        CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\kX Mixer.lnk" "$INSTDIR\kxmixer.exe"
        ${If} ${RunningX64}
            CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\kX Mixer (32-bit).lnk" "$PROGRAMFILES32\${INSTALL_DIR}\kxmixer.exe"
        ${EndIf}
        CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Browse the SDK.lnk" "$INSTDIR\SDK"
        CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\kX Help.lnk" "$INSTDIR\kxhelp.chm"
        CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall kX Project audio driver.lnk" "$INSTDIR\Uninstall.exe"
    !insertmacro MUI_STARTMENU_WRITE_END

    ExecShell "open" "$INSTDIR\readme.html"

    HideWindow

    ClearErrors
    // Set current directory once again (kxsetup needs to run from this folder)
    SetOutPath "$INSTDIR"
    ExecWait '"$INSTDIR\kxsetup" --install'
    Quit

SectionEnd

Section "Uninstall"

  // instruct to place start menu items into 'all users' group ($SMPROGRAMS)
  SetShellVarContext all

  ${If} ${RunningX64}
    ${DisableX64FSRedirection}
  ${EndIf}

  ${If} ${RunningX64}
     StrCpy $INSTDIR "$PROGRAMFILES64\${INSTALL_DIR}"
  ${Else}
     StrCpy $INSTDIR "$PROGRAMFILES\${INSTALL_DIR}"
  ${EndIf}

  ExecWait '"$INSTDIR\kxsetup" --clean'

  // instdir is \pm\kx
  RmDir /r "$INSTDIR\SDK"

  Delete "$INSTDIR\Uninstall.exe"

  // instdir is \pm\kx
  RmDir /r "$INSTDIR"

  // repeat for 32-bit components
  ${If} ${RunningX64}
    RmDir /r "$PROGRAMFILES32\${INSTALL_DIR}"
  ${EndIf}

  ;Remove shortcut
  !insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP
    
  StrCmp $MUI_TEMP "" noshortcuts

    Delete "$SMPROGRAMS\$MUI_TEMP\kX Mixer.lnk"
    Delete "$SMPROGRAMS\$MUI_TEMP\kX Mixer (32-bit).lnk"
    Delete "$SMPROGRAMS\$MUI_TEMP\Browse the SDK.lnk"
    Delete "$SMPROGRAMS\$MUI_TEMP\kX Help.lnk"
    Delete "$SMPROGRAMS\$MUI_TEMP\Uninstall kX Project audio driver.lnk"

  RMDir "$SMPROGRAMS\$MUI_TEMP" ;Only if empty, so it won't delete other shortcuts

noshortcuts:

SectionEnd

Function un.onInit

  !insertmacro MUI_UNGETLANGUAGE
  
FunctionEnd
