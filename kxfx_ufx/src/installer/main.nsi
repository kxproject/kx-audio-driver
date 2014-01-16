
;..............................................................................
; Common Stuff

!define PRODUCT_NAME   "UFX Plugins"
 
Name                   "${PRODUCT_NAME}"
Caption                "UFX 1.60.3551 Setup"
InstallDir             "$PROGRAMFILES\kX Audio Driver\UFX"
BrandingText           " "
ShowInstDetails        nevershow
ShowUninstDetails      nevershow

OutFile                "ufxsetup.51.exe"

!define KXPLUGINS_KEY  "Software\kX\Plugins"
!define UNINSTALL_KEY  "Software\Microsoft\Windows\CurrentVersion\Uninstall"

;..............................................................................
; Files

!define FILES_SRC_DIR "data" ; source files directory

!define ICON_FILE     "${FILES_SRC_DIR}\install.ico"
!define LICENSE_FILE  "${FILES_SRC_DIR}\license.txt"

!define SETUPDLL_FILE "ufxsetup"  ; w/o ext

!macro PACKAGE_FILES_LIST CMD DIR ; files to be installed (do not change or remove '${CMD} "${DIR}\')
    ${CMD} "${DIR}\ufx.kxl" 
    ${CMD} "${DIR}\ufxrc.dll"
!macroend

!macro PACKAGE_EXTRA_FILES_LIST CMD DIR ; files to be installed to 'ext' dir
    ${CMD} "${DIR}\p.da"
    ${CMD} "${DIR}\t.da"
    ${CMD} "${DIR}\s.da"
    ${CMD} "${DIR}\w.da" 
    ${CMD} "${DIR}\h.da"
!macroend
    
!macro INST_FILES_LIST CMD DIR    ; files used during installation
    ${CMD} "${DIR}\${SETUPDLL_FILE}.dll"
!macroend

!macro UNINST_FILES_LIST CMD DIR  ; files needed for uninstallation  
    ${CMD} "${DIR}\${SETUPDLL_FILE}.dll" 
!macroend

;..............................................................................
; Plugins

!macro PLUGIN_REG ID GUID
    WriteRegStr HKCU "${KXPLUGINS_KEY}" "${GUID}" "${PLUGINS_DLL}" 
    WriteRegStr HKCU "${KXPLUGINS_KEY}" "${GUID}.name" "${ID}"
!macroend

!macro PLUGIN_UNREG ID GUID
    DeleteRegValue HKCU "${KXPLUGINS_KEY}" "${GUID}" 
    DeleteRegValue HKCU "${KXPLUGINS_KEY}" "${GUID}.name"
!macroend

!macro UFX_PLUGINS_LIST CMD 
    !insertmacro DEFINE PLUGINS_DLL "$INSTDIR\ufx.kxl"    
        !insertmacro ${CMD} "EQ G10"       80100005-0ADF-11D6-BFBC-D4F706E10C52
        !insertmacro ${CMD} "Timbre"       80100004-0ADF-11D6-BFBC-D4F706E10C52
        !insertmacro ${CMD} "Reverb R"     80100009-0ADF-11D6-BFBC-D4F706E10C52
        !insertmacro ${CMD} "GainX"        E063BF42-054C-4485-B11D-68BCA792AEA8
        !insertmacro ${CMD} "EQ P5"        8010000F-0ADF-11D6-BFBC-D4F706E10C52
        !insertmacro ${CMD} "EQ P5 (Mono)" 8010000F-0ADF-11D7-BFBC-D4F706E10C52
        !insertmacro ${CMD} "EQ P1"        8010000D-0ADF-11D6-BFBC-D4F706E10C52
        !insertmacro ${CMD} "EQ P1 (Mono)" 8010000D-0ADF-11D7-BFBC-D4F706E10C52
        !insertmacro ${CMD} "PeakX"        80100010-0ADF-11D6-BFBC-D4F706E10C52
        !insertmacro ${CMD} "Crossover-4"  9211B061-7871-11D8-B0D1-8B8FB634CD13
        !insertmacro ${CMD} "Crossover-2"  993A73A0-7893-11D8-B0D1-8B8FB634CD13 
!macroend

!macro NATIVE_PLUGINS_LIST CMD ; used to restore native plugins when uninstalling
    !insertmacro DEFINE PLUGINS_DLL "kxfxlib.kxl"
        !insertmacro ${CMD} "10 Band EQ"   80100005-0ADF-11D6-BFBC-D4F706E10C52
        !insertmacro ${CMD} "Timbre"       80100004-0ADF-11D6-BFBC-D4F706E10C52
!macroend

;..............................................................................
; Include

!include "extra.nsh"                 ; extra plugins
!include "mui.nsh"                   ; using MUI
!include "messages.nsh"              ; all the messages and text the user will see
!include "details\details.nsh"       ; MUI customization
!include "details\ufxsetup.dll.nsh"  ; ufxsetup interface


;..............................................................................
; Install Pages

!define MUI_ICON                 ${ICON_FILE}
!define MUI_UNICON               ${ICON_FILE}
!define MUI_ABORTWARNING
!define MUI_ABORTWARNING_TEXT    "${ABORTWARNING_TEXT}"
;!define MUI_CUSTOMFUNCTION_ABORT userAbort

!insertmacro MUI_PAGE_ WELCOME "${PAGE_WELCOME_TITLE}" "${PAGE_WELCOME_TEXT}" "${PAGE_WELCOME_BUTTON}"

!insertmacro MUI_PAGE_ INSTFILES "Querying Environment..." "" "" ; check kx version

;!define PAGE_SUBFUNCTION_PRE SkipIfNotFailed
;!insertmacro MUI_PAGE_ FINISH "${PAGE_WRONG_TITLE}" "${PAGE_WRONG_TEXT}" "${PAGE_WRONG_BUTTON}"

!insertmacro SKIP_IF_FAILED
    !define CUSTOM_LICENSE_PAGE 1 ; set to 0 to use default MUI license page
    !insertmacro PAGE_LICENSE "${LICENSE_FILE}" "${PAGE_LICENSE_BOTTOM}" "${PAGE_LICENSE_BUTTON}"

!insertmacro SKIP_IF_FAILED
    !insertmacro MUI_PAGE_ DIRECTORY "" "" ""

!insertmacro SKIP_IF_FAILED
    !insertmacro MUI_PAGE_ INSTFILES "Installing Files..." "" ""

!insertmacro MODY_IF_FAILED "${PAGE_WRONG_TITLE}" "${PAGE_WRONG_TEXT}" "${PAGE_WRONG_BUTTON}"
    !insertmacro MUI_PAGE_ FINISH "${PAGE_FINISH_TITLE}" "${PAGE_FINISH_TEXT}" "${PAGE_FINISH_BUTTON}"

;..............................................................................
; Uninstall Pages

!insertmacro MUI_UNPAGE_INSTFILES

     !define MUI_FINISHPAGE_TITLE  "${UNPAGE_FINISH_TITLE}"
     !define MUI_FINISHPAGE_TEXT   "${UNPAGE_FINISH_TEXT}"
     !define MUI_FINISHPAGE_BUTTON "${UNPAGE_FINISH_BUTTON}"
!insertmacro MUI_UNPAGE_FINISH

; language
!insertmacro MUI_LANGUAGE "English"

;..............................................................................
; Global Variables

Var STEP

;..............................................................................
; Install Section

Section "Install"  InstallSec

    SetPluginUnload alwaysoff

    StrCmp $STEP '1' STEP_1
    StrCpy $STEP '1'
    
    ; extract setup files
    SetOutPath   $PLUGINSDIR
    !insertmacro INST_FILES_LIST File ${FILES_SRC_DIR}
    
    DetailPrint "Searching for kX Audio Driver ..."
    !insertmacro UFXSETUP CHK_VERSION
    DetailPrint "OK"
    IntFmt $FAILED "%i" $0
    
    return
    
STEP_1:

    DetailPrint "Waiting for kX Mixer to exit ..."
    !insertmacro UFXSETUP PRE
    Sleep 500
    
    ; ufx files
    SetOutPath   "$INSTDIR"
    !insertmacro PACKAGE_FILES_LIST File ${FILES_SRC_DIR}
    
    SetOutPath   "$INSTDIR\ext"
    !insertmacro PACKAGE_EXTRA_FILES_LIST File "${FILES_SRC_DIR}\ext"
    
    ; registry uninstaller
    WriteUninstaller "$INSTDIR\uninst.exe"
    WriteRegStr HKLM "${UNINSTALL_KEY}\${PRODUCT_NAME}" "DisplayName" "${PRODUCT_NAME}"
    WriteRegStr HKLM "${UNINSTALL_KEY}\${PRODUCT_NAME}" "DisplayIcon" "$INSTDIR\ufxrc.dll, 0"
    WriteRegStr HKLM "${UNINSTALL_KEY}\${PRODUCT_NAME}" "UninstallString" "$INSTDIR\uninst.exe"
    
    ; register plugins
    !insertmacro UFX_PLUGINS_LIST PLUGIN_REG
    
    ; register extra plugins
    !insertmacro EXT_PLUGINS DA_PLUGIN_REG
    
    DetailPrint "Waiting for kX Mixer to launch ..."
    !insertmacro UFXSETUP POST

    ; fixme, should be before UFXSETUP POST, 
    ; but for some reason kx->rename_microcode does not work w/o kxmixer
    !insertmacro UFXSETUP DO

SectionEnd
    
;..............................................................................
; Uninstall Section
    
Section "Uninstall" UninstallSec

    ; extract setup files
    SetOutPath   $PLUGINSDIR
    !insertmacro INST_FILES_LIST File ${FILES_SRC_DIR}
    
    SetPluginUnload alwaysoff
    
    DetailPrint "Waiting for kX Mixer to exit ..."
    !insertmacro UFXSETUP PRE
    Sleep 500
    
    ; registry uninstaller
    DeleteRegKey   HKLM "${UNINSTALL_KEY}\${PRODUCT_NAME}"
    
    ; unregister plugins
    !insertmacro UFX_PLUGINS_LIST PLUGIN_UNREG
    !insertmacro NATIVE_PLUGINS_LIST PLUGIN_REG ; restore native plugins
    
    ; unregister extra plugins
    !insertmacro EXT_PLUGINS DA_PLUGIN_UNREG
    
    ; files
    !insertmacro PACKAGE_FILES_LIST       Delete "$INSTDIR"
    !insertmacro PACKAGE_EXTRA_FILES_LIST Delete "$INSTDIR\ext"
    Delete "$INSTDIR\uninst.exe"
    RMDir  "$INSTDIR\ext"
    RMDir  "$INSTDIR"
    
    !insertmacro UFXSETUP POST
    
SectionEnd
    
;..............................................................................
; Functions
    
Function .onInit
    StrCpy $FAILED '0' ; declared in details.nsh
    !insertmacro MUI_INSTALLOPTIONS_EXTRACT_AS "details\page_license.ini" "page_license.ini"
FunctionEnd

Function un.onInit
    MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "${UNPAGE_CONFIRM_TEXT}" IDYES +2
    Abort
FunctionEnd

Function .onGUIEnd
    !insertmacro SETUP_UNLOAD_DLLS
FunctionEnd

;..............................................................................


