
; definitions to use with ufxsetup.dll

; functions
!define UFXSETUP_FUNC_CHK_VERSION   0 ; check for kx and its version
!define UFXSETUP_FUNC_PRE           1 ; close kxmixer
!define UFXSETUP_FUNC_DO            3 ; does the job (fixing eqg10 name)
!define UFXSETUP_FUNC_POST          2 ; start kxmixer

; errors
!define UFXSETUP_ERROR_NO_ERROR     '0'
!define UFXSETUP_ERROR_WRONG_FUNC   '1' 
!define UFXSETUP_ERROR_NO_KX        '2'   
!define UFXSETUP_ERROR_OLD_KX       '3'
!define UFXSETUP_ERROR_UNEXPECTED  '-1'

/* !macro FORMAT_CESETUP_ERROR 

    StrCmp $FAILED ${CESETUP_ERROR_NO_DEVICE_FOUND} 0 +3
        StrCpy $FAILED ""
        goto end_format
    StrCmp $FAILED ${CESETUP_ERROR_NO_DEVICE_UPDATED} 0 +3
        StrCpy $FAILED ""
        goto end_format
    StrCmp $FAILED ${CESETUP_ERROR_BAD_INF} 0 +3
        StrCpy $FAILED ""
        goto end_format
        
    StrCpy $FAILED "<Unexpected error, code: $FAILED>\n"
    end_format:
    
!macroend */

!macro UFXSETUP FUNC
    System::Call "${SETUPDLL_FILE}::entry(i ${UFXSETUP_FUNC_${FUNC}}) i .r0"
!macroend

!macro SETUP_UNLOAD_DLLS
    SetPluginUnload manual
    System::Call "${SETUPDLL_FILE}::entry(i -1) ? u" ; unload ufxsetup
    System::Free 0                                   ; unload System
!macroend