# kX Driver
# Copyright (c) Eugene Gavrilov, 2001-2014
# All rights reserved

# Configuration

COPY=copy /b /d
REGSVR32=regsvr32
DONE=$(OBJECT_ROOT)\done\$(TARGET_CPU)
SAY=@echo ==

INF_SOURCE=kx
BLD_INF_FNAME=kx
BLD_CAT_FNAME=kx

HTTPD_ROOT=..\www
C_DEFINES=$(C_DEFINES) -DKX_DEBUG 
SETUP_SETUP_FILE=kxdrv$(KX_VERSION)-full.exe
KXSDK_ROOT=$(DONE)\..\SDK

# Code signing:
SIGNTOOL=SignTool sign /ac ..\VeriSignG5.cer /s my /n "CEntrance, Inc."
# SIGNTOOL=echo -- Warning: no code signing
!if "$(TIMESTAMP_SIGN)" == "YES"
SIGNTOOL=$(SIGNTOOL) /t http://timestamp.verisign.com/scripts/timestamp.dll
!endif
INF2CAT=inf2cat.exe

!if "$(TARGET_ARCH)" == "amd64"
# note: nmake is 32-bit application, thus, it cannot install files into 'true' 64-bit system32 and system32/drivers
# workaround here:
WINSYS32DIR=$(WINDIR)\Sysnative
!else
WINSYS32DIR=$(WINDIR)\System32
!endif

# generate map files:
LINKER_FLAGS=$(LINKER_FLAGS) /MAP /MAPINFO:EXPORTS

# don't ignore error codes
!CMDSWITCHES -I

# common targets:

sign: $(DONE)\$(TARGETNAME).$(TARGETEXT)
	$(SAY) Signing $(TARGETNAME).$(TARGETEXT)...
	$(SIGNTOOL) $(DONE)\$(TARGETNAME).$(TARGETEXT)

register_dll: $(DONE)\$(TARGETNAME).$(TARGETEXT)
	$(SAY) Registering with regsvr32...
	$(REGSVR32) /s $(DONE)\$(TARGETNAME).$(TARGETEXT)

$(DONE)\$(TARGETNAME).$(TARGETEXT): create_folders $(OBJ_PATH)\$(O)\$(TARGETNAME).$(TARGETEXT)
	$(SAY) Installing into $(DONE)...
	$(COPY) $(OBJ_PATH)\$(O)\$(TARGETNAME).$(TARGETEXT) $(SKINFILE) $(DONE)\$(TARGETNAME).$(TARGETEXT)

install: $(DONE)\$(TARGETNAME).$(TARGETEXT)
	$(SAY) Installed into $(DONE)

install_sign: $(DONE)\$(TARGETNAME).$(TARGETEXT) sign
	$(SAY) Installed and signed $(DONE)\$(TARGETNAME).$(TARGETEXT)

$(DONE)\$(TARGETNAME).pdb: create_folders $(OBJ_PATH)\$(O)\$(TARGETNAME).pdb
	$(SAY) Installing PDB into $(DONE)...
	$(COPY) $(OBJ_PATH)\$(O)\$(TARGETNAME).pdb $(DONE)\$(TARGETNAME).pdb

pdb: $(DONE)\$(TARGETNAME).pdb
	$(SAY) PDB copied into $(DONE)

$(WINSYS32DIR)\$(TARGETNAME).$(TARGETEXT): $(DONE)\$(TARGETNAME).$(TARGETEXT)
	$(SAY) Installing into $(WINDIR)\System32...
	$(COPY) $(DONE)\$(TARGETNAME).$(TARGETEXT) $(WINSYS32DIR)

install_sysdir: $(WINSYS32DIR)\$(TARGETNAME).$(TARGETEXT)
	$(SAY) Installed into System32 folder

$(WINSYS32DIR)\drivers\$(TARGETNAME).$(TARGETEXT): $(DONE)\$(TARGETNAME).$(TARGETEXT)
	$(SAY) Installing into $(WINDIR)\System32\Drivers...
	$(COPY) $(DONE)\$(TARGETNAME).$(TARGETEXT)  $(WINSYS32DIR)\drivers
	$(COPY) $(DONE)\$(TARGETNAME).pdb  $(WINSYS32DIR)\drivers

install_sysdir_sys: $(WINSYS32DIR)\drivers\$(TARGETNAME).$(TARGETEXT)
	$(SAY) Installed into System32/drivers folder

$(DONE):
	$(SAY) Creating $(DONE) folder
	if not exist $(DONE) mkdir $(DONE)

create_folders: $(DONE)

sync:
	sync

TARGET_INF_OS=XP_X86,Server2003_X86,Vista_X86,Server2008_X86,7_X86
!if "$(TARGET_CPU)" == "amd64"
TARGET_INF_OS=$(TARGET_INF_OS),XP_X64,Server2003_X64,Vista_X64,Server2008_X64,7_X64
!endif

# INF file generation

cat: $(DONE)\$(BLD_CAT_FNAME).cat
	$(SAY) Cat file generated

inf: $(DONE)\$(BLD_INF_FNAME).inf
	$(SAY) Inf file generated

$(DONE)\$(BLD_INF_FNAME).inf: ..\inf\$(INF_SOURCE).cpp create_folders
	$(SAY) Generating .INF file..
	cl -nologo -I..\h -EP ..\inf\$(INF_SOURCE).cpp > $(DONE)\$(BLD_INF_FNAME).inf

$(DONE)\$(BLD_CAT_FNAME).cat: $(DONE)\$(BLD_INF_FNAME).inf create_folders $(DONE)\kx.sys
	$(SAY) Generating .CAT file...
	$(INF2CAT) /driver:$(DONE) /os:$(TARGET_INF_OS)
	$(SAY) Signing catalog...
	$(SIGNTOOL) $(DONE)\$(BLD_CAT_FNAME).cat

# generating NSIS setup

!if "$(TARGET_ARCH)" == "amd64"
distr: help readme inf sdk $(DONE)\$(SETUP_SETUP_FILE)
!else
distr: help readme inf sdk
	$(SAY) x86 build does not create NSIS installer
!endif

$(DONE)\kx-full.nsi: ../nsis/kx-full.cpp
	$(SAY) Precompiling NSIS script
	cl -nologo -I..\h -EP ../nsis/kx-full.cpp -DKXSDK_ROOT="$(KXSDK_ROOT)" -DCE_DONE="$(DONE)" -DKX_HTTPD_ROOT="$(HTTPD_ROOT)" -DKX_SYSTEM_ROOT="$(WINDIR)\system32" > $(DONE)\kx-full.nsi

$(DONE)\$(SETUP_SETUP_FILE): $(DONE)\kx-full.nsi $(DONE)\$(BLD_CAT_FNAME).cat $(DONE)\$(BLD_INF_FNAME).inf $(DONE)\kx.sys create_folders
	$(SAY) Generating NSIS installer
	$(NSIS_ROOT)\makensis.exe /NOCD $(DONE)\kx-full.nsi > $(DONE)\kx-full.log
	$(SAY) Signing NSIS installer
	$(SIGNTOOL) $(DONE)\$(SETUP_SETUP_FILE)

help: $(DONE)\kxhelp.chm
	$(SAY) Help file is not re-compiled, re-compile manually from \www\help folder

$(DONE)\kxhelp.chm: $(HTTPD_ROOT)\help\kxhelp.chm
	$(SAY) Installing help file...
	$(COPY) $(HTTPD_ROOT)\help\kxhelp.chm $(DONE)\kxhelp.chm>nul

readme: $(DONE)\readme.html

$(DONE)\readme.html: ..\texts\readme.cpp
	$(SAY) Creating readme file
	cl /EP -I$(HTTPD_ROOT) -nologo ..\texts\readme.cpp > $(DONE)\readme.html

sdk:
    if not exist $(KXSDK_ROOT) mkdir $(KXSDK_ROOT)
# create $(KXSDK_ROOT)\lib\$(TARGET_CPU)
    if not exist $(KXSDK_ROOT)\lib mkdir $(KXSDK_ROOT)\lib
    if not exist $(KXSDK_ROOT)\lib\$(TARGET_CPU) mkdir $(KXSDK_ROOT)\lib\$(TARGET_CPU)

    $(COPY) $(OBJ_PATH)\..\kxapi\$O\kxapi.lib $(KXSDK_ROOT)\lib\$(TARGET_CPU) 
    $(COPY) $(OBJ_PATH)\..\kxgui\$O\kxgui.lib $(KXSDK_ROOT)\lib\$(TARGET_CPU) 
    $(COPY) $(OBJ_PATH)\..\kxmixer\$O\kxmixer.lib $(KXSDK_ROOT)\lib\$(TARGET_CPU) 

# create \h
    if not exist $(KXSDK_ROOT)\h mkdir $(KXSDK_ROOT)\h
    if not exist $(KXSDK_ROOT)\h\hw mkdir $(KXSDK_ROOT)\h\hw
    if not exist $(KXSDK_ROOT)\h\interface mkdir $(KXSDK_ROOT)\h\interface
    if not exist $(KXSDK_ROOT)\h\dane mkdir $(KXSDK_ROOT)\h\dane

    $(COPY) ..\h\interface\*.* $(KXSDK_ROOT)\h\interface 
    $(COPY) ..\h\dane\*.* $(KXSDK_ROOT)\h\dane 
    $(COPY) ..\h\vers.h $(KXSDK_ROOT)\h 
    $(COPY) ..\h\defs.h $(KXSDK_ROOT)\h 
    $(COPY) ..\h\debug.h $(KXSDK_ROOT)\h 
    $(COPY) ..\h\i386.h $(KXSDK_ROOT)\h 
    $(COPY) ..\h\hw\8010.h $(KXSDK_ROOT)\h\hw\8010.h 
    $(COPY) ..\h\hw\ac97.h $(KXSDK_ROOT)\h\hw 
    $(COPY) ..\h\hw\ecard.h $(KXSDK_ROOT)\h\hw 
    $(COPY) ..\h\hw\emu.h $(KXSDK_ROOT)\h\hw 
    $(COPY) ..\h\hw\p16v.h $(KXSDK_ROOT)\h\hw 
    $(COPY) ..\h\hw\p17v.h $(KXSDK_ROOT)\h\hw 
    $(COPY) ..\h\hw\tina.h $(KXSDK_ROOT)\h\hw 

    if not exist $(KXSDK_ROOT)\h\gui mkdir $(KXSDK_ROOT)\h\gui
    $(COPY) ..\h\gui\*.* $(KXSDK_ROOT)\h\gui > nul
    if not exist $(KXSDK_ROOT)\h\gui\efx mkdir $(KXSDK_ROOT)\h\gui\efx
    $(COPY) ..\h\gui\efx\*.* $(KXSDK_ROOT)\h\gui\efx > nul

    if not exist $(KXSDK_ROOT)\kxfxlib mkdir $(KXSDK_ROOT)\kxfxlib
    $(COPY) ..\kxfxlib\*.* $(KXSDK_ROOT)\kxfxlib 

    if not exist $(KXSDK_ROOT)\kxfx_pack mkdir $(KXSDK_ROOT)\kxfx_pack
    $(COPY) ..\kxfx_pack\*.* $(KXSDK_ROOT)\kxfx_pack 

    if not exist $(KXSDK_ROOT)\kxfx_kxm120 mkdir $(KXSDK_ROOT)\kxfx_kxm120
    if not exist $(KXSDK_ROOT)\kxfx_kxm120\presets mkdir $(KXSDK_ROOT)\kxfx_kxm120\presets
    if not exist $(KXSDK_ROOT)\kxfx_kxm120\settings mkdir $(KXSDK_ROOT)\kxfx_kxm120\settings
    if not exist $(KXSDK_ROOT)\kxfx_kxm120\da mkdir $(KXSDK_ROOT)\kxfx_kxm120\da
    if not exist $(KXSDK_ROOT)\kxfx_kxm120\docs mkdir $(KXSDK_ROOT)\kxfx_kxm120\docs
    $(COPY) ..\kxfx_kxm120\*.* $(KXSDK_ROOT)\kxfx_kxm120 
    $(COPY) ..\kxfx_kxm120\presets\*.* $(KXSDK_ROOT)\kxfx_kxm120\presets 
    $(COPY) ..\kxfx_kxm120\da\*.* $(KXSDK_ROOT)\kxfx_kxm120\da 
    $(COPY) ..\kxfx_kxm120\settings\*.* $(KXSDK_ROOT)\kxfx_kxm120\settings 
    $(COPY) ..\kxfx_kxm120\docs\*.* $(KXSDK_ROOT)\kxfx_kxm120\docs 

    if not exist $(KXSDK_ROOT)\fx_demo mkdir $(KXSDK_ROOT)\fx_demo
    $(COPY) ..\kxfxlib\da_demo.cpp $(KXSDK_ROOT)\fx_demo\*.* > nul
    $(COPY) ..\kxfxlib\demo.* $(KXSDK_ROOT)\fx_demo\*.* > nul
    $(COPY) ..\kxfxlib\debug.cpp $(KXSDK_ROOT)\fx_demo\*.* > nul
    $(COPY) ..\kxfxlib\fxlib.h $(KXSDK_ROOT)\fx_demo\*.* > nul
    $(COPY) ..\kxfxlib\fxlib.rc $(KXSDK_ROOT)\fx_demo\*.* > nul
    $(COPY) ..\kxfxlib\simple.cpp $(KXSDK_ROOT)\fx_demo\*.* > nul
    $(COPY) ..\kxfxlib\stdafx.* $(KXSDK_ROOT)\fx_demo\*.* > nul
    $(COPY) ..\kxfxlib\*.inc $(KXSDK_ROOT)\fx_demo\*.* > nul
    $(COPY) ..\kxfxlib\demo.da $(KXSDK_ROOT)\fx_demo\*.* > nul
    $(COPY) ..\kxfxlib\makefile.demo $(KXSDK_ROOT)\fx_demo\makefile > nul

    if not exist $(KXSDK_ROOT)\kxfx_dynamica mkdir $(KXSDK_ROOT)\kxfx_dynamica
    $(COPY) ..\kxfx_dynamica\*.* $(KXSDK_ROOT)\kxfx_dynamica 

    if not exist $(KXSDK_ROOT)\kxfx_efx_tube mkdir $(KXSDK_ROOT)\kxfx_efx_tube
    $(COPY) ..\kxfx_efx_tube\*.* $(KXSDK_ROOT)\kxfx_efx_tube 

    if not exist $(KXSDK_ROOT)\kxfx_efx_reverb mkdir $(KXSDK_ROOT)\kxfx_efx_reverb
    $(COPY) ..\kxfx_efx_reverb\*.* $(KXSDK_ROOT)\kxfx_efx_reverb

    if not exist $(KXSDK_ROOT)\kxfx_efx_library mkdir $(KXSDK_ROOT)\kxfx_efx_library
    $(COPY) ..\kxfx_efx_library\*.* $(KXSDK_ROOT)\kxfx_efx_library

    if not exist $(KXSDK_ROOT)\kxaddons mkdir $(KXSDK_ROOT)\kxaddons
    $(COPY) ..\kxaddons\*.* $(KXSDK_ROOT)\kxaddons 

    if not exist $(KXSDK_ROOT)\sample_addon mkdir $(KXSDK_ROOT)\sample_addon
    $(COPY) ..\sample_addon\*.* $(KXSDK_ROOT)\sample_addon 

    if not exist $(KXSDK_ROOT)\kxapi mkdir $(KXSDK_ROOT)\kxapi
    $(COPY) ..\kxapi\*.* $(KXSDK_ROOT)\kxapi > nul

    if not exist $(KXSDK_ROOT)\kxmixer mkdir $(KXSDK_ROOT)\kxmixer
    $(COPY) ..\kxmixer\addon.cpp    $(KXSDK_ROOT)\kxmixer > nul
    $(COPY) ..\kxmixer\addonmgr.cpp    $(KXSDK_ROOT)\kxmixer > nul
    $(COPY) ..\kxmixer\addonmgr.h    $(KXSDK_ROOT)\kxmixer > nul
    $(COPY) ..\kxmixer\manager.cpp    $(KXSDK_ROOT)\kxmixer > nul
    $(COPY) ..\kxmixer\plugin.cpp    $(KXSDK_ROOT)\kxmixer > nul

    if not exist $(KXSDK_ROOT)\fx_skin mkdir $(KXSDK_ROOT)\fx_skin
    $(COPY) ..\kxskin\plugin\*.* $(KXSDK_ROOT)\fx_skin 

    if not exist $(KXSDK_ROOT)\da mkdir $(KXSDK_ROOT)\da
    $(COPY) ..\da\*.* $(KXSDK_ROOT)\da

    if not exist $(KXSDK_ROOT)\kxctrl mkdir $(KXSDK_ROOT)\kxctrl
    $(COPY) ..\kxctrl\*.* $(KXSDK_ROOT)\kxctrl 

    if not exist $(KXSDK_ROOT)\kxsfman mkdir $(KXSDK_ROOT)\kxsfman
    $(COPY) ..\kxsfman\*.* $(KXSDK_ROOT)\kxsfman 

    if not exist $(KXSDK_ROOT)\edspctrl mkdir $(KXSDK_ROOT)\edspctrl
    $(COPY) ..\edspctrl\*.* $(KXSDK_ROOT)\edspctrl 

# Mac OS X sources

    if not exist $(KXSDK_ROOT)\macosx mkdir $(KXSDK_ROOT)\macosx
    $(COPY) ..\macosx\*. $(KXSDK_ROOT)\macosx > nul
    xcopy /E /R /H /Y /I /G ..\macosx\package $(KXSDK_ROOT)\macosx\package > nul
    xcopy /E /R /H /Y /I /G ..\macosx\kxmanager $(KXSDK_ROOT)\macosx\kxmanager > nul
    xcopy /E /R /H /Y /I /G ..\macosx\kxac97 $(KXSDK_ROOT)\macosx\kxac97 > nul
    xcopy /E /R /H /Y /I /G ..\macosx\kext $(KXSDK_ROOT)\macosx\kext > nul
    xcopy /E /R /H /Y /I /G ..\macosx\"10kx driver.xcodeproj" $(KXSDK_ROOT)\macosx\"10kx driver.xcodeproj" > nul
    xcopy /E /R /H /Y /I /G ..\macosx\pcmblitterlib $(KXSDK_ROOT)\macosx\pcmblitterlib > nul

# licenses
    $(COPY) $(HTTPD_ROOT)\help\license3.txt $(KXSDK_ROOT)\copying 
    $(COPY) ..\texts\kxapi.txt $(KXSDK_ROOT)\readme.txt > nul

