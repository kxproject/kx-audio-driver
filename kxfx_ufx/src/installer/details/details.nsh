
; macros and fuctions to help with deeper MUI customization

;..............................................................................
; Global Variables

Var FAILED

;..............................................................................
; Standard Macros

!macro DEFINE SYMBOL VAR
    !ifdef "${SYMBOL}"
        !undef "${SYMBOL}"
    !endif
    !define "${SYMBOL}" "${VAR}"
!macroend

;..............................................................................

!macro SKIP_IF_FAILED
    !insertmacro MUI_UNSET PAGE_SUBFUNCTION_PRE
    !define PAGE_SUBFUNCTION_PRE SkipIfFailed_${__LINE__}
    Function ${PAGE_SUBFUNCTION_PRE}
        StrCmp $FAILED '0' +2
            abort
    FunctionEnd
!macroend

!macro MODY_IF_FAILED TITLE TEXT BUTTON ; for simplicity, it's hardcoded for "ioSpecial.ini" only
    !insertmacro MUI_UNSET PAGE_SUBFUNCTION_PRE
    !define PAGE_SUBFUNCTION_PRE ModyIfFailed_${__LINE__}
    Function ${PAGE_SUBFUNCTION_PRE}
        StrCmp $FAILED '0' 0 +2
            return
        !insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Field 2" "Text" "${TITLE}"
        !insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Field 3" "Text" "${TEXT}"
        !insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Settings" "NextButtonText" "${BUTTON}"
    FunctionEnd
!macroend

!macro MUI_PAGE_ PAGE TITLE TEXT BUTTON

    !if ${PAGE} == INSTFILES
    
        !define MUI_PAGE_HEADER_TEXT    "${TITLE}"
        !define MUI_PAGE_HEADER_SUBTEXT "${TEXT}"
        ; note: BUTTON has no effect on INSTFILES
        
    !else if ${PAGE} == DIRECTORY
    
        ; nothing
        
    !else
    
        !define MUI_${PAGE}PAGE_TITLE   "${TITLE}"
        !define MUI_${PAGE}PAGE_TEXT    "${TEXT}"
             
    !endif
    
        !define MUI_PAGE_CUSTOMFUNCTION_PRE PageFunctionPre_${__LINE__} 
        Function ${MUI_PAGE_CUSTOMFUNCTION_PRE}
            !insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Settings" "NextButtonText" "${BUTTON}"
            !ifdef PAGE_SUBFUNCTION_PRE
                call ${PAGE_SUBFUNCTION_PRE}
            !endif
        FunctionEnd 
    
    !insertmacro MUI_PAGE_${PAGE}
    
    !insertmacro MUI_UNSET SKIP_IF_FAILED
    !insertmacro MUI_UNSET MODY_IF_FAILED
                
!macroend

;..............................................................................



!macro PAGE_LICENSE DATA TEXT BUTTON

    !if ${NSIS_MAX_STRLEN} < 8192
        !error "custom license page is works only with 'large strings' NSIS build$\n$\t(see http://nsis.sourceforge.net/Special_Builds)$\n$\t- using default MUI page."
        !undef  CUSTOM_LICENSE_PAGE
        !define CUSTOM_LICENSE_PAGE 0
    !endif 

    LicenseLangString License ${LANG_ENGLISH} ${DATA}
    !define LICENSE_PAGE_FUNCTION LisencePage_${__LINE__}
    Page custom LICENSE_PAGE_FUNCTION
    
    Function LICENSE_PAGE_FUNCTION
        !ifdef PAGE_SUBFUNCTION_PRE
            call ${PAGE_SUBFUNCTION_PRE}
        !endif
        
        !insertmacro MUI_HEADER_TEXT "License Agreement" "Please review the license terms before installing this software."
        !insertmacro MUI_INSTALLOPTIONS_WRITE      "page_license.ini" "Field 1" "State" "$(License)"
        !insertmacro MUI_INSTALLOPTIONS_WRITE      "page_license.ini" "Field 2" "Text" "${TEXT}"
        !insertmacro MUI_INSTALLOPTIONS_WRITE      "page_license.ini" "Settings" "NextButtonText" "${BUTTON}"
        !insertmacro MUI_INSTALLOPTIONS_WRITE      "page_license.ini" "Settings" "BackEnabled" "0"
        !insertmacro MUI_INSTALLOPTIONS_INITDIALOG "page_license.ini"
        Pop $0
        GetDlgItem $0 $0 1200
        SetCtlColors $0 0 0xffffff
        !insertmacro MUI_INSTALLOPTIONS_SHOW
        
    FunctionEnd
    
    !undef LICENSE_PAGE_FUNCTION
    
    !insertmacro MUI_UNSET SKIP_IF_FAILED
    
!macroend

